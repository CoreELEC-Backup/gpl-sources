// SPDX-License-Identifier: LGPL-2.1+
/*
 * Copyright (C) 2015-2019 Jason A. Donenfeld <Jason@zx2c4.com>. All Rights Reserved.
 * Copyright (C) 2008-2012 Pablo Neira Ayuso <pablo@netfilter.org>.
 */

#define _GNU_SOURCE

#include <errno.h>
#include <linux/genetlink.h>
#include <linux/if_link.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#include <libmnl/libmnl.h>

#include "src/shared/mnlg.h"
#include "wireguard.h"

/* wireguard.h netlink uapi: */

#define WG_GENL_NAME "wireguard"
#define WG_GENL_VERSION 1

enum wg_cmd {
	WG_CMD_GET_DEVICE,
	WG_CMD_SET_DEVICE,
	__WG_CMD_MAX
};

enum wgdevice_flag {
	WGDEVICE_F_REPLACE_PEERS = 1U << 0
};
enum wgdevice_attribute {
	WGDEVICE_A_UNSPEC,
	WGDEVICE_A_IFINDEX,
	WGDEVICE_A_IFNAME,
	WGDEVICE_A_PRIVATE_KEY,
	WGDEVICE_A_PUBLIC_KEY,
	WGDEVICE_A_FLAGS,
	WGDEVICE_A_LISTEN_PORT,
	WGDEVICE_A_FWMARK,
	WGDEVICE_A_PEERS,
	__WGDEVICE_A_LAST
};

enum wgpeer_flag {
	WGPEER_F_REMOVE_ME = 1U << 0,
	WGPEER_F_REPLACE_ALLOWEDIPS = 1U << 1
};
enum wgpeer_attribute {
	WGPEER_A_UNSPEC,
	WGPEER_A_PUBLIC_KEY,
	WGPEER_A_PRESHARED_KEY,
	WGPEER_A_FLAGS,
	WGPEER_A_ENDPOINT,
	WGPEER_A_PERSISTENT_KEEPALIVE_INTERVAL,
	WGPEER_A_LAST_HANDSHAKE_TIME,
	WGPEER_A_RX_BYTES,
	WGPEER_A_TX_BYTES,
	WGPEER_A_ALLOWEDIPS,
	WGPEER_A_PROTOCOL_VERSION,
	__WGPEER_A_LAST
};

enum wgallowedip_attribute {
	WGALLOWEDIP_A_UNSPEC,
	WGALLOWEDIP_A_FAMILY,
	WGALLOWEDIP_A_IPADDR,
	WGALLOWEDIP_A_CIDR_MASK,
	__WGALLOWEDIP_A_LAST
};

/* wireguard-specific parts: */

struct inflatable_buffer {
	char *buffer;
	char *next;
	bool good;
	size_t len;
	size_t pos;
};

#define max(a, b) ((a) > (b) ? (a) : (b))

static int add_next_to_inflatable_buffer(struct inflatable_buffer *buffer)
{
	size_t len, expand_to;
	char *new_buffer;

	if (!buffer->good || !buffer->next) {
		free(buffer->next);
		buffer->good = false;
		return 0;
	}

	len = strlen(buffer->next) + 1;

	if (len == 1) {
		free(buffer->next);
		buffer->good = false;
		return 0;
	}

	if (buffer->len - buffer->pos <= len) {
		expand_to = max(buffer->len * 2, buffer->len + len + 1);
		new_buffer = realloc(buffer->buffer, expand_to);
		if (!new_buffer) {
			free(buffer->next);
			buffer->good = false;
			return -errno;
		}
		memset(&new_buffer[buffer->len], 0, expand_to - buffer->len);
		buffer->buffer = new_buffer;
		buffer->len = expand_to;
	}
	memcpy(&buffer->buffer[buffer->pos], buffer->next, len);
	free(buffer->next);
	buffer->good = false;
	buffer->pos += len;
	return 0;
}

static int parse_linkinfo(const struct nlattr *attr, void *data)
{
	struct inflatable_buffer *buffer = data;

	if (mnl_attr_get_type(attr) == IFLA_INFO_KIND && !strcmp(WG_GENL_NAME, mnl_attr_get_str(attr)))
		buffer->good = true;
	return MNL_CB_OK;
}

static int parse_infomsg(const struct nlattr *attr, void *data)
{
	struct inflatable_buffer *buffer = data;

	if (mnl_attr_get_type(attr) == IFLA_LINKINFO)
		return mnl_attr_parse_nested(attr, parse_linkinfo, data);
	else if (mnl_attr_get_type(attr) == IFLA_IFNAME)
		buffer->next = strdup(mnl_attr_get_str(attr));
	return MNL_CB_OK;
}

static int read_devices_cb(const struct nlmsghdr *nlh, void *data)
{
	struct inflatable_buffer *buffer = data;
	int ret;

	buffer->good = false;
	buffer->next = NULL;
	ret = mnl_attr_parse(nlh, sizeof(struct ifinfomsg), parse_infomsg, data);
	if (ret != MNL_CB_OK)
		return ret;
	ret = add_next_to_inflatable_buffer(buffer);
	if (ret < 0)
		return ret;
	if (nlh->nlmsg_type != NLMSG_DONE)
		return MNL_CB_OK + 1;
	return MNL_CB_OK;
}

static int fetch_device_names(struct inflatable_buffer *buffer)
{
	struct mnl_socket *nl = NULL;
	char *rtnl_buffer = NULL;
	size_t message_len;
	unsigned int portid, seq;
	ssize_t len;
	int ret = 0;
	struct nlmsghdr *nlh;
	struct ifinfomsg *ifm;

	ret = -ENOMEM;
	rtnl_buffer = calloc(MNL_SOCKET_BUFFER_SIZE, 1);
	if (!rtnl_buffer)
		goto cleanup;

	nl = mnl_socket_open(NETLINK_ROUTE);
	if (!nl) {
		ret = -errno;
		goto cleanup;
	}

	if (mnl_socket_bind(nl, 0, MNL_SOCKET_AUTOPID) < 0) {
		ret = -errno;
		goto cleanup;
	}

	seq = time(NULL);
	portid = mnl_socket_get_portid(nl);
	nlh = mnl_nlmsg_put_header(rtnl_buffer);
	nlh->nlmsg_type = RTM_GETLINK;
	nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | NLM_F_DUMP;
	nlh->nlmsg_seq = seq;
	ifm = mnl_nlmsg_put_extra_header(nlh, sizeof(*ifm));
	ifm->ifi_family = AF_UNSPEC;
	message_len = nlh->nlmsg_len;

	if (mnl_socket_sendto(nl, rtnl_buffer, message_len) < 0) {
		ret = -errno;
		goto cleanup;
	}

another:
	if ((len = mnl_socket_recvfrom(nl, rtnl_buffer, MNL_SOCKET_BUFFER_SIZE)) < 0) {
		ret = -errno;
		goto cleanup;
	}
	if ((len = mnl_cb_run(rtnl_buffer, len, seq, portid, read_devices_cb, buffer)) < 0) {
		/* Netlink returns NLM_F_DUMP_INTR if the set of all tunnels changed
		 * during the dump. That's unfortunate, but is pretty common on busy
		 * systems that are adding and removing tunnels all the time. Rather
		 * than retrying, potentially indefinitely, we just work with the
		 * partial results. */
		if (errno != EINTR) {
			ret = -errno;
			goto cleanup;
		}
	}
	if (len == MNL_CB_OK + 1)
		goto another;
	ret = 0;

cleanup:
	free(rtnl_buffer);
	if (nl)
		mnl_socket_close(nl);
	return ret;
}

static int add_del_iface(const char *ifname, bool add)
{
	struct mnl_socket *nl = NULL;
	char *rtnl_buffer;
	ssize_t len;
	int ret;
	struct nlmsghdr *nlh;
	struct ifinfomsg *ifm;
	struct nlattr *nest;

	rtnl_buffer = calloc(MNL_SOCKET_BUFFER_SIZE, 1);
	if (!rtnl_buffer) {
		ret = -ENOMEM;
		goto cleanup;
	}

	nl = mnl_socket_open(NETLINK_ROUTE);
	if (!nl) {
		ret = -errno;
		goto cleanup;
	}

	if (mnl_socket_bind(nl, 0, MNL_SOCKET_AUTOPID) < 0) {
		ret = -errno;
		goto cleanup;
	}

	nlh = mnl_nlmsg_put_header(rtnl_buffer);
	nlh->nlmsg_type = add ? RTM_NEWLINK : RTM_DELLINK;
	nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | (add ? NLM_F_CREATE | NLM_F_EXCL : 0);
	nlh->nlmsg_seq = time(NULL);
	ifm = mnl_nlmsg_put_extra_header(nlh, sizeof(*ifm));
	ifm->ifi_family = AF_UNSPEC;
	mnl_attr_put_strz(nlh, IFLA_IFNAME, ifname);
	nest = mnl_attr_nest_start(nlh, IFLA_LINKINFO);
	mnl_attr_put_strz(nlh, IFLA_INFO_KIND, WG_GENL_NAME);
	mnl_attr_nest_end(nlh, nest);

	if (mnl_socket_sendto(nl, rtnl_buffer, nlh->nlmsg_len) < 0) {
		ret = -errno;
		goto cleanup;
	}
	if ((len = mnl_socket_recvfrom(nl, rtnl_buffer, MNL_SOCKET_BUFFER_SIZE)) < 0) {
		ret = -errno;
		goto cleanup;
	}
	if (mnl_cb_run(rtnl_buffer, len, nlh->nlmsg_seq, mnl_socket_get_portid(nl), NULL, NULL) < 0) {
		ret = -errno;
		goto cleanup;
	}
	ret = 0;

cleanup:
	free(rtnl_buffer);
	if (nl)
		mnl_socket_close(nl);
	return ret;
}

int wg_set_device(wg_device *dev)
{
	int ret = 0;
	wg_peer *peer = NULL;
	wg_allowedip *allowedip = NULL;
	struct nlattr *peers_nest, *peer_nest, *allowedips_nest, *allowedip_nest;
	struct nlmsghdr *nlh;
	struct mnlg_socket *nlg;

	nlg = mnlg_socket_open(WG_GENL_NAME, WG_GENL_VERSION);
	if (!nlg)
		return -errno;

again:
	nlh = mnlg_msg_prepare(nlg, WG_CMD_SET_DEVICE, NLM_F_REQUEST | NLM_F_ACK);
	mnl_attr_put_strz(nlh, WGDEVICE_A_IFNAME, dev->name);

	if (!peer) {
		uint32_t flags = 0;

		if (dev->flags & WGDEVICE_HAS_PRIVATE_KEY)
			mnl_attr_put(nlh, WGDEVICE_A_PRIVATE_KEY, sizeof(dev->private_key), dev->private_key);
		if (dev->flags & WGDEVICE_HAS_LISTEN_PORT)
			mnl_attr_put_u16(nlh, WGDEVICE_A_LISTEN_PORT, dev->listen_port);
		if (dev->flags & WGDEVICE_HAS_FWMARK)
			mnl_attr_put_u32(nlh, WGDEVICE_A_FWMARK, dev->fwmark);
		if (dev->flags & WGDEVICE_REPLACE_PEERS)
			flags |= WGDEVICE_F_REPLACE_PEERS;
		if (flags)
			mnl_attr_put_u32(nlh, WGDEVICE_A_FLAGS, flags);
	}
	if (!dev->first_peer)
		goto send;
	peers_nest = peer_nest = allowedips_nest = allowedip_nest = NULL;
	peers_nest = mnl_attr_nest_start(nlh, WGDEVICE_A_PEERS);
	for (peer = peer ? peer : dev->first_peer; peer; peer = peer->next_peer) {
		uint32_t flags = 0;

		peer_nest = mnl_attr_nest_start_check(nlh, MNL_SOCKET_BUFFER_SIZE, 0);
		if (!peer_nest)
			goto toobig_peers;
		if (!mnl_attr_put_check(nlh, MNL_SOCKET_BUFFER_SIZE, WGPEER_A_PUBLIC_KEY, sizeof(peer->public_key), peer->public_key))
			goto toobig_peers;
		if (peer->flags & WGPEER_REMOVE_ME)
			flags |= WGPEER_F_REMOVE_ME;
		if (!allowedip) {
			if (peer->flags & WGPEER_REPLACE_ALLOWEDIPS)
				flags |= WGPEER_F_REPLACE_ALLOWEDIPS;
			if (peer->flags & WGPEER_HAS_PRESHARED_KEY) {
				if (!mnl_attr_put_check(nlh, MNL_SOCKET_BUFFER_SIZE, WGPEER_A_PRESHARED_KEY, sizeof(peer->preshared_key), peer->preshared_key))
					goto toobig_peers;
			}
			if (peer->endpoint.addr.sa_family == AF_INET) {
				if (!mnl_attr_put_check(nlh, MNL_SOCKET_BUFFER_SIZE, WGPEER_A_ENDPOINT, sizeof(peer->endpoint.addr4), &peer->endpoint.addr4))
					goto toobig_peers;
			} else if (peer->endpoint.addr.sa_family == AF_INET6) {
				if (!mnl_attr_put_check(nlh, MNL_SOCKET_BUFFER_SIZE, WGPEER_A_ENDPOINT, sizeof(peer->endpoint.addr6), &peer->endpoint.addr6))
					goto toobig_peers;
			}
			if (peer->flags & WGPEER_HAS_PERSISTENT_KEEPALIVE_INTERVAL) {
				if (!mnl_attr_put_u16_check(nlh, MNL_SOCKET_BUFFER_SIZE, WGPEER_A_PERSISTENT_KEEPALIVE_INTERVAL, peer->persistent_keepalive_interval))
					goto toobig_peers;
			}
		}
		if (flags) {
			if (!mnl_attr_put_u32_check(nlh, MNL_SOCKET_BUFFER_SIZE, WGPEER_A_FLAGS, flags))
				goto toobig_peers;
		}
		if (peer->first_allowedip) {
			if (!allowedip)
				allowedip = peer->first_allowedip;
			allowedips_nest = mnl_attr_nest_start_check(nlh, MNL_SOCKET_BUFFER_SIZE, WGPEER_A_ALLOWEDIPS);
			if (!allowedips_nest)
				goto toobig_allowedips;
			for (; allowedip; allowedip = allowedip->next_allowedip) {
				allowedip_nest = mnl_attr_nest_start_check(nlh, MNL_SOCKET_BUFFER_SIZE, 0);
				if (!allowedip_nest)
					goto toobig_allowedips;
				if (!mnl_attr_put_u16_check(nlh, MNL_SOCKET_BUFFER_SIZE, WGALLOWEDIP_A_FAMILY, allowedip->family))
					goto toobig_allowedips;
				if (allowedip->family == AF_INET) {
					if (!mnl_attr_put_check(nlh, MNL_SOCKET_BUFFER_SIZE, WGALLOWEDIP_A_IPADDR, sizeof(allowedip->ip4), &allowedip->ip4))
						goto toobig_allowedips;
				} else if (allowedip->family == AF_INET6) {
					if (!mnl_attr_put_check(nlh, MNL_SOCKET_BUFFER_SIZE, WGALLOWEDIP_A_IPADDR, sizeof(allowedip->ip6), &allowedip->ip6))
						goto toobig_allowedips;
				}
				if (!mnl_attr_put_u8_check(nlh, MNL_SOCKET_BUFFER_SIZE, WGALLOWEDIP_A_CIDR_MASK, allowedip->cidr))
					goto toobig_allowedips;
				mnl_attr_nest_end(nlh, allowedip_nest);
				allowedip_nest = NULL;
			}
			mnl_attr_nest_end(nlh, allowedips_nest);
			allowedips_nest = NULL;
		}

		mnl_attr_nest_end(nlh, peer_nest);
		peer_nest = NULL;
	}
	mnl_attr_nest_end(nlh, peers_nest);
	peers_nest = NULL;
	goto send;
toobig_allowedips:
	if (allowedip_nest)
		mnl_attr_nest_cancel(nlh, allowedip_nest);
	if (allowedips_nest)
		mnl_attr_nest_end(nlh, allowedips_nest);
	mnl_attr_nest_end(nlh, peer_nest);
	mnl_attr_nest_end(nlh, peers_nest);
	goto send;
toobig_peers:
	if (peer_nest)
		mnl_attr_nest_cancel(nlh, peer_nest);
	mnl_attr_nest_end(nlh, peers_nest);
	goto send;
send:
	if (mnlg_socket_send(nlg, nlh) < 0) {
		ret = -errno;
		goto out;
	}
	errno = 0;
	if (mnlg_socket_recv_run(nlg, NULL, NULL) < 0) {
		ret = errno ? -errno : -EINVAL;
		goto out;
	}
	if (peer)
		goto again;

out:
	mnlg_socket_close(nlg);
	errno = -ret;
	return ret;
}

static int parse_allowedip(const struct nlattr *attr, void *data)
{
	wg_allowedip *allowedip = data;

	switch (mnl_attr_get_type(attr)) {
	case WGALLOWEDIP_A_UNSPEC:
		break;
	case WGALLOWEDIP_A_FAMILY:
		if (!mnl_attr_validate(attr, MNL_TYPE_U16))
			allowedip->family = mnl_attr_get_u16(attr);
		break;
	case WGALLOWEDIP_A_IPADDR:
		if (mnl_attr_get_payload_len(attr) == sizeof(allowedip->ip4))
			memcpy(&allowedip->ip4, mnl_attr_get_payload(attr), sizeof(allowedip->ip4));
		else if (mnl_attr_get_payload_len(attr) == sizeof(allowedip->ip6))
			memcpy(&allowedip->ip6, mnl_attr_get_payload(attr), sizeof(allowedip->ip6));
		break;
	case WGALLOWEDIP_A_CIDR_MASK:
		if (!mnl_attr_validate(attr, MNL_TYPE_U8))
			allowedip->cidr = mnl_attr_get_u8(attr);
		break;
	}

	return MNL_CB_OK;
}

static int parse_allowedips(const struct nlattr *attr, void *data)
{
	wg_peer *peer = data;
	wg_allowedip *new_allowedip = calloc(1, sizeof(wg_allowedip));
	int ret;

	if (!new_allowedip)
		return MNL_CB_ERROR;
	if (!peer->first_allowedip)
		peer->first_allowedip = peer->last_allowedip = new_allowedip;
	else {
		peer->last_allowedip->next_allowedip = new_allowedip;
		peer->last_allowedip = new_allowedip;
	}
	ret = mnl_attr_parse_nested(attr, parse_allowedip, new_allowedip);
	if (!ret)
		return ret;
	if (!((new_allowedip->family == AF_INET && new_allowedip->cidr <= 32) || (new_allowedip->family == AF_INET6 && new_allowedip->cidr <= 128))) {
		errno = EAFNOSUPPORT;
		return MNL_CB_ERROR;
	}
	return MNL_CB_OK;
}

bool wg_key_is_zero(const wg_key key)
{
	volatile uint8_t acc = 0;
	unsigned int i;

	for (i = 0; i < sizeof(wg_key); ++i) {
		acc |= key[i];
		__asm__ ("" : "=r" (acc) : "0" (acc));
	}
	return 1 & ((acc - 1) >> 8);
}

static int parse_peer(const struct nlattr *attr, void *data)
{
	wg_peer *peer = data;

	switch (mnl_attr_get_type(attr)) {
	case WGPEER_A_UNSPEC:
		break;
	case WGPEER_A_PUBLIC_KEY:
		if (mnl_attr_get_payload_len(attr) == sizeof(peer->public_key)) {
			memcpy(peer->public_key, mnl_attr_get_payload(attr), sizeof(peer->public_key));
			peer->flags |= WGPEER_HAS_PUBLIC_KEY;
		}
		break;
	case WGPEER_A_PRESHARED_KEY:
		if (mnl_attr_get_payload_len(attr) == sizeof(peer->preshared_key)) {
			memcpy(peer->preshared_key, mnl_attr_get_payload(attr), sizeof(peer->preshared_key));
			if (!wg_key_is_zero(peer->preshared_key))
				peer->flags |= WGPEER_HAS_PRESHARED_KEY;
		}
		break;
	case WGPEER_A_ENDPOINT: {
		struct sockaddr *addr;

		if (mnl_attr_get_payload_len(attr) < sizeof(*addr))
			break;
		addr = mnl_attr_get_payload(attr);
		if (addr->sa_family == AF_INET && mnl_attr_get_payload_len(attr) == sizeof(peer->endpoint.addr4))
			memcpy(&peer->endpoint.addr4, addr, sizeof(peer->endpoint.addr4));
		else if (addr->sa_family == AF_INET6 && mnl_attr_get_payload_len(attr) == sizeof(peer->endpoint.addr6))
			memcpy(&peer->endpoint.addr6, addr, sizeof(peer->endpoint.addr6));
		break;
	}
	case WGPEER_A_PERSISTENT_KEEPALIVE_INTERVAL:
		if (!mnl_attr_validate(attr, MNL_TYPE_U16))
			peer->persistent_keepalive_interval = mnl_attr_get_u16(attr);
		break;
	case WGPEER_A_LAST_HANDSHAKE_TIME:
		if (mnl_attr_get_payload_len(attr) == sizeof(peer->last_handshake_time))
			memcpy(&peer->last_handshake_time, mnl_attr_get_payload(attr), sizeof(peer->last_handshake_time));
		break;
	case WGPEER_A_RX_BYTES:
		if (!mnl_attr_validate(attr, MNL_TYPE_U64))
			peer->rx_bytes = mnl_attr_get_u64(attr);
		break;
	case WGPEER_A_TX_BYTES:
		if (!mnl_attr_validate(attr, MNL_TYPE_U64))
			peer->tx_bytes = mnl_attr_get_u64(attr);
		break;
	case WGPEER_A_ALLOWEDIPS:
		return mnl_attr_parse_nested(attr, parse_allowedips, peer);
	}

	return MNL_CB_OK;
}

static int parse_peers(const struct nlattr *attr, void *data)
{
	wg_device *device = data;
	wg_peer *new_peer = calloc(1, sizeof(wg_peer));
	int ret;

	if (!new_peer)
		return MNL_CB_ERROR;
	if (!device->first_peer)
		device->first_peer = device->last_peer = new_peer;
	else {
		device->last_peer->next_peer = new_peer;
		device->last_peer = new_peer;
	}
	ret = mnl_attr_parse_nested(attr, parse_peer, new_peer);
	if (!ret)
		return ret;
	if (!(new_peer->flags & WGPEER_HAS_PUBLIC_KEY)) {
		errno = ENXIO;
		return MNL_CB_ERROR;
	}
	return MNL_CB_OK;
}

static int parse_device(const struct nlattr *attr, void *data)
{
	wg_device *device = data;

	switch (mnl_attr_get_type(attr)) {
	case WGDEVICE_A_UNSPEC:
		break;
	case WGDEVICE_A_IFINDEX:
		if (!mnl_attr_validate(attr, MNL_TYPE_U32))
			device->ifindex = mnl_attr_get_u32(attr);
		break;
	case WGDEVICE_A_IFNAME:
		if (!mnl_attr_validate(attr, MNL_TYPE_STRING)) {
			strncpy(device->name, mnl_attr_get_str(attr), sizeof(device->name) - 1);
			device->name[sizeof(device->name) - 1] = '\0';
		}
		break;
	case WGDEVICE_A_PRIVATE_KEY:
		if (mnl_attr_get_payload_len(attr) == sizeof(device->private_key)) {
			memcpy(device->private_key, mnl_attr_get_payload(attr), sizeof(device->private_key));
			device->flags |= WGDEVICE_HAS_PRIVATE_KEY;
		}
		break;
	case WGDEVICE_A_PUBLIC_KEY:
		if (mnl_attr_get_payload_len(attr) == sizeof(device->public_key)) {
			memcpy(device->public_key, mnl_attr_get_payload(attr), sizeof(device->public_key));
			device->flags |= WGDEVICE_HAS_PUBLIC_KEY;
		}
		break;
	case WGDEVICE_A_LISTEN_PORT:
		if (!mnl_attr_validate(attr, MNL_TYPE_U16))
			device->listen_port = mnl_attr_get_u16(attr);
		break;
	case WGDEVICE_A_FWMARK:
		if (!mnl_attr_validate(attr, MNL_TYPE_U32))
			device->fwmark = mnl_attr_get_u32(attr);
		break;
	case WGDEVICE_A_PEERS:
		return mnl_attr_parse_nested(attr, parse_peers, device);
	}

	return MNL_CB_OK;
}

static int read_device_cb(const struct nlmsghdr *nlh, void *data)
{
	return mnl_attr_parse(nlh, sizeof(struct genlmsghdr), parse_device, data);
}

static void coalesce_peers(wg_device *device)
{
	wg_peer *old_next_peer, *peer = device->first_peer;

	while (peer && peer->next_peer) {
		if (memcmp(peer->public_key, peer->next_peer->public_key, sizeof(wg_key))) {
			peer = peer->next_peer;
			continue;
		}
		if (!peer->first_allowedip) {
			peer->first_allowedip = peer->next_peer->first_allowedip;
			peer->last_allowedip = peer->next_peer->last_allowedip;
		} else {
			peer->last_allowedip->next_allowedip = peer->next_peer->first_allowedip;
			peer->last_allowedip = peer->next_peer->last_allowedip;
		}
		old_next_peer = peer->next_peer;
		peer->next_peer = old_next_peer->next_peer;
		free(old_next_peer);
	}
}

int wg_get_device(wg_device **device, const char *device_name)
{
	int ret = 0;
	struct nlmsghdr *nlh;
	struct mnlg_socket *nlg;

try_again:
	*device = calloc(1, sizeof(wg_device));
	if (!*device)
		return -errno;

	nlg = mnlg_socket_open(WG_GENL_NAME, WG_GENL_VERSION);
	if (!nlg) {
		wg_free_device(*device);
		*device = NULL;
		return -errno;
	}

	nlh = mnlg_msg_prepare(nlg, WG_CMD_GET_DEVICE, NLM_F_REQUEST | NLM_F_ACK | NLM_F_DUMP);
	mnl_attr_put_strz(nlh, WGDEVICE_A_IFNAME, device_name);
	if (mnlg_socket_send(nlg, nlh) < 0) {
		ret = -errno;
		goto out;
	}
	errno = 0;
	if (mnlg_socket_recv_run(nlg, read_device_cb, *device) < 0) {
		ret = errno ? -errno : -EINVAL;
		goto out;
	}
	coalesce_peers(*device);

out:
	if (nlg)
		mnlg_socket_close(nlg);
	if (ret) {
		wg_free_device(*device);
		if (ret == -EINTR)
			goto try_again;
		*device = NULL;
	}
	errno = -ret;
	return ret;
}

/* first\0second\0third\0forth\0last\0\0 */
char *wg_list_device_names(void)
{
	struct inflatable_buffer buffer = { .len = MNL_SOCKET_BUFFER_SIZE };
	int ret;

	ret = -ENOMEM;
	buffer.buffer = calloc(1, buffer.len);
	if (!buffer.buffer)
		goto err;

	ret = fetch_device_names(&buffer);
err:
	errno = -ret;
	if (errno) {
		free(buffer.buffer);
		return NULL;
	}
	return buffer.buffer;
}

int wg_add_device(const char *device_name)
{
	return add_del_iface(device_name, true);
}

int wg_del_device(const char *device_name)
{
	return add_del_iface(device_name, false);
}

void wg_free_device(wg_device *dev)
{
	wg_peer *peer, *np;
	wg_allowedip *allowedip, *na;

	if (!dev)
		return;
	for (peer = dev->first_peer, np = peer ? peer->next_peer : NULL; peer; peer = np, np = peer ? peer->next_peer : NULL) {
		for (allowedip = peer->first_allowedip, na = allowedip ? allowedip->next_allowedip : NULL; allowedip; allowedip = na, na = allowedip ? allowedip->next_allowedip : NULL)
			free(allowedip);
		free(peer);
	}
	free(dev);
}

static void encode_base64(char dest[static 4], const uint8_t src[static 3])
{
	const uint8_t input[] = { (src[0] >> 2) & 63, ((src[0] << 4) | (src[1] >> 4)) & 63, ((src[1] << 2) | (src[2] >> 6)) & 63, src[2] & 63 };
	unsigned int i;

	for (i = 0; i < 4; ++i)
		dest[i] = input[i] + 'A'
			  + (((25 - input[i]) >> 8) & 6)
			  - (((51 - input[i]) >> 8) & 75)
			  - (((61 - input[i]) >> 8) & 15)
			  + (((62 - input[i]) >> 8) & 3);

}

void wg_key_to_base64(wg_key_b64_string base64, const wg_key key)
{
	unsigned int i;

	for (i = 0; i < 32 / 3; ++i)
		encode_base64(&base64[i * 4], &key[i * 3]);
	encode_base64(&base64[i * 4], (const uint8_t[]){ key[i * 3 + 0], key[i * 3 + 1], 0 });
	base64[sizeof(wg_key_b64_string) - 2] = '=';
	base64[sizeof(wg_key_b64_string) - 1] = '\0';
}

static int decode_base64(const char src[static 4])
{
	int val = 0;
	unsigned int i;

	for (i = 0; i < 4; ++i)
		val |= (-1
			    + ((((('A' - 1) - src[i]) & (src[i] - ('Z' + 1))) >> 8) & (src[i] - 64))
			    + ((((('a' - 1) - src[i]) & (src[i] - ('z' + 1))) >> 8) & (src[i] - 70))
			    + ((((('0' - 1) - src[i]) & (src[i] - ('9' + 1))) >> 8) & (src[i] + 5))
			    + ((((('+' - 1) - src[i]) & (src[i] - ('+' + 1))) >> 8) & 63)
			    + ((((('/' - 1) - src[i]) & (src[i] - ('/' + 1))) >> 8) & 64)
			) << (18 - 6 * i);
	return val;
}

int wg_key_from_base64(wg_key key, const wg_key_b64_string base64)
{
	unsigned int i;
	int val;
	volatile uint8_t ret = 0;

	if (strlen(base64) != sizeof(wg_key_b64_string) - 1 || base64[sizeof(wg_key_b64_string) - 2] != '=') {
		errno = EINVAL;
		goto out;
	}

	for (i = 0; i < 32 / 3; ++i) {
		val = decode_base64(&base64[i * 4]);
		ret |= (uint32_t)val >> 31;
		key[i * 3 + 0] = (val >> 16) & 0xff;
		key[i * 3 + 1] = (val >> 8) & 0xff;
		key[i * 3 + 2] = val & 0xff;
	}
	val = decode_base64((const char[]){ base64[i * 4 + 0], base64[i * 4 + 1], base64[i * 4 + 2], 'A' });
	ret |= ((uint32_t)val >> 31) | (val & 0xff);
	key[i * 3 + 0] = (val >> 16) & 0xff;
	key[i * 3 + 1] = (val >> 8) & 0xff;
	errno = EINVAL & ~((ret - 1) >> 8);
out:
	return -errno;
}

typedef int64_t fe[16];

static __attribute__((noinline)) void memzero_explicit(void *s, size_t count)
{
	memset(s, 0, count);
	__asm__ __volatile__("": :"r"(s) :"memory");
}

static void carry(fe o)
{
	int i;

	for (i = 0; i < 16; ++i) {
		o[(i + 1) % 16] += (i == 15 ? 38 : 1) * (o[i] >> 16);
		o[i] &= 0xffff;
	}
}

static void cswap(fe p, fe q, int b)
{
	int i;
	int64_t t, c = ~(b - 1);

	for (i = 0; i < 16; ++i) {
		t = c & (p[i] ^ q[i]);
		p[i] ^= t;
		q[i] ^= t;
	}

	memzero_explicit(&t, sizeof(t));
	memzero_explicit(&c, sizeof(c));
	memzero_explicit(&b, sizeof(b));
}

static void pack(uint8_t *o, const fe n)
{
	int i, j, b;
	fe m, t;

	memcpy(t, n, sizeof(t));
	carry(t);
	carry(t);
	carry(t);
	for (j = 0; j < 2; ++j) {
		m[0] = t[0] - 0xffed;
		for (i = 1; i < 15; ++i) {
			m[i] = t[i] - 0xffff - ((m[i - 1] >> 16) & 1);
			m[i - 1] &= 0xffff;
		}
		m[15] = t[15] - 0x7fff - ((m[14] >> 16) & 1);
		b = (m[15] >> 16) & 1;
		m[14] &= 0xffff;
		cswap(t, m, 1 - b);
	}
	for (i = 0; i < 16; ++i) {
		o[2 * i] = t[i] & 0xff;
		o[2 * i + 1] = t[i] >> 8;
	}

	memzero_explicit(m, sizeof(m));
	memzero_explicit(t, sizeof(t));
	memzero_explicit(&b, sizeof(b));
}

static void add(fe o, const fe a, const fe b)
{
	int i;

	for (i = 0; i < 16; ++i)
		o[i] = a[i] + b[i];
}

static void subtract(fe o, const fe a, const fe b)
{
	int i;

	for (i = 0; i < 16; ++i)
		o[i] = a[i] - b[i];
}

static void multmod(fe o, const fe a, const fe b)
{
	int i, j;
	int64_t t[31] = { 0 };

	for (i = 0; i < 16; ++i) {
		for (j = 0; j < 16; ++j)
			t[i + j] += a[i] * b[j];
	}
	for (i = 0; i < 15; ++i)
		t[i] += 38 * t[i + 16];
	memcpy(o, t, sizeof(fe));
	carry(o);
	carry(o);

	memzero_explicit(t, sizeof(t));
}

static void invert(fe o, const fe i)
{
	fe c;
	int a;

	memcpy(c, i, sizeof(c));
	for (a = 253; a >= 0; --a) {
		multmod(c, c, c);
		if (a != 2 && a != 4)
			multmod(c, c, i);
	}
	memcpy(o, c, sizeof(fe));

	memzero_explicit(c, sizeof(c));
}

static void clamp_key(uint8_t *z)
{
	z[31] = (z[31] & 127) | 64;
	z[0] &= 248;
}

void wg_generate_public_key(wg_key public_key, const wg_key private_key)
{
	int i, r;
	uint8_t z[32];
	fe a = { 1 }, b = { 9 }, c = { 0 }, d = { 1 }, e, f;

	memcpy(z, private_key, sizeof(z));
	clamp_key(z);

	for (i = 254; i >= 0; --i) {
		r = (z[i >> 3] >> (i & 7)) & 1;
		cswap(a, b, r);
		cswap(c, d, r);
		add(e, a, c);
		subtract(a, a, c);
		add(c, b, d);
		subtract(b, b, d);
		multmod(d, e, e);
		multmod(f, a, a);
		multmod(a, c, a);
		multmod(c, b, e);
		add(e, a, c);
		subtract(a, a, c);
		multmod(b, a, a);
		subtract(c, d, f);
		multmod(a, c, (const fe){ 0xdb41, 1 });
		add(a, a, d);
		multmod(c, c, a);
		multmod(a, d, f);
		multmod(d, b, (const fe){ 9 });
		multmod(b, e, e);
		cswap(a, b, r);
		cswap(c, d, r);
	}
	invert(c, c);
	multmod(a, a, c);
	pack(public_key, a);

	memzero_explicit(&r, sizeof(r));
	memzero_explicit(z, sizeof(z));
	memzero_explicit(a, sizeof(a));
	memzero_explicit(b, sizeof(b));
	memzero_explicit(c, sizeof(c));
	memzero_explicit(d, sizeof(d));
	memzero_explicit(e, sizeof(e));
	memzero_explicit(f, sizeof(f));
}

void wg_generate_private_key(wg_key private_key)
{
	wg_generate_preshared_key(private_key);
	clamp_key(private_key);
}

void wg_generate_preshared_key(wg_key preshared_key)
{
	ssize_t ret;
	size_t i;
	int fd;
#if defined(__OpenBSD__) || (defined(__APPLE__) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_12) || (defined(__GLIBC__) && (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 25)))
	if (!getentropy(preshared_key, sizeof(wg_key)))
		return;
#endif
#if defined(__NR_getrandom) && defined(__linux__)
	if (syscall(__NR_getrandom, preshared_key, sizeof(wg_key), 0) == sizeof(wg_key))
		return;
#endif
	fd = open("/dev/urandom", O_RDONLY);
	assert(fd >= 0);
	for (i = 0; i < sizeof(wg_key); i += ret) {
		ret = read(fd, preshared_key + i, sizeof(wg_key) - i);
		assert(ret > 0);
	}
	close(fd);
}
