include $(REPODIR)/Rules.mk

content:
	mkdir -p content/usr/local/share/lua/5.1
	mkdir -p content/bin
	mkdir -p content/etc/init.d
	mkdir -p content/etc/rc.d
	cp $(ROOTDIR)/user/elster/mgate/lua/mgate.lua content/usr/local/share/lua/5.1/mgate.lua
	cp $(ROOTDIR)/user/elster/bin/mgate/mgate content/bin/mgate
	cp $(ROOTDIR)/user/elster/mgate/etc/mgate content/etc/init.d/
	cp $(ROOTDIR)/user/elster/mgate/etc/mgate.conf content/etc/mgate.conf
	ln -s /etc/init.d/mgate content/etc/rc.d/S50mgate
