/*
 *
 *  Connection Manager
 *
 *  Copyright (C) 2014  Intel Corporation. All rights reserved.
 *  Copyright (C) 2018 GlobalLogic. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stdio.h>

#include "tethering.h"

void __connmanctl_tethering_clients_list(DBusMessageIter *iter)
{
	DBusMessageIter array;
	char *addr = NULL;

	if (dbus_message_iter_get_arg_type(iter) != DBUS_TYPE_ARRAY)
		return;

	dbus_message_iter_recurse(iter, &array);
	while (dbus_message_iter_get_arg_type(&array) == DBUS_TYPE_STRING) {
		dbus_message_iter_get_basic(&array, &addr);

		fprintf(stdout, "%s", addr);

		if (dbus_message_iter_has_next(&array))
			fprintf(stdout, "\n");

		dbus_message_iter_next(&array);
	}

	dbus_message_iter_next(iter);
	if (dbus_message_iter_get_arg_type(iter) != DBUS_TYPE_ARRAY)
		return;

	dbus_message_iter_recurse(iter, &array);
	while (dbus_message_iter_get_arg_type(&array) == DBUS_TYPE_STRING) {
		dbus_message_iter_get_basic(&array, &addr);

		fprintf(stdout, "\n%s %s", "removed", addr);

		if (dbus_message_iter_has_next(&array))
			fprintf(stdout, "\n");

		dbus_message_iter_next(&array);
	}
}
