# $Id: Makefile,v 1.36 2009/09/21 17:02:44 mascarenhas Exp $

OBJS= uci.o

static: libucilua.a

libucilua.a: $(OBJS)
	$(AR) rcu $@ $(OBJS)
	$(RANLIB) $@

clean:
	rm -f libucilua.a $(OBJS)
