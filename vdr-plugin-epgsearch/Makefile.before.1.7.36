#
# Makefile for epgsearch, a Video Disk Recorder plugin
#
# Christian Wieninger cwieninger at gmx.de
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

### The C++ compiler and options:

CXX      ?= g++
CXXFLAGS ?= -g -O2 -Wall -Woverloaded-virtual -Wno-parentheses -Wno-format-y2k

### The directory environment:

#DVBDIR = ../../../../DVB
VDRDIR = ../../..
LIBDIR = ../../lib
TMPDIR = /tmp

### auto configuring modules
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
endif

### Make sure that necessary options are included:
-include $(VDRDIR)/Make.global


### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config

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

### The version number of VDR (taken from VDR's "config.h"):

VDRVERSION = $(shell grep 'define VDRVERSION ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')
APIVERSION = $(shell grep 'define APIVERSION ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')
ifeq ($(strip $(APIVERSION)),)
   APIVERSION = $(VDRVERSION)
endif

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):

INCLUDES += -I$(VDRDIR)/include -I$(DVBDIR)/include
#INCLUDES += -I$(VDRDIR)/include

EPGSEARCH_DEFINES += -D_GNU_SOURCE
ifneq ($(SENDMAIL),)
EPGSEARCH_DEFINES += -DSENDMAIL='"$(SENDMAIL)"'
endif
DEFINES += $(EPGSEARCH_DEFINES)

### The object files (add further files here):

OBJS = afuzzy.o blacklist.o changrp.o confdloader.o conflictcheck.o conflictcheck_thread.o distance.o $(PLUGIN).o epgsearchcats.o epgsearchcfg.o epgsearchext.o epgsearchsetup.o  epgsearchsvdrp.o epgsearchtools.o mail.o md5.o menu_announcelist.o menu_blacklistedit.o menu_blacklists.o menu_commands.o menu_conflictcheck.o menu_deftimercheckmethod.o menu_dirselect.o menu_event.o menu_favorites.o menu_main.o menu_myedittimer.o menu_quicksearch.o menu_recsdone.o menu_search.o menu_searchactions.o menu_searchedit.o menu_searchresults.o menu_searchtemplate.o menu_switchtimers.o menu_templateedit.o menu_timersdone.o menu_whatson.o noannounce.o pending_notifications.o rcfile.o  recdone.o recstatus.o searchtimer_thread.o services.o switchtimer.o switchtimer_thread.o templatefile.o timer_thread.o timerdone.o timerstatus.o uservars.o varparser.o

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
### overwrite this with PLUGIN_EPGSEARCH_SEP_ITEMS=--- in your Make.config
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

all: $(ALL) i18n

### Implicit rules:

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) -DPLUGIN_NAME_I18N='"$(PLUGIN)"' $(INCLUDES) $<
mainmenushortcut.o: mainmenushortcut.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) -DPLUGIN_NAME_I18N='"$(MAINMENUSHORTCUT)"' $(INCLUDES) $<
epgsearchonly.o: epgsearchonly.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) -DPLUGIN_NAME_I18N='"$(PLUGIN2)"' $(INCLUDES) $<
conflictcheckonly.o: conflictcheckonly.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) -DPLUGIN_NAME_I18N='"$(PLUGIN3)"' $(INCLUDES) $<
quickepgsearch.o: quickepgsearch.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) -DPLUGIN_NAME_I18N='"$(PLUGIN4)"' $(INCLUDES) $<

# Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(CXXFLAGS) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) $(OBJS2:%.o=%.c) $(OBJS3:%.o=%.c) $(OBJS4:%.o=%.c)> $@

-include $(DEPFILE)

### Internationalization (I18N):

PODIR     = po
LOCALEDIR = $(VDRDIR)/locale
I18Npo    = $(wildcard $(PODIR)/*.po)
I18Nmsgs  = $(addprefix $(LOCALEDIR)/, $(addsuffix /LC_MESSAGES/vdr-$(PLUGIN).mo, $(notdir $(foreach file, $(I18Npo), $(basename $(file))))))
I18Npot   = $(PODIR)/$(PLUGIN).pot

%.mo: %.po
	msgfmt -c -o $@ $<

$(I18Npot): $(wildcard *.[ch])
	xgettext -C -cTRANSLATORS --no-wrap --no-location -k -ktr -ktrNOOP -kI18nTranslate --msgid-bugs-address='<cwieninger@gmx.de>' -o $@ `ls $^`

%.po: $(I18Npot)
	msgmerge -U --no-wrap --no-location --backup=none -q $@ $<
	@touch $@

$(I18Nmsgs): $(LOCALEDIR)/%/LC_MESSAGES/vdr-$(PLUGIN).mo: $(PODIR)/%.mo
	@mkdir -p $(dir $@)
	cp $< $@

.PHONY: i18n
i18n: $(I18Nmsgs) $(I18Npot)

### Targets:

libvdr-$(PLUGIN).so: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared $(OBJS) $(LIBS) -o $@
	@cp --remove-destination $@ $(LIBDIR)/$@.$(APIVERSION)

libvdr-$(PLUGIN2).so: $(OBJS2)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared $(OBJS2) $(LIBS2) -o $@
	@cp --remove-destination $@ $(LIBDIR)/$@.$(APIVERSION)

libvdr-$(PLUGIN3).so: $(OBJS3)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared $(OBJS3) $(LIBS3) -o $@
	@cp --remove-destination $@ $(LIBDIR)/$@.$(APIVERSION)

libvdr-$(PLUGIN4).so: $(OBJS4)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared $(OBJS4) $(LIBS4) -o $@
	@cp --remove-destination $@ $(LIBDIR)/$@.$(APIVERSION)

createcats: createcats.o Makefile
	$(CXX) $(CXXFLAGS) $(LDFLAGS) createcats.o -o $@

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

docs:
	@./docsrc2man.sh
	@./docsrc2html.sh
	@ln -sf ./doc/en/epgsearch.4.txt MANUAL
	@ln -sf ./doc/en/epgsearch.1.txt README
	@ln -sf ./doc/de/epgsearch.1.txt README.DE

install-doc: docs
	@mkdir -p $(MANDIR)/man1
	@mkdir -p $(MANDIR)/man4
	@mkdir -p $(MANDIR)/man5
	@mkdir -p $(MANDIR)/de/man1
	@mkdir -p $(MANDIR)/de/man5

	@cp man/en/*1.gz $(MANDIR)/man1/
	@cp man/en/*4.gz $(MANDIR)/man4/
	@cp man/en/*5.gz $(MANDIR)/man5/
	@cp man/de/*1.gz $(MANDIR)/de/man1/
	@cp man/de/*5.gz $(MANDIR)/de/man5/

clean:
	@-rm -f $(PODIR)/*.mo $(PODIR)/*.pot
	@-rm -f $(OBJS) $(OBJS2) $(OBJS3) $(OBJS4) $(DEPFILE) *.so *.tgz core* createcats createcats.o pod2*.tmp
	@-find . \( -name "*~" -o -name "#*#" \) -print0 | xargs -0r rm -f
	@-rm -rf doc html man
