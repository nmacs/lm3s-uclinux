EXEC = gunzip
SRC_DIR = gzip124

CONFIGURE_OPTIONS += --host=arm-uclinux

CFLAGS += -DSMALL_MEM -DDYN_ALLOC -DMAXSEG_64K

all: gunzip

gunzip: configure
	$(MAKE) -C $(SRC_DIR)

.phony: configure
configure: $(SRC_DIR)/Makefile

$(SRC_DIR)/Makefile : Makefile
	cd $(SRC_DIR) && ./configure $(CONFIGURE_OPTIONS)

romfs:
	$(ROMFSINST) $(SRC_DIR)/$(EXEC) /bin/$(EXEC)

clean:
	$(MAKE) -C $(SRC_DIR) clean

