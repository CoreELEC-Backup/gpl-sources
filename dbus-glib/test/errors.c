/* Regression test for dbus-glib GError mapping
 *
 * Copyright © 2004 Red Hat, Inc.
 * Copyright © 2011 Nokia Corporation
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

#include <config.h>

#include <string.h>

#include <glib.h>

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>

typedef struct {
    int dummy;
} Fixture;

static void
setup (Fixture *f G_GNUC_UNUSED,
    gconstpointer context G_GNUC_UNUSED)
{
}

static void
test_errors (Fixture *f G_GNUC_UNUSED,
    gconstpointer context G_GNUC_UNUSED)
{
  DBusError err;
  GError *gerror = NULL;

  dbus_error_init (&err);
  dbus_set_error_const (&err, DBUS_ERROR_NO_MEMORY, "Out of memory!");

  dbus_set_g_error (&gerror, &err);
  g_assert (gerror != NULL);
  g_assert_error (gerror, DBUS_GERROR, DBUS_GERROR_NO_MEMORY);
  g_assert_cmpstr (gerror->message, ==, "Out of memory!");

  dbus_error_init (&err);
  g_clear_error (&gerror);
}

static void
teardown (Fixture *f G_GNUC_UNUSED,
    gconstpointer addr G_GNUC_UNUSED)
{
}

int
main (int argc,
    char **argv)
{
  g_test_init (&argc, &argv, NULL);
  g_type_init ();

  g_test_add ("/errors", Fixture, NULL, setup, test_errors, teardown);

  return g_test_run ();
}
