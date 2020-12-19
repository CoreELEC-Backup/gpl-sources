/*
 *
 *  Connection Manager
 *
 *  Copyright (C) 2007-2014  Intel Corporation. All rights reserved.
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
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/timex.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <glib.h>

#include "connman.h"

struct ntp_short {
	uint16_t seconds;
	uint16_t fraction;
} __attribute__ ((packed));

struct ntp_time {
	uint32_t seconds;
	uint32_t fraction;
} __attribute__ ((packed));

struct ntp_msg {
	uint8_t flags;			/* Mode, version and leap indicator */
	uint8_t stratum;		/* Stratum details */
	int8_t poll;			/* Maximum interval in log2 seconds */
	int8_t precision;		/* Clock precision in log2 seconds */
	struct ntp_short rootdelay;	/* Root delay */
	struct ntp_short rootdisp;	/* Root dispersion */
	uint32_t refid;			/* Reference ID */
	struct ntp_time reftime;	/* Reference timestamp */
	struct ntp_time orgtime;	/* Origin timestamp */
	struct ntp_time rectime;	/* Receive timestamp */
	struct ntp_time xmttime;	/* Transmit timestamp */
} __attribute__ ((packed));

#define OFFSET_1900_1970  2208988800UL	/* 1970 - 1900 in seconds */

#define STEPTIME_MIN_OFFSET  0.4

#define LOGTOD(a)  ((a) < 0 ? 1. / (1L << -(a)) : 1L << (int)(a))
#define NSEC_PER_SEC  ((uint64_t)1000000000ULL)
#ifndef ADJ_SETOFFSET
#define ADJ_SETOFFSET		0x0100  /* add 'time' to current time */
#endif

#define NTP_SEND_TIMEOUT       2
#define NTP_SEND_RETRIES       3

#define NTP_FLAG_LI_SHIFT      6
#define NTP_FLAG_LI_MASK       0x3
#define NTP_FLAG_LI_NOWARNING  0x0
#define NTP_FLAG_LI_ADDSECOND  0x1
#define NTP_FLAG_LI_DELSECOND  0x2
#define NTP_FLAG_LI_NOTINSYNC  0x3

#define NTP_FLAG_VN_SHIFT      3
#define NTP_FLAG_VN_MASK       0x7

#define NTP_FLAG_MD_SHIFT      0
#define NTP_FLAG_MD_MASK       0x7
#define NTP_FLAG_MD_UNSPEC     0
#define NTP_FLAG_MD_ACTIVE     1
#define NTP_FLAG_MD_PASSIVE    2
#define NTP_FLAG_MD_CLIENT     3
#define NTP_FLAG_MD_SERVER     4
#define NTP_FLAG_MD_BROADCAST  5
#define NTP_FLAG_MD_CONTROL    6
#define NTP_FLAG_MD_PRIVATE    7

#define NTP_FLAG_VN_VER3       3
#define NTP_FLAG_VN_VER4       4

#define NTP_FLAGS_ENCODE(li, vn, md)  ((uint8_t)( \
                      (((li) & NTP_FLAG_LI_MASK) << NTP_FLAG_LI_SHIFT) | \
                      (((vn) & NTP_FLAG_VN_MASK) << NTP_FLAG_VN_SHIFT) | \
                      (((md) & NTP_FLAG_MD_MASK) << NTP_FLAG_MD_SHIFT)))

#define NTP_FLAGS_LI_DECODE(flags)    ((uint8_t)(((flags) >> NTP_FLAG_LI_SHIFT) & NTP_FLAG_LI_MASK))
#define NTP_FLAGS_VN_DECODE(flags)    ((uint8_t)(((flags) >> NTP_FLAG_VN_SHIFT) & NTP_FLAG_VN_MASK))
#define NTP_FLAGS_MD_DECODE(flags)    ((uint8_t)(((flags) >> NTP_FLAG_MD_SHIFT) & NTP_FLAG_MD_MASK))

#define NTP_PRECISION_S    0
#define NTP_PRECISION_DS   -3
#define NTP_PRECISION_CS   -6
#define NTP_PRECISION_MS   -9
#define NTP_PRECISION_US   -19
#define NTP_PRECISION_NS   -29

struct ntp_data {
	char *timeserver;
	struct sockaddr_in6 timeserver_addr;
	struct timespec mtx_time;
	int transmit_fd;
	gint timeout_id;
	guint retries;
	guint channel_watch;
	gint poll_id;
	uint32_t timeout;
	__connman_ntp_cb_t cb;
	void *user_data;
};

static struct ntp_data *ntp_data;

static void free_ntp_data(struct ntp_data *nd)
{
	if (nd->poll_id)
		g_source_remove(nd->poll_id);
	if (nd->timeout_id)
		g_source_remove(nd->timeout_id);
	if (nd->channel_watch)
		g_source_remove(nd->channel_watch);
	if (nd->timeserver)
		g_free(nd->timeserver);
	g_free(nd);
}

static void send_packet(struct ntp_data *nd, struct sockaddr *server,
			uint32_t timeout);

static gboolean send_timeout(gpointer user_data)
{
	struct ntp_data *nd = user_data;

	DBG("send timeout %u (retries %d)", nd->timeout, nd->retries);

	if (nd->retries++ == NTP_SEND_RETRIES)
		nd->cb(false, nd->user_data);
	else
		send_packet(nd,	(struct sockaddr *)&nd->timeserver_addr,
			nd->timeout << 1);

	return FALSE;
}

static void send_packet(struct ntp_data *nd, struct sockaddr *server,
			uint32_t timeout)
{
	struct ntp_msg msg;
	struct timeval transmit_timeval;
	ssize_t len;
	void * addr;
	int size;
	char ipaddrstring[INET6_ADDRSTRLEN + 1];

	/*
	 * At some point, we could specify the actual system precision with:
	 *
	 *   clock_getres(CLOCK_REALTIME, &ts);
	 *   msg.precision = (int)log2(ts.tv_sec + (ts.tv_nsec * 1.0e-9));
	 */
	memset(&msg, 0, sizeof(msg));
	msg.flags = NTP_FLAGS_ENCODE(NTP_FLAG_LI_NOTINSYNC, NTP_FLAG_VN_VER4,
	    NTP_FLAG_MD_CLIENT);
	msg.poll = 10;
	msg.precision = NTP_PRECISION_S;

	if (server->sa_family == AF_INET) {
		size = sizeof(struct sockaddr_in);
		addr = (void *)&(((struct sockaddr_in *)&nd->timeserver_addr)->sin_addr);
	} else if (server->sa_family == AF_INET6) {
		size = sizeof(struct sockaddr_in6);
		addr = (void *)&nd->timeserver_addr.sin6_addr;
	} else {
		DBG("Family is neither ipv4 nor ipv6");
		nd->cb(false, nd->user_data);
		return;
	}

	gettimeofday(&transmit_timeval, NULL);
	clock_gettime(CLOCK_MONOTONIC, &nd->mtx_time);

	msg.xmttime.seconds = htonl(transmit_timeval.tv_sec + OFFSET_1900_1970);
	msg.xmttime.fraction = htonl(transmit_timeval.tv_usec * 1000);

	len = sendto(nd->transmit_fd, &msg, sizeof(msg), MSG_DONTWAIT,
						server, size);
	if (len < 0 || len != sizeof(msg)) {
		DBG("Time request for server %s failed",
			inet_ntop(server->sa_family, addr, ipaddrstring, sizeof(ipaddrstring)));
		nd->cb(false, nd->user_data);
		return;
	}

	/*
	 * Add an exponential retry timeout to retry the existing
	 * request. After a set number of retries, we'll fallback to
	 * trying another server.
	 */

	nd->timeout = timeout;
	nd->timeout_id = g_timeout_add_seconds(timeout, send_timeout, nd);
}

static gboolean next_poll(gpointer user_data)
{
	struct ntp_data *nd = user_data;
	nd->poll_id = 0;

	if (!nd->timeserver || nd->transmit_fd == 0)
		return FALSE;

	send_packet(nd, (struct sockaddr *)&nd->timeserver_addr, NTP_SEND_TIMEOUT);

	return FALSE;
}

static void reset_timeout(struct ntp_data *nd)
{
	if (nd->timeout_id > 0) {
		g_source_remove(nd->timeout_id);
		nd->timeout_id = 0;
	}

	nd->retries = 0;
}

static void decode_msg(struct ntp_data *nd, void *base, size_t len,
		struct timeval *tv, struct timespec *mrx_time)
{
	struct ntp_msg *msg = base;
	double m_delta, org, rec, xmt, dst;
	double delay, offset;
	static guint transmit_delay;
	struct timex tmx = {};

	if (len < sizeof(*msg)) {
		connman_error("Invalid response from time server");
		return;
	}

	if (!tv) {
		connman_error("Invalid packet timestamp from time server");
		return;
	}

	DBG("flags      : 0x%02x", msg->flags);
	DBG("stratum    : %u", msg->stratum);
	DBG("poll       : %f seconds (%d)",
				LOGTOD(msg->poll), msg->poll);
	DBG("precision  : %f seconds (%d)",
				LOGTOD(msg->precision), msg->precision);
	DBG("root delay : %u seconds (fraction %u)",
			msg->rootdelay.seconds, msg->rootdelay.fraction);
	DBG("root disp. : %u seconds (fraction %u)",
			msg->rootdisp.seconds, msg->rootdisp.fraction);
	DBG("reference  : 0x%04x", msg->refid);

	if (!msg->stratum) {
		/* RFC 4330 ch 8 Kiss-of-Death packet */
		uint32_t code = ntohl(msg->refid);

		connman_info("Skipping server %s KoD code %c%c%c%c",
			nd->timeserver, code >> 24, code >> 16 & 0xff,
			code >> 8 & 0xff, code & 0xff);
		nd->cb(false, nd->user_data);
		return;
	}

	transmit_delay = LOGTOD(msg->poll);

	if (NTP_FLAGS_LI_DECODE(msg->flags) == NTP_FLAG_LI_NOTINSYNC) {
		DBG("ignoring unsynchronized peer");
		nd->cb(false, nd->user_data);
		return;
	}


	if (NTP_FLAGS_VN_DECODE(msg->flags) != NTP_FLAG_VN_VER4) {
		if (NTP_FLAGS_VN_DECODE(msg->flags) == NTP_FLAG_VN_VER3) {
			DBG("requested version %d, accepting version %d",
				NTP_FLAG_VN_VER4, NTP_FLAGS_VN_DECODE(msg->flags));
		} else {
			DBG("unsupported version %d", NTP_FLAGS_VN_DECODE(msg->flags));
			nd->cb(false, nd->user_data);
			return;
		}
	}

	if (NTP_FLAGS_MD_DECODE(msg->flags) != NTP_FLAG_MD_SERVER) {
		DBG("unsupported mode %d", NTP_FLAGS_MD_DECODE(msg->flags));
		nd->cb(false, nd->user_data);
		return;
	}

	m_delta = mrx_time->tv_sec - nd->mtx_time.tv_sec +
		1.0e-9 * (mrx_time->tv_nsec - nd->mtx_time.tv_nsec);

	org = tv->tv_sec + (1.0e-6 * tv->tv_usec) - m_delta + OFFSET_1900_1970;
	rec = ntohl(msg->rectime.seconds) +
			((double) ntohl(msg->rectime.fraction) / UINT_MAX);
	xmt = ntohl(msg->xmttime.seconds) +
			((double) ntohl(msg->xmttime.fraction) / UINT_MAX);
	dst = tv->tv_sec + (1.0e-6 * tv->tv_usec) + OFFSET_1900_1970;

	DBG("org=%f rec=%f xmt=%f dst=%f", org, rec, xmt, dst);

	offset = ((rec - org) + (xmt - dst)) / 2;
	delay = (dst - org) - (xmt - rec);

	DBG("offset=%f delay=%f", offset, delay);

	/* Remove the timeout, as timeserver has responded */

	reset_timeout(nd);

	/*
	 * Now poll the server every transmit_delay seconds
	 * for time correction.
	 */
	if (nd->poll_id > 0)
		g_source_remove(nd->poll_id);

	DBG("Timeserver %s, next sync in %d seconds", nd->timeserver,
		transmit_delay);

	nd->poll_id = g_timeout_add_seconds(transmit_delay, next_poll, nd);

	if (offset < STEPTIME_MIN_OFFSET && offset > -STEPTIME_MIN_OFFSET) {
		tmx.modes = ADJ_STATUS | ADJ_NANO | ADJ_OFFSET | ADJ_TIMECONST | ADJ_MAXERROR | ADJ_ESTERROR;
		tmx.status = STA_PLL;
		tmx.offset = offset * NSEC_PER_SEC;
		tmx.constant = msg->poll - 4;
		tmx.maxerror = 0;
		tmx.esterror = 0;

		connman_info("ntp: adjust (slew): %+.6f sec", offset);
	} else {
		tmx.modes = ADJ_STATUS | ADJ_NANO | ADJ_SETOFFSET | ADJ_MAXERROR | ADJ_ESTERROR;

		/* ADJ_NANO uses nanoseconds in the microseconds field */
		tmx.time.tv_sec = (long)offset;
		tmx.time.tv_usec = (offset - tmx.time.tv_sec) * NSEC_PER_SEC;
		tmx.maxerror = 0;
		tmx.esterror = 0;

		/* the kernel expects -0.3s as {-1, 7000.000.000} */
		if (tmx.time.tv_usec < 0) {
			tmx.time.tv_sec  -= 1;
			tmx.time.tv_usec += NSEC_PER_SEC;
		}

		connman_info("ntp: adjust (jump): %+.6f sec", offset);
	}

	if (NTP_FLAGS_LI_DECODE(msg->flags) & NTP_FLAG_LI_ADDSECOND)
		tmx.status |= STA_INS;
	else if (NTP_FLAGS_LI_DECODE(msg->flags) & NTP_FLAG_LI_DELSECOND)
		tmx.status |= STA_DEL;

	if (adjtimex(&tmx) < 0) {
		connman_error("Failed to adjust time: %s (%d)", strerror(errno), errno);
		nd->cb(false, nd->user_data);
		return;
	}

	DBG("interval/delta/delay/drift %fs/%+.3fs/%.3fs/%+ldppm",
		LOGTOD(msg->poll), offset, delay, tmx.freq / 65536);

	nd->cb(true, nd->user_data);
}

static gboolean received_data(GIOChannel *channel, GIOCondition condition,
							gpointer user_data)
{
	struct ntp_data *nd = user_data;
	unsigned char buf[128];
	struct sockaddr_in6 sender_addr;
	struct msghdr msg;
	struct iovec iov;
	struct cmsghdr *cmsg;
	struct timeval *tv;
	struct timespec mrx_time;
	char aux[128];
	ssize_t len;
	int fd;
	int size;
	void * addr_ptr;
	void * src_ptr;

	if (condition & (G_IO_HUP | G_IO_ERR | G_IO_NVAL)) {
		connman_error("Problem with timer server channel");
		nd->channel_watch = 0;
		return FALSE;
	}

	fd = g_io_channel_unix_get_fd(channel);

	iov.iov_base = buf;
	iov.iov_len = sizeof(buf);

	memset(&msg, 0, sizeof(msg));
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = aux;
	msg.msg_controllen = sizeof(aux);
	msg.msg_name = &sender_addr;
	msg.msg_namelen = sizeof(sender_addr);

	len = recvmsg(fd, &msg, MSG_DONTWAIT);
	if (len < 0)
		return TRUE;

	if (sender_addr.sin6_family == AF_INET) {
		size = 4;
		addr_ptr = &((struct sockaddr_in *)&nd->timeserver_addr)->sin_addr;
		src_ptr = &((struct sockaddr_in *)&sender_addr)->sin_addr;
	} else if (sender_addr.sin6_family == AF_INET6) {
		size = 16;
		addr_ptr = &((struct sockaddr_in6 *)&nd->timeserver_addr)->sin6_addr;
		src_ptr = &((struct sockaddr_in6 *)&sender_addr)->sin6_addr;
	} else {
		connman_error("Not a valid family type");
		return TRUE;
	}

	if(memcmp(addr_ptr, src_ptr, size) != 0)
		return TRUE;

	tv = NULL;
	clock_gettime(CLOCK_MONOTONIC, &mrx_time);

	for (cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
		if (cmsg->cmsg_level != SOL_SOCKET)
			continue;

		switch (cmsg->cmsg_type) {
		case SCM_TIMESTAMP:
			tv = (struct timeval *) CMSG_DATA(cmsg);
			break;
		}
	}

	decode_msg(nd, iov.iov_base, iov.iov_len, tv, &mrx_time);

	return TRUE;
}

static void start_ntp(struct ntp_data *nd)
{
	GIOChannel *channel;
	struct addrinfo hint;
	struct addrinfo *info;
	struct sockaddr * addr;
	struct sockaddr_in  * in4addr;
	struct sockaddr_in6 in6addr;
	int size;
	int family;
	int tos = IPTOS_LOWDELAY, timestamp = 1;
	int ret;

	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_DGRAM;
	hint.ai_flags = AI_NUMERICHOST | AI_PASSIVE;
	ret = getaddrinfo(nd->timeserver, NULL, &hint, &info);

	if (ret) {
		connman_error("cannot get server info");
		return;
	}

	family = info->ai_family;

	memcpy(&ntp_data->timeserver_addr, info->ai_addr, info->ai_addrlen);
	freeaddrinfo(info);
	memset(&in6addr, 0, sizeof(in6addr));

	if (family == AF_INET) {
		((struct sockaddr_in *)&ntp_data->timeserver_addr)->sin_port = htons(123);
		in4addr = (struct sockaddr_in *)&in6addr;
		in4addr->sin_family = family;
		addr = (struct sockaddr *)in4addr;
		size = sizeof(struct sockaddr_in);
	} else if (family == AF_INET6) {
		ntp_data->timeserver_addr.sin6_port = htons(123);
		in6addr.sin6_family = family;
		addr = (struct sockaddr *)&in6addr;
		size = sizeof(in6addr);
	} else {
		connman_error("Family is neither ipv4 nor ipv6");
		return;
	}

	DBG("server %s family %d", nd->timeserver, family);

	if (nd->channel_watch > 0)
		goto send;

	nd->transmit_fd = socket(family, SOCK_DGRAM | SOCK_CLOEXEC, 0);

	if (nd->transmit_fd <= 0) {
		if (errno != EAFNOSUPPORT)
			connman_error("Failed to open time server socket");
	}

	if (bind(nd->transmit_fd, (struct sockaddr *) addr, size) < 0) {
		connman_error("Failed to bind time server socket");
		goto err;
	}

	if (family == AF_INET) {
		if (setsockopt(nd->transmit_fd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos)) < 0) {
			connman_error("Failed to set type of service option");
			goto err;
		}
	}

	if (setsockopt(nd->transmit_fd, SOL_SOCKET, SO_TIMESTAMP, &timestamp,
						sizeof(timestamp)) < 0) {
		connman_error("Failed to enable timestamp support");
		goto err;
	}

	channel = g_io_channel_unix_new(nd->transmit_fd);
	if (!channel)
		goto err;

	g_io_channel_set_encoding(channel, NULL, NULL);
	g_io_channel_set_buffered(channel, FALSE);

	g_io_channel_set_close_on_unref(channel, TRUE);

	nd->channel_watch = g_io_add_watch_full(channel, G_PRIORITY_DEFAULT,
				G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_NVAL,
				received_data, nd, NULL);

	g_io_channel_unref(channel);

send:
	send_packet(nd, (struct sockaddr*)&ntp_data->timeserver_addr,
		NTP_SEND_TIMEOUT);
	return;

err:
	if (nd->transmit_fd > 0)
		close(nd->transmit_fd);

	nd->cb(false, nd->user_data);
}

int __connman_ntp_start(char *server, __connman_ntp_cb_t callback,
			void *user_data)
{
	if (!server)
		return -EINVAL;

	if (ntp_data) {
		connman_warn("ntp_data is not NULL (timerserver %s)",
			ntp_data->timeserver);
		free_ntp_data(ntp_data);
	}

	ntp_data = g_new0(struct ntp_data, 1);

	ntp_data->timeserver = g_strdup(server);
	ntp_data->cb = callback;
	ntp_data->user_data = user_data;

	start_ntp(ntp_data);

	return 0;
}

void __connman_ntp_stop()
{
	if (ntp_data) {
		free_ntp_data(ntp_data);
		ntp_data = NULL;
	}
}
