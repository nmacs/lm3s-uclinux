EXEC = opkg-cl
OPKG = opkg-0.1.8

CONFIGURE_OPTIONS += --host=arm-uclinux
CONFIGURE_OPTIONS += --disable-curl
CONFIGURE_OPTIONS += --disable-gpg
CONFIGURE_OPTIONS += --disable-sha256
CONFIGURE_OPTIONS += --disable-largefile
CONFIGURE_OPTIONS += --disable-fast-install

all: $(EXEC)

$(EXEC): configure
	$(MAKE) -C $(OPKG)

.phony: configure
configure: $(OPKG)/Makefile

$(OPKG)/Makefile : Makefile
	cd $(OPKG) && ./configure $(CONFIGURE_OPTIONS)

romfs:
	$(ROMFSINST) $(OPKG)/src/$(EXEC) /bin/$(EXEC)

clean:
	$(MAKE) -C $(OPKG) clean

