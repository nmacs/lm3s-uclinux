romfs:
	$(ROMFSINST) -d $(ROOTDIR)/user/init/init /bin/init
	$(MAKE)  -C $(ROOTDIR)/user/keyutils romfs
	$(ROMFSINST) -d -p 444 etc/rc                     /etc/rc
	$(ROMFSINST) -d -p 444 etc/inittab                /etc/inittab
	$(ROMFSINST) -d -p 544 etc/init.d/leds            /etc/init.d/leds
	$(ROMFSINST) -d -p 544 etc/init.d/network         /etc/init.d/network
	$(ROMFSINST) -d -p 544 etc/init.d/nfs             /etc/init.d/nfs
	$(ROMFSINST) -d -s /etc/init.d/network            /etc/rc.d/S10network
	$(ROMFSINST) -d -s /etc/init.d/leds               /etc/rc.d/S99leds
	$(ROMFSINST) -d -p 444 etc/default_config/network /etc/default_config/network
	$(ROMFSINST) -d -p 444 etc/default_config/system  /etc/default_config/system

repo:
	$(REPOINST) $(ROOTDIR)/user/init/init /bin/init
	$(MAKE)  -C $(ROOTDIR)/user/keyutils repo
	$(REPOINST) -p 444 etc/rc                     /etc/rc
	$(REPOINST) -p 444 etc/inittab                /etc/inittab
	$(REPOINST) -p 544 etc/init.d/leds            /etc/init.d/leds
	$(REPOINST) -p 544 etc/init.d/network         /etc/init.d/network
	$(REPOINST) -p 544 etc/init.d/nfs             /etc/init.d/nfs
	$(REPOINST) -s /etc/init.d/network            /etc/rc.d/S10network
	$(REPOINST) -s /etc/init.d/leds               /etc/rc.d/S99leds
	$(REPOINST) -p 444 etc/default_config/network /etc/default_config/network
	$(REPOINST) -p 444 etc/default_config/system  /etc/default_config/system