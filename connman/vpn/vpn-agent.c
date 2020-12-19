/*
 *
 *  Connection Manager
 *
 *  Copyright (C) 2012  Intel Corporation. All rights reserved.
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

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <gdbus.h>
#include <connman/log.h>
#include <connman/agent.h>
#include <connman/vpn-dbus.h>
#include <connman/task.h>
#include <vpn/vpn-provider.h>

#include "vpn-agent.h"
#include "vpn.h"

bool vpn_agent_check_reply_has_dict(DBusMessage *reply)
{
	const char *signature = DBUS_TYPE_ARRAY_AS_STRING
		DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
		DBUS_TYPE_STRING_AS_STRING
		DBUS_TYPE_VARIANT_AS_STRING
		DBUS_DICT_ENTRY_END_CHAR_AS_STRING;

	if (dbus_message_has_signature(reply, signature))
		return true;

	connman_warn("Reply %s to %s from %s has wrong signature %s",
			signature,
			dbus_message_get_interface(reply),
			dbus_message_get_sender(reply),
			dbus_message_get_signature(reply));

	return false;
}

static void request_input_append_name(DBusMessageIter *iter, void *user_data)
{
	struct vpn_provider *provider = user_data;
	const char *str = "string";

	connman_dbus_dict_append_basic(iter, "Type",
				DBUS_TYPE_STRING, &str);
	str = "informational";
	connman_dbus_dict_append_basic(iter, "Requirement",
				DBUS_TYPE_STRING, &str);

	str = vpn_provider_get_name(provider);
	connman_dbus_dict_append_basic(iter, "Value",
				DBUS_TYPE_STRING, &str);
}

static void request_input_append_host(DBusMessageIter *iter, void *user_data)
{
	struct vpn_provider *provider = user_data;
	const char *str = "string";

	connman_dbus_dict_append_basic(iter, "Type",
				DBUS_TYPE_STRING, &str);
	str = "informational";
	connman_dbus_dict_append_basic(iter, "Requirement",
				DBUS_TYPE_STRING, &str);

	str = vpn_provider_get_host(provider);
	connman_dbus_dict_append_basic(iter, "Value",
				DBUS_TYPE_STRING, &str);
}

void vpn_agent_append_host_and_name(DBusMessageIter *iter,
				struct vpn_provider *provider)
{
	connman_dbus_dict_append_dict(iter, "Host",
				request_input_append_host,
				provider);

	connman_dbus_dict_append_dict(iter, "Name",
				request_input_append_name,
				provider);
}

struct user_info_data {
	struct vpn_provider *provider;
	const char *username_str;
	const char *type_str;
};

static void request_input_append_user_info(DBusMessageIter *iter,
						void *user_data)
{
	struct user_info_data *data = user_data;
	struct vpn_provider *provider = data->provider;
	const char *str = NULL;

	connman_dbus_dict_append_basic(iter, "Type",
				DBUS_TYPE_STRING, &data->type_str);
	str = "mandatory";
	connman_dbus_dict_append_basic(iter, "Requirement",
				DBUS_TYPE_STRING, &str);

	if (data->username_str) {
		str = vpn_provider_get_string(provider, data->username_str);
		if (str)
			connman_dbus_dict_append_basic(iter, "Value",
						DBUS_TYPE_STRING, &str);
	}
}

void vpn_agent_append_user_info(DBusMessageIter *iter,
				struct vpn_provider *provider,
				const char *username_str)
{
	struct user_info_data data = {
		.provider = provider,
		.username_str = username_str
	};

	data.type_str = "string";
	connman_dbus_dict_append_dict(iter, "Username",
				request_input_append_user_info,
				&data);

	data.username_str = NULL;
	data.type_str = "password";
	connman_dbus_dict_append_dict(iter, "Password",
				request_input_append_user_info,
				&data);
}

static void request_input_append_flag(DBusMessageIter *iter,
						void *user_data)
{
	dbus_bool_t data = (dbus_bool_t)GPOINTER_TO_INT(user_data);
	const char *str = NULL;

	str = "boolean";
	connman_dbus_dict_append_basic(iter, "Type",
				DBUS_TYPE_STRING, &str);

	str = "control";
	connman_dbus_dict_append_basic(iter, "Requirement",
				DBUS_TYPE_STRING, &str);

	connman_dbus_dict_append_basic(iter, "Value",
				DBUS_TYPE_BOOLEAN, &data);
}

void vpn_agent_append_allow_credential_storage(DBusMessageIter *iter,
				bool allow)
{
	connman_dbus_dict_append_dict(iter, "AllowStoreCredentials",
				request_input_append_flag,
				GINT_TO_POINTER(allow));
}

void vpn_agent_append_allow_credential_retrieval(DBusMessageIter *iter,
				bool allow)
{
	connman_dbus_dict_append_dict(iter, "AllowRetrieveCredentials",
				request_input_append_flag,
				GINT_TO_POINTER(allow));
}

void vpn_agent_append_keep_credentials(DBusMessageIter *iter, bool allow)
{
	connman_dbus_dict_append_dict(iter, "KeepCredentials",
				request_input_append_flag,
				GINT_TO_POINTER(allow));
}

struct failure_data {
	struct vpn_provider *provider;
	const char* type_str;
	const char *key;
	const char* str;
};

static void request_input_append_failure(DBusMessageIter *iter,
						void *user_data)
{
	struct failure_data *data;
	const char *str;

	data = user_data;

	connman_dbus_dict_append_basic(iter, "Type",
				DBUS_TYPE_STRING, &data->type_str);
	str = "informational";
	connman_dbus_dict_append_basic(iter, "Requirement",
				DBUS_TYPE_STRING, &str);

	str = data->str;

	/* Try to get information from provider about error */
	if (!str)
		str = vpn_provider_get_string(data->provider, data->key);

	if (str)
		connman_dbus_dict_append_basic(iter, "Value",
						DBUS_TYPE_STRING, &str);
}

void vpn_agent_append_auth_failure(DBusMessageIter *iter,
				struct vpn_provider *provider,
				const char* information)
{
	struct failure_data data;
	unsigned int value;

	/* Skip if there are no auth errors */
	value = vpn_provider_get_authentication_errors(provider);
	if (!value)
		return;

	data.provider = provider;
	data.type_str = "string";
	data.key = "VpnAgent.AuthFailure";
	data.str = information;

	connman_dbus_dict_append_dict(iter, data.key,
				request_input_append_failure, &data);
}

int vpn_agent_check_and_process_reply_error(DBusMessage *reply,
				struct vpn_provider *provider,
				struct connman_task *task,
				vpn_provider_connect_cb_t cb, void *user_data)
{
	DBusError error;
	int err;

	if (!reply || !provider)
		return EINVAL;

	dbus_error_init(&error);

	if (!dbus_set_error_from_message(&error, reply))
		return 0;

	if (!g_strcmp0(error.name, VPN_AGENT_INTERFACE ".Error.Canceled"))
		err = ECANCELED;
	else if (!g_strcmp0(error.name, "org.freedesktop.DBus.Error.Timeout"))
		err = ETIMEDOUT;
	else if (!g_strcmp0(error.name, "org.freedesktop.DBus.Error.NoReply"))
		err = ENOMSG;
	else
		err = EACCES;

	dbus_error_free(&error);

	if (cb)
		cb(provider, user_data, err);

	if (task)
		connman_task_stop(task);

	/*
	 * VPN agent dialog cancel, timeout, broken connection should set the
	 * VPN back to idle state
	 */
	vpn_provider_set_state(provider, VPN_PROVIDER_STATE_IDLE);

	return err;
}
