ifdef CONFIG_USER_BSDIFF_FEATURE_LZMA
CFLAGS		+=	-llzma -DENABLE_FEATURE_LZMA
HOST_CFLAGS	+=	-llzma -DENABLE_FEATURE_LZMA
endif
ifdef CONFIG_USER_BSDIFF_FEATURE_GZIP
CFLAGS		+=	-lz -DENABLE_FEATURE_GZIP
HOST_CFLAGS	+=	-lz -DENABLE_FEATURE_GZIP
endif
ifdef CONFIG_USER_BSDIFF_FEATURE_SMALL
CFLAGS		+=	-DSMALL_MEMORY
HOST_CFLAGS	+=	-DSMALL_MEMORY
endif

HOST_CC		:=	gcc
HOST_CFLAGS	+=	-O2

APPS		+= host_bsdiff host_bspatch
ifdef CONFIG_USER_BSDIFF_BSDIFF
APPS		+=	bsdiff
endif
ifdef CONFIG_USER_BSDIFF_BSPATCH
APPS		+=	bspatch
endif

ifdef CONFIG_LIB_ZLIB_MAXWBITS
	CFLAGS += -DMAX_WBITS=$(CONFIG_LIB_ZLIB_MAXWBITS)
	HOST_CFLAGS += -DMAX_WBITS=$(CONFIG_LIB_ZLIB_MAXWBITS)
endif

all:		$(APPS)

bsdiff:		goobsdiff.c
	$(CC) $^ -o $@ $(CFLAGS)

bspatch:	goobspatch.c
	$(CC) $^ -o $@ $(CFLAGS)

host_bsdiff:	goobsdiff.c
	$(HOST_CC) $^ -o $@ $(HOST_CFLAGS)

host_bspatch:	goobspatch.c
	$(HOST_CC) $^ -o $@ $(HOST_CFLAGS)

romfs:
	$(ROMFSINST) -e CONFIG_USER_BSDIFF_BSDIFF  -d /bin/bsdiff
	$(ROMFSINST) -e CONFIG_USER_BSDIFF_BSPATCH -d /bin/bspatch

clean:
	rm -f bsdiff bspatch *.gdb host_bsdiff host_bspatch