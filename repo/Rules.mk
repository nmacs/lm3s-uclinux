.EXPORT_ALL_VARIABLES:

include $(ROOTDIR)/vendors/config/common/config.arch

CONTENT  := $(CURDIR)/.content
REPOINST := $(ROMFSINST) -r $(CONTENT) -d

.PHONY: repo content clean

repo: content
	build_package.sh $(CURDIR) $(APTREPODIR) ekit

clean:
	rm -rf .content
	rm -f *.deb *.ucdeb