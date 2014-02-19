OBJS= lcron.o

static: liblcron.a

liblcron.a: $(OBJS)
	$(AR) rcu $@ $(OBJS)
	$(RANLIB) $@

clean:
	rm -f liblcron.a $(OBJS)
