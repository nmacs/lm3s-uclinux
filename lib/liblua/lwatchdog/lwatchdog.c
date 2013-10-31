#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/watchdog.h>
#include "lua.h"
#include "lauxlib.h"

#define WD_DEV_PATH "/dev/watchdog"

static int fd = -1;

static int wd_set_counter(int count)
{
	return ioctl(fd, WDIOC_SETTIMEOUT, &count);
}

static int wd_get_counter()
{
	int count;
	int err;
	if ((err = ioctl(fd, WDIOC_GETTIMEOUT, &count))) {
		count = err;
	}
	return count;
}

static int wd_keep_alive(void)
{
	return ioctl(fd, WDIOC_KEEPALIVE);
}

static int wd_open(void)
{
	int ret = open(WD_DEV_PATH, O_WRONLY);
	if (ret < 0)
		return ret;
	fd = ret;
	return 0;
}

static int l_set_timeout(lua_State *L)
{
	int timeout = luaL_checkinteger(L, 1);
	int actual_timeout;
	int ret;
	
	if (fd < 0) {
		lua_pushnil(L);
		lua_pushstring(L, "watchdog: set_timeout: no watchdog available");
		return 2;
	}

	int old_timeout = wd_get_counter();
	if (old_timeout < 0) {
		lua_pushnil(L);
		lua_pushfstring(L, "watchdog: set_timeout: get old timeout error:%i", old_timeout);
		return 2;
	}
	if ((ret = wd_set_counter(timeout))) {
		lua_pushnil(L);
		lua_pushfstring(L, "watchdog: set_timeout: set new tiemout error:%i", ret);
		return 2;
	}
	actual_timeout = wd_get_counter();
	if (actual_timeout < 0) {
		lua_pushnil(L);
		lua_pushfstring(L, "watchdog: set_timeout: get actual timeout error:%i", actual_timeout);
		return 2;
	}
	if (actual_timeout != timeout) {
		lua_pushnil(L);
		lua_pushstring(L, "watchdog: set_timeout: actual timeout differs from required");
		return 2;
	}
	
	lua_pushinteger(L, old_timeout);
	return 1;
}

static int l_get_timeout(lua_State *L)
{
	int timeout;
	if (fd < 0) {
		lua_pushnil(L);
		lua_pushstring(L, "watchdog: get_timeout: no watchdog available");
		return 2;
	}
	timeout = wd_get_counter();
	if (timeout < 0) {
		lua_pushnil(L);
		lua_pushfstring(L, "watchdog: get_timeout: error:%i", timeout);
		return 2;
	}
	lua_pushinteger(L, timeout);
	return 1;
}

static int l_keep_alive(lua_State *L)
{
	int ret;
	if (fd < 0) {
		lua_pushnil(L);
		lua_pushstring(L, "watchdog: keep_alive: no watchdog available");
		return 2;
	}
	if ((ret = wd_keep_alive())) {
		lua_pushnil(L);
		lua_pushfstring(L, "watchdog: keep_alive: error:%i", ret);
		return 2;
	}
	lua_pushboolean(L, 1);
	return 1;
}

static const struct luaL_Reg mylib [] = {
	{ "set_timeout", l_set_timeout },
	{ "get_timeout", l_get_timeout },
	{ "keep_alive", l_keep_alive },
	{ NULL, NULL }
};

int luaopen_watchdog(lua_State *L)
{
	wd_open();
	luaL_register(L, "watchdog", mylib);
	return 1;
}
