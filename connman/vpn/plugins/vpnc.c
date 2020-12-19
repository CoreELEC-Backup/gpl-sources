/*
 *
 *  ConnMan VPN daemon
 *
 *  Copyright (C) 2010,2013  BMW Car IT GmbH.
 *  Copyright (C) 2010,2012-2013  Intel Corporation. All rights reserved.
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

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <net/if.h>
#include <linux/if_tun.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>

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
#include "../vpn.h"

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
#define PID_PATH_ROOT "/var/run/user"

enum {
	OPT_STRING = 1,
	OPT_BOOLEAN = 2,
};

struct {
	const char *cm_opt;
	const char *vpnc_opt;
	const char *vpnc_default;
	int type;
	bool cm_save;
} vpnc_options[] = {
	{ "Host", "IPSec gateway", NULL, OPT_STRING, true },
	{ "VPNC.IPSec.ID", "IPSec ID", NULL, OPT_STRING, true },
	{ "VPNC.IPSec.Secret", "IPSec secret", NULL, OPT_STRING, false },
	{ "VPNC.Xauth.Username", "Xauth username", NULL, OPT_STRING, false },
	{ "VPNC.Xauth.Password", "Xauth password", NULL, OPT_STRING, false },
	{ "VPNC.IKE.Authmode", "IKE Authmode", NULL, OPT_STRING, true },
	{ "VPNC.IKE.DHGroup", "IKE DH Group", NULL, OPT_STRING, true },
	{ "VPNC.PFS", "Perfect Forward Secrecy", NULL, OPT_STRING, true },
	{ "VPNC.Domain", "Domain", NULL, OPT_STRING, true },
	{ "VPNC.Vendor", "Vendor", NULL, OPT_STRING, true },
	{ "VPNC.LocalPort", "Local Port", "0", OPT_STRING, true, },
	{ "VPNC.CiscoPort", "Cisco UDP Encapsulation Port", "0", OPT_STRING,
									true },
	{ "VPNC.AppVersion", "Application version", NULL, OPT_STRING, true },
	{ "VPNC.NATTMode", "NAT Traversal Mode", "cisco-udp", OPT_STRING,
									true },
	{ "VPNC.DPDTimeout", "DPD idle timeout (our side)", NULL, OPT_STRING,
									true },
	{ "VPNC.SingleDES", "Enable Single DES", NULL, OPT_BOOLEAN, true },
	{ "VPNC.NoEncryption", "Enable no encryption", NULL, OPT_BOOLEAN,
									true },
};

struct vc_private_data {
	struct vpn_provider *provider;
	struct connman_task *task;
	char *if_name;
	vpn_provider_connect_cb_t cb;
	void *user_data;
	int err_ch_id;
	GIOChannel *err_ch;
};

static void vc_connect_done(struct vc_private_data *data, int err)
{
	DBG("data %p err %d", data, err);

	if (data && data->cb) {
		vpn_provider_connect_cb_t cb = data->cb;
		void *user_data = data->user_data;

		/* Make sure we don't invoke this callback twice */
		data->cb = NULL;
		data->user_data = NULL;
		cb(data->provider, user_data, err);
	}
}

static void close_io_channel(struct vc_private_data *data, GIOChannel *channel)
{
	if (!data || !channel)
		return;

	if (data->err_ch == channel) {
		DBG("closing stderr");

		if (data->err_ch_id) {
			g_source_remove(data->err_ch_id);
			data->err_ch_id = 0;
		}

		if (!data->err_ch)
			return;

		g_io_channel_shutdown(data->err_ch, FALSE, NULL);
		g_io_channel_unref(data->err_ch);

		data->err_ch = NULL;
	}
}

static void free_private_data(struct vc_private_data *data)
{
	DBG("data %p", data);

	if (!data || !data->provider)
		return;

	DBG("provider %p", data->provider);

	if (vpn_provider_get_plugin_data(data->provider) == data)
		vpn_provider_set_plugin_data(data->provider, NULL);

	vpn_provider_unref(data->provider);

	g_free(data->if_name);
	g_free(data);
}

static int vc_notify(DBusMessage *msg, struct vpn_provider *provider)
{
	DBusMessageIter iter, dict;
	char *address = NULL, *netmask = NULL, *gateway = NULL;
	struct connman_ipaddress *ipaddress;
	const char *reason, *key, *value;
	struct vc_private_data *data;
	int type;

	data = vpn_provider_get_plugin_data(provider);

	dbus_message_iter_init(msg, &iter);

	type = dbus_message_iter_get_arg_type(&iter);
	if (type != DBUS_TYPE_STRING) {
		DBG("invalid D-Bus arg type %d", type);
		return VPN_STATE_FAILURE;
	}

	dbus_message_iter_get_basic(&iter, &reason);
	dbus_message_iter_next(&iter);

	if (!provider) {
		connman_error("No provider found");
		vc_connect_done(data, ENOENT);
		return VPN_STATE_FAILURE;
	}

	if (g_strcmp0(reason, "connect")) {
		vc_connect_done(data, EIO);
		return VPN_STATE_DISCONNECT;
	}

	dbus_message_iter_recurse(&iter, &dict);

	while (dbus_message_iter_get_arg_type(&dict) == DBUS_TYPE_DICT_ENTRY) {
		DBusMessageIter entry;

		dbus_message_iter_recurse(&dict, &entry);

		type = dbus_message_iter_get_arg_type(&entry);
		if (type != DBUS_TYPE_STRING)
			continue;

		dbus_message_iter_get_basic(&entry, &key);
		dbus_message_iter_next(&entry);

		type = dbus_message_iter_get_arg_type(&entry);
		if (type != DBUS_TYPE_STRING)
			continue;

		dbus_message_iter_get_basic(&entry, &value);

		DBG("%s = %s", key, value);

		if (!strcmp(key, "VPNGATEWAY"))
			gateway = g_strdup(value);

		if (!strcmp(key, "INTERNAL_IP4_ADDRESS"))
			address = g_strdup(value);

		if (!strcmp(key, "INTERNAL_IP4_NETMASK"))
			netmask = g_strdup(value);

		if (!strcmp(key, "INTERNAL_IP4_DNS"))
			vpn_provider_set_nameservers(provider, value);

		if (!strcmp(key, "CISCO_DEF_DOMAIN"))
			vpn_provider_set_domain(provider, value);

		if (g_str_has_prefix(key, "CISCO_SPLIT_INC") ||
			g_str_has_prefix(key, "CISCO_IPV6_SPLIT_INC"))
			vpn_provider_append_route(provider, key, value);

		dbus_message_iter_next(&dict);
	}


	ipaddress = connman_ipaddress_alloc(AF_INET);
	if (!ipaddress) {
		g_free(address);
		g_free(netmask);
		g_free(gateway);
		vc_connect_done(data, EIO);
		return VPN_STATE_FAILURE;
	}

	connman_ipaddress_set_ipv4(ipaddress, address, netmask, gateway);
	connman_ipaddress_set_p2p(ipaddress, true);
	vpn_provider_set_ipaddress(provider, ipaddress);

	g_free(address);
	g_free(netmask);
	g_free(gateway);
	connman_ipaddress_free(ipaddress);

	vc_connect_done(data, 0);
	return VPN_STATE_CONNECT;
}

static ssize_t full_write(int fd, const void *buf, size_t len)
{
	ssize_t byte_write;

	while (len) {
		byte_write = write(fd, buf, len);
		if (byte_write < 0) {
			connman_error("failed to write config to vpnc: %s\n",
					strerror(errno));
			return byte_write;
		}
		len -= byte_write;
		buf += byte_write;
	}

	return 0;
}

static ssize_t write_option(int fd, const char *key, const char *value)
{
	gchar *buf;
	ssize_t ret = 0;

	if (key && value) {
		buf = g_strdup_printf("%s %s\n", key, value);
		ret = full_write(fd, buf, strlen(buf));

		g_free(buf);
	}

	return ret;
}

static ssize_t write_bool_option(int fd, const char *key, const char *value)
{
	gchar *buf;
	ssize_t ret = 0;

	if (key && value) {
		if (strcasecmp(value, "yes") == 0 ||
				strcasecmp(value, "true") == 0 ||
				strcmp(value, "1") == 0) {
			buf = g_strdup_printf("%s\n", key);
			ret = full_write(fd, buf, strlen(buf));

			g_free(buf);
		}
	}

	return ret;
}

static int vc_write_config_data(struct vpn_provider *provider, int fd)
{
	const char *opt_s;
	int i;

	for (i = 0; i < (int)ARRAY_SIZE(vpnc_options); i++) {
		opt_s = vpn_provider_get_string(provider,
					vpnc_options[i].cm_opt);
		if (!opt_s)
			opt_s = vpnc_options[i].vpnc_default;

		if (!opt_s)
			continue;

		if (vpnc_options[i].type == OPT_STRING) {
			if (write_option(fd,
					vpnc_options[i].vpnc_opt, opt_s) < 0)
				return -EIO;
		} else if (vpnc_options[i].type == OPT_BOOLEAN) {
			if (write_bool_option(fd,
					vpnc_options[i].vpnc_opt, opt_s) < 0)
				return -EIO;
		}

	}

	return 0;
}

static int vc_save(struct vpn_provider *provider, GKeyFile *keyfile)
{
	const char *option;
	int i;

	for (i = 0; i < (int)ARRAY_SIZE(vpnc_options); i++) {
		if (strncmp(vpnc_options[i].cm_opt, "VPNC.", 5) == 0) {

			if (!vpnc_options[i].cm_save)
				continue;

			option = vpn_provider_get_string(provider,
							vpnc_options[i].cm_opt);
			if (!option)
				continue;

			g_key_file_set_string(keyfile,
					vpn_provider_get_save_group(provider),
					vpnc_options[i].cm_opt, option);
		}
	}
	return 0;
}

static void vc_died(struct connman_task *task, int exit_code, void *user_data)
{
	struct vc_private_data *data = user_data;

	DBG("task %p data %p exit_code %d user_data %p", task, data, exit_code,
				user_data);

	if (!data)
		return;

	if (data->provider) {
		connman_agent_cancel(data->provider);

		if (task)
			vpn_died(task, exit_code, data->provider);
	}

	free_private_data(data);
}

static gboolean io_channel_cb(GIOChannel *source, GIOCondition condition,
			gpointer user_data)
{
	struct vc_private_data *data;
	const char *auth_failures[] = {
			VPNC ": hash comparison failed",
			VPNC ": authentication unsuccessful",
			VPNC ": expected xauth packet; rejected",
			NULL
	};
	const char *conn_failures[] = {
			VPNC ": unknown host",
			VPNC ": no response from target",
			VPNC ": receiving packet: No route to host",
			NULL
	};
	char *str;
	int i;

	data = user_data;

	if ((condition & G_IO_IN) &&
		g_io_channel_read_line(source, &str, NULL, NULL, NULL) ==
							G_IO_STATUS_NORMAL) {
		str[strlen(str) - 1] = '\0';

		for (i = 0; auth_failures[i]; i++) {
			if (g_str_has_prefix(str, auth_failures[i])) {
				DBG("authentication failed: %s", str);

				vpn_provider_indicate_error(data->provider,
					VPN_PROVIDER_ERROR_AUTH_FAILED);
			}
		}

		for (i = 0; conn_failures[i]; i++) {
			if (g_str_has_prefix(str, conn_failures[i])) {
				DBG("connection failed: %s", str);

				vpn_provider_indicate_error(data->provider,
					VPN_PROVIDER_ERROR_CONNECT_FAILED);
			}
		}

		g_free(str);
	} else if (condition & (G_IO_ERR | G_IO_HUP)) {
		DBG("Channel termination");
		close_io_channel(data, source);
		return G_SOURCE_REMOVE;
	}

	return G_SOURCE_CONTINUE;
}

static char *create_pid_path(const char *user, const char *group)
{
	struct passwd *pwd;
	struct group *grp;
	char *uid_str;
	char *pid_path = NULL;
	int mode = S_IRWXU|S_IRWXG;
	gid_t gid;

	if (!user || !*user)
		return NULL;

	if (vpn_settings_is_system_user(user))
		return NULL;

	pwd = vpn_util_get_passwd(user);
	uid_str = g_strdup_printf("%d", pwd->pw_uid);

	grp = vpn_util_get_group(group);
	gid = grp ? grp->gr_gid : pwd->pw_gid;

	pid_path = g_build_filename(PID_PATH_ROOT, uid_str, "vpnc", "pid",
				NULL);
	if (vpn_util_create_path(pid_path, pwd->pw_uid, gid, mode)) {
		g_free(pid_path);
		pid_path = NULL;
	}

	g_free(uid_str);

	return pid_path;
}

static int run_connect(struct vc_private_data *data)
{
	struct vpn_provider *provider;
	struct connman_task *task;
	struct vpn_plugin_data *plugin_data;
	const char *credentials[] = {"VPNC.IPSec.Secret", "VPNC.Xauth.Username",
				"VPNC.Xauth.Password", NULL};
	const char *if_name;
	const char *option;
	char *pid_path;
	int err;
	int fd_in;
	int fd_err;
	int i;

	provider = data->provider;
	task = data->task;
	if_name = data->if_name;

	DBG("provider %p task %p interface %s user_data %p", provider, task,
				if_name, data->user_data);

	/*
	 * Change to use C locale, options should be in ASCII according to
	 * documentation. To be on the safe side, set both LANG and LC_ALL.
	 * This is required especially when the VPNC processe is ran using an
	 * user other than root.
	 */
	connman_task_add_variable(task,"LANG", "C");
	connman_task_add_variable(task,"LC_ALL", "C");

	connman_task_add_argument(task, "--non-inter", NULL);
	connman_task_add_argument(task, "--no-detach", NULL);

	connman_task_add_argument(task, "--ifname", if_name);
	option = vpn_provider_get_string(provider, "VPNC.DeviceType");
	if (option) {
		connman_task_add_argument(task, "--ifmode", option);
	} else {
		/*
		 * Default to tun for backwards compatibility.
		 */
		connman_task_add_argument(task, "--ifmode", "tun");
	}

	plugin_data = vpn_settings_get_vpn_plugin_config("vpnc");

	option = vpn_settings_get_binary_user(plugin_data);
	if (option) {
		pid_path = create_pid_path(option,
					vpn_settings_get_binary_group(
						plugin_data));
		if (pid_path)
			connman_task_add_argument(task, "--pid-file",
								pid_path);

		g_free(pid_path);
	}

	connman_task_add_argument(task, "--script", SCRIPTDIR "/vpn-script");

	option = vpn_provider_get_string(provider, "VPNC.Debug");
	if (option)
		connman_task_add_argument(task, "--debug", option);

	connman_task_add_argument(task, "-", NULL);

	err = connman_task_run(data->task, vc_died, data, &fd_in, NULL,
				&fd_err);
	if (err < 0) {
		connman_error("vpnc failed to start");
		err = -EIO;
		goto done;
	}

	err = vc_write_config_data(provider, fd_in);

	if (err) {
		DBG("config write error %s", strerror(err));
		goto done;
	}

	err = -EINPROGRESS;

	data->err_ch = g_io_channel_unix_new(fd_err);
	data->err_ch_id = g_io_add_watch(data->err_ch,
				G_IO_IN | G_IO_ERR | G_IO_HUP,
				(GIOFunc)io_channel_cb, data);

done:
	close(fd_in);

	/*
	 * Clear out credentials if they are non-immutable. If this is called
	 * directly from vc_connect() all credentials are read from config and
	 * are set as immutable, so no change is done. In case a VPN agent is
	 * used these values should be reset to "-" in order to retrieve them
	 * from VPN agent next time VPN connection is established. This supports
	 * then partially defined credentials in .config and some can be
	 * retrieved using an agent.
	 */
	for (i = 0; credentials[i]; i++) {
		const char *key = credentials[i];
		if (!vpn_provider_get_string_immutable(provider, key))
			vpn_provider_set_string(provider, key, "-");
	}

	return err;
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

	if (!user_data)
		return;

	str = user_data;
	connman_dbus_dict_append_basic(iter, "Value", DBUS_TYPE_STRING, &str);
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

	if (!user_data)
		return;

	str = user_data;
	connman_dbus_dict_append_basic(iter, "Value", DBUS_TYPE_STRING, &str);
}

static void request_input_append_informational(DBusMessageIter *iter,
		void *user_data)
{
	char *str = "password";

	connman_dbus_dict_append_basic(iter, "Type",
				DBUS_TYPE_STRING, &str);
	str = "informational";
	connman_dbus_dict_append_basic(iter, "Requirement",
				DBUS_TYPE_STRING, &str);

	if (!user_data)
		return;

	str = user_data;
	connman_dbus_dict_append_basic(iter, "Value", DBUS_TYPE_STRING, &str);
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

	connman_dbus_dict_append_dict(dict, key, function_cb, (void *)str);
}

static void request_input_credentials_reply(DBusMessage *reply, void *user_data)
{
	struct vc_private_data *data = user_data;
	char *secret = NULL, *username = NULL, *password = NULL;
	const char *key;
	DBusMessageIter iter, dict;
	int err;

	DBG("provider %p", data->provider);

	if (!reply)
		goto err;

	err = vpn_agent_check_and_process_reply_error(reply, data->provider,
				data->task, data->cb, data->user_data);
	if (err) {
		/* Ensure cb is called only once */
		data->cb = NULL;
		data->user_data = NULL;
		return;
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

		if (g_str_equal(key, "VPNC.IPSec.Secret")) {
			dbus_message_iter_next(&entry);
			if (dbus_message_iter_get_arg_type(&entry)
							!= DBUS_TYPE_VARIANT)
				break;
			dbus_message_iter_recurse(&entry, &value);
			if (dbus_message_iter_get_arg_type(&value)
							!= DBUS_TYPE_STRING)
				break;
			dbus_message_iter_get_basic(&value, &secret);
			vpn_provider_set_string_hide_value(data->provider,
					key, secret);

		} else if (g_str_equal(key, "VPNC.Xauth.Username")) {
			dbus_message_iter_next(&entry);
			if (dbus_message_iter_get_arg_type(&entry)
							!= DBUS_TYPE_VARIANT)
				break;
			dbus_message_iter_recurse(&entry, &value);
			if (dbus_message_iter_get_arg_type(&value)
							!= DBUS_TYPE_STRING)
				break;
			dbus_message_iter_get_basic(&value, &username);
			vpn_provider_set_string(data->provider, key, username);

		} else if (g_str_equal(key, "VPNC.Xauth.Password")) {
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

	if (!secret || !username || !password)
		goto err;

	err = run_connect(data);
	if (err != -EINPROGRESS)
		goto err;

	return;

err:
	vc_connect_done(data, EACCES);
}

static int request_input_credentials(struct vc_private_data *data,
					const char* dbus_sender)
{
	DBusMessage *message;
	const char *path, *agent_sender, *agent_path;
	DBusMessageIter iter;
	DBusMessageIter dict;
	int err;
	void *agent;

	if (!data || !data->provider)
		return -ENOENT;

	DBG("data %p provider %p sender %s", data, data->provider, dbus_sender);

	agent = connman_agent_get_info(dbus_sender, &agent_sender, &agent_path);
	if (!agent || !agent_path)
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

	if (vpn_provider_get_authentication_errors(data->provider))
		vpn_agent_append_auth_failure(&dict, data->provider, NULL);

	request_input_append_to_dict(data->provider, &dict,
				request_input_append_password,
				"VPNC.IPSec.Secret");
	request_input_append_to_dict(data->provider, &dict,
				request_input_append_mandatory,
				"VPNC.Xauth.Username");
	request_input_append_to_dict(data->provider, &dict,
				request_input_append_password,
				"VPNC.Xauth.Password");

	vpn_agent_append_host_and_name(&dict, data->provider);

	connman_dbus_dict_close(&iter, &dict);

	err = connman_agent_queue_message(data->provider, message,
				connman_timeout_input_request(),
				request_input_credentials_reply, data, agent);

	dbus_message_unref(message);

	if (err < 0 && err != -EBUSY) {
		DBG("error %d sending agent request", err);
		return err;
	}

	return -EINPROGRESS;
}

static int vc_connect(struct vpn_provider *provider,
			struct connman_task *task, const char *if_name,
			vpn_provider_connect_cb_t cb, const char *dbus_sender,
			void *user_data)
{
	struct vc_private_data *data;
	const char *option;
	bool username_set = false;
	bool password_set = false;
	bool ipsec_secret_set = false;
	int err;

	DBG("provider %p if_name %s user_data %p", provider, if_name, user_data);

	option = vpn_provider_get_string(provider, "VPNC.IPSec.ID");
	if (!option) {
		connman_error("Group not set; cannot enable VPN");
		return -EINVAL;
	}

	option = vpn_provider_get_string(provider, "VPNC.IPSec.Secret");
	if (option && *option && g_strcmp0(option, "-"))
		ipsec_secret_set = true;

	option = vpn_provider_get_string(provider, "VPNC.Xauth.Username");
	if (option && *option && g_strcmp0(option, "-"))
		username_set = true;

	option = vpn_provider_get_string(provider, "VPNC.Xauth.Password");
	if (option && *option && g_strcmp0(option, "-"))
		password_set = true;

	data = g_try_new0(struct vc_private_data, 1);
	if (!data)
		return -ENOMEM;

	vpn_provider_set_plugin_data(provider, data);
	data->provider = vpn_provider_ref(provider);
	data->task = task;
	data->if_name = g_strdup(if_name);
	data->cb = cb;
	data->user_data = user_data;

	if (!ipsec_secret_set || !username_set || !password_set) {
		err = request_input_credentials(data, dbus_sender);
		if (err != -EINPROGRESS) {
			vc_connect_done(data, ECONNABORTED);
			vpn_provider_indicate_error(data->provider,
					VPN_PROVIDER_ERROR_LOGIN_FAILED);
			free_private_data(data);
		}

		return err;
	}

	return run_connect(data);
}

static void vc_disconnect(struct vpn_provider *provider)
{
	if (!provider)
		return;

	connman_agent_cancel(provider);
}

static int vc_error_code(struct vpn_provider *provider, int exit_code)
{
	switch (exit_code) {
	case 1:
		return VPN_PROVIDER_ERROR_CONNECT_FAILED;
	case 2:
		return VPN_PROVIDER_ERROR_LOGIN_FAILED;
	default:
		return VPN_PROVIDER_ERROR_UNKNOWN;
	}
}

static int vc_device_flags(struct vpn_provider *provider)
{
	const char *option;

	option = vpn_provider_get_string(provider, "VPNC.DeviceType");
	if (!option) {
		return IFF_TUN;
	}

	if (g_str_equal(option, "tap")) {
		return IFF_TAP;
	}

	if (!g_str_equal(option, "tun")) {
		connman_warn("bad VPNC.DeviceType value, falling back to tun");
	}

	return IFF_TUN;
}

static struct vpn_driver vpn_driver = {
	.notify		= vc_notify,
	.connect	= vc_connect,
	.disconnect	= vc_disconnect,
	.error_code	= vc_error_code,
	.save		= vc_save,
	.device_flags	= vc_device_flags,
};

static int vpnc_init(void)
{
	return vpn_register("vpnc", &vpn_driver, VPNC);
}

static void vpnc_exit(void)
{
	vpn_unregister("vpnc");
}

CONNMAN_PLUGIN_DEFINE(vpnc, "vpnc plugin", VERSION,
	CONNMAN_PLUGIN_PRIORITY_DEFAULT, vpnc_init, vpnc_exit)
