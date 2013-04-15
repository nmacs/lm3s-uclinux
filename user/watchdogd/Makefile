CFLAGS += -Wall -DWATCHDOG_SILENT
EXEC   =  watchdogd

all: $(EXEC)

romfs:
	$(ROMFSINST) -d -e CONFIG_USER_WATCHDOGD /bin/$(EXEC)
	$(ROMFSINST) -d -e CONFIG_USER_WATCHDOGD etc/init.d/watchdog /etc/init.d/watchdog
	$(ROMFSINST) -d -e CONFIG_USER_WATCHDOGD -s /etc/init.d/watchdog /etc/rc.d/S00watchdog

repo:
	$(REPOINST) -e CONFIG_USER_WATCHDOGD /bin/$(EXEC)
	$(REPOINST) -e CONFIG_USER_WATCHDOGD etc/init.d/watchdog /etc/init.d/watchdog
	$(REPOINST) -e CONFIG_USER_WATCHDOGD -s /etc/init.d/watchdog /etc/rc.d/S00watchdog

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

.PHONY: all clean romfs
