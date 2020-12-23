#
# Makefile for epgsearch, a Video Disk Recorder plugin
#
# Christian Wieninger cwieninger at gmx.de
#
# Adapted to the new VDR makefile environment by Copperhead,
# refined by Stefan Hofmann
#

### ------------
### CONFIG START
###
### to change an option just edit the value: 0 => false, 1 => true


### edit one of these lines to '1', if you don't want the addon epgsearchonly, 
### conflictcheckonly or quickepgsearch

WITHOUT_EPGSEARCHONLY=0
WITHOUT_CONFLICTCHECKONLY=0
WITHOUT_QUICKSEARCH=0

### edit this to '0' if you don't want epgsearch to auto config itself
AUTOCONFIG=1

### if AUTOCONFIG is not active you can manually enable the
### optional modules or patches for other plugins
ifeq ($(AUTOCONFIG),0)
# if you want to use Perl compatible regular expressions (PCRE) or libtre for
# unlimited fuzzy searching, uncomment this and set the value to pcre or tre
# also have a look at INSTALL for further notes on this

#REGEXLIB = pcre

# uncomment this to enable support for the pin plugin.

#USE_PINPLUGIN = 1

# uncomment this to enable support for the graphtft plugin.
#USE_GRAPHTFT = 1

endif

### the sendmail executable to use when epgsearch is configured to use the
### sendmail method for sending mail
SENDMAIL = /usr/sbin/sendmail

###
### CONFIG END
### do not edit below this line if you don't know what you do ;-)
### -------------------------------------------------------------

PLUGIN = epgsearch
MAINMENUSHORTCUT = epgsearchmainmenushortcut
PLUGIN2 = epgsearchonly
PLUGIN3 = conflictcheckonly
PLUGIN4 = quickepgsearch

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'static const char VERSION\[\] *=' $(PLUGIN).c | awk '{ print $$6 }' | sed -e 's/[";]//g')

### The directory environment:

# Use package data if installed...otherwise assume we're under the VDR source directory:
#PKGCFG   = $(if $(VDRDIR),$(shell pkg-config --variable=$(1) $(VDRDIR)/vdr.pc),$(shell pkg-config --variable=$(1) vdr || pkg-config --variable=$(1) ../../../vdr.pc))
PKGCFG   = $(if $(VDRDIR),$(shell pkg-config --variable=$(1) $(VDRDIR)/vdr.pc),$(shell PKG_CONFIG_PATH="$$PKG_CONFIG_PATH:../../.." pkg-config --variable=$(1) vdr))
 
LIBDIR   = $(call PKGCFG,libdir)
LOCDIR   = $(call PKGCFG,locdir)
MANDIR   = $(call PKGCFG,mandir)
CONFDIR  = $(call PKGCFG,configdir)
BINDIR   = $(call PKGCFG,bindir)
#
TMPDIR ?= /tmp

PLGCFG = $(call PKGCFG,plgcfg)
-include $(PLGCFG)

### The compiler options:

export CFLAGS   = $(call PKGCFG,cflags)
export CXXFLAGS = $(call PKGCFG,cxxflags)

### configuring modules
ifeq ($(AUTOCONFIG),1)
	ifeq (exists, $(shell pkg-config libpcre && echo exists))
		REGEXLIB = pcre
	else ifeq (exists, $(shell pkg-config tre && echo exists))
		REGEXLIB = tre
	endif
	ifeq (exists, $(shell test -e ../pin && echo exists))
		USE_PINPLUGIN = 1
	endif
	ifeq (exists, $(shell test -e ../graphtft && echo exists))
		USE_GRAPHTFT = 1
	endif
	ifeq (exists, $(shell test -e ../graphtftng && echo exists))
		USE_GRAPHTFT = 1
	endif
endif

### The version number of VDR's plugin API:

APIVERSION = $(call PKGCFG,apiversion)

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):

INCLUDES +=

DEFINES += 

ifneq ($(SENDMAIL),)
DEFINES += -DSENDMAIL='"$(SENDMAIL)"'
endif

### The object files (add further files here):

ALL = libvdr-$(PLUGIN).so createcats
ifeq ($(WITHOUT_EPGSEARCHONLY), 0)
  ALL += libvdr-$(PLUGIN2).so
endif
ifeq ($(WITHOUT_CONFLICTCHECKONLY), 0)
  ALL += libvdr-$(PLUGIN3).so
endif
ifeq ($(WITHOUT_QUICKSEARCH), 0)
  ALL += libvdr-$(PLUGIN4).so
endif

OBJS = afuzzy.o blacklist.o changrp.o confdloader.o conflictcheck.o conflictcheck_thread.o distance.o $(PLUGIN).o epgsearchcats.o epgsearchcfg.o epgsearchext.o epgsearchsetup.o  epgsearchsvdrp.o epgsearchtools.o mail.o md5.o menu_announcelist.o menu_blacklistedit.o menu_blacklists.o menu_commands.o menu_conflictcheck.o menu_deftimercheckmethod.o menu_dirselect.o menu_event.o menu_favorites.o menu_main.o menu_myedittimer.o menu_quicksearch.o menu_recsdone.o menu_search.o menu_searchactions.o menu_searchedit.o menu_searchresults.o menu_searchtemplate.o menu_switchtimers.o menu_templateedit.o menu_timersdone.o menu_whatson.o noannounce.o pending_notifications.o rcfile.o  recdone.o recdone_thread.o recstatus.o searchtimer_thread.o services.o switchtimer.o switchtimer_thread.o templatefile.o timer_thread.o timerdone.o timerstatus.o uservars.o varparser.o

ifeq ($(REGEXLIB), pcre)
LIBS += $(shell pcre-config --libs-posix)
#LIBS += -L/usr/lib -lpcreposix -lpcre
INCLUDE += $(shell pcre-config --cflags)
DEFINES += -DHAVE_PCREPOSIX
else ifeq ($(REGEXLIB), tre)
LIBS += -L$(shell pkg-config --variable=libdir tre) $(shell pkg-config --libs tre)
#LIBS += -L/usr/lib -ltre
DEFINES += -DHAVE_LIBTRE
INCLUDE += $(shell pkg-config --cflags tre)
endif

ifdef USE_PINPLUGIN
DEFINES += -DUSE_PINPLUGIN
endif

ifdef USE_GRAPHTFT
DEFINES += -DUSE_GRAPHTFT
endif

ifdef CFLC
DEFINES += -DCFLC
endif

ifdef DEBUG_CONFL
DEFINES += -DDEBUG_CONFL
endif

ifdef PLUGIN_EPGSEARCH_MAX_SUBTITLE_LENGTH
DEFINES += -DMAX_SUBTITLE_LENGTH='$(PLUGIN_EPGSEARCH_MAX_SUBTITLE_LENGTH)'
endif

### length of the filling '-' in the channel separators, defaults to
### "----------------------------------------"
### overwrite this with PLUGIN_EPGSEARCH_SEP_ITEMS=---
### to avoid problems with graphlcd
ifdef PLUGIN_EPGSEARCH_SEP_ITEMS
DEFINES += -DMENU_SEPARATOR_ITEMS='"$(PLUGIN_EPGSEARCH_SEP_ITEMS)"'
endif

OBJS2    = mainmenushortcut.o epgsearchonly.o
LIBS2    =

OBJS3    = mainmenushortcut.o conflictcheckonly.o
LIBS3    =

OBJS4    = mainmenushortcut.o quickepgsearch.o
LIBS4    =

### The main target:

all: $(ALL) i18n docs

### Implicit rules:

%.o: %.c
	@echo CC $@
	$(Q)$(CXX) $(CXXFLAGS) -c $(DEFINES) -DPLUGIN_NAME_I18N='"$(PLUGIN)"' $(INCLUDES) -o $@ $<
mainmenushortcut.o: mainmenushortcut.c
	@echo CC $@
	$(Q)$(CXX) $(CXXFLAGS) -c $(DEFINES) -DPLUGIN_NAME_I18N='"$(MAINMENUSHORTCUT)"' $(INCLUDES) -o $@ $<
epgsearchonly.o: epgsearchonly.c
	@echo CC $@
	$(Q)$(CXX) $(CXXFLAGS) -c $(DEFINES) -DPLUGIN_NAME_I18N='"$(PLUGIN2)"' $(INCLUDES) -o $@ $<
conflictcheckonly.o: conflictcheckonly.c
	@echo CC $@
	$(Q)$(CXX) $(CXXFLAGS) -c $(DEFINES) -DPLUGIN_NAME_I18N='"$(PLUGIN3)"' $(INCLUDES) -o $@ $<
quickepgsearch.o: quickepgsearch.c
	@echo CC $@
	$(Q)$(CXX) $(CXXFLAGS) -c $(DEFINES) -DPLUGIN_NAME_I18N='"$(PLUGIN4)"' $(INCLUDES) -o $@ $<

# Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(CXXFLAGS) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) $(OBJS2:%.o=%.c) $(OBJS3:%.o=%.c) $(OBJS4:%.o=%.c)> $@

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPFILE)
endif

DEPFILE_DOC = .dependencies_doc
DEPFILE_stmp = .doc_stmp
$(DEPFILE_DOC): Makefile
	@rm -f $(DEPFILE_DOC)
	@./docsrc2man.sh --depend $(DEPFILE_stmp) > $(DEPFILE_DOC)

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPFILE_DOC)
endif

### Internationalization (I18N):

PODIR     = po
I18Npo    = $(wildcard $(PODIR)/*.po)
I18Nmo    = $(addsuffix .mo, $(foreach file, $(I18Npo), $(basename $(file))))
I18Nmsgs  = $(addprefix $(DESTDIR)$(LOCDIR)/, $(addsuffix /LC_MESSAGES/vdr-$(PLUGIN).mo, $(notdir $(foreach file, $(I18Npo), $(basename $(file))))))
I18Npot   = $(PODIR)/$(PLUGIN).pot

%.mo: %.po
	@echo MO $@
	$(Q)msgfmt -c -o $@ $<

$(I18Npot): $(wildcard *.c)
	@echo GT$@
	$(Q)xgettext -C -cTRANSLATORS --no-wrap --no-location -k -ktr -ktrNOOP --package-name=vdr-$(PLUGIN) --package-version=$(VERSION) --msgid-bugs-address='<see README>' -o $@ `ls $^`

%.po: $(I18Npot)
	@echo PO $@
	$(Q)msgmerge -U --no-wrap --no-location --backup=none -q -N $@ $<
	@touch $@

$(I18Nmsgs): $(DESTDIR)$(LOCDIR)/%/LC_MESSAGES/vdr-$(PLUGIN).mo: $(PODIR)/%.mo
	@echo IN $@
	$(Q)install -D -m644 $< $@

.PHONY: i18n
i18n: $(I18Nmo) $(I18Npot)

install-i18n: $(I18Nmsgs)

### Targets:

libvdr-$(PLUGIN).so: $(OBJS)
	@echo LD $@
	$(Q)$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared $(OBJS) $(LIBS) -o $@

libvdr-$(PLUGIN2).so: $(OBJS2)
	@echo LD $@
	$(Q)$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared $(OBJS2) $(LIBS2) -o $@

libvdr-$(PLUGIN3).so: $(OBJS3)
	@echo LD $@
	$(Q)$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared $(OBJS3) $(LIBS3) -o $@

libvdr-$(PLUGIN4).so: $(OBJS4)
	@echo LD $@
	$(Q)$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared $(OBJS4) $(LIBS4) -o $@

createcats: createcats.o Makefile
	@echo LD $@
	$(Q)$(CXX) $(CXXFLAGS) $(LDFLAGS) createcats.o -o $@

$(DEPFILE_stmp):
	./docsrc2man.sh
	./docsrc2html.sh
	ln -sf ./doc/en/epgsearch.4.txt MANUAL
	ln -sf ./doc/en/epgsearch.1.txt README
	ln -sf ./doc/de/epgsearch.1.txt README.DE
	@rm -f $(DEPFILE_stmp)
	@date > $(DEPFILE_stmp)

docs: $(DEPFILE_stmp)

install-$(PLUGIN): libvdr-$(PLUGIN).so
	@echo IN $@
	$(Q)install -D libvdr-$(PLUGIN).so $(DESTDIR)$(LIBDIR)/libvdr-$(PLUGIN).so.$(APIVERSION)

install-$(PLUGIN2): libvdr-$(PLUGIN2).so
	@echo IN $@
	$(Q)install -D libvdr-$(PLUGIN2).so $(DESTDIR)$(LIBDIR)/libvdr-$(PLUGIN2).so.$(APIVERSION)

install-$(PLUGIN3): libvdr-$(PLUGIN3).so
	@echo IN $@
	$(Q)install -D libvdr-$(PLUGIN3).so $(DESTDIR)$(LIBDIR)/libvdr-$(PLUGIN3).so.$(APIVERSION)

install-$(PLUGIN4): libvdr-$(PLUGIN4).so
	@echo IN $@
	$(Q)install -D libvdr-$(PLUGIN4).so $(DESTDIR)$(LIBDIR)/libvdr-$(PLUGIN4).so.$(APIVERSION)

install-conf:
	mkdir -p $(DESTDIR)$(CONFDIR)/plugins/$(PLUGIN)/conf.d
	cp -n conf/* $(DESTDIR)$(CONFDIR)/plugins/$(PLUGIN)

install-doc:
	mkdir -p $(DESTDIR)$(MANDIR)/man1
	mkdir -p $(DESTDIR)$(MANDIR)/man4
	mkdir -p $(DESTDIR)$(MANDIR)/man5
	mkdir -p $(DESTDIR)$(MANDIR)/de/man1
	mkdir -p $(DESTDIR)$(MANDIR)/de/man5
	cp man/en/*1.gz $(DESTDIR)$(MANDIR)/man1/
	cp man/en/*4.gz $(DESTDIR)$(MANDIR)/man4/
	cp man/en/*5.gz $(DESTDIR)$(MANDIR)/man5/
	cp man/de/*1.gz $(DESTDIR)$(MANDIR)/de/man1/
	cp man/de/*5.gz $(DESTDIR)$(MANDIR)/de/man5/

install-bin: createcats
	mkdir -p $(DESTDIR)$(BINDIR)
	cp createcats $(DESTDIR)$(BINDIR)

install: install-lib install-i18n install-conf install-doc install-bin

install-lib: install-$(PLUGIN) install-$(PLUGIN2) install-$(PLUGIN3) install-$(PLUGIN4)

dist: docs clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)/doc-src
	@-rm -rf $(TMPDIR)/$(ARCHIVE)/html
	@-rm -rf $(TMPDIR)/$(ARCHIVE)/docsrc2man.sh
	@-rm -rf $(TMPDIR)/$(ARCHIVE)/docsrc2html.sh
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@ln -sf README.git README
	@echo Distribution package created as $(PACKAGE).tgz

distfull: docs clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@ln -sf README.git README
	@echo complete distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(PODIR)/*.mo $(PODIR)/*.pot
	@-rm -f $(OBJS) $(OBJS2) $(OBJS3) $(OBJS4) $(DEPFILE) *.so *.tgz core* createcats createcats.o pod2*.tmp
	@-find . \( -name "*~" -o -name "#*#" \) -print0 | xargs -0r rm -f
	@-rm -rf doc html man
	@-rm -f MANUAL README README.DE
	@-rm -f $(DEPFILE_stmp) $(DEPFILE_DOC)
