include $(REPODIR)/Rules.mk

content:
	$(MAKE) -C $(ROOTDIR)/user/htpdate repo
	$(REPOINST) etc/init.d/clock /etc/init.d/clock
	$(REPOINST) -s /etc/init.d/clock /etc/rc.d/S11clock