LUA_STACK_SIZE    := 65536

LUA_DIR           := lua-5.1.5
LUAFILESYSTEM_DIR := luafilesystem-1.5.0
LUASOCKET_DIR     := luasocket-2.0.2
LUACOXPCALL_DIR   := coxpcall-1.13
LUACOPAS_DIR      := copas-1.1.6
LUAXAVANTE_DIR    := xavante-2.2.1
JSON_DIR          := json4lua-0.9.50
SQLITE_DIR        := lsqlite3_svn08
UCILUA_DIR        := libuci
BITSTRING_DIR     := bitstring-1.0
LSYSLOG_DIR       := lsyslog

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

.PHONY: all lua repo romfs

all: lua

lua: $(LUA_DIR)/src/autoconf.h $(lua_libs)
	$(MAKE) -C $(LUA_DIR) generic

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

############################################################################

clean:
	$(MAKE) -C $(LUA_DIR) clean
	$(MAKE) -C $(LUAFILESYSTEM_DIR) clean
	$(MAKE) -C $(LUASOCKET_DIR) clean
	$(MAKE) -C $(SQLITE_DIR) clean
	$(MAKE) -C $(UCILUA_DIR) clean
	$(MAKE) -C $(BITSTRING_DIR) clean
	-rm -f $(BITSTRING_DIR)/Makefile
	$(MAKE) -C $(LSYSLOG_DIR) clean

romfs:
ifdef CONFIG_LIB_LUA_SHELL
	$(ROMFSINST) $(LUA_DIR)/src/lua /bin/lua
endif
ifdef CONFIG_LIB_LUA_LUASOCKET
	$(ROMFSINST) -d $(LUASOCKET_DIR)/src/socket.lua /usr/local/share/lua/5.1/socket.lua
	$(ROMFSINST) -d $(LUASOCKET_DIR)/src/http.lua /usr/local/share/lua/5.1/socket/http.lua
	$(ROMFSINST) -d $(LUASOCKET_DIR)/src/wget.lua /usr/local/share/lua/5.1/socket/wget.lua
	$(ROMFSINST) -d $(LUASOCKET_DIR)/src/url.lua /usr/local/share/lua/5.1/socket/url.lua
	$(ROMFSINST) -d $(LUASOCKET_DIR)/src/ltn12.lua /usr/local/share/lua/5.1/ltn12.lua
	$(ROMFSINST) -d $(LUASOCKET_DIR)/src/mime.lua /usr/local/share/lua/5.1/mime.lua
endif
ifdef CONFIG_LIB_LUA_LUACOXPCALL
	$(ROMFSINST) -d $(LUACOXPCALL_DIR)/src/coxpcall.lua /usr/local/share/lua/5.1/coxpcall.lua
endif
ifdef CONFIG_LIB_LUA_LUACOPAS
	$(ROMFSINST) -d $(LUACOPAS_DIR)/src/copas/copas.lua /usr/local/share/lua/5.1/copas.lua
	$(ROMFSINST) -d $(LUACOPAS_DIR)/tests/cosocket.lua /usr/local/share/lua/5.1/copas/cosocket.lua
endif
ifdef CONFIG_LIB_LUA_LUAXAVANTE
	$(ROMFSINST) -d $(LUAXAVANTE_DIR)/src/xavante/xavante.lua /usr/local/share/lua/5.1/xavante.lua
	$(ROMFSINST) -d $(LUAXAVANTE_DIR)/src/xavante /usr/local/share/lua/5.1/xavante
endif
ifdef CONFIG_LIB_LUA_JSON
	$(ROMFSINST) -d $(JSON_DIR)/json/json.lua /usr/local/share/lua/5.1/json.lua
	$(ROMFSINST) -d $(JSON_DIR)/json/jsonrpc.lua /usr/local/share/lua/5.1/jsonrpc.lua
endif

REPO_TARGET="$(REPODIR)/lua/content"

repo:
ifdef CONFIG_LIB_LUA_SHELL
	mkdir -p $(REPO_TARGET)/bin
	cp $(LUA_DIR)/src/lua $(REPO_TARGET)/bin/lua
endif
ifdef CONFIG_LIB_LUA_LUASOCKET
	mkdir -p $(REPO_TARGET)/usr/local/share/lua/5.1/socket
	cp $(LUASOCKET_DIR)/src/socket.lua $(REPO_TARGET)/usr/local/share/lua/5.1/socket.lua
	cp $(LUASOCKET_DIR)/src/http.lua $(REPO_TARGET)/usr/local/share/lua/5.1/socket/http.lua
	cp $(LUASOCKET_DIR)/src/wget.lua $(REPO_TARGET)/usr/local/share/lua/5.1/socket/wget.lua
	cp $(LUASOCKET_DIR)/src/url.lua $(REPO_TARGET)/usr/local/share/lua/5.1/socket/url.lua
	cp $(LUASOCKET_DIR)/src/ltn12.lua $(REPO_TARGET)/usr/local/share/lua/5.1/ltn12.lua
	cp $(LUASOCKET_DIR)/src/mime.lua $(REPO_TARGET)/usr/local/share/lua/5.1/mime.lua
endif
ifdef CONFIG_LIB_LUA_LUACOXPCALL
	mkdir -p $(REPO_TARGET)/usr/local/share/lua/5.1
	cp $(LUACOXPCALL_DIR)/src/coxpcall.lua $(REPO_TARGET)/usr/local/share/lua/5.1/coxpcall.lua
endif
ifdef CONFIG_LIB_LUA_LUACOPAS
	mkdir -p $(REPO_TARGET)/usr/local/share/lua/5.1/copas
	cp $(LUACOPAS_DIR)/src/copas/copas.lua $(REPO_TARGET)/usr/local/share/lua/5.1/copas.lua
	cp $(LUACOPAS_DIR)/tests/cosocket.lua $(REPO_TARGET)/usr/local/share/lua/5.1/copas/cosocket.lua
endif
ifdef CONFIG_LIB_LUA_LUAXAVANTE
	mkdir -p $(REPO_TARGET)/usr/local/share/lua/5.1
	cp $(LUAXAVANTE_DIR)/src/xavante/xavante.lua $(REPO_TARGET)/usr/local/share/lua/5.1/xavante.lua
	cp -r $(LUAXAVANTE_DIR)/src/xavante $(REPO_TARGET)/usr/local/share/lua/5.1/
endif
ifdef CONFIG_LIB_LUA_JSON
	mkdir -p $(REPO_TARGET)/usr/local/share/lua/5.1
	cp $(JSON_DIR)/json/json.lua $(REPO_TARGET)/usr/local/share/lua/5.1/json.lua
	cp $(JSON_DIR)/json/jsonrpc.lua $(REPO_TARGET)/usr/local/share/lua/5.1/jsonrpc.lua
endif

romfs_user:

