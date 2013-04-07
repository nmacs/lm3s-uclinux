BUILDDIR   := $(CURDIR)/build
KBUILD_SRC := $(ROOTDIR)/busybox

all:
	mkdir -p $(BUILDDIR)
	cp $(PRODUCTDIR)/config.busybox $(BUILDDIR)/.config
	$(MAKE) -C $(BUILDDIR) KBUILD_SRC=$(KBUILD_SRC) -f $(KBUILD_SRC)/Makefile busybox

romfs:

clean:
	$(MAKE) -C $(BUILDDIR) clean

srctree := $(KBUILD_SRC)
objtree := $(BUILDDIR)

include Makefile.custom

