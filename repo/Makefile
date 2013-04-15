.EXPORT_ALL_VARIABLES:

include $(ROOTDIR)/vendors/config/common/config.arch

CALL_OPKG   := $(OPKG) --tmp-dir /tmp -o /tmp -f /tmp/opkg.conf
PUBLISH_DEB := publish_package.sh
LOAD_DEB    := load_package.sh

APTREPODIR  := $(REPODIR)/apt

.PHONY: repo
repo:
	$(MAKE) -C sys repo
	$(MAKE) -C init repo
	$(MAKE) -C clock repo
	$(MAKE) -C kernel repo
	$(MAKE) -C mtdutils repo
	$(MAKE) -C syslog repo
	$(MAKE) -C busybox repo
	$(MAKE) -C devtools repo
	$(MAKE) -C lua repo
	$(MAKE) -C mgate repo
	$(MAKE) -C opkg repo
	$(MAKE) -C watchdog repo
	$(MAKE) -C firmware repo
	$(MAKE) -C manufacturing

# .PHONY: firmware
# firmware:
# 	rm -rf $(REPODIR)/output/.firmware
# 	mkdir -p $(REPODIR)/output/.firmware
# 	mkdir -p /tmp/usr/lib/opkg
# 	echo "src snapshots file://$(REPODIR)/output/ucdeb" > /tmp/opkg.conf
# 	echo "lists_dir ext /" >> /tmp/opkg.conf
# 	$(CALL_OPKG) update
# 	$(CALL_OPKG) --cache $(REPODIR)/output/.firmware --download-only  install firmware
# 	cp $(REPODIR)/output/ucdeb/Packages $(REPODIR)/output/.firmware/Packages
# 	version=`cat $$REPODIR/firmware/control | grep Version: | cut -d' ' -f 2`; \
# 	filename="firmware_$$version"; \
# 	mkfs.cramfs "$$REPODIR/output/.firmware/" "$$REPODIR/output/$$filename"

.PHONY: clean
clean:
	for dir in `ls`; do [ ! -f $$dir/Makefile ] || $(MAKEARCH) -C $$dir clean ; done
	rm -rf output
