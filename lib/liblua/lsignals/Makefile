
OBJS= lsignals.o

static: liblsignals.a

liblsignals.a: $(OBJS)
	$(AR) rcu $@ $(OBJS)
	$(RANLIB) $@

clean:
	rm -f liblsignals.a $(OBJS)
