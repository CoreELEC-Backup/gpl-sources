#define HID_INTERNAL

#include <hid.h>
#include <hid_helpers.h>

#include <string.h>

#include <debug.h>
#include <assert.h>

static void hid_prepare_parse_path(HIDInterface* const hidif,
    int const path[], unsigned int const depth)
{
  ASSERT(hid_is_opened(hidif));
  ASSERT(hidif->hid_data);

  unsigned int i = 0;

  TRACE("preparing search path of depth %d for parse tree of USB device %s...",
      depth, hidif->id);
  for (; i < depth; ++i) {
    hidif->hid_data->Path.Node[i].UPage = path[i] >> 16;
    hidif->hid_data->Path.Node[i].Usage = path[i] & 0x0000ffff;
  }

  hidif->hid_data->Path.Size = depth;

  TRACE("search path prepared for parse tree of USB device %s.", hidif->id);
}

hid_return hid_init_parser(HIDInterface* const hidif)
{
  if (!hid_is_opened(hidif)) {
    ERROR("cannot initialise parser of unopened HIDinterface.");
    return HID_RET_DEVICE_NOT_OPENED;
  }
  
  ASSERT(!hidif->hid_parser);
  ASSERT(!hidif->hid_data);

  TRACE("initialising the HID parser for USB Device %s...", hidif->id);

  TRACE("allocating space for HIDData structure...");
  hidif->hid_data = (HIDData*)malloc(sizeof(HIDData));
  if (!hidif->hid_data) {
    ERROR("failed to allocate memory for HIDData structure of USB device %s.",
        hidif->id);
    return HID_RET_FAIL_ALLOC;
  }
  TRACE("successfully allocated memory for HIDData strcture.");

  TRACE("allocating space for HIDParser structure...");
  hidif->hid_parser = (HIDParser*)malloc(sizeof(HIDParser));
  if (!hidif->hid_parser) {
    ERROR("failed to allocate memory for HIDParser structure of USB device %s.",
        hidif->id);
    return HID_RET_FAIL_ALLOC;
  }
  TRACE("successfully allocated memory for HIDParser strcture.");

  NOTICE("successfully initialised the HID parser for USB Device %s.",
      hidif->id);
  
  return HID_RET_SUCCESS;
}

hid_return hid_prepare_parser(HIDInterface* const hidif)
{
  if (!hid_is_opened(hidif)) {
    ERROR("cannot prepare parser of unopened HIDinterface.");
    return HID_RET_DEVICE_NOT_OPENED;
  }
  ASSERT(hidif->hid_parser);
  
  TRACE("setting up the HID parser for USB device %s...", hidif->id);

  hid_reset_parser(hidif);

  TRACE("dumping the raw report descriptor");
  {
	  char buffer[160], tmp[10];
	  int i;

	  sprintf(buffer, "0x%03x: ", 0);
	  for(i=0; i<hidif->hid_parser->ReportDescSize; i++) {
		  if(!(i % 8)) {
			  if(i != 0) TRACE("%s", buffer);
			  sprintf(buffer, "0x%03x: ", i);
		  }
		  sprintf(tmp, "0x%02x ", (int)(hidif->hid_parser->ReportDesc[i]));
		  strcat(buffer, tmp);
	  }
	  TRACE("%s", buffer);
  }
  
  /* TODO: the return value here should be used, no? */
  TRACE("parsing the HID tree of USB device %s...", hidif->id);
  HIDParse(hidif->hid_parser, hidif->hid_data);

  NOTICE("successfully set up the HID parser for USB device %s.", hidif->id);

  return HID_RET_SUCCESS;
}

void hid_reset_parser(HIDInterface* const hidif)
{
  if (!hid_is_opened(hidif)) {
    ERROR("cannot prepare parser of unopened HIDinterface.");
    return;
  }
  ASSERT(hidif->hid_parser);
  
  TRACE("resetting the HID parser for USB device %s...", hidif->id);
  ResetParser(hidif->hid_parser);
}

hid_return hid_find_object(HIDInterface* const hidif,
    int const path[], unsigned int const depth)
{
  if (!hid_is_opened(hidif)) {
    ERROR("cannot prepare parser of unopened HIDinterface.");
    return HID_RET_DEVICE_NOT_OPENED;
  }
  ASSERT(hidif->hid_parser);
  ASSERT(hidif->hid_data);
  
  hid_prepare_parse_path(hidif, path, depth);

  if (FindObject(hidif->hid_parser, hidif->hid_data) == 1) {
    NOTICE("found requested item.");
    return HID_RET_SUCCESS;
  }

  byte const ITEMSIZE = sizeof("0xdeadbeef");
  char* buffer = (char*)malloc(depth * ITEMSIZE);
  hid_format_path(buffer, depth * ITEMSIZE, path, depth); 
  WARNING("can't find requested item %s of USB device %s.\n", buffer, hidif->id);
  free(buffer);
  
  return HID_RET_NOT_FOUND;
}

hid_return hid_extract_value(HIDInterface* const hidif,
    unsigned char *const buffer, double *const value)
{
  if (!hid_is_opened(hidif)) {
    ERROR("cannot extract value from unopened HIDinterface.");
    return HID_RET_DEVICE_NOT_OPENED;
  }
  ASSERT(hidif->hid_parser);
  ASSERT(hidif->hid_data);

  if (!buffer) {
    ERROR("cannot extract value from USB device %s into NULL raw buffer.",
        hidif->id);
    return HID_RET_INVALID_PARAMETER;
  }

  if (!value) {
    ERROR("cannot extract value from USB device %s into NULL value buffer.",
        hidif->id);
    return HID_RET_INVALID_PARAMETER;
  }
  
  TRACE("extracting data value...");

  /* Extract the data value */
  GetValue(buffer, hidif->hid_data);

  /* FIXME: unit conversion and exponent?! */
  *value = hidif->hid_data->Value;
	
  return HID_RET_SUCCESS;
}

hid_return hid_get_report_size(HIDInterface* const hidif,
    unsigned int const reportID, unsigned int const reportType,
    unsigned int *size)
{
  if (!hid_is_opened(hidif)) {
    ERROR("cannot get report size of unopened HIDinterface.");
    return HID_RET_DEVICE_NOT_OPENED;
  }
  ASSERT(hidif->hid_parser);
  ASSERT(hidif->hid_data);
  
  if (!size) {
    ERROR("cannot read report size from USB device %s into NULL size buffer.",
        hidif->id);
    return HID_RET_INVALID_PARAMETER;
  }
  
  /* FIXME: GetReportOffset has to be rewritten! */
  *size = *GetReportOffset(hidif->hid_parser, reportID, reportType);
	
  return HID_RET_SUCCESS;
}

hid_return hid_format_path(char* const buffer, unsigned int length,
    int const path[], unsigned int const depth)
{
  if (!buffer) {
    ERROR("cannot format path into NULL buffer.");
    return HID_RET_INVALID_PARAMETER;
  }

  byte const ITEMSIZE = sizeof("0xdeadbeef");
  unsigned int i = 0;

  TRACE("formatting device path...");
  for (; i < depth; ++i) {
    if (length < ITEMSIZE) {
      WARNING("not enough space in buffer to finish formatting of path.")
      return HID_RET_OUT_OF_SPACE;
    }
    snprintf(buffer + i * ITEMSIZE, ITEMSIZE + 1, "0x%08x.", path[i]);
    length -= ITEMSIZE;
  }
  buffer[i * ITEMSIZE - 1] = '\0';

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
