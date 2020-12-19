/*
 *
 *  Connection Manager
 *
 *  Copyright (C) 2018  Commend International GmbH. All rights reserved.
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
 */

/*
 *  Address Conflict Detection (RFC 5227)
 *
 *  based on DHCP client library with GLib integration,
 *      Copyright (C) 2009-2014  Intel Corporation. All rights reserved.
 *
 */

#include <netinet/if_ether.h>
#include <net/if_arp.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "connman.h"
#include <connman/acd.h>
#include <connman/log.h>
#include <connman/inet.h>
#include <connman/dbus.h>
#include <glib.h>
#include "src/shared/arp.h"

enum acd_state {
	ACD_STATE_PROBE,
	ACD_STATE_ANNOUNCE,
	ACD_STATE_MONITOR,
	ACD_STATE_DEFEND,
};

static const char* acd_state_texts[] = {
	"PROBE",
	"ANNOUNCE",
	"MONITOR",
	"DEFEND"
};

struct acd_host {
	enum acd_state state;
	int ifindex;
	char *interface;
	uint8_t mac_address[6];
	uint32_t requested_ip; /* host byte order */

	/* address conflict fields */
	uint32_t ac_ip; /* host byte order */
	uint8_t ac_mac[6];
	gint64 ac_timestamp;
	bool ac_resolved;
	const char *path;

	bool listen_on;
	int listener_sockfd;
	unsigned int retry_times;
	unsigned int conflicts;
	guint timeout;
	guint listener_watch;

	acd_host_cb_t ipv4_available_cb;
	gpointer ipv4_available_data;
	acd_host_cb_t ipv4_lost_cb;
	gpointer ipv4_lost_data;
	acd_host_cb_t ipv4_conflict_cb;
	gpointer ipv4_conflict_data;
	acd_host_cb_t ipv4_max_conflicts_cb;
	gpointer ipv4_max_conflicts_data;
};

static int start_listening(struct acd_host *acd);
static void stop_listening(struct acd_host *acd);
static gboolean acd_listener_event(GIOChannel *channel, GIOCondition condition,
							gpointer acd_data);
static int acd_recv_arp_packet(struct acd_host *acd);
static void send_probe_packet(gpointer acd_data);
static gboolean acd_probe_timeout(gpointer acd_data);
static gboolean send_announce_packet(gpointer acd_data);
static gboolean acd_announce_timeout(gpointer acd_data);
static gboolean acd_defend_timeout(gpointer acd_data);

/* for D-Bus property */
static void report_conflict(struct acd_host *acd, const struct ether_arp* arp);

static void debug(struct acd_host *acd, const char *format, ...)
{
	char str[256];
	va_list ap;

	va_start(ap, format);

	if (vsnprintf(str, sizeof(str), format, ap) > 0)
		connman_info("ACD index %d: %s", acd->ifindex, str);

	va_end(ap);
}

void acd_host_free(struct acd_host *acd)
{
	if (!acd)
		return;

	g_free(acd->interface);
	g_free(acd);
}

struct acd_host *acd_host_new(int ifindex, const char *path)
{
	struct acd_host *acd;

	if (ifindex < 0) {
		connman_error("Invalid interface index %d", ifindex);
		return NULL;
	}

	acd = g_try_new0(struct acd_host, 1);
	if (!acd) {
		connman_error("Could not allocate ACD data structure");
		return NULL;
	}

	acd->interface = connman_inet_ifname(ifindex);
	if (!acd->interface) {
		connman_error("Interface with index %d is not available", ifindex);
		goto error;
	}

	if (!connman_inet_is_ifup(ifindex)) {
		connman_error("Interface with index %d and name %s is down", ifindex,
				acd->interface);
		goto error;
	}

	__connman_inet_get_interface_mac_address(ifindex, acd->mac_address);

	acd->listener_sockfd = -1;
	acd->listen_on = false;
	acd->ifindex = ifindex;
	acd->listener_watch = 0;
	acd->retry_times = 0;

	acd->ipv4_available_cb = NULL;
	acd->ipv4_lost_cb = NULL;
	acd->ipv4_conflict_cb = NULL;
	acd->ipv4_max_conflicts_cb = NULL;

	acd->ac_ip = 0;
	memset(acd->ac_mac, 0, sizeof(acd->ac_mac));
	acd->ac_timestamp = 0;
	acd->ac_resolved = true;
	acd->path = path;

	return acd;

error:
	acd_host_free(acd);
	return NULL;
}

static void remove_timeout(struct acd_host *acd)
{
	if (acd->timeout > 0)
		g_source_remove(acd->timeout);

	acd->timeout = 0;
}

static int start_listening(struct acd_host *acd)
{
	GIOChannel *listener_channel;
	int listener_sockfd;

	if (acd->listen_on)
		return 0;

	debug(acd, "start listening");

	listener_sockfd = arp_socket(acd->ifindex);
	if (listener_sockfd < 0)
		return -EIO;

	listener_channel = g_io_channel_unix_new(listener_sockfd);
	if (!listener_channel) {
		/* Failed to create listener channel */
		close(listener_sockfd);
		return -EIO;
	}

	acd->listen_on = true;
	acd->listener_sockfd = listener_sockfd;

	g_io_channel_set_close_on_unref(listener_channel, TRUE);
	acd->listener_watch =
			g_io_add_watch_full(listener_channel, G_PRIORITY_HIGH,
				G_IO_IN | G_IO_NVAL | G_IO_ERR | G_IO_HUP,
						acd_listener_event, acd,
								NULL);
	g_io_channel_unref(listener_channel);

	return 0;
}

static void stop_listening(struct acd_host *acd)
{
	if (!acd->listen_on)
		return;

	if (acd->listener_watch > 0)
		g_source_remove(acd->listener_watch);
	acd->listen_on = FALSE;
	acd->listener_sockfd = -1;
	acd->listener_watch = 0;
}

static gboolean acd_listener_event(GIOChannel *channel, GIOCondition condition,
							gpointer acd_data)
{
	struct acd_host *acd = acd_data;

	if (condition & (G_IO_NVAL | G_IO_ERR | G_IO_HUP)) {
		acd->listener_watch = 0;
		return FALSE;
	}

	if (!acd->listen_on)
		return FALSE;

	acd_recv_arp_packet(acd);

	return TRUE;
}

static bool is_link_local(uint32_t ip)
{
	return (ip & LINKLOCAL_ADDR) == LINKLOCAL_ADDR;
}

static int acd_recv_arp_packet(struct acd_host *acd)
{
	ssize_t cnt;
	struct ether_arp arp;
	uint32_t ip_n; /* network byte order */
	struct in_addr addr;
	int source_conflict;
	int target_conflict;
	bool probe;
	char* confltxt;
	uint8_t* mac;
	uint8_t* omac;

	memset(&arp, 0, sizeof(arp));
	cnt = read(acd->listener_sockfd, &arp, sizeof(arp));
	if (cnt != sizeof(arp))
		return -EINVAL;

	if (arp.arp_op != htons(ARPOP_REPLY) &&
			arp.arp_op != htons(ARPOP_REQUEST))
		return -EINVAL;

	if (memcmp(arp.arp_sha, acd->mac_address, ETH_ALEN) == 0)
		return 0;

	ip_n = htonl(acd->requested_ip);
	source_conflict = !memcmp(arp.arp_spa, &ip_n, sizeof(uint32_t));
	probe = !memcmp(arp.arp_spa, "\0\0\0\0", sizeof(uint32_t));
	target_conflict = probe &&
		!memcmp(arp.arp_tpa, &ip_n, sizeof(uint32_t));

	if (!source_conflict && !target_conflict)
		return 0;

	acd->conflicts++;

	confltxt = target_conflict ? "target" : "source";

	addr.s_addr = ip_n;
	debug(acd, "IPv4 %d %s conflicts detected for address %s. "
			"State=%s", acd->conflicts, confltxt, inet_ntoa(addr),
			acd_state_texts[acd->state]);
	mac = acd->mac_address;
	omac = arp.arp_sha;
	debug(acd, "Our MAC: %02x:%02x:%02x:%02x:%02x:%02x"
			   " other MAC: %02x:%02x:%02x:%02x:%02x:%02x",
			mac[0], mac[1], mac[2],mac[3], mac[4], mac[5],
			omac[0], omac[1], omac[2],omac[3], omac[4], omac[5]);

	if (acd->state == ACD_STATE_MONITOR) {
		if (!source_conflict)
			return 0;

		acd->state = ACD_STATE_DEFEND;
		debug(acd, "DEFEND mode conflicts: %d", acd->conflicts);
		/* Try to defend with a single announce. */
		send_announce_packet(acd);
		return 0;
	} else if (acd->state == ACD_STATE_DEFEND) {
		if (!source_conflict)
			return 0;

		debug(acd, "LOST IPv4 address %s", inet_ntoa(addr));
		if (!is_link_local(acd->requested_ip))
			report_conflict(acd, &arp);

		if (acd->ipv4_lost_cb)
			acd->ipv4_lost_cb(acd, acd->ipv4_lost_data);
		return 0;
	}

	if (acd->conflicts < MAX_CONFLICTS) {
		if (!is_link_local(acd->requested_ip))
			report_conflict(acd, &arp);

		acd_host_stop(acd);

		/* we need a new request_ip */
		if (acd->ipv4_conflict_cb)
			acd->ipv4_conflict_cb(acd, acd->ipv4_conflict_data);
	} else {
		acd_host_stop(acd);

		/*
		 * Here we got a lot of conflicts, RFC3927 and RFC5227 state that we
		 * have to wait RATE_LIMIT_INTERVAL before retrying.
		 */
		if (acd->ipv4_max_conflicts_cb)
			acd->ipv4_max_conflicts_cb(acd,	acd->ipv4_max_conflicts_data);
	}

	return 0;
}

int acd_host_start(struct acd_host *acd, uint32_t ip)
{
	guint timeout;
	int err;

	remove_timeout(acd);

	err = start_listening(acd);
	if (err)
		return err;

	acd->retry_times = 0;
	acd->requested_ip = ip;

	/* First wait a random delay to avoid storm of ARP requests on boot */
	timeout = __connman_util_random_delay_ms(PROBE_WAIT);
	acd->state = ACD_STATE_PROBE;

	acd->timeout = g_timeout_add_full(G_PRIORITY_HIGH,
						timeout,
						acd_probe_timeout,
						acd,
						NULL);

	return 0;
}

void acd_host_stop(struct acd_host *acd)
{
	stop_listening(acd);

	remove_timeout(acd);

	if (acd->listener_watch > 0) {
		g_source_remove(acd->listener_watch);
		acd->listener_watch = 0;
	}

	acd->state = ACD_STATE_PROBE;
	acd->retry_times = 0;
	acd->requested_ip = 0;
}

static void send_probe_packet(gpointer acd_data)
{
	guint timeout;
	struct acd_host *acd = acd_data;

	debug(acd, "sending ARP probe request");
	remove_timeout(acd);
	if (acd->retry_times == 1) {
		acd->state = ACD_STATE_PROBE;
		start_listening(acd);
	}
	arp_send_packet(acd->mac_address, 0,
			acd->requested_ip, acd->ifindex);

	if (acd->retry_times < PROBE_NUM) {
		/* Add a random timeout in range of PROBE_MIN to PROBE_MAX. */
		timeout = __connman_util_random_delay_ms(PROBE_MAX-PROBE_MIN);
		timeout += PROBE_MIN * 1000;
	} else
		timeout = ANNOUNCE_WAIT * 1000;

	acd->timeout = g_timeout_add_full(G_PRIORITY_HIGH,
						 timeout,
						 acd_probe_timeout,
						 acd,
						 NULL);
}

static gboolean acd_probe_timeout(gpointer acd_data)
{
	struct acd_host *acd = acd_data;

	acd->timeout = 0;

	debug(acd, "acd probe timeout (retries %d)", acd->retry_times);
	if (acd->retry_times == PROBE_NUM) {
		acd->state = ACD_STATE_ANNOUNCE;
		acd->retry_times = 1;

		send_announce_packet(acd);
		return FALSE;
	}

	acd->retry_times++;
	send_probe_packet(acd);

	return FALSE;
}

static gboolean send_announce_packet(gpointer acd_data)
{
	struct acd_host *acd = acd_data;

	debug(acd, "sending ACD announce request");

	arp_send_packet(acd->mac_address,
				acd->requested_ip,
				acd->requested_ip,
				acd->ifindex);

	remove_timeout(acd);

	if (acd->state == ACD_STATE_DEFEND)
		acd->timeout = g_timeout_add_seconds_full(G_PRIORITY_HIGH,
						DEFEND_INTERVAL,
						acd_defend_timeout,
						acd,
						NULL);
	else
		acd->timeout = g_timeout_add_seconds_full(G_PRIORITY_HIGH,
						ANNOUNCE_INTERVAL,
						acd_announce_timeout,
						acd,
						NULL);
	return TRUE;
}

static gboolean acd_announce_timeout(gpointer acd_data)
{
	struct acd_host *acd = acd_data;

	acd->timeout = 0;

	debug(acd, "acd announce timeout (retries %d)", acd->retry_times);
	if (acd->retry_times != ANNOUNCE_NUM) {
		acd->retry_times++;
		send_announce_packet(acd);
		return FALSE;
	}

	debug(acd, "switching to monitor mode");
	acd->state = ACD_STATE_MONITOR;

	if (!acd->ac_resolved && !is_link_local(acd->requested_ip))
		report_conflict(acd, NULL);

	if (acd->ipv4_available_cb)
		acd->ipv4_available_cb(acd,
					acd->ipv4_available_data);
	acd->conflicts = 0;

	return FALSE;
}

static gboolean acd_defend_timeout(gpointer acd_data)
{
	struct acd_host *acd = acd_data;

	debug(acd, "back to MONITOR mode");
	acd->timeout = 0;
	acd->conflicts = 0;
	acd->state = ACD_STATE_MONITOR;

	return FALSE;
}

void acd_host_register_event(struct acd_host *acd,
			    enum acd_host_event event,
			    acd_host_cb_t func,
			    gpointer user_data)
{
	switch (event) {
	case ACD_HOST_EVENT_IPV4_AVAILABLE:
		acd->ipv4_available_cb = func;
		acd->ipv4_available_data = user_data;
		break;
	case ACD_HOST_EVENT_IPV4_LOST:
		acd->ipv4_lost_cb = func;
		acd->ipv4_lost_data = user_data;
		break;
	case ACD_HOST_EVENT_IPV4_CONFLICT:
		acd->ipv4_conflict_cb = func;
		acd->ipv4_conflict_data = user_data;
		break;
	case ACD_HOST_EVENT_IPV4_MAXCONFLICT:
		acd->ipv4_max_conflicts_cb = func;
		acd->ipv4_max_conflicts_data = user_data;
		break;
	default:
		connman_warn("%s unknown event %d.", __FUNCTION__, event);
		break;
	}
}

static void append_ac_mac(DBusMessageIter *iter, void *user_data)
{
	struct acd_host *acd = user_data;
	char mac[32];
	uint8_t *m = acd->ac_mac;
	const char *str = mac;

	snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x",
			m[0], m[1], m[2], m[3], m[4], m[5]);
	connman_dbus_dict_append_basic(iter, "Address",	DBUS_TYPE_STRING, &str);
}

static void append_ac_ipv4(DBusMessageIter *iter, void *user_data)
{
	struct acd_host *acd = user_data;
	struct in_addr addr;
	char *a;

	addr.s_addr = htonl(acd->ac_ip);
	a = inet_ntoa(addr);
	if (!a)
		a = "";
	connman_dbus_dict_append_basic(iter, "Address",	DBUS_TYPE_STRING, &a);
}

static void append_ac_property(DBusMessageIter *iter, void *user_data)
{
	struct acd_host *acd = user_data;

	connman_dbus_dict_append_dict(iter, "IPv4", append_ac_ipv4, acd);
	connman_dbus_dict_append_dict(iter, "Ethernet",	append_ac_mac, acd);
	connman_dbus_dict_append_basic(iter, "Timestamp", DBUS_TYPE_INT64,
			&acd->ac_timestamp);
	connman_dbus_dict_append_basic(iter, "Resolved", DBUS_TYPE_BOOLEAN,
			&acd->ac_resolved);
}

void acd_host_append_dbus_property(struct acd_host *acd, DBusMessageIter *dict)
{
	connman_dbus_dict_append_dict(dict, "LastAddressConflict",
			append_ac_property, acd);
}

static void report_conflict(struct acd_host *acd, const struct ether_arp* arp)
{
	if (arp) {
		acd->ac_ip = acd->requested_ip;
		memcpy(acd->ac_mac, arp->arp_sha, sizeof(acd->ac_mac));
		acd->ac_timestamp = g_get_real_time();
		acd->ac_resolved = false;
	} else {
		acd->ac_resolved = true;
	}

	connman_dbus_property_changed_dict(acd->path, CONNMAN_SERVICE_INTERFACE,
			"LastAddressConflict", append_ac_property, acd);
}

unsigned int acd_host_get_conflicts_count(struct acd_host *acd)
{
	return acd->conflicts;
}
