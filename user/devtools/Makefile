BUILDDIR   := $(CURDIR)/build
KBUILD_SRC := $(ROOTDIR)/busybox
REPO_TARGETDIR=$(REPODIR)/devtools/content

all:
	mkdir -p $(BUILDDIR)
	cp config.devtools $(BUILDDIR)/.config
	$(MAKE) -C $(BUILDDIR) KBUILD_SRC=$(KBUILD_SRC) -f $(KBUILD_SRC)/Makefile busybox

romfs:

clean:
	$(MAKE) -C $(BUILDDIR) clean

srctree := $(KBUILD_SRC)
objtree := $(BUILDDIR)
INSTALLER=$(CURDIR)/install-romfs.sh

$(objtree)/busybox.links: $(srctree)/applets/busybox.mkll $(objtree)/include/autoconf.h $(objtree)/include/applets.h
	$(Q)-$(SHELL) $^ >$@

REPO_TARGETDIR=$(REPODIR)/devtools/content

# Before copying, remove all existing links
repo: $(INSTALLER) $(objtree)/busybox.links
	if [ -f "$$REPO_TARGETDIR/bin/busybox" ]; then \
		inode=`ls -i $$REPO_TARGETDIR/bin/busybox | awk '{print $$1}'`; \
		ls -iL "$$REPO_TARGETDIR/bin" | grep "^ *$$inode" | awk '{print $$2}' | \
		sed "s:^:$$REPO_TARGETDIR/bin:" | env -i xargs rm -f; \
	fi
	mkdir -p $(REPO_TARGETDIR)/bin
	cp $(objtree)/busybox $(REPO_TARGETDIR)/bin/devtools
	cd $(objtree) && $(SHELL) $< $(REPO_TARGETDIR)/bin/ --nosubdir