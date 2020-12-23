# AM_MAKEFLAGS = @MAKEFLAGS@
ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS =
AM_CFLAGS = @OS_CFLAGS@ -fPIC -I$(top_srcdir)/include -I$(top_srcdir)/hidparser
AM_LDFLAGS = @OS_LDFLAGS@

if OS_LINUX
OS_SUPPORT_SOURCE = linux.c
else
if OS_BSD
OS_SUPPORT_SOURCE = bsd.c
else
if OS_SOLARIS
OS_SUPPORT_SOURCE = bsd.c
else
if OS_DARWIN
OS_SUPPORT_SOURCE = darwin.c
AM_CFLAGS += -no-cpp-precomp
AM_LDFLAGS += -lIOKit -framework "CoreFoundation"
else
OS_SUPPORT =
endif
endif
endif
endif

lib_LTLIBRARIES = libhid.la
libhid_la_DEPENDENCIES = ../hidparser/libhidparser.la
libhid_la_LIBADD = -lusb ../hidparser/libhidparser.la
libhid_la_SOURCES = \
  $(OS_SUPPORT_SOURCE) \
	hid_initialisation.c \
	hid_preparation.c \
	hid_opening.c \
	hid_parsing.c \
	hid_exchange.c \
	hid_presentation.c \
	debug.c

bin_PROGRAMS = libhid-detach-device
libhid_detach_device_DEPENDENCIES = libhid.la
libhid_detach_device_LDADD = libhid.la
libhid_detach_device_SOURCES = libhid-detach-device.c

include_HEADERS = ../include/hid.h

../hidparser/libhidparser.la::
	@$(MAKE) -sC ../hidparser libhidparser.la

.PHONY: clean-local
clean-local:
	rm -f $(wildcard *.loT)
	rm -f $(wildcard *.i)
	rm -f $(wildcard *.s)

# COPYRIGHT --
#
# This file is part of libhid, a user-space HID access library.
# libhid is (c) 2003-2007
#   Martin F. Krafft <libhid@pobox.madduck.net>
#   Charles Lepple <clepple+libhid@ghz.cc>
#   Arnaud Quette <arnaud.quette@free.fr> && <arnaud.quette@mgeups.com>
# and distributed under the terms of the GNU General Public License.
# See the file ./COPYING in the source distribution for more information.
#
# THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES
# OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.

