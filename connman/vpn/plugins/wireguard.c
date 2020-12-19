/*
 *  ConnMan VPN daemon
 *
 *  Copyright (C) 2019  Daniel Wagner. All rights reserved.
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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <glib.h>

#define CONNMAN_API_SUBJECT_TO_CHANGE
#include <connman/plugin.h>
#include <connman/log.h>
#include <connman/task.h>
#include <connman/ipconfig.h>
#include <connman/inet.h>
#include <connman/dbus.h>
#include <connman/setting.h>
#include <connman/vpn-dbus.h>

#include "../vpn-provider.h"
#include "../vpn.h"

#include "vpn.h"
#include "wireguard.h"

#define DNS_RERESOLVE_TIMEOUT 20

struct wireguard_info {
	struct wg_device device;
	struct wg_peer peer;
	char *endpoint_fqdn;
	char *port;
	int reresolve_id;
};

struct sockaddr_u {
	union {
		struct sockaddr sa;
		struct sockaddr_in sin;
		struct sockaddr_in6 sin6;
	};
};

static int parse_key(const char *str, wg_key key)
{
	unsigned char *buf;
	size_t len;

	buf = g_base64_decode(str, &len);

	if (len != 32) {
		g_free(buf);
		return -EINVAL;
	}

	memcpy(key, buf, 32);

	g_free(buf);
	return 0;
}

static int parse_allowed_ips(const char *allowed_ips, wg_peer *peer)
{
	struct wg_allowedip *curaip, *allowedip;
	char buf[INET6_ADDRSTRLEN];
	char **tokens, **toks;
	char *send;
	int i;

	curaip = NULL;
	tokens = g_strsplit(allowed_ips, ", ", -1);
	for (i = 0; tokens[i]; i++) {
		toks = g_strsplit(tokens[i], "/", -1);
		if (g_strv_length(toks) != 2) {
			DBG("Ignore AllowedIPs value %s", tokens[i]);
			g_strfreev(toks);
			continue;
		}

		allowedip = g_malloc0(sizeof(*allowedip));

		if (inet_pton(AF_INET, toks[0], buf) == 1) {
			allowedip->family = AF_INET;
			memcpy(&allowedip->ip4, buf, sizeof(allowedip->ip4));
		} else if (inet_pton(AF_INET6, toks[0], buf) == 1) {
			allowedip->family = AF_INET6;
			memcpy(&allowedip->ip6, buf, sizeof(allowedip->ip6));
		} else {
			DBG("Ignore AllowedIPs value %s", tokens[i]);
			g_free(allowedip);
			g_strfreev(toks);
			continue;
		}

		allowedip->cidr = g_ascii_strtoull(toks[1], &send, 10);

		if (!curaip)
			peer->first_allowedip = allowedip;
		else
			curaip->next_allowedip = allowedip;

		curaip = allowedip;
	}

	peer->last_allowedip = curaip;
	g_strfreev(tokens);

	return 0;
}

static int parse_endpoint(const char *host, const char *port, struct sockaddr_u *addr)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sk;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	if (getaddrinfo(host, port, &hints, &result) < 0) {
		DBG("Failed to resolve host address");
		return -EINVAL;
	}

	for (rp = result; rp; rp = rp->ai_next) {
		sk = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sk < 0)
			continue;
		if (connect(sk, rp->ai_addr, rp->ai_addrlen) != -1) {
			/* success */
			close(sk);
			break;
		}

		close(sk);
	}

	if (!rp) {
		freeaddrinfo(result);
		return -EINVAL;
	}

	memcpy(addr, rp->ai_addr, rp->ai_addrlen);
	freeaddrinfo(result);

	return 0;
}

static int parse_address(const char *address, const char *gateway,
		struct connman_ipaddress **ipaddress)
{
	char buf[INET6_ADDRSTRLEN];
	unsigned char prefixlen;
	char **tokens;
	char *end, *netmask;
	int err;

	tokens = g_strsplit(address, "/", -1);
	if (g_strv_length(tokens) != 2) {
		g_strfreev(tokens);
		return -EINVAL;
	}

	prefixlen = g_ascii_strtoull(tokens[1], &end, 10);

	if (inet_pton(AF_INET, tokens[0], buf) == 1) {
		netmask = g_strdup_printf("%d.%d.%d.%d",
				((0xffffffff << (32 - prefixlen)) >> 24) & 0xff,
				((0xffffffff << (32 - prefixlen)) >> 16) & 0xff,
				((0xffffffff << (32 - prefixlen)) >> 8) & 0xff,
				((0xffffffff << (32 - prefixlen)) >> 0) & 0xff);

		*ipaddress = connman_ipaddress_alloc(AF_INET);
		err = connman_ipaddress_set_ipv4(*ipaddress, tokens[0],
						netmask, gateway);
		g_free(netmask);
	} else if (inet_pton(AF_INET6, tokens[0], buf) == 1) {
		*ipaddress = connman_ipaddress_alloc(AF_INET6);
		err = connman_ipaddress_set_ipv6(*ipaddress, tokens[0],
						prefixlen, gateway);
	} else {
		DBG("Invalid Wireguard.Address value");
		err = -EINVAL;
	}

	connman_ipaddress_set_p2p(*ipaddress, true);

	g_strfreev(tokens);
	if (err)
		connman_ipaddress_free(*ipaddress);

	return err;
}

struct ifname_data {
	char *ifname;
	bool found;
};

static void ifname_check_cb(int index, void *user_data)
{
	struct ifname_data *data = (struct ifname_data *)user_data;
	char *ifname;

	ifname = connman_inet_ifname(index);

	if (!g_strcmp0(ifname, data->ifname))
		data->found = true;
}

static char *get_ifname(void)
{
	struct ifname_data data;
	int i;

	for (i = 0; i < 256; i++) {
		data.ifname = g_strdup_printf("wg%d", i);
		data.found = false;
		__vpn_ipconfig_foreach(ifname_check_cb, &data);

		if (!data.found)
			return data.ifname;

		g_free(data.ifname);
	}

	return NULL;
}

static bool sockaddr_cmp_addr(struct sockaddr_u *a, struct sockaddr_u *b)
{
	if (a->sa.sa_family != b->sa.sa_family)
		return false;

	if (a->sa.sa_family == AF_INET)
		return !memcmp(&a->sin, &b->sin, sizeof(struct sockaddr_in));
	else if (a->sa.sa_family == AF_INET6)
		return !memcmp(a->sin6.sin6_addr.s6_addr,
				b->sin6.sin6_addr.s6_addr,
				sizeof(a->sin6.sin6_addr.s6_addr));

	return false;
}

static gboolean wg_dns_reresolve_cb(gpointer user_data)
{
	struct wireguard_info *info = user_data;
	struct sockaddr_u addr;
	int err;

	DBG("");

	err = parse_endpoint(info->endpoint_fqdn,
			info->port, &addr);
	if (err)
		return TRUE;

	if (sockaddr_cmp_addr(&addr,
			(struct sockaddr_u *)&info->peer.endpoint.addr))
		return TRUE;

	if (addr.sa.sa_family == AF_INET)
		memcpy(&info->peer.endpoint.addr, &addr.sin,
			sizeof(info->peer.endpoint.addr4));
	else
		memcpy(&info->peer.endpoint.addr, &addr.sin6,
			sizeof(info->peer.endpoint.addr6));

	DBG("Endpoint address has changed, udpate WireGuard device");
	err = wg_set_device(&info->device);
	if (err)
		DBG("Failed to update Endpoint address for WireGuard device %s",
			info->device.name);

	return TRUE;
}

static int wg_connect(struct vpn_provider *provider,
			struct connman_task *task, const char *if_name,
			vpn_provider_connect_cb_t cb,
			const char *dbus_sender, void *user_data)
{
	struct connman_ipaddress *ipaddress = NULL;
	struct wireguard_info *info;
	const char *option, *gateway;
	char *ifname;
	int err = -EINVAL;

	info = g_malloc0(sizeof(struct wireguard_info));
	info->peer.flags = WGPEER_HAS_PUBLIC_KEY | WGPEER_REPLACE_ALLOWEDIPS;
	info->device.flags = WGDEVICE_HAS_PRIVATE_KEY;
	info->device.first_peer = &info->peer;
	info->device.last_peer = &info->peer;

	vpn_provider_set_plugin_data(provider, info);

	option = vpn_provider_get_string(provider, "WireGuard.ListenPort");
	if (option) {
		char *end;
		info->device.listen_port = g_ascii_strtoull(option, &end, 10);
		info->device.flags |= WGDEVICE_HAS_LISTEN_PORT;
	}

	option = vpn_provider_get_string(provider, "WireGuard.DNS");
	if (option) {
		err = vpn_provider_set_nameservers(provider, option);
		if (err)
			goto done;
	}

	option = vpn_provider_get_string(provider, "WireGuard.PrivateKey");
	if (!option) {
		DBG("WireGuard.PrivateKey is missing");
		goto done;
	}
	err = parse_key(option, info->device.private_key);
	if (err)
		goto done;

	option = vpn_provider_get_string(provider, "WireGuard.PublicKey");
	if (!option) {
		DBG("WireGuard.PublicKey is missing");
		goto done;
	}
	err = parse_key(option, info->peer.public_key);
	if (err)
		goto done;

	option = vpn_provider_get_string(provider, "WireGuard.PresharedKey");
	if (option) {
		info->peer.flags |= WGPEER_HAS_PRESHARED_KEY;
		err = parse_key(option, info->peer.preshared_key);
		if (err)
			goto done;
	}

	option = vpn_provider_get_string(provider, "WireGuard.AllowedIPs");
	if (!option) {
		DBG("WireGuard.AllowedIPs is missing");
		goto done;
	}
	err = parse_allowed_ips(option, &info->peer);
	if (err)
		goto done;

	option = vpn_provider_get_string(provider,
					"WireGuard.PersistentKeepalive");
	if (option) {
		char *end;
		info->peer.persistent_keepalive_interval =
			g_ascii_strtoull(option, &end, 10);
		info->peer.flags |= WGPEER_HAS_PERSISTENT_KEEPALIVE_INTERVAL;
	}

	option = vpn_provider_get_string(provider, "WireGuard.EndpointPort");
	if (!option)
		option = "51820";

	gateway = vpn_provider_get_string(provider, "Host");
	err = parse_endpoint(gateway, option,
			(struct sockaddr_u *)&info->peer.endpoint.addr);
	if (err)
		goto done;

	info->endpoint_fqdn = g_strdup(gateway);
	info->port = g_strdup(option);

	option = vpn_provider_get_string(provider, "WireGuard.Address");
	if (!option) {
		DBG("Missing WireGuard.Address configuration");
		goto done;
	}
	err = parse_address(option, gateway, &ipaddress);
	if (err)
		goto done;

	ifname = get_ifname();
	if (!ifname) {
		DBG("Failed to find an usable device name");
		err = -ENOENT;
		goto done;
	}
	stpncpy(info->device.name, ifname, sizeof(info->device.name));
	g_free(ifname);

	err = wg_add_device(info->device.name);
	if (err) {
		DBG("Failed to creating WireGuard device %s", info->device.name);
		goto done;
	}

	err = wg_set_device(&info->device);
	if (err) {
		DBG("Failed to configure WireGuard device %s", info->device.name);
		wg_del_device(info->device.name);
	}

	vpn_set_ifname(provider, info->device.name);
	if (ipaddress)
		vpn_provider_set_ipaddress(provider, ipaddress);

done:
	if (cb)
		cb(provider, user_data, err);

	connman_ipaddress_free(ipaddress);

	if (!err)
		info->reresolve_id =
			g_timeout_add_seconds(DNS_RERESOLVE_TIMEOUT,
						wg_dns_reresolve_cb, info);

	return err;
}

static void wg_disconnect(struct vpn_provider *provider)
{
	struct wireguard_info *info;

	info = vpn_provider_get_plugin_data(provider);
	if (!info)
		return;

	if (info->reresolve_id > 0)
		g_source_remove(info->reresolve_id);

	vpn_provider_set_plugin_data(provider, NULL);

	wg_del_device(info->device.name);

	g_free(info->endpoint_fqdn);
	g_free(info->port);
	g_free(info);
}

static struct vpn_driver vpn_driver = {
	.flags		= VPN_FLAG_NO_TUN | VPN_FLAG_NO_DAEMON,
	.connect	= wg_connect,
	.disconnect	= wg_disconnect,
};

static int wg_init(void)
{
	return vpn_register("wireguard", &vpn_driver, NULL);
}

static void wg_exit(void)
{
	vpn_unregister("wireguard");
}

CONNMAN_PLUGIN_DEFINE(wireguard, "WireGuard VPN plugin", VERSION,
	CONNMAN_PLUGIN_PRIORITY_DEFAULT, wg_init, wg_exit)
