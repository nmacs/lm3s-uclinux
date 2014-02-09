.EXPORT_ALL_VARIABLES:

include $(ROOTDIR)/vendors/config/common/config.arch

PUBLISH_DEB := publish_package.sh
LOAD_DEB    := load_package.sh

APTREPODIR  := $(REPODIR)/apt

PACKAGES := sys init clock kernel mtdutils syslog busybox devtools lua mgate watchdog

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

.PHONY: clean
clean:
	for dir in `ls`; do [ ! -f $$dir/Makefile ] || $(MAKEARCH) -C $$dir clean; done
	rm -rf output
