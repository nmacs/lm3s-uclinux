# $Id: Makefile,v 1.36 2009/09/21 17:02:44 mascarenhas Exp $

OBJS= lsyslog.o

static: liblsyslog.a

liblsyslog.a: $(OBJS)
	$(AR) rcu $@ $(OBJS)
	$(RANLIB) $@

clean:
	rm -f liblsyslog.a $(OBJS)
