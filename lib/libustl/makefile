IN_LIBUSTL := true
export IN_LIBUSTL

OUTPUT       := "../output"
CONFIG_OPTS  := --prefix=$(OUTPUT) --without-shared --with-static --without-bounds --without-demangler --without-libstdc++

SRC_DIR := ustl-1.6

.PHONY: all lib

all: $(SRC_DIR)/config.h lib

lib:
	$(MAKE) -C $(SRC_DIR)
	$(MAKE) -C $(SRC_DIR) install

$(SRC_DIR)/config.h: makefile
	cd $(SRC_DIR); ./configure $(CONFIG_OPTS)

############################################################################

clean:
	$(MAKE) -C $(SRC_DIR) clean
	-rm -rf $(OUTPUT)

romfs:


romfs_user:



