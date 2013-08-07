#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "lua.h"
#include "lauxlib.h"

static sigset_t pending;
static int sigpipe[2];

static void sighandler(int sig)
{
	sigaddset(&pending, sig);
	write(sigpipe[1], &sig, sizeof(sig));
}

static sigset_t block_signals(void)
{
        sigset_t old;
        sigset_t new;
        sigfillset(&new);
        sigprocmask(SIG_BLOCK, &new, &old);
        return old;
}

static void unblock_signals(sigset_t old)
{
        sigprocmask(SIG_SETMASK, &old, NULL);
}

/* Lua: sigaction(sig, handler) */
static int l_signal(lua_State *L)
{
	struct sigaction sa;
	int signum = luaL_checkinteger(L, 1);
	
	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = sighandler;
	if (sigaction(signum, &sa, NULL)) {
		lua_pushboolean(L, 0);
		lua_pushstring(L, strerror(errno));
		return 2;
	}

	if (lua_type(L, 2) == LUA_TFUNCTION) {
		lua_pushinteger(L, signum);
		lua_pushvalue(L, 2);
		lua_settable(L, LUA_ENVIRONINDEX);
	}

	lua_pushboolean(L, 1);
	return 1;
}

static int l_sigpipe(lua_State *L)
{
        lua_pushinteger(L, sigpipe[0]);
        return 1;
}

static int l_callpending(lua_State *L)
{
	sigset_t signals;
	sigset_t old;
	int sig;

	old = block_signals();
	signals = pending;
	sigemptyset(&pending);
	while( read(sigpipe[0], &sig, 4) > 0 ) {}
	unblock_signals(old);

	lua_pushvalue(L, LUA_ENVIRONINDEX);
	lua_pushnil(L);
	while (lua_next(L, -2)) {
		int signo;
		// stack: -1 => value; -2 => key; -3 => table
		lua_pushvalue(L, -1);
		// stack: -1 => value; -2 => value; -3 => key; -4 => table
		lua_pushvalue(L, -3);
		// stack: -1 => key; -2 => value; -3 => value; -4 => key; -5 => table
		signo = luaL_checkinteger(L, -1);
		if (sigismember(&signals, signo) == 1)
			lua_call(L, 1, 0);
		else
			lua_pop(L, 2);
		// pop value
		lua_pop(L, 1);
		// stack: -1 => key; -2 => table
	}
	lua_pop(L, 1);
	return 0;
}

static const struct luaL_Reg mylib [] = {
	{ "signal", l_signal },
	{ "callpending", l_callpending },
	{ "sigpipe", l_sigpipe },
	{ NULL, NULL }
};

int luaopen_signals (lua_State *L)
{
        int res = pipe2(sigpipe, O_NONBLOCK);
        if (res)
                return 0;

	sigemptyset(&pending);

	/* create a module environment to keep the ident string */
	lua_newtable(L);
	lua_replace(L, LUA_ENVIRONINDEX);
	luaL_register(L, "signals", mylib);
	return 1;
}
