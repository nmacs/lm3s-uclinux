include $(ROOTDIR)/vendors/config/common/config.arch

.PHONY: repo content clean

repo: content
	mkdir -p content/DEBIAN
	version=`cat control | grep Version: | cut -d' ' -f 2`; \
	name=`cat control | grep Package: | cut -d' ' -f 2`; \
	deb="$$name-$$version.deb"; \
	ucdeb="$$name-$$version.ucdeb"; \
	cp control content/DEBIAN/control; \
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
	mv "$$ucdeb" "$$REPODIR/output/ucdeb"; \
	cd ..; \
	cat content/DEBIAN/control >> $$REPODIR/output/ucdeb/Packages; \
	md5=`md5sum "$$REPODIR/output/ucdeb/$$ucdeb" | cut -d' ' -f 1`; \
	echo "MD5Sum: $$md5" >> $$REPODIR/output/ucdeb/Packages; \
	echo "" >> $$REPODIR/output/ucdeb/Packages; \
	cp control content/DEBIAN/control ; \
	echo "Filename: $$deb" >> content/DEBIAN/control; \
	dpkg --build content "$$deb"; \
	mv "$$deb" "$$REPODIR/output/deb"; \
	cat content/DEBIAN/control >> $$REPODIR/output/deb/Packages; \
	md5=`md5sum "$$REPODIR/output/deb/$$deb" | cut -d' ' -f 1`; \
	echo "MD5Sum: $$md5" >> $$REPODIR/output/deb/Packages; \
	echo "" >> $$REPODIR/output/deb/Packages

clean:
	rm -rf content
	rm -f *.deb *.ucdeb