#
#	Makefile -- Build instructions for Elster uWIC
#

include $(LINUX_CONFIG)
include $(CONFIG_CONFIG)
include $(ARCH_CONFIG)


LINUXFILE           = linux.bin
LINUXCOMPRESSEDFILE = Image.lzo
LINUXUBOOTFILE      = Image.uboot
LINUXSIGFILE        = Image.uboot.sig
UBIFSFILE           = Image.ubifs
EXT2FILE            = Image.ext2

LINUXBOOTDIR        = $(ROOTDIR)/$(LINUXDIR)/arch/arm/boot
LINUXCOMPRESSEDIMG  = $(LINUXBOOTDIR)/$(LINUXCOMPRESSEDFILE)
LINUXUBOOTIMG       = $(LINUXBOOTDIR)/$(LINUXUBOOTFILE)
LINUXSIG            = $(LINUXBOOTDIR)/$(LINUXSIGFILE)
LINUXIMG            = $(IMAGEDIR)/$(LINUXFILE)
UBIFSIMG            = $(IMAGEDIR)/$(UBIFSFILE)
EXT2IMG             = $(IMAGEDIR)/$(EXT2FILE)

KEY                ?= $(CURDIR)/security/key
PRIV_KEY           ?= $(CURDIR)/security/priv_key
PUB_KEY            ?= $(CURDIR)/security/pub_key

UBIFS      := mkfs.ubifs --verbose --min-io-size 1 -e 65408 -c 60 -x favor_lzo --reserved=0

clean:
	rm -rf $(ROMFSDIR)
	rm -rf $(IMAGEDIR)

romfs:
	[ -d $(ROMFSDIR) ] || mkdir -p $(ROMFSDIR)
	$(MAKE) -C sys romfs
	$(MAKE) -C init romfs
	$(ROMFSINST) -d $(LINUXIMG) /boot/linux.bin

romdisk:
	$(ROMFSINST) -r $(ROMDISK) $(KEY)     /key
	$(ROMFSINST) -r $(ROMDISK) $(PUB_KEY) /pub_key

modules.dep:
ifdef CONFIG_MODULES
	( \
		cd $(ROMFSDIR); \
		find ./lib/modules -type f -a ! -name modules.dep | \
		sed -e 's/$$/:/' -e 's/^\.//' \
	) > $(ROMFSDIR)/lib/modules/modules.dep
endif

linux_image: $(LINUXIMG)

$(LINUXIMG): $(LINUXBOOTDIR)/Image
	[ -d $(IMAGEDIR) ] || mkdir -p $(IMAGEDIR)
	cat $(LINUXBOOTDIR)/Image | lzop -9 -f -o $(LINUXCOMPRESSEDIMG) -
#	gzip --best --force --stdout $(LINUXBOOTDIR)/Image > $(LINUXCOMPRESSEDIMG)
	mkimage -A arm -O linux -T kernel -C lzo -a 0x60008000 -e 0x60008000 -n "Linux kernel image" -d $(LINUXCOMPRESSEDIMG) $(LINUXUBOOTIMG)
	openssl dgst -sha1 -sign $(PRIV_KEY) -out $(LINUXSIG) $(LINUXUBOOTIMG)
	cat $(LINUXSIG) $(LINUXUBOOTIMG) > $(LINUXIMG)

image:
	[ -d $(IMAGEDIR) ] || mkdir -p $(IMAGEDIR)
	genext2fs -b 5120 -D $(PRODUCTDIR)/dev.txt -d $(ROMFSDIR) $(EXT2IMG)
#	mkfs.cramfs -v $(ROMFSDIR) $(IMAGEDIR)/Image.cramfs
	$(UBIFS) -r $(ROMFSDIR) -o $(UBIFSIMG)