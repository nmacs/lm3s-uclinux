#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/times.h>

#define DEBUG 0

#if DEBUG
#  include <syslog.h>
#  define dprint(s, ...) { syslog(LOG_DEBUG, s, ##__VA_ARGS__); usleep(100000); }
//#  define dprint(s, ...) { fprintf(stderr, s, ##__VA_ARGS__); usleep(100000); }
#else
#  define dprint(...) while(0) {}
#endif

#define lscheduler_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
#include "lcoco.h"

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
	struct epoll_event *event;
	struct timeout_ctx *timeout;
	lua_State *L_thread;
	int fd;
	int suspended;
};

static int epoll_fd;
static struct timeout_ctx timeout_head;
static size_t MS_PER_TICK;
static int exit_scheduler;

#if DEBUG
static int dump_timeouts()
{
	struct timeout_ctx *item;
	dprint("timeouts: head:%p\n", &timeout_head);
	for (item = timeout_head.next; item != &timeout_head; item = item->next) {
		dprint("timer:%p, expire:%u, next:%p, data:%p\n", item, item->expire, item->next, item->data);
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

static void insert_timeout(struct timeout_ctx* ctx)
{
	struct timeout_ctx *insert_before;
	
	dprint("insert_timeout: ctx:%p\n", ctx);
	
	for (insert_before = timeout_head.next; insert_before != &timeout_head; insert_before = insert_before->next) {
		if (time_before(ctx->expire, insert_before->expire))
			break;
	}

	ctx->next = insert_before;
	ctx->prev = insert_before->prev;
	insert_before->prev->next = ctx;
	insert_before->prev = ctx;
}

static void set_timeout(struct timeout_ctx* ctx, int timeout, void *data)
{
	ctx->expire = times(0) + (clock_t)timeout / MS_PER_TICK;
	ctx->data = data;
	insert_timeout(ctx);
}

static void remove_timeout(struct timeout_ctx* ctx)
{
	dprint("remove_timeout ctx:%p\n", ctx);
	
	if (ctx == NULL || ctx->next == ctx)
		return;

	ctx->next->prev = ctx->prev;
	ctx->prev->next = ctx->next;
	ctx->prev = ctx;
	ctx->next = ctx;
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

	dprint("luaL_wait: co:%p, ctx:%p, fd:%i, write:%i, timeout:%i\n", L, &ctx, fd, write, timeout);

	if (timeout >= 0) {
		set_timeout(&tctx, timeout, &ctx);
		ctx.timeout = &tctx;
	}
	else
		ctx.timeout = 0;
	ctx.L_thread = L;
	ctx.fd = fd;
	ctx.suspended = 0;

	if (fd >= 0) {
		int ret;
		struct epoll_event event;
		event.events = write ? EPOLLOUT : EPOLLIN;
		event.data.ptr = &ctx;
		ctx.event = &event;
		ret = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event);
		if (ret) {
			if (errno == ENOENT) {
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event)) {
					remove_timeout(ctx.timeout);
					return -errno;
				}
				delfd = 1;
			}
			else {
				remove_timeout(ctx.timeout);
				return -errno;
			}
		}
	}
	else
		ctx.event = NULL;
	
	lua_pushlightuserdata(L, (void*)&ctx);
	lua_pushthread(L);
	lua_settable(L, LUA_REGISTRYINDEX);
	luaCOCO_settls(L, &ctx);

	dprint("yield: co:%p\n", L);
	lua_yield(L, 0);
	dprint("resumed: co:%p\n", L);
	status = luaL_checkinteger(L, -1);
	lua_pop(L, 1);
	
	luaCOCO_settls(L, NULL);
	lua_pushlightuserdata(L, (void*)&ctx);
	lua_pushnil(L);
	lua_settable(L, LUA_REGISTRYINDEX);

	if (delfd) {
		struct epoll_event event = {0};
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &event);
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
	dprint("resume_thread L:%p, co:%p, ctx:%p, status:%i, suspended:%i\n", 
	       L, co, ctx, status, ctx->suspended);
	if (ctx->suspended)
		return 0;
	remove_timeout(ctx->timeout);
	lua_pushinteger(L, status);
	return do_resume_thread(L, co, 1);
}

static int l_loop(lua_State *L)
{
	struct epoll_event events[MAXEVENTS];
	struct timeout_ctx *tctx;
	struct timeout_ctx *tnext;
	clock_t now;
	int timeout;
	int ret;

	while (1) {
		if (exit_scheduler) {
			lua_pushfstring(L, "scheduler terminated");
			return 1;
		}
		
		timeout = get_timeout();
		dprint("loop: wait for events timeout:%i\n", timeout);
		ret = epoll_wait(epoll_fd, events, MAXEVENTS, timeout);
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

static int suspend_thread(lua_State *L, lua_State *co)
{
	struct wait_ctx *ctx;
	struct epoll_event event = {0};
	
	if (luaCOCO_gettls(co, (unsigned long*)&ctx))
		luaL_argcheck(L, 0, 1, "alive c-coroutine expected");
	
	dprint("suspend_thread: co:%p, ctx:%p\n", co, ctx);
	
	if (ctx == NULL || ctx->suspended)
		return 0;
	
	ctx->suspended = 1;
	if (ctx->timeout) {
		dprint("suspend_thread: remove timeout:%p\n", ctx->timeout);
		remove_timeout(ctx->timeout);
	}
	
	if (ctx->fd >= 0)
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, ctx->fd, &event);
	
	return 1;
}

static int l_suspend_thread(lua_State *L)
{
	int ret;
	lua_State *co = lua_tothread(L, 1);
	luaL_argcheck(L, co, 1, "coroutine expected");
	ret = suspend_thread(L, co);
	lua_pushboolean(L, ret ? 1 : 0);
	return 1;
}

static int l_resume_thread(lua_State *L)
{
	struct wait_ctx *ctx;
	lua_State *co = lua_tothread(L, 1);
	luaL_argcheck(L, co, 1, "coroutine expected");
	if (luaCOCO_gettls(co, (unsigned long*)&ctx))
		luaL_argcheck(L, 0, 1, "alive c-coroutine expected");

	if (ctx == NULL || !ctx->suspended) {
		lua_pushboolean(L, 0);
		return 1;
	}

	ctx->suspended = 0;

	if (ctx->timeout)
		insert_timeout(ctx->timeout);

	if (ctx->event) {
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, ctx->fd, ctx->event)) {
			luaL_error(L, "epoll_clt error:%d", errno);
		}
	}
	
	lua_pushboolean(L, 1);
	return 1;
}

static int l_cancel_wait(lua_State *L)
{
	struct wait_ctx *ctx;
	lua_State *co = lua_tothread(L, 1);
	luaL_argcheck(L, co, 1, "coroutine expected");
	if (luaCOCO_gettls(co, (unsigned long*)&ctx))
		luaL_argcheck(L, 0, 1, "alive c-coroutine expected");

	if (ctx == NULL) {
		lua_pushboolean(L, 0);
		return 1;
	}

	suspend_thread(L, co);
	lua_pushinteger(L, 1);
	do_resume_thread(L, co, 1);

	lua_pushboolean(L, 1);
	return 1;
}

static int l_exit(lua_State *L)
{
	exit_scheduler = 1;
	return 0;
}

static const luaL_Reg lib[] = {
	{"wait",   l_wait},
	{"cancel_wait", l_cancel_wait},
	{"msleep", l_msleep},
	{"loop",   l_loop},
	{"run",    l_run},
	{"addfd",  l_addfd},
	{"delfd",  l_delfd},
	{"suspend_thread", l_suspend_thread},
	{"resume_thread",  l_resume_thread},
	{"exit", l_exit},
  {NULL, NULL}
};

/* }====================================================== */

LUALIB_API int luaopen_scheduler (lua_State *L)
{
	lua_setthis(L);
	timeout_head.next = &timeout_head;
	timeout_head.prev = &timeout_head;
	exit_scheduler = 0;
	MS_PER_TICK = 1000UL / sysconf(_SC_CLK_TCK);
	if (init_epoll())
		return 0;
	luaL_register(L, LUA_SCHEDULERLIBNAME, lib);
	return 1;
}
