OBJS= lwatchdog.o

static: liblwatchdog.a

liblwatchdog.a: $(OBJS)
	$(AR) rcu $@ $(OBJS)
	$(RANLIB) $@

clean:
	rm -f liblwatchdog.a $(OBJS)
