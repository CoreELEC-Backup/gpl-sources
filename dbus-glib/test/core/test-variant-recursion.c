#include <config.h>

/* -*- mode: C; c-file-style: "gnu" -*- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <glib-object.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>

#include "dbus-gmain/tests/util.h"

static gboolean
make_recursive_stringify_call (int recursion_depth, 
                               DBusGProxy *proxy, 
                               GError **error)
{
  gchar *out_str;
  gboolean ret;
  int i;
  GValue *val = g_new0 (GValue, 1);

  g_value_init (val, G_TYPE_STRING);
  g_value_set_string (val, "end of the line");

  for (i = 0; i < recursion_depth; i++)
    {
      GValue *tmp = g_new0 (GValue, 1);

      g_value_init (tmp, G_TYPE_VALUE);
      g_value_take_boxed (tmp, val);
      val = tmp;
    }

  ret = dbus_g_proxy_call (proxy, "Stringify", error,
                           G_TYPE_VALUE, val,
                           G_TYPE_INVALID,
                           G_TYPE_STRING, &out_str,
                           G_TYPE_INVALID);

  g_boxed_free (G_TYPE_VALUE, val);

  /* the out parameter is meaningless if it failed */
  if (ret)
    g_free (out_str);

  return ret;
}

int
main (int argc, char **argv)
{
  DBusGConnection *connection;
  GError *error = NULL;
  DBusGProxy *proxy;
  GMainLoop *loop;
    
  g_type_init ();

  g_log_set_always_fatal (G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL);
  
  loop = g_main_loop_new (NULL, FALSE);

  connection = dbus_g_bus_get_private (DBUS_BUS_SESSION, NULL, &error);
  if (connection == NULL)
    g_error ("Failed to open connection to bus: %s", error->message);

  proxy = dbus_g_proxy_new_for_name (connection,
                                     "org.freedesktop.DBus.GLib.TestService",
                                     "/org/freedesktop/DBus/GLib/Tests/MyTestObject",
                                     "org.freedesktop.DBus.GLib.Tests.MyObject");
  
  if (proxy == NULL)
    g_error ("Failed to create proxy for name owner: %s", error->message);
  
  /* Do an echo to be sure it started */
  if (!dbus_g_proxy_call (proxy, "DoNothing", &error,
			  G_TYPE_INVALID,
			  G_TYPE_INVALID))
    g_error ("Failed to complete DoNothing call: %s", error->message);

  /* Fewer than the current internal limit (16) */
  if (make_recursive_stringify_call (10, proxy, &error))
    g_error ("Unexpected success code from 10 recursive variant call: %s", error->message);
  if (error->code != DBUS_GERROR_REMOTE_EXCEPTION)
    g_error ("Error code was not remote exception: %s", error->message);
  g_printerr ("Got expected error %d: \"%s\" from recursive variant call\n", error->code, error->message);
  g_clear_error (&error);
  /* More than the current internal limit (16) */
  if (make_recursive_stringify_call (50, proxy, &error))
    g_error ("Unexpected success code from 50 recursive variant call: %s", error->message);
  if (error->code != DBUS_GERROR_REMOTE_EXCEPTION)
    g_error ("Error code was not remote exception: %s", error->message);
  g_printerr ("Got expected error %d: \"%s\" from recursive variant call\n", error->code, error->message);
  g_clear_error (&error);

  g_object_unref (G_OBJECT (proxy));

  test_run_until_disconnected (dbus_g_connection_get_connection (connection), NULL);
  dbus_g_connection_unref (connection);

  g_main_loop_unref (loop);
  dbus_shutdown ();

  return 0;
}
