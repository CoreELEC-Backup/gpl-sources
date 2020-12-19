/*
 *
 *  Connection Manager
 *
 *  Copyright (C) 2007-2013  Intel Corporation. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>

#include <glib.h>
#include <stdlib.h>
#include <gweb/gresolv.h>
#include <netdb.h>

#include "connman.h"

#define TS_RECHECK_INTERVAL     7200

static struct connman_service *ts_service;
static GSList *timeservers_list = NULL;
static GSList *ts_list = NULL;
static char *ts_current = NULL;
static int ts_recheck_id = 0;
static int ts_backoff_id = 0;

static GResolv *resolv = NULL;
static int resolv_id = 0;

static void sync_next(void);

static void resolv_debug(const char *str, void *data)
{
	connman_info("%s: %s\n", (const char *) data, str);
}

static void ntp_callback(bool success, void *user_data)
{
	DBG("success %d", success);

	if (!success)
		sync_next();
}

static void save_timeservers(char **servers)
{
	GKeyFile *keyfile;
	int cnt;

	keyfile = __connman_storage_load_global();
	if (!keyfile)
		keyfile = g_key_file_new();

	for (cnt = 0; servers && servers[cnt]; cnt++);

	g_key_file_set_string_list(keyfile, "global", "Timeservers",
			   (const gchar **)servers, cnt);

	__connman_storage_save_global(keyfile);

	g_key_file_free(keyfile);
}

static char **load_timeservers(void)
{
	GKeyFile *keyfile;
	char **servers = NULL;

	keyfile = __connman_storage_load_global();
	if (!keyfile)
		return NULL;

	servers = g_key_file_get_string_list(keyfile, "global",
						"Timeservers", NULL, NULL);

	g_key_file_free(keyfile);

	return servers;
}

static void resolv_result(GResolvResultStatus status, char **results,
				gpointer user_data)
{
	int i;

	DBG("status %d", status);

	if (status == G_RESOLV_RESULT_STATUS_SUCCESS) {
		if (results) {
			/* prepend the results in reverse order */

			for (i = 0; results[i]; i++)
				/* count */;
			i--;

			for (; i >= 0; i--) {
				DBG("result[%d]: %s", i, results[i]);

				ts_list = __connman_timeserver_add_list(
					ts_list, results[i]);
			}
		}
	}

	sync_next();
}

/*
 * Once the timeserver list (timeserver_list) is created, we start
 * querying the servers one by one. If resolving fails on one of them,
 * we move to the next one. The user can enter either an IP address or
 * a URL for the timeserver. We only resolve the URLs. Once we have an
 * IP for the NTP server, we start querying it for time corrections.
 */
static void timeserver_sync_start(void)
{
	GSList *list;

	for (list = timeservers_list; list; list = list->next) {
		char *timeserver = list->data;

		ts_list = g_slist_prepend(ts_list, g_strdup(timeserver));
	}
	ts_list = g_slist_reverse(ts_list);

	sync_next();
}

static gboolean timeserver_sync_restart(gpointer user_data)
{
	timeserver_sync_start();
	ts_backoff_id = 0;

	return FALSE;
}

/*
 * Select the next time server from the working list (ts_list) because
 * for some reason the first time server in the list didn't work. If
 * none of the server did work we start over with the first server
 * with a backoff.
 */
static void sync_next(void)
{
	if (ts_current) {
		g_free(ts_current);
		ts_current = NULL;
	}

	__connman_ntp_stop();

	while (ts_list) {
		ts_current = ts_list->data;
		ts_list = g_slist_delete_link(ts_list, ts_list);

		/* if it's an IP, directly query it. */
		if (connman_inet_check_ipaddress(ts_current) > 0) {
			DBG("Using timeserver %s", ts_current);
			__connman_ntp_start(ts_current, ntp_callback, NULL);
			return;
		}

		DBG("Resolving timeserver %s", ts_current);
		resolv_id = g_resolv_lookup_hostname(resolv, ts_current,
						resolv_result, NULL);
		return;
	}

	DBG("No timeserver could be used, restart probing in 5 seconds");
	ts_backoff_id = g_timeout_add_seconds(5, timeserver_sync_restart, NULL);
}

GSList *__connman_timeserver_add_list(GSList *server_list,
		const char *timeserver)
{
	GSList *list = server_list;

	if (!timeserver)
		return server_list;

	while (list) {
		char *existing_server = list->data;
		if (strcmp(timeserver, existing_server) == 0)
			return server_list;
		list = g_slist_next(list);
	}
	return g_slist_prepend(server_list, g_strdup(timeserver));
}

/*
 * __connman_timeserver_get_all function creates the timeserver
 * list which will be used to determine NTP server for time corrections.
 * The service settings take priority over the global timeservers.
 */
GSList *__connman_timeserver_get_all(struct connman_service *service)
{
	GSList *list = NULL;
	struct connman_network *network;
	char **timeservers;
	char **service_ts;
	char **service_ts_config;
	const char *service_gw;
	char **fallback_ts;
	int index, i;

	if (__connman_clock_timeupdates() == TIME_UPDATES_MANUAL)
		return NULL;

	service_ts_config = connman_service_get_timeservers_config(service);

	/* First add Service Timeservers.Configuration to the list */
	for (i = 0; service_ts_config && service_ts_config[i];
			i++)
		list = __connman_timeserver_add_list(list,
				service_ts_config[i]);

	service_ts = connman_service_get_timeservers(service);

	/* Then add Service Timeservers via DHCP to the list */
	for (i = 0; service_ts && service_ts[i]; i++)
		list = __connman_timeserver_add_list(list, service_ts[i]);

	/*
	 * Then add Service Gateway to the list, if UseGatewaysAsTimeservers
	 * configuration option is set to true.
	 */
	if (connman_setting_get_bool("UseGatewaysAsTimeservers")) {
		network = __connman_service_get_network(service);
		if (network) {
			index = connman_network_get_index(network);
			service_gw = __connman_ipconfig_get_gateway_from_index(index,
				CONNMAN_IPCONFIG_TYPE_ALL);

			if (service_gw)
				list = __connman_timeserver_add_list(list, service_gw);
		}
	}

	/* Then add Global Timeservers to the list */
	timeservers = load_timeservers();

	for (i = 0; timeservers && timeservers[i]; i++)
		list = __connman_timeserver_add_list(list, timeservers[i]);

	g_strfreev(timeservers);

	fallback_ts = connman_setting_get_string_list("FallbackTimeservers");

	/* Lastly add the fallback servers */
	for (i = 0; fallback_ts && fallback_ts[i]; i++)
		list = __connman_timeserver_add_list(list, fallback_ts[i]);

	return g_slist_reverse(list);
}

static gboolean ts_recheck(gpointer user_data)
{
	GSList *ts;

	ts = __connman_timeserver_get_all(connman_service_get_default());

	if (!ts) {
		DBG("timeservers disabled");

		return TRUE;
	}

	if (g_strcmp0(ts_current, ts->data) != 0) {
		DBG("current %s preferred %s", ts_current, (char *)ts->data);

		g_slist_free_full(ts, g_free);

		__connman_timeserver_sync(NULL);

		return FALSE;
	}

	DBG("");

	g_slist_free_full(ts, g_free);

	return TRUE;
}

static void ts_recheck_disable(void)
{
	if (ts_recheck_id == 0)
		return;

	g_source_remove(ts_recheck_id);
	ts_recheck_id = 0;

	if (ts_backoff_id) {
		g_source_remove(ts_backoff_id);
		ts_backoff_id = 0;
	}

	if (ts_current) {
		g_free(ts_current);
		ts_current = NULL;
	}
}

static void ts_recheck_enable(void)
{
	if (ts_recheck_id > 0)
		return;

	ts_recheck_id = g_timeout_add_seconds(TS_RECHECK_INTERVAL, ts_recheck,
			NULL);
}

/*
 * This function must be called every time the default service changes, the
 * service timeserver(s) or gateway changes or the global timeserver(s) changes.
 */
int __connman_timeserver_sync(struct connman_service *default_service)
{
	struct connman_service *service;
	char **nameservers;
	int i;

	if (default_service)
		service = default_service;
	else
		service = connman_service_get_default();

	if (!service)
		return -EINVAL;

	if (service == ts_service)
		return -EALREADY;

	if (!resolv)
		return 0;
	/*
	 * Before we start creating the new timeserver list we must stop
	 * any ongoing ntp query and server resolution.
	 */

	__connman_ntp_stop();

	ts_recheck_disable();

	if (resolv_id > 0)
		g_resolv_cancel_lookup(resolv, resolv_id);

	g_resolv_flush_nameservers(resolv);

	nameservers = connman_service_get_nameservers(service);
	if (nameservers) {
		for (i = 0; nameservers[i]; i++)
			g_resolv_add_nameserver(resolv, nameservers[i], 53, 0);

		g_strfreev(nameservers);
	}

	g_slist_free_full(timeservers_list, g_free);

	timeservers_list = __connman_timeserver_get_all(service);

	__connman_service_timeserver_changed(service, timeservers_list);

	if (!timeservers_list) {
		DBG("No timeservers set.");
		return 0;
	}

	ts_recheck_enable();

	ts_service = service;
	timeserver_sync_start();

	return 0;
}

static int timeserver_start(struct connman_service *service)
{
	char **nameservers;
	int i;

	DBG("service %p", service);

	i = __connman_service_get_index(service);
	if (i < 0)
		return -EINVAL;

	nameservers = connman_service_get_nameservers(service);

	/* Stop an already ongoing resolution, if there is one */
	if (resolv && resolv_id > 0)
		g_resolv_cancel_lookup(resolv, resolv_id);

	/* get rid of the old resolver */
	if (resolv) {
		g_resolv_unref(resolv);
		resolv = NULL;
	}

	resolv = g_resolv_new(i);
	if (!resolv) {
		g_strfreev(nameservers);
		return -ENOMEM;
	}

	if (getenv("CONNMAN_RESOLV_DEBUG"))
		g_resolv_set_debug(resolv, resolv_debug, "RESOLV");

	if (nameservers) {
		for (i = 0; nameservers[i]; i++)
			g_resolv_add_nameserver(resolv, nameservers[i], 53, 0);

		g_strfreev(nameservers);
	}

	return __connman_timeserver_sync(service);
}

static void timeserver_stop(void)
{
	DBG(" ");

	ts_service = NULL;

	if (resolv) {
		g_resolv_unref(resolv);
		resolv = NULL;
	}

	g_slist_free_full(timeservers_list, g_free);
	timeservers_list = NULL;

	g_slist_free_full(ts_list, g_free);
	ts_list = NULL;

	__connman_ntp_stop();

	ts_recheck_disable();
}

int __connman_timeserver_system_set(char **servers)
{
	save_timeservers(servers);

	__connman_timeserver_sync(NULL);

	return 0;
}

char **__connman_timeserver_system_get()
{
	char **servers;

	servers = load_timeservers();
	return servers;
}

static void default_changed(struct connman_service *default_service)
{
	if (default_service)
		timeserver_start(default_service);
	else
		timeserver_stop();
}

static const struct connman_notifier timeserver_notifier = {
	.name			= "timeserver",
	.default_changed	= default_changed,
};

int __connman_timeserver_init(void)
{
	DBG("");

	connman_notifier_register(&timeserver_notifier);

	return 0;
}

void __connman_timeserver_cleanup(void)
{
	DBG("");

	connman_notifier_unregister(&timeserver_notifier);
}
