BUILDDIR   := $(CURDIR)/build
KBUILD_SRC := $(ROOTDIR)/busybox
REPO_TARGETDIR=$(REPODIR)/syslog/content

all:
	mkdir -p $(BUILDDIR)
	cp config.logging $(BUILDDIR)/.config
	$(MAKE) -C $(BUILDDIR) KBUILD_SRC=$(KBUILD_SRC) -f $(KBUILD_SRC)/Makefile busybox

romfs:

repo:
	mkdir -p $(REPO_TARGETDIR)
	mkdir -p $(REPO_TARGETDIR)/bin
	mkdir -p $(REPO_TARGETDIR)/etc/init.d
	mkdir -p $(REPO_TARGETDIR)/etc/rc.d
	cp $(BUILDDIR)/busybox    $(REPO_TARGETDIR)/bin/syslog_bb
ifdef CONFIG_USER_SYSLOG_BB_SYSLOGD
	ln -s syslog_bb           $(REPO_TARGETDIR)/bin/syslogd
	cp etc/syslogd            $(REPO_TARGETDIR)/etc/init.d/syslogd
	ln -s /etc/init.d/syslogd $(REPO_TARGETDIR)/etc/rc.d/S20syslogd
endif
ifdef CONFIG_USER_SYSLOG_BB_KLOGD
	ln -s syslog_bb           $(REPO_TARGETDIR)/bin/klogd
	cp etc/klogd              $(REPO_TARGETDIR)/etc/init.d/klogd
	ln -s /etc/init.d/klogd   $(REPO_TARGETDIR)/etc/rc.d/S21klogd
endif
ifdef CONFIG_USER_SYSLOG_BB_LOGGER
	ln -s syslog_bb           $(REPO_TARGETDIR)/bin/logger
endif

clean:
	$(MAKE) -C $(BUILDDIR) clean