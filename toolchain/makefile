OUTPUT        := $(CURDIR)/output

BINUTILS_DIR  := $(CURDIR)/src/binutils-2011.03
GCC_DIR       := $(CURDIR)/src/gcc-4.5-2011.03
LIBELF_DIR    := $(CURDIR)/src/libelf-2011.03
LIBGMP_DIR    := $(CURDIR)/src/gmp-2011.03
LIBMPFR_DIR   := $(CURDIR)/src/mpfr-2011.03
LIBMPC_DIR    := $(CURDIR)/src/mpc-0.8.1
LIBPPL_DIR    := $(CURDIR)/src/ppl-0.10.2
LIBCLOOG_DIR  := $(CURDIR)/src/cloog-0.15
UCLIBC_DIR    := $(CURDIR)/src/uclibc-0.29
ELF2FLT_DIR   := $(CURDIR)/src/elf2flt-2006q1
ZLIB_DIR      := $(CURDIR)/src/zlib-1.2.3

BINUTILS_CONFIG_OPTS   := --prefix=$(OUTPUT) --target=arm-uclinuxeabi --enable-install-libbfd --disable-nls --disable-werror \
                          '--with-pkgversion=Sourcery G++ Lite 2011.03-46' --with-bugurl=https://support.codesourcery.com/GNUToolchain/ \
                          --with-sysroot=$(OUTPUT)/arm-uclinuxeabi/libc

LIBELF_CONFIG_OPTS     := --prefix=$(OUTPUT) --target=arm-uclinuxeabi --disable-shared --disable-nls
LIBGMP_CONFIG_OPTS     := --prefix=$(OUTPUT) --disable-shared --enable-cxx --disable-nls
LIBMPFR_CONFIG_OPTS    := --prefix=$(OUTPUT) --target=arm-uclinuxeabi --disable-shared --disable-nls --with-gmp=$(OUTPUT)
LIBMPC_CONFIG_OPTS     := --prefix=$(OUTPUT) --target=arm-uclinuxeabi --disable-shared --disable-nls \
                          --with-gmp=$(OUTPUT) --with-mpfr=$(OUTPUT)
LIBPPL_CONFIG_OPTS     := --prefix=$(OUTPUT) --target=arm-uclinuxeabi --disable-shared  --disable-nls --with-libgmp-prefix=$(OUTPUT) \
                          CPPFLAGS=-fpermissive
LIBCLOOG_CONFIG_OPTS   := --prefix=$(OUTPUT) --target=arm-uclinuxeabi --disable-shared --disable-nls \
                          --with-ppl=$(OUTPUT) --with-gmp=$(OUTPUT)
GCC_CONFIG_OPTS        := --target=arm-uclinuxeabi --disable-threads --disable-libmudflap --disable-libssp \
                          --disable-libstdcxx-pch --with-gnu-as \
                          --disable-multilib --with-cpu=cortex-m3 --with-mode=thumb --with-arch=armv7-m --with-tune=cortex-m3 \
                          --with-gnu-ld '--with-specs=%{save-temps: -fverbose-asm} -D__CS_SOURCERYGXX_MAJ__=2011 -D__CS_SOURCERYGXX_MIN__=3 -D__CS_SOURCERYGXX_REV__=46 %{O2:%{!fno-remove-local-statics: -fremove-local-statics}} %{O*:%{O|O0|O1|O2|Os:;:%{!fno-remove-local-statics: -fremove-local-statics}}}' \
                          --enable-languages=c,c++ --disable-shared --enable-lto '--with-pkgversion=Sourcery G++ Lite 2011.03-46' \
                          --with-bugurl=https://support.codesourcery.com/GNUToolchain/ --disable-nls --prefix=$(OUTPUT) \
                          --disable-shared --disable-threads --disable-libssp --disable-libgomp --without-headers \
                          --disable-decimal-float --disable-libffi --disable-libquadmath --enable-languages=c,c++ \
                          --with-sysroot=$(OUTPUT)/arm-uclinuxeabi/libc --with-build-sysroot=$(OUTPUT)/arm-uclinuxeabi/libc \
                          --with-gmp=$(OUTPUT) \
                          --with-mpfr=$(OUTPUT) \
                          --with-mpc=$(OUTPUT) \
                          --with-ppl=$(OUTPUT) \
                          '--with-host-libstdcxx=-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm' \
                          --with-cloog=$(OUTPUT) \
                          --with-libelf=$(OUTPUT) \
                          --disable-libgomp \
                          --with-build-time-tools=$(OUTPUT)/bin \
                          --disable-wchar_t \
                          CFLAGS="-Os -ffunction-sections -fdata-sections -Wl,--gc-sections" CXXFLAGS="-Os -ffunction-sections -fdata-sections -Wl,--gc-sections"

ELF2FLT_CONFIG_OPTS    := --prefix=$(OUTPUT) --target=arm-uclinuxeabi \
                          '--with-pkgversion=Sourcery G++ Lite 2011.03-46' \
                          --with-bugurl=https://support.codesourcery.com/GNUToolchain/ \
                          --disable-nls \
                          --with-bfd-include-dir=$(OUTPUT)/`$(GCC_DIR)/config.guess`/arm-uclinuxeabi/include \
                          --with-libbfd=$(OUTPUT)/`$(GCC_DIR)/config.guess`/arm-uclinuxeabi/lib/libbfd.a \
                          --with-binutils-include-dir=$(BINUTILS_DIR)/include \
                          --with-zlib-prefix=$(OUTPUT) \
                          --with-libiberty=$(OUTPUT)/lib/libiberty.a

ZLIB_CONFIG_OPTS       := --prefix=$(OUTPUT)

.PHONY: all
all: unpack-s toolchain-s

unpack-s:
	rm -rf $(CURDIR)/src; mkdir -p $(CURDIR)/src; cd $(CURDIR)/src; for f in ../*.tar.bz2; do tar -xf $$f; done
	touch unpack-s

toolchain-s:
	$(MAKE) gcc || exit 1
	touch toolchain-s

.PHONY: binutils
binutils: binutils_config
	$(MAKE) -j4 -C $(BINUTILS_DIR)/build
	$(MAKE) -C $(BINUTILS_DIR)/build install

binutils_config:
	mkdir -p $(BINUTILS_DIR)/build; cd $(BINUTILS_DIR)/build; ../configure $(BINUTILS_CONFIG_OPTS) || exit 1

libelf: libelf_config
	$(MAKE) -j4 -C $(LIBELF_DIR)/build
	$(MAKE) -C $(LIBELF_DIR)/build install

libelf_config:
	mkdir -p $(LIBELF_DIR)/build; cd $(LIBELF_DIR)/build; ../configure $(LIBELF_CONFIG_OPTS) || exit 1

libgmp: libgmp_config
	$(MAKE) -j4 -C $(LIBGMP_DIR)/build
	$(MAKE) -C $(LIBGMP_DIR)/build install

libgmp_config:
	mkdir -p $(LIBGMP_DIR)/build; cd $(LIBGMP_DIR)/build; ../configure $(LIBGMP_CONFIG_OPTS) || exit 1

libmpfr: libmpfr_config
	$(MAKE) -j4 -C $(LIBMPFR_DIR)/build
	$(MAKE) -C $(LIBMPFR_DIR)/build install

libmpfr_config:
	mkdir -p $(LIBMPFR_DIR)/build; cd $(LIBMPFR_DIR)/build; ../configure $(LIBMPFR_CONFIG_OPTS) || exit 1

libmpc: libmpc_config
	$(MAKE) -j4 -C $(LIBMPC_DIR)/build
	$(MAKE) -C $(LIBMPC_DIR)/build install

libmpc_config:
	mkdir -p $(LIBMPC_DIR)/build; cd $(LIBMPC_DIR)/build; ../configure $(LIBMPC_CONFIG_OPTS) || exit 1

libppl: libppl_config
	$(MAKE) -j4 -C $(LIBPPL_DIR)/build
	$(MAKE) -C $(LIBPPL_DIR)/build install

libppl_config:
	mkdir -p $(LIBPPL_DIR)/build; cd $(LIBPPL_DIR)/build; ../configure $(LIBPPL_CONFIG_OPTS) || exit 1

libcloog: libcloog_config
	$(MAKE) -j4 -C $(LIBCLOOG_DIR)/build
	$(MAKE) -C $(LIBCLOOG_DIR)/build install

libcloog_config:
	mkdir -p $(LIBCLOOG_DIR)/build; cd $(LIBCLOOG_DIR)/build; ../configure $(LIBCLOOG_CONFIG_OPTS) || exit 1

gcc: gcc_stage_1 elf2flt libc
	AR_FOR_TARGET=arm-uclinuxeabi-ar NM_FOR_TARGET=arm-uclinuxeabi-nm OBJDUMP_FOR_TARGET=arm-uclinuxeabi-objdump STRIP_FOR_TARGET=arm-uclinuxeabi-strip $(MAKE) -j4 -C $(GCC_DIR)/build LDFLAGS_FOR_TARGET=--sysroot=$(OUTPUT)/arm-uclinuxeabi/libc CPPFLAGS_FOR_TARGET=--sysroot=$(OUTPUT)/arm-uclinuxeabi/libc build_tooldir=$(OUTPUT)/arm-uclinuxeabi all
	$(MAKE) -C $(GCC_DIR)/build install

gcc_stage_1: linux-headers patch_gcc-s binutils libelf libgmp libmpfr libmpc libppl libcloog gcc_config
	AR_FOR_TARGET=arm-uclinuxeabi-ar NM_FOR_TARGET=arm-uclinuxeabi-nm OBJDUMP_FOR_TARGET=arm-uclinuxeabi-objdump STRIP_FOR_TARGET=arm-uclinuxeabi-strip $(MAKE) -j4 -C $(GCC_DIR)/build LDFLAGS_FOR_TARGET=--sysroot=$(OUTPUT)/arm-uclinuxeabi/libc CPPFLAGS_FOR_TARGET=--sysroot=$(OUTPUT)/arm-uclinuxeabi/libc build_tooldir=$(OUTPUT)/arm-uclinuxeabi all-gcc
	$(MAKE) -C $(GCC_DIR)/build install-gcc

patch_gcc-s:
	cd $(GCC_DIR); patch -p1 < $(CURDIR)/gcc-patch-01
	cd $(GCC_DIR); patch -p1 < $(CURDIR)/gcc-patch-02
	touch patch_gcc-s

libc:
	cp $(PRODUCTDIR)/config.uClibc $(UCLIBC_DIR)/.config
	yes "" | $(MAKE) -C $(UCLIBC_DIR) oldconfig
	STAGEDIR="$(OUTPUT)/arm-uclinuxeabi/libc" CROSS="$(OUTPUT)/bin/arm-uclinuxeabi-" CFLAGS="-Os -ffunction-sections -fdata-sections -Wl,--gc-sections" $(MAKE) -C $(UCLIBC_DIR) V=1
	STAGEDIR="$(OUTPUT)/arm-uclinuxeabi/libc" CROSS="$(OUTPUT)/bin/arm-uclinuxeabi-" PREFIX=$(OUTPUT)/arm-uclinuxeabi/libc $(MAKE) -C $(UCLIBC_DIR) V=1 install

linux-headers:
	$(MAKE) -C $(ROOTDIR)/$(LINUXDIR) ARCH=arm INSTALL_HDR_PATH=$(OUTPUT)/arm-uclinuxeabi/libc/usr headers_install

gcc_config:
	mkdir -p $(GCC_DIR)/build; cd $(GCC_DIR)/build; ../configure $(GCC_CONFIG_OPTS) || exit 1

zlib: zlib_config
	$(MAKE) -j4 -C $(ZLIB_DIR)
	$(MAKE) -C $(ZLIB_DIR) install

zlib_config:
	cd $(ZLIB_DIR); ./configure $(ZLIB_CONFIG_OPTS) || exit 1

elf2flt: patch_elf2flt-s zlib elf2flt_config
	$(MAKE) -j4 -C $(ELF2FLT_DIR)/build
	$(MAKE) -C $(ELF2FLT_DIR)/build install

patch_elf2flt-s:
	cd $(ELF2FLT_DIR); patch -p1 < $(CURDIR)/elf2flt-patch-01
	touch patch_elf2flt-s

elf2flt_config:
	mkdir -p $(ELF2FLT_DIR)/build; cd $(ELF2FLT_DIR)/build; ../configure $(ELF2FLT_CONFIG_OPTS) || exit 1

clean:
	rm -rf $(CURDIR)/src
	rm -f  $(CURDIR)/*-s
	rm -rf $(OUTPUT)

romfs:

romfs_user:



