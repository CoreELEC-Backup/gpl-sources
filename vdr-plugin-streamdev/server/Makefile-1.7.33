#
# Makefile for a Video Disk Recorder plugin
#
# $Id: Makefile,v 1.2 2010/07/19 13:49:31 schmirl Exp $

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN = streamdev-server

### Includes and Defines (add further entries here):

DEFINES += -DPLUGIN_NAME_I18N='"$(PLUGIN)"'

### The object files (add further files here):

COMMONOBJS = ../common.o

SERVEROBJS = $(PLUGIN).o \
	server.o component.o connection.o \
	componentVTP.o connectionVTP.o \
	componentHTTP.o connectionHTTP.o menuHTTP.o \
	componentIGMP.o connectionIGMP.o \
	streamer.o livestreamer.o livefilter.o recstreamer.o recplayer.o \
	menu.o suspend.o setup.o
	
### The main target:

.PHONY: all i18n clean
all: libvdr-$(PLUGIN).so i18n

### Implicit rules:

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) -o $@ $<

### Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies

$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(SERVEROBJS:%.o=%.c) $(COMMONOBJS:%.o=%.c) > $@

-include $(DEPFILE)

### Internationalization (I18N):

PODIR     = po
LOCALEDIR = $(VDRDIR)/locale
I18Npo    = $(wildcard $(PODIR)/*.po)
I18Nmsgs  = $(addprefix $(LOCALEDIR)/, $(addsuffix /LC_MESSAGES/vdr-$(PLUGIN).mo, $(notdir $(foreach file, $(I18Npo), $(basename $(file))))))
I18Npot   = $(PODIR)/$(PLUGIN).pot

%.mo: %.po
	msgfmt -c -o $@ $<

$(I18Npot): $(SERVEROBJS:%.o=%.c)
	xgettext -C -cTRANSLATORS --no-wrap --no-location -k -ktr -ktrNOOP --msgid-bugs-address='<http://www.vdr-developer.org/mantisbt/>' -o $@ $^

%.po: $(I18Npot)
	msgmerge -U --no-wrap --no-location --backup=none -q $@ $<
	@touch $@

$(I18Nmsgs): $(LOCALEDIR)/%/LC_MESSAGES/vdr-$(PLUGIN).mo: $(PODIR)/%.mo
	@mkdir -p $(dir $@)
	cp $< $@

i18n: $(I18Nmsgs)

### Targets:

libvdr-$(PLUGIN).so: $(SERVEROBJS) $(COMMONOBJS) \
	../tools/sockettools.a ../remux/remux.a ../libdvbmpeg/libdvbmpegtools.a

%.so: 
	$(CXX) $(CXXFLAGS) -shared $^ -o $@
	@cp --remove-destination $@ $(LIBDIR)/$@.$(APIVERSION)

clean:
	@-rm -f $(COMMONOBJS) $(SERVEROBJS) $(DEPFILE) $(PODIR)/*.mo $(PODIR)/*.pot *.so *.tgz core* *~
