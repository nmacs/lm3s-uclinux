include $(ROOTDIR)/vendors/config/common/config.arch

.PHONY: repo content clean

repo: content
	mkdir -p content/DEBIAN
	cp control content/DEBIAN/control
	version=`cat control | grep Version: | cut -d' ' -f 2`; \
	name=`cat control | grep Package: | cut -d' ' -f 2`; \
	deb="$$name-$$version.deb"; \
	ucdeb="$$name-$$version.ucdeb"; \
	echo "Filename: $$ucdeb" >> content/DEBIAN/control; \
	dpkg --build content "$$deb"; \
	rm -rf .tmp; \
	mkdir -p .tmp; \
	cp "$$deb" "./.tmp/$$deb"; \
	cd ./.tmp; \
	ar -x "$$deb"; \
	rm -f "$$deb"; \
	gunzip data.tar.gz; \
	gunzip control.tar.gz; \
	ar rcs "$$ucdeb" control.tar data.tar; \
	mv "$$ucdeb" "$$REPODIR/output/"; \
	cd ..; \
	mv "$$deb" "$$REPODIR/output/"; \
	cat content/DEBIAN/control >> $$REPODIR/output/Packages; \
	echo "" >> $$REPODIR/output/Packages

clean:
	rm -rf content
	rm -f *.deb *.ucdeb