IN_LIBUSTL := true
export IN_LIBUSTL

OUTPUT       := "$(ROOTDIR)/lib/libustl/output"
CONFIG_OPTS  := --prefix=$(OUTPUT) --oldincludedir="$(ROOTDIR)/lib/libc/include" --without-shared --with-static --without-bounds --without-demangler --without-libstdc++

SRC_DIR := ustl-1.6

.PHONY: all lib

all: lib

lib: $(SRC_DIR)/config.h
	$(MAKE) -C $(SRC_DIR)
	$(MAKE) -C $(SRC_DIR) install

$(SRC_DIR)/config.h: makefile
	cd $(SRC_DIR); ./configure $(CONFIG_OPTS)

############################################################################

clean:
	$(MAKE) -C $(SRC_DIR) clean
	-rm -rf $(OUTPUT)
	-rm -f $(SRC_DIR)/config.h

romfs:


romfs_user:



