/*
 *
 *  IPV4 Local Link library with GLib integration
 *
 *  Copyright (C) 2009-2010  Aldebaran Robotics. All rights reserved.
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
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <netinet/if_ether.h>

#include <arpa/inet.h>

#include <glib.h>
#include "ipv4ll.h"
#include "common.h"
#include "../src/connman.h"

/**
 * Return a random link local IP (in host byte order)
 */
uint32_t ipv4ll_random_ip(void)
{
	unsigned tmp;
	uint64_t rand;

	do {
		__connman_util_get_random(&rand);
		tmp = rand;
		tmp = tmp & IN_CLASSB_HOST;
	} while (tmp > (IN_CLASSB_HOST - 0x0200));
	return ((LINKLOCAL_ADDR + 0x0100) + tmp);
}

