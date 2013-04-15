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
UBIFSDEVFILE        = Image-dev.ubifs
UBIFSFULLFILE       = Image-full.ubifs
EXT2FILE            = Image.ext2

LINUXBOOTDIR       = $(ROOTDIR)/$(LINUXDIR)/arch/arm/boot
LINUXCOMPRESSEDIMG = $(LINUXBOOTDIR)/$(LINUXCOMPRESSEDFILE)
LINUXUBOOTIMG      = $(LINUXBOOTDIR)/$(LINUXUBOOTFILE)
LINUXSIG           = $(LINUXBOOTDIR)/$(LINUXSIGFILE)
LINUXIMG           = $(IMAGEDIR)/$(LINUXFILE)
UBIFSIMG           = $(IMAGEDIR)/$(UBIFSFILE)
UBIFSDEVIMG        = $(IMAGEDIR)/$(UBIFSDEVFILE)
UBIFSFULLIMG       = $(IMAGEDIR)/$(UBIFSFULLFILE)
EXT2IMG            = $(IMAGEDIR)/$(EXT2FILE)

UBIFS      := mkfs.ubifs --verbose --min-io-size 1 -e 65408 -c 60 -x favor_lzo --reserved=0

clean:
	rm -rf $(ROMFSDIR)
	rm -rf $(IMAGEDIR)

romfs:
	[ -d $(ROMFSDIR) ] || mkdir -p $(ROMFSDIR)
	$(MAKE) -C sys romfs
	$(MAKE) -C init romfs

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
	lzop -9 -f -o $(LINUXCOMPRESSEDIMG) $(LINUXBOOTDIR)/Image
#	gzip --best --force --stdout $(LINUXBOOTDIR)/Image > $(LINUXCOMPRESSEDIMG)
	mkimage -A arm -O linux -T kernel -C lzo -a 0x60008000 -e 0x60008000 -n "Linux kernel image" -d $(LINUXCOMPRESSEDIMG) $(LINUXUBOOTIMG)
	openssl dgst -sha1 -sign $(SIGNATURE_KEY) -out $(LINUXSIG) $(LINUXUBOOTIMG)
	cat $(LINUXSIG) $(LINUXUBOOTIMG) > $(LINUXIMG)

image:
	[ -d $(IMAGEDIR) ] || mkdir -p $(IMAGEDIR)
	genext2fs -b 5120 -D $(ROOTDIR)/vendors/$(CONFIG_VENDOR)/uwic/dev.txt -d $(ROMFSDIR) $(EXT2IMG)
#	mkfs.cramfs -v $(ROMFSDIR) $(IMAGEDIR)/Image.cramfs
