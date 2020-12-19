/* Regression tests for DBusGProxy's NameOwnerChanged handling.
 *
 * Author: Simon McVittie <simon.mcvittie@collabora.co.uk>
 * Copyright © 2011 Nokia Corporation
 * Copyright © 2013 Collabora Ltd.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* To compile outside dbus-glib:
    cc -otest-proxy-noc \
    `pkg-config --cflags --libs gobject-2.0 dbus-glib-1 dbus-1` \
    proxy-noc.c
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <glib-object.h>

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>

#define WELL_KNOWN_NAME "com.example.AuthService"
#define PATH "/"
#define IFACE WELL_KNOWN_NAME
#define AUTH_RESULT "AuthenticationResult"

typedef struct {
    GError *error;
    DBusError dbus_error;

    DBusConnection *service_conn;
    DBusGConnection *service_gconn;
    DBusConnection *attacker_conn;
    DBusGConnection *attacker_gconn;
    DBusConnection *client_conn;
    DBusGConnection *client_gconn;
    DBusGProxy *proxy;

    GPtrArray *auth_results;
} Fixture;

static void oom (void) G_GNUC_NORETURN;

static void
oom (void)
{
  g_error ("out of memory");
}

static void
assert_no_error (const DBusError *e)
{
  if (G_UNLIKELY (dbus_error_is_set (e)))
    g_error ("expected success but got error: %s: %s", e->name, e->message);
}

static void
auth_result_cb (DBusGProxy *proxy,
    const gchar *res,
    gpointer user_data)
{
  Fixture *f = user_data;

  g_assert (proxy == f->proxy);
  g_ptr_array_add (f->auth_results, g_strdup (res));
}

static void
setup (Fixture *f,
    gconstpointer addr)
{
  int ret;

  dbus_error_init (&f->dbus_error);

  /* A trusted service on the bus. (For this to be a vulnerability, it
   * would have to be the system bus.) */
  f->service_gconn = dbus_g_bus_get_private (DBUS_BUS_SESSION, NULL,
      &f->error);
  g_assert_no_error (f->error);
  g_assert (f->service_gconn != NULL);
  f->service_conn = dbus_g_connection_get_connection (f->service_gconn);

  /* An attacker that intends to pretend to be that service. */
  f->attacker_gconn = dbus_g_bus_get_private (DBUS_BUS_SESSION, NULL,
      &f->error);
  g_assert_no_error (f->error);
  g_assert (f->attacker_gconn != NULL);
  f->attacker_conn = dbus_g_connection_get_connection (f->attacker_gconn);

  /* The service owns a well-known name. */
  ret = dbus_bus_request_name (f->service_conn, WELL_KNOWN_NAME,
      DBUS_NAME_FLAG_DO_NOT_QUEUE, &f->dbus_error);
  assert_no_error (&f->dbus_error);
  g_assert_cmpint (ret, ==, DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER);

  /* The victim of the attack. */
  f->client_gconn = dbus_g_bus_get_private (DBUS_BUS_SESSION, NULL, &f->error);
  g_assert_no_error (f->error);
  g_assert (f->client_gconn != NULL);
  f->client_conn = dbus_g_connection_get_connection (f->client_gconn);

  f->proxy = dbus_g_proxy_new_for_name (f->client_gconn, WELL_KNOWN_NAME,
      PATH, IFACE);
  g_assert (DBUS_IS_G_PROXY (f->proxy));

  /* The proxy is listening for the signal. */
  f->auth_results = g_ptr_array_new_with_free_func (g_free);
  dbus_g_proxy_add_signal (f->proxy, AUTH_RESULT,
      G_TYPE_STRING,
      G_TYPE_INVALID);
  dbus_g_proxy_connect_signal (f->proxy, AUTH_RESULT,
      G_CALLBACK (auth_result_cb), f, NULL);
}

static void
test_spoof (Fixture *f,
    gconstpointer addr)
{
  DBusMessage *message;
  const char *well_known_name = WELL_KNOWN_NAME;
  const char *service_name = dbus_bus_get_unique_name (f->service_conn);
  const char *client_name = dbus_bus_get_unique_name (f->client_conn);
  const char *attacker_name = dbus_bus_get_unique_name (f->attacker_conn);
  const char *auth_result;

  auth_result = "on yr bus spoofing yr authentication";

  /* The attacker tries to pretend to be the service by spoofing
   * NameOwnerChanged(name=WELL_KNOWN_NAME, old=service, new=attacker). */
  message = dbus_message_new_signal (DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS,
      "NameOwnerChanged");

  if (message == NULL ||
      /* Bypass match rules by sending it unicast */
      !dbus_message_set_destination (message, client_name) ||
      !dbus_message_append_args (message,
        DBUS_TYPE_STRING, &well_known_name,
        DBUS_TYPE_STRING, &service_name,
        DBUS_TYPE_STRING, &attacker_name,
        DBUS_TYPE_INVALID) ||
      !dbus_connection_send (f->attacker_conn, message, NULL))
    {
      oom ();
    }

  dbus_message_unref (message);

  /* The attacker sends a message purporting to be from the service. */
  message = dbus_message_new_signal (PATH, IFACE, AUTH_RESULT);

  if (message == NULL ||
      /* Again, bypass match rules by sending it unicast */
      !dbus_message_set_destination (message, client_name) ||
      !dbus_message_append_args (message,
        DBUS_TYPE_STRING, &auth_result,
        DBUS_TYPE_INVALID) ||
      !dbus_connection_send (f->attacker_conn, message, NULL))
    {
      oom ();
    }

  dbus_message_unref (message);

  /* The service sends a message - too slow! The attacker won the race. */
  auth_result = "access denied";
  message = dbus_message_new_signal (PATH, IFACE, AUTH_RESULT);

  if (message == NULL ||
      !dbus_message_append_args (message,
        DBUS_TYPE_STRING, &auth_result,
        DBUS_TYPE_INVALID) ||
      !dbus_connection_send (f->service_conn, message, NULL))
    {
      oom ();
    }

  dbus_message_unref (message);

  /* Client waits for the first response */
  while (f->auth_results->len < 1)
    g_main_context_iteration (NULL, TRUE);

  /* The spoofed result was ignored, the real result was used. */
  g_assert_cmpstr (g_ptr_array_index (f->auth_results, 0), ==,
      "access denied");
}

static void
teardown (Fixture *f,
    gconstpointer addr G_GNUC_UNUSED)
{
  f->client_gconn = NULL;
  f->service_gconn = NULL;
  f->attacker_gconn = NULL;

  g_ptr_array_unref (f->auth_results);

  if (f->proxy != NULL)
    {
      dbus_g_proxy_disconnect_signal (f->proxy, AUTH_RESULT,
          G_CALLBACK (auth_result_cb), f);
      g_object_unref (f->proxy);
      f->proxy = NULL;
    }

  if (f->client_conn != NULL)
    {
      dbus_connection_close (f->client_conn);
      dbus_connection_unref (f->client_conn);
      f->client_conn = NULL;
    }

  if (f->service_conn != NULL)
    {
      dbus_connection_close (f->service_conn);
      dbus_connection_unref (f->service_conn);
      f->service_conn = NULL;
    }

  if (f->attacker_conn != NULL)
    {
      dbus_connection_close (f->attacker_conn);
      dbus_connection_unref (f->attacker_conn);
      f->attacker_conn = NULL;
    }
}

int
main (int argc,
    char **argv)
{
  g_test_init (&argc, &argv, NULL);
  g_type_init ();
  dbus_g_type_specialized_init ();

  g_test_bug_base ("https://bugs.freedesktop.org/show_bug.cgi?id=");

  g_test_add ("/proxy/spoof", Fixture, "unix:tmpdir=/tmp", setup,
      test_spoof, teardown);

  return g_test_run ();
}
