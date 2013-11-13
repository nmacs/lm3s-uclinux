BUILDDIR   := $(CURDIR)/build
KBUILD_SRC := $(ROOTDIR)/busybox

ifdef HOSTBUILD
AR := ar
CFLAGS += -g
STRIP := strip
endif

all:
	mkdir -p $(BUILDDIR)
	cmp -s $(PRODUCTDIR)/config.busybox $(BUILDDIR)/.config || cp $(PRODUCTDIR)/config.busybox $(BUILDDIR)/.config
ifndef HOSTBUILD
	$(MAKE) -C $(BUILDDIR) KBUILD_SRC=$(KBUILD_SRC) -f $(KBUILD_SRC)/Makefile busybox
else
	$(MAKE) -C $(BUILDDIR) CFLAGS="$(CFLAGS)" AR="$(AR)" STRIP="$(STRIP)" KBUILD_SRC=$(KBUILD_SRC) -f $(KBUILD_SRC)/Makefile busybox
endif

romfs:

clean:
	-$(MAKE) -C $(BUILDDIR) clean
	-rm -f $(BUILDDIR)/.config

srctree := $(KBUILD_SRC)
objtree := $(BUILDDIR)

include Makefile.custom

