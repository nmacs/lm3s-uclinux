
OBJS = smspdu.o

static: libsmspdu.a

libsmspdu.a: $(OBJS) smspdu.h
	$(AR) rcu $@ $(OBJS)
	$(RANLIB) $@

clean:
	rm -f libsmspdu.a $(OBJS)
