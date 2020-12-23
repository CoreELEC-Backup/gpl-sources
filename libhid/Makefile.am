ACLOCAL_AMFLAGS = -I m4

# Flags to pass to ./configure when calling "make distcheck":
DISTCHECK_CONFIGURE_FLAGS = --without-doxygen

if HAVE_SWIG
  MAYBE_PYTHONWRAPPER = swig
endif
SUBDIRS = hidparser src $(MAYBE_PYTHONWRAPPER) test doc

dist_noinst_HEADERS = \
	include/assert.h \
	include/constants.h \
	include/debug.h \
	include/hid_helpers.h \
	include/compiler.h \
	include/os.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = pkgconfig/libhid.pc

dist_noinst_DATA = \
	debian/changelog \
	debian/compat \
	debian/control \
	debian/copyright \
	debian/libhid-dev.docs \
	debian/libhid-dev.examples \
	debian/libhid-dev.install \
	debian/libhid-dev.lintian-overrides \
	debian/libhid-dev.linda-overrides \
	debian/libhid0.install \
	debian/python-hid.docs \
	debian/python-hid.examples \
	debian/python-hid.install \
	debian/rules \
	README.licence

clean-local:
	rm -f $(wildcard $(PACKAGE)-*.tar.gz)

include Makefile.debian

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

