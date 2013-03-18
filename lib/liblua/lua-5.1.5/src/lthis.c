#define lthis_c
#define LUA_LIB

#include "lua.h"

static lua_State *L_global;

LUA_API lua_State* lua_this(void)
{
  return L_global;
}

LUA_API void lua_setthis(lua_State* L)
{
  L_global = L;
}
