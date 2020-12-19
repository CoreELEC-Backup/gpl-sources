# {{{ global flags

BUILT_SOURCES =
CLEANFILES =
EXTRA_DIST =
DISTCLEANFILES =

AM_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(OPENSSL_CFLAGS) \
	$(LIBCURL_CFLAGS) \
	-DG_LOG_DOMAIN=\"Mega\" \
	-I$(srcdir)/lib \
	-I$(srcdir)

# }}}
# {{{ libmega

noinst_LIBRARIES = libmega.a

libmega_a_SOURCES = \
	lib/sjson.gen.c \
	lib/sjson.h \
	lib/http.c \
	lib/http.h \
	lib/mega.c \
	lib/mega.h \
	lib/tools.c \
	lib/tools.h \
	lib/alloc.h \
	lib/mega.h

# }}}
# {{{ tools

EXTRA_DIST += \
	lib/sjson.c

LDADD = \
	libmega.a \
	$(GLIB_LIBS) \
	$(OPENSSL_LIBS) \
	$(LIBCURL_LIBS)

bin_PROGRAMS = megadf megadl megaget megals megamkdir megaput megareg megarm megacopy

megadf_SOURCES     = tools/df.c
megadl_SOURCES     = tools/dl.c
megaget_SOURCES    = tools/get.c
megals_SOURCES     = tools/ls.c
megamkdir_SOURCES  = tools/mkdir.c
megaput_SOURCES    = tools/put.c
megareg_SOURCES    = tools/reg.c
megarm_SOURCES     = tools/rm.c
megacopy_SOURCES   = tools/copy.c

# }}}
# {{{ docs

MAN1 = megadf megadl megaget megals megamkdir megaput megareg megarm megacopy
MAN5 = megarc
MAN7 = megatools

EXTRA_DIST += \
	docs/asciidoc.conf \
	docs/footer.txt \
	docs/remote-paths.txt \
	docs/basic-options.txt \
	docs/auth-options.txt \
	$(addprefix docs/,$(addsuffix .txt, $(MAN1) $(MAN5) $(MAN7)))

if ENABLE_DOCS

man1_MANS = $(addprefix docs/,$(addsuffix .1, $(MAN1)))
man5_MANS = $(addprefix docs/,$(addsuffix .5, $(MAN5)))
man7_MANS = $(addprefix docs/,$(addsuffix .7, $(MAN7)))

MANS_ALL = $(man1_MANS) $(man5_MANS) $(man7_MANS)

BUILT_SOURCES += $(MANS_ALL)
DISTCLEANFILES += $(MANS_ALL)
EXTRA_DIST += $(MANS_ALL)

%.1 %.5 %.7: %.txt docs/auth-options.txt docs/basic-options.txt docs/remote-paths.txt docs/footer.txt docs/asciidoc.conf
	$(AM_V_GEN)mkdir -p "$(top_builddir)/docs"
	$(AM_V_GEN)a2x --asciidoc-opts="-f $(top_srcdir)/docs/asciidoc.conf -o $@" --destination-dir $(top_builddir)/docs -f manpage $<
	@sed -i 's/\[FIXME: manual]/Megatools Manual/' $@
	@sed -i 's/\[FIXME: source]/megatools $(VERSION)/' $@

endif

# }}}
# {{{ more docs

EXTRA_DIST += LICENSE HACKING contrib/bash-completion/megatools

doc_DATA = LICENSE NEWS TODO README INSTALL HACKING

# }}}

export MAKEFLAGS += --no-print-directory -s
ACLOCAL_AMFLAGS = -I m4
