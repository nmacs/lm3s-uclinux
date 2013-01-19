
.PHONY: repo content clean

repo: content
	mkdir -p content/DEBIAN
	cp control content/DEBIAN/control
	version=`cat control | grep Version: | cut -d' ' -f 2`; \
	name=`cat control | grep Package: | cut -d' ' -f 2`; \
	filename="$$name-$$version.ucdeb"; \
	dpkg --build content "$$filename"; \
	rm -rf .tmp; \
	mkdir -p .tmp; \
	mv "$$filename" ./.tmp; \
	cd ./.tmp; \
	ar -x "$$filename"; \
	rm -f "$$filename"; \
	gunzip data.tar.gz; \
	gunzip control.tar.gz; \
	ar rcs "$$filename" control.tar data.tar; \
	mv "$$filename" $$REPODIR/output/; \
	cd ..; \
	echo "Filename: $$filename" >> content/DEBIAN/control; \
	cat control >> $$REPODIR/output/Packages; \
	echo "" >> $$REPODIR/output/Packages

clean:
	rm -rf content
	rm -f *.deb