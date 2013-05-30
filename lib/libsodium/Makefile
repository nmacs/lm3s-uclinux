BUILDDIR  := $(CURDIR)/build
STAGEDIR  := $(CURDIR)/staging
SRCDIR    := $(CURDIR)/libsodium-0.4.1
CONFIGURE := $(SRCDIR)/configure

CONFIGURE_OPTIONS := --disable-ssp --enable-static --disable-shared \
                     --host=arm --target=arm \
                     --prefix=$(STAGEDIR)

.PHONY: all
all: libsodium

.PHONY: libsodium
libsodium: $(BUILDDIR)/Makefile
	$(MAKE) V=1 -C $(BUILDDIR)
	$(MAKE) -C $(BUILDDIR) install

$(BUILDDIR)/Makefile:
	mkdir -p $(BUILDDIR); cd $(BUILDDIR); $(CONFIGURE) V=1 CC="$(CC)" CFLAGS="$(CFLAGS) -std=c99" CPPFLAGS="$(CFLAGS) -std=c99" $(CONFIGURE_OPTIONS)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR) $(STAGEDIR)

.PHONY: romfs
romfs:
