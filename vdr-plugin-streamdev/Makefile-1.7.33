#
# Makefile for a Video Disk Recorder plugin
#
# $Id: Makefile,v 1.23 2010/08/02 10:36:59 schmirl Exp $

# The main source file name.
#
PLUGIN = streamdev

### The C/C++ compiler and options:

CC       ?= gcc
CFLAGS   ?= -g -O2 -Wall

CXX      ?= g++
CXXFLAGS ?= -g -O2 -Wall -Woverloaded-virtual -Wno-parentheses

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'const char \*VERSION *=' common.c | awk '{ print $$5 }' | sed -e 's/[";]//g')

### The directory environment:

VDRDIR = ../../..
LIBDIR = ../../lib
TMPDIR = /tmp

### The version number of VDR (taken from VDR's "config.h"):

APIVERSION = $(shell grep 'define APIVERSION ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')
APIVERSNUM = $(shell grep 'define APIVERSNUM ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')
TSPLAYVERSNUM = $(shell grep 'define TSPLAY_PATCH_VERSION ' $(VDRDIR)/device.h | awk '{ print $$3 }')

### Allow user defined options to overwrite defaults:

ifeq ($(shell test $(APIVERSNUM) -ge 10713; echo $$?),0)
include $(VDRDIR)/Make.global
else
ifeq ($(shell test $(APIVERSNUM) -ge 10704 -o -n "$(TSPLAYVERSNUM)" ; echo $$?),0)
DEFINES  += -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
CFLAGS   += -fPIC
CXXFLAGS += -fPIC
else
CFLAGS   += -fPIC
CXXFLAGS += -fPIC
endif
endif

-include $(VDRDIR)/Make.config

### export all vars for sub-makes, using absolute paths

VDRDIR := $(shell cd $(VDRDIR) >/dev/null 2>&1 && pwd)
LIBDIR := $(shell cd $(LIBDIR) >/dev/null 2>&1 && pwd)
export
unexport PLUGIN

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):

INCLUDES += -I$(VDRDIR)/include -I..

DEFINES += -D_GNU_SOURCE

ifdef DEBUG
DEFINES += -DDEBUG
endif
ifdef STREAMDEV_DEBUG
DEFINES += -DDEBUG
endif

### The main target:

.PHONY: all client server dist clean
all: client server

### Targets:

client:
	$(MAKE) -C ./tools
	$(MAKE) -C ./client
	# installs to $(LIBDIR)/libvdr-streamdev-client.so.$(APIVERSION)

server:
	$(MAKE) -C ./tools
	$(MAKE) -C ./libdvbmpeg
	$(MAKE) -C ./remux
	$(MAKE) -C ./server
	# installs to $(LIBDIR)/libvdr-streamdev-server.so.$(APIVERSION)

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz --exclude CVS -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	$(MAKE) -C ./tools clean
	$(MAKE) -C ./libdvbmpeg clean
	$(MAKE) -C ./remux clean
	$(MAKE) -C ./client clean
	$(MAKE) -C ./server clean
