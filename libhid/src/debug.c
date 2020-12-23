#define HID_INTERNAL

#include <debug.h>
#include <assert.h>

HIDDebugLevel hid_debug_level = HID_DEBUG_NONE;
FILE* hid_debug_stream = NULL;

void hid_set_debug(HIDDebugLevel const level)
{
  hid_debug_level = level;
}

void hid_set_debug_stream(FILE* const outstream)
{
  hid_debug_stream = outstream;
}

void hid_set_usb_debug(int const level)
{
  usb_set_debug(level);
}

struct usb_dev_handle;

void trace_usb_bus(FILE* out, struct usb_bus const* usbbus)
{
  fprintf(out, "usb_bus instance at: %10p\n", usbbus);
  fprintf(out, "  dirname:           %s\n", usbbus->dirname);
  fprintf(out, "  devices:           %10p\n", usbbus->devices);
  fprintf(out, "  prev:              %10p\n", usbbus->prev);
  fprintf(out, "  next:              %10p\n", usbbus->next);
}

void trace_usb_device(FILE* out, struct usb_device const* usbdev)
{
  fprintf(out, "usb_device instance at: %10p\n", usbdev);
  fprintf(out, "  prev:                 %10p\n", usbdev->prev);
  fprintf(out, "  next:                 %10p\n", usbdev->next);
  fprintf(out, "  filename:             %s\n", usbdev->filename);
  fprintf(out, "  bus:                  %10p\n", usbdev->bus);
  fprintf(out, "  descriptor:           %10p\n", &usbdev->descriptor);
  fprintf(out, "  config:               %10p\n", usbdev->config);
  fprintf(out, "  dev:                  %10p\n", usbdev->dev);
}

void trace_usb_device_descriptor(FILE* out, struct usb_device_descriptor const* descriptor)
{
  fprintf(out, "usb_device_descriptor instance at: %10p\n", descriptor);
  fprintf(out, "  bLength:                         %d\n", descriptor->bLength);
  fprintf(out, "  bDescriptorType:                 %d\n", descriptor->bDescriptorType); 
  fprintf(out, "  bcdUSB:                          0x%04x\n", descriptor->bcdUSB);
  fprintf(out, "  bDeviceClass:                    %d\n", descriptor->bDeviceClass);
  fprintf(out, "  bDeviceSubClass:                 %d\n", descriptor->bDeviceSubClass);
  fprintf(out, "  bDeviceProtocol:                 %d\n", descriptor->bDeviceProtocol);
  fprintf(out, "  bMaxPacketSize0:                 %d\n", descriptor->bMaxPacketSize0);
  fprintf(out, "  idVendor:                        0x%04x\n", descriptor->idVendor);
  fprintf(out, "  idProduct:                       0x%04x\n", descriptor->idProduct);
  fprintf(out, "  bcdDevice:                       0x%04x\n", descriptor->bcdDevice);
  fprintf(out, "  iManufacturer:                   %d\n", descriptor->iManufacturer);
  fprintf(out, "  iProduct:                        %d\n", descriptor->iProduct);
  fprintf(out, "  iSerialNumber:                   %d\n", descriptor->iSerialNumber);
  fprintf(out, "  bNumConfigurations:              %d\n", descriptor->bNumConfigurations);
}

void trace_usb_config_descriptor(FILE* out, struct usb_config_descriptor const* config)
{
  fprintf(out, "usb_config_descriptor instance at: %10p\n", config);
  fprintf(out, "  bLength:                         %d\n", config->bLength);
  fprintf(out, "  bDescriptorType:                 %d\n", config->bDescriptorType);
  fprintf(out, "  wTotalLength:                    %d\n", config->wTotalLength);
  fprintf(out, "  bNumInterfaces:                  %d\n", config->bNumInterfaces);
  fprintf(out, "  bConfigurationValue:             %d\n", config->bConfigurationValue);
  fprintf(out, "  iConfiguration:                  %d\n", config->iConfiguration);
  fprintf(out, "  bmAttributes:                    %d\n", config->bmAttributes);
  fprintf(out, "  MaxPower:                        %d mA\n", config->MaxPower * 2);
}

void trace_usb_dev_handle(FILE* out, usb_dev_handle const* usbdev_h)
{
  struct usb_device *device = usb_device((usb_dev_handle *)usbdev_h);
  trace_usb_device(out, device);
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
