
.PHONY: repo content clean

repo: content
	mkdir -p content/DEBIAN
	cp control content/DEBIAN/control
	version=`cat control | grep Version: | cut -d' ' -f 2`; \
	name=`cat control | grep Package: | cut -d' ' -f 2`; \
	filename="$$name-$$version.deb"; \
	dpkg --build content "$$filename"; \
	mv "$$filename" $$REPODIR/output/; \
	echo "Filename: $$filename" >> content/DEBIAN/control; \
	cat control >> $$REPODIR/output/Packages; \
	echo "" >> $$REPODIR/output/Packages

clean:
	rm -rf content
	rm -f *.deb