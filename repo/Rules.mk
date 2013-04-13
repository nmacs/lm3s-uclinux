include $(ROOTDIR)/vendors/config/common/config.arch

.PHONY: repo content clean

repo: content
	build_package.sh $(CURDIR) $(APTREPODIR) ekit

clean:
	rm -rf .content
	rm -f *.deb *.ucdeb