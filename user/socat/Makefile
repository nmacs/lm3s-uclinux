EXEC = socat
SOCAT_SRC = socat-1.7.2.1

CONFIGURE_OPTIONS  = --disable-udp
CONFIGURE_OPTIONS += --disable-proxy
CONFIGURE_OPTIONS += --disable-openssl
CONFIGURE_OPTIONS += --disable-tun
CONFIGURE_OPTIONS += --enable-fips
#CONFIGURE_OPTIONS += --disable-sycls
CONFIGURE_OPTIONS += --disable-gopen
CONFIGURE_OPTIONS += --disable-pipe
CONFIGURE_OPTIONS += --disable-unix
CONFIGURE_OPTIONS += --disable-abstract-unixsocket
CONFIGURE_OPTIONS += --disable-socks4
CONFIGURE_OPTIONS += --disable-socks4a
#CONFIGURE_OPTIONS += --disable-exec
CONFIGURE_OPTIONS += --disable-system
CONFIGURE_OPTIONS += --disable-ext2
CONFIGURE_OPTIONS += --disable-libwrap
CONFIGURE_OPTIONS += --disable-listen
CONFIGURE_OPTIONS += --disable-interface
CONFIGURE_OPTIONS += --disable-rawip

all: build
	$(MAKE) -C build progs

.phony: build
build: Makefile
	mkdir -p build
	(cd build; CC="$(CC) $(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LDLIBS)" ../$(SOCAT_SRC)/configure --host=$(CROSS_COMPILE:-=) $(CONFIGURE_OPTIONS))

romfs:
	$(ROMFSINST) build/$(EXEC) /bin/$(EXEC)

clean:
	rm -rf build