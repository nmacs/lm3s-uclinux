include $(REPODIR)/Rules.mk

content:
	$(REPOINST) $(ROOTDIR)/user/init/init /bin/init
	$(REPOINST) $(ROOTDIR)/user/keyutils/insert-passphrase/insert-passphrase /bin/insert-passphrase
	$(REPOINST) $(ROOTDIR)/user/keyutils/keyutils-1.5.5/keyctl-static /bin/keyctl
	$(REPOINST) -p 444 etc/rc /etc/rc
	$(REPOINST) -p 444 etc/inittab /etc/inittab
	$(REPOINST) -p 544 etc/init.d/leds /etc/init.d/leds
	$(REPOINST) -p 544 etc/init.d/network /etc/init.d/network
	$(REPOINST) -p 544 etc/init.d/nfs /etc/init.d/nfs
	$(REPOINST) -s /etc/init.d/network /etc/rc.d/S10network
	$(REPOINST) -s /etc/init.d/leds /etc/rc.d/S99leds
	$(REPOINST) -p 444 etc/config/network.default /etc/config/network.default
	$(REPOINST) -p 444 etc/config/system.default /etc/config/system.default