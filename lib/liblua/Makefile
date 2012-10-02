LUA_DIR           := lua-5.1.5
LUAFILESYSTEM_DIR := luafilesystem-1.5.0
LUASOCKET_DIR     := luasocket-2.0.2

CFLAGS            += $(LUA_INC) -DAUTOCONF -DLUA_STATIC_MODULES
LUA_INC           := "-I$(CURDIR)/$(LUA_DIR)/src"

ifdef CONFIG_LIB_LUA_LUAFILESYSTEM
  CFLAGS          += -Wl,-llfs -L$(CURDIR)/$(LUAFILESYSTEM_DIR)/src
endif

ifdef CONFIG_LIB_LUA_LUASOCKET
  CFLAGS          += -Wl,-lsocket -L$(CURDIR)/$(LUASOCKET_DIR)/src
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
	$(MAKE) -C $(LUA_DIR) generic

############################################################################

clean:
	$(MAKE) -C $(LUA_DIR) clean
	$(MAKE) -C $(LUAFILESYSTEM_DIR) clean
	$(MAKE) -C $(LUASOCKET_DIR) clean

romfs:
	$(ROMFSINST) $(LUA_DIR)/src/lua /bin/lua
ifdef CONFIG_LIB_LUA_LUASOCKET
	$(ROMFSINST) -d $(LUASOCKET_DIR)/src/socket.lua /usr/local/lib/lua/5.1/socket.lua
endif

romfs_user:

