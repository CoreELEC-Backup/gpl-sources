/* Feature test for exported object methods raising errors
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
#include <gio/gio.h>

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
    gchar *error_name;
    DBusGConnection *conn;
    DBusGProxy *proxy;
    GObject *object;
} Fixture;

#define assert_contains(haystack, needle) \
  assert_contains_impl (__FILE__, __LINE__, G_STRINGIFY (haystack), haystack, \
      G_STRINGIFY (needle), needle)

static void
assert_contains_impl (const gchar *file,
    gint line,
    const gchar *haystack_desc,
    const gchar *haystack,
    const gchar *needle_desc,
    const gchar *needle)
{
  if (G_UNLIKELY (strstr (haystack, needle) == NULL))
    {
      g_error ("%s:%d: assertion failed: (%s) contains (%s): "
          "values are \"%s\", \"%s\"",
          file, line, haystack_desc, needle_desc, haystack, needle);
    }
}

static void
setup (Fixture *f,
    gconstpointer context)
{
  static gsize once = 0;

  dbus_g_type_specialized_init ();

  if (g_once_init_enter (&once))
    {
      /* this may only be called once */
      dbus_g_error_domain_register (MY_OBJECT_ERROR, NULL, MY_TYPE_ERROR);

      g_once_init_leave (&once, 1);
    }

  f->conn = dbus_g_bus_get_private (DBUS_BUS_SESSION, NULL, &f->error);
  g_assert_no_error (f->error);
  g_assert (f->conn != NULL);

  f->object = g_object_new (MY_TYPE_OBJECT, NULL);
  g_assert (MY_IS_OBJECT (f->object));
  dbus_g_connection_register_g_object (f->conn, "/com/example/Test/Object",
      f->object);

  f->proxy = dbus_g_proxy_new_for_name (f->conn,
      dbus_bus_get_unique_name (dbus_g_connection_get_connection (f->conn)),
      "/com/example/Test/Object", "org.freedesktop.DBus.GLib.Tests.MyObject");
  g_assert (f->proxy != NULL);
}

static void
throw_error_cb (DBusGProxy *proxy,
    GError *error,
    gpointer user_data)
{
  Fixture *f = user_data;

  g_assert (error != NULL);
  g_clear_error (&f->error);
  g_free (f->error_name);
  f->error = g_error_copy (error);

  if (g_error_matches (error, DBUS_GERROR, DBUS_GERROR_REMOTE_EXCEPTION))
    f->error_name = g_strdup (dbus_g_error_get_name (error));
  else
    f->error_name = NULL;

  /* On error, this callback is expected to free it, which is pretty
   * astonishing, but is how it's always been. In principle, the generated
   * code ought to document this, or something.
   * https://bugs.freedesktop.org/show_bug.cgi?id=29195
   */
  g_error_free (error);
}

static void
test_async (Fixture *f,
    gconstpointer context)
{
  /* This is equivalent to test_simple but uses a method that's implemented
   * async at the service side - it's a different calling convention for the
   * service, but is indistinguishable here. */

  my_object_save_error ((MyObject *) f->object, MY_OBJECT_ERROR,
      MY_OBJECT_ERROR_FOO, "<foo>");

  if (!org_freedesktop_DBus_GLib_Tests_MyObject_async_throw_error_async (
        f->proxy, throw_error_cb, f))
    g_error ("Failed to start async AsyncThrowError call");

  while (f->error == NULL)
    g_main_context_iteration (NULL, TRUE);

  g_assert_error (f->error, DBUS_GERROR, DBUS_GERROR_REMOTE_EXCEPTION);
  g_assert_cmpstr (f->error_name, ==,
      "org.freedesktop.DBus.GLib.Tests.MyObject.Foo");
  assert_contains (f->error->message, "<foo>");
}

static void
test_simple (Fixture *f,
    gconstpointer context)
{
  my_object_save_error ((MyObject *) f->object, MY_OBJECT_ERROR,
      MY_OBJECT_ERROR_FOO, "<foo>");

  if (!org_freedesktop_DBus_GLib_Tests_MyObject_throw_error_async (
        f->proxy, throw_error_cb, f))
    g_error ("Failed to start async ThrowError call");

  while (f->error == NULL)
    g_main_context_iteration (NULL, TRUE);

  g_assert_error (f->error, DBUS_GERROR, DBUS_GERROR_REMOTE_EXCEPTION);
  g_assert_cmpstr (f->error_name, ==,
      "org.freedesktop.DBus.GLib.Tests.MyObject.Foo");
  assert_contains (f->error->message, "<foo>");
}

static void
test_builtin (Fixture *f,
    gconstpointer context)
{
  g_test_bug ("16776");

  my_object_save_error ((MyObject *) f->object, DBUS_GERROR,
      DBUS_GERROR_NOT_SUPPORTED, "<not supported>");

  if (!org_freedesktop_DBus_GLib_Tests_MyObject_throw_error_async (
        f->proxy, throw_error_cb, f))
    g_error ("Failed to start async ThrowError call");

  while (f->error == NULL)
    g_main_context_iteration (NULL, TRUE);

  g_assert_error (f->error, DBUS_GERROR, DBUS_GERROR_NOT_SUPPORTED);
  assert_contains (f->error->message, "<not supported>");
}

static void
test_multi_word (Fixture *f,
    gconstpointer context)
{
  /* no bug#, but this is a regression test for commit 3d69cfeab177e */

  my_object_save_error ((MyObject *) f->object, MY_OBJECT_ERROR,
      MY_OBJECT_ERROR_MULTI_WORD, "this method's error has a hyphen");

  if (!org_freedesktop_DBus_GLib_Tests_MyObject_throw_error_async (
        f->proxy, throw_error_cb, f))
    g_error ("Failed to start async ThrowError call");

  while (f->error == NULL)
    g_main_context_iteration (NULL, TRUE);

  g_assert_error (f->error, DBUS_GERROR, DBUS_GERROR_REMOTE_EXCEPTION);
  g_assert_cmpstr (f->error_name, ==,
      "org.freedesktop.DBus.GLib.Tests.MyObject.MultiWord");
  assert_contains (f->error->message, "this method's error has a hyphen");
}

static void
test_underscore (Fixture *f,
    gconstpointer context)
{
  g_test_bug ("30274");

  my_object_save_error ((MyObject *) f->object, MY_OBJECT_ERROR,
      MY_OBJECT_ERROR_UNDER_SCORE, "this method's error has an underscore");

  if (!org_freedesktop_DBus_GLib_Tests_MyObject_throw_error_async (
        f->proxy, throw_error_cb, f))
    g_error ("Failed to start async ThrowError call");

  while (f->error == NULL)
    g_main_context_iteration (NULL, TRUE);

  g_assert_error (f->error, DBUS_GERROR, DBUS_GERROR_REMOTE_EXCEPTION);
  g_assert_cmpstr (f->error_name, ==,
      "org.freedesktop.DBus.GLib.Tests.MyObject.Under_score");
  assert_contains (f->error->message, "this method's error has an underscore");
}

static void
test_unregistered (Fixture *f,
    gconstpointer context)
{
  g_test_bug ("27799");

  my_object_save_error ((MyObject *) f->object, G_IO_ERROR,
      G_IO_ERROR_NOT_INITIALIZED,
      "dbus-glib does not know about this error domain");

  if (!org_freedesktop_DBus_GLib_Tests_MyObject_throw_error_async (
        f->proxy, throw_error_cb, f))
    g_error ("Failed to start async ThrowError call");

  while (f->error == NULL)
    g_main_context_iteration (NULL, TRUE);

  g_assert_error (f->error, DBUS_GERROR, DBUS_GERROR_REMOTE_EXCEPTION);
  assert_contains (f->error->message,
      "dbus-glib does not know about this error domain");
}

static void
teardown (Fixture *f,
    gconstpointer context G_GNUC_UNUSED)
{
  g_free (f->error_name);

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

  g_test_add ("/error-mapping/async", Fixture, NULL, setup, test_async,
      teardown);
  g_test_add ("/error-mapping/builtin", Fixture, NULL, setup, test_builtin,
      teardown);
  g_test_add ("/error-mapping/multi-word", Fixture, NULL, setup,
      test_multi_word, teardown);
  g_test_add ("/error-mapping/simple", Fixture, NULL, setup, test_simple,
      teardown);
  g_test_add ("/error-mapping/underscore", Fixture, NULL, setup,
      test_underscore, teardown);
  g_test_add ("/error-mapping/unregistered", Fixture, NULL, setup,
      test_unregistered, teardown);

  return g_test_run ();
}
