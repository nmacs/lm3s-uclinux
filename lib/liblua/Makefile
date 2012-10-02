LUA_DIR           := lua-5.1.5
LUA_INC           := "-I$(CURDIR)/$(LUA_DIR)/src"
LUAFILESYSTEM_DIR := luafilesystem-1.5.0
CFLAGS            += -DAUTOCONF -DLUA_STATIC_MODULES

ifdef CONFIG_LIB_LUA_LUAFILESYSTEM
  CFLAGS          += -Wl,-llfs -L$(CURDIR)/$(LUAFILESYSTEM_DIR)/src
endif

.PHONY: all lua

all: lua

lua:
	ln -sf $(ROOTDIR)/config/autoconf.h $(LUA_DIR)/src/autoconf.h
ifdef CONFIG_LIB_LUA_LUAFILESYSTEM
	$(MAKE) CFLAGS="$(CFLAGS) $(LUA_INC)" -C $(LUAFILESYSTEM_DIR) static
endif
	$(MAKE) -C $(LUA_DIR) generic

luafilesystem:


############################################################################

clean:
	$(MAKE) -C $(LUA_DIR) clean
	$(MAKE) -C $(LUAFILESYSTEM_DIR) clean

romfs:


romfs_user:

