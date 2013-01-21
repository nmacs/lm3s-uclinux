include $(REPODIR)/Rules.mk

content:
	mkdir -p content/bin
	mkdir -p content/etc/init.d
	mkdir -p content/etc/rc.d
	cp $(ROOTDIR)/user/watchdogd/watchdogd content/bin/watchdogd
	cp watchdog content/etc/init.d/watchdog
	ln -s /etc/init.d/watchdog content/etc/rc.d/S00watchdog