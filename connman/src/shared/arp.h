/*
 *
 *  Connection Manager
 *
 *  based on IPv4 Local Link library with GLib integration,
 *	    Copyright (C) 2009-2010  Aldebaran Robotics. All rights reserved.
 *
 *  Copyright (C) 2018  Commend International. All rights reserved.
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

#ifndef SHARED_ARP_H
#define SHARED_ARP_H

#include <stdint.h>

/* IPv4 Link-Local (RFC 3927), IPv4 Address Conflict Detection (RFC 5227) */
#define PROBE_WAIT	     1
#define PROBE_NUM	     3
#define PROBE_MIN	     1
#define PROBE_MAX	     2
#define ANNOUNCE_WAIT	     2
#define ANNOUNCE_NUM	     2
#define ANNOUNCE_INTERVAL    2
#define MAX_CONFLICTS	    10
#define RATE_LIMIT_INTERVAL 60
#define DEFEND_INTERVAL	    10

/* 169.254.0.0 */
#define LINKLOCAL_ADDR 0xa9fe0000

int arp_send_packet(uint8_t* source_eth, uint32_t source_ip,
		    uint32_t target_ip, int ifindex);
int arp_socket(int ifindex);

uint32_t arp_random_ip(void);

#endif
