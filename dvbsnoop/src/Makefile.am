
bin_PROGRAMS = dvbsnoop

AM_CPPFLAGS = \
	-I$(top_srcdir)

EXTRA_DIST = \
	$(man_MANS)


SUBDIRS = \
	dvb_api \
	pes \
	ts \
	bitdata \
	datacarousel \
	tvanytime \
	testdata \
	descriptors \
	sections \
	private \
	ebu \
	net_ip \
	strings \
	misc


noinst_HEADERS = \
	dvbsnoop.h \
	version.h

dvbsnoop_SOURCES = \
	dvbsnoop.c


dvbsnoop_LDADD = \
	$(top_builddir)/src/dvb_api/libdvbapi.a \
	$(top_builddir)/src/pes/libpes.a \
	$(top_builddir)/src/sections/libsections.a \
	$(top_builddir)/src/ts/libts.a \
	$(top_builddir)/src/bitdata/libbitdata.a \
	$(top_builddir)/src/datacarousel/libdatacarousel.a \
	$(top_builddir)/src/tvanytime/libtvanytime.a \
	$(top_builddir)/src/testdata/libtestdata.a \
	$(top_builddir)/src/descriptors/libdescriptors.a \
	$(top_builddir)/src/private/libprivate.a \
	$(top_builddir)/src/private/default/libprivatedefault.a \
	$(top_builddir)/src/private/eicta.org/libprivateEictaORG.a \
	$(top_builddir)/src/private/nordig.org/libprivateNordigORG.a \
	$(top_builddir)/src/private/premiere.de/libprivatePremiereDE.a \
	$(top_builddir)/src/private/bskyb.uk/libprivateBskybUK.a \
	$(top_builddir)/src/ebu/libebu.a \
	$(top_builddir)/src/net_ip/libnetip.a \
	$(top_builddir)/src/strings/libstrings.a \
	$(top_builddir)/src/misc/libmisc.a 

dvbsnoop_LDADD += -lm

man_MANS = dvbsnoop.1

