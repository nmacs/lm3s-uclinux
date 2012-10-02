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
#endif

static const lua_CFunction modules[] = {
#ifdef CONFIG_LIB_LUA_LUAFILESYSTEM
  luaopen_lfs,
#endif
#ifdef CONFIG_LIB_LUA_LUASOCKET
  luaopen_socket_core,
#endif
  NULL
};


LUALIB_API void luaL_load_static_modules (lua_State *L) {
  const lua_CFunction *func = modules;
  for (; *func; func++) {
    lua_pushcfunction(L, *func);
    lua_call(L, 0, 0);
  }
}

#endif
