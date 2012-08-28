EXEC = opkg-cl
OPKG = opkg-0.1.8

all: $(EXEC)

$(EXEC): .built

romfs:
	$(ROMFSINST) $(OPKG)/output/bin/$(EXEC) /bin/$(EXEC)

clean:
	$(MAKE) -C $(OPKG) clean
	-rm .built

.built:
	-cd $(OPKG) && ./build.sh
	-touch .built

