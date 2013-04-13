include $(REPODIR)/Rules.mk

CONTENT := $(CURDIR)/.content

content:
	mkdir -p $(CONTENT)/bin
	mkdir -p $(CONTENT)/etc/init.d
	mkdir -p $(CONTENT)/etc/rc.d
	mkdir -p $(CONTENT)/etc/config
	cp $(ROOTDIR)/user/init/init $(CONTENT)/bin/init
	cp $(ROOTDIR)/user/keyutils/insert-passphrase/insert-passphrase $(CONTENT)/bin/insert-passphrase
	cp $(ROOTDIR)/user/keyutils/keyutils-1.5.5/keyctl-static $(CONTENT)/bin/keyctl
	cp etc/rc $(CONTENT)/etc/rc
	chmod 444 $(CONTENT)/etc/rc
	cp etc/init.d/* $(CONTENT)/etc/init.d/
	chmod 544 $(CONTENT)/etc/init.d/*
	cp -d etc/rc.d/* $(CONTENT)/etc/rc.d/
	cp etc/inittab $(CONTENT)/etc/inittab
	chmod 444 $(CONTENT)/etc/inittab
	cp -d etc/config/* $(CONTENT)/etc/config/
	chmod 444 $(CONTENT)/etc/config/*