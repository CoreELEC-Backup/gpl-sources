/*
 *
 *  ConnMan VPN daemon
 *
 *  Copyright (C) 2010-2014  BMW Car IT GmbH.
 *  Copyright (C) 2016-2019  Jolla Ltd.
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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <net/if.h>
#include <linux/if_tun.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <glib.h>

#define CONNMAN_API_SUBJECT_TO_CHANGE
#include <connman/plugin.h>
#include <connman/log.h>
#include <connman/task.h>
#include <connman/dbus.h>
#include <connman/ipconfig.h>
#include <connman/agent.h>
#include <connman/setting.h>
#include <connman/vpn-dbus.h>

#include "../vpn-provider.h"
#include "../vpn-agent.h"

#include "vpn.h"

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

static DBusConnection *connection;

struct {
	const char *cm_opt;
	const char *ov_opt;
	char       has_value;
} ov_options[] = {
	{ "Host", "--remote", 1 },
	{ "OpenVPN.CACert", "--ca", 1 },
	{ "OpenVPN.Cert", "--cert", 1 },
	{ "OpenVPN.Key", "--key", 1 },
	{ "OpenVPN.MTU", "--tun-mtu", 1 },
	{ "OpenVPN.NSCertType", "--ns-cert-type", 1 },
	{ "OpenVPN.Proto", "--proto", 1 },
	{ "OpenVPN.Port", "--port", 1 },
	{ "OpenVPN.AuthUserPass", "--auth-user-pass", 1 },
	{ "OpenVPN.AskPass", "--askpass", 1 },
	{ "OpenVPN.AuthNoCache", "--auth-nocache", 0 },
	{ "OpenVPN.TLSRemote", "--tls-remote", 1 },
	{ "OpenVPN.TLSAuth", NULL, 1 },
	{ "OpenVPN.TLSAuthDir", NULL, 1 },
	{ "OpenVPN.TLSCipher", "--tls-cipher", 1},
	{ "OpenVPN.Cipher", "--cipher", 1 },
	{ "OpenVPN.Auth", "--auth", 1 },
	{ "OpenVPN.CompLZO", "--comp-lzo", 0 },
	{ "OpenVPN.RemoteCertTls", "--remote-cert-tls", 1 },
	{ "OpenVPN.ConfigFile", "--config", 1 },
	{ "OpenVPN.DeviceType", NULL, 1 },
	{ "OpenVPN.Verb", "--verb", 1 },
	{ "OpenVPN.Ping", "--ping", 1},
	{ "OpenVPN.PingExit", "--ping-exit", 1},
	{ "OpenVPN.RemapUsr1", "--remap-usr1", 1},
};

struct ov_private_data {
	struct vpn_provider *provider;
	struct connman_task *task;
	char *dbus_sender;
	char *if_name;
	vpn_provider_connect_cb_t cb;
	void *user_data;
	char *mgmt_path;
	guint mgmt_timer_id;
	guint mgmt_event_id;
	GIOChannel *mgmt_channel;
	int connect_attempts;
	int failed_attempts_privatekey;
};

static void ov_connect_done(struct ov_private_data *data, int err)
{
	if (data && data->cb) {
		vpn_provider_connect_cb_t cb = data->cb;
		void *user_data = data->user_data;

		/* Make sure we don't invoke this callback twice */
		data->cb = NULL;
		data->user_data = NULL;
		cb(data->provider, user_data, err);
	}

	if (!err)
		data->failed_attempts_privatekey = 0;
}

static void free_private_data(struct ov_private_data *data)
{
	if (vpn_provider_get_plugin_data(data->provider) == data)
		vpn_provider_set_plugin_data(data->provider, NULL);

	ov_connect_done(data, EIO);
	vpn_provider_unref(data->provider);
	g_free(data->dbus_sender);
	g_free(data->if_name);
	g_free(data->mgmt_path);
	g_free(data);
}

struct nameserver_entry {
	int id;
	char *nameserver;
};

static struct nameserver_entry *ov_append_dns_entries(const char *key,
						const char *value)
{
	struct nameserver_entry *entry = NULL;
	gchar **options;

	if (!g_str_has_prefix(key, "foreign_option_"))
		return NULL;

	options = g_strsplit(value, " ", 3);
	if (options[0] &&
		!strcmp(options[0], "dhcp-option") &&
			options[1] &&
			!strcmp(options[1], "DNS") &&
				options[2]) {

		entry = g_try_new(struct nameserver_entry, 1);
		if (!entry)
			return NULL;

		entry->nameserver = g_strdup(options[2]);
		entry->id = atoi(key + 15); /* foreign_option_XXX */
	}

	g_strfreev(options);

	return entry;
}

static char *ov_get_domain_name(const char *key, const char *value)
{
	gchar **options;
	char *domain = NULL;

	if (!g_str_has_prefix(key, "foreign_option_"))
		return NULL;

	options = g_strsplit(value, " ", 3);
	if (options[0] &&
		!strcmp(options[0], "dhcp-option") &&
			options[1] &&
			!strcmp(options[1], "DOMAIN") &&
				options[2]) {

		domain = g_strdup(options[2]);
	}

	g_strfreev(options);

	return domain;
}

static gint cmp_ns(gconstpointer a, gconstpointer b)
{
	struct nameserver_entry *entry_a = (struct nameserver_entry *)a;
	struct nameserver_entry *entry_b = (struct nameserver_entry *)b;

	if (entry_a->id < entry_b->id)
		return -1;

	if (entry_a->id > entry_b->id)
		return 1;

	return 0;
}

static void free_ns_entry(gpointer data)
{
	struct nameserver_entry *entry = data;

	g_free(entry->nameserver);
	g_free(entry);
}

static int ov_notify(DBusMessage *msg, struct vpn_provider *provider)
{
	DBusMessageIter iter, dict;
	const char *reason, *key, *value;
	char *address = NULL, *gateway = NULL, *peer = NULL, *netmask = NULL;
	struct connman_ipaddress *ipaddress;
	GSList *nameserver_list = NULL;
	struct ov_private_data *data = vpn_provider_get_plugin_data(provider);

	dbus_message_iter_init(msg, &iter);

	dbus_message_iter_get_basic(&iter, &reason);
	dbus_message_iter_next(&iter);

	if (!provider) {
		connman_error("No provider found");
		return VPN_STATE_FAILURE;
	}

	DBG("%p %s", vpn_provider_get_name(provider), reason);

	if (strcmp(reason, "up")) {
		ov_connect_done(data, EIO);
		return VPN_STATE_DISCONNECT;
	}

	dbus_message_iter_recurse(&iter, &dict);

	while (dbus_message_iter_get_arg_type(&dict) == DBUS_TYPE_DICT_ENTRY) {
		struct nameserver_entry *ns_entry = NULL;
		DBusMessageIter entry;

		dbus_message_iter_recurse(&dict, &entry);
		dbus_message_iter_get_basic(&entry, &key);
		dbus_message_iter_next(&entry);
		dbus_message_iter_get_basic(&entry, &value);

		DBG("%s = %s", key, value);

		if (!strcmp(key, "trusted_ip"))
			gateway = g_strdup(value);

		if (!strcmp(key, "ifconfig_local"))
			address = g_strdup(value);

		if (!strcmp(key, "ifconfig_netmask"))
			netmask = g_strdup(value);

		if (!strcmp(key, "ifconfig_remote"))
			peer = g_strdup(value);

		if (g_str_has_prefix(key, "route_"))
			vpn_provider_append_route(provider, key, value);

		if ((ns_entry = ov_append_dns_entries(key, value)))
			nameserver_list = g_slist_prepend(nameserver_list,
							ns_entry);
		else {
			char *domain = ov_get_domain_name(key, value);
			if (domain) {
				vpn_provider_set_domain(provider, domain);
				g_free(domain);
			}
		}

		dbus_message_iter_next(&dict);
	}

	ipaddress = connman_ipaddress_alloc(AF_INET);
	if (!ipaddress) {
		g_slist_free_full(nameserver_list, free_ns_entry);
		g_free(address);
		g_free(gateway);
		g_free(peer);
		g_free(netmask);

		return VPN_STATE_FAILURE;
	}

	connman_ipaddress_set_ipv4(ipaddress, address, netmask, gateway);
	connman_ipaddress_set_peer(ipaddress, peer);
	connman_ipaddress_set_p2p(ipaddress, true);
	vpn_provider_set_ipaddress(provider, ipaddress);

	if (nameserver_list) {
		char *nameservers = NULL;
		GSList *tmp;

		nameserver_list = g_slist_sort(nameserver_list, cmp_ns);
		for (tmp = nameserver_list; tmp;
						tmp = g_slist_next(tmp)) {
			struct nameserver_entry *ns = tmp->data;

			if (!nameservers) {
				nameservers = g_strdup(ns->nameserver);
			} else {
				char *str;
				str = g_strjoin(" ", nameservers,
						ns->nameserver, NULL);
				g_free(nameservers);
				nameservers = str;
			}
		}

		g_slist_free_full(nameserver_list, free_ns_entry);

		vpn_provider_set_nameservers(provider, nameservers);

		g_free(nameservers);
	}

	g_free(address);
	g_free(gateway);
	g_free(peer);
	g_free(netmask);
	connman_ipaddress_free(ipaddress);

	ov_connect_done(data, 0);
	return VPN_STATE_CONNECT;
}

static int ov_save(struct vpn_provider *provider, GKeyFile *keyfile)
{
	const char *option;
	int i;

	for (i = 0; i < (int)ARRAY_SIZE(ov_options); i++) {
		if (strncmp(ov_options[i].cm_opt, "OpenVPN.", 8) == 0) {
			option = vpn_provider_get_string(provider,
							ov_options[i].cm_opt);
			if (!option)
				continue;

			g_key_file_set_string(keyfile,
					vpn_provider_get_save_group(provider),
					ov_options[i].cm_opt, option);
		}
	}
	return 0;
}

static int task_append_config_data(struct vpn_provider *provider,
					struct connman_task *task)
{
	const char *option;
	int i;

	for (i = 0; i < (int)ARRAY_SIZE(ov_options); i++) {
		if (!ov_options[i].ov_opt)
			continue;

		option = vpn_provider_get_string(provider,
					ov_options[i].cm_opt);
		if (!option)
			continue;

		/*
		 * If the AuthUserPass option is "-", provide the input
		 * via management interface
		 */
		if (!strcmp(ov_options[i].cm_opt, "OpenVPN.AuthUserPass") &&
						!strcmp(option, "-"))
			option = NULL;

		if (connman_task_add_argument(task,
				ov_options[i].ov_opt,
				ov_options[i].has_value ? option : NULL) < 0)
			return -EIO;

	}

	return 0;
}

static void close_management_interface(struct ov_private_data *data)
{
	if (data->mgmt_path) {
		if (unlink(data->mgmt_path) && errno != ENOENT)
			connman_warn("Unable to unlink management socket %s: "
						"%d", data->mgmt_path, errno);

		g_free(data->mgmt_path);
		data->mgmt_path = NULL;
	}

	if (data->mgmt_timer_id != 0) {
		g_source_remove(data->mgmt_timer_id);
		data->mgmt_timer_id = 0;
	}

	if (data->mgmt_event_id) {
		g_source_remove(data->mgmt_event_id);
		data->mgmt_event_id = 0;
	}

	if (data->mgmt_channel) {
		g_io_channel_shutdown(data->mgmt_channel, FALSE, NULL);
		g_io_channel_unref(data->mgmt_channel);
		data->mgmt_channel = NULL;
	}
}

static void ov_died(struct connman_task *task, int exit_code, void *user_data)
{
	struct ov_private_data *data = user_data;

	/* Cancel any pending agent requests */
	connman_agent_cancel(data->provider);

	close_management_interface(data);

	vpn_died(task, exit_code, data->provider);

	free_private_data(data);
}

static int run_connect(struct ov_private_data *data,
			vpn_provider_connect_cb_t cb, void *user_data)
{
	struct vpn_provider *provider = data->provider;
	struct connman_task *task = data->task;
	const char *option;
	int err = 0;

	option = vpn_provider_get_string(provider, "OpenVPN.ConfigFile");
	if (!option) {
		/*
		 * Set some default options if user has no config file.
		 */
		option = vpn_provider_get_string(provider, "OpenVPN.TLSAuth");
		if (option) {
			connman_task_add_argument(task, "--tls-auth", option);
			option = vpn_provider_get_string(provider,
							"OpenVPN.TLSAuthDir");
			if (option)
				connman_task_add_argument(task, option, NULL);
		}

		connman_task_add_argument(task, "--nobind", NULL);
		connman_task_add_argument(task, "--persist-key", NULL);
		connman_task_add_argument(task, "--client", NULL);
	}

	if (data->mgmt_path) {
		connman_task_add_argument(task, "--management", NULL);
		connman_task_add_argument(task, data->mgmt_path, NULL);
		connman_task_add_argument(task, "unix", NULL);
		connman_task_add_argument(task, "--management-query-passwords",
								NULL);
		connman_task_add_argument(task, "--auth-retry", "interact");
	}

	connman_task_add_argument(task, "--syslog", NULL);

	connman_task_add_argument(task, "--script-security", "2");

	connman_task_add_argument(task, "--up",
					SCRIPTDIR "/openvpn-script");
	connman_task_add_argument(task, "--up-restart", NULL);

	connman_task_add_argument(task, "--setenv", NULL);
	connman_task_add_argument(task, "CONNMAN_BUSNAME",
					dbus_bus_get_unique_name(connection));

	connman_task_add_argument(task, "--setenv", NULL);
	connman_task_add_argument(task, "CONNMAN_INTERFACE",
					CONNMAN_TASK_INTERFACE);

	connman_task_add_argument(task, "--setenv", NULL);
	connman_task_add_argument(task, "CONNMAN_PATH",
					connman_task_get_path(task));

	connman_task_add_argument(task, "--dev", data->if_name);
	option = vpn_provider_get_string(provider, "OpenVPN.DeviceType");
	if (option) {
		connman_task_add_argument(task, "--dev-type", option);
	} else {
		/*
		 * Default to tun for backwards compatibility.
		 */
		connman_task_add_argument(task, "--dev-type", "tun");
	}

	connman_task_add_argument(task, "--persist-tun", NULL);

	connman_task_add_argument(task, "--route-noexec", NULL);
	connman_task_add_argument(task, "--ifconfig-noexec", NULL);

	/*
	 * Disable client restarts with TCP because we can't handle this at
	 * the moment. The problem is that when OpenVPN decides to switch
	 * from CONNECTED state to RECONNECTING and then to RESOLVE,
	 * it is not possible to do a DNS lookup. The DNS server is
	 * not accessible through the tunnel anymore and so we end up
	 * trying to resolve the OpenVPN servers address.
	 *
	 * Disable connetion retrying when OpenVPN is connected over TCP.
	 * With TCP OpenVPN attempts to handle reconnection silently without
	 * reporting the error back when establishing a connection or
	 * reconnecting as succesful one. The latter causes trouble if the
	 * retries are not limited to 1 (no retry) as the interface is up and
	 * connman regards it as the default route and network ceases to work,
	 * including DNS.
	*/
	option = vpn_provider_get_string(provider, "OpenVPN.Proto");
	if (option && g_str_has_prefix(option, "tcp")) {
		option = vpn_provider_get_string(provider, "OpenVPN.PingExit");
		if (!option)
			connman_task_add_argument(task, "--ping-restart", "0");

		connman_task_add_argument(task, "--connect-retry-max", "1");
	/* Apply defaults for --ping and --ping-exit only with UDP protocol. */
	} else {
		/* Apply default of 10 second interval for ping if omitted. */
		option = vpn_provider_get_string(provider, "OpenVPN.Ping");
		if (!option)
			connman_task_add_argument(task, "--ping", "10");

		/* Apply default of 60 seconds for ping exit if omitted. */
		option = vpn_provider_get_string(provider, "OpenVPN.PingExit");
		if (!option)
			connman_task_add_argument(task, "--ping-exit", "60");
	}

	err = connman_task_run(task, ov_died, data, NULL, NULL, NULL);
	if (err < 0) {
		data->cb = NULL;
		data->user_data = NULL;
		connman_error("openvpn failed to start");
		return -EIO;
	} else {
		/* This lets the caller know that the actual result of
		 * the operation will be reported to the callback */
		return -EINPROGRESS;
	}
}

static void ov_quote_credential(GString *line, const char *cred)
{
	if (!line)
		return;

	g_string_append_c(line, '"');

	while (*cred != '\0') {

		switch (*cred) {
		case ' ':
		case '"':
		case '\\':
			g_string_append_c(line, '\\');
			break;
		default:
			break;
		}

		g_string_append_c(line, *cred++);
	}

	g_string_append_c(line, '"');
}

static void ov_return_credentials(struct ov_private_data *data,
				const char *username, const char *password)
{
	GString *reply_string;
	gchar *reply;
	gsize len;

	reply_string = g_string_new(NULL);

	g_string_append(reply_string, "username \"Auth\" ");
	ov_quote_credential(reply_string, username);
	g_string_append_c(reply_string, '\n');

	g_string_append(reply_string, "password \"Auth\" ");
	ov_quote_credential(reply_string, password);
	g_string_append_c(reply_string, '\n');

	len = reply_string->len;
	reply = g_string_free(reply_string, FALSE);

	g_io_channel_write_chars(data->mgmt_channel, reply, len, NULL, NULL);
	g_io_channel_flush(data->mgmt_channel, NULL);

	memset(reply, 0, len);
	g_free(reply);
}

static void ov_return_private_key_password(struct ov_private_data *data,
				const char *privatekeypass)
{
	GString *reply_string;
	gchar *reply;
	gsize len;

	reply_string = g_string_new(NULL);

	g_string_append(reply_string, "password \"Private Key\" ");
	ov_quote_credential(reply_string, privatekeypass);
	g_string_append_c(reply_string, '\n');

	len = reply_string->len;
	reply = g_string_free(reply_string, FALSE);

	g_io_channel_write_chars(data->mgmt_channel, reply, len, NULL, NULL);
	g_io_channel_flush(data->mgmt_channel, NULL);

	memset(reply, 0, len);
	g_free(reply);
}

static void request_input_append_informational(DBusMessageIter *iter,
		void *user_data)
{
	char *str = "string";

	connman_dbus_dict_append_basic(iter, "Type",
				DBUS_TYPE_STRING, &str);
	str = "informational";
	connman_dbus_dict_append_basic(iter, "Requirement",
				DBUS_TYPE_STRING, &str);
}

static void request_input_append_mandatory(DBusMessageIter *iter,
		void *user_data)
{
	char *str = "string";

	connman_dbus_dict_append_basic(iter, "Type",
				DBUS_TYPE_STRING, &str);
	str = "mandatory";
	connman_dbus_dict_append_basic(iter, "Requirement",
				DBUS_TYPE_STRING, &str);
}

static void request_input_append_password(DBusMessageIter *iter,
		void *user_data)
{
	char *str = "password";

	connman_dbus_dict_append_basic(iter, "Type",
				DBUS_TYPE_STRING, &str);
	str = "mandatory";
	connman_dbus_dict_append_basic(iter, "Requirement",
				DBUS_TYPE_STRING, &str);
}

static void request_input_credentials_reply(DBusMessage *reply,
							void *user_data)
{
	struct ov_private_data *data = user_data;
	char *username = NULL;
	char *password = NULL;
	char *key;
	DBusMessageIter iter, dict;
	DBusError error;
	int err = 0;

	connman_info("provider %p", data->provider);

	/*
	 * When connmand calls disconnect because of connection timeout no
	 * reply is received.
	 */
	if (!reply) {
		err = ENOENT;
		goto err;
	}

	dbus_error_init(&error);

	err = vpn_agent_check_and_process_reply_error(reply, data->provider,
				data->task, data->cb, data->user_data);
	if (err) {
		/* Ensure cb is called only once */
		data->cb = NULL;
		data->user_data = NULL;
		return;
	}

	if (!vpn_agent_check_reply_has_dict(reply)) {
		err = ENOENT;
		goto err;
	}

	dbus_message_iter_init(reply, &iter);
	dbus_message_iter_recurse(&iter, &dict);
	while (dbus_message_iter_get_arg_type(&dict) == DBUS_TYPE_DICT_ENTRY) {
		DBusMessageIter entry, value;

		dbus_message_iter_recurse(&dict, &entry);
		if (dbus_message_iter_get_arg_type(&entry) != DBUS_TYPE_STRING)
			break;

		dbus_message_iter_get_basic(&entry, &key);

		if (g_str_equal(key, "OpenVPN.Password")) {
			dbus_message_iter_next(&entry);
			if (dbus_message_iter_get_arg_type(&entry)
							!= DBUS_TYPE_VARIANT)
				break;
			dbus_message_iter_recurse(&entry, &value);
			if (dbus_message_iter_get_arg_type(&value)
							!= DBUS_TYPE_STRING)
				break;
			dbus_message_iter_get_basic(&value, &password);
			vpn_provider_set_string_hide_value(data->provider,
					key, password);

		} else if (g_str_equal(key, "OpenVPN.Username")) {
			dbus_message_iter_next(&entry);
			if (dbus_message_iter_get_arg_type(&entry)
							!= DBUS_TYPE_VARIANT)
				break;
			dbus_message_iter_recurse(&entry, &value);
			if (dbus_message_iter_get_arg_type(&value)
							!= DBUS_TYPE_STRING)
				break;
			dbus_message_iter_get_basic(&value, &username);
			vpn_provider_set_string_hide_value(data->provider,
					key, username);
		}

		dbus_message_iter_next(&dict);
	}

	if (!password || !username) {
		vpn_provider_indicate_error(data->provider,
					VPN_PROVIDER_ERROR_AUTH_FAILED);
		err = EACCES;
		goto err;
	}

	ov_return_credentials(data, username, password);

	return;

err:
	ov_connect_done(data, err);
}

static int request_credentials_input(struct ov_private_data *data)
{
	DBusMessage *message;
	const char *path, *agent_sender, *agent_path;
	DBusMessageIter iter;
	DBusMessageIter dict;
	int err;
	void *agent;

	agent = connman_agent_get_info(data->dbus_sender, &agent_sender,
							&agent_path);
	if (!agent || !agent_path)
		return -ESRCH;

	message = dbus_message_new_method_call(agent_sender, agent_path,
					VPN_AGENT_INTERFACE,
					"RequestInput");
	if (!message)
		return -ENOMEM;

	dbus_message_iter_init_append(message, &iter);

	path = vpn_provider_get_path(data->provider);
	dbus_message_iter_append_basic(&iter,
				DBUS_TYPE_OBJECT_PATH, &path);

	connman_dbus_dict_open(&iter, &dict);

	if (vpn_provider_get_authentication_errors(data->provider))
		vpn_agent_append_auth_failure(&dict, data->provider, NULL);

	/* Request temporary properties to pass on to openvpn */
	connman_dbus_dict_append_dict(&dict, "OpenVPN.Username",
			request_input_append_mandatory, NULL);

	connman_dbus_dict_append_dict(&dict, "OpenVPN.Password",
			request_input_append_password, NULL);

	vpn_agent_append_host_and_name(&dict, data->provider);

	connman_dbus_dict_close(&iter, &dict);

	err = connman_agent_queue_message(data->provider, message,
			connman_timeout_input_request(),
			request_input_credentials_reply, data, agent);

	if (err < 0 && err != -EBUSY) {
		connman_error("error %d sending agent request", err);
		dbus_message_unref(message);

		return err;
	}

	dbus_message_unref(message);

	return -EINPROGRESS;
}

static void request_input_private_key_reply(DBusMessage *reply,
							void *user_data)
{
	struct ov_private_data *data = user_data;
	const char *privatekeypass = NULL;
	const char *key;
	DBusMessageIter iter, dict;
	DBusError error;
	int err = 0;

	connman_info("provider %p", data->provider);

	/*
	 * When connmand calls disconnect because of connection timeout no
	 * reply is received.
	 */
	if (!reply) {
		err = ENOENT;
		goto err;
	}

	dbus_error_init(&error);

	err = vpn_agent_check_and_process_reply_error(reply, data->provider,
				data->task, data->cb, data->user_data);
	if (err) {
		/* Ensure cb is called only once */
		data->cb = NULL;
		data->user_data = NULL;
		return;
	}

	if (!vpn_agent_check_reply_has_dict(reply)) {
		err = ENOENT;
		goto err;
	}

	dbus_message_iter_init(reply, &iter);
	dbus_message_iter_recurse(&iter, &dict);
	while (dbus_message_iter_get_arg_type(&dict) == DBUS_TYPE_DICT_ENTRY) {
		DBusMessageIter entry, value;

		dbus_message_iter_recurse(&dict, &entry);
		if (dbus_message_iter_get_arg_type(&entry) != DBUS_TYPE_STRING)
			break;

		dbus_message_iter_get_basic(&entry, &key);

		if (g_str_equal(key, "OpenVPN.PrivateKeyPassword")) {
			dbus_message_iter_next(&entry);
			if (dbus_message_iter_get_arg_type(&entry)
							!= DBUS_TYPE_VARIANT)
				break;
			dbus_message_iter_recurse(&entry, &value);
			if (dbus_message_iter_get_arg_type(&value)
							!= DBUS_TYPE_STRING)
				break;
			dbus_message_iter_get_basic(&value, &privatekeypass);
			vpn_provider_set_string_hide_value(data->provider,
					key, privatekeypass);

		}

		dbus_message_iter_next(&dict);
	}

	if (!privatekeypass) {
		vpn_provider_indicate_error(data->provider,
					VPN_PROVIDER_ERROR_AUTH_FAILED);

		err = EACCES;
		goto err;
	}

	ov_return_private_key_password(data, privatekeypass);

	return;

err:
	ov_connect_done(data, err);
}

static int request_private_key_input(struct ov_private_data *data)
{
	DBusMessage *message;
	const char *path, *agent_sender, *agent_path;
	const char *privatekeypass;
	DBusMessageIter iter;
	DBusMessageIter dict;
	int err;
	void *agent;

	/*
	 * First check if this is the second attempt to get the key within
	 * this connection. In such case there has been invalid Private Key
	 * Password and it must be reset, and queried from user.
	 */
	if (data->failed_attempts_privatekey) {
		vpn_provider_set_string_hide_value(data->provider,
					"OpenVPN.PrivateKeyPassword", NULL);
	} else {
		/* If the encrypted Private key password is kept in memory and
		 * use it first. If authentication fails this is cleared,
		 * likewise it is when connman-vpnd is restarted.
		 */
		privatekeypass = vpn_provider_get_string(data->provider,
					"OpenVPN.PrivateKeyPassword");
		if (privatekeypass) {
			ov_return_private_key_password(data, privatekeypass);
			goto out;
		}
	}

	agent = connman_agent_get_info(data->dbus_sender, &agent_sender,
							&agent_path);
	if (!agent || !agent_path)
		return -ESRCH;

	message = dbus_message_new_method_call(agent_sender, agent_path,
					VPN_AGENT_INTERFACE,
					"RequestInput");
	if (!message)
		return -ENOMEM;

	dbus_message_iter_init_append(message, &iter);

	path = vpn_provider_get_path(data->provider);
	dbus_message_iter_append_basic(&iter,
				DBUS_TYPE_OBJECT_PATH, &path);

	connman_dbus_dict_open(&iter, &dict);

	connman_dbus_dict_append_dict(&dict, "OpenVPN.PrivateKeyPassword",
			request_input_append_password, NULL);

	vpn_agent_append_host_and_name(&dict, data->provider);

	/* Do not allow to store or retrieve the encrypted Private Key pass */
	vpn_agent_append_allow_credential_storage(&dict, false);
	vpn_agent_append_allow_credential_retrieval(&dict, false);

	/*
	 * Indicate to keep credentials, the enc Private Key password should
	 * not affect the credential storing.
	 */
	vpn_agent_append_keep_credentials(&dict, true);

	connman_dbus_dict_append_dict(&dict, "Enter Private Key password",
			request_input_append_informational, NULL);

	connman_dbus_dict_close(&iter, &dict);

	err = connman_agent_queue_message(data->provider, message,
			connman_timeout_input_request(),
			request_input_private_key_reply, data, agent);

	if (err < 0 && err != -EBUSY) {
		connman_error("error %d sending agent request", err);
		dbus_message_unref(message);

		return err;
	}

	dbus_message_unref(message);

out:
	return -EINPROGRESS;
}

static gboolean ov_management_handle_input(GIOChannel *source,
				GIOCondition condition, gpointer user_data)
{
	struct ov_private_data *data = user_data;
	char *str = NULL;
	int err = 0;
	bool close = false;

	if (condition & G_IO_IN) {
		/*
		 * Just return if line is not read and str is not allocated.
		 * Condition check handles closing of the channel later.
		 */
		if (g_io_channel_read_line(source, &str, NULL, NULL, NULL) !=
							G_IO_STATUS_NORMAL)
			return true;

		str[strlen(str) - 1] = '\0';
		connman_warn("openvpn request %s", str);

		if (g_str_has_prefix(str, ">PASSWORD:Need 'Auth'")) {
			/*
			 * Request credentials from the user
			 */
			err = request_credentials_input(data);
			if (err != -EINPROGRESS)
				close = true;
		} else if (g_str_has_prefix(str,
				">PASSWORD:Need 'Private Key'")) {
			err = request_private_key_input(data);
			if (err != -EINPROGRESS)
				close = true;
		} else if (g_str_has_prefix(str,
				">PASSWORD:Verification Failed: 'Auth'")) {
			/*
			 * Add error only, state change indication causes
			 * signal to be sent, which is not desired when
			 * OpenVPN is in interactive mode.
			 */
			vpn_provider_add_error(data->provider,
					VPN_PROVIDER_ERROR_AUTH_FAILED);
		/*
		 * According to the OpenVPN manual about management interface
		 * https://openvpn.net/community-resources/management-interface/
		 * this should be received but it does not seem to be reported
		 * when decrypting private key fails. This requires following
		 * patch for OpenVPN (at least <= 2.4.5) in order to work:
		 * https://git.sailfishos.org/mer-core/openvpn/blob/
		 * 4f4b4af116292a207416c8a990392e35a6fc41af/rpm/privatekey-
		 * passphrase-handling.diff
		 */
		} else if (g_str_has_prefix(str, ">PASSWORD:Verification "
				"Failed: 'Private Key'")) {
			data->failed_attempts_privatekey++;
		}

		g_free(str);
	} else if (condition & (G_IO_ERR | G_IO_HUP)) {
		connman_warn("Management channel termination");
		close = true;
	}

	if (close)
		close_management_interface(data);

	return true;
}

static int ov_management_connect_timer_cb(gpointer user_data)
{
	struct ov_private_data *data = user_data;

	if (!data->mgmt_channel) {
		int fd = socket(AF_UNIX, SOCK_STREAM, 0);
		if (fd >= 0) {
			struct sockaddr_un remote;
			int err;

			memset(&remote, 0, sizeof(remote));
			remote.sun_family = AF_UNIX;
			g_strlcpy(remote.sun_path, data->mgmt_path,
						sizeof(remote.sun_path));

			err = connect(fd, (struct sockaddr *)&remote,
						sizeof(remote));
			if (err == 0) {
				data->mgmt_channel = g_io_channel_unix_new(fd);
				data->mgmt_event_id =
					g_io_add_watch(data->mgmt_channel,
						G_IO_IN | G_IO_ERR | G_IO_HUP,
						ov_management_handle_input,
						data);

				connman_warn("Connected management socket");
				data->mgmt_timer_id = 0;
				return G_SOURCE_REMOVE;
			}
			close(fd);
		}
	}

	data->connect_attempts++;
	if (data->connect_attempts > 30) {
		connman_warn("Unable to connect management socket");
		data->mgmt_timer_id = 0;
		return G_SOURCE_REMOVE;
	}

	return G_SOURCE_CONTINUE;
}

static int ov_connect(struct vpn_provider *provider,
			struct connman_task *task, const char *if_name,
			vpn_provider_connect_cb_t cb, const char *dbus_sender,
			void *user_data)
{
	const char *tmpdir;
	struct ov_private_data *data;

	data = g_try_new0(struct ov_private_data, 1);
	if (!data)
		return -ENOMEM;

	vpn_provider_set_plugin_data(provider, data);
	data->provider = vpn_provider_ref(provider);
	data->task = task;
	data->dbus_sender = g_strdup(dbus_sender);
	data->if_name = g_strdup(if_name);
	data->cb = cb;
	data->user_data = user_data;

	/*
	 * We need to use the management interface to provide
	 * the user credentials and password for decrypting private key.
	 */

	/* Use env TMPDIR for creating management socket, fall back to /tmp */
	tmpdir = getenv("TMPDIR");
	if (!tmpdir || !*tmpdir)
		tmpdir = "/tmp";

	/* Set up the path for the management interface */
	data->mgmt_path = g_strconcat(tmpdir, "/connman-vpn-management-",
		vpn_provider_get_ident(provider), NULL);
	if (unlink(data->mgmt_path) != 0 && errno != ENOENT) {
		connman_warn("Unable to unlink management socket %s: %d",
					data->mgmt_path, errno);
	}

	data->mgmt_timer_id = g_timeout_add(200,
				ov_management_connect_timer_cb, data);

	task_append_config_data(provider, task);

	return run_connect(data, cb, user_data);
}

static void ov_disconnect(struct vpn_provider *provider)
{
	if (!provider)
		return;

	connman_agent_cancel(provider);

	vpn_provider_set_state(provider, VPN_PROVIDER_STATE_DISCONNECT);
}

static int ov_device_flags(struct vpn_provider *provider)
{
	const char *option;

	option = vpn_provider_get_string(provider, "OpenVPN.DeviceType");
	if (!option) {
		return IFF_TUN;
	}

	if (g_str_equal(option, "tap")) {
		return IFF_TAP;
	}

	if (!g_str_equal(option, "tun")) {
		connman_warn("bad OpenVPN.DeviceType value "
					"falling back to tun");
	}

	return IFF_TUN;
}

static int ov_route_env_parse(struct vpn_provider *provider, const char *key,
					int *family, unsigned long *idx,
					enum vpn_provider_route_type *type)
{
	char *end;
	const char *start;

	if (g_str_has_prefix(key, "route_network_")) {
		start = key + strlen("route_network_");
		*type = VPN_PROVIDER_ROUTE_TYPE_ADDR;
	} else if (g_str_has_prefix(key, "route_netmask_")) {
		start = key + strlen("route_netmask_");
		*type = VPN_PROVIDER_ROUTE_TYPE_MASK;
	} else if (g_str_has_prefix(key, "route_gateway_")) {
		start = key + strlen("route_gateway_");
		*type = VPN_PROVIDER_ROUTE_TYPE_GW;
	} else
		return -EINVAL;

	*family = AF_INET;
	*idx = g_ascii_strtoull(start, &end, 10);

	return 0;
}

static struct vpn_driver vpn_driver = {
	.notify	= ov_notify,
	.connect	= ov_connect,
	.disconnect	= ov_disconnect,
	.save		= ov_save,
	.device_flags = ov_device_flags,
	.route_env_parse = ov_route_env_parse,
};

static int openvpn_init(void)
{
	connection = connman_dbus_get_connection();

	return vpn_register("openvpn", &vpn_driver, OPENVPN);
}

static void openvpn_exit(void)
{
	vpn_unregister("openvpn");

	dbus_connection_unref(connection);
}

CONNMAN_PLUGIN_DEFINE(openvpn, "OpenVPN plugin", VERSION,
	CONNMAN_PLUGIN_PRIORITY_DEFAULT, openvpn_init, openvpn_exit)
