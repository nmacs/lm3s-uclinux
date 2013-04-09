include $(REPODIR)/Rules.mk

DIRS  = bin dev etc home lib mnt proc sys usr tmp boot root var
DIRS += etc/config
DIRS += etc/rc.d
DIRS += etc/romdisk
DIRS += etc/secret
DIRS += etc/.secret
DIRS += var/tmp
DIRS += var/opkg
DIRS += var/opkg/upgrade
DIRS += var/log
DIRS += usr/lib
DIRS += usr/lib/opkg
DIRS += tmp/run

content:
	for i in $(DIRS); do \
		[ -d content/$$i ] || mkdir -p content/$$i; \
	done
	ln -s /tmp/run content/var/run
	ln -s /tmp/resolv.conf content/etc/resolv.conf
	ln -s /tmp/hosts content/etc/hosts
	$(ROOTDIR)/tools/device-table.pl -d content/ -f $(ROOTDIR)/vendors/Elster/uwic/dev.txt