KEYUTILS_SRC = keyutils-1.5.5
INSERT_PASSPHRASE_SRC = insert-passphrase

all:
	$(MAKE) -C $(KEYUTILS_SRC) BUILDFOR=arm keyctl-static
ifdef CONFIG_USER_KEYUTILS_INSERT_PASSPHRASE
	$(MAKE) -C $(INSERT_PASSPHRASE_SRC) KEYUTILS="$(CURDIR)/$(KEYUTILS_SRC)"
endif

romfs:
ifdef CONFIG_USER_KEYUTILS_KEYCTL
	$(ROMFSINST) $(KEYUTILS_SRC)/keyctl-static /bin/keyctl
endif
ifdef CONFIG_USER_KEYUTILS_INSERT_PASSPHRASE
	$(ROMFSINST) $(INSERT_PASSPHRASE_SRC)/insert-passphrase /bin/insert-passphrase
endif

clean:
	$(MAKE) -C $(KEYUTILS_SRC) clean
	$(MAKE) -C $(INSERT_PASSPHRASE_SRC) clean