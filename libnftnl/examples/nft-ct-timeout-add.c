/*
 * (C) 2012-2016 by Pablo Neira Ayuso <pablo@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <netinet/in.h>

#include <linux/netfilter.h>
#include <linux/netfilter/nf_tables.h>

#include <obj.h>
#include <libmnl/libmnl.h>
#include <libnftnl/object.h>

static struct nftnl_obj *obj_add_parse(int argc, char *argv[])
{
	size_t timeout_array_size;
	struct nftnl_obj *t;
	uint32_t *timeout;
	uint16_t family;
	uint8_t l4proto;

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
		return NULL;
	}

	if (strcmp(argv[4], "udp") == 0)
		l4proto = IPPROTO_UDP;
	else if (strcmp(argv[4], "tcp") == 0)
		l4proto = IPPROTO_TCP;
	else {
		fprintf(stderr, "Unknown layer 4 protocol\n");
		return NULL;
	}

	t = nftnl_obj_alloc();
	if (t == NULL) {
		perror("OOM");
		return NULL;
	}

	timeout_array_size = sizeof(uint32_t) * (NFTNL_CTTIMEOUT_TCP_MAX);
	timeout = calloc(1, timeout_array_size);
	if (timeout == NULL) {
		perror("OOM");
		return NULL;
	}

	timeout[NFTNL_CTTIMEOUT_TCP_ESTABLISHED] = 111;
	timeout[NFTNL_CTTIMEOUT_TCP_CLOSE] = 16;
	timeout[NFTNL_CTTIMEOUT_TCP_CLOSE_WAIT] = 14;
	nftnl_obj_set_u32(t, NFTNL_OBJ_FAMILY, family);
	nftnl_obj_set_u32(t, NFTNL_OBJ_TYPE, NFT_OBJECT_CT_TIMEOUT);
	nftnl_obj_set_str(t, NFTNL_OBJ_TABLE, argv[2]);
	nftnl_obj_set_str(t, NFTNL_OBJ_NAME, argv[3]);
	nftnl_obj_set_u8(t, NFTNL_OBJ_CT_TIMEOUT_L4PROTO, l4proto);
	nftnl_obj_set_u16(t, NFTNL_OBJ_CT_TIMEOUT_L3PROTO, NFPROTO_IPV4);
	nftnl_obj_set(t, NFTNL_OBJ_CT_TIMEOUT_ARRAY, timeout);
	return t;

}

int main(int argc, char *argv[])
{
	struct mnl_socket *nl;
	char buf[MNL_SOCKET_BUFFER_SIZE];
	struct nlmsghdr *nlh;
	uint32_t portid, seq, obj_seq, family;
	struct nftnl_obj *t;
	struct mnl_nlmsg_batch *batch;
	int ret;

	if (argc != 5) {
		fprintf(stderr, "%s <family> <table> <name> <protocol> \n", argv[0]);
		exit(EXIT_FAILURE);
	}

	t = obj_add_parse(argc, argv);
	if (t == NULL) {
		exit(EXIT_FAILURE);
	}

	seq = time(NULL);
	batch = mnl_nlmsg_batch_start(buf, sizeof(buf));

	nftnl_batch_begin(mnl_nlmsg_batch_current(batch), seq++);
	mnl_nlmsg_batch_next(batch);

	obj_seq = seq;
	family = nftnl_obj_get_u32(t, NFTNL_OBJ_FAMILY);
	nlh = nftnl_nlmsg_build_hdr(mnl_nlmsg_batch_current(batch),
				    NFT_MSG_NEWOBJ, family, NLM_F_ACK | NLM_F_CREATE, seq++);
	nftnl_obj_nlmsg_build_payload(nlh, t);
	nftnl_obj_free(t);
	mnl_nlmsg_batch_next(batch);

	nftnl_batch_end(mnl_nlmsg_batch_current(batch), seq++);
	mnl_nlmsg_batch_next(batch);

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

	if (mnl_socket_sendto(nl, mnl_nlmsg_batch_head(batch),
			      mnl_nlmsg_batch_size(batch)) < 0) {
		perror("mnl_socket_send");
		exit(EXIT_FAILURE);
	}

	mnl_nlmsg_batch_stop(batch);

	ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	while (ret > 0) {
		ret = mnl_cb_run(buf, ret, obj_seq, portid, NULL, NULL);
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
