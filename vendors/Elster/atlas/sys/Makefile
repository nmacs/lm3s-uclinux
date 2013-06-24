DIRS  = bin dev etc home lib mnt proc sys usr tmp boot root var
DIRS += etc/config
DIRS += etc/default_config
DIRS += etc/rc.d
DIRS += etc/romdisk
DIRS += etc/ppp
DIRS += usr/lib
DIRS += usr/lib/opkg
DIRS += tmp/run

romfs:
	for i in $(DIRS); do \
		[ -d $(ROMFSDIR)/$$i ] || mkdir -p $(ROMFSDIR)/$$i; \
	done
	$(ROMFSINST) -s /tmp/run /var/run
	$(ROMFSINST) -s /media/flash/var/log /var/log
	$(ROMFSINST) -s /tmp/resolv.conf /etc/resolv.conf
	$(ROMFSINST) -s /tmp/hosts /etc/hosts
	$(ROMFSINST) -s /tmp/ppp0.resolv /etc/ppp/ppp0.resolv
	device-table.pl -d $(ROMFSDIR) -f $(PRODUCTDIR)/dev.txt

repo:
	for i in $(DIRS); do \
		[ -d $(CONTENT)/$$i ] || mkdir -p $(CONTENT)/$$i; \
	done
	$(REPOINST) -s /tmp/run /var/run
	$(REPOINST) -s /media/flash/var/log /var/log
	$(REPOINST) -s /tmp/resolv.conf /etc/resolv.conf
	$(REPOINST) -s /tmp/hosts /etc/hosts
	$(REPOINST) -s /tmp/ppp0.resolv /etc/ppp/ppp0.resolv
	device-table.pl -d $(CONTENT) -f $(PRODUCTDIR)/dev.txt