LUA_STACK_SIZE    := 65536

LUA_DIR           := lua-5.1.5
LUAFILESYSTEM_DIR := luafilesystem-1.5.0
LUASOCKET_DIR     := luasocket-2.0.2
LUACOXPCALL_DIR   := coxpcall-1.13
LUAXAVANTE_DIR    := xavante-2.2.1
JSON_DIR          := json4lua-0.9.50
SQLITE_DIR        := lsqlite3_svn08
UCILUA_DIR        := libuci
BITSTRING_DIR     := bitstring-1.0
LSYSLOG_DIR       := lsyslog
LSIGNALS_DIR      := lsignals
LWATCHDOG_DIR     := lwatchdog
LAIO_DIR          := laio

CFLAGS            += $(LUA_INC) -DAUTOCONF -DLUA_STATIC_MODULES -Wl,-elf2flt="-s$(LUA_STACK_SIZE)"
LUA_INC           := "-I$(CURDIR)/$(LUA_DIR)/src"

lua_libs =

ifdef CONFIG_LIB_LUA_LUAFILESYSTEM
	CFLAGS          += -Wl,-llfs -L$(CURDIR)/$(LUAFILESYSTEM_DIR)/src
	lua_libs        += luafilesystem
endif

ifdef CONFIG_LIB_LUA_LUASOCKET
	CFLAGS          += -Wl,-lsocket -Wl,-lmime -L$(CURDIR)/$(LUASOCKET_DIR)/src
	lua_libs        += luasocket
endif

ifdef CONFIG_LIB_LUA_SQLITE
	CFLAGS          += -Wl,-llsqlite3 -Wl,-lsqlite3 -L$(CURDIR)/$(SQLITE_DIR)
	lua_libs        += luasqlite
endif

ifdef CONFIG_LIB_LUA_UCI
	CFLAGS          += -Wl,-lucilua -Wl,-luci -L$(CURDIR)/$(UCILUA_DIR)
	lua_libs        += luauci
endif

ifdef CONFIG_LIB_LUA_BITSTRING
	CFLAGS          += -Wl,-lbitstring -L$(CURDIR)/$(BITSTRING_DIR)/src/bitstring/.libs
	lua_libs        += luabitstring
endif

ifdef CONFIG_LIB_LUA_LSYSLOG
	CFLAGS          += -Wl,-llsyslog -L$(CURDIR)/$(LSYSLOG_DIR)
	lua_libs        += lsyslog
endif

ifdef CONFIG_LIB_LUA_LSIGNALS
	CFLAGS          += -Wl,-llsignals -L$(CURDIR)/$(LSIGNALS_DIR)
	lua_libs        += lsignals
endif

ifdef CONFIG_LIB_LUA_LWATCHDOG
	CFLAGS          += -Wl,-llwatchdog -L$(CURDIR)/$(LWATCHDOG_DIR)
	lua_libs        += lwatchdog
endif

ifdef CONFIG_LIB_LUA_LAIO
	CFLAGS          += -Wl,-llaio -L$(CURDIR)/$(LAIO_DIR)
	lua_libs        += laio
endif

.PHONY: all lua repo romfs

all: lua

lua: $(LUA_DIR)/src/autoconf.h $(lua_libs)
	$(MAKE) -C $(LUA_DIR) generic

.PHONY: lua_x86
lua_x86:
	mkdir -p $(LUA_DIR)-x86
	$(MAKE) -C $(LUA_DIR)-x86 -f $(CURDIR)/$(LUA_DIR)/src/Makefile \
		SRC_DIR=$(CURDIR)/$(LUA_DIR)/src MYCFLAGS="-DLUA_USE_POSIX -m32" \
		CC=gcc RANLIB=ranlib AR=ar all

$(LUA_DIR)/src/autoconf.h:
	ln -sf $(ROOTDIR)/config/autoconf.h $(LUA_DIR)/src/autoconf.h

.PHONY: luafilesystem
luafilesystem:
	$(MAKE) -C $(LUAFILESYSTEM_DIR) static

.PHONY: luasocket
luasocket:
	$(MAKE) -C $(LUASOCKET_DIR)/src libsocket.a libmime.a

.PHONY: luasqlite
luasqlite:
	$(MAKE) -C $(SQLITE_DIR) liblsqlite3.a

.PHONY: luauci
luauci:
	$(MAKE) -C $(UCILUA_DIR) static

.PHONY: luabitstring
luabitstring: $(BITSTRING_DIR)/Makefile
	$(MAKE) -C $(BITSTRING_DIR)

$(BITSTRING_DIR)/Makefile: Makefile
	cd $(BITSTRING_DIR) && ./configure --host=arm

.PHONY: lsyslog
lsyslog: $(LSYSLOG_DIR)/Makefile
	$(MAKE) -C $(LSYSLOG_DIR)

.PHONY: lsignals
lsignals: $(LSIGNALS_DIR)/Makefile
	$(MAKE) -C $(LSIGNALS_DIR)

.PHONY: lwatchdog
lwatchdog: $(LWATCHDOG_DIR)/Makefile
	$(MAKE) -C $(LWATCHDOG_DIR)

.PHONY: laio
laio: $(LAIO_DIR)/Makefile
	$(MAKE) -C $(LAIO_DIR)

############################################################################

clean:
	-$(MAKE) -C $(LUA_DIR) clean
	-$(MAKE) -C $(LUAFILESYSTEM_DIR) clean
	-$(MAKE) -C $(LUASOCKET_DIR) clean
	-$(MAKE) -C $(SQLITE_DIR) clean
	-$(MAKE) -C $(UCILUA_DIR) clean
	-$(MAKE) -C $(BITSTRING_DIR) clean
	-rm -f $(BITSTRING_DIR)/Makefile
	-$(MAKE) -C $(LSYSLOG_DIR) clean
	-$(MAKE) -C $(LSIGNALS_DIR) clean
	-$(MAKE) -C $(LWATCHDOG_DIR) clean
	-$(MAKE) -C $(LAIO_DIR) clean
	-rm -rf $(LUA_DIR)-x86

romfs:
	$(ROMFSINST) -e CONFIG_LIB_LUA_SHELL -d $(LUA_DIR)/src/lua /bin/lua
	$(ROMFSINST) -e CONFIG_LIB_LUA_LUASOCKET -d $(LUASOCKET_DIR)/src/socket.lua /usr/local/share/lua/5.1/socket.lua
	$(ROMFSINST) -e CONFIG_LIB_LUA_LUASOCKET -d $(LUASOCKET_DIR)/src/http.lua /usr/local/share/lua/5.1/socket/http.lua
	$(ROMFSINST) -e CONFIG_LIB_LUA_LUASOCKET -d $(LUASOCKET_DIR)/src/wget.lua /usr/local/share/lua/5.1/socket/wget.lua
	$(ROMFSINST) -e CONFIG_LIB_LUA_LUASOCKET -d $(LUASOCKET_DIR)/src/url.lua /usr/local/share/lua/5.1/socket/url.lua
	$(ROMFSINST) -e CONFIG_LIB_LUA_LUASOCKET -d $(LUASOCKET_DIR)/src/ltn12.lua /usr/local/share/lua/5.1/ltn12.lua
	$(ROMFSINST) -e CONFIG_LIB_LUA_LUASOCKET -d $(LUASOCKET_DIR)/src/mime.lua /usr/local/share/lua/5.1/mime.lua
	$(ROMFSINST) -e CONFIG_LIB_LUA_LUASOCKET -d $(LUASOCKET_DIR)/etc/cosocket.lua /usr/local/share/lua/5.1/socket/cosocket.lua
	$(ROMFSINST) -e CONFIG_LIB_LUA_LUAXAVANTE -d $(LUAXAVANTE_DIR)/src/xavante/xavante.lua /usr/local/share/lua/5.1/xavante.lua
	$(ROMFSINST) -e CONFIG_LIB_LUA_LUAXAVANTE -d $(LUAXAVANTE_DIR)/src/xavante /usr/local/share/lua/5.1/xavante
	$(ROMFSINST) -e CONFIG_LIB_LUA_JSON -d $(JSON_DIR)/json/json.lua /usr/local/share/lua/5.1/json.lua
	$(ROMFSINST) -e CONFIG_LIB_LUA_JSON -d $(JSON_DIR)/json/jsonrpc.lua /usr/local/share/lua/5.1/jsonrpc.lua

repo:
	$(REPOINST) -e CONFIG_LIB_LUA_SHELL $(LUA_DIR)/src/lua /bin/lua
	$(REPOINST) -e CONFIG_LIB_LUA_LUASOCKET $(LUASOCKET_DIR)/src/socket.lua /usr/local/share/lua/5.1/socket.lua
	$(REPOINST) -e CONFIG_LIB_LUA_LUASOCKET $(LUASOCKET_DIR)/src/http.lua /usr/local/share/lua/5.1/socket/http.lua
	$(REPOINST) -e CONFIG_LIB_LUA_LUASOCKET $(LUASOCKET_DIR)/src/wget.lua /usr/local/share/lua/5.1/socket/wget.lua
	$(REPOINST) -e CONFIG_LIB_LUA_LUASOCKET $(LUASOCKET_DIR)/src/url.lua /usr/local/share/lua/5.1/socket/url.lua
	$(REPOINST) -e CONFIG_LIB_LUA_LUASOCKET $(LUASOCKET_DIR)/src/ltn12.lua /usr/local/share/lua/5.1/ltn12.lua
	$(REPOINST) -e CONFIG_LIB_LUA_LUASOCKET $(LUASOCKET_DIR)/src/mime.lua /usr/local/share/lua/5.1/mime.lua
	$(REPOINST) -e CONFIG_LIB_LUA_LUASOCKET $(LUASOCKET_DIR)/etc/cosocket.lua /usr/local/share/lua/5.1/socket/cosocket.lua
	$(REPOINST) -e CONFIG_LIB_LUA_LUAXAVANTE $(LUAXAVANTE_DIR)/src/xavante/xavante.lua /usr/local/share/lua/5.1/xavante.lua
	$(REPOINST) -e CONFIG_LIB_LUA_LUAXAVANTE $(LUAXAVANTE_DIR)/src/xavante /usr/local/share/lua/5.1/xavante
	$(REPOINST) -e CONFIG_LIB_LUA_JSON $(JSON_DIR)/json/json.lua /usr/local/share/lua/5.1/json.lua
	$(REPOINST) -e CONFIG_LIB_LUA_JSON $(JSON_DIR)/json/jsonrpc.lua /usr/local/share/lua/5.1/jsonrpc.lua
	lua-compile $(CONTENT)

romfs_user:

