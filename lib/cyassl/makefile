OUTPUT       := "$(ROOTDIR)/lib/cyassl/output"
CONFIG_OPTS  := --disable-shared --enable-crl-monitor --enable-ecc --enable-fastmath --prefix=$(OUTPUT) --host=arm-linux

AM_CFLAGS    := -DNO_DES3 -DNO_ERROR_STRINGS -DNO_RABBIT -DNO_PSK -DNO_DSA -DNO_DH -DNO_PWDBASED
#-DNO_SHA256

SRC_DIR := cyassl-2.3.0

.PHONY: all lib

all: lib

lib: $(SRC_DIR)/Makefile
	$(MAKE) -C $(SRC_DIR)
	$(MAKE) -C $(SRC_DIR) install

$(SRC_DIR)/Makefile: makefile
	cd $(SRC_DIR); ac_cv_func_malloc_0_nonnull=yes ac_cv_func_realloc_0_nonnull=yes AM_CFLAGS="$(AM_CFLAGS)" ./configure $(CONFIG_OPTS)

############################################################################

clean:
	$(MAKE) -C $(SRC_DIR) clean
	-rm -rf $(OUTPUT)
	-rm -f $(SRC_DIR)/Makefile

romfs:


romfs_user:



