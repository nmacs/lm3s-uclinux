OBJS   := lcrypto.o
LIBS   := -lssl -lcrypto
WARN   := -Wall -pedantic
CFLAGS := $(CFLAGS) $(WARN)
LIB    := liblcrypto.a

.PHONY: static all clean install none linux bsd macosx

static: $(LIB)

$(LIB): $(OBJS)
	$(AR) rcu $@ $(OBJS)
	$(RANLIB) $@

clean:
	rm -f $(LIB) $(OBJS)

lcrypto.o: lcrypto.c lcrypto.h
