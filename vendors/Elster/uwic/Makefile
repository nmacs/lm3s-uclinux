#
#	Makefile -- Build instructions for Elster uWIC
#

include $(LINUX_CONFIG)
include $(CONFIG_CONFIG)
include $(ARCH_CONFIG)

LINUXFILE           = linux.bin
LINUXCOMPRESSEDFILE = Image.lzo
LINUXUBOOTFILE      = Image.uboot
UBIFSFILE           = Image.ubifs
EXT2FILE            = Image.ext2

LINUXBOOTDIR       = $(ROOTDIR)/$(LINUXDIR)/arch/arm/boot
LINUXCOMPRESSEDIMG = $(LINUXBOOTDIR)/$(LINUXCOMPRESSEDFILE)
LINUXUBOOTIMG      = $(LINUXBOOTDIR)/$(LINUXUBOOTFILE)
LINUXIMG           = $(IMAGEDIR)/$(LINUXFILE)
UBIFSIMG           = $(IMAGEDIR)/$(UBIFSFILE)
EXT2IMG            = $(IMAGEDIR)/$(EXT2FILE)

ROMFS_DIRS  = bin dev etc home lib mnt proc sys usr tmp boot root var
ROMFS_DIRS += etc/config
ROMFS_DIRS += etc/rc.d
ROMFS_DIRS += etc/secret
ROMFS_DIRS += etc/.secret
ROMFS_DIRS += var/tmp

clean:
	rm -rf $(ROMFSDIR)
	rm -rf $(IMAGEDIR)

romfs: linux_image
	[ -d $(ROMFSDIR) ] || mkdir -p $(ROMFSDIR)
	for i in $(ROMFS_DIRS); do \
		[ -d $(ROMFSDIR)/$$i ] || mkdir -p $(ROMFSDIR)/$$i; \
	done
	$(ROMFSINST) /etc/passwd
	$(ROMFSINST) /etc/inittab
	$(ROMFSINST) /etc/rc
	$(ROMFSINST) /etc/opkg.conf
	$(ROMFSINST) /etc/resolv.conf
	$(ROMFSINST) /etc/stunnel.conf
	$(ROMFSINST) -p 600 /etc/stunnel.pem
	$(ROMFSINST) $(LINUXUBOOTIMG) /boot/$(LINUXFILE)
ifdef CONFIG_USER_PPPD_PPPD_PPPD
	$(ROMFSINST) -d pppd-peers-gprs /etc/ppp/peers/gprs
	$(ROMFSINST) -d pppd-chat-gprs /etc/ppp/chat/gprs
endif
	$(ROMFSINST) -s /tmp/run /var/run
	$(ROMFSINST) -d config/config /etc/config/config
	$(ROMFSINST) -d init.d/upgrade /etc/init.d/upgrade
	$(ROMFSINST) -d init.d/network /etc/init.d/network
	$(ROMFSINST) -d init.d/watchdog /etc/init.d/watchdog
	$(ROMFSINST) -d init.d/leds /etc/init.d/leds
	$(ROMFSINST) -d -s /etc/init.d/watchdog /etc/rc.d/S00watchdog
	$(ROMFSINST) -d -s /etc/init.d/upgrade /etc/rc.d/S01upgrade
	$(ROMFSINST) -d -s /etc/init.d/network /etc/rc.d/S10network
	$(ROMFSINST) -d -s /etc/init.d/leds /etc/rc.d/S99leds
	$(ROMFSINST) -s /tmp/resolv.conf /etc/resolv.conf
	$(ROMFSINST) /etc/key
	$(ROMFSINST) /etc/public.der

modules.dep:
ifdef CONFIG_MODULES
	( \
		cd $(ROMFSDIR); \
		find ./lib/modules -type f -a ! -name modules.dep | \
		sed -e 's/$$/:/' -e 's/^\.//' \
	) > $(ROMFSDIR)/lib/modules/modules.dep
endif

linux_image:
	lzop -9 -f -o $(LINUXCOMPRESSEDIMG) $(LINUXBOOTDIR)/Image
#	gzip --best --force --stdout $(LINUXBOOTDIR)/Image > $(LINUXCOMPRESSEDIMG)
	mkimage -A arm -O linux -T kernel -C lzo -a 0x60008000 -e 0x60008000 -n "Linux kernel image" -d $(LINUXCOMPRESSEDIMG) $(LINUXUBOOTIMG)

image:
	[ -d $(IMAGEDIR) ] || mkdir -p $(IMAGEDIR)
	cp $(LINUXUBOOTIMG) $(LINUXIMG)
	genext2fs -b 5120 -D $(ROOTDIR)/vendors/$(CONFIG_VENDOR)/uwic/dev.txt -d $(ROMFSDIR) $(EXT2IMG)
	mkfs.ubifs --verbose --min-io-size 1 -e 65408 -c 60 -x favor_lzo --reserved=0 --devtable=$(ROOTDIR)/vendors/$(CONFIG_VENDOR)/uwic/dev.txt --root=$(ROMFSDIR) --output=$(UBIFSIMG)
	mkfs.cramfs -v $(ROMFSDIR) $(IMAGEDIR)/Image.cramfs
