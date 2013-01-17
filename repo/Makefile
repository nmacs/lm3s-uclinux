
repo:
	rm -rf $(REPODIR)/output
	mkdir -p $(REPODIR)/output
	touch $(REPODIR)/output/Packages
	for dir in `ls`; do [ ! -f $$dir/Makefile ] || $(MAKE) -C $$dir repo ; done

clean:
	for dir in `ls`; do [ ! -f $$dir/Makefile ] || $(MAKE) -C $$dir clean ; done
