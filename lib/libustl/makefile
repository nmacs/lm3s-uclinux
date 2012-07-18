IN_LIBUSTL := true
export IN_LIBUSTL

#
# we need to do different things for 64 bit systems
#
ifdef CONFIG_64BIT
MACHINE=uClinux-dist64
else
MACHINE=uClinux-dist
endif

CONFIG_OPTS  := --prefix=// --install_prefix=$(shell pwd)/build/install

all: build/configured
	$(MAKE) -j4 -C build build_libs

############################################################################
#
# we may not have the library here, so setup for downloading if needed

ifeq ($(wildcard Configure),)

openssl-0.9.8i-url := http://www.openssl.org/source/openssl-0.9.8i.tar.gz
openssl-0.9.8k-url := http://www.openssl.org/source/openssl-0.9.8k.tar.gz
openssl-0.9.8n-url := http://www.openssl.org/source/openssl-0.9.8n.tar.gz
openssl-0.9.8r-url := http://www.openssl.org/source/openssl-0.9.8r.tar.gz

SRC_DIR := openssl-0.9.8r

WGET ?= wget

%.tar.bz2 %.tar.gz:
	@echo "Downloading $* ..."
	@[ -d "$$HOME/.downloads" ] || mkdir ~/.downloads
	@if [ ! -f "$$HOME/.downloads/$@" ]; then \
		(cd ~/.downloads; $(WGET) "$($*-url)"); \
	fi
	@if [ -f "$$HOME/.downloads/$@" ]; then \
			ln -fs "$$HOME/.downloads/$@" $@; \
	fi
	if [ ! -f "$@" ]; then \
		echo "Cannot find download for $@" >&2 ; \
		exit 1; \
	fi

$(SRC_DIR)/Configure $(SRC_DIR)/config: makefile $(SRC_DIR).tar.gz \
		patches/$(SRC_DIR).patch
	rm -rf $(SRC_DIR) build
	gunzip < $(SRC_DIR).tar.gz | tar xf -
	patch -p0 < patches/$(SRC_DIR).patch
	touch $(SRC_DIR)/Configure $(SRC_DIR)/config

else

SRC_DIR := .

endif

############################################################################

build/configured: makefile $(SRC_DIR)/config $(SRC_DIR)/Configure
	rm -rf build
	(cd $(SRC_DIR)/.; find . -type d) > .dirs
	(cd $(SRC_DIR)/.; find . ! -type d) | grep -v ./makefile > .files
	while read t; do mkdir -p build/$$t; done < .dirs
	while read t; do ln -s `pwd`/$(SRC_DIR)/$$t build/$$t; done < .files
	rm -f .dirs .files
	chmod +x build/config
	cd build; CROSS_COMPILE= MACHINE=$(MACHINE) ./config $(CONFIG_OPTS)
	$(MAKE) -C build depend
	$(MAKE) -C build links
	touch build/configured

clean:
	-rm -rf build

romfs:
	cd build/install/lib; \
	for i in *.so*; do \
		if [ -L $$i ]; then \
			$(ROMFSINST) -s `find $$i -printf %l` /lib/$$i; \
		elif [ -f $$i ]; then \
			$(ROMFSINST) /lib/$$i; \
		fi; \
	done

romfs_user:
	$(ROMFSINST) -e CONFIG_USER_OPENSSL_APPS build/install/bin/openssl /bin/openssl
	# $(ROMFSINST) -e CONFIG_USER_OPENSSL_APPS build/install/bin/c_rehash /bin/c_rehash


