include $(REPODIR)/Rules.mk

content:
	mkdir -p content/usr/local/share/lua/5.1/mgate
	mkdir -p content/bin
	mkdir -p content/www
	mkdir -p content/www/css
	mkdir -p content/www/lib
	mkdir -p content/www/img
	mkdir -p content/etc/config
	mkdir -p content/etc/init.d
	mkdir -p content/etc/rc.d
	cp $(ROOTDIR)/user/elster/mgate/www/index.html                        content/www/index.html
	cp $(ROOTDIR)/user/elster/mgate/www/css/jgauge.css                    content/www/css/jgauge.css
	cp $(ROOTDIR)/user/elster/mgate/www/lib/jQueryRotateCompressed.2.2.js content/www/lib/jQueryRotateCompressed.2.2.js
	cp $(ROOTDIR)/user/elster/mgate/www/lib/jgauge-0.3.0.a3.min.js        content/www/lib/jgauge-0.3.0.a3.min.js
	cp $(ROOTDIR)/user/elster/mgate/www/img/jgauge_face_default.png       content/www/img/jgauge_face_default.png
	cp $(ROOTDIR)/user/elster/mgate/www/img/jgauge_needle_default.png     content/www/img/jgauge_needle_default.png
	cp $(ROOTDIR)/user/elster/mgate/lua/mgate.lua         content/usr/local/share/lua/5.1/mgate.lua
	cp $(ROOTDIR)/user/elster/mgate/lua/firmware.lua      content/usr/local/share/lua/5.1/mgate/firmware.lua
	cp $(ROOTDIR)/user/elster/mgate/lua/wan.lua           content/usr/local/share/lua/5.1/mgate/wan.lua
	cp $(ROOTDIR)/user/elster/mgate/lua/meter.lua         content/usr/local/share/lua/5.1/mgate/meter.lua
	cp $(ROOTDIR)/user/elster/mgate/lua/uwic.lua          content/usr/local/share/lua/5.1/mgate/uwic.lua
	cp $(ROOTDIR)/user/elster/eiwebplus/lua/eiwebplus.lua content/usr/local/share/lua/5.1/eiwebplus.lua
	cp $(ROOTDIR)/user/elster/bin/mgate/mgate             content/bin/mgate
	cp $(ROOTDIR)/user/elster/mgate/etc/config/*          content/etc/config/
	chmod 444                                             content/etc/config/*
	cp $(ROOTDIR)/user/elster/mgate/etc/mgate             content/etc/init.d/mgate
	ln -s /etc/init.d/mgate                               content/etc/rc.d/S50mgate
