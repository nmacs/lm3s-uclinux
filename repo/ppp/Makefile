include $(REPODIR)/Rules.mk

DIRS := etc bin
DIRS += etc/ppp
DIRS += etc/ppp/chat
DIRS += etc/ppp/peers

content:
	for i in $(DIRS); do \
		[ -d $(CONTENT)/$$i ] || mkdir -p $(CONTENT)/$$i; \
	done
	$(REPOINST) -s /tmp/ppp0.resolv /etc/ppp/ppp0.resolv
	$(REPOINST) $(ROMFSDIR)/bin/pppd /bin/pppd
	$(REPOINST) $(ROMFSDIR)/bin/chat /bin/chat
	$(REPOINST) -s /tmp/pppd-chat-gprs /etc/ppp/chat/gprs
	$(REPOINST) $(PRODUCTDIR)/pppd-peers-gprs /etc/ppp/peers/gprs