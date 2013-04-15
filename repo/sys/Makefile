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
		[ -d $(CONTENT)/$$i ] || mkdir -p $(CONTENT)/$$i; \
	done
	$(REPOINST) -s /tmp/run /var/run
	$(REPOINST) -s /tmp/resolv.conf /etc/resolv.conf
	$(REPOINST) -s /tmp/hosts /etc/hosts
	$(ROOTDIR)/tools/device-table.pl -d $(CONTENT) -f $(PRODUCTDIR)/dev.txt