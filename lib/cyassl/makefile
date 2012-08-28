OUTPUT       := "$(ROOTDIR)/lib/cyassl/output"
CONFIG_OPTS  := --prefix=$(OUTPUT) --host=arm-linux

SRC_DIR := cyassl-2.3.0

.PHONY: all lib

all: lib

lib: $(SRC_DIR)/Makefile
	$(MAKE) -C $(SRC_DIR)
#	$(MAKE) -C $(SRC_DIR) install

$(SRC_DIR)/Makefile: makefile
	cd $(SRC_DIR); ./configure $(CONFIG_OPTS)

############################################################################

clean:
	$(MAKE) -C $(SRC_DIR) clean
	-rm -rf $(OUTPUT)
	-rm -f $(SRC_DIR)/Makefile

romfs:


romfs_user:



