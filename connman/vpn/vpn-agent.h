/*
 *
 *  ConnMan VPN daemon
 *
 *  Copyright (C) 2012  Intel Corporation. All rights reserved.
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

#ifndef __VPN_AGENT_H
#define __VPN_AGENT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * SECTION:agent
 * @title: Agent premitives
 * @short_description: Functions for interaction with agent
 */

void vpn_agent_append_host_and_name(DBusMessageIter *iter,
				struct vpn_provider *provider);
bool vpn_agent_check_reply_has_dict(DBusMessage *reply);
void vpn_agent_append_user_info(DBusMessageIter *iter,
				struct vpn_provider *provider,
				const char *username_str);
void vpn_agent_append_allow_credential_storage(DBusMessageIter *iter,
				bool allow);
void vpn_agent_append_allow_credential_retrieval(DBusMessageIter *iter,
				bool allow);
void vpn_agent_append_keep_credentials(DBusMessageIter *iter, bool allow);
void vpn_agent_append_auth_failure(DBusMessageIter *iter,
				struct vpn_provider *provider,
				const char *information);
int vpn_agent_check_and_process_reply_error(DBusMessage *reply,
				struct vpn_provider *provider,
				struct connman_task *task,
				vpn_provider_connect_cb_t cb, void *user_data);

#ifdef __cplusplus
}
#endif

#endif /* __VPN_AGENT_H */
