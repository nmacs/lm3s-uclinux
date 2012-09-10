CAYSSL       := "$(ROOTDIR)/lib/cyassl/output"
OUTPUT       := "$(ROOTDIR)/user/stunnel/output"

CONFIG_OPTS  := --enable-static --disable-shared --with-ssl=$(CAYSSL) --prefix=$(OUTPUT) --host=arm-linux

SRC_DIR := stunnel-4.53
EXEC    := stunnel

ifdef CONFIG_LIB_CYASSL
  CFLAGS := $(CFLAGS) -DHAVE_LIB_CYASSL -DOPENSSL_NO_ENGINE -DOPENSSL_NO_COMP -DOPENSSL_NO_DH
endif

CFLAGS := $(CFLAGS) -Wl,--verbose

.PHONY: all lib configure

all: $(EXEC)

$(EXEC): configure
	$(MAKE) -C "$(SRC_DIR)/src"
	$(MAKE) -C "$(SRC_DIR)/src" install

configure : $(SRC_DIR)/src/Makefile

$(SRC_DIR)/src/Makefile: makefile
	cd $(SRC_DIR); ./configure $(CONFIG_OPTS)

############################################################################

clean:
	rm -rf $(OUTPUT)
	$(MAKE) -C $(SRC_DIR)/src clean
	rm -f $(SRC_DIR)/Makefile
	rm -f $(SRC_DIR)/src/Makefile

romfs:
	$(ROMFSINST) $(OUTPUT)/bin/$(EXEC) /bin/$(EXEC)

romfs_user:



