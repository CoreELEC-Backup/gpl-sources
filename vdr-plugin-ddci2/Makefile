##############################################################################
#
# Makefile for a Video Disk Recorder plugin
#
# If you have the VDR sources around and you would like to compile this plugin
# outside the VDR PLUGIN directory, then compile first VDR. Then compile this
# plugin with:
#   $ VDRDIR=<path-to-vdr-dir> INCLUDES='-I$(VDRDIR)/include' make
#
# Additionally you can set custom CXXFLAGS during development.
#   CXXFLAGS_CUSTOM='-save-temps -<another_opt>'
#     -save-temps .. keep preprocessor output
#
# Additional preprocessor macros, can be in DEFINES.
#   DEFINES='-D_DEBUG -DOPT_DEBUG=2'
#
# If you like to change the directory for temp files, you can set TMPDIR
#   TMPDIR=/opt/temp
#
# To generate the shared library without optimization, define DEBUG.
#
# All the above variables needs to be defined with "export var=..." (bash), or
# written before "make" (see '$' line at the beginning).  
#
##############################################################################

.DELETE_ON_ERROR:

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.

PLUGIN = ddci2

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'static const char \*VERSION *=' $(PLUGIN).cpp | awk '{ print $$6 }' | sed -e 's/[";]//g')

### The directory environment:

# Use package data if installed...otherwise assume we're under the VDR source directory:
PKGCFG = $(if $(VDRDIR),$(shell pkg-config --variable=$(1) $(VDRDIR)/vdr.pc),$(shell PKG_CONFIG_PATH="$$PKG_CONFIG_PATH:../../.." pkg-config --variable=$(1) vdr))
LIBDIR = $(call PKGCFG,libdir)
PLGCFG = $(call PKGCFG,plgcfg)

#
TMPDIR ?= /tmp

### The compiler options:
# You can use *_CUSTOM to add additional flags during development
CFLAGS_1   = $(call PKGCFG,cflags) $(CFLAGS_CUSTOM)
CXXFLAGS_1 = $(call PKGCFG,cxxflags) $(CXXFLAGS_CUSTOM)

# use no optimization for debug build 
ifneq ($(strip $(DEBUG)),)
CFLAGS_2   = $(filter-out -O3, $(CFLAGS_1)) -O0
CXXFLAGS_2 = $(filter-out -O3, $(CXXFLAGS_1)) -O0
else
CFLAGS_2   = $(CFLAGS_1)
CXXFLAGS_2 = $(CXXFLAGS_1)
endif  

export CFLAGS   = $(CFLAGS_2)
export CXXFLAGS = $(CXXFLAGS_2)

### The version number of VDR's plugin API:

APIVERSION = $(call PKGCFG,apiversion)

### Allow user defined options to overwrite defaults:

-include $(PLGCFG)

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### The name of the shared object file:

SOFILE = libvdr-$(PLUGIN).so

### Includes and Defines (add further entries here):

INCLUDES +=

DEFINES += -DPLUGIN_NAME_I18N='"$(PLUGIN)"'

### The source files (any found c/cpp file)
SRC_C = $(wildcard *.c)
SRC_CXX = $(wildcard *.cpp)
SRCS = $(SRC_C) $(SRC_CXX)

### The object files (derived from source files)
OBJS_C = $(SRC_C:%.c=%.o)
OBJS_CXX = $(SRC_CXX:%.cpp=%.o)
OBJS = $(OBJS_C) $(OBJS_CXX)

### The main target:

all: $(SOFILE)
	@true

### Implicit rules:

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c $(DEFINES) $(INCLUDES) -o $@ $<

### Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile $(SRCS)
	@$(MAKEDEP) $(CXXFLAGS) $(DEFINES) $(INCLUDES) $(SRCS) > $@

-include $(DEPFILE)

### Targets:

$(SOFILE): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared $(OBJS) -o $@

install-lib: $(SOFILE)
	install -D $^ $(DESTDIR)$(LIBDIR)/$^.$(APIVERSION)

install: install-lib

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz core* *~ *.ii *.s

