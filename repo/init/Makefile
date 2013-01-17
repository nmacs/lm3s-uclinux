include $(REPODIR)/Rules.mk

content:
	mkdir -p content/bin
	mkdir -p content/etc/init.d
	mkdir -p content/etc/rc.d
	cp $(ROMFSDIR)/bin/init content/bin/init
	cp $(ROMFSDIR)/etc/rc content/etc/rc
	cp $(ROMFSDIR)/etc/init.d/* content/etc/init.d/
	cp -d $(ROMFSDIR)/etc/rc.d/* content/etc/rc.d/
	cp $(ROMFSDIR)/etc/inittab content/etc/inittab
