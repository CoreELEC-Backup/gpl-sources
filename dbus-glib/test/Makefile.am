SUBDIRS = . core interfaces
DIST_SUBDIRS = core interfaces

AM_CPPFLAGS = \
	-I$(top_srcdir)				\
	-I$(top_builddir)			\
	-I$(top_builddir)/dbus			\
	$(DBUS_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(NULL)

LDADD = \
	$(top_builddir)/dbus/libdbus-glib-1.la \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(NULL)

if DBUS_BUILD_TESTS
test_programs = \
	test-errors \
	test-specialized-types \
	$(NULL)

# not actually tests, but used by tests
test_related_programs = \
	test-service \
	$(NULL)
endif

test_errors_SOURCES = errors.c
test_specialized_types_SOURCES = specialized-types.c

test_service_SOURCES=				\
	test-service.c

test_service_LDADD=$(DBUS_LIBS)

EXTRA_DIST = data/nested-introspect.xml test-compile-nested.sh

TESTS_ENVIRONMENT=top_builddir=$(top_builddir) srcdir=$(srcdir)

test_scripts = \
	test-compile-nested.sh \
	$(NULL)

TESTS = \
	$(test_programs) \
	$(test_scripts) \
	$(NULL)

noinst_PROGRAMS = \
	$(test_programs) \
	$(test_related_programs) \
	$(NULL)

check-valgrind: all
	$(MAKE) -C core check-valgrind
	$(MAKE) -C interfaces check-valgrind
