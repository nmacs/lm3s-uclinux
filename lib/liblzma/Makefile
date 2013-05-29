BUILDDIR := $(CURDIR)/build
STAGEDIR := $(CURDIR)/staging
XZDIR    := $(CURDIR)/xz-5.0.4
CONFIGURE := $(XZDIR)/configure

CONFIGURE_OPTIONS := --disable-largefile --disable-rpath --disable-nls --enable-static --disable-shared \
                     --disable-scripts --disable-lzma-links --disable-lzmainfo --disable-lzmadec --disable-xzdec \
                     --disable-xz --disable-threads --enable-small \
                     --host=arm \
                     --prefix=$(STAGEDIR)

.PHONY: all
all: liblzma

.PHONY: liblzma
liblzma: $(BUILDDIR)/Makefile
	$(MAKE) -C $(BUILDDIR)
	$(MAKE) -C $(BUILDDIR) install

$(BUILDDIR)/Makefile:
	mkdir -p $(BUILDDIR); cd $(BUILDDIR); $(CONFIGURE) $(CONFIGURE_OPTIONS)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR) $(STAGEDIR)

.PHONY: romfs
romfs:
