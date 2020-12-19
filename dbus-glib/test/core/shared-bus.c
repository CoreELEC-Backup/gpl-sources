/* Regression test for the shared bus instance.
 * This test is expected to "leak" the shared connection.
 *
 * Copyright © 2006-2010 Red Hat, Inc.
 * Copyright © 2006-2008 Collabora Ltd. <http://www.collabora.co.uk/>
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

#include <glib.h>

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>

GMainLoop *loop = NULL;

typedef struct {
    DBusGConnection *bus;
    DBusGConnection *priv;
    GError *error;
} Fixture;

static void
destroy_cb (DBusGProxy *proxy G_GNUC_UNUSED,
            gpointer user_data)
{
  gboolean *disconnected = user_data;

  *disconnected = TRUE;
}

static void
disconnect (DBusGConnection **bus)
{
  DBusGProxy *proxy;
  gboolean disconnected = FALSE;

  g_printerr ("Disconnecting... ");

  dbus_connection_set_exit_on_disconnect (dbus_g_connection_get_connection (*bus),
                                          FALSE);
  proxy = dbus_g_proxy_new_for_peer (*bus, "/",
                                     "org.freedesktop.DBus.Peer");
  g_signal_connect (G_OBJECT (proxy), "destroy", G_CALLBACK (destroy_cb),
                    &disconnected);

  dbus_connection_close (dbus_g_connection_get_connection (*bus));

  while (!disconnected)
    {
      g_printerr (".");
      g_main_context_iteration (NULL, TRUE);
    }

  g_signal_handlers_disconnect_by_func (proxy, destroy_cb, &disconnected);
  g_object_unref (proxy);
  dbus_g_connection_unref (*bus);
  *bus = NULL;

  g_printerr (" disconnected\n");
}

static void
setup (Fixture *f,
    gconstpointer test_data G_GNUC_UNUSED)
{
}

static void
teardown (Fixture *f,
    gconstpointer test_data G_GNUC_UNUSED)
{
  if (f->bus != NULL)
    dbus_g_connection_unref (f->bus);

  if (f->priv != NULL)
    disconnect (&f->priv);

  g_clear_error (&f->error);
  dbus_shutdown ();
}

static void
test_shared_bus (Fixture *f,
    gconstpointer test_data G_GNUC_UNUSED)
{
  f->bus = dbus_g_bus_get (DBUS_BUS_SESSION, &f->error);
  g_assert_no_error (f->error);
  g_assert (f->bus != NULL);
  dbus_connection_set_exit_on_disconnect (dbus_g_connection_get_connection (f->bus),
                                          FALSE);

  g_assert (f->bus == dbus_g_bus_get (DBUS_BUS_SESSION, NULL));
  g_assert (f->bus == dbus_g_bus_get (DBUS_BUS_SESSION, NULL));
  g_assert (f->bus == dbus_g_bus_get (DBUS_BUS_SESSION, NULL));

  f->priv = dbus_g_bus_get_private (DBUS_BUS_SESSION, NULL, &f->error);
  g_assert_no_error (f->error);
  g_assert (f->priv != NULL);
  g_assert (f->priv != f->bus);
  dbus_connection_set_exit_on_disconnect (dbus_g_connection_get_connection (f->priv),
                                          FALSE);
}

int
main (int argc, char **argv)
{
  g_type_init ();
  g_log_set_always_fatal (G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL);
  g_test_bug_base ("https://bugs.freedesktop.org/show_bug.cgi?id=");
  g_test_init (&argc, &argv, NULL);

  g_test_add ("/shared-bus", Fixture, NULL, setup, test_shared_bus,
      teardown);

  return g_test_run ();
}
