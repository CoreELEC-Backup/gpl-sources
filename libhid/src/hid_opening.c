#define HID_INTERNAL

#include <hid.h>
#include <hid_helpers.h>
#include <os.h>

#include <debug.h>
#include <assert.h>

enum USBMatchFlags {
  USB_MATCH_NONE = 0x0,
  USB_MATCH_VENDOR = 0x1,
  USB_MATCH_PRODUCT = 0x2,
  USB_MATCH_CUSTOM = 0x4,
  USB_MATCH_ALL = USB_MATCH_VENDOR | USB_MATCH_PRODUCT | USB_MATCH_CUSTOM
};

static unsigned int hid_compare_usb_device(struct usb_dev_handle const* dev_h,
    HIDInterfaceMatcher const* const match)
{
  ASSERT(dev_h);
  ASSERT(match);

  unsigned int ret = USB_MATCH_NONE;

  TRACE("comparing match specifications to USB device...");

  struct usb_device const* dev = usb_device((usb_dev_handle*)dev_h);
  
  TRACE("inspecting vendor ID...");
  if (dev->descriptor.idVendor > 0 &&
      (dev->descriptor.idVendor & match->vendor_id) == match->vendor_id) {
      TRACE("match on vendor ID: 0x%04x.", dev->descriptor.idVendor);
      ret |= USB_MATCH_VENDOR;
  }
  else TRACE("no match on vendor ID.");

  TRACE("inspecting product ID...");
  if ((dev->descriptor.idProduct & match->product_id) == match->product_id) {
      TRACE("match on product ID: 0x%04x.", dev->descriptor.idProduct);
      ret |= USB_MATCH_PRODUCT;
  }
  else TRACE("no match on product ID.");

  if (match->matcher_fn) {
    TRACE("calling custom matching function...");
    if ((*match->matcher_fn)(dev_h, match->custom_data, match->custom_data_length)) {
      TRACE("match on custom matching function.");
      ret |= USB_MATCH_CUSTOM;
    }
    else TRACE("no match on custom matching function.");
  }
  else {
    TRACE("no custom matching function supplied.");
    ret |= USB_MATCH_CUSTOM;
  }

  return ret;
}

static hid_return hid_find_usb_device(HIDInterface* const hidif,
    HIDInterfaceMatcher const* const match)
{
  ASSERT(!hid_is_opened(hidif));
  ASSERT(match);

  /* WARNING: not thread-safe. usb_busses in libusb/usb.c is a global
   * variable.
   */
  struct usb_bus *usbbus = usb_get_busses();
  struct usb_device *usbdev;

  TRACE("enumerating USB busses...");
  for (; usbbus; usbbus = usbbus->next) {

    TRACE("enumerating USB devices on bus %s...", usbbus->dirname);
    for (usbdev = usbbus->devices; usbdev; usbdev=usbdev->next) {

      snprintf(hidif->id, sizeof(hidif->id), "%s/%s[%d]",
          usbbus->dirname, usbdev->filename, hidif->interface);

      TRACE("inspecting USB device %s...", hidif->id);
      usb_dev_handle *usbdev_h = usb_open(usbdev);

      if (usbdev_h) {
        usb_claim_interface(usbdev_h, hidif->interface);

        unsigned int flags = hid_compare_usb_device(usbdev_h, match);
        if (flags == USB_MATCH_ALL) {
          NOTICE("found a matching USB device %s.", hidif->id);
          hidif->dev_handle = usbdev_h;
          hidif->device = usb_device(usbdev_h);
          return HID_RET_SUCCESS;
        }

        if (!(flags & USB_MATCH_VENDOR)) {
          NOTICE("vendor 0x%04x of USB device %s does not match 0x%04x.",
             usbdev->descriptor.idVendor, hidif->id, match->vendor_id);
        }
        else if (!(flags & USB_MATCH_PRODUCT)) {
          NOTICE("product 0x%04x of USB device %s does not match 0x%04x.",
             usbdev->descriptor.idProduct, hidif->id, match->product_id);
        }
        else if (!(flags & USB_MATCH_CUSTOM)) {
          NOTICE("custom matching function returned false on %s.", hidif->id);
        }
        //usb_release_interface(usbdev_h, hidif->interface);
        usb_close(usbdev_h);
      }
      else {
        ERROR("failed to open USB device %s", hidif->id);
        return HID_RET_FAIL_OPEN_DEVICE;
      }
    }
  }
  WARNING("no matching USB device found.");
  return HID_RET_DEVICE_NOT_FOUND;
}

static hid_return hid_get_usb_handle(HIDInterface* const hidif,
    HIDInterfaceMatcher const* const match)
{
  ASSERT(!hid_is_opened(hidif));
  ASSERT(match);

  TRACE("acquiring handle for a USB device...");
  
  hid_return ret = hid_find_usb_device(hidif, match);
  if (ret != HID_RET_SUCCESS) {
    hidif->dev_handle = NULL;
    hidif->device = NULL;
    return ret;
  }

  return HID_RET_SUCCESS;
}

hid_return hid_open(HIDInterface* const hidif, int const interface,
    HIDInterfaceMatcher const* const matcher)
{
  if (!hid_is_initialised()) {
    ERROR("cannot open HIDInterface when HID library has not been initialised.");
    return HID_RET_NOT_INITIALISED;
  }

  if (!hidif) {
    ERROR("cannot open NULL HIDInterface.");
    return HID_RET_INVALID_PARAMETER;
  }

  if (hid_is_opened(hidif)) {
    ERROR("cannot open already opened HIDInterface %s.", hidif->id);
    return HID_RET_DEVICE_ALREADY_OPENED;
  }

  if (!matcher) {
    ERROR("cannot match against NULL HIDInterfaceMatcher.");
    return HID_RET_INVALID_PARAMETER;
  }

  hidif->interface = interface;

  TRACE("opening a device interface according to matching criteria...");
  hid_return ret = hid_get_usb_handle(hidif, matcher);
  if (ret != HID_RET_SUCCESS) return ret;

  TRACE("claiming USB device %s.", hidif->id);
  if (usb_claim_interface(hidif->dev_handle, interface) < 0) {
    WARNING("failed to claim USB device %s.", hidif->id);
    hid_close(hidif);
    return HID_RET_FAIL_CLAIM_IFACE;
  }
  NOTICE("successfully claimed USB device %s.", hidif->id);

  ret = hid_prepare_interface(hidif);
  if (ret != HID_RET_SUCCESS) return ret;

  NOTICE("successfully opened USB device %s.", hidif->id);
  return HID_RET_SUCCESS;
}

hid_return hid_force_open(HIDInterface* const hidif, int const interface,
    HIDInterfaceMatcher const* const matcher, unsigned short retries)
{
  if (!hid_is_initialised()) {
    ERROR("cannot open HIDInterface when HID library has not been initialised.");
    return HID_RET_NOT_INITIALISED;
  }

  if (!hidif) {
    ERROR("cannot open NULL HIDInterface.");
    return HID_RET_INVALID_PARAMETER;
  }

  if (hid_is_opened(hidif)) {
    ERROR("cannot open already opened HIDInterface %s.", hidif->id);
    return HID_RET_DEVICE_ALREADY_OPENED;
  }

  if (!matcher) {
    ERROR("cannot match against NULL HIDInterfaceMatcher.");
    return HID_RET_INVALID_PARAMETER;
  }

  hidif->interface = interface;

  TRACE("forcefully opening a device interface "
        "according to matching criteria...");
  hid_return ret = hid_get_usb_handle(hidif, matcher);
  if (ret != HID_RET_SUCCESS) return ret;

  TRACE("claiming USB device %s.", hidif->id);
  ret = hid_os_force_claim(hidif, interface, matcher, retries);
  if (ret != HID_RET_SUCCESS) {
    WARNING("failed to claim USB device %s.", hidif->id);
    hid_close(hidif);
    return ret;
  }
  NOTICE("successfully claimed USB device %s.", hidif->id);
 
  ret = hid_prepare_interface(hidif);
  if (ret != HID_RET_SUCCESS) return ret;

  NOTICE("successfully opened USB device %s.", hidif->id);
  return HID_RET_SUCCESS;
}

hid_return hid_close(HIDInterface* const hidif)
{
  int ret = -1;

  if (hid_is_opened(hidif)) {

    TRACE("closing USB device %s...", hidif->id);

#if 0
    TRACE("releasing USB device %s...", hidif->id);
    if (usb_release_interface(hidif->dev_handle, hidif->interface) < 0)
      WARNING("failed to release USB device %s.", hidif->id);
#endif

    TRACE("closing handle of USB device %s...", hidif->id);
    if ((ret = usb_close(hidif->dev_handle)) < 0) {
      WARNING("failed to close USB device %s.", hidif->id);
    }
    else {
      NOTICE("successfully closed USB device %s.", hidif->id);
    }
  }
  else WARNING("attempt to close unopened USB device %s.", hidif->id);

  if (hidif->hid_parser) hid_reset_parser(hidif);
    
  TRACE("freeing memory allocated for HID parser...");
  if(hidif->hid_parser) free(hidif->hid_parser);
  if(hidif->hid_data) free(hidif->hid_data);
    
  TRACE("resetting HIDInterface...");
  hid_reset_HIDInterface(hidif);

  if (ret < 0) return HID_RET_FAIL_CLOSE_DEVICE;

  return HID_RET_SUCCESS;
}

bool hid_is_opened(HIDInterface const* hidif)
{
  if (!hidif) WARNING("attempt to query open status of NULL HIDInterface.");
  return hidif && hidif->dev_handle;
}

/* COPYRIGHT --
 *
 * This file is part of libhid, a user-space HID access library.
 * libhid is (c) 2003-2005
 *   Martin F. Krafft <libhid@pobox.madduck.net>
 *   Charles Lepple <clepple+libhid@ghz.cc>
 *   Arnaud Quette <arnaud.quette@free.fr> && <arnaud.quette@mgeups.com>
 * and distributed under the terms of the GNU General Public License.
 * See the file ./COPYING in the source distribution for more information.
 *
 * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES
 * OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
