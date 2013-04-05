include $(REPODIR)/Rules.mk

content:
	$(MAKE) -C $(ROOTDIR)/user/busybox repo
	if [ -x content/bin/syslogd ]; then \
		mkdir -p content/bin; \
		mkdir -p content/etc/init.d; \
		mkdir -p content/etc/rc.d; \
		cp etc/syslogd content/etc/init.d/syslogd; \
		ln -s /etc/init.d/syslogd content/etc/rc.d/S20syslogd; \
	fi
	if [ -x content/bin/klogd ]; then \
		mkdir -p content/bin; \
		mkdir -p content/etc/init.d; \
		mkdir -p content/etc/rc.d; \
		cp etc/klogd content/etc/init.d/klogd; \
		ln -s /etc/init.d/klogd content/etc/rc.d/S21klogd; \
	fi
