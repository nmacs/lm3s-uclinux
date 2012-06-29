#
#	Makefile -- Build instructions for TI/TMS320DM270
#

include $(LINUX_CONFIG)
include $(CONFIG_CONFIG)
include $(ARCH_CONFIG)

LINUXFILE  = linux
ROMFSFILE  = romfs.img
INITRDFILE = initrd.img
JFFS2FILE  = jffs2.img

LINUXIMG   = $(IMAGEDIR)/$(LINUXFILE)
ROMFSIMG   = $(IMAGEDIR)/$(ROMFSFILE)
INITRDIMG  = $(IMAGEDIR)/$(INITRDFILE)
JFFS2IMG   = $(IMAGEDIR)/$(JFFS2FILE)

IMAGE      = $(IMAGEDIR)/image.bin
ELFIMAGE   = $(IMAGEDIR)/image.elf

ROMFS_DIRS = bin dev etc home lib mnt proc sys usr var tmp

clean:

romfs:
	[ -d $(ROMFSDIR)/$$i ] || mkdir -p $(ROMFSDIR)
	for i in $(ROMFS_DIRS); do \
		[ -d $(ROMFSDIR)/$$i ] || mkdir -p $(ROMFSDIR)/$$i; \
	done
ifdef CONFIG_PPP
	[ -d $(ROMFSDIR)/dev/pts ] || mkdir -p $(ROMFSDIR)/dev/pts
	for i in $(PPPPTS); do \
		touch $(ROMFSDIR)/dev/pts/@$$i; \
	done
endif
	$(ROMFSINST) /etc/inittab
	$(ROMFSINST) /etc/opkg.conf
	$(ROMFSINST) /etc/resolv.conf

modules.dep:
ifdef CONFIG_MODULES
	( \
		cd $(ROMFSDIR); \
		find ./lib/modules -type f -a ! -name modules.dep | \
		sed -e 's/$$/:/' -e 's/^\.//' \
	) > $(ROMFSDIR)/lib/modules/modules.dep
endif

$(JFFS2IMG): modules.dep
	$(ROOTDIR)/user/mtd-utils/build/mkfs.jffs2 -o $(JFFS2IMG) -q -e 65536 -d $(ROMFSDIR)

$(INITRDIMG): modules.dep
	genext2fs -b 4096 -f dev.txt -d $(ROMFSDIR) $(INITRDIMG)
	gzip -9 -f $(INITRDIMG)

$(ROMFSIMG): modules.dep
	genromfs -v -V "ROMdisk" -f $(ROMFSIMG) -d $(ROMFSDIR)

.PHONY: image $(FSIMG) modules.dep
image: $(FSIMG)
	[ -d $(IMAGEDIR) ] || mkdir -p $(IMAGEDIR)
	$(CROSS_COMPILE)objcopy -O binary --remove-section=.ramvec \
			--remove-section=.eram \
			--set-section-flags=.romvec=CONTENTS,ALLOC,LOAD,READONLY,CODE \
			$(ROOTDIR)/$(LINUXDIR)/linux $(IMAGEDIR)/linux.bin
ifdef CONFIG_RAM_ATTACHED_ROMFS
	cat $(IMAGEDIR)/linux.bin $(ROMFSIMG) > $(IMAGE)
endif
