.EXPORT_ALL_VARIABLES:

include $(ROOTDIR)/vendors/config/common/config.arch

CALL_OPKG   := $(OPKG) --tmp-dir /tmp -o /tmp -f /tmp/opkg.conf
PUBLISH_DEB := publish_package.sh
LOAD_DEB    := load_package.sh

APTREPODIR  := $(REPODIR)/apt

.PHONY: repo
repo:
	$(MAKE) CODENAME=ekit -C sys repo
	$(MAKE) CODENAME=ekit -C init repo
	$(MAKE) CODENAME=ekit -C clock repo
	$(MAKE) CODENAME=ekit -C kernel repo
	$(MAKE) CODENAME=ekit -C mtdutils repo
	$(MAKE) CODENAME=ekit -C syslog repo
	$(MAKE) CODENAME=ekit -C busybox repo
	$(MAKE) CODENAME=ekit -C devtools repo
	$(MAKE) CODENAME=ekit -C lua repo
	$(MAKE) CODENAME=ekit -C mgate repo
	$(MAKE) CODENAME=ekit -C opkg repo
	$(MAKE) CODENAME=ekit -C watchdog repo
	$(MAKE) CODENAME=ekit -C firmware repo
	$(MAKE) CODENAME=ekit-dev PKG_CROSS=i386 -C manufacturing repo
	$(MAKE) CODENAME=ekit-dev PKG_CROSS=amd64 -C manufacturing repo
	echo -n "Make ekit repository snapshot? [Y/n]: "; \
	read answer; \
	if [ -z "$$answer" ]; then \
		answer="y"; \
	fi; \
	if [ "$$answer" = "y" ]; then \
		date_stamp=`date +%F-%H-%M-%S`; \
		reprepro -b $(APTREPODIR) gensnapshot ekit $$date_stamp; \
	fi

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
