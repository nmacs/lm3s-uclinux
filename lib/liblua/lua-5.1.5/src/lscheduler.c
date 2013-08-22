#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/times.h>

#define DEBUG 0

#if DEBUG
#  include <syslog.h>
#  define dprint(s, ...) { syslog(LOG_DEBUG, s, ##__VA_ARGS__); usleep(100000); }
#else
#  define dprint(...) while(0) {}
#endif

#define lscheduler_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

#define time_after(a,b)  ((long)(b) - (long)(a) < 0)
#define time_before(a,b) time_after(b, a)

#define MAXEVENTS 128

struct timeout_ctx {
	void *data;
	clock_t expire;
	struct timeout_ctx *next;
	struct timeout_ctx *prev;
};

struct wait_ctx {
	struct timeout_ctx *timeout;
	lua_State *L_thread;
	int fd;
};

static int epoll_fd;
static struct timeout_ctx timeout_head;
static size_t MS_PER_TICK;

#if DEBUG
static int dump_timeouts()
{
	struct timeout_ctx *item;
	dprint("timeouts:\n");
	for (item = timeout_head.next; item != &timeout_head; item = item->next) {
		dprint("timer:%p, expire:%u\n", item, item->expire);
	}
	dprint("done\n");
}
#else
#define dump_timeouts() while(0) {}
#endif

static int init_epoll()
{
	int ret = epoll_create1(0);
	if (ret < 0)
		return ret;
	epoll_fd = ret;
	return 0;
}

static void set_timeout(struct timeout_ctx* ctx, int timeout, void *data)
{
	struct timeout_ctx *insert_before;
	clock_t expire = times(0) + (clock_t)timeout / MS_PER_TICK;

	dprint("set_timeout\n");

	ctx->expire = expire;
	ctx->data = data;

	for (insert_before = timeout_head.next; insert_before != &timeout_head; insert_before = insert_before->next) {
		if (time_before(expire, insert_before->expire))
			break;
	}

	ctx->next = insert_before;
	ctx->prev = insert_before->prev;
	insert_before->prev->next = ctx;
	insert_before->prev = ctx;
	
	dump_timeouts();
}

static void clear_timeout(struct timeout_ctx* ctx)
{
	dprint("clear_timeout ctx:%p\n", ctx);
	
	if (ctx == NULL || ctx->next == ctx)
		return;

	ctx->next->prev = ctx->prev;
	ctx->prev->next = ctx->next;
	ctx->prev = ctx;
	ctx->next = ctx;
	
	dump_timeouts();
}

static int get_timeout(void)
{
	clock_t now = times(0);
	struct timeout_ctx *first = timeout_head.next;
	if (first != &timeout_head) {
		long diff = (long)first->expire - (long)now;
		if (diff <= 0)
			return 0;
		return diff * MS_PER_TICK;
	}
	else
		return -1;
}

LUALIB_API int luaL_wait(lua_State *L, int fd, int write, int timeout)
{
	int status;
	int delfd = 0;
	struct wait_ctx ctx;
	struct timeout_ctx tctx;
	
	dprint("luaL_wait (1): L:%p, ctx:%p, fd:%i, write:%i, timeout:%i\n", L, &ctx, fd, write, timeout);

	if (timeout >= 0) {
		set_timeout(&tctx, timeout, &ctx);
		ctx.timeout = &tctx;
	}
	else
		ctx.timeout = 0;
	ctx.L_thread = L;
	ctx.fd = fd;
	
	dprint("luaL_wait (2)\n");

	if (fd >= 0) {
		int ret;
		struct epoll_event event;
		event.events = write ? EPOLLOUT : EPOLLIN;
		event.data.ptr = &ctx;
		ret = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event);
		if (ret) {
			if (errno == ENOENT) {
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event)) {
					clear_timeout(ctx.timeout);
					return -errno;
				}
				delfd = 1;
			}
			else {
				clear_timeout(ctx.timeout);
				return -errno;
			}
		}
	}

	dprint("yield: co:%p\n", L);
	lua_yield(L, 0);
	dprint("resumed: co:%p\n", L);
	status = luaL_checkinteger(L, -1);
	lua_pop(L, 1);

	if (delfd) {
		struct epoll_event event = {0};
		epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
	}

	return status;
}

static int auxwait(lua_State *L, int fd, int write, int timeout)
{
	int ret = luaL_wait(L, fd, write, timeout);
	if (ret < 0) {
		lua_pushnil(L);
		lua_pushstring(L, "error");
		lua_pushfstring(L, "wait error:%d", -ret);
		return 3;
	}
	else if (ret == 0) {
		lua_pushnil(L);
		lua_pushstring(L, "timeout");
		return 2;
	}

	lua_pushboolean(L, 1);
	lua_pushfstring(L, "status:%d", ret);

	return 2;
}

LUALIB_API int luaL_addfd(lua_State *L, int fd)
{
	struct epoll_event event = {0};
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event)) {
		return -errno;
	}
	return 0;
}

LUALIB_API int luaL_delfd(lua_State *L, int fd)
{
	struct epoll_event event = {0};
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &event)) {
		return -errno;
	}
	return 0;
}

static int do_resume_thread(lua_State *L, lua_State *co, int nargs)
{
	int ret;
	dprint("do_resume_thread L:%p, co:%p, nargs:%i\n", L, co, nargs);
	lua_xmove(L, co, nargs);
	lua_setthis(co);
	ret = lua_resume(co, nargs);
	lua_setthis(L);
	if (ret == LUA_YIELD || ret == 0) {
		lua_pop(co, lua_gettop(co));
		return 0;
	}
	else {
		lua_xmove(co, L, 1);
		return -1;
	}
}

static int resume_thread(lua_State *L, struct wait_ctx *ctx, int status)
{
	lua_State *co = ctx->L_thread;
	dprint("resume_thread L:%p, co:%p, ctx:%p, status:%i\n", L, co, ctx, status);
	clear_timeout(ctx->timeout);
	lua_pushinteger(L, status);
	return do_resume_thread(L, co, 1);
}

static int l_loop(lua_State *L)
{
	struct epoll_event events[MAXEVENTS];
	struct timeout_ctx *tctx;
	struct timeout_ctx *tnext;
	clock_t now;

	while (1) {
		int timeout = get_timeout();
		dprint("loop: wait for events timeout:%i\n", timeout);
		int ret = epoll_wait(epoll_fd, events, MAXEVENTS, timeout);
		dprint("loop: wait ret:%i\n", ret);
		if (ret > 0) {
			int i;
			for (i = 0; i < ret; i++) {
				struct wait_ctx *ctx = events[i].data.ptr;
				int status = events[i].events & (EPOLLHUP | EPOLLERR) ? -1 : 1;
				if (resume_thread(L, ctx, status))
					lua_pop(L, 1);
			}
		}
		else if (ret < 0 && errno != EINTR) {
			lua_pushfstring(L, "loop error:%d\n", errno);
			return 1;
		}

		now = times(0);
		dprint("loop: now:%u\n", now);
		for (tctx = timeout_head.next, tnext = tctx->next; tctx != &timeout_head; tctx = tnext, tnext = tctx->next) {
			dprint("loop: expire:%u, diff:%i\n", tctx->expire, (long)tctx->expire - (long)now);
			if (time_before(tctx->expire, now)) {
				dprint("loop: resume thread by timeout\n");
				if (resume_thread(L, (struct wait_ctx*)tctx->data, 0))
					lua_pop(L, 1);
			}
			else
				break;
		}
	}

	return 0;
}

static int l_wait(lua_State *L)
{
	int fd = luaL_optinteger(L, 1, -1);
	static const char* operations[] = {"read", "write", NULL};
	int op = luaL_checkoption(L, 2, "read", operations);
	int timeout = luaL_optinteger(L, 3, -1);
	return auxwait(L, fd, op, timeout);
}

static int l_msleep(lua_State *L)
{
	int timeout = luaL_optinteger(L, 1, -1);
	return auxwait(L, -1, 0, timeout);
}

static int l_run(lua_State *L)
{
	lua_State *co = lua_tothread(L, 1);
	luaL_argcheck(L, co, 1, "coroutine expected");
	if (do_resume_thread(L, co, lua_gettop(L) - 1)) {
		lua_pushnil(L);
		lua_pushvalue(L, -2);
		return 2;
	}
	else {
		lua_pushboolean(L, 1);
		return 1;
	}
}

static int l_addfd(lua_State *L)
{
	int fd = luaL_checkinteger(L, 1);
	int ret = luaL_addfd(L, fd);
	if (ret < 0) {
		lua_pushnil(L);
		lua_pushfstring(L, "addfd error:%d", -ret);
		return 2;
	}
	
	lua_pushboolean(L, 1);
	return 1;
}

static int l_delfd(lua_State *L)
{
	int fd = luaL_checkinteger(L, 1);
	int ret = luaL_delfd(L, fd);
	if (ret < 0) {
		lua_pushnil(L);
		lua_pushfstring(L, "delfd error:%d", -ret);
		return 2;
	}
	
	lua_pushboolean(L, 1);
	return 1;
}

static int l_suspend_thread(lua_State *L)
{
	lua_State *co = lua_tothread(L, 1);
	luaL_argcheck(L, co, 1, "coroutine expected");
	
	
	
	return 0;
}

static const luaL_Reg lib[] = {
	{"wait", l_wait},
	{"msleep", l_msleep},
	{"loop", l_loop},
	{"run", l_run},
	{"addfd", l_addfd},
	{"delfd", l_delfd},
  {NULL, NULL}
};

/* }====================================================== */

LUALIB_API int luaopen_scheduler (lua_State *L)
{
	timeout_head.next = &timeout_head;
	timeout_head.prev = &timeout_head;
	MS_PER_TICK = 1000UL / sysconf(_SC_CLK_TCK);
	if (init_epoll())
		return 0;
	luaL_register(L, LUA_SCHEDULERLIBNAME, lib);
	return 1;
}
