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

LINUXBOOTDIR       = $(ROOTDIR)/$(LINUXDIR)/arch/arm/boot
LINUXCOMPRESSEDIMG = $(LINUXBOOTDIR)/$(LINUXCOMPRESSEDFILE)
LINUXUBOOTIMG      = $(LINUXBOOTDIR)/$(LINUXUBOOTFILE)
LINUXIMG           = $(IMAGEDIR)/$(LINUXFILE)
UBIFSIMG           = $(IMAGEDIR)/$(UBIFSFILE)


ROMFS_DIRS = bin dev etc home lib mnt proc sys usr var tmp boot root

clean:
	rm -rf $(ROMFSDIR)
	rm -rf $(IMAGEDIR)

romfs: linux_image
	[ -d $(ROMFSDIR) ] || mkdir -p $(ROMFSDIR)
	for i in $(ROMFS_DIRS); do \
		[ -d $(ROMFSDIR)/$$i ] || mkdir -p $(ROMFSDIR)/$$i; \
	done
ifdef CONFIG_PPP
	[ -d $(ROMFSDIR)/dev/pts ] || mkdir -p $(ROMFSDIR)/dev/pts
endif
	$(ROMFSINST) /etc/passwd
	$(ROMFSINST) /etc/inittab
	$(ROMFSINST) /etc/opkg.conf
	$(ROMFSINST) /etc/resolv.conf
	$(ROMFSINST) $(LINUXUBOOTIMG) /boot/$(LINUXFILE)

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

image: romfs
	[ -d $(IMAGEDIR) ] || mkdir -p $(IMAGEDIR)
	cp $(LINUXUBOOTIMG) $(LINUXIMG)
	mkfs.ubifs --verbose --min-io-size 1 -e 65408 -c 60 -x favor_lzo --reserved=0 --devtable=$(ROOTDIR)/vendors/$(CONFIG_VENDOR)/uwic/dev.txt --root=$(ROMFSDIR) --output=$(UBIFSIMG)