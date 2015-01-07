OUTPUT        := $(CURDIR)/output

BINUTILS_DIR  := $(CURDIR)/src/binutils-2.25
GCC_DIR       := $(CURDIR)/src/gcc-4.9.2
LIBELF_DIR    := $(CURDIR)/src/libelf-2011.03
LIBGMP_DIR    := $(CURDIR)/src/gmp-6.0.0
LIBMPFR_DIR   := $(CURDIR)/src/mpfr-2011.03
LIBMPC_DIR    := $(CURDIR)/src/mpc-0.8.1
LIBPPL_DIR    := $(CURDIR)/src/ppl-1.1
LIBCLOOG_DIR  := $(CURDIR)/src/cloog-0.18.1
UCLIBC_DIR    := $(CURDIR)/src/uClibc-0.9.33.2
ELF2FLT_DIR   := $(CURDIR)/src/elf2flt-2006q1
ZLIB_DIR      := $(CURDIR)/src/zlib-1.2.3

BINUTILS_CONFIG_OPTS   := --prefix=$(OUTPUT) --target=arm-uclinuxeabi --enable-install-libbfd --disable-nls --disable-werror \
                          --with-sysroot=$(OUTPUT)/arm-uclinuxeabi/libc --disable-shared --enable-static

LIBELF_CONFIG_OPTS     := --prefix=$(OUTPUT) --target=arm-uclinuxeabi --disable-shared --disable-nls
LIBGMP_CONFIG_OPTS     := --prefix=$(OUTPUT) --disable-shared --enable-cxx --disable-nls
LIBMPFR_CONFIG_OPTS    := --prefix=$(OUTPUT) --target=arm-uclinuxeabi --disable-shared --disable-nls --with-gmp=$(OUTPUT)
LIBMPC_CONFIG_OPTS     := --prefix=$(OUTPUT) --target=arm-uclinuxeabi --disable-shared --disable-nls \
                          --with-gmp=$(OUTPUT) --with-mpfr=$(OUTPUT)
LIBPPL_CONFIG_OPTS     := --prefix=$(OUTPUT) --target=arm-uclinuxeabi --disable-shared  --disable-nls --with-libgmp-prefix=$(OUTPUT) \
                          CPPFLAGS=-fpermissive
LIBCLOOG_CONFIG_OPTS   := --prefix=$(OUTPUT) --target=arm-uclinuxeabi --disable-shared --disable-nls \
                          --with-ppl=$(OUTPUT) --with-gmp=$(OUTPUT)
GCC_CONFIG_OPTS        := --target=arm-uclinuxeabi --enable-threads --disable-libmudflap --disable-libssp \
                          --disable-libstdcxx-pch --with-gnu-as \
                          --disable-multilib --with-cpu=cortex-m4 --with-mode=thumb --with-arch=armv7e-m --with-tune=cortex-m4 \
                          --with-gnu-ld '--with-specs=%{save-temps: -fverbose-asm}' \
                          --enable-languages=c,c++ --disable-shared --enable-lto \
                          --disable-nls --prefix=$(OUTPUT) \
                          --disable-shared --disable-libssp --disable-libgomp --without-headers \
                          --disable-decimal-float --disable-libffi --disable-libquadmath --enable-languages=c,c++ \
                          --with-sysroot=$(OUTPUT)/arm-uclinuxeabi/libc --with-build-sysroot=$(OUTPUT)/arm-uclinuxeabi/libc \
                          --enable-poison-system-directories \
                          '--with-host-libstdcxx=-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm' \
                          --disable-libgomp \
                          --with-build-time-tools=$(OUTPUT)/bin \
                          --disable-wchar_t \
                          --disable-dlopen \
                          CFLAGS="-Os -ffunction-sections -fdata-sections -Wl,--gc-sections" CXXFLAGS="-Os -ffunction-sections -fdata-sections -Wl,--gc-sections" \
                          TARGET_CFLAGS="-mtune=cortex-m4 -march=armv7e-m -mthumb -Os -ffunction-sections -fdata-sections -Wl,--gc-sections" TARGET_CXXFLAGS="-mtune=cortex-m4 -march=armv7e-m -mthumb -Os -ffunction-sections -fdata-sections -Wl,--gc-sections"

#                          --with-gmp=$(OUTPUT) \
#                          --with-mpfr=$(OUTPUT) \
#                          --with-mpc=$(OUTPUT) \
#                          --with-ppl=$(OUTPUT) \
#                          --with-cloog=$(OUTPUT) \
#                          --with-libelf=$(OUTPUT) \

ELF2FLT_CONFIG_OPTS    := --prefix=$(OUTPUT) --target=arm-uclinuxeabi \
                          --disable-nls \
                          --with-bfd-include-dir=$(OUTPUT)/`$(GCC_DIR)/config.guess`/arm-uclinuxeabi/include \
                          --with-libbfd=$(OUTPUT)/`$(GCC_DIR)/config.guess`/arm-uclinuxeabi/lib/libbfd.a \
                          --with-binutils-include-dir=$(BINUTILS_DIR)/include \
                          --with-zlib-prefix=$(OUTPUT) \
                          --with-libiberty=$(BINUTILS_DIR)/build/libiberty/libiberty.a \
                          LIBS="-ldl"

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

#patch_cloog-s:
#	cd $(LIBCLOOG_DIR); patch -p1 < $(CURDIR)/cloog-patch-01
#	touch patch_cloog-s

gcc: gcc_stage_1 elf2flt libc
	AR_FOR_TARGET=arm-uclinuxeabi-ar NM_FOR_TARGET=arm-uclinuxeabi-nm OBJDUMP_FOR_TARGET=arm-uclinuxeabi-objdump STRIP_FOR_TARGET=arm-uclinuxeabi-strip $(MAKE) -j4 -C $(GCC_DIR)/build LDFLAGS_FOR_TARGET=--sysroot=$(OUTPUT)/arm-uclinuxeabi/libc CPPFLAGS_FOR_TARGET=--sysroot=$(OUTPUT)/arm-uclinuxeabi/libc build_tooldir=$(OUTPUT)/arm-uclinuxeabi all
	$(MAKE) -C $(GCC_DIR)/build install

#libelf libgmp libmpfr libmpc libppl libcloog
gcc_stage_1: linux-headers binutils gcc_config
	AR_FOR_TARGET=arm-uclinuxeabi-ar NM_FOR_TARGET=arm-uclinuxeabi-nm OBJDUMP_FOR_TARGET=arm-uclinuxeabi-objdump STRIP_FOR_TARGET=arm-uclinuxeabi-strip $(MAKE) -j4 -C $(GCC_DIR)/build LDFLAGS_FOR_TARGET=--sysroot=$(OUTPUT)/arm-uclinuxeabi/libc CPPFLAGS_FOR_TARGET=--sysroot=$(OUTPUT)/arm-uclinuxeabi/libc build_tooldir=$(OUTPUT)/arm-uclinuxeabi all-gcc
	$(MAKE) -C $(GCC_DIR)/build install-gcc

#patch_gcc-s:
#	cd $(GCC_DIR); patch -p1 < $(CURDIR)/gcc-patch-01
#	cd $(GCC_DIR); patch -p1 < $(CURDIR)/gcc-patch-02
#	touch patch_gcc-s

libc: patch_uclibc-s
	cp $(PRODUCTDIR)/config.uClibc $(UCLIBC_DIR)/.config
	yes "" | $(MAKE) -C $(UCLIBC_DIR) oldconfig
	STAGEDIR="$(OUTPUT)/arm-uclinuxeabi/libc" CROSS="$(OUTPUT)/bin/arm-uclinuxeabi-" CFLAGS="-mtune=cortex-m4 -march=armv7e-m -mthumb -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -D__thumb2__" $(MAKE) -C $(UCLIBC_DIR) V=1
	STAGEDIR="$(OUTPUT)/arm-uclinuxeabi/libc" CROSS="$(OUTPUT)/bin/arm-uclinuxeabi-" PREFIX=$(OUTPUT)/arm-uclinuxeabi/libc $(MAKE) -C $(UCLIBC_DIR) V=1 install
	mkdir -p $(OUTPUT)/arm-uclinuxeabi/libc/usr/local; ln -f -s ../include $(OUTPUT)/arm-uclinuxeabi/libc/usr/local/include

patch_uclibc-s:
	cd $(UCLIBC_DIR); patch -p1 < $(CURDIR)/uclibc-patch-01
	touch patch_uclibc-s

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



