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
	rm -rf $(ROMDISK); mkdir -p $(ROMDISK)
	$(ROMFSINST) -r $(ROMDISK) $(KEY)     /key
	$(ROMFSINST) -r $(ROMDISK) $(PUB_KEY) /pub_key
	mkfs.cramfs $(ROMDISK) $(ROMDISK_IMG)

uboot:
	-$(MAKE) -C $(ROOTDIR)/uboot mrproper
	$(MAKE) -C $(ROOTDIR)/uboot atlas_config
	$(MAKE) -j 4 -C $(ROOTDIR)/uboot ARCH=arm u-boot.bin
	cp $(ROOTDIR)/uboot/u-boot.bin $(UBOOT_IMG)

.PHONY: flash_mcu
flash_mcu: uboot romdisk
	openocd -f $(PRODUCTDIR)/jtag.cfg -c "flash_mcu $(UBOOT_IMG) $(ROMDISK_IMG)"

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
#	cat $(LINUXBOOTDIR)/Image | lzma -c -z -e - > $(LINUXCOMPRESSEDIMG)
	cat $(LINUXBOOTDIR)/Image | lzop -9 -f -o $(LINUXCOMPRESSEDIMG) -
#	gzip --fast --force --stdout $(LINUXBOOTDIR)/Image > $(LINUXCOMPRESSEDIMG)
	mkimage -A arm -O linux -T kernel -C lzo -a 0x60008000 -e 0x60008000 -n "Linux kernel image" -d $(LINUXCOMPRESSEDIMG) $(LINUXUBOOTIMG)
	openssl dgst -sha1 -sign $(PRIV_KEY) -out $(LINUXSIG) $(LINUXUBOOTIMG)
	cat $(LINUXSIG) $(LINUXUBOOTIMG) > $(LINUXIMG)

image:
	[ -d $(IMAGEDIR) ] || mkdir -p $(IMAGEDIR)
	genext2fs -b 5120 -D $(PRODUCTDIR)/dev.txt -d $(ROMFSDIR) $(EXT2IMG)
#	mkfs.cramfs -v $(ROMFSDIR) $(IMAGEDIR)/Image.cramfs
	$(UBIFS) -r $(ROMFSDIR) -o $(UBIFSIMG)