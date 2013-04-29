BUILDDIR   := $(CURDIR)/build
KBUILD_SRC := $(ROOTDIR)/busybox
REPO_TARGETDIR=$(REPODIR)/syslog/content

all:
	mkdir -p $(BUILDDIR)
	cmp -s config.logging $(BUILDDIR)/.config || cp config.logging $(BUILDDIR)/.config
	$(MAKE) -C $(BUILDDIR) KBUILD_SRC=$(KBUILD_SRC) -f $(KBUILD_SRC)/Makefile busybox

romfs:
	$(ROMFSINST) -d $(BUILDDIR)/busybox /bin/syslog_bb
	$(ROMFSINST) -d -e CONFIG_USER_SYSLOG_BB_SYSLOGD -s syslog_bb /bin/syslogd
	$(ROMFSINST) -d -e CONFIG_USER_SYSLOG_BB_SYSLOGD etc/init.d/syslogd /etc/init.d/syslogd
	$(ROMFSINST) -d -e CONFIG_USER_SYSLOG_BB_SYSLOGD -s /etc/init.d/syslogd /etc/rc.d/S20syslogd
	$(ROMFSINST) -d -e CONFIG_USER_SYSLOG_BB_KLOGD -s syslog_bb /bin/klogd
	$(ROMFSINST) -d -e CONFIG_USER_SYSLOG_BB_KLOGD etc/init.d/klogd /etc/init.d/klogd
	$(ROMFSINST) -d -e CONFIG_USER_SYSLOG_BB_KLOGD -s /etc/init.d/klogd /etc/rc.d/S21klogd
	$(ROMFSINST) -d -e CONFIG_USER_SYSLOG_BB_LOGGER -s syslog_bb /bin/logger

repo:
	$(REPOINST) $(BUILDDIR)/busybox /bin/syslog_bb
	$(REPOINST) -e CONFIG_USER_SYSLOG_BB_SYSLOGD -s syslog_bb /bin/syslogd
	$(REPOINST) -e CONFIG_USER_SYSLOG_BB_SYSLOGD etc/init.d/syslogd /etc/init.d/syslogd
	$(REPOINST) -e CONFIG_USER_SYSLOG_BB_SYSLOGD -s /etc/init.d/syslogd /etc/rc.d/S20syslogd
	$(REPOINST) -e CONFIG_USER_SYSLOG_BB_KLOGD -s syslog_bb /bin/klogd
	$(REPOINST) -e CONFIG_USER_SYSLOG_BB_KLOGD etc/init.d/klogd /etc/init.d/klogd
	$(REPOINST) -e CONFIG_USER_SYSLOG_BB_KLOGD -s /etc/init.d/klogd /etc/rc.d/S21klogd
	$(REPOINST) -e CONFIG_USER_SYSLOG_BB_LOGGER -s syslog_bb /bin/logger

clean:
	-$(MAKE) -C $(BUILDDIR) clean
	-rm -f $(BUILDDIR)/.config