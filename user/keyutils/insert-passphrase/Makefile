
CFLAGS += -I./include -I$(KEYUTILS)

insert-passphrase : insert_passphrase.o Makefile
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(KEYUTILS)/keyutils.o

clean:
	rm -f insert-passphrase *.o *.gdb