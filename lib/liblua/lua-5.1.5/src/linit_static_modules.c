/*
** $Id: linit.c,v 1.14.1.1 2007/12/27 13:02:25 roberto Exp $
** Initialization of libraries for lua.c
** See Copyright Notice in lua.h
*/


#define linit_c
#define LUA_LIB

#include "lua.h"

#include "lualib.h"
#include "lauxlib.h"

#ifdef AUTOCONF
#  include "autoconf.h"
#endif

#ifdef LUA_STATIC_MODULES

#ifdef CONFIG_LIB_LUA_LUAFILESYSTEM
int luaopen_lfs (lua_State *L);
#endif

#ifdef CONFIG_LIB_LUA_LUASOCKET
int luaopen_socket_core(lua_State *L);
int luaopen_mime_core(lua_State *L);
#endif

#ifdef CONFIG_LIB_LUA_SQLITE
int luaopen_lsqlite3(lua_State *L);
#endif

#ifdef CONFIG_LIB_LUA_UCI
int luaopen_uci(lua_State *L);
#endif

#ifdef CONFIG_LIB_LUA_BITSTRING
int luaopen_bitstring(lua_State *L);
#endif

#ifdef CONFIG_LIB_LUA_LSYSLOG
int luaopen_syslog(lua_State *L);
#endif

#ifdef CONFIG_LIB_LUA_LSIGNALS
int luaopen_signals(lua_State *L);
#endif

#ifdef CONFIG_LIB_LUA_LWATCHDOG
int luaopen_watchdog(lua_State *L);
#endif

#ifdef CONFIG_LIB_LUA_LAIO
int luaopen_aio(lua_State *L);
#endif

#ifdef CONFIG_LIB_LUA_LNTP
int luaopen_ntp(lua_State *L);
#endif

#ifdef CONFIG_LIB_LUA_LUASEC
int luaopen_ssl_context(lua_State *L);
int luaopen_ssl_core(lua_State *L);
#endif

#ifdef CONFIG_LIB_LUA_LCRYPTO
int luaopen_crypto(lua_State *L);
#endif

static const luaL_Reg modules[] = {
#ifdef CONFIG_LIB_LUA_LUAFILESYSTEM
  {"lfs", luaopen_lfs},
#endif
#ifdef CONFIG_LIB_LUA_LUASOCKET
	{"socket.core", luaopen_socket_core},
	{"mime.core", luaopen_mime_core},
#endif
#ifdef CONFIG_LIB_LUA_SQLITE
	{"lsqlite3", luaopen_lsqlite3},
#endif
#ifdef CONFIG_LIB_LUA_UCI
	{"uci", luaopen_uci},
#endif
#ifdef CONFIG_LIB_LUA_BITSTRING
	{"bitstring", luaopen_bitstring},
#endif
#ifdef CONFIG_LIB_LUA_LSYSLOG
	{"syslog", luaopen_syslog},
#endif
#ifdef CONFIG_LIB_LUA_LSIGNALS
	{"signals", luaopen_signals},
#endif
#ifdef CONFIG_LIB_LUA_LWATCHDOG
	{"watchdog", luaopen_watchdog},
#endif
#ifdef CONFIG_LIB_LUA_LAIO
	{"aio", luaopen_aio},
#endif
#ifdef CONFIG_LIB_LUA_LNTP
	{"ntp", luaopen_ntp},
#endif
#ifdef CONFIG_LIB_LUA_LUASEC
	{"ssl.context", luaopen_ssl_context},
	{"ssl.core", luaopen_ssl_core},
#endif
#ifdef CONFIG_LIB_LUA_LCRYPTO
	{"crypto", luaopen_crypto},
#endif
	{NULL, NULL}
};

LUALIB_API void luaL_preload_static_modules (lua_State *L) {
  const luaL_Reg *module = modules;

  lua_getfield(L, LUA_GLOBALSINDEX, "package");
  if (!lua_istable(L, -1))
    luaL_error(L, LUA_QL("package") " must be a table");

  lua_getfield(L, -1, "preload");
  if (!lua_istable(L, -1))
    luaL_error(L, LUA_QL("package.preload") " must be a table");

  for (; module->func; module++) {
    lua_pushstring(L, module->name);
    lua_pushcfunction(L, module->func);
    lua_settable(L, -3);
  }
}

#endif
