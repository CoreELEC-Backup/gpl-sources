/*
 * Copyright (c) 2018 Máté Eckl <ecklm94@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <linux/netfilter/nf_tables.h>

#include "internal.h"
#include <libmnl/libmnl.h>
#include <libnftnl/expr.h>
#include <libnftnl/rule.h>

struct nftnl_expr_socket {
	enum nft_socket_keys	key;
	enum nft_registers	dreg;
};

static int
nftnl_expr_socket_set(struct nftnl_expr *e, uint16_t type,
		       const void *data, uint32_t data_len)
{
	struct nftnl_expr_socket *socket = nftnl_expr_data(e);

	switch (type) {
	case NFTNL_EXPR_SOCKET_KEY:
		memcpy(&socket->key, data, sizeof(socket->key));
		break;
	case NFTNL_EXPR_SOCKET_DREG:
		memcpy(&socket->dreg, data, sizeof(socket->dreg));
		break;
	default:
		return -1;
	}
	return 0;
}

static const void *
nftnl_expr_socket_get(const struct nftnl_expr *e, uint16_t type,
		       uint32_t *data_len)
{
	struct nftnl_expr_socket *socket = nftnl_expr_data(e);

	switch (type) {
	case NFTNL_EXPR_SOCKET_KEY:
		*data_len = sizeof(socket->key);
		return &socket->key;
	case NFTNL_EXPR_SOCKET_DREG:
		*data_len = sizeof(socket->dreg);
		return &socket->dreg;
	}
	return NULL;
}

static int nftnl_expr_socket_cb(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = data;
	int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, NFTA_SOCKET_MAX) < 0)
		return MNL_CB_OK;

	switch (type) {
	case NFTA_SOCKET_KEY:
	case NFTA_SOCKET_DREG:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0)
			abi_breakage();
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

static void
nftnl_expr_socket_build(struct nlmsghdr *nlh, const struct nftnl_expr *e)
{
	struct nftnl_expr_socket *socket = nftnl_expr_data(e);

	if (e->flags & (1 << NFTNL_EXPR_SOCKET_KEY))
		mnl_attr_put_u32(nlh, NFTA_SOCKET_KEY, htonl(socket->key));
	if (e->flags & (1 << NFTNL_EXPR_SOCKET_DREG))
		mnl_attr_put_u32(nlh, NFTA_SOCKET_DREG, htonl(socket->dreg));
}

static int
nftnl_expr_socket_parse(struct nftnl_expr *e, struct nlattr *attr)
{
	struct nftnl_expr_socket *socket = nftnl_expr_data(e);
	struct nlattr *tb[NFTA_SOCKET_MAX+1] = {};

	if (mnl_attr_parse_nested(attr, nftnl_expr_socket_cb, tb) < 0)
		return -1;

	if (tb[NFTA_SOCKET_KEY]) {
		socket->key = ntohl(mnl_attr_get_u32(tb[NFTA_SOCKET_KEY]));
		e->flags |= (1 << NFTNL_EXPR_SOCKET_KEY);
	}
	if (tb[NFTA_SOCKET_DREG]) {
		socket->dreg = ntohl(mnl_attr_get_u32(tb[NFTA_SOCKET_DREG]));
		e->flags |= (1 << NFTNL_EXPR_SOCKET_DREG);
	}

	return 0;
}

static const char *socket_key2str_array[NFT_SOCKET_MAX + 1] = {
	[NFT_SOCKET_TRANSPARENT] = "transparent",
	[NFT_SOCKET_MARK] = "mark",
};

static const char *socket_key2str(uint8_t key)
{
	if (key < NFT_SOCKET_MAX + 1)
		return socket_key2str_array[key];

	return "unknown";
}

static inline int str2socket_key(const char *str)
{
	int i;

	for (i = 0; i < NFT_SOCKET_MAX + 1; i++) {
		if (strcmp(str, socket_key2str_array[i]) == 0)
			return i;
	}

	errno = EINVAL;
	return -1;
}

static int
nftnl_expr_socket_snprintf_default(char *buf, size_t len,
			       const struct nftnl_expr *e)
{
	struct nftnl_expr_socket *socket = nftnl_expr_data(e);

	if (e->flags & (1 << NFTNL_EXPR_SOCKET_DREG)) {
		return snprintf(buf, len, "load %s => reg %u ",
				socket_key2str(socket->key), socket->dreg);
	}
	return 0;
}

static int
nftnl_expr_socket_snprintf(char *buf, size_t len, uint32_t type,
		       uint32_t flags, const struct nftnl_expr *e)
{
	switch (type) {
	case NFTNL_OUTPUT_DEFAULT:
		return nftnl_expr_socket_snprintf_default(buf, len, e);
	case NFTNL_OUTPUT_XML:
	case NFTNL_OUTPUT_JSON:
	default:
		break;
	}
	return -1;
}

struct expr_ops expr_ops_socket = {
	.name		= "socket",
	.alloc_len	= sizeof(struct nftnl_expr_socket),
	.max_attr	= NFTA_SOCKET_MAX,
	.set		= nftnl_expr_socket_set,
	.get		= nftnl_expr_socket_get,
	.parse		= nftnl_expr_socket_parse,
	.build		= nftnl_expr_socket_build,
	.snprintf	= nftnl_expr_socket_snprintf,
};
