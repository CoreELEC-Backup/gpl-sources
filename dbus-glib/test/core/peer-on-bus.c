/* Regression test for object registration and unregistration
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

#include <config.h>

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>

#include "dbus-gmain/tests/util.h"

GMainLoop *loop = NULL;

typedef struct {
    DBusGConnection *bus;
    DBusGConnection *bus2;
} Fixture;

static void
setup (Fixture *f,
    gconstpointer path_to_use)
{
  f->bus = dbus_g_bus_get_private (DBUS_BUS_SESSION, NULL, NULL);
  g_assert (f->bus != NULL);

  f->bus2 = dbus_g_bus_get_private (DBUS_BUS_SESSION, NULL, NULL);
  g_assert (f->bus2 != NULL);
}

static void
teardown (Fixture *f,
    gconstpointer test_data G_GNUC_UNUSED)
{
  if (f->bus != NULL)
    {
      test_run_until_disconnected (dbus_g_connection_get_connection (f->bus), NULL);
      dbus_g_connection_unref (f->bus);
    }

  if (f->bus2 != NULL)
    {
      test_run_until_disconnected (dbus_g_connection_get_connection (f->bus2), NULL);
      dbus_g_connection_unref (f->bus2);
    }

  dbus_shutdown ();
}

static void
destroy_cb (DBusGProxy *proxy G_GNUC_UNUSED,
            gpointer user_data)
{
  gboolean *destroyed = user_data;

  *destroyed = TRUE;
}

static void
test_name_owner_changed (Fixture *f,
    gconstpointer test_data G_GNUC_UNUSED)
{
  DBusGProxy *peer;
  DBusGProxy *named;
  gboolean destroyed = FALSE;

  g_test_bug ("41126");

  /* bus has a proxy for bus2... */
  named = dbus_g_proxy_new_for_name (f->bus,
      dbus_bus_get_unique_name (dbus_g_connection_get_connection (f->bus2)),
      "/", "org.freedesktop.DBus.Peer");
  /* ... and also a proxy for the peer (i.e. the dbus-daemon) */
  peer = dbus_g_proxy_new_for_peer (f->bus, "/", "org.freedesktop.DBus.Peer");

  g_signal_connect (G_OBJECT (named), "destroy", G_CALLBACK (destroy_cb),
                    &destroyed);

  /* Disconnect bus2, to provoke a NameOwnerChanged signal on bus */
  test_run_until_disconnected (dbus_g_connection_get_connection (f->bus2), NULL);
  dbus_g_connection_unref (f->bus2);
  f->bus2 = NULL;

  /* Wait for that NameOwnerChanged to be processed */
  while (!destroyed)
    g_main_context_iteration (NULL, TRUE);

  g_signal_handlers_disconnect_by_func (named, destroy_cb, &destroyed);

  /* The first part of the bug was that we'd never get here, because checking
   * whether 'peer' was affected by the NameOwnerChanged caused a NULL
   * dereference and segfault. If we get here, all is OK.
   *
   * The second part of the bug was that if the last proxy in existence was
   * for a peer, when it was unregistered there would be no owner_match_rules,
   * causing a crash. Unref named before peer, to exercise that. */

  g_object_unref (named);
  g_object_unref (peer);
}

int
main (int argc, char **argv)
{
  g_setenv ("DBUS_FATAL_WARNINGS", "1", TRUE);
  g_type_init ();
  g_log_set_always_fatal (G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL);
  dbus_g_type_specialized_init ();
  g_test_bug_base ("https://bugs.freedesktop.org/show_bug.cgi?id=");
  g_test_init (&argc, &argv, NULL);

  g_test_add ("/peer-on-bus/name-owner-changed", Fixture, NULL,
      setup, test_name_owner_changed, teardown);

  return g_test_run ();
}
