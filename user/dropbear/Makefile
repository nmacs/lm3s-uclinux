DROPBEAR=dropbear-2012.55

prefix=/
exec_prefix=${prefix}
bindir=${exec_prefix}/bin
sbindir=${exec_prefix}/sbin

INSTALL=install
CFLAGS += -Ilibtomcrypt
CONF_OPTIONS = --host=arm --disable-zlib --disable-syslog --disable-shadow --disable-lastlog --disable-utmp --disable-utmpx --disable-wtmp --disable-wtmpx

# these are exported so that libtomcrypt's makefile will use them
# export CC
export CFLAGS
# export LD
# export RANLIB AR STRIP

.PHONY: all romfs dropbearkey-host dropbear clean

# all: dropbear dropbearkey dropbearconvert
all: $(DROPBEAR)/Makefile dropbear dropbearkey-host

romfs:
	$(ROMFSINST) -e CONFIG_USER_DROPBEAR_DROPBEAR $(DROPBEAR)/dropbear /bin/dropbear
	#$(ROMFSINST) -e CONFIG_USER_DROPBEAR_DROPBEAR /etc/dropbear_dss_host_key
	$(ROMFSINST) -e CONFIG_USER_DROPBEAR_DROPBEAR /etc/dropbear_rsa_host_key

dropbearkey-host:
	rm -f dropbear_rsa_host_key
	dropbearkey -t rsa -f dropbear_rsa_host_key
	#rm -f dropbear_dss_host_key
	#dropbearkey -t dss -f dropbear_dss_host_key

dropbear:
	$(MAKE) -C $(DROPBEAR) "PROGRAMS=dropbear dbclient"

$(DROPBEAR)/Makefile: Makefile
	cd $(DROPBEAR); ./configure $(CONF_OPTIONS)

clean:
	@if [ -f $(DROPBEAR)/Makefile ]; then \
		$(MAKE) -C $(DROPBEAR) clean; \
	fi
	-rm -f $(DROPBEAR)/Makefile
	-rm -f $(DROPBEAR)/dropbearkey-host $(DROPBEAR)/dropbear_dss_host_key $(DROPBEAR)/dropbear_rsa_host_key
	-rm -f $(DROPBEAR)/*.o $(DROPBEAR)/*.da $(DROPBEAR)/*.bb $(DROPBEAR)/*.bbg $(DROPBEAR)/*.prof $(DROPBEAR)/*.gdb
