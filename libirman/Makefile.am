AUTOMAKE_OPTIONS = subdir-objects -Wno-portability
ACLOCAL_AMFLAGS  = -I m4
AM_DISTCHECK_CONFIGURE_FLAGS = --disable-rpath

EXTRA_DIST = COPYING.lib TECHNICAL libirman.pc.in
CLEANFILES = $(DESTDIR)$(plugindir)/irman.so

DESTDIR         ?= $(CURDIR)

if BUILD_PLUGIN

install-data-hook:
	rm -f $(DESTDIR)$(plugindir)/irman.la
	rm -f $(DESTDIR)$(plugindir)/irman.a

uninstall-hook:
	rm -f $(DESTDIR)$(plugindir)/irman.so

plugindir       = $(shell pkg-config --variable=plugindir lirc-driver)
configdir       = $(shell pkg-config --variable=configdir lirc-driver)
plugindocsdir   = $(shell pkg-config --variable=plugindocs lirc-driver)

plugin_LTLIBRARIES      = irman.la
irman_la_SOURCES        = lirc-plugin/irman.c
irman_la_LDFLAGS        = -module -avoid-version -llirc_client -lirman
irman_la_CFLAGS         = -DPLUGINDOCS='"$(plugindocsdir)"'
irman_la_DEPENDENCIES   = libirman.la

dist_config_DATA        = lirc-plugin/irman.conf
dist_plugindocs_DATA    = lirc-plugin/irman.html

endif


dist_doc_DATA = README TECHNICAL

include_HEADERS = irman.h

dist_sysconf_DATA = irman.conf

pkgconfigdir = $(libdir)/pkgconfig
dist_pkgconfig_DATA = libirman.pc

bin_PROGRAMS = workmanir test_func test_io test_name
lib_LTLIBRARIES = libirman.la
noinst_PROGRAMS = test_io test_func test_name

if BUILD_SWTEST
bin_PROGRAMS += workmanir_sw
lib_LTLIBRARIES += libirman_sw.la
noinst_PROGRAMS += test_io_sw test_func_sw test_name_sw
endif

test_io_SOURCES = test_io.c
test_io_LDADD = libirman.la

workmanir_SOURCES = workmanir.c
workmanir_LDADD = libirman.la

test_func_SOURCES = test_func.c
test_func_LDADD = libirman.la

test_name_SOURCES = test_name.c
test_name_LDADD = libirman.la

libirman_la_SOURCES = chunk.c chunk.h hashtable.c hashtable.h irio.c \
			irfunc.c ircmd.c
libirman_la_CFLAGS = -DIR_SYSCONF_DIR=\"$(sysconfdir)\"

if BUILD_SWTEST
test_func_sw_SOURCES = test_func.c
test_func_sw_LDADD = libirman_sw.la

test_name_sw_SOURCES = test_name.c
test_name_sw_LDADD = libirman_sw.la

test_io_sw_SOURCES = test_io.c
test_io_sw_LDADD = libirman.la

workmanir_sw_SOURCES = workmanir.c
workmanir_sw_LDADD = libirman_sw.la

libirman_sw_la_SOURCES = chunk.c chunk.h hashtable.c hashtable.h \
			irfunc.c ircmd.c
libirman_sw_la_CFLAGS = -DIR_SOFTWARE_TEST
endif

