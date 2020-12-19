/*
 * (C) 2013 by Pablo Neira Ayuso <pablo@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This software has been sponsored by Sophos Astaro <http://www.sophos.com>
 */

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <netinet/in.h>

#include <linux/netfilter.h>
#include <linux/netfilter/nf_tables.h>

#include <libmnl/libmnl.h>
#include <libnftnl/set.h>

int main(int argc, char *argv[])
{
	struct mnl_socket *nl;
	char buf[MNL_SOCKET_BUFFER_SIZE];
	struct nlmsghdr *nlh;
	uint32_t portid, seq, family, data;
	struct nftnl_set *s;
	struct nftnl_set_elem *e;
	int ret;

	if (argc != 4) {
		fprintf(stderr, "%s <family> <table> <set>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	s = nftnl_set_alloc();
	if (s == NULL) {
		perror("OOM");
		exit(EXIT_FAILURE);
	}

	seq = time(NULL);
	if (strcmp(argv[1], "ip") == 0)
		family = NFPROTO_IPV4;
	else if (strcmp(argv[1], "ip6") == 0)
		family = NFPROTO_IPV6;
	else if (strcmp(argv[1], "bridge") == 0)
		family = NFPROTO_BRIDGE;
	else if (strcmp(argv[1], "arp") == 0)
		family = NFPROTO_ARP;
	else {
		fprintf(stderr, "Unknown family: ip, ip6, bridge, arp\n");
		exit(EXIT_FAILURE);
	}

	nftnl_set_set(s, NFTNL_SET_TABLE, argv[2]);
	nftnl_set_set(s, NFTNL_SET_NAME, argv[3]);

	/* Add to dummy elements to set */
	e = nftnl_set_elem_alloc();
	if (e == NULL) {
		perror("OOM");
		exit(EXIT_FAILURE);
	}

	data = 0x1;
	nftnl_set_elem_set(e, NFTNL_SET_ELEM_KEY, &data, sizeof(data));
	nftnl_set_elem_add(s, e);

	e = nftnl_set_elem_alloc();
	if (e == NULL) {
		perror("OOM");
		exit(EXIT_FAILURE);
	}
	data = 0x2;
	nftnl_set_elem_set(e, NFTNL_SET_ELEM_KEY, &data, sizeof(data));
	nftnl_set_elem_add(s, e);

	nlh = nftnl_set_nlmsg_build_hdr(buf, NFT_MSG_DELSETELEM, family,
				      NLM_F_ACK, seq);
	nftnl_set_elems_nlmsg_build_payload(nlh, s);
	nftnl_set_free(s);

	nl = mnl_socket_open(NETLINK_NETFILTER);
	if (nl == NULL) {
		perror("mnl_socket_open");
		exit(EXIT_FAILURE);
	}

	if (mnl_socket_bind(nl, 0, MNL_SOCKET_AUTOPID) < 0) {
		perror("mnl_socket_bind");
		exit(EXIT_FAILURE);
	}
	portid = mnl_socket_get_portid(nl);

	if (mnl_socket_sendto(nl, nlh, nlh->nlmsg_len) < 0) {
		perror("mnl_socket_send");
		exit(EXIT_FAILURE);
	}

	ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	while (ret > 0) {
		ret = mnl_cb_run(buf, ret, seq, portid, NULL, NULL);
		if (ret <= 0)
			break;
		ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	}
	if (ret == -1) {
		perror("error");
		exit(EXIT_FAILURE);
	}
	mnl_socket_close(nl);

	return EXIT_SUCCESS;
}
