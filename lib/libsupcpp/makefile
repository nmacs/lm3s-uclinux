IN_LIBSUPCPP := true
export IN_LIBSUPCPP

OUTPUT       := "$(ROOTDIR)/lib/libsupcpp/output"

GCC_CONFIG_OPTS        := --target=arm-uclinuxeabi --enable-languages=c,c++ --without-headers --disable-nls
LIBSTDCPP_CONFIG_OPTS  := --prefix=$(OUTPUT) --host=arm-uclinuxeabi --with-target-subdir="." \
                          --disable-wchar_t --disable-hosted-libstdcxx --enable-languages=c++ \
                          --without-headers --disable-nls --disable-multilib

LIBGCC_CONFIG_OPTS     := --prefix=$(OUTPUT) --host=arm-uclinuxeabi

GCC_DIR := gcc-4.5.2
LIBSTDCPP_DIR := $(GCC_DIR)/libstdc++-v3
LIBGCC_DIR    := $(GCC_DIR)/libgcc

.PHONY: all libsupcpp clean romfs romfs_user

all: libsupcpp

.PHONY: gcc
gcc: gcc_config
	$(MAKE) -C $(GCC_DIR)

.PHONY: libgcc
libgcc: libgcc_config
	$(MAKE) -C $(LIBGCC_DIR)

.PHONY: libgcc_config
libgcc_config: $(LIBGCC_DIR)/Makefile

$(LIBGCC_DIR)/Makefile: gcc_config makefile
	-cd $(LIBGCC_DIR); bash ./configure $(LIBGCC_CONFIG_OPTS) || exit 1

libsupcpp: libstdcpp_config
	$(MAKE) -C $(LIBSTDCPP_DIR)
	$(MAKE) -C $(LIBSTDCPP_DIR) install

.PHONY: gcc_config
gcc_config: $(GCC_DIR)/Makefile

$(GCC_DIR)/Makefile: makefile
	-cd $(GCC_DIR); CFLAGS="" LDFLAGS="" CXX=g++ CC=gcc AR=ar RANLIB=ranlib ./configure $(GCC_CONFIG_OPTS) || exit 1

.PHONY: libstdcpp_config
libstdcpp_config: $(LIBSTDCPP_DIR)/Makefile

$(LIBSTDCPP_DIR)/Makefile: $(GCC_DIR)/Makefile makefile
	-cd $(LIBSTDCPP_DIR); ./configure $(LIBSTDCPP_CONFIG_OPTS) CXXFLAGS="$(CXXFLAGS)" || exit 1

############################################################################

clean:
	$(MAKE) -C $(LIBSTDCPP_DIR) clean
	rm -f $(LIBSTDCPP_DIR)/Makefile

romfs:


romfs_user:



