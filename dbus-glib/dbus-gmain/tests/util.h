/* Regression test utilities
 *
 * Copyright © 2009-2018 Collabora Ltd. <http://www.collabora.co.uk/>
 * Copyright © 2009-2011 Nokia Corporation
 *
 * Licensed under the Academic Free License version 2.1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#ifndef DBUS_GLIB_TEST_UTIL_H

#include <dbus/dbus.h>
#include <glib.h>

void test_run_until_disconnected (DBusConnection *connection,
                                  GMainContext *context);

#endif
