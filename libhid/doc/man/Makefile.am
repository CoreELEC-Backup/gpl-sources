ACLOCAL_AMFLAGS = -I m4

dist_man1_MANS = libhid-detach-device.1

XP = xsltproc -''-nonet
%.1: %.dbk
	$(XP) $(DB2MAN_FILE) $<

maintainerclean-local:
	rm -f ${wildcard *.[0-9]}
