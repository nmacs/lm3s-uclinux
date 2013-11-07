OBJS   := context.o ssl.o
LIBS   := -lssl -lcrypto
WARN   := -Wall -pedantic
CFLAGS := $(CFLAGS) $(WARN)
LIB    := libluasec.a

.PHONY: all clean install none linux bsd macosx

static: $(LIB)

$(LIB): $(OBJS)
	$(AR) rcu $@ $(OBJS)
	$(RANLIB) $@

clean:
	rm -f $(LIB) $(OBJS)

context.o: context.c context.h
ssl.o: ssl.c ssl.h context.h context.c
