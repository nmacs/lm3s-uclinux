OBJS= lntp.o

static: liblntp.a

liblntp.a: $(OBJS)
	$(AR) rcu $@ $(OBJS)
	$(RANLIB) $@

clean:
	rm -f liblntp.a $(OBJS)
