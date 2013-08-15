#define _GNU_SOURCE		/* syscall() is not POSIX */

#include <stdio.h>		/* for perror() */
#include <unistd.h>		/* for syscall() */
#include <sys/syscall.h>	/* for __NR_* definitions */
#include <sys/eventfd.h>
#include <linux/aio_abi.h>	/* for AIO types and constants */
#include <time.h>
#include <string.h>
#include <errno.h>
#include "lua.h"
#include "lauxlib.h"

#define AIO_CTX_METATABLE "aio.ctx.meta"

struct aio_ctx {
	aio_context_t ctx;
	int evfd;
};

inline static int io_setup(unsigned nr, aio_context_t *ctxp)
{
	return syscall(__NR_io_setup, nr, ctxp);
}

inline static int io_destroy(aio_context_t ctx) 
{
	return syscall(__NR_io_destroy, ctx);
}

inline static int io_submit(aio_context_t ctx, long nr,  struct iocb **iocbpp) 
{
	return syscall(__NR_io_submit, ctx, nr, iocbpp);
}

inline int io_getevents(aio_context_t ctx, long min_nr, long max_nr,
		struct io_event *events, struct timespec *timeout)
{
	return syscall(__NR_io_getevents, ctx, min_nr, max_nr, events, timeout);
}

static int l_io_setup(lua_State *L)
{
	struct aio_ctx *lctx = (struct aio_ctx*)lua_newuserdata(L, sizeof(struct aio_ctx));
	lctx->ctx = 0;
	if (io_setup(1, &lctx->ctx) < 0) {
		lua_pushnil(L);
		lua_pushfstring(L, "io_setup error:%d", errno);
		return 2;
	}
	
	lctx->evfd = eventfd(0, 0);
	if (lctx->evfd < 0) {
		io_destroy(lctx->ctx);
		lua_pushnil(L);
		lua_pushfstring(L, "eventfd error:%d", errno);
		return 2;
	}
	
	luaL_getmetatable(L, AIO_CTX_METATABLE);
	lua_setmetatable(L, -2);
	return 1;
}

static int l_io_destroy(lua_State *L)
{
	struct aio_ctx *lctx = (struct aio_ctx*)luaL_checkudata(L, 1, AIO_CTX_METATABLE);
	io_destroy(lctx->ctx);
	close(lctx->evfd);
	return 0;
}

static int l_io_writesub(lua_State *L)
{
	int ret;
	struct iocb cb;
	struct iocb *cbs[1];
	struct aio_ctx *lctx = (struct aio_ctx*)luaL_checkudata(L, 1, AIO_CTX_METATABLE);
	size_t l;
	int fd = luaL_checkinteger(L, 2);
	int foffset = luaL_checkinteger(L, 3);
	const char *s = luaL_checklstring(L, 4, &l);
	int offset = luaL_checkinteger(L, 5);
	int size = luaL_checkinteger(L, 6);
	if (l < offset + size) {
		lua_pushnil(L);
		lua_pushstring(L, "invalid argument");
		return 2;
	}
	
	memset(&cb, 0, sizeof(cb));
	cb.aio_fildes = fd;
	cb.aio_lio_opcode = IOCB_CMD_PWRITE;
	cb.aio_buf = (uintptr_t)(s + offset);
	cb.aio_nbytes = size;
	cb.aio_offset = foffset;
	cb.aio_flags = IOCB_FLAG_RESFD;
	cb.aio_resfd = lctx->evfd;
	cbs[0] = &cb;
	
	ret = io_submit(lctx->ctx, 1, cbs);
	if (ret != 1) {
		lua_pushnil(L);
		if (ret < 0)
			lua_pushfstring(L, "io_setup error:%d", errno);
		else
			lua_pushstring(L, "could not submit IOs");
		return  2;
	}
	
	lua_pushinteger(L, 0);
	return 1;
}

static int l_io_complete(lua_State *L)
{
	int ret;
	struct io_event events[1];
	struct aio_ctx *lctx = (struct aio_ctx*)luaL_checkudata(L, 1, AIO_CTX_METATABLE);
	uint64_t counter;
	
	ret = io_getevents(lctx->ctx, 1, 1, events, NULL);
	if (ret != 1) {
		lua_pushnil(L);
		lua_pushfstring(L, "io_getevents error:%d", errno);
		return 2;
	}
	
	read(lctx->evfd, &counter, sizeof(counter));
	
	lua_pushinteger(L, (int)events[0].res);
	return 1;
}

static int l_io_get_event_fd(lua_State *L)
{
	struct aio_ctx *lctx = (struct aio_ctx*)luaL_checkudata(L, 1, AIO_CTX_METATABLE);
	lua_pushinteger(L, lctx->evfd);
	return 1;
}

static const struct luaL_Reg mylib [] = {
	{ "setup", l_io_setup },
	{ NULL, NULL }
};

static int ctx_create_meta(lua_State *L)
{
	luaL_newmetatable (L, AIO_CTX_METATABLE);
	lua_createtable(L, 0, 3);
		lua_pushcfunction(L, l_io_writesub); lua_setfield(L, -2, "writesub");
		lua_pushcfunction(L, l_io_complete); lua_setfield(L, -2, "complete");
		lua_pushcfunction(L, l_io_get_event_fd); lua_setfield(L, -2, "get_event_fd");
	lua_setfield(L, -2, "__index");
		
	lua_pushcfunction(L, l_io_destroy); lua_setfield(L, -2, "__gc");
	return 1;
}

int luaopen_aio(lua_State *L)
{
	ctx_create_meta(L);
	luaL_register(L, "aio", mylib);
	return 1;
}
