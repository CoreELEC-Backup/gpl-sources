/*!@file
 *@brief Initialization routines for libhid structures.
 */
#define HID_INTERNAL

#include "config.h"
#include <hid.h>
#include <hid_helpers.h>

#include <debug.h>
#include <assert.h>

/*!@brief Programmers can check this with hid_is_initialized().
 */
static bool initialised = false;

HIDInterface* hid_new_HIDInterface()
{
  TRACE("creating a new HIDInterface instance...");

  HIDInterface* ret = (HIDInterface*)malloc(sizeof(HIDInterface));
  if (!ret) {
    ERROR("could not allocate memory for HIDInterface instance.");
    return 0;
  }

  hid_reset_HIDInterface(ret);
  return ret;
}

void hid_delete_HIDInterface(HIDInterface** const ixs)
{
  if (!ixs || !*ixs) {
    ERROR("cannot delete NULL HIDInterface.");
    return;
  }

  free(*ixs);
  *ixs = 0;
}

void hid_reset_HIDInterface(HIDInterface* const hidif)
{
  if (!hidif) {
    ERROR("cannot reset NULL HIDInterface.");
    return;
  }

  hidif->dev_handle = NULL;
  hidif->device = NULL;
  hidif->interface = -1;
  hidif->id[0] = '\0';
  hidif->hid_data = NULL;
  hidif->hid_parser = NULL;
}

/*!@brief Initialize libhid: scan for USB busses and devices using libusb.
 *
 * Call this routine before making any other libhid calls.
 *
 * @return HID_RET_SUCCESS if everything was properly initialized.
 */
hid_return hid_init()
{
  if (hid_is_initialised()) {
    ERROR("cannot initialised already initialised HID library");
    return HID_RET_ALREADY_INITIALISED;
  }

  /* Include version to make deciphering logfiles easier: */
  NOTICE(PACKAGE_STRING " is being initialized.");
  
  TRACE("initialising USB subsystem...");
  usb_init();

  TRACE("scanning for USB busses...");
  if (usb_find_busses() < 0) {
    ERROR("failed to scan for USB busses");
    return HID_RET_FAIL_FIND_BUSSES;
  }

  TRACE("scanning for USB devices...");
  if (usb_find_devices() < 0) {
    ERROR("failed to scan for USB devices");
    return HID_RET_FAIL_FIND_DEVICES;
  }

  initialised = true;

  NOTICE("successfully initialised HID library.");
  return HID_RET_SUCCESS;
}

/*!@brief Complement to hid_init(): cleans up after libhid.
 */
hid_return hid_cleanup()
{
  if (!hid_is_initialised()) {
    ERROR("cannot cleanup uninitialised HID library.");
    return HID_RET_NOT_INITIALISED;
  }

  initialised = false;
  NOTICE("successfully deinitialised HID library.");
  return HID_RET_SUCCESS;
}

/*!@brief Check to see that hid_init() has been called.
 * @return Non-zero if libhid has been initialized.
 */
bool hid_is_initialised()
{
  return initialised;
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
