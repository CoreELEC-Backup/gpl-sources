/* Feature test for freedesktop.org #23633 - non-default main context
 *
 * Copyright © 2009 Collabora Ltd. <http://www.collabora.co.uk/>
 * Copyright © 2009-2011 Nokia Corporation
 *
 * In preparation for dbus-glib relicensing (if it ever happens), this file is
 * licensed under (at your option) either the AFL v2.1, the GPL v2 or later,
 * or an MIT/X11-style license:
 *
 * Licensed under the Academic Free License version 2.1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>

#include "my-object.h"

/* MyObject needs this, but it doesn't do anything useful... */
GMainLoop *loop = NULL;

typedef struct {
    GMainContext *context;
    DBusGConnection *bus;
    GObject *object;
    DBusGProxy *proxy;
    DBusGProxyCall *call;

    gsize in_flight;
} Fixture;

static void
call_cb (DBusGProxy *proxy,
    DBusGProxyCall *call,
    gpointer user_data)
{
  Fixture *f = user_data;

  g_assert (proxy == f->proxy);
  g_assert (call == f->call);

  f->in_flight--;
}

static void
frobnicate_cb (DBusGProxy *proxy,
    int arg,
    gpointer user_data)
{
  Fixture *f = user_data;

  g_assert (proxy == f->proxy);

  f->in_flight--;

  g_assert_cmpint (arg, ==, 42);
}

static void
setup (Fixture *f,
    gconstpointer path_to_use)
{
  f->context = g_main_context_new ();

  f->in_flight = 0;

  f->bus = dbus_g_bus_get_private (DBUS_BUS_SESSION, f->context, NULL);
  g_assert (f->bus != NULL);

  f->object = g_object_new (MY_TYPE_OBJECT, NULL);
  g_assert (MY_IS_OBJECT (f->object));
  dbus_g_connection_register_g_object (f->bus, "/object",
      (GObject *) f->object);

  f->proxy = dbus_g_proxy_new_for_name (f->bus,
      dbus_bus_get_unique_name (dbus_g_connection_get_connection (f->bus)),
      "/object", "org.freedesktop.DBus.GLib.Tests.MyObject");

  dbus_g_proxy_add_signal (f->proxy, "Frobnicate", G_TYPE_INT, G_TYPE_INVALID);
  dbus_g_proxy_connect_signal (f->proxy, "Frobnicate",
      G_CALLBACK (frobnicate_cb), f, NULL);
}

static void
teardown (Fixture *f,
    gconstpointer test_data G_GNUC_UNUSED)
{
  if (f->proxy != NULL)
    {
      dbus_g_proxy_disconnect_signal (f->proxy, "Frobnicate",
          G_CALLBACK (frobnicate_cb), f);

      g_object_unref (f->proxy);
    }

  if (f->object != NULL)
    {
      dbus_g_connection_unregister_g_object (f->bus, f->object);
      g_object_unref (f->object);
    }

  if (f->bus != NULL)
    {
      dbus_connection_close (dbus_g_connection_get_connection (f->bus));
      dbus_g_connection_unref (f->bus);
    }

  if (f->context != NULL)
    g_main_context_unref (f->context);
}

static void
test_call (Fixture *f,
    gconstpointer test_data G_GNUC_UNUSED)
{
  guint i, iterations;
  double t;

  if (g_test_perf ())
    iterations = 100000;
  else
    iterations = 1000;

  g_test_timer_start ();

  for (i = 0; i < iterations; i++)
    {
      GError *error = NULL;
      guint result;
      gboolean ok;

      f->in_flight++;
      f->call = dbus_g_proxy_begin_call (f->proxy, "Increment",
          call_cb, f, NULL,
          G_TYPE_UINT, 666,
          G_TYPE_INVALID);

      while (f->in_flight)
        g_main_context_iteration (f->context, TRUE);

      ok = dbus_g_proxy_end_call (f->proxy, f->call, &error,
            G_TYPE_UINT, &result,
            G_TYPE_INVALID);

      g_assert_no_error (error);
      g_assert (ok);
      g_assert_cmpuint (result, ==, 667);
    }

  t = g_test_timer_elapsed ();

  g_test_maximized_result (t / iterations,
      "%f seconds / %u iterations = %f s**-1", t, iterations, t / iterations);
}

static void
test_emit (Fixture *f,
    gconstpointer test_data G_GNUC_UNUSED)
{
  guint i, iterations;
  double t;

  if (g_test_perf ())
    iterations = 100000;
  else
    iterations = 1000;

  g_test_timer_start ();

  for (i = 0; i < iterations; i++)
    {
      f->in_flight++;

      my_object_emit_frobnicate (MY_OBJECT (f->object), NULL);

      while (f->in_flight)
        g_main_context_iteration (f->context, TRUE);
    }

  t = g_test_timer_elapsed ();

  g_test_maximized_result (t / iterations,
      "%f seconds / %u iterations = %f s**-1", t, iterations, t / iterations);
}

static void
test_timeout (Fixture *f,
    gconstpointer test_data G_GNUC_UNUSED)
{
  guint i;

  for (i = 0; i < 100; i++)
    {
      GError *error = NULL;
      guint result;
      gboolean ok;

      f->in_flight++;
      /* AsyncIncrement doesn't return until the default main context
       * runs, and we're not letting it run, so this will time out after 1
       * millisecond */
      f->call = dbus_g_proxy_begin_call_with_timeout (f->proxy,
          "AsyncIncrement",
          call_cb, f, NULL, 1,
          G_TYPE_UINT, 666,
          G_TYPE_INVALID);

      while (f->in_flight)
        g_main_context_iteration (f->context, TRUE);

      ok = dbus_g_proxy_end_call (f->proxy, f->call, &error,
            G_TYPE_UINT, &result,
            G_TYPE_INVALID);

      g_assert_error (error, DBUS_GERROR, DBUS_GERROR_NO_REPLY);
      g_assert (!ok);
      g_clear_error (&error);
    }

  /* drain the queue of idles with replies (which will be ignored)
   * just so we don't leak them */
  while (g_main_context_pending (NULL))
    g_main_context_iteration (NULL, FALSE);

  while (g_main_context_pending (f->context))
    g_main_context_iteration (f->context, FALSE);
}

int
main (int argc,
    char **argv)
{
  g_type_init ();
  dbus_g_type_specialized_init ();

  g_test_init (&argc, &argv, NULL);

  g_test_add ("/private/call", Fixture, NULL, setup, test_call, teardown);
  g_test_add ("/private/emit", Fixture, NULL, setup, test_emit, teardown);
  g_test_add ("/private/timeout", Fixture, NULL, setup, test_timeout,
      teardown);

  return g_test_run ();
}
