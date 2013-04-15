include $(REPODIR)/Rules.mk

content:
	$(REPOINST) $(ROOTDIR)/user/watchdogd/watchdogd /bin/watchdogd
	$(REPOINST) etc/init.d/watchdog /etc/init.d/watchdog
	$(REPOINST) -s /etc/init.d/watchdog /etc/rc.d/S00watchdog