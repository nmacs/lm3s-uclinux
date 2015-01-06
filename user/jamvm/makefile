PREFIX=
DESTDIR=$(shell pwd)/install
CLASSPATH=$(CURDIR)/../../lib/classpath/install
CONFOPTS = --prefix=$(PREFIX) --with-classpath-install-dir=$(CLASSPATH) --disable-shared --enable-ffi
CFLAGS += -DDEFAULT_MAX_HEAP=4*MB 

LIBS = -ljavanio -ljavaio -ljavalang -ljavalangmanagement -ljavalangreflect -ljavanet -ljavautil
LIBDIRS = -L$(CURDIR)/../../lib/classpath/install/lib/classpath

CFLAGS += $(LIBS) $(LIBDIRS)

CFLAGS += -Wl,-elf2flt=-s131072

all: build/build
	$(MAKE) -C build
	$(MAKE) -C build/src install-exec DESTDIR=$(DESTDIR)
	$(MAKE) -C build/lib install-data DESTDIR=$(DESTDIR)

build/build: makefile
	rm -rf build install
	mkdir build
	(cd build; sh ../configure $(CONFIGURE_OPTS) $(CONFOPTS) CFLAGS="$(CFLAGS)")
	touch build/build

clean:
	rm -rf build install

romfs:
	$(ROMFSINST) -f $(DESTDIR)/bin /bin
	$(ROMFSINST) -f $(DESTDIR)/share /share
