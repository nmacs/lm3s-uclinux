include $(REPODIR)/Rules.mk

content:
	$(MAKE) -C $(ROOTDIR)/user/devtools repo
	$(REPOINST) $(ROMFSDIR)/bin/ntpclient /bin/ntpclient
	$(REPOINST) $(ROMFSDIR)/bin/adjtimex /bin/adjtimex
