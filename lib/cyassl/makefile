OUTPUT       := "$(ROOTDIR)/lib/cyassl/output"
CONFIG_OPTS  := --disable-shared --enable-crl-monitor --enable-ecc --enable-fastmath --enable-opensslExtra --prefix=$(OUTPUT) --host=arm-linux

AM_CFLAGS    :=
#-DNO_SHA256 -DNO_DH -DNO_DES3 -DNO_DSA -DNO_ERROR_STRINGS -DNO_RABBIT -DNO_PSK -DNO_PWDBASED

SRC_DIR := cyassl-2.3.0

.PHONY: all lib

all: lib

lib: $(SRC_DIR)/Makefile
	$(MAKE) -C $(SRC_DIR)
	$(MAKE) -C $(SRC_DIR) install
	cd $(OUTPUT)/include && ln -fs cyassl/openssl openssl

$(SRC_DIR)/Makefile: makefile
	cd $(SRC_DIR); ac_cv_func_malloc_0_nonnull=yes ac_cv_func_realloc_0_nonnull=yes AM_CFLAGS="$(AM_CFLAGS)" ./configure $(CONFIG_OPTS)

############################################################################

clean:
	$(MAKE) -C $(SRC_DIR) clean
	-rm -rf $(OUTPUT)
	-rm -f $(SRC_DIR)/Makefile

romfs:


romfs_user:



