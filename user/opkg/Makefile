EXEC = opkg-cl
OPKG = opkg-649

CONFIGURE_OPTIONS += --host=arm-uclinux
CONFIGURE_OPTIONS += --disable-curl
CONFIGURE_OPTIONS += --disable-gpg
CONFIGURE_OPTIONS += --disable-sha256
CONFIGURE_OPTIONS += --disable-largefile
CONFIGURE_OPTIONS += --disable-fast-install
CONFIGURE_OPTIONS += --with-opkglockfile=/tmp/opkg-lock

OPKG_CFLAGS  = -Wl,-elf2flt="-s32768"
OPKG_CFLAGS += -DHAVE_ATOMIC_EXTRACT
OPKG_CFLAGS += -DHAVE_FIRMWARE_UPGRADE
OPKG_CFLAGS += -DHAVE_BUILTIN_CONFIG

all: $(EXEC)

$(EXEC): configure
	$(MAKE) CFLAGS='$(OPKG_CFLAGS)' -C $(OPKG)

.phony: configure
configure: $(OPKG)/Makefile

$(OPKG)/Makefile : Makefile
	cd $(OPKG) && ./configure $(CONFIGURE_OPTIONS)

romfs:
	$(ROMFSINST) $(OPKG)/src/$(EXEC) /bin/$(EXEC)

clean:
	$(MAKE) -C $(OPKG) clean

