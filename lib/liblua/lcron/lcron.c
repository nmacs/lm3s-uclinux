#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/times.h>
#include <syslog.h>
#include <string.h>

//#define DEBUG 1

#if DEBUG
#  include <syslog.h>
#  define dprint(s, ...) { printf(s, ##__VA_ARGS__); /*usleep(10000);*/ }
//#  define dprint(s, ...) { fprintf(stderr, s, ##__VA_ARGS__); usleep(100000); }
#else
#  define dprint(...) while(0) {}
#endif

#define lcron_c
#define LUA_LIB

#define CRON_TIMER_METATABLE "cron_timer_metatable"
#define CRON_TIMERS_TABLE "cron_timers_table"

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
#include "lcoco.h"
#include "ltimer.h"

#define LUA_CRONLIBNAME "cron.core"

static struct timer_context timers;
static size_t MS_PER_TICK;

static int l_new(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TFUNCTION);

    lua_newtable(L);

    lua_pushstring(L, "callback");
    lua_pushvalue(L, 1);
    lua_settable(L, -3);

    if (!lua_isstring(L, 2)) {
        lua_pushstring(L, "name");
        lua_pushvalue(L, 2);
        lua_settable(L, -3);
    }

    lua_pushstring(L, "timer");
    struct timer_list *timer = lua_newuserdata(L, sizeof(struct timer_list));
    memset(timer, 0, sizeof(struct timer_list));
    lua_settable(L, -3);

    luaL_getmetatable(L, CRON_TIMER_METATABLE);
    lua_setmetatable(L, -2);

    return 1;
}

static int l_update(lua_State *L)
{
    unsigned long now = (unsigned long)times(0);
    if (get_next_timeout(&timers, now) == 0)
        process_timers(&timers, now);
    return 0;
}

static int l_next(lua_State *L)
{
    unsigned long now = (unsigned long)times(0);
    unsigned long next = get_next_timeout(&timers, now) * MS_PER_TICK;
    lua_pushnumber(L, (lua_Number)next);
    return 1;
}

static struct timer_list* lua_checktimer(lua_State *L, int index)
{
    struct timer_list *timer = 0;
    luaL_checktype(L, index, LUA_TTABLE);

    if (!lua_getmetatable(L, index))
      luaL_typerror(L, index, CRON_TIMER_METATABLE);

    lua_getfield(L, LUA_REGISTRYINDEX, CRON_TIMER_METATABLE);  /* get correct metatable */
    if( !lua_rawequal(L, -1, -2) )
      luaL_typerror(L, index, CRON_TIMER_METATABLE);

    lua_pop(L, 2);

    lua_pushstring(L, "timer");
    lua_rawget(L, index);
    if( lua_isnil(L, -1) )
      luaL_typerror(L, index, CRON_TIMER_METATABLE);

    timer = (struct timer_list*)lua_touserdata(L, -1);
    lua_pop(L, 1);

    return timer;
}

static void register_timer(lua_State *L, int index, struct timer_list* timer)
{
    lua_pushstring(L, CRON_TIMERS_TABLE);
    lua_gettable(L, LUA_REGISTRYINDEX);
    luaL_checktype(L, -1, LUA_TTABLE);

    lua_pushlightuserdata(L, timer);
    lua_pushvalue(L, index);
    lua_settable(L, -3);

    lua_pop(L, 1);
}

static void unregister_timer(lua_State *L, struct timer_list* timer)
{
    lua_pushstring(L, CRON_TIMERS_TABLE);
    lua_gettable(L, LUA_REGISTRYINDEX);
    luaL_checktype(L, -1, LUA_TTABLE);

    lua_pushlightuserdata(L, timer);
    lua_pushnil(L);
    lua_settable(L, -3);

    lua_pop(L, 1);
}

static void lua_pushtimer(lua_State *L, struct timer_list* timer)
{
    lua_pushstring(L, CRON_TIMERS_TABLE);
    lua_gettable(L, LUA_REGISTRYINDEX);
    luaL_checktype(L, -1, LUA_TTABLE);

    lua_pushlightuserdata(L, timer);
    lua_gettable(L, -2);
}

static void lua_poptimer(lua_State *L)
{
    lua_pop(L, 2);
}

static void callback(struct timer_list* timer, void *data)
{
    int res;
    lua_State *L = (lua_State *)data;

    lua_pushtimer(L, timer);
    if (!lua_istable(L, -1)) {
        syslog(LOG_ERR, "CRON: system error: unable to get cron timer\n");
        return;
    }

    unregister_timer(L, timer);

    lua_pushstring(L, "callback");
    lua_gettable(L, -2);
    luaL_checktype(L, -1, LUA_TFUNCTION);
    lua_pushvalue(L, -2);

    res = lua_pcall(L, 1, 0, 0);
    if (res) {
        char const *name = "?";
        char const *err = luaL_checkstring(L, -1);
        lua_pushstring(L, "name");
        lua_gettable(L, -2);
        if (!lua_isnil(L, -1))
            name = lua_tostring(L, -1);
        syslog(LOG_NOTICE, "CRON: timer [%s] callback error: %s\n", name, err);
        lua_pop(L, 2);
    }

    lua_poptimer(L);
}

static int l_set_timer(lua_State *L)
{
    unsigned long expire;
    int timeout;
    struct timer_list* timer;

    timer = lua_checktimer(L, 1);
    timeout = luaL_checkinteger(L, 2);
    luaL_argcheck(L, timeout >= 0, 2, "negative timeout");

    expire = (unsigned long)times(0) + ((unsigned long)timeout + (MS_PER_TICK-1)) / MS_PER_TICK;

    set_timer(timer, expire, callback, L);
    add_timer(&timers, timer);
    register_timer(L, 1, timer);

    return 0;
}

static int l_reset_timer(lua_State *L)
{
    struct timer_list* timer = lua_checktimer(L, 1);

    del_timer(timer);
    unregister_timer(L, timer);

    return 0;
}

/* }====================================================== */

static const luaL_Reg lib[] = {
    {"new", l_new},
    {"set", l_set_timer},
    {"reset", l_reset_timer},
    {"update", l_update},
    {"next", l_next},
    {NULL, NULL}
};

/* }====================================================== */

static void create_timer_metatable(lua_State *L)
{
    luaL_newmetatable(L, CRON_TIMER_METATABLE);
}

LUALIB_API int luaopen_cron (lua_State *L)
{
    MS_PER_TICK = 1000UL / sysconf(_SC_CLK_TCK);
    init_timers(&timers, (unsigned long)times(0));

    lua_pushstring(L, CRON_TIMERS_TABLE);
    lua_newtable(L);
    lua_settable(L, LUA_REGISTRYINDEX);

    create_timer_metatable(L);

    luaL_register(L, LUA_CRONLIBNAME, lib);
    return 1;
}
