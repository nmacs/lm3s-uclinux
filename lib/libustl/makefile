OUTPUT       := "$(CURDIR)/output"
CONFIG_OPTS  := --prefix=$(OUTPUT) --without-shared --with-static --without-bounds --without-demangler

ifndef HOSTBUILD
IN_LIBUSTL  := true
export IN_LIBUSTL
CONFIG_OPTS += --without-libstdc++ --oldincludedir="$(ROOTDIR)/toolchain/output/arm-uclinuxeabi/libc/usr/include"
EXTERNCONFIG := $(CURDIR)/config.sed
export EXTERNCONFIG
endif

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
	-$(MAKE) -C $(SRC_DIR) clean
	-rm -rf $(OUTPUT)
	-rm -f $(SRC_DIR)/config.h
	-rm -f $(SRC_DIR)/config.sed

romfs:


romfs_user:



