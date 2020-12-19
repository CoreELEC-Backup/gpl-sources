AM_CPPFLAGS = \
	-I$(top_srcdir)				\
	-I$(top_srcdir)/dbus			\
	-I$(top_builddir)			\
	-I$(top_builddir)/dbus			\
	$(DBUS_CFLAGS)				\
	$(GLIB_CFLAGS)				\
	-DDBUS_COMPILATION

LDADD = \
	$(GLIB_THREADS_LIBS) \
	$(GLIB_LIBS) \
	$(DBUS_LIBS) \
	$(top_builddir)/dbus/libdbus-glib-1.la \
	$(top_builddir)/dbus-gmain/tests/libtest.la \
	$(NULL)

tool_ldadd = \
	$(LDADD) \
	$(DBUS_GLIB_TOOL_LIBS) \
	$(top_builddir)/dbus/libdbus-gtool.la

## note that TESTS has special meaning (stuff to use in make check)
## so if adding tests not to be run in make check, don't add them to 
## TESTS
if DBUS_BUILD_TESTS
TESTS_ENVIRONMENT=DBUS_TOP_BUILDDIR=@abs_top_builddir@
TESTS=run-test.sh run-peer-test.sh
else
TESTS=
endif

VALGRIND_ENV = G_DEBUG=gc-friendly G_SLICE=always-malloc
VALGRIND = valgrind
VALGRIND_ARGS = \
	--gen-suppressions=all \
	--leak-check=full \
	--leak-resolution=high \
	--num-callers=20 \
	--suppressions=$(abs_top_srcdir)/tools/dbus-glib.supp \
	--suppressions=$(abs_top_srcdir)/tools/dbus-glib-tests.supp \
	--verbose \
	$(NULL)

check-valgrind: all
	$(MAKE) check-TESTS \
	DEBUG="env $(VALGRIND_ENV) $(VALGRIND) $(VALGRIND_ARGS)"

EXTRA_DIST = \
	run-peer-test.sh \
	run-test.sh \
	test-service-glib-subclass.xml \
	test-service-glib.xml \
	$(NULL)

if DBUS_BUILD_TESTS

## we use noinst_PROGRAMS not check_PROGRAMS for TESTS so that we
## build even when not doing "make check"
noinst_PROGRAMS = \
	test-dbus-glib \
	test-error-mapping \
	test-service-glib \
	test-profile \
	manual/test-invalid-usage \
	peer-server \
	peer-client \
	test-types \
	test-private \
	test-peer-on-bus \
	test-proxy-noc \
	test-proxy-peer \
	test-registrations \
	test-variant-recursion \
	test-gvariant \
	test-shared-bus \
	$(NULL)

# This "test" exercises invalid usage. It is deliberately not run in
# TESTS, because it's (by design) full of bugs.
manual_test_invalid_usage_SOURCES = \
	manual/invalid-usage.c \
	my-object.c \
	my-object.h \
	$(NULL)

test_private_SOURCES = \
	my-object.c \
	my-object.h \
	private.c

test_proxy_noc_SOURCES = \
	proxy-noc.c

test_proxy_peer_SOURCES = \
	my-object.c \
	my-object.h \
	proxy-peer.c

test_registrations_SOURCES = \
	my-object.c \
	my-object.h \
	my-object-subclass.c \
	my-object-subclass.h \
	registrations.c

test_dbus_glib_SOURCES=				\
	my-object.c \
	my-object.h \
	test-dbus-glib.c

test_dbus_glib_LDADD= $(tool_ldadd)

test_error_mapping_SOURCES = \
	my-object.c \
	my-object.h \
	error-mapping.c

test_variant_recursion_SOURCES=test-variant-recursion.c

test_variant_recursion_LDADD= $(tool_ldadd)

BUILT_SOURCES = \
	test-service-glib-bindings.h \
	test-service-glib-glue.h \
	test-service-glib-subclass-glue.h \
	$(NULL)

test_service_glib_SOURCES=				\
	my-object.c                             \
	my-object.h                             \
	my-object-subclass.c                    \
	my-object-subclass.h                    \
	test-service-glib.c 

test-service-glib-glue.h: test-service-glib.xml $(top_builddir)/dbus/dbus-binding-tool$(EXEEXT)
	$(DEBUG) $(DBUS_BINDING_TOOL) --prefix=my_object --mode=glib-server --output=test-service-glib-glue.h $(srcdir)/test-service-glib.xml

test-service-glib-subclass-glue.h: test-service-glib-subclass.xml $(top_builddir)/dbus/dbus-binding-tool$(EXEEXT)
	$(DEBUG) $(DBUS_BINDING_TOOL) --prefix=my_object_subclass --mode=glib-server --output=test-service-glib-subclass-glue.h $(srcdir)/test-service-glib-subclass.xml

test-service-glib-bindings.h: test-service-glib.xml $(top_builddir)/dbus/dbus-binding-tool$(EXEEXT)
	$(DEBUG) $(DBUS_BINDING_TOOL) --prefix=my_object --mode=glib-client --output=test-service-glib-bindings.h $(srcdir)/test-service-glib.xml

peer_server_SOURCES = \
	my-object.c                             \
	my-object.h                             \
	my-object-subclass.c                    \
	my-object-subclass.h                    \
	peer-server.c

peer_client_SOURCES = \
	peer-client.c

test_types_SOURCES = \
	test-types.c

test_gvariant_SOURCES = \
	test-gvariant.c

test_peer_on_bus_SOURCES = peer-on-bus.c

test_shared_bus_SOURCES = shared-bus.c

CLEANFILES = \
	$(BUILT_SOURCES) \
	run-with-tmp-session-bus.conf

else
### not building tests

noinst_PROGRAMS=test-profile

endif

test_profile_SOURCES=				\
	test-profile.c
