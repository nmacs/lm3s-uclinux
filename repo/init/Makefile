include $(REPODIR)/Rules.mk

content:
	mkdir -p content/bin
	mkdir -p content/etc/init.d
	mkdir -p content/etc/rc.d
	mkdir -p content/etc/config
	cp $(ROOTDIR)/user/init/init content/bin/init
	cp $(ROOTDIR)/user/keyutils/insert-passphrase/insert-passphrase content/bin/insert-passphrase
	cp $(ROOTDIR)/user/keyutils/keyutils-1.5.5/keyctl-static content/bin/keyctl
	cp etc/rc content/etc/rc
	chmod 444 content/etc/rc
	cp etc/init.d/* content/etc/init.d/
	chmod 544 content/etc/init.d/*
	cp -d etc/rc.d/* content/etc/rc.d/
	cp etc/inittab content/etc/inittab
	chmod 444 content/etc/inittab
	cp -d etc/config/* content/etc/config/
	chmod 444 content/etc/config/*