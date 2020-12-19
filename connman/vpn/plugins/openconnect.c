/*
 *
 *  ConnMan VPN daemon
 *
 *  Copyright (C) 2007-2013  Intel Corporation. All rights reserved.
 *  Copyright (C) 2019  Jolla Ltd. All rights reserved.
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
#include <net/if.h>

#include <glib.h>

#define CONNMAN_API_SUBJECT_TO_CHANGE
#include <connman/plugin.h>
#include <connman/log.h>
#include <connman/task.h>
#include <connman/ipconfig.h>
#include <connman/dbus.h>
#include <connman/agent.h>
#include <connman/setting.h>
#include <connman/vpn-dbus.h>

#include "../vpn-provider.h"
#include "../vpn-agent.h"

#include "vpn.h"

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
#define OC_MAX_READBUF_LEN 128

enum opt_type {
	OPT_STRING	= 0,
	OPT_BOOL	= 1,
};

struct {
	const char	*cm_opt;
	const char	*oc_opt;
	bool		has_value;
	bool		enabled; // Use as task parameter
	enum opt_type	type;
} oc_options[] = {
	{ "OpenConnect.AllowSelfSignedCert", NULL, 1, 0, OPT_BOOL},
	{ "OpenConnect.AuthType", NULL, 1, 0, OPT_STRING},
	{ "OpenConnect.CACert", "--cafile", 1, 1, OPT_STRING},
	{ "OpenConnect.ClientCert", NULL, 1, 0, OPT_STRING},
	{ "OpenConnect.DisableIPv6", "--disable-ipv6", 1, 1, OPT_BOOL},
	{ "OpenConnect.PKCSClientCert", NULL, 1, 0, OPT_STRING},
	{ "OpenConnect.Protocol", "--protocol", 1, 1, OPT_STRING},
	/* --no-cert-check is disabled in openconnect 8.02 */
	{ "OpenConnect.NoCertCheck", "--no-cert-check", 0, 0, OPT_BOOL},
	{ "OpenConnect.NoHTTPKeepalive", "--no-http-keepalive", 1, 1, OPT_BOOL},
	{ "OpenConnect.NoDTLS", "--no-dtls", 1, 1, OPT_BOOL},
	{ "OpenConnect.ServerCert", "--servercert", 1, 1, OPT_STRING},
	{ "OpenConnect.Usergroup", "--usergroup", 1, 1, OPT_STRING},
	{ "OpenConnect.UserPrivateKey", NULL, 1, 0, OPT_STRING},
	{ "VPN.MTU", "--base-mtu", 1, 1, OPT_STRING},
};

enum oc_connect_type {
	OC_CONNECT_COOKIE = 0,
	OC_CONNECT_COOKIE_WITH_USERPASS,
	OC_CONNECT_USERPASS,
	OC_CONNECT_PUBLICKEY,
	OC_CONNECT_PKCS,
};

static const char *connect_types[] = {"cookie", "cookie_with_userpass",
			"userpass", "publickey", "pkcs", NULL};
static const char *protocols[] = { "anyconnect", "nc", "gp", NULL};

struct oc_private_data {
	struct vpn_provider *provider;
	struct connman_task *task;
	char *if_name;
	char *dbus_sender;
	vpn_provider_connect_cb_t cb;
	void *user_data;
	int fd_in;
	int out_ch_id;
	int err_ch_id;
	GIOChannel *out_ch;
	GIOChannel *err_ch;
	enum oc_connect_type connect_type;
	bool interactive;
};

static bool is_valid_protocol(const char* protocol)
{
	if (!protocol || !*protocol)
		return false;

	return g_strv_contains(protocols, protocol);
}

static void oc_connect_done(struct oc_private_data *data, int err)
{
	connman_info("data %p err %d/%s", data, err, strerror(err));

	if (data && data->cb) {
		vpn_provider_connect_cb_t cb = data->cb;
		void *user_data = data->user_data;

		/* Make sure we don't invoke this callback twice */
		data->cb = NULL;
		data->user_data = NULL;
		cb(data->provider, user_data, err);
	}
}

static void close_io_channel(struct oc_private_data *data, GIOChannel *channel)
{
	int id = 0;

	connman_info("data %p channel %p", data, channel);

	if (!data || !channel)
		return;

	if (data->out_ch == channel) {
		id = data->out_ch_id;
		data->out_ch = NULL;
		data->out_ch_id = 0;
	} else if (data->err_ch == channel) {
		id = data->err_ch_id;
		data->err_ch = NULL;
		data->err_ch_id = 0;
	} else {
		return;
	}

	if (id)
		g_source_remove(id);

	g_io_channel_shutdown(channel, FALSE, NULL);
	g_io_channel_unref(channel);
}

static void free_private_data(struct oc_private_data *data)
{
	connman_info("data %p", data);

	if (!data || !data->provider)
		return;

	connman_info("provider %p", data->provider);

	if (vpn_provider_get_plugin_data(data->provider) == data)
		vpn_provider_set_plugin_data(data->provider, NULL);

	vpn_provider_unref(data->provider);

	if (data->fd_in > 0)
		close(data->fd_in);
	data->fd_in = -1;
	close_io_channel(data, data->out_ch);
	close_io_channel(data, data->err_ch);

	g_free(data->dbus_sender);
	g_free(data->if_name);
	g_free(data);
}

static int task_append_config_data(struct vpn_provider *provider,
					struct connman_task *task)
{
	const char *option = NULL;
	int i;

	for (i = 0; i < (int)ARRAY_SIZE(oc_options); i++) {
		if (!oc_options[i].oc_opt || !oc_options[i].enabled)
			continue;

		if (oc_options[i].has_value) {
			option = vpn_provider_get_string(provider,
						oc_options[i].cm_opt);
			if (!option)
				continue;

			/* Add boolean type values only if set as true. */
			if (oc_options[i].type == OPT_BOOL) {
				if (!vpn_provider_get_boolean(provider,
							oc_options[i].cm_opt,
							false))
					continue;

				/* No option is set for boolean type values. */
				option = NULL;
			}

			/* Skip protocol if it is invalid. */
			if (!g_strcmp0(oc_options[i].cm_opt,
						"OpenConnect.Protocol")) {
				if (!is_valid_protocol(option))
					continue;
			}
		}

		/*
		 * Add server certificate fingerprint only when self signed
		 * certificates are explicitly allowed. Using --servercert as
		 * parameter will accept any server with matching fingerprint,
		 * which would disregard the setting of AllowSelfSignedCert.
		 */
		if (!g_strcmp0(oc_options[i].cm_opt,
					"OpenConnect.ServerCert")) {
			if (!vpn_provider_get_boolean(provider,
					"OpenConnect.AllowSelfSignedCert",
					false))
				continue;
		}

		if (connman_task_add_argument(task,
				oc_options[i].oc_opt,
				oc_options[i].has_value ? option : NULL) < 0)
			return -EIO;
	}

	return 0;
}

static int oc_notify(DBusMessage *msg, struct vpn_provider *provider)
{
	DBusMessageIter iter, dict;
	const char *reason, *key, *value;
	char *domain = NULL;
	char *addressv4 = NULL, *addressv6 = NULL;
	char *netmask = NULL, *gateway = NULL;
	unsigned char prefix_len = 0;
	struct connman_ipaddress *ipaddress;
	struct oc_private_data *data;

	connman_info("provider %p", provider);

	data = vpn_provider_get_plugin_data(provider);

	dbus_message_iter_init(msg, &iter);

	dbus_message_iter_get_basic(&iter, &reason);
	dbus_message_iter_next(&iter);

	if (!provider) {
		connman_error("No provider found");
		oc_connect_done(data, ENOENT);
		return VPN_STATE_FAILURE;
	}

	if (strcmp(reason, "connect"))
		return VPN_STATE_DISCONNECT;

	domain = g_strdup(vpn_provider_get_string(provider, "VPN.Domain"));

	dbus_message_iter_recurse(&iter, &dict);

	while (dbus_message_iter_get_arg_type(&dict) == DBUS_TYPE_DICT_ENTRY) {
		DBusMessageIter entry;

		dbus_message_iter_recurse(&dict, &entry);
		dbus_message_iter_get_basic(&entry, &key);
		dbus_message_iter_next(&entry);
		dbus_message_iter_get_basic(&entry, &value);

		if (strcmp(key, "CISCO_CSTP_OPTIONS"))
			DBG("%s = %s", key, value);

		if (!strcmp(key, "VPNGATEWAY"))
			gateway = g_strdup(value);

		if (!strcmp(key, "INTERNAL_IP4_ADDRESS"))
			addressv4 = g_strdup(value);

		if (!strcmp(key, "INTERNAL_IP6_ADDRESS")) {
			addressv6 = g_strdup(value);
			prefix_len = 128;
		}

		if (!strcmp(key, "INTERNAL_IP4_NETMASK"))
			netmask = g_strdup(value);

		if (!strcmp(key, "INTERNAL_IP6_NETMASK")) {
			char *sep;

			/* The netmask contains the address and the prefix */
			sep = strchr(value, '/');
			if (sep) {
				unsigned char ip_len = sep - value;

				addressv6 = g_strndup(value, ip_len);
				prefix_len = (unsigned char)
						strtol(sep + 1, NULL, 10);
			}
		}

		if (!strcmp(key, "INTERNAL_IP4_DNS") ||
				!strcmp(key, "INTERNAL_IP6_DNS"))
			vpn_provider_set_nameservers(provider, value);

		if (!strcmp(key, "CISCO_PROXY_PAC"))
			vpn_provider_set_pac(provider, value);

		if (!domain && !strcmp(key, "CISCO_DEF_DOMAIN")) {
			g_free(domain);
			domain = g_strdup(value);
		}

		if (g_str_has_prefix(key, "CISCO_SPLIT_INC") ||
			g_str_has_prefix(key, "CISCO_IPV6_SPLIT_INC"))
			vpn_provider_append_route(provider, key, value);

		dbus_message_iter_next(&dict);
	}

	DBG("%p %p", addressv4, addressv6);

	if (addressv4)
		ipaddress = connman_ipaddress_alloc(AF_INET);
	else if (addressv6)
		ipaddress = connman_ipaddress_alloc(AF_INET6);
	else
		ipaddress = NULL;

	if (!ipaddress) {
		g_free(addressv4);
		g_free(addressv6);
		g_free(netmask);
		g_free(gateway);
		g_free(domain);

		return VPN_STATE_FAILURE;
	}

	if (addressv4)
		connman_ipaddress_set_ipv4(ipaddress, addressv4,
						netmask, gateway);
	else
		connman_ipaddress_set_ipv6(ipaddress, addressv6,
						prefix_len, gateway);

	connman_ipaddress_set_p2p(ipaddress, true);
	vpn_provider_set_ipaddress(provider, ipaddress);
	vpn_provider_set_domain(provider, domain);

	g_free(addressv4);
	g_free(addressv6);
	g_free(netmask);
	g_free(gateway);
	g_free(domain);
	connman_ipaddress_free(ipaddress);

	oc_connect_done(data, 0);
	return VPN_STATE_CONNECT;
}

static ssize_t full_write(int fd, const void *buf, size_t len)
{
	ssize_t byte_write;

	while (len) {
		byte_write = write(fd, buf, len);
		if (byte_write < 0) {
			connman_error("failed to write config to openconnect: "
					" %s\n", strerror(errno));
			return byte_write;
		}
		len -= byte_write;
		buf += byte_write;
	}

	return len;
}

static ssize_t write_data(int fd, const char *data)
{
	gchar *buf;
	ssize_t len;

	if (!data || !*data)
		return -1;

	buf = g_strdup_printf("%s\n", data);

	len = full_write(fd, buf, strlen(buf));

	g_free(buf);

	return len;
}

static void oc_died(struct connman_task *task, int exit_code, void *user_data)
{
	struct oc_private_data *data = user_data;

	connman_info("task %p data %p exit_code %d user_data %p", task, data,
				exit_code, user_data);

	if (!data)
		return;

	if (data->provider) {
		connman_agent_cancel(data->provider);

		if (task)
			vpn_died(task, exit_code, data->provider);
	}

	free_private_data(data);
}

static gboolean io_channel_out_cb(GIOChannel *source, GIOCondition condition,
			gpointer user_data)
{
	struct oc_private_data *data;
	char *str;

	data = user_data;

	if (data->out_ch != source)
		return G_SOURCE_REMOVE;

	if ((condition & G_IO_IN) &&
		g_io_channel_read_line(source, &str, NULL, NULL, NULL) ==
							G_IO_STATUS_NORMAL) {

		g_strchomp(str);

		/* Only cookie is printed to stdout */
		vpn_provider_set_string_hide_value(data->provider,
					"OpenConnect.Cookie", str);

		g_free(str);
	} else if (condition & (G_IO_ERR | G_IO_HUP)) {
		connman_info("Out channel termination");
		close_io_channel(data, source);
		return G_SOURCE_REMOVE;
	}

	return G_SOURCE_CONTINUE;
}

static bool strv_contains_prefix(const char *strv[], const char *str)
{
	int i;

	if (!strv || !str || !*str)
		return false;

	for (i = 0; strv[i]; i++) {
		if (g_str_has_prefix(str, strv[i]))
			return true;
	}

	return false;
}

static void clear_provider_credentials(struct vpn_provider *provider)
{
	const char *keys[] = { "OpenConnect.Username",
				"OpenConnect.Password",
				"OpenConnect.PKCSPassword",
				"OpenConnect.Cookie",
				NULL
	};
	int i;

	connman_info("provider %p", provider);

	for (i = 0; keys[i]; i++) {
		if (!vpn_provider_get_string_immutable(provider, keys[i]))
			vpn_provider_set_string_hide_value(provider, keys[i],
						"-");
	}
}

typedef void (* request_input_reply_cb_t) (DBusMessage *reply,
					void *user_data);

static int request_input_credentials(struct oc_private_data *data,
			request_input_reply_cb_t cb);


static void request_input_pkcs_reply(DBusMessage *reply, void *user_data)
{
	struct oc_private_data *data = user_data;
	const char *password = NULL;
	const char *key;
	DBusMessageIter iter, dict;
	int err;

	connman_info("provider %p", data->provider);

	if (!reply)
		goto err;

	err = vpn_agent_check_and_process_reply_error(reply, data->provider,
				data->task, data->cb, data->user_data);
	if (err) {
		/* Ensure cb is called only once */
		data->cb = NULL;
		data->user_data = NULL;
		goto err;
	}

	if (!vpn_agent_check_reply_has_dict(reply))
		goto err;

	dbus_message_iter_init(reply, &iter);
	dbus_message_iter_recurse(&iter, &dict);
	while (dbus_message_iter_get_arg_type(&dict) == DBUS_TYPE_DICT_ENTRY) {
		DBusMessageIter entry, value;

		dbus_message_iter_recurse(&dict, &entry);
		if (dbus_message_iter_get_arg_type(&entry) != DBUS_TYPE_STRING)
			break;

		dbus_message_iter_get_basic(&entry, &key);

		if (g_str_equal(key, "OpenConnect.PKCSPassword")) {
			dbus_message_iter_next(&entry);
			if (dbus_message_iter_get_arg_type(&entry)
							!= DBUS_TYPE_VARIANT)
				break;
			dbus_message_iter_recurse(&entry, &value);
			if (dbus_message_iter_get_arg_type(&value)
							!= DBUS_TYPE_STRING)
				break;
			dbus_message_iter_get_basic(&value, &password);
			vpn_provider_set_string_hide_value(data->provider, key,
						password);
		}

		dbus_message_iter_next(&dict);
	}

	if (data->connect_type != OC_CONNECT_PKCS || !password)
		goto err;

	if (write_data(data->fd_in, password) != 0) {
		connman_error("openconnect failed to take PKCS pass phrase on"
					" stdin");
		goto err;
	}

	clear_provider_credentials(data->provider);

	return;
err:
	oc_connect_done(data, EACCES);
}

static gboolean io_channel_err_cb(GIOChannel *source, GIOCondition condition,
			gpointer user_data)
{
	struct oc_private_data *data;
	const char *auth_failures[] = {
				/* Login failed */
				"Got HTTP response: HTTP/1.1 401 Unauthorized",
				"Failed to obtain WebVPN cookie",
				/* Cookie not valid */
				"Got inappropriate HTTP CONNECT response: "
						"HTTP/1.1 401 Unauthorized",
				/* Invalid cookie */
				"VPN service unavailable",
				/* Problem with certificates */
				"SSL connection failure",
				"Creating SSL connection failed",
				"SSL connection cancelled",
				NULL
	};
	const char *conn_failures[] = {
				"Failed to connect to",
				"Failed to open HTTPS connection to",
				NULL
	};
	/* Handle both PKCS#12 and PKCS#8 failures */
	const char *pkcs_failures[] = {
				"Failed to decrypt PKCS#12 certificate file",
				"Failed to decrypt PKCS#8 certificate file",
				NULL
	};
	/* Handle both PKCS#12 and PKCS#8 requests */
	const char *pkcs_requests[] = {
				"Enter PKCS#12 pass phrase",
				"Enter PKCS#8 pass phrase",
				NULL
	};
	const char *server_key_hash = "    --servercert";
	char *str;
	bool close = false;
	int err = 0;

	data = user_data;

	if (!data)
		return G_SOURCE_REMOVE;

	if (source && data->err_ch != source)
		return G_SOURCE_REMOVE;

	if ((condition & G_IO_IN)) {
		gsize len;
		int pos;

		if (!data->interactive) {
			if (g_io_channel_read_line(source, &str, &len, NULL,
						NULL) != G_IO_STATUS_NORMAL)
				err = EIO;
			else
				str[len - 1] = '\0';
		} else {
			GIOStatus status;
			str = g_try_new0(char, OC_MAX_READBUF_LEN);
			if (!str)
				return G_SOURCE_REMOVE;

			for (pos = 0; pos < OC_MAX_READBUF_LEN - 1 ; ++pos) {
				status = g_io_channel_read_chars(source,
						str+pos, 1, &len, NULL);

				if (status == G_IO_STATUS_EOF) {
					break;
				} else if (status != G_IO_STATUS_NORMAL) {
					err = EIO;
					break;
				}

				/* Ignore control chars and digits at start */
				if (!pos && (g_ascii_iscntrl(str[pos]) ||
						g_ascii_isdigit(str[pos])))
					--pos;

				/* Read zero length or no more to read */
				if (!len || g_io_channel_get_buffer_condition(
							source) != G_IO_IN ||
							str[pos] == '\n')
					break;
			}

			/*
			 * When self signed certificates are allowed and server
			 * SHA1 fingerprint is printed to stderr there is a
			 * newline char at the end of SHA1 fingerprint.
			 */
			if (str[pos] == '\n')
				str[pos] = '\0';
		}

		connman_info("openconnect: %s", str);

		if (err || !str || !*str) {
			connman_info("error reading from openconnect");
		} else if (g_str_has_prefix(str, server_key_hash)) {
			const char *fingerprint;
			int position;
			bool allow_self_signed;

			allow_self_signed = vpn_provider_get_boolean(
					data->provider,
					"OpenConnect.AllowSelfSignedCert",
					false);

			if (allow_self_signed) {
				position = strlen(server_key_hash) + 1;
				fingerprint = g_strstrip(str + position);

				connman_info("Set server key hash: \"%s\"",
							fingerprint);

				vpn_provider_set_string(data->provider,
						"OpenConnect.ServerCert",
						fingerprint);

				/*
				 * OpenConnect waits for "yes" or "no" as
				 * response to certificate acceptance request.
				 */
				if (write_data(data->fd_in, "yes") != 0)
					connman_error("openconnect: cannot "
						"write answer to certificate "
						"accept request");

			} else {
				connman_warn("Self signed certificate is not "
							" allowed");

				/*
				 * Close IO channel to avoid deadlock as an
				 * answer is expected for the certificate
				 * accept request.
				 */
				close = true;
				err = ECONNREFUSED;
			}
		} else if (strv_contains_prefix(pkcs_failures, str)) {
			connman_warn("PKCS failure: %s", str);
			close = true;
			err = EACCES;
		} else if (strv_contains_prefix(pkcs_requests, str)) {
			connman_info("PKCS file pass phrase request: %s", str);
			err = request_input_credentials(data,
						request_input_pkcs_reply);

			if (err != -EINPROGRESS) {
				err = EACCES;
				close = true;
			} else {
				err = 0;
			}
		} else if (strv_contains_prefix(auth_failures, str)) {
			connman_warn("authentication failed: %s", str);
			err = EACCES;
		} else if (strv_contains_prefix(conn_failures, str)) {
			connman_warn("connection failed: %s", str);
			err = ECONNREFUSED;
		}

		g_free(str);
	} else if (condition & (G_IO_ERR | G_IO_HUP)) {
		connman_info("Err channel termination");
		close = true;
	}

	if (err) {
		switch (err) {
		case EACCES:
			clear_provider_credentials(data->provider);
			break;
		case ECONNREFUSED:
			/*
			 * This will trigger VPN_PROVIDER_ERROR_CONNECT_FAILED
			 * in vpn-provider.c:connect_cb().
			 */
		default:
			break;
		}

		oc_connect_done(data, err);
	}

	if (close) {
		close_io_channel(data, source);
		return G_SOURCE_REMOVE;
	}

	return G_SOURCE_CONTINUE;
}

static int run_connect(struct oc_private_data *data)
{
	struct vpn_provider *provider;
	struct connman_task *task;
	const char *vpnhost;
	const char *vpncookie = NULL;
	const char *username;
	const char *password = NULL;
	const char *certificate = NULL;
	const char *private_key;
	const char *setting_str;
	bool setting;
	bool use_stdout = false;
	int fd_out = -1;
	int fd_err;
	int err = 0;

	if (!data)
		return -EINVAL;

	provider = data->provider;
	task = data->task;

	connman_info("provider %p task %p", provider, task);

	switch (data->connect_type) {
	case OC_CONNECT_COOKIE:
		vpncookie = vpn_provider_get_string(provider,
					"OpenConnect.Cookie");
		if (!vpncookie || !g_strcmp0(vpncookie, "-")) {
			err = -EACCES;
			goto done;
		}

		connman_task_add_argument(task, "--cookie-on-stdin", NULL);
		break;
	case OC_CONNECT_COOKIE_WITH_USERPASS:
		vpncookie = vpn_provider_get_string(provider,
					"OpenConnect.Cookie");
		/* No cookie set yet, username and password used first */
		if (!vpncookie || !g_strcmp0(vpncookie, "-")) {
			username = vpn_provider_get_string(provider,
						"OpenConnect.Username");
			password = vpn_provider_get_string(provider,
						"OpenConnect.Password");
			if (!username || !password ||
						!g_strcmp0(username, "-") ||
						!g_strcmp0(password, "-")) {
				err = -EACCES;
				goto done;
			}

			connman_task_add_argument(task, "--cookieonly", NULL);
			connman_task_add_argument(task, "--user", username);
			connman_task_add_argument(task, "--passwd-on-stdin",
						NULL);

			/* Use stdout only when cookie is to be read. */
			use_stdout = true;
		} else {
			connman_task_add_argument(task, "--cookie-on-stdin",
						NULL);
		}

		break;
	case OC_CONNECT_USERPASS:
		username = vpn_provider_get_string(provider,
					"OpenConnect.Username");
		password = vpn_provider_get_string(provider,
					"OpenConnect.Password");
		if (!username || !password || !g_strcmp0(username, "-") ||
					!g_strcmp0(password, "-")) {
			err = -EACCES;
			goto done;
		}

		connman_task_add_argument(task, "--user", username);
		connman_task_add_argument(task, "--passwd-on-stdin", NULL);
		break;
	case OC_CONNECT_PUBLICKEY:
		certificate = vpn_provider_get_string(provider,
					"OpenConnect.ClientCert");
		private_key = vpn_provider_get_string(provider,
					"OpenConnect.UserPrivateKey");

		if (!certificate || !private_key) {
			err = -EACCES;
			goto done;
		}

		connman_task_add_argument(task, "--certificate", certificate);
		connman_task_add_argument(task, "--sslkey", private_key);
		break;
	case OC_CONNECT_PKCS:
		certificate = vpn_provider_get_string(provider,
					"OpenConnect.PKCSClientCert");
		if (!certificate) {
			err = -EACCES;
			goto done;
		}

		connman_task_add_argument(task, "--certificate", certificate);

		password = vpn_provider_get_string(data->provider,
					"OpenConnect.PKCSPassword");
		/* Add password only if it is has been set */
		if (!password || !g_strcmp0(password, "-"))
			break;

		connman_task_add_argument(task, "--passwd-on-stdin", NULL);
		break;
	}

	vpnhost = vpn_provider_get_string(provider, "OpenConnect.VPNHost");
	if (!vpnhost || !*vpnhost)
		vpnhost = vpn_provider_get_string(provider, "Host");

	task_append_config_data(provider, task);

	/*
	 * To clarify complex situation, if cookie is expected to be printed
	 * to stdout all other output must go to syslog. But with PKCS all
	 * output must be caught in order to get message about file decryption
	 * error. For this reason, the mode has to be interactive as well.
	 */
	switch (data->connect_type) {
	case OC_CONNECT_COOKIE:
		/* fall through */
	case OC_CONNECT_COOKIE_WITH_USERPASS:
		/* fall through */
	case OC_CONNECT_USERPASS:
		/* fall through */
	case OC_CONNECT_PUBLICKEY:
		connman_task_add_argument(task, "--syslog", NULL);

		setting = vpn_provider_get_boolean(provider,
					"OpenConnect.AllowSelfSignedCert",
					false);
		setting_str = vpn_provider_get_string(provider,
					"OpenConnect.ServerCert");

		/*
		 * Run in interactive mode if self signed certificates are
		 * allowed and there is no set server SHA1 fingerprint.
		 */
		if (setting_str || !setting)
			connman_task_add_argument(task, "--non-inter", NULL);
		else
			data->interactive = true;
		break;
	case OC_CONNECT_PKCS:
		data->interactive = true;
		break;
	}

	connman_task_add_argument(task, "--script", SCRIPTDIR "/vpn-script");

	connman_task_add_argument(task, "--interface", data->if_name);

	connman_task_add_argument(task, (char *)vpnhost, NULL);

	err = connman_task_run(task, oc_died, data, &data->fd_in, use_stdout ?
				&fd_out : NULL, &fd_err);
	if (err < 0) {
		err = -EIO;
		goto done;
	}

	switch (data->connect_type) {
	case OC_CONNECT_COOKIE:
		if (write_data(data->fd_in, vpncookie) != 0) {
			connman_error("openconnect failed to take cookie on "
						"stdin");
			err = -EIO;
		}

		break;
	case OC_CONNECT_USERPASS:
		if (write_data(data->fd_in, password) != 0) {
			connman_error("openconnect failed to take password on "
						"stdin");
			err = -EIO;
		}

		break;
	case OC_CONNECT_COOKIE_WITH_USERPASS:
		if (!vpncookie || !g_strcmp0(vpncookie, "-")) {
			if (write_data(data->fd_in, password) != 0) {
				connman_error("openconnect failed to take "
							"password on stdin");
				err = -EIO;
			}
		} else {
			if (write_data(data->fd_in, vpncookie) != 0) {
				connman_error("openconnect failed to take "
							"cookie on stdin");
				err = -EIO;
			}
		}

		break;
	case OC_CONNECT_PUBLICKEY:
		break;
	case OC_CONNECT_PKCS:
		if (!password || !g_strcmp0(password, "-"))
			break;

		if (write_data(data->fd_in, password) != 0) {
			connman_error("openconnect failed to take PKCS "
						"pass phrase on stdin");
			err = -EIO;
		}

		break;
	}

	if (err) {
		if (fd_out > 0)
			close(fd_out);

		if (fd_err > 0)
			close(fd_err);

		goto done;
	}

	err = -EINPROGRESS;

	if (use_stdout) {
		data->out_ch = g_io_channel_unix_new(fd_out);

		/* Use ASCII encoding only */
		if (g_io_channel_set_encoding(data->out_ch, NULL, NULL) !=
					G_IO_STATUS_NORMAL) {
			close_io_channel(data, data->out_ch);
			err = -EIO;
		} else {
			data->out_ch_id = g_io_add_watch(data->out_ch,
						G_IO_IN | G_IO_ERR | G_IO_HUP,
						(GIOFunc)io_channel_out_cb,
						data);
		}
	}

	data->err_ch = g_io_channel_unix_new(fd_err);

	/* Use ASCII encoding only */
	if (g_io_channel_set_encoding(data->err_ch, NULL, NULL) !=
				G_IO_STATUS_NORMAL) {
		close_io_channel(data, data->err_ch);
		err = -EIO;
	} else {
		data->err_ch_id = g_io_add_watch(data->err_ch,
					G_IO_IN | G_IO_ERR | G_IO_HUP,
					(GIOFunc)io_channel_err_cb, data);
	}

done:
	clear_provider_credentials(data->provider);

	return err;
}

static void request_input_append(DBusMessageIter *iter,
		const char *str_type, const char *str, void *user_data)
{
	const char *string;

	connman_dbus_dict_append_basic(iter, "Type",
				DBUS_TYPE_STRING, &str_type);
	connman_dbus_dict_append_basic(iter, "Requirement",
				DBUS_TYPE_STRING, &str);

	if (!user_data)
		return;

	string = user_data;
	connman_dbus_dict_append_basic(iter, "Value", DBUS_TYPE_STRING,
				&string);
}

static void request_input_append_informational(DBusMessageIter *iter,
		void *user_data)
{
	request_input_append(iter, "string", "informational", user_data);
}

static void request_input_append_mandatory(DBusMessageIter *iter,
		void *user_data)
{
	request_input_append(iter, "string", "mandatory", user_data);
}

static void request_input_append_optional(DBusMessageIter *iter,
		void *user_data)
{
	request_input_append(iter, "string", "optional", user_data);
}

static void request_input_append_password(DBusMessageIter *iter,
		void *user_data)
{
	request_input_append(iter, "password", "mandatory", user_data);
}

static void request_input_append_to_dict(struct vpn_provider *provider,
			DBusMessageIter *dict,
			connman_dbus_append_cb_t function_cb, const char *key)
{
	const char *str;
	bool immutable = false;

	if (!provider || !dict || !function_cb || !key)
		return;

	str = vpn_provider_get_string(provider, key);
	/* Ignore empty informational content */
	if (!str && function_cb == request_input_append_informational)
		return;

	/* If value is "-", it is cleared by VPN agent */
	if (!g_strcmp0(str, "-"))
		str = NULL;

	if (str)
		immutable = vpn_provider_get_string_immutable(provider, key);

	if (immutable) {
		/* Hide immutable password types */
		if (function_cb == request_input_append_password)
			str = "********";

		/* Send immutable as informational */
		function_cb = request_input_append_informational;
	}

	connman_dbus_dict_append_dict(dict, key, function_cb,
				str ? (void *)str : NULL);
}

static void request_input_credentials_reply(DBusMessage *reply, void *user_data)
{
	struct oc_private_data *data = user_data;
	const char *cookie = NULL;
	const char *servercert = NULL;
	const char *vpnhost = NULL;
	const char *username = NULL;
	const char *password = NULL;
	const char *pkcspassword = NULL;
	const char *key;
	DBusMessageIter iter, dict;
	int err;

	connman_info("provider %p", data->provider);

	if (!reply)
		goto err;

	err = vpn_agent_check_and_process_reply_error(reply, data->provider,
				data->task, data->cb, data->user_data);
	if (err) {
		/* Ensure cb is called only once */
		data->cb = NULL;
		data->user_data = NULL;
		goto out;
	}

	if (!vpn_agent_check_reply_has_dict(reply))
		goto err;

	dbus_message_iter_init(reply, &iter);
	dbus_message_iter_recurse(&iter, &dict);
	while (dbus_message_iter_get_arg_type(&dict) == DBUS_TYPE_DICT_ENTRY) {
		DBusMessageIter entry, value;

		dbus_message_iter_recurse(&dict, &entry);
		if (dbus_message_iter_get_arg_type(&entry) != DBUS_TYPE_STRING)
			break;

		dbus_message_iter_get_basic(&entry, &key);

		if (g_str_equal(key, "OpenConnect.Cookie")) {
			dbus_message_iter_next(&entry);
			if (dbus_message_iter_get_arg_type(&entry)
							!= DBUS_TYPE_VARIANT)
				break;
			dbus_message_iter_recurse(&entry, &value);
			if (dbus_message_iter_get_arg_type(&value)
							!= DBUS_TYPE_STRING)
				break;
			dbus_message_iter_get_basic(&value, &cookie);
			vpn_provider_set_string_hide_value(data->provider,
					key, cookie);
		} else if (g_str_equal(key, "OpenConnect.ServerCert")) {
			dbus_message_iter_next(&entry);
			if (dbus_message_iter_get_arg_type(&entry)
							!= DBUS_TYPE_VARIANT)
				break;
			dbus_message_iter_recurse(&entry, &value);
			if (dbus_message_iter_get_arg_type(&value)
							!= DBUS_TYPE_STRING)
				break;
			dbus_message_iter_get_basic(&value, &servercert);
			vpn_provider_set_string(data->provider, key,
					servercert);

		} else if (g_str_equal(key, "OpenConnect.VPNHost")) {
			dbus_message_iter_next(&entry);
			if (dbus_message_iter_get_arg_type(&entry)
							!= DBUS_TYPE_VARIANT)
				break;
			dbus_message_iter_recurse(&entry, &value);
			if (dbus_message_iter_get_arg_type(&value)
							!= DBUS_TYPE_STRING)
				break;
			dbus_message_iter_get_basic(&value, &vpnhost);
			vpn_provider_set_string(data->provider, key, vpnhost);
		} else if (g_str_equal(key, "Username")) {
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
					"OpenConnect.Username", username);
		} else if (g_str_equal(key, "Password")) {
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
					"OpenConnect.Password", password);
		} else if (g_str_equal(key, "OpenConnect.PKCSPassword")) {
			dbus_message_iter_next(&entry);
			if (dbus_message_iter_get_arg_type(&entry)
							!= DBUS_TYPE_VARIANT)
				break;
			dbus_message_iter_recurse(&entry, &value);
			if (dbus_message_iter_get_arg_type(&value)
							!= DBUS_TYPE_STRING)
				break;
			dbus_message_iter_get_basic(&value, &pkcspassword);
			vpn_provider_set_string_hide_value(data->provider, key,
						pkcspassword);
		}

		dbus_message_iter_next(&dict);
	}

	switch (data->connect_type) {
	case OC_CONNECT_COOKIE:
		if (!cookie)
			goto err;

		break;
	case OC_CONNECT_USERPASS:
		/* fall through */
	case OC_CONNECT_COOKIE_WITH_USERPASS:
		if (!username || !password)
			goto err;

		break;
	case OC_CONNECT_PUBLICKEY:
		break; // This should not be reached.
	case OC_CONNECT_PKCS:
		if (!pkcspassword)
			goto err;

		break;
	}

	err = run_connect(data);
	if (err != -EINPROGRESS)
		goto err;

	return;

err:
	oc_connect_done(data, EACCES);

out:
	free_private_data(data);
}

static int request_input_credentials(struct oc_private_data *data,
			request_input_reply_cb_t cb)
{
	DBusMessage *message;
	const char *path;
	const char *agent_sender;
	const char *agent_path;
	const char *username;
	DBusMessageIter iter;
	DBusMessageIter dict;
	int err;
	void *agent;

	if (!data || !cb)
		return -ESRCH;

	connman_info("provider %p", data->provider);

	agent = connman_agent_get_info(data->dbus_sender,
				&agent_sender, &agent_path);
	if (!data->provider || !agent || !agent_path)
		return -ESRCH;

	message = dbus_message_new_method_call(agent_sender, agent_path,
					VPN_AGENT_INTERFACE,
					"RequestInput");
	if (!message)
		return -ENOMEM;

	dbus_message_iter_init_append(message, &iter);

	path = vpn_provider_get_path(data->provider);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_OBJECT_PATH, &path);

	connman_dbus_dict_open(&iter, &dict);

	request_input_append_to_dict(data->provider, &dict,
				request_input_append_informational,
				"OpenConnect.CACert");

	/*
	 * For backwards compatibility add OpenConnect.ServerCert and
	 * OpenConnect.VPNHost as madnatory only in the default authentication
	 * mode. Otherwise. add the fields as informational. These should be
	 * set in provider settings and not to be queried with every connection
	 * attempt.
	 */
	request_input_append_to_dict(data->provider, &dict,
				data->connect_type == OC_CONNECT_COOKIE ?
				request_input_append_optional :
				request_input_append_informational,
				"OpenConnect.ServerCert");

	request_input_append_to_dict(data->provider, &dict,
				data->connect_type == OC_CONNECT_COOKIE ?
				request_input_append_optional :
				request_input_append_informational,
				"OpenConnect.VPNHost");

	if (vpn_provider_get_authentication_errors(data->provider))
		vpn_agent_append_auth_failure(&dict, data->provider, NULL);

	switch (data->connect_type) {
	case OC_CONNECT_COOKIE:
		request_input_append_to_dict(data->provider, &dict,
					request_input_append_mandatory,
					"OpenConnect.Cookie");
		break;
	/*
	 * The authentication is done with username and password to get the
	 * cookie for connection.
	 */
	case OC_CONNECT_COOKIE_WITH_USERPASS:
		/* fallthrough */
	case OC_CONNECT_USERPASS:
		username = vpn_provider_get_string(data->provider,
					"OpenConnect.Username");
		vpn_agent_append_user_info(&dict, data->provider, username);
		break;
	case OC_CONNECT_PUBLICKEY:
		return -EINVAL;
	case OC_CONNECT_PKCS:
		request_input_append_to_dict(data->provider, &dict,
				request_input_append_informational,
				"OpenConnect.PKCSClientCert");

		request_input_append_to_dict(data->provider, &dict,
					request_input_append_password,
					"OpenConnect.PKCSPassword");
		break;
	}

	vpn_agent_append_host_and_name(&dict, data->provider);

	connman_dbus_dict_close(&iter, &dict);

	err = connman_agent_queue_message(data->provider, message,
			connman_timeout_input_request(), cb, data, agent);

	dbus_message_unref(message);

	if (err < 0 && err != -EBUSY) {
		connman_error("cannot send agent request, error: %d", err);
		return err;
	}

	return -EINPROGRESS;
}

static enum oc_connect_type get_authentication_type(
			struct vpn_provider *provider)
{
	const char *auth;
	enum oc_connect_type type;

	auth = vpn_provider_get_string(provider, "OpenConnect.AuthType");
	if (!auth)
		goto out;

	for (type = 0; connect_types[type]; type++) {
		if (!g_strcmp0(auth, connect_types[type])) {
			connman_info("auth type %d/%s", type,
						connect_types[type]);
			return type;
		}
	}

out:
	/* Default to cookie */
	return OC_CONNECT_COOKIE;
}

static int oc_connect(struct vpn_provider *provider,
			struct connman_task *task, const char *if_name,
			vpn_provider_connect_cb_t cb,
			const char *dbus_sender, void *user_data)
{
	struct oc_private_data *data;
	const char *vpncookie;
	const char *certificate;
	const char *username;
	const char *password;
	const char *private_key;
	int err;

	connman_info("provider %p task %p", provider, task);

	data = g_try_new0(struct oc_private_data, 1);
	if (!data)
		return -ENOMEM;

	vpn_provider_set_plugin_data(provider, data);
	data->provider = vpn_provider_ref(provider);
	data->task = task;
	data->if_name = g_strdup(if_name);
	data->dbus_sender = g_strdup(dbus_sender);
	data->cb = cb;
	data->user_data = user_data;
	data->connect_type = get_authentication_type(provider);

	switch (data->connect_type) {
	case OC_CONNECT_COOKIE:
		vpncookie = vpn_provider_get_string(provider,
					"OpenConnect.Cookie");
		if (!vpncookie || !g_strcmp0(vpncookie, "-"))
			goto request_input;

		break;
	case OC_CONNECT_USERPASS:
		username = vpn_provider_get_string(provider,
					"OpenConnect.Username");
		password = vpn_provider_get_string(provider,
					"OpenConnect.Password");
		if (!username || !password || !g_strcmp0(username, "-") ||
					!g_strcmp0(password, "-"))
			goto request_input;

		break;
	case OC_CONNECT_COOKIE_WITH_USERPASS:
		vpncookie = vpn_provider_get_string(provider,
					"OpenConnect.Cookie");
		/* Username and password must be set if cookie is missing */
		if (!vpncookie) {
			username = vpn_provider_get_string(provider,
						"OpenConnect.Username");
			password = vpn_provider_get_string(provider,
						"OpenConnect.Password");

			if (!username || !password ||
						!g_strcmp0(username, "-") ||
						!g_strcmp0(password, "-"))
				goto request_input;
		} else if (!g_strcmp0(vpncookie, "-")) {
			goto request_input;
		}

		break;
	case OC_CONNECT_PUBLICKEY:
		certificate = vpn_provider_get_string(provider,
				"OpenConnect.ClientCert");
		private_key = vpn_provider_get_string(provider,
				"OpenConnect.UserPrivateKey");

		if (!certificate || !private_key) {
			connman_warn("missing certificate and/or private key");
			oc_connect_done(data, EACCES);
			free_private_data(data);
			return -EACCES;
		}

		break;
	case OC_CONNECT_PKCS:
		certificate = vpn_provider_get_string(provider,
					"OpenConnect.PKCSClientCert");
		if (!certificate) {
			connman_warn("missing PKCS certificate");
			oc_connect_done(data, EACCES);
			free_private_data(data);
			return -EACCES;
		}

		break;
	}

	return run_connect(data);

request_input:
	err = request_input_credentials(data, request_input_credentials_reply);
	if (err != -EINPROGRESS) {
		oc_connect_done(data, err);
		vpn_provider_indicate_error(data->provider,
					VPN_PROVIDER_ERROR_LOGIN_FAILED);
		free_private_data(data);
	}

	return err;
}

static void oc_disconnect(struct vpn_provider *provider)
{
	connman_info("provider %p", provider);

	if (!provider)
		return;

	/*
	* OpenConnect may be disconnect by timeout in connmand before running
	* the openconnect process. In such case it is important to cancel the
	* agent request to avoid having multiple ones visible.
	*/
	connman_agent_cancel(provider);
}

static int oc_save(struct vpn_provider *provider, GKeyFile *keyfile)
{
	const char *save_group;
	const char *option;
	int i;

	save_group = vpn_provider_get_save_group(provider);

	for (i = 0; i < (int)ARRAY_SIZE(oc_options); i++) {
		if (strncmp(oc_options[i].cm_opt, "OpenConnect.", 12) == 0) {
			option = vpn_provider_get_string(provider,
							oc_options[i].cm_opt);
			if (!option)
				continue;

			g_key_file_set_string(keyfile, save_group,
					oc_options[i].cm_opt, option);
		}
	}

	return 0;
}

static int oc_error_code(struct vpn_provider *provider, int exit_code)
{
	connman_info("%d", exit_code);

	/* OpenConnect process return values are ambiguous in definition
	 * https://github.com/openconnect/openconnect/blob/master/main.c#L1693
	 * and it is safer not to rely on them. Login error cannot be
	 * differentiated from connection errors, e.g., when self signed
	 * certificate is rejected by user setting.
	 */

	switch (exit_code) {
	case 2:
		/* Cookie has failed */
		clear_provider_credentials(provider);
		return VPN_PROVIDER_ERROR_LOGIN_FAILED;
	case 1:
		/* fall through */
	default:
		return VPN_PROVIDER_ERROR_UNKNOWN;
	}
}

static int oc_route_env_parse(struct vpn_provider *provider, const char *key,
		int *family, unsigned long *idx, enum vpn_provider_route_type *type)
{
	char *end;
	const char *start;

	if (g_str_has_prefix(key, "CISCO_SPLIT_INC_")) {
		*family = AF_INET;
		start = key + strlen("CISCO_SPLIT_INC_");
	} else if (g_str_has_prefix(key, "CISCO_IPV6_SPLIT_INC_")) {
		*family = AF_INET6;
		start = key + strlen("CISCO_IPV6_SPLIT_INC_");
	} else
		return -EINVAL;

	*idx = g_ascii_strtoull(start, &end, 10);

	if (strncmp(end, "_ADDR", 5) == 0)
		*type = VPN_PROVIDER_ROUTE_TYPE_ADDR;
	else if (strncmp(end, "_MASK", 5) == 0)
		*type = VPN_PROVIDER_ROUTE_TYPE_MASK;
	else if (strncmp(end, "_MASKLEN", 8) == 0 && *family == AF_INET6)
		*type = VPN_PROVIDER_ROUTE_TYPE_MASK;
	else
		return -EINVAL;

	return 0;
}

static struct vpn_driver vpn_driver = {
	.notify         = oc_notify,
	.connect	= oc_connect,
	.disconnect	= oc_disconnect,
	.error_code	= oc_error_code,
	.save		= oc_save,
	.route_env_parse = oc_route_env_parse,
};

static int openconnect_init(void)
{
	return vpn_register("openconnect", &vpn_driver, OPENCONNECT);
}

static void openconnect_exit(void)
{
	vpn_unregister("openconnect");
}

CONNMAN_PLUGIN_DEFINE(openconnect, "OpenConnect VPN plugin", VERSION,
	CONNMAN_PLUGIN_PRIORITY_DEFAULT, openconnect_init, openconnect_exit)
