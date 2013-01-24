.EXPORT_ALL_VARIABLES:

include $(ROOTDIR)/vendors/config/common/config.arch

CALL_OPKG=$(OPKG) --tmp-dir /tmp -o /tmp -f /tmp/opkg.conf

.PHONY: repo
repo:
	rm -rf $(REPODIR)/output
	mkdir -p $(REPODIR)/output
	touch $(REPODIR)/output/Packages
	for dir in `ls`; do [ ! -f $$dir/Makefile ] || $(MAKEARCH) -C $$dir repo ; done

.PHONY: firmware
firmware:
	rm -rf $(REPODIR)/output/.firmware
	mkdir -p $(REPODIR)/output/.firmware
	mkdir -p /tmp/usr/lib/opkg
	echo "src snapshots file://$(REPODIR)/output" > /tmp/opkg.conf
	echo "lists_dir ext /" >> /tmp/opkg.conf
	$(CALL_OPKG) update
	$(CALL_OPKG) --cache $(REPODIR)/output/.firmware --download-only  install firmware
	cp $(REPODIR)/output/Packages $(REPODIR)/output/.firmware/Packages
	version=`cat $$REPODIR/firmware/control | grep Version: | cut -d' ' -f 2`; \
	filename="firmware-$$version"; \
	mkfs.cramfs "$$REPODIR/output/.firmware/" "$$REPODIR/output/$$filename"

.PHONY: clean
clean:
	for dir in `ls`; do [ ! -f $$dir/Makefile ] || $(MAKEARCH) -C $$dir clean ; done
	rm -rf output
