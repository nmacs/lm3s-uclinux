LIBTOMFASTMATH_DIR   := tomsfastmath-0.12
LIBTOMMATH_DIR       := libtommath-0.42.0

.PHONY: all
LIBPATH := $(CURDIR)/output/lib
export LIBPATH
INCPATH := $(CURDIR)/output/include
export INCPATH
DATAPATH := $(CURDIR)/output/doc
export DATAPATH
INSTALL_USER := $(shell whoami)
export INSTALL_USER
INSTALL_GROUP := $(shell whoami)
export INSTALL_GROUP

all:
ifdef CONFIG_LIB_TOMMATH_FASTMATH
	$(MAKE) -C $(LIBTOMFASTMATH_DIR)
	$(MAKE) -C $(LIBTOMFASTMATH_DIR) install
endif
ifdef CONFIG_LIB_TOMMATH_MATH
	$(MAKE) -C $(LIBTOMMATH_DIR)
	$(MAKE) -C $(LIBTOMMATH_DIR) install
endif

clean:
	$(MAKE) -C $(LIBTOMFASTMATH_DIR) clean
	$(MAKE) -C $(LIBTOMMATH_DIR) clean
	rm -rf output

romfs:
