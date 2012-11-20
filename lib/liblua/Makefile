LUA_STACK_SIZE    := 65536

LUA_DIR           := lua-5.1.5
LUAFILESYSTEM_DIR := luafilesystem-1.5.0
LUASOCKET_DIR     := luasocket-2.0.2
LUACOXPCALL_DIR   := coxpcall-1.13
LUACOPAS_DIR      := copas-1.1.6
LUAXAVANTE_DIR    := xavante-2.2.1
JSON_DIR          := json4lua-0.9.50
SQLITE_DIR        := lsqlite3_svn08

CFLAGS            += $(LUA_INC) -DAUTOCONF -DLUA_STATIC_MODULES -Wl,-elf2flt="-s$(LUA_STACK_SIZE)"
LUA_INC           := "-I$(CURDIR)/$(LUA_DIR)/src"

ifdef CONFIG_LIB_LUA_LUAFILESYSTEM
	CFLAGS          += -Wl,-llfs -L$(CURDIR)/$(LUAFILESYSTEM_DIR)/src
endif

ifdef CONFIG_LIB_LUA_LUASOCKET
	CFLAGS          += -Wl,-lsocket -L$(CURDIR)/$(LUASOCKET_DIR)/src
endif

ifdef CONFIG_LIB_LUA_SQLITE
	CFLAGS          += -Wl,-llsqlite3 -Wl,-lsqlite3 -L$(CURDIR)/$(SQLITE_DIR)
endif

.PHONY: all lua

all: lua

lua:
	ln -sf $(ROOTDIR)/config/autoconf.h $(LUA_DIR)/src/autoconf.h
ifdef CONFIG_LIB_LUA_LUAFILESYSTEM
	$(MAKE) -C $(LUAFILESYSTEM_DIR) static
endif
ifdef CONFIG_LIB_LUA_LUASOCKET
	$(MAKE) -C $(LUASOCKET_DIR)/src libsocket.a
endif
ifdef CONFIG_LIB_LUA_SQLITE
	$(MAKE) -C $(SQLITE_DIR) liblsqlite3.a
endif
	$(MAKE) -C $(LUA_DIR) generic

############################################################################

clean:
	$(MAKE) -C $(LUA_DIR) clean
	$(MAKE) -C $(LUAFILESYSTEM_DIR) clean
	$(MAKE) -C $(LUASOCKET_DIR) clean
	$(MAKE) -C $(SQLITE_DIR) clean

romfs:
	$(ROMFSINST) $(LUA_DIR)/src/lua /bin/lua
ifdef CONFIG_LIB_LUA_LUASOCKET
	$(ROMFSINST) -d $(LUASOCKET_DIR)/src/socket.lua /usr/local/share/lua/5.1/socket.lua
	$(ROMFSINST) -d $(LUASOCKET_DIR)/src/url.lua /usr/local/share/lua/5.1/socket/url.lua
endif
ifdef CONFIG_LIB_LUA_LUACOXPCALL
	$(ROMFSINST) -d $(LUACOXPCALL_DIR)/src/coxpcall.lua /usr/local/share/lua/5.1/coxpcall.lua
endif
ifdef CONFIG_LIB_LUA_LUACOPAS
	$(ROMFSINST) -d $(LUACOPAS_DIR)/src/copas/copas.lua /usr/local/share/lua/5.1/copas.lua
endif
ifdef CONFIG_LIB_LUA_LUAXAVANTE
	$(ROMFSINST) -d $(LUAXAVANTE_DIR)/src/xavante/xavante.lua /usr/local/share/lua/5.1/xavante.lua
	$(ROMFSINST) -d $(LUAXAVANTE_DIR)/src/xavante /usr/local/share/lua/5.1/xavante
endif
ifdef CONFIG_LIB_LUA_JSON
	$(ROMFSINST) -d $(JSON_DIR)/json/json.lua /usr/local/share/lua/5.1/json.lua
endif

romfs_user:

