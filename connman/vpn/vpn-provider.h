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

#ifndef __VPN_PROVIDER_H
#define __VPN_PROVIDER_H

#include <stdbool.h>

#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * SECTION:provider
 * @title: Provider premitives
 * @short_description: Functions for handling providers
 */

enum vpn_provider_state {
	VPN_PROVIDER_STATE_UNKNOWN       = 0,
	VPN_PROVIDER_STATE_IDLE          = 1,
	VPN_PROVIDER_STATE_CONNECT       = 2,
	VPN_PROVIDER_STATE_READY         = 3,
	VPN_PROVIDER_STATE_DISCONNECT    = 4,
	VPN_PROVIDER_STATE_FAILURE       = 5,
};

enum vpn_provider_error {
	VPN_PROVIDER_ERROR_UNKNOWN		= 0,
	VPN_PROVIDER_ERROR_CONNECT_FAILED	= 1,
	VPN_PROVIDER_ERROR_LOGIN_FAILED	= 2,
	VPN_PROVIDER_ERROR_AUTH_FAILED	= 3,
};

enum vpn_provider_route_type {
	VPN_PROVIDER_ROUTE_TYPE_NONE = 0,
	VPN_PROVIDER_ROUTE_TYPE_MASK = 1,
	VPN_PROVIDER_ROUTE_TYPE_ADDR = 2,
	VPN_PROVIDER_ROUTE_TYPE_GW   = 3,
};

struct vpn_provider;
struct connman_ipaddress;

#define vpn_provider_ref(provider) \
	vpn_provider_ref_debug(provider, __FILE__, __LINE__, __func__)

#define vpn_provider_unref(provider) \
	vpn_provider_unref_debug(provider, __FILE__, __LINE__, __func__)

struct vpn_provider *
vpn_provider_ref_debug(struct vpn_provider *provider,
			const char *file, int line, const char *caller);
void vpn_provider_unref_debug(struct vpn_provider *provider,
			const char *file, int line, const char *caller);

int vpn_provider_set_string(struct vpn_provider *provider,
					const char *key, const char *value);
int vpn_provider_set_string_hide_value(struct vpn_provider *provider,
					const char *key, const char *value);
const char *vpn_provider_get_string(struct vpn_provider *provider,
							const char *key);
bool vpn_provider_get_string_immutable(struct vpn_provider *provider,
							const char *key);
int vpn_provider_set_boolean(struct vpn_provider *provider, const char *key,
							bool value,
							bool force_change);
bool vpn_provider_get_boolean(struct vpn_provider *provider, const char *key,
							bool default_value);

int vpn_provider_set_state(struct vpn_provider *provider,
					enum vpn_provider_state state);

void vpn_provider_add_error(struct vpn_provider *provider,
					enum vpn_provider_error error);
int vpn_provider_indicate_error(struct vpn_provider *provider,
					enum vpn_provider_error error);

void vpn_provider_set_index(struct vpn_provider *provider, int index);
int vpn_provider_get_index(struct vpn_provider *provider);

void vpn_provider_set_data(struct vpn_provider *provider, void *data);
void *vpn_provider_get_data(struct vpn_provider *provider);
void vpn_provider_set_plugin_data(struct vpn_provider *provider, void *data);
void *vpn_provider_get_plugin_data(struct vpn_provider *provider);
int vpn_provider_set_ipaddress(struct vpn_provider *provider,
					struct connman_ipaddress *ipaddress);
int vpn_provider_set_pac(struct vpn_provider *provider,
				const char *pac);
int vpn_provider_set_domain(struct vpn_provider *provider,
				const char *domain);
int vpn_provider_set_nameservers(struct vpn_provider *provider,
					const char *nameservers);
int vpn_provider_append_route(struct vpn_provider *provider,
					const char *key, const char *value);

const char *vpn_provider_get_driver_name(struct vpn_provider *provider);
const char *vpn_provider_get_save_group(struct vpn_provider *provider);

const char *vpn_provider_get_name(struct vpn_provider *provider);
const char *vpn_provider_get_host(struct vpn_provider *provider);
const char *vpn_provider_get_path(struct vpn_provider *provider);
const char *vpn_provider_get_ident(struct vpn_provider *provider);

unsigned int vpn_provider_get_authentication_errors(
					struct vpn_provider *provider);
unsigned int vpn_provider_get_connection_errors(
					struct vpn_provider *provider);

void vpn_provider_change_address(struct vpn_provider *provider);
void vpn_provider_clear_address(struct vpn_provider *provider, int family);

typedef void (* vpn_provider_connect_cb_t) (struct vpn_provider *provider,
					void *user_data, int error);

typedef void (* vpn_provider_auth_cb_t) (struct vpn_provider *provider,
					const char *authenticator,
					const char *error, void *user_data);

typedef void (* vpn_provider_password_cb_t) (struct vpn_provider *provider,
					const char *username,
					const char *password,
					const char *error, void *user_data);

struct vpn_provider_driver {
	const char *name;
	int (*probe) (struct vpn_provider *provider);
	int (*remove) (struct vpn_provider *provider);
	int (*connect) (struct vpn_provider *provider,
			vpn_provider_connect_cb_t cb, const char *dbus_sender,
			void *user_data);
	int (*disconnect) (struct vpn_provider *provider);
	int (*save) (struct vpn_provider *provider, GKeyFile *keyfile);
	int (*set_state)(struct vpn_provider *provider,
						enum vpn_provider_state state);
	int (*route_env_parse) (struct vpn_provider *provider, const char *key,
			int *family, unsigned long *idx,
			enum vpn_provider_route_type *type);
};

int vpn_provider_driver_register(struct vpn_provider_driver *driver);
void vpn_provider_driver_unregister(struct vpn_provider_driver *driver);

#ifdef __cplusplus
}
#endif

#endif /* __VPN_PROVIDER_H */
