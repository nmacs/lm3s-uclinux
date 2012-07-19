IN_LIBUSTL := true
export IN_LIBUSTL

CONFIG_OPTS  := --prefix=output --without-shared --with-static --without-bounds --without-demangler --without-libstdc++

SRC_DIR := ustl-1.6

#CXXFLAGS += -Os

all: $(SRC_DIR)/config.h
	$(MAKE) -C $(SRC_DIR)
	$(MAKE) -C $(SRC_DIR) install

$(SRC_DIR)/config.h: makefile
	cd $(SRC_DIR); ./configure $(CONFIG_OPTS)

############################################################################

clean:
	$(MAKE) -C $(SRC_DIR) clean

romfs:


romfs_user:



