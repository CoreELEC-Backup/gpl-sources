#define HID_INTERNAL

#include <hid.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include <debug.h>

int main(int argc, char** argp)
{
  HIDInterface* hid;
  hid_return ret;
  char* const HEXPREFIX = "0x";
  unsigned char const HEXPREFIXLEN = 2;
  unsigned char const HEXSTRINGLEN = 6;
  unsigned char const HEXNUMLEN = 4;
  char vendor_id_s[HEXSTRINGLEN + 1], product_id_s[HEXSTRINGLEN + 1];
  unsigned short vendor_id = 0, product_id = 0;

  if (argc != 2 || strlen(argp[1]) != strlen("dead:beef")) {
    fprintf(stderr, "Usage: %s vendor_id:product_id\n", argp[0]);
    fprintf(stderr, "   Eg: %s 06c2:0038\n", argp[0]);
    return 255;
  }

  memcpy(vendor_id_s, HEXPREFIX, HEXPREFIXLEN);
  strncpy(vendor_id_s + HEXPREFIXLEN, argp[1], HEXNUMLEN);
  vendor_id = 0xffff & strtol(vendor_id_s, NULL, 16);

  memcpy(product_id_s, HEXPREFIX, HEXPREFIXLEN);
  strncpy(product_id_s + HEXPREFIXLEN, argp[1] + HEXNUMLEN + 1, HEXNUMLEN);
  product_id = 0xffff & strtol(product_id_s, NULL, 16);

  fprintf(stderr, "Trying to detach HID with IDs %04x:%04x... ",
      vendor_id, product_id);
    
  HIDInterfaceMatcher matcher = { vendor_id, product_id, NULL, NULL, 0 };

  ret = hid_init();
  if (ret != HID_RET_SUCCESS) {
    fprintf(stderr, "hid_init failed with return code %d.\n", ret);
    return 1;
  }

  hid = hid_new_HIDInterface();
  if (hid == 0) {
    fprintf(stderr, "hid_new_HIDInterface() failed, out of memory?\n");
    return 1;
  }

  ret = hid_force_open(hid, 0, &matcher, 3);
  if (ret != HID_RET_SUCCESS) {
    fprintf(stderr, "hid_force_open failed with return code %d.\n", ret);
    return 1;
  }

  ret = hid_close(hid);
  if (ret != HID_RET_SUCCESS) {
    fprintf(stderr, "hid_close failed with return code %d.\n", ret);
    return 1;
  }

  hid_delete_HIDInterface(&hid);

  ret = hid_cleanup();
  if (ret != HID_RET_SUCCESS) {
    fprintf(stderr, "hid_cleanup failed with return code %d.\n", ret);
    return 1;
  }

  fprintf(stderr, "done.\n");
  
  return 0;
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
