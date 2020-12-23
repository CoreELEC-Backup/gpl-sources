#!/usr/bin/python
#
# This is just an example. Please see the test_libhid.c file for more
# comments.
#

import sys

# allow it to run right out of the build dir
import os
libsdir = os.getcwd() + '/.libs'
if os.path.isdir(libsdir) and os.path.isfile(libsdir + '/_hid.so'):
  sys.path.insert(0, libsdir)

from hid import *

def main():
  hid_set_debug(HID_DEBUG_ALL)
  hid_set_debug_stream(sys.stderr)
  hid_set_usb_debug(0)

  ret = hid_init()
  if ret != HID_RET_SUCCESS:
    sys.stderr.write("hid_init failed with return code %d.\n" % ret)

  hid = hid_new_HIDInterface()
  matcher = HIDInterfaceMatcher()
  matcher.vendor_id = 0x06c2
  matcher.product_id = 0x0038

  ret = hid_force_open(hid, 0, matcher, 3)
  if ret != HID_RET_SUCCESS:
    sys.stderr.write("hid_force_open failed with return code %d.\n" % ret)

  ret = hid_write_identification(sys.stdout, hid);
  if ret != HID_RET_SUCCESS:
    sys.stderr.write("hid_write_identification failed with return code %d.\n" % ret)

  ret = hid_dump_tree(sys.stdout, hid);
  if ret != HID_RET_SUCCESS:
    sys.stderr.write("hid_dump_tree failed with return code %d.\n" % ret)

  ret = hid_close(hid)
  if ret != HID_RET_SUCCESS:
    sys.stderr.write("hid_close failed with return code %d.\n" % ret)

  hid_cleanup()

if __name__ == '__main__':
  main()

# COPYRIGHT --
#
# This file is part of libhid, a user-space HID access library.
# libhid is (c) 2003-2005
#   Martin F. Krafft <libhid@pobox.madduck.net>
#   Charles Lepple <clepple@ghz.cc>
#   Arnaud Quette <arnaud.quette@free.fr> && <arnaud.quette@mgeups.com>
# and distributed under the terms of the GNU General Public License.
# See the file ./COPYING in the source distribution for more information.
#
# THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES
# OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
