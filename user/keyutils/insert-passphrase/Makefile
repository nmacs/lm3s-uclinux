SPECFILE	:= keyutils.spec

###############################################################################
#
# Determine the current package version from the specfile
#
###############################################################################
vermajor	:= $(shell grep "%define vermajor" $(SPECFILE))
verminor	:= $(shell grep "%define verminor" $(SPECFILE))
MAJOR		:= $(word 3,$(vermajor))
MINOR		:= $(word 3,$(verminor))
VERSION		:= $(MAJOR).$(MINOR)

VCPPFLAGS	:= -DPKGBUILD="\"$(shell date -u +%F)\""
VCPPFLAGS	+= -DPKGVERSION="\"keyutils-$(VERSION)\""
VCPPFLAGS	+= -DAPIVERSION="\"libkeyutils-$(APIVERSION)\""

CFLAGS += -I./include -I./ $(VCPPFLAGS)

BUILD_DIR       ?= .

$(BUILD_DIR)/insert-passphrase : $(BUILD_DIR)/insert_passphrase.o $(BUILD_DIR)/keyutils.o Makefile
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(BUILD_DIR)/insert_passphrase.o $(BUILD_DIR)/keyutils.o

$(BUILD_DIR)/%.o : %.c
	$(CC) -c -o $@ $< $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(BUILD_DIR)insert-passphrase $(BUILD_DIR)/*.o $(BUILD_DIR)/*.gdb