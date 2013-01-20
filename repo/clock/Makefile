include $(REPODIR)/Rules.mk

content:
	mkdir -p content/bin
	mkdir -p content/etc/init.d
	mkdir -p content/etc/rc.d
	cp $(ROOTDIR)/user/htpdate/htpdate content/bin/htpdate
	cp clock content/etc/init.d/
	ln -s /etc/init.d/clock content/etc/rc.d/S11clock