MTDUTILS_SRC := mtd-utils-1.5.0

CFLAGS += -I$(STAGEDIR)/include
CFLAGS += -L$(STAGEDIR)/lib
CFLAGS += $(ZLIBCPPFLAGS) $(LZOCPPFLAGS)
CFLAGS += -DWITHOUT_XATTR

all:
	$(MAKE) -C $(MTDUTILS_SRC) WITHOUT_LARGEFILE=1

clean:
	$(MAKE) -C $(MTDUTILS_SRC) clean
	rm -rf $(MTDUTILS_SRC)/$(CROSS:-=)

romfs:
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_ERASE          $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/flash_erase
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_ERASEALL       $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/flash_eraseall
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_LOCK           $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/flash_lock
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_UNLOCK         $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/flash_unlock
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_FLASH_INFO     $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/flash_info
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_FLASH_OTP_INFO $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/flash_otp_info
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_FLASH_OTP_DUMP $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/flash_otp_dump
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_FLASHCP        $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/flashcp
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_FTL_CHECK      $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/ftl_check
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_FTL_FORMAT     $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/ftl_format
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_JFFS2DUMP      $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/jffs2dump
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_MKFSJFFS2      $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/mkfs.jffs2
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_MTD_DEBUG      $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/mtd_debug
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_NFTLDUMP       $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/nftldump
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_NFTL_FORMAT    $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/nftl_format
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_NANDDUMP       $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/nanddump
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_NANDTEST       $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/nandtest
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_NANDWRITE      $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/nandwrite
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_DOC_LOADBIOS   $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/doc_loadbios
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_DOC_LOADIPL    $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/doc_loadipl
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_DOCFDISK       $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/docfdisk
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_RFDDUMP        $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/rfddump
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_RFDFORMAT      $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/rfdformat
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_RECV_IMAGE     $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/recv_image
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_SERVE_IMAGE    $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/server_image
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_SUMTOOL        $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/sumtool

	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_UBIUPDATEVOL   $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/ubiupdatevol /bin/ubiupdatevol
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_UBIMKVOL       $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/ubimkvol /bin/ubimkvol
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_UBIRMVOL       $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/ubirmvol /bin/ubirmvol
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_UBICRC32       $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/ubicrc32 /bin/ubicrc32
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_UBINFO         $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/ubinfo /bin/ubinfo
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_UBIATTACH      $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/ubiattach /bin/ubiattach
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_UBIDETACH      $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/ubidetach /bin/ubidetach
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_UBINIZE        $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/ubinize /bin/ubinize
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_UBIFORMAT      $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/ubiformat /bin/ubiformat
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_UBIRENAME      $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/ubirename /bin/ubirename
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_MTDINFO        $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/mtdinfo /bin/mtdinfo
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_UBIRSVOL       $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/ubirsvol /bin/ubirsvol


repo:
	$(REPOINST) -e CONFIG_USER_MTDUTILS_ERASE          $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/flash_erase
	$(REPOINST) -e CONFIG_USER_MTDUTILS_ERASEALL       $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/flash_eraseall
	$(REPOINST) -e CONFIG_USER_MTDUTILS_LOCK           $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/flash_lock
	$(REPOINST) -e CONFIG_USER_MTDUTILS_UNLOCK         $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/flash_unlock
	$(REPOINST) -e CONFIG_USER_MTDUTILS_FLASH_INFO     $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/flash_info
	$(REPOINST) -e CONFIG_USER_MTDUTILS_FLASH_OTP_INFO $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/flash_otp_info
	$(REPOINST) -e CONFIG_USER_MTDUTILS_FLASH_OTP_DUMP $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/flash_otp_dump
	$(REPOINST) -e CONFIG_USER_MTDUTILS_FLASHCP        $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/flashcp
	$(REPOINST) -e CONFIG_USER_MTDUTILS_FTL_CHECK      $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/ftl_check
	$(REPOINST) -e CONFIG_USER_MTDUTILS_FTL_FORMAT     $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/ftl_format
	$(REPOINST) -e CONFIG_USER_MTDUTILS_JFFS2DUMP      $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/jffs2dump
	$(REPOINST) -e CONFIG_USER_MTDUTILS_MKFSJFFS2      $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/mkfs.jffs2
	$(REPOINST) -e CONFIG_USER_MTDUTILS_MTD_DEBUG      $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/mtd_debug
	$(REPOINST) -e CONFIG_USER_MTDUTILS_NFTLDUMP       $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/nftldump
	$(REPOINST) -e CONFIG_USER_MTDUTILS_NFTL_FORMAT    $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/nftl_format
	$(REPOINST) -e CONFIG_USER_MTDUTILS_NANDDUMP       $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/nanddump
	$(REPOINST) -e CONFIG_USER_MTDUTILS_NANDTEST       $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/nandtest
	$(REPOINST) -e CONFIG_USER_MTDUTILS_NANDWRITE      $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/nandwrite
	$(REPOINST) -e CONFIG_USER_MTDUTILS_DOC_LOADBIOS   $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/doc_loadbios
	$(REPOINST) -e CONFIG_USER_MTDUTILS_DOC_LOADIPL    $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/doc_loadipl
	$(REPOINST) -e CONFIG_USER_MTDUTILS_DOCFDISK       $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/docfdisk
	$(REPOINST) -e CONFIG_USER_MTDUTILS_RFDDUMP        $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/rfddump
	$(REPOINST) -e CONFIG_USER_MTDUTILS_RFDFORMAT      $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/rfdformat
	$(REPOINST) -e CONFIG_USER_MTDUTILS_RECV_IMAGE     $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/recv_image
	$(REPOINST) -e CONFIG_USER_MTDUTILS_SERVE_IMAGE    $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/server_image
	$(REPOINST) -e CONFIG_USER_MTDUTILS_SUMTOOL        $(MTDUTILS_SRC)/$(CROSS:-=)/ /bin/sumtool

	$(REPOINST) -e CONFIG_USER_MTDUTILS_UBIUPDATEVOL   $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/ubiupdatevol /bin/ubiupdatevol
	$(REPOINST) -e CONFIG_USER_MTDUTILS_UBIMKVOL       $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/ubimkvol /bin/ubimkvol
	$(REPOINST) -e CONFIG_USER_MTDUTILS_UBIRMVOL       $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/ubirmvol /bin/ubirmvol
	$(REPOINST) -e CONFIG_USER_MTDUTILS_UBICRC32       $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/ubicrc32 /bin/ubicrc32
	$(REPOINST) -e CONFIG_USER_MTDUTILS_UBINFO         $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/ubinfo /bin/ubinfo
	$(REPOINST) -e CONFIG_USER_MTDUTILS_UBIATTACH      $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/ubiattach /bin/ubiattach
	$(REPOINST) -e CONFIG_USER_MTDUTILS_UBIDETACH      $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/ubidetach /bin/ubidetach
	$(REPOINST) -e CONFIG_USER_MTDUTILS_UBINIZE        $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/ubinize /bin/ubinize
	$(REPOINST) -e CONFIG_USER_MTDUTILS_UBIFORMAT      $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/ubiformat /bin/ubiformat
	$(REPOINST) -e CONFIG_USER_MTDUTILS_UBIRENAME      $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/ubirename /bin/ubirename
	$(REPOINST) -e CONFIG_USER_MTDUTILS_MTDINFO        $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/mtdinfo /bin/mtdinfo
	$(REPOINST) -e CONFIG_USER_MTDUTILS_UBIRSVOL       $(MTDUTILS_SRC)/$(CROSS:-=)/ubi-utils/ubirsvol /bin/ubirsvol