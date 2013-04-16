EXEC = opkg-cl
OPKG = $(CURDIR)/opkg-649

CONFIGURE_OPTIONS += --disable-curl
CONFIGURE_OPTIONS += --disable-gpg
CONFIGURE_OPTIONS += --disable-sha256
CONFIGURE_OPTIONS += --disable-largefile
CONFIGURE_OPTIONS += --disable-fast-install
CONFIGURE_OPTIONS += --disable-shave
CONFIGURE_OPTIONS += --with-opkglockfile=/tmp/opkg-lock

OPKG_CFLAGS  = -Wl,-elf2flt="-s32768"

ifdef HOST_ARCH
	ifeq "$(HOST_ARCH)" "i386"
		CFLAGS = -m32
		CC = gcc
		AR = ar
		LD = ld
		STRIP = strip
		RANLIB = ranlib
		CONFIGURE_OPTIONS   += --host=$(HOST_ARCH)
	endif
	ifeq "$(HOST_ARCH)" "amd64"
		CFLAGS = -m64
		CC = gcc
		AR = ar
		LD = ld
		STRIP = strip
		RANLIB = ranlib
		CONFIGURE_OPTIONS   += --host=$(HOST_ARCH)
	endif
else
	HOST_ARCH   := arm-uclinux
	OPKG_CFLAGS += -DHAVE_ATOMIC_EXTRACT
	OPKG_CFLAGS += -DHAVE_FIRMWARE_UPGRADE
	OPKG_CFLAGS += -DHAVE_BUILTIN_CONFIG
	CONFIGURE_OPTIONS   += --host=$(HOST_ARCH)
endif

BUILD_DIR           := $(CURDIR)/build-$(HOST_ARCH)

all: $(EXEC)

$(EXEC): $(BUILD_DIR)/Makefile
	$(MAKE) CFLAGS='$(OPKG_CFLAGS)' -C $(BUILD_DIR)

$(BUILD_DIR)/Makefile: $(OPKG)/configure Makefile
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && $(OPKG)/configure $(CONFIGURE_OPTIONS)

$(OPKG)/configure:
	cd $(OPKG) && ./autogen.sh

romfs:
	$(ROMFSINST) -d $(BUILD_DIR)/src/$(EXEC) /bin/$(EXEC)

repo:
	$(REPOINST) $(BUILD_DIR)/src/$(EXEC) /bin/$(EXEC)

clean:
	rm -f $(OPKG)/configure
	rm -rf build*

