EXEC = socat
SOCAT_SRC = socat-1.7.2.1

all: build
	$(MAKE) -C build

.phony: build
build: Makefile
	mkdir -p build
	(cd build; CC="$(CC) $(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LDLIBS)" ../$(SOCAT_SRC)/configure --host=$(CROSS_COMPILE:-=))

romfs:
#        $(ROMFSINST) build/src/grep /bin/grep
#        $(ROMFSINST) -s /bin/grep /bin/egrep
#        $(ROMFSINST) -s /bin/grep /bin/fgrep

clean:
	rm -rf build