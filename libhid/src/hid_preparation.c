#define HID_INTERNAL

#include <hid.h>
#include <hid_helpers.h>
#include <constants.h>

#include <debug.h>
#include <assert.h>

/*! @todo This code does not seem to properly retrieve descriptors for devices
 * with multiple interfaces. We probably need to parse each interface a little
 * more to determine which endpoints we want to talk to with usb_control_msg
 * (EP1IN can't be right for everything).
 */
static hid_return hid_prepare_hid_descriptor(HIDInterface* const hidif)
{
  ASSERT(hid_is_opened(hidif));
  ASSERT(hidif->hid_parser);

  TRACE("initialising the HID descriptor for USB device %s...", hidif->id);

  /* TODO: BUFLEN seems to depend on the device, so we need to do something
   * about the following.
   */
  byte const BUFLEN = 9;
  byte buffer[BUFLEN];
  
  TRACE("retrieving HID descriptor for USB device %s...", hidif->id);
  int len = usb_control_msg(hidif->dev_handle,
      USB_ENDPOINT_IN+1,
      USB_REQ_GET_DESCRIPTOR,
      (USB_DT_HID << 8) + 0, hidif->interface,
      (char*)buffer, BUFLEN,
      USB_TIMEOUT);

  if (len < 0) {
    WARNING("failed to get HID descriptor for USB device %s:%s", hidif->id, usb_strerror());
    return HID_RET_NOT_HID_DEVICE;
  }

  if (len < BUFLEN) {
    WARNING("HID descriptor for USB device %s is too short; "
        "expected: %d bytes; got: %d bytes.\n", hidif->id, BUFLEN, len);
    return HID_RET_HID_DESC_SHORT;
  }

  /* TODO:
   * the constants 7 and 8 should be exported.
   */
  hidif->hid_parser->ReportDescSize = buffer[7] | (buffer[8] << 8);

  NOTICE("successfully initialised HID descriptor for USB device %s (%d bytes).",
      hidif->id, hidif->hid_parser->ReportDescSize);

  return HID_RET_SUCCESS;
}

static hid_return hid_prepare_report_descriptor(HIDInterface* const hidif)
{
  ASSERT(hid_is_opened(hidif));
  ASSERT(hidif->hid_parser);

  TRACE("initialising the report descriptor for USB device %s...", hidif->id);

  if (hidif->hid_parser->ReportDescSize > REPORT_DSC_SIZE) {
    ERROR("report descriptor size for USB device %s exceeds maximum size: "
        "%d > %d.\n", hidif->id, hidif->hid_parser->ReportDescSize,
        REPORT_DSC_SIZE);

    return HID_RET_REPORT_DESC_LONG;
  }

  TRACE("retrieving report descriptor for USB device %s...", hidif->id);
  int len = usb_control_msg(hidif->dev_handle,
      USB_ENDPOINT_IN+1,
      USB_REQ_GET_DESCRIPTOR,
      (USB_DT_REPORT << 8) + 0, hidif->interface,
      (char*)hidif->hid_parser->ReportDesc, hidif->hid_parser->ReportDescSize,
      USB_TIMEOUT);

  if (len < 0) {
    WARNING("failed to get report descriptor for USB device %s...", hidif->id);
    NOTICE("Error from libusb: %s", usb_strerror());
    return HID_RET_FAIL_GET_REPORT;
  }

  if (len < hidif->hid_parser->ReportDescSize) {
    WARNING("HID report descriptor for USB device %s is too short; "
        "expected: %d bytes; got: %d bytes.\n", hidif->id,
        hidif->hid_parser->ReportDescSize, len);
    return HID_RET_REPORT_DESC_SHORT;
  }

  NOTICE("successfully initialised report descriptor for USB device %s.",
      hidif->id);

  return HID_RET_SUCCESS;
}

hid_return hid_prepare_interface(HIDInterface* const hidif)
{
  if (!hid_is_opened(hidif)) {
    ERROR("cannot prepare unopened HIDinterface.");
    return HID_RET_DEVICE_NOT_OPENED;
  }
  
  hid_return ret = hid_init_parser(hidif);
  if (ret != HID_RET_SUCCESS) {
    hid_close(hidif);
    return ret;
  }

  ret = hid_prepare_hid_descriptor(hidif);
  if (ret != HID_RET_SUCCESS) {
    hid_close(hidif);
    return ret;
  }

  ret = hid_prepare_report_descriptor(hidif);
  if (ret != HID_RET_SUCCESS) {
    hid_close(hidif);
    return ret;
  }

  ret = hid_prepare_parser(hidif);
  if (ret != HID_RET_SUCCESS) {
    hid_close(hidif);
    return ret;
  }

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
