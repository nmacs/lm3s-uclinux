#
#	Makefile -- Build instructions for Elster uWIC
#

include $(LINUX_CONFIG)
include $(CONFIG_CONFIG)
include $(ARCH_CONFIG)

SIGNATURE_KEY       = $(ROOTDIR)/../u-boot-uwic/board/elster/uwic/testkey.pem

LINUXFILE           = linux.bin
LINUXCOMPRESSEDFILE = Image.lzo
LINUXUBOOTFILE      = Image.uboot
LINUXSIGFILE        = Image.uboot.sig
UBIFSFILE           = Image.ubifs
EXT2FILE            = Image.ext2

LINUXBOOTDIR       = $(ROOTDIR)/$(LINUXDIR)/arch/arm/boot
LINUXCOMPRESSEDIMG = $(LINUXBOOTDIR)/$(LINUXCOMPRESSEDFILE)
LINUXUBOOTIMG      = $(LINUXBOOTDIR)/$(LINUXUBOOTFILE)
LINUXSIG           = $(LINUXBOOTDIR)/$(LINUXSIGFILE)
LINUXIMG           = $(IMAGEDIR)/$(LINUXFILE)
UBIFSIMG           = $(IMAGEDIR)/$(UBIFSFILE)
EXT2IMG            = $(IMAGEDIR)/$(EXT2FILE)

ROMFS_DIRS  = bin dev etc home lib mnt proc sys usr tmp boot root var
ROMFS_DIRS += etc/config
ROMFS_DIRS += etc/rc.d
ROMFS_DIRS += etc/romdisk
ROMFS_DIRS += etc/secret
ROMFS_DIRS += etc/.secret
ROMFS_DIRS += var/tmp
ROMFS_DIRS += var/opkg
ROMFS_DIRS += var/opkg/upgrade
ROMFS_DIRS += var/log
ROMFS_DIRS += usr/lib
ROMFS_DIRS += usr/lib/opkg
ROMFS_DIRS += tmp/run

clean:
	rm -rf $(ROMFSDIR)
	rm -rf $(IMAGEDIR)

romfs:
	[ -d $(ROMFSDIR) ] || mkdir -p $(ROMFSDIR)
	for i in $(ROMFS_DIRS); do \
		[ -d $(ROMFSDIR)/$$i ] || mkdir -p $(ROMFSDIR)/$$i; \
	done
	$(ROOTDIR)/tools/opkg-cl -o $(ROMFSDIR) install $(REPODIR)/output/*.deb

ifdef CONFIG_USER_PPPD_PPPD_PPPD
	$(ROMFSINST) -d pppd-peers-gprs /etc/ppp/peers/gprs
	$(ROMFSINST) -d pppd-chat-gprs /etc/ppp/chat/gprs
endif

modules.dep:
ifdef CONFIG_MODULES
	( \
		cd $(ROMFSDIR); \
		find ./lib/modules -type f -a ! -name modules.dep | \
		sed -e 's/$$/:/' -e 's/^\.//' \
	) > $(ROMFSDIR)/lib/modules/modules.dep
endif

linux_image:
	[ -d $(IMAGEDIR) ] || mkdir -p $(IMAGEDIR)
	lzop -9 -f -o $(LINUXCOMPRESSEDIMG) $(LINUXBOOTDIR)/Image
#	gzip --best --force --stdout $(LINUXBOOTDIR)/Image > $(LINUXCOMPRESSEDIMG)
	mkimage -A arm -O linux -T kernel -C lzo -a 0x60008000 -e 0x60008000 -n "Linux kernel image" -d $(LINUXCOMPRESSEDIMG) $(LINUXUBOOTIMG)
	openssl dgst -sha1 -sign $(SIGNATURE_KEY) -out $(LINUXSIG) $(LINUXUBOOTIMG)
	cat $(LINUXSIG) $(LINUXUBOOTIMG) > $(LINUXIMG)

image:
	[ -d $(IMAGEDIR) ] || mkdir -p $(IMAGEDIR)
	genext2fs -b 5120 -D $(ROOTDIR)/vendors/$(CONFIG_VENDOR)/uwic/dev.txt -d $(ROMFSDIR) $(EXT2IMG)
	mkfs.ubifs --verbose --min-io-size 1 -e 65408 -c 60 -x favor_lzo --reserved=0 --root=$(ROMFSDIR) --output=$(UBIFSIMG)
#	mkfs.cramfs -v $(ROMFSDIR) $(IMAGEDIR)/Image.cramfs
