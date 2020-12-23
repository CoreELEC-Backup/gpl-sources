#
# Makefile for epgdata2xmltv
#

### The C++ compiler and options:

CXX      ?= g++
CXXFLAGS ?= -g -rdynamic -O2 -Wall -Wextra -Woverloaded-virtual -Wno-parentheses
PKG-CONFIG ?= pkg-config
STRIP ?= -s

### Includes and Defines (add further entries here):

PKG-LIBS += libxml-2.0 libxslt libexslt libcurl libzip libpcrecpp enca
PKG-INCLUDES += libxml-2.0 libxslt libexslt libcurl libzip libpcrecpp enca

DEFINES += -D_GNU_SOURCE 
DEFINES += -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
DEFINES += -D__STDC_CONSTANT_MACROS -D__USE_XOPEN_EXTENDED

INCLUDES += $(shell $(PKG-CONFIG) --cflags $(PKG-INCLUDES))
LIBS     += $(shell $(PKG-CONFIG) --libs --static $(PKG-LIBS)) 

INCLUDES += -I..

### directory environment

TMPDIR = /tmp

### install directory ###

INSTALL = $(DESTDIR)/usr/bin

### The object files (add further files here):

OBJS = epgdata2xmltv.o

### The main target:

all: epgdata2xmltv_xsl epgdata2xmltv

### Implicit rules:

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) $<

### Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.cpp) > $@

-include $(DEPFILE)

### Targets:

epgdata2xmltv_xsl: 
	echo -n "char xsl[]=\"" > epgdata2xmltv_xsl.h
	sed -e "s/\"/\\\\\"/g;" epgdata2xmltv.xsl | awk '{ printf("%s",$$0); }' >> epgdata2xmltv_xsl.h
	echo "\";" >> epgdata2xmltv_xsl.h

epgdata2xmltv: epgdata2xmltv_xsl $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LIBS) -o $@

install: epgdata2xmltv
	@mkdir -p $(INSTALL)
	@install $(STRIP) epgdata2xmltv $(INSTALL)	
	@mkdir -p $(DESTDIR)/var/lib/epgsources
	@mkdir -p /var/lib/epgsources/epgdata2xmltv-img
	@cp epgdata2xmltv.dist /var/lib/epgsources/epgdata2xmltv
	@chown -R vdr.vdr /var/lib/epgsources

dist: clean
	@-rm -rf $(TMPDIR)/epgdata2xmltv
	@mkdir $(TMPDIR)/epgdata2xmltv
	@cp -a *.cpp *.h *.xsl epgdata2xmltv.dist Makefile $(TMPDIR)/epgdata2xmltv
	@tar cfz epgdata2xmltv.tgz -C $(TMPDIR) epgdata2xmltv
	@-rm -rf $(TMPDIR)/epgdata2xmltv 

clean:
	@-rm -f $(OBJS) $(DEPFILE) *.*~ epgdata2xmltv epgdata2xmltv_xsl.h

distclean: clean
	@-rm -f *~ *.tgz
