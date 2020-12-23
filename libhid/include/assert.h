#ifndef __INCLUDED_ASSERT_H__
#define __INCLUDED_ASSERT_H__

#ifndef HID_INTERNAL
#  error "this file is only supposed to be used from within libhid."
#endif /* HID_INTERNAL */

#include <hid.h>
#include <debug.h>

#define ASSERT(a) if (!(a) && hid_debug_stream && hid_debug_level & HID_DEBUG_ASSERTS) \
  fprintf(hid_debug_stream, "*** ASSERTION FAILURE in %s() [%s:%d]: %s\n", \
      __FUNCTION__, __FILE__, __LINE__, #a)

#endif /* __INCLUDED_DEBUG_H__ */

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
