KEYUTILS_SRC = keyutils-1.5.5

all:
ifdef CONFIG_USER_KEYUTILS_KEYCTL
	$(MAKE) -C $(KEYUTILS_SRC) BUILDFOR=arm keyctl-static
endif

romfs:
ifdef CONFIG_USER_KEYUTILS_KEYCTL
	$(ROMFSINST) $(KEYUTILS_SRC)/keyctl-static /bin/keyctl
endif

clean:
	$(MAKE) -C $(KEYUTILS_SRC) clean