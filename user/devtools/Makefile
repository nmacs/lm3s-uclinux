BUILDDIR   := $(CURDIR)/build
KBUILD_SRC := $(ROOTDIR)/busybox

all:
	mkdir -p $(BUILDDIR)
	cp config.devtools $(BUILDDIR)/.config
	$(MAKE) -C $(BUILDDIR) KBUILD_SRC=$(KBUILD_SRC) -f $(KBUILD_SRC)/Makefile busybox

clean:
	$(MAKE) -C $(BUILDDIR) clean

srctree := $(KBUILD_SRC)
objtree := $(BUILDDIR)
INSTALLER=$(CURDIR)/install-romfs.sh

$(objtree)/busybox.links: $(srctree)/applets/busybox.mkll $(objtree)/include/autoconf.h $(objtree)/include/applets.h
	$(Q)-$(SHELL) $^ >$@

ifdef CONFIG_UCLINUX
TARGETDIR=$(ROMFSDIR)/bin/ --nosubdir
else
TARGETDIR=$(ROMFSDIR)/
endif

# Before copying, remove all existing busybox links
romfs: $(INSTALLER) $(objtree)/busybox.links
	if [ -f "$$ROMFSDIR/bin/busybox" ]; then \
		inode=`ls -i $$ROMFSDIR/bin/busybox | awk '{print $$1}'`; \
		ls -iL "$$ROMFSDIR/bin" | grep "^ *$$inode" | awk '{print $$2}' | \
		sed "s:^:$$ROMFSDIR/bin/:" | env -i xargs rm -f; \
	fi
	$(ROMFSINST) -d $(objtree)/busybox /bin/devtools
	cd $(objtree) && $(SHELL) $< $(TARGETDIR)

# Before copying, remove all existing busybox links
repo: $(INSTALLER) $(objtree)/busybox.links
	if [ -f "$$CONTENT/bin/busybox" ]; then \
		inode=`ls -i $$CONTENT/bin/busybox | awk '{print $$1}'`; \
		ls -iL "$$CONTENT/bin" | grep "^ *$$inode" | awk '{print $$2}' | \
		sed "s:^:$$CONTENT/bin:" | env -i xargs rm -f; \
	fi
	$(REPOINST) $(objtree)/busybox /bin/devtools
	cd $(objtree) && $(SHELL) $< $(CONTENT)/bin/ --nosubdir