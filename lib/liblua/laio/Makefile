
OBJS= laio.o

static: liblaio.a

liblaio.a: $(OBJS)
	$(AR) rcu $@ $(OBJS)
	$(RANLIB) $@

clean:
	rm -f liblaio.a $(OBJS)
