include $(REPODIR)/Rules.mk

content:
	mkdir -p content/var
	mkdir -p content/etc
	ln -s /tmp/run content/var/run
	ln -s /tmp/resolv.conf content/etc/resolv.conf
	ln -s /tmp/hosts content/etc/hosts
	$(ROOTDIR)/tools/device-table.pl -d content/ -f $(ROOTDIR)/vendors/Elster/uwic/dev.txt