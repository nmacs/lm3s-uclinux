.EXPORT_ALL_VARIABLES:

include $(ROOTDIR)/vendors/config/common/config.arch

CALL_OPKG   := $(OPKG) --tmp-dir /tmp -o /tmp -f /tmp/opkg.conf
PUBLISH_DEB := publish_package.sh
LOAD_DEB    := load_package.sh

APTREPODIR  := $(REPODIR)/apt

PACKAGES := sys init clock kernel mtdutils syslog busybox devtools lua mgate opkg watchdog
FIRMWARE_PACKAGES := sys init clock syslog busybox lua mgate watchdog devtools
DISTRIBUTION_PACKAGES := sys init clock syslog busybox lua ppp mgate watchdog devtools

.PHONY: repo
repo:
	for p in $$PACKAGES; do \
		$(MAKE) CODENAME=ekit -C $$p repo; \
	done
ifdef CONFIG_USER_ELSTER_MANUFACTURING
	$(MAKE) CODENAME=ekit-dev PKG_CROSS=i386 -C manufacturing repo
	$(MAKE) CODENAME=ekit-dev PKG_CROSS=amd64 -C manufacturing repo
endif
	echo -n "Make ekit repository snapshot? [Y/n]: "; \
	read answer; \
	if [ -z "$$answer" ]; then \
		answer="y"; \
	fi; \
	if [ "$$answer" = "y" ]; then \
		date_stamp=`date +%F-%H-%M-%S`; \
		reprepro -b $(APTREPODIR) gensnapshot ekit $$date_stamp; \
	fi

.PHONY: packages
packages:
	rm -rf $(PACKAGESDIR)
	mkdir -p $(PACKAGESDIR)
	for p in $$PACKAGES; do \
		$(MAKE) -C $$p package; \
		mv $$p/.pkg.cramfs $(PACKAGESDIR)/$$p; \
	done

.PHONY: distribution
distribution:
	rm -rf $(IMAGEDIR)/distribution
	mkdir -p $(IMAGEDIR)/distribution
	for p in $$DISTRIBUTION_PACKAGES; do \
		$(MAKE) -C $$p content; \
		cp -rd $$p/.content/* $(IMAGEDIR)/distribution; \
	done
	echo "0.0" > $(IMAGEDIR)/distribution/etc/version
	mkfs.cramfs $(IMAGEDIR)/distribution $(IMAGEDIR)/distribution.cramfs
	openssl dgst -sha1 -sign $(PRIV_KEY) -out $(IMAGEDIR)/distribution.cramfs.sig $(IMAGEDIR)/distribution.cramfs

UBIFS := mkfs.ubifs --verbose --min-io-size 1 -e 65408 -c 60 -x favor_lzo --reserved=0

.PHONY: firmware
firmware: distribution
	rm -rf $(FIRMWAREDIR)
	mkdir -p $(FIRMWAREDIR)
	mkdir $(FIRMWAREDIR)/boot
	mkdir $(FIRMWAREDIR)/opt
	mkdir $(FIRMWAREDIR)/etc
	mkdir $(FIRMWAREDIR)/etc/config
	mkdir $(FIRMWAREDIR)/var
	mkdir $(FIRMWAREDIR)/var/log
	cp $(IMAGEDIR)/linux.bin $(FIRMWAREDIR)/boot/linux.bin
	cp $(IMAGEDIR)/distribution.cramfs $(FIRMWAREDIR)/opt/distribution.cramfs
	cp $(IMAGEDIR)/distribution.cramfs.sig $(FIRMWAREDIR)/opt/distribution.cramfs.sig
	$(UBIFS) -r $(FIRMWAREDIR) -o $(IMAGEDIR)/firmware.ubifs
	$(ROOTDIR)/user/elster/scripts/make_firmware_mtd.py -o $(IMAGEDIR)/firmware.mtd $(IMAGEDIR)/linux.bin:kernel $(IMAGEDIR)/distribution.cramfs:distr $(IMAGEDIR)/distribution.cramfs.sig:distrsig

.PHONY: clean
clean:
	for dir in `ls`; do [ ! -f $$dir/Makefile ] || $(MAKEARCH) -C $$dir clean; done
	rm -rf output
