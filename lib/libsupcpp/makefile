IN_LIBSUPCPP := true
export IN_LIBSUPCPP

OUTPUT       := "$(ROOTDIR)/lib/libsupcpp/output"

GCC_CONFIG_OPTS        := --host=arm-uclinux --enable-languages=c,c++ --without-headers --disable-nls
LIBSTDCPP_CONFIG_OPTS  := --prefix=$(OUTPUT) --host=arm-uclinux --with-target-subdir="." \
                          --disable-wchar_t --disable-hosted-libstdcxx --enable-languages=c++ \
                          --without-headers --disable-nls --disable-multilib

GCC_DIR := gcc-4.5.2
LIBSTDCPP_DIR := $(GCC_DIR)/libstdc++-v3

.PHONY: all gcc_config libstdcpp_config libsupcpp clean romfs romfs_user

all: libsupcpp

libsupcpp: libstdcpp_config
	$(MAKE) -C $(LIBSTDCPP_DIR)
	$(MAKE) -C $(LIBSTDCPP_DIR) install

gcc_config: makefile
	-cd $(GCC_DIR); ./configure $(GCC_CONFIG_OPTS) || exit 1

libstdcpp_config: $(LIBSTDCPP_DIR)/Makefile

$(LIBSTDCPP_DIR)/Makefile: makefile
	-cd $(LIBSTDCPP_DIR); ./configure $(LIBSTDCPP_CONFIG_OPTS) CXXFLAGS="$(CXXFLAGS)" || exit 1

############################################################################

clean:
	$(MAKE) -C $(LIBSTDCPP_DIR) clean
	rm -f $(LIBSTDCPP_DIR)/Makefile

romfs:


romfs_user:



