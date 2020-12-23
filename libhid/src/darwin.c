/*!@file 
 *@brief Mac OS X/Darwin-specific support routines.
 */
#define HID_INTERNAL

#include <hid.h>
#include <os.h>
#include <compiler.h>

#include <debug.h>
#include <assert.h>

hid_return hid_os_force_claim(HIDInterface* const hidif, int const interface,
    HIDInterfaceMatcher const* const matcher, unsigned short retries UNUSED)
{
  if (!hidif) {
    ERROR("cannot open NULL HIDInterface.");
    return HID_RET_INVALID_PARAMETER;
  }

  if (!hid_is_opened(hidif)) {
    ERROR("cannot force claim interface of unopened HIDInterface.");
    return HID_RET_DEVICE_ALREADY_OPENED;
  }

  if (!matcher) {
    ERROR("cannot match against NULL HIDInterfaceMatcher.");
    return HID_RET_INVALID_PARAMETER;
  }

  WARNING("code not tested on the Darwin platform!");
  TRACE("claiming USB device %s...", hidif->id);
#if 1
  if (usb_claim_interface(hidif->dev_handle, interface) < 0) {
    WARNING("failed to claim USB device %s...", hidif->id);
    /* return HID_RET_FAIL_CLAIM_IFACE; */
  }
#endif
  return HID_RET_SUCCESS;
}

/* COPYRIGHT --
 *
 * This file is part of libhid, a user-space HID access library.
 * libhid is (c) 2003-2005
 *   Martin F. Krafft <libhid@pobox.madduck.net>
 *   Charles Lepple <clepple@ghz.cc>
 *   Arnaud Quette <arnaud.quette@free.fr> && <arnaud.quette@mgeups.com>
 * and distributed under the terms of the GNU General Public License.
 * See the file ./COPYING in the source distribution for more information.
 *
 * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES
 * OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
