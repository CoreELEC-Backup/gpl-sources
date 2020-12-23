# AM_MAKEFLAGS = @MAKEFLAGS@
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = www man

if DOXYGEN
all-local: html
endif

EXTRA_DIST = frontpage.dox

DOXY_SOURCES = \
	$(wildcard ../src/*.c) \
	$(wildcard ../include/*.h) \
	$(wildcard ../hidparser/*.[ch]) \
	$(wildcard *.dox)

.stamp_doxygen: $(DOXY_SOURCES) doxygen.rc
	doxygen doxygen.rc
	@touch $@

html: .stamp_doxygen
	chmod -R a+rX html
	find html -type d -exec chmod 711 {} \;

clean-local:
	rm -f .stamp_doxygen
	rm -rf html

# COPYRIGHT --
#
# This file is part of libhid, a user-space HID access library.
# libhid is (c) 2003-2007
#   Martin F. Krafft <libhid@pobox.madduck.net>
#   Charles Lepple <clepple+libhid@ghz.cc>
#   Arnaud Quette <arnaud.quette@free.fr> && <arnaud.quette@mgeups.com>
# and distributed under the terms of the GNU General Public License.
# See the ./COPYING file in the source tree root for more information.
#
# THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES
# OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
