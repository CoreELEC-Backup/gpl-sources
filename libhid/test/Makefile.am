# AM_MAKEFLAGS = @MAKEFLAGS@
ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS =
AM_CFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/hidparser
AM_LDFLAGS =

noinst_PROGRAMS = test_libhid lshid
test_libhid_SOURCES = test_libhid.c
test_libhid_DEPENDENCIES = ../src/libhid.la
test_libhid_LDADD = ../src/libhid.la

lshid_SOURCES = lshid.c
lshid_DEPENDENCIES = ../src/libhid.la
lshid_LDADD = ../src/libhid.la

../src/libhid.la::
	@$(MAKE) -sC ../src libhid.la

../hidparser/libhidparser.la::
	@$(MAKE) -sC ../hidparser libhidparser.la

.PHONY: debug
debug: test_libhid
	libtool --mode=execute gdb -q $<

.PHONY: run
run: test_libhid
	libtool --mode=execute ./$<

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
