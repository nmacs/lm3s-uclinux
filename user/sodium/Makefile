.PHONY: all
all: sodium
	

.PHONY: sodium
sodium: sodium.c
	$(CC) -o sodium $< $(CFLAGS) -lsodium

romfs:
	$(ROMFSINST) -d /bin/sodium