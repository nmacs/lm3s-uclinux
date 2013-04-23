KEYUTILS_SRC = keyutils-1.5.5
INSERT_PASSPHRASE_SRC = insert-passphrase

ifdef HOST_ARCH
	ifeq "$(HOST_ARCH)" "i386"
		CFLAGS = -m32
		CC = gcc
		AR = ar
		LD = ld
		STRIP = strip
		RANLIB = ranlib
		LDFLAGS=
	endif
	ifeq "$(HOST_ARCH)" "amd64"
		CFLAGS = -m64
		CC = gcc
		AR = ar
		LD = ld
		STRIP = strip
		RANLIB = ranlib
		LDFLAGS=
	endif
else
	HOST_ARCH   := arm-uclinux
	OPKG_CFLAGS += -DHAVE_ATOMIC_EXTRACT
	OPKG_CFLAGS += -DHAVE_FIRMWARE_UPGRADE
	OPKG_CFLAGS += -DHAVE_BUILTIN_CONFIG
	CONFIGURE_OPTIONS   += --host=$(HOST_ARCH)
endif

all:
ifdef CONFIG_USER_KEYUTILS_KEYCTL
	$(MAKE) -C $(KEYUTILS_SRC) BUILDFOR=arm keyctl-static
endif
ifdef CONFIG_USER_KEYUTILS_INSERT_PASSPHRASE
	mkdir -p $(INSERT_PASSPHRASE_SRC)/$(HOST_ARCH)
	$(MAKE) -C $(INSERT_PASSPHRASE_SRC) BUILD_DIR=$(HOST_ARCH)
endif

romfs:
ifdef CONFIG_USER_KEYUTILS_KEYCTL
	$(ROMFSINST) -d $(KEYUTILS_SRC)/keyctl-static /bin/keyctl
endif
ifdef CONFIG_USER_KEYUTILS_INSERT_PASSPHRASE
	$(ROMFSINST) -d $(INSERT_PASSPHRASE_SRC)/$(HOST_ARCH)/insert-passphrase /bin/insert-passphrase
endif

repo:
ifdef CONFIG_USER_KEYUTILS_KEYCTL
	$(REPOINST) $(KEYUTILS_SRC)/keyctl-static /bin/keyctl
endif
ifdef CONFIG_USER_KEYUTILS_INSERT_PASSPHRASE
	$(REPOINST) $(INSERT_PASSPHRASE_SRC)/$(HOST_ARCH)/insert-passphrase /bin/insert-passphrase
endif

clean:
	-$(MAKE) -C $(KEYUTILS_SRC) BUILD_DIR=$(HOST_ARCH) clean
	-$(MAKE) -C $(INSERT_PASSPHRASE_SRC) BUILD_DIR=$(HOST_ARCH) clean