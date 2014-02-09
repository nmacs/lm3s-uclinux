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

DISTRIBUTION_PACKAGES := sys init syslog busybox lua ppp mgate
DISTRIBUTION_PACKAGES += devtools

UBIFS      := mkfs.ubifs --verbose --min-io-size 1 -e 65408 -c 60 -x favor_lzo --reserved=0

clean:
	rm -rf $(ROMFSDIR)
	rm -rf $(IMAGEDIR)

romfs:
	[ -d $(ROMFSDIR) ] || mkdir -p $(ROMFSDIR)
	$(MAKE) -C sys romfs
	$(MAKE) -C init romfs
	if [ -f $(LINUXIMG) ]; then \
		$(ROMFSINST) -d $(LINUXIMG) /boot/linux.bin; \
	fi

romdisk:
	rm -rf $(ROMDISK); mkdir -p $(ROMDISK)
	$(ROMFSINST) -r $(ROMDISK) $(KEY)     /key
	$(ROMFSINST) -r $(ROMDISK) $(PUB_KEY) /pub_key
	$(ROMFSINST) -r $(ROMDISK) $(CURDIR)/security/ca.pem /ca.pem
	$(ROMFSINST) -r $(ROMDISK) $(CURDIR)/security/rex00001.pem /rex00001.pem
	$(ROMFSINST) -r $(ROMDISK) $(CURDIR)/security/rex00001.key /rex00001.key
	mkfs.cramfs $(ROMDISK) $(ROMDISK_IMG)

uboot: romdisk
	-$(MAKE) -C $(ROOTDIR)/uboot mrproper
	-rm -f "$(UBOOT_IMG).full"
	$(MAKE) -C $(ROOTDIR)/uboot atlas_config
	$(MAKE) -j 4 -C $(ROOTDIR)/uboot ARCH=arm u-boot.bin
	cp $(ROOTDIR)/uboot/u-boot.bin $(UBOOT_IMG)
	image_size=$$((1024 * 1024)); \
	rom_disk_offset=327680; \
	bl_size=$$(stat -c%s "$(UBOOT_IMG)"); \
	rd_size=$$(stat -c%s "$(ROMDISK_IMG)"); \
	gap1=$$(($$rom_disk_offset - $$bl_size)); \
	gap2=$$(($$image_size - $$rom_disk_offset)); \
	gap2=$$(($$gap2 - $$rd_size)); \
	cat "$(UBOOT_IMG)" > "$(UBOOT_IMG).full"; \
	tr "\000" "\377" < /dev/zero | dd ibs=1 count=$$gap1 >> "$(UBOOT_IMG).full"; \
	cat "$(ROMDISK_IMG)" >> "$(UBOOT_IMG).full"; \
	tr "\000" "\377" < /dev/zero | dd ibs=1 count=$$gap2 >> "$(UBOOT_IMG).full"

.PHONY: initramfs
initramfs:
	rm -rf $(IMAGEDIR)/initramfs
	mkdir -p $(IMAGEDIR)/initramfs
	for p in $(DISTRIBUTION_PACKAGES); do \
		$(MAKE) -C $(REPODIR)/$$p content; \
		cp -rd $(REPODIR)/$$p/.content/* $(IMAGEDIR)/initramfs; \
	done
	cp upgrade.sh $(IMAGEDIR)/initramfs/bin/upgrade
	cd $(IMAGEDIR)/initramfs; ln -s bin/init init
	mkinitramfs $(IMAGEDIR)/initramfs $(IMAGEDIR)/initramfs.cpio.lzo
	mkimage -A arm -O linux -T ramdisk -C lzo -n "initramfs image" -d $(IMAGEDIR)/initramfs.cpio.lzo $(IMAGEDIR)/initramfs.cpio.lzo.uboot
	openssl dgst -sha1 -sign $(PRIV_KEY) -out $(IMAGEDIR)/initramfs.cpio.lzo.uboot.sig $(IMAGEDIR)/initramfs.cpio.lzo.uboot
	cat $(IMAGEDIR)/initramfs.cpio.lzo.uboot.sig $(IMAGEDIR)/initramfs.cpio.lzo.uboot > $(IMAGEDIR)/initrd.bin

.PHONY: firmware
firmware: initramfs
	rm -rf $(FIRMWAREDIR)
	rm -f $(IMAGEDIR)/firmware.mtd
	rm -f $(IMAGEDIR)/firmware.ubifs
	mkdir -p $(FIRMWAREDIR)
	mkdir $(FIRMWAREDIR)/boot
	mkdir $(FIRMWAREDIR)/opt
	mkdir $(FIRMWAREDIR)/etc
	mkdir $(FIRMWAREDIR)/etc/config
	mkdir $(FIRMWAREDIR)/var
	mkdir $(FIRMWAREDIR)/var/log
	cp $(IMAGEDIR)/linux.bin $(FIRMWAREDIR)/boot/linux.bin
	cp $(IMAGEDIR)/initrd.bin $(FIRMWAREDIR)/boot/initrd.bin
	$(UBIFS) -r $(FIRMWAREDIR) -o $(IMAGEDIR)/firmware.ubifs
	if [ -f $(ROOTDIR)/user/elster/scripts/make_firmware_mtd.py ]; then \
		$(ROOTDIR)/user/elster/scripts/make_firmware_mtd.py -o $(IMAGEDIR)/firmware.mtd $(IMAGEDIR)/linux.bin:kernel $(IMAGEDIR)/initrd.bin:initrd; \
	fi

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
	mkimage -A arm -O linux -T kernel -C lzo -a 0x60008000 -e 0x60008000 -n "linux kernel image" -d $(LINUXCOMPRESSEDIMG) $(LINUXUBOOTIMG)
	openssl dgst -sha1 -sign $(PRIV_KEY) -out $(LINUXSIG) $(LINUXUBOOTIMG)
	cat $(LINUXSIG) $(LINUXUBOOTIMG) > $(LINUXIMG)

image:
	[ -d $(IMAGEDIR) ] || mkdir -p $(IMAGEDIR)
	genext2fs -b 5120 -D $(PRODUCTDIR)/dev.txt -d $(ROMFSDIR) $(EXT2IMG)
#	mkfs.cramfs -v $(ROMFSDIR) $(IMAGEDIR)/Image.cramfs
	$(UBIFS) -r $(ROMFSDIR) -o $(UBIFSIMG)

.PHONY: host-%
host-%:
	$(MAKE) -C $(ROOTDIR)/user/elster $@