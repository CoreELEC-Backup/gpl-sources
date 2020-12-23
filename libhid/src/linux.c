/*!@file 
 *@brief Linux-specific support routines.
 */
#define HID_INTERNAL

#include <hid.h>
#include <os.h>

#include <debug.h>
#include <assert.h>

/*!@brief Forcibly claim an interface from an attached kernel driver.
 *
 * This function is necessary in Linux because the kernel prefers that
 * applications and drivers claim a device's appropriate interface
 * before accessing it.  For libhid to successfully claim an interface
 * that has already been claimed by hiddev (for instance), libhid must
 * first tell the kernel interface to relinquish control over the
 * interface.  This may take several tries.
 *
 * If the return code is anything but #HID_RET_SUCCESS, there will
 * probably be more information in the kernel message buffer
 * (available from the "dmesg" command).
 */
hid_return hid_os_force_claim(HIDInterface* const hidif, int const interface,
    HIDInterfaceMatcher const* const matcher, unsigned short retries)
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

  int ret;

  while ((ret = usb_claim_interface(hidif->dev_handle, interface)) != 0
            && retries-- > 0) {

    TRACE("failed to claim USB device %s, trying %d more time(s)...",
        hidif->id, retries);
    
    TRACE("detaching kernel driver from USB device %s...", hidif->id);
    if (usb_detach_kernel_driver_np(hidif->dev_handle, interface) < 0) {
      WARNING("failed to detach kernel driver from USB device %s...",
          hidif->id);
      return HID_RET_FAIL_DETACH_DRIVER;
    }

    TRACE("trying again to claim USB device %s...", hidif->id);
  }

  if (ret == 0) return HID_RET_SUCCESS;
  return HID_RET_FAIL_CLAIM_IFACE;
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
