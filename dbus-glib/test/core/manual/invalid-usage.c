/* Manual test for various invalid usages which should not crash us (in order
 * to be nice to fallible programmers), unless checks have been disabled (in
 * which case, you asked for it, you got it).
 *
 * Copyright © 2006-2010 Red Hat, Inc.
 * Copyright © 2006-2010 Collabora Ltd.
 * Copyright © 2006-2011 Nokia Corporation
 * Copyright © 2006 Steve Frécinaux
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301  USA
 */

#include <config.h>

#include <glib.h>

#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>

#include <string.h>

#include "my-object.h"
#include "test-service-glib-bindings.h"

/* my-object wants this to exist */
GMainLoop *loop = NULL;

typedef struct {
    GError *error;
    DBusGConnection *conn;
    DBusGProxy *proxy;
    DBusGProxy *proxy_for_self;
    GObject *object;
} Fixture;

static void
setup (Fixture *f,
    gconstpointer context)
{
  /* this test is all about (mostly critical) warnings, so don't crash out on
   * programming errors */
  g_setenv ("DBUS_FATAL_WARNINGS", "0", TRUE);
  g_log_set_always_fatal (G_LOG_LEVEL_ERROR);

  dbus_g_type_specialized_init ();

  /* This is a bug: you're not meant to register any domain more than
   * once. It shouldn't crash, though. */
  dbus_g_error_domain_register (MY_OBJECT_ERROR, NULL, MY_TYPE_ERROR);

  f->conn = dbus_g_bus_get_private (DBUS_BUS_SESSION, NULL, &f->error);
  g_assert_no_error (f->error);
  g_assert (f->conn != NULL);

  f->proxy = dbus_g_proxy_new_for_name (f->conn, "com.example.Test",
      "/com/example/Test/Object", "com.example.Test.Fallible");
  g_assert (f->proxy != NULL);

  f->object = g_object_new (MY_TYPE_OBJECT, NULL);
  g_assert (MY_IS_OBJECT (f->object));
  dbus_g_connection_register_g_object (f->conn, "/com/example/Test/Object",
      f->object);

  f->proxy_for_self = dbus_g_proxy_new_for_name (f->conn,
      dbus_bus_get_unique_name (dbus_g_connection_get_connection (f->conn)),
      "/com/example/Test/Object", "org.freedesktop.DBus.GLib.Tests.MyObject");
  g_assert (f->proxy_for_self != NULL);
}

static void
test_invalid_gtype (Fixture *f,
    gconstpointer context)
{
  /* G_TYPE_GTYPE is not handled by the dbus-glib type system (and would make
   * no sense anyway) */
  dbus_g_proxy_call_no_reply (f->proxy, "Fail",
      G_TYPE_GTYPE, G_TYPE_STRING,
      G_TYPE_INVALID);
}

static void
test_invalid_utf8 (Fixture *f,
    gconstpointer context)
{
  g_test_bug ("30171");

  /* This provokes a libdbus warning, which is fatal-by-default */
  dbus_g_proxy_call_no_reply (f->proxy, "Fail",
      G_TYPE_STRING, "\xfe\xfe\xfe",
      G_TYPE_INVALID);
}

static void
test_invalid_bool (Fixture *f,
    gconstpointer context)
{
  g_test_bug ("30171");

  /* This provokes a libdbus warning, which is fatal-by-default */
  dbus_g_proxy_call_no_reply (f->proxy, "Fail",
      G_TYPE_BOOLEAN, (gboolean) (-42),
      G_TYPE_INVALID);
}

static void
test_invalid_path (Fixture *f,
    gconstpointer context)
{
  g_test_bug ("30171");

  /* This provokes a libdbus warning, which is fatal-by-default */
  dbus_g_proxy_call_no_reply (f->proxy, "Fail",
      DBUS_TYPE_G_OBJECT_PATH, "$%#*!",
      G_TYPE_INVALID);
}

static void
test_invalid_utf8s (Fixture *f,
    gconstpointer context)
{
  gchar *bad_strings[] = { "\xfe\xfe\xfe", NULL };
  GStrv bad_strv = bad_strings;

  g_test_bug ("30171");

  /* This provokes a libdbus warning, which is fatal-by-default */
  dbus_g_proxy_call_no_reply (f->proxy, "Fail",
      G_TYPE_STRV, bad_strv,
      G_TYPE_INVALID);
}

static void
test_invalid_bools (Fixture *f,
    gconstpointer context)
{
  GArray *array;
  gboolean maybe = (gboolean) (-23);

  g_test_bug ("30171");

  array = g_array_new (FALSE, FALSE, sizeof (gboolean));

  g_array_append_val (array, maybe);

  /* This provokes a libdbus warning, which is fatal-by-default */
  dbus_g_proxy_call_no_reply (f->proxy, "Fail",
      dbus_g_type_get_collection ("GArray", G_TYPE_BOOLEAN), array,
      G_TYPE_INVALID);

  g_array_free (array, TRUE);
}

static void
test_invalid_paths (Fixture *f,
    gconstpointer context)
{
  GPtrArray *array;

  g_test_bug ("30171");

  array = g_ptr_array_new ();
  g_ptr_array_add (array, "bees");

  /* This provokes a libdbus warning, which is fatal-by-default */
  dbus_g_proxy_call_no_reply (f->proxy, "Fail",
      dbus_g_type_get_collection ("GPtrArray", DBUS_TYPE_G_OBJECT_PATH), array,
      G_TYPE_INVALID);

  g_ptr_array_free (array, TRUE);
}

static void
throw_error_cb (DBusGProxy *proxy,
    GError *error,
    gpointer user_data)
{
  GError **error_out = user_data;

  g_assert (error != NULL);
  *error_out = g_error_copy (error);
}

static void
test_error_out_of_range (Fixture *f,
    gconstpointer context)
{
  GError *error = NULL;

  g_test_bug ("40151");

  /* This is a bug: -1 isn't a valid code for the domain. */
  my_object_save_error ((MyObject *) f->object, MY_OBJECT_ERROR, -1,
      "stop being so negative");

  if (!org_freedesktop_DBus_GLib_Tests_MyObject_throw_error_async (
        f->proxy_for_self, throw_error_cb, &error))
    g_error ("Failed to start async ThrowError call");

  while (error == NULL)
    g_main_context_iteration (NULL, TRUE);

  g_assert_error (error, DBUS_GERROR, DBUS_GERROR_REMOTE_EXCEPTION);
  g_clear_error (&error);

  /* This is a bug: 666 isn't a valid code for the domain. */
  my_object_save_error ((MyObject *) f->object, MY_OBJECT_ERROR, 666,
      "demonic possession detected");

  if (!org_freedesktop_DBus_GLib_Tests_MyObject_throw_error_async (
        f->proxy_for_self, throw_error_cb, &error))
    g_error ("Failed to start async ThrowError call");

  while (error == NULL)
    g_main_context_iteration (NULL, TRUE);

  g_assert_error (error, DBUS_GERROR, DBUS_GERROR_REMOTE_EXCEPTION);
  g_clear_error (&error);
}

static void
test_error_domain_0 (Fixture *f,
    gconstpointer context)
{
  /* This throws an error with domain 0 and code 0, which makes no sense.
   * It's programmer error, really: g_error_new() would critical if given
   * the same domain and code. See GNOME#660371.
   *
   * This was added for fd.o #27799, but there's a difference between
   * "this is an error domain, but not one registered with dbus-glib" and
   * "this isn't even an error domain". */
  g_test_bug ("27799");

  if (!org_freedesktop_DBus_GLib_Tests_MyObject_throw_unregistered_error_async (
        f->proxy_for_self, throw_error_cb, f))
    g_error ("Failed to start async ThrowUnregisteredError call");

  while (f->error == NULL)
    g_main_context_iteration (NULL, TRUE);

  g_assert_error (f->error, DBUS_GERROR, DBUS_GERROR_REMOTE_EXCEPTION);
}

static void
teardown (Fixture *f,
    gconstpointer context G_GNUC_UNUSED)
{
  g_clear_error (&f->error);

  if (f->proxy != NULL)
    {
      g_object_unref (f->proxy);
      f->proxy = NULL;
    }

  if (f->object != NULL)
    {
      g_object_unref (f->object);
      f->object = NULL;
    }

  if (f->proxy_for_self != NULL)
    {
      g_object_unref (f->proxy_for_self);
      f->proxy_for_self = NULL;
    }

  if (f->conn != NULL)
    {
      dbus_connection_close (dbus_g_connection_get_connection (f->conn));
      dbus_g_connection_unref (f->conn);
      f->conn = NULL;
    }
}

int
main (int argc,
    char **argv)
{
  g_test_init (&argc, &argv, NULL);
  g_test_bug_base ("https://bugs.freedesktop.org/show_bug.cgi?id=");

  g_type_init ();

  g_test_add ("/invalid/gtype", Fixture, NULL, setup, test_invalid_gtype,
      teardown);
  g_test_add ("/invalid/utf8", Fixture, NULL, setup, test_invalid_utf8,
      teardown);
  g_test_add ("/invalid/bool", Fixture, NULL, setup, test_invalid_bool,
      teardown);
  g_test_add ("/invalid/path", Fixture, NULL, setup, test_invalid_path,
      teardown);
  g_test_add ("/invalid/utf8s", Fixture, NULL, setup, test_invalid_utf8s,
      teardown);
  g_test_add ("/invalid/bools", Fixture, NULL, setup, test_invalid_bools,
      teardown);
  g_test_add ("/invalid/paths", Fixture, NULL, setup, test_invalid_paths,
      teardown);
  g_test_add ("/invalid/error/out-of-range", Fixture, NULL, setup,
      test_error_out_of_range, teardown);
  g_test_add ("/invalid/error/domain-0", Fixture, NULL, setup,
      test_error_domain_0, teardown);

  return g_test_run ();
}
