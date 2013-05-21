#
# For stand alone build include architecture specific build rules.
#

ifndef UCLINUX_BUILD_USER
.EXPORT_ALL_VARIABLES:
include ../user.mak
endif

BUILDDIR   := $(CURDIR)/build
KBUILD_SRC := $(ROOTDIR)/busybox

all:
	mkdir -p $(BUILDDIR)
	cmp -s $(PRODUCTDIR)/config.initramfs $(BUILDDIR)/.config || cp $(PRODUCTDIR)/config.initramfs $(BUILDDIR)/.config
	$(MAKE) -C $(BUILDDIR) KBUILD_SRC=$(KBUILD_SRC) -f $(KBUILD_SRC)/Makefile busybox

clean:
	-$(MAKE) -C $(BUILDDIR) clean
	-rm -f $(BUILDDIR)/.config

