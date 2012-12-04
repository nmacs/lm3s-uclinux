EXEC = cli-static

VERBOSE             = "VERBOSE=1"
CMAKE_TOOLCHAIN     = -DCMAKE_TOOLCHAIN_FILE="../elsterlibs/CMake/Toolchains/arm-uclinuxeabi.cmake"
USTL_HOST_PATH      = "$(CURDIR)/ustl-1.6"

BUILD_DIR           = "./bin"

HOST_ARGS           = -DUNIX=1 -DHOST_BUILD=1 -DUSE_USTL=1 -DUSTL_PATH="$(USTL_HOST_PATH)/output/"

all: $(EXEC)

.PHONY: $(EXEC)
$(EXEC): configuration
	$(MAKE) $(VERBOSE) -C $(BUILD_DIR) $(EXEC)

.PHONY: configuration
configuration: $(BUILD_DIR)/Makefile

$(BUILD_DIR)/Makefile : Makefile
	-mkdir -p $(BUILD_DIR); cd $(BUILD_DIR); cmake -D__uClinux__=1 -DCMAKE_BUILD_TYPE=MinSizeRel -DUNIX=1 ..

.PHONY: romfs
romfs:
ifdef CONFIG_LIB_UCI_APP
	$(ROMFSINST) $(BUILD_DIR)/uci-static /bin/uci
endif

.PHONY: clean
clean:
	-rm -rf ./bin
