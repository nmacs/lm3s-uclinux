include $(REPODIR)/Rules.mk

content:
	mkdir -p content/bin
	mkdir -p content/etc/init.d
	mkdir -p content/etc/rc.d
	cp $(ROOTDIR)/user/init/init content/bin/init
	cp $(ROOTDIR)/vendors/Elster/uwic/rc content/etc/rc
	cp $(ROOTDIR)/vendors/Elster/uwic/init.d/* content/etc/init.d/
	cp -d $(ROOTDIR)/vendors/Elster/uwic/rc.d/* content/etc/rc.d/
	cp $(ROOTDIR)/vendors/Elster/uwic/inittab content/etc/inittab