CFLAGS += -Wall -DWATCHDOG_SILENT
EXEC   =  watchdogd

all: $(EXEC)

romfs:
	$(ROMFSINST) -e CONFIG_USER_WATCHDOGD /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

.PHONY: all clean romfs
