.EXPORT_ALL_VARIABLES:

include $(ROOTDIR)/vendors/config/common/config.arch

repo:
	rm -rf $(REPODIR)/output
	mkdir -p $(REPODIR)/output
	touch $(REPODIR)/output/Packages
	for dir in `ls`; do [ ! -f $$dir/Makefile ] || $(MAKEARCH) -C $$dir repo ; done

clean:
	for dir in `ls`; do [ ! -f $$dir/Makefile ] || $(MAKEARCH) -C $$dir clean ; done
	rm -rf output
