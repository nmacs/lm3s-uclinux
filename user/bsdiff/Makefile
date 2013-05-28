CFLAGS		+=	-lbz2

ifdef CONFIG_USER_BSDIFF_BSDIFF
APPS		+=	bsdiff
endif

ifdef CONFIG_USER_BSDIFF_BSPATCH
APPS		+=	bspatch
endif

all:		$(APPS)

bsdiff:		bsdiff.c
	$(CC) $^ -o $@ $(CFLAGS)

bspatch:	bspatch.c
	$(CC) $^ -o $@ $(CFLAGS)

romfs:
	$(ROMFSINST) -e CONFIG_USER_BSDIFF_BSDIFF  -d /bin/bsdiff
	$(ROMFSINST) -e CONFIG_USER_BSDIFF_BSPATCH -d /bin/bspatch

clean:
	rm -f bsdiff bspatch *.gdb