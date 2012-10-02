LUA_DIR           := lua-5.1.5
LUA_INC           := "-I$(CURDIR)/$(LUA_DIR)/src"
LUAFILESYSTEM_DIR := luafilesystem-1.5.0

.PHONY: all lua

all: lua

lua:
	$(MAKE) -C $(LUA_DIR) generic
ifdef CONFIG_LIB_LUA_LUAFILESYSTEM
	$(MAKE) CFLAGS="$(CFLAGS) $(LUA_INC)" -C $(LUAFILESYSTEM_DIR) static
endif

luafilesystem:


############################################################################

clean:
	$(MAKE) -C $(LUA_DIR) clean
	$(MAKE) -C $(LUAFILESYSTEM_DIR) clean

romfs:


romfs_user:

