
# -*- sh -*-

VERSION = 1.5.0

CPPFLAGS += -I./include -I$(BUILDDIR)/include -I./ubi-utils/include $(ZLIBCPPFLAGS) $(LZOCPPFLAGS)

ifeq ($(WITHOUT_XATTR), 1)
  CPPFLAGS += -DWITHOUT_XATTR
endif
ifeq ($(WITHOUT_LZO), 1)
  CPPFLAGS += -DWITHOUT_LZO
else
  LZOLDLIBS = -llzo2
endif

TESTS = tests

MTD_BINS = \
	ftl_format flash_erase nanddump doc_loadbios \
	ftl_check mkfs.jffs2 flash_lock flash_unlock \
	flash_otp_info flash_otp_dump mtd_debug flashcp nandwrite nandtest \
	jffs2dump \
	nftldump nftl_format docfdisk \
	rfddump rfdformat \
	serve_image recv_image \
	sumtool jffs2reader
UBI_BINS = \
	ubiupdatevol ubimkvol ubirmvol ubicrc32 ubinfo ubiattach \
	ubidetach ubinize ubiformat ubirename mtdinfo ubirsvol

BINS = $(MTD_BINS)
BINS += mkfs.ubifs/mkfs.ubifs
BINS += $(addprefix ubi-utils/,$(UBI_BINS))
SCRIPTS = flash_eraseall

TARGETS_$(CONFIG_USER_MTDUTILS_DOCFDISK)	+= doc_fdisk
TARGETS_$(CONFIG_USER_MTDUTILS_DOC_LOADBIOS)	+= doc_loadbios
TARGETS_$(CONFIG_USER_MTDUTILS_DOC_LOADIPL)	+= doc_loadipl
TARGETS_$(CONFIG_USER_MTDUTILS_ERASE)		+= flash_erase
TARGETS_$(CONFIG_USER_MTDUTILS_LOCK)		+= flash_lock
TARGETS_$(CONFIG_USER_MTDUTILS_UNLOCK)		+= flash_unlock
TARGETS_$(CONFIG_USER_MTDUTILS_FLASH_INFO)	+= flash_info
TARGETS_$(CONFIG_USER_MTDUTILS_FLASH_OTP_INFO)	+= flash_otp_info
TARGETS_$(CONFIG_USER_MTDUTILS_FLASH_OTP_DUMP)	+= flash_otp_dump
TARGETS_$(CONFIG_USER_MTDUTILS_FLASHCP)		+= flashcp
TARGETS_$(CONFIG_USER_MTDUTILS_FTL_CHECK)	+= ftl_check
TARGETS_$(CONFIG_USER_MTDUTILS_FTL_FORMAT)	+= ftl_format
TARGETS_$(CONFIG_USER_MTDUTILS_JFFS2DUMP)	+= jffs2dump
TARGETS_$(CONFIG_USER_MTDUTILS_MKFSJFFS2)	+= mkfs.jffs2
TARGETS_$(CONFIG_USER_MTDUTILS_MTD_DEBUG)	+= mtd_debug
TARGETS_$(CONFIG_USER_MTDUTILS_NFTL_FORMAT)	+= nftl_format
TARGETS_$(CONFIG_USER_MTDUTILS_NFTLDUMP)	+= nftldump
TARGETS_$(CONFIG_USER_MTDUTILS_NANDDUMP)	+= nanddump
TARGETS_$(CONFIG_USER_MTDUTILS_NANDTEST)	+= nandtest
TARGETS_$(CONFIG_USER_MTDUTILS_NANDWRITE)	+= nandwrite
TARGETS_$(CONFIG_USER_MTDUTILS_RFDDUMP)		+= rfddump
TARGETS_$(CONFIG_USER_MTDUTILS_RFDFORMAT)	+= rfdformat
TARGETS_$(CONFIG_USER_MTDUTILS_RECV_IMAGE)	+= recv_image
TARGETS_$(CONFIG_USER_MTDUTILS_SERVE_IMAGE)	+= serve_image
TARGETS_$(CONFIG_USER_MTDUTILS_SUMTOOL)		+= sumtool

TARGETS_UBI_$(CONFIG_USER_MTDUTILS_UBIUPDATEVOL)	+= ubiupdatevol
TARGETS_UBI_$(CONFIG_USER_MTDUTILS_UBIMKVOL)	+= ubimkvol
TARGETS_UBI_$(CONFIG_USER_MTDUTILS_UBIRMVOL)	+= ubirmvol
TARGETS_UBI_$(CONFIG_USER_MTDUTILS_UBICRC32)	+= ubicrc32
TARGETS_UBI_$(CONFIG_USER_MTDUTILS_UBINFO)		+= ubinfo
TARGETS_UBI_$(CONFIG_USER_MTDUTILS_UBIATTACH)	+= ubiattach
TARGETS_UBI_$(CONFIG_USER_MTDUTILS_UBIDETACH)	+= ubidetach
TARGETS_UBI_$(CONFIG_USER_MTDUTILS_UBINIZE)		+= ubinize
TARGETS_UBI_$(CONFIG_USER_MTDUTILS_UBIFORMAT)	+= ubiformat
TARGETS_UBI_$(CONFIG_USER_MTDUTILS_UBIRENAME)	+= ubirename
TARGETS_UBI_$(CONFIG_USER_MTDUTILS_MTDINFO)		+= mtdinfo
TARGETS_UBI_$(CONFIG_USER_MTDUTILS_UBIRSVOL)	+= ubirsvol

BIN_TARGETS = $(TARGETS_y)
BIN_TARGETS += $(addprefix ubi-utils/,$(TARGETS_UBI_y))
TARGETS = $(BIN_TARGETS)
TARGETS += lib/libmtd.a
TARGETS += ubi-utils/libubi.a

OBJDEPS = $(BUILDDIR)/include/version.h

include common.mk

CLEAN_FIND = find "$(BUILDDIR)/" -xdev '(' -name '*.[ao]' -o -name '.*.c.dep' ')'

clean::
ifneq ($(BUILDDIR)/.git,)
ifneq ($(BUILDDIR),.)
ifneq ($(BUILDDIR),$(CURDIR))
	rm -rf $(BUILDDIR)
endif
endif
endif
# findutils v4.1.x (RHEL 4) do not have '+' syntax
	@if test -d "$(BUILDDIR)/"; then \
		$(CLEAN_FIND) -exec rm -f {} + 2> /dev/null || \
		$(CLEAN_FIND) -exec rm -f {} \; ; \
	fi
	rm -f $(BUILDDIR)/include/version.h
	$(MAKE) -C $(TESTS) clean

install:: $(addprefix $(BUILDDIR)/,${BINS}) ${SCRIPTS}
	mkdir -p ${DESTDIR}/${SBINDIR}
	install -m 0755 $^ ${DESTDIR}/${SBINDIR}/
	mkdir -p ${DESTDIR}/${MANDIR}/man1
	install -m 0644 mkfs.jffs2.1 ${DESTDIR}/${MANDIR}/man1/
	-gzip -9f ${DESTDIR}/${MANDIR}/man1/*.1

tests::
	$(MAKE) -C $(TESTS)

cscope:
	cscope -bR

$(BUILDDIR)/include/version.h: $(BUILDDIR)/include/version.h.tmp
	$(call BECHO,CHK)
	$(Q)cmp -s $@ $@.tmp && rm -f $@.tmp || mv $@.tmp $@
$(BUILDDIR)/include/version.h.tmp:
	${Q}mkdir -p $(dir $@)
	$(Q)echo '#define VERSION "$(VERSION)"' > $@

#
# Utils in top level
#
obj-mkfs.jffs2 = compr_rtime.o compr_zlib.o compr_lzo.o compr.o rbtree.o
LDFLAGS_mkfs.jffs2 = $(ZLIBLDFLAGS) $(LZOLDFLAGS)
LDLIBS_mkfs.jffs2  = -lz $(LZOLDLIBS)

LDFLAGS_jffs2reader = $(ZLIBLDFLAGS) $(LZOLDFLAGS)
LDLIBS_jffs2reader  = -lz $(LZOLDLIBS)

$(foreach v,$(MTD_BINS),$(eval $(call mkdep,,$(v))))

#
# Common libmtd
#
obj-libmtd.a = libmtd.o libmtd_legacy.o libcrc32.o libfec.o
$(call _mkdep,lib/,libmtd.a)

#
# Utils in mkfs.ubifs subdir
#
obj-mkfs.ubifs = crc16.o lpt.o compr.o devtable.o \
	hashtable/hashtable.o hashtable/hashtable_itr.o
LDLIBS_mkfs.ubifs = -lz -llzo2 -lm -luuid
$(call mkdep,mkfs.ubifs/,mkfs.ubifs,,ubi-utils/libubi.a)

#
# Utils in ubi-utils/ subdir
#
obj-libiniparser.a = libiniparser.o dictionary.o
obj-libscan.a      = libscan.o
obj-libubi.a       = libubi.o
obj-libubigen.a    = libubigen.o

obj-mtdinfo   = libubigen.a
obj-ubinize   = libubigen.a libiniparser.a
obj-ubiformat = libubigen.a libscan.a

$(foreach v,libubi.a libubigen.a libiniparser.a libscan.a,$(eval $(call _mkdep,ubi-utils/,$(v))))
$(foreach v,$(UBI_BINS),$(eval $(call mkdep,ubi-utils/,$(v),libubi.a ubiutils-common.o)))
