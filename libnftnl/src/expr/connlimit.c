/*
 * (C) 2018 by Pablo Neira Ayuso <pablo@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <errno.h>
#include <inttypes.h>

#include <linux/netfilter/nf_tables.h>

#include "internal.h"
#include <libmnl/libmnl.h>
#include <libnftnl/expr.h>
#include <libnftnl/rule.h>

struct nftnl_expr_connlimit {
	uint32_t	count;
	uint32_t	flags;
};

static int
nftnl_expr_connlimit_set(struct nftnl_expr *e, uint16_t type,
			  const void *data, uint32_t data_len)
{
	struct nftnl_expr_connlimit *connlimit = nftnl_expr_data(e);

	switch(type) {
	case NFTNL_EXPR_CONNLIMIT_COUNT:
		memcpy(&connlimit->count, data, sizeof(connlimit->count));
		break;
	case NFTNL_EXPR_CONNLIMIT_FLAGS:
		memcpy(&connlimit->flags, data, sizeof(connlimit->flags));
		break;
	default:
		return -1;
	}
	return 0;
}

static const void *
nftnl_expr_connlimit_get(const struct nftnl_expr *e, uint16_t type,
			  uint32_t *data_len)
{
	struct nftnl_expr_connlimit *connlimit = nftnl_expr_data(e);

	switch(type) {
	case NFTNL_EXPR_CONNLIMIT_COUNT:
		*data_len = sizeof(connlimit->count);
		return &connlimit->count;
	case NFTNL_EXPR_CONNLIMIT_FLAGS:
		*data_len = sizeof(connlimit->flags);
		return &connlimit->flags;
	}
	return NULL;
}

static int nftnl_expr_connlimit_cb(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = data;
	int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, NFTA_CONNLIMIT_MAX) < 0)
		return MNL_CB_OK;

	switch(type) {
	case NFTA_CONNLIMIT_COUNT:
	case NFTA_CONNLIMIT_FLAGS:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0)
			abi_breakage();
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

static void
nftnl_expr_connlimit_build(struct nlmsghdr *nlh, const struct nftnl_expr *e)
{
	struct nftnl_expr_connlimit *connlimit = nftnl_expr_data(e);

	if (e->flags & (1 << NFTNL_EXPR_CONNLIMIT_COUNT))
		mnl_attr_put_u32(nlh, NFTA_CONNLIMIT_COUNT,
				 htonl(connlimit->count));
	if (e->flags & (1 << NFTNL_EXPR_CONNLIMIT_FLAGS))
		mnl_attr_put_u32(nlh, NFTA_CONNLIMIT_FLAGS,
				 htonl(connlimit->flags));
}

static int
nftnl_expr_connlimit_parse(struct nftnl_expr *e, struct nlattr *attr)
{
	struct nftnl_expr_connlimit *connlimit = nftnl_expr_data(e);
	struct nlattr *tb[NFTA_CONNLIMIT_MAX+1] = {};

	if (mnl_attr_parse_nested(attr, nftnl_expr_connlimit_cb, tb) < 0)
		return -1;

	if (tb[NFTA_CONNLIMIT_COUNT]) {
		connlimit->count =
			ntohl(mnl_attr_get_u32(tb[NFTA_CONNLIMIT_COUNT]));
		e->flags |= (1 << NFTNL_EXPR_CONNLIMIT_COUNT);
	}
	if (tb[NFTA_CONNLIMIT_FLAGS]) {
		connlimit->flags =
			ntohl(mnl_attr_get_u32(tb[NFTA_CONNLIMIT_FLAGS]));
		e->flags |= (1 << NFTNL_EXPR_CONNLIMIT_FLAGS);
	}

	return 0;
}

static int nftnl_expr_connlimit_snprintf_default(char *buf, size_t len,
					       const struct nftnl_expr *e)
{
	struct nftnl_expr_connlimit *connlimit = nftnl_expr_data(e);

	return snprintf(buf, len, "count %u flags %x ",
			connlimit->count, connlimit->flags);
}

static int nftnl_expr_connlimit_snprintf(char *buf, size_t len, uint32_t type,
				       uint32_t flags,
				       const struct nftnl_expr *e)
{
	switch (type) {
	case NFTNL_OUTPUT_DEFAULT:
		return nftnl_expr_connlimit_snprintf_default(buf, len, e);
	case NFTNL_OUTPUT_XML:
	case NFTNL_OUTPUT_JSON:
	default:
		break;
	}
	return -1;
}

struct expr_ops expr_ops_connlimit = {
	.name		= "connlimit",
	.alloc_len	= sizeof(struct nftnl_expr_connlimit),
	.max_attr	= NFTA_CONNLIMIT_MAX,
	.set		= nftnl_expr_connlimit_set,
	.get		= nftnl_expr_connlimit_get,
	.parse		= nftnl_expr_connlimit_parse,
	.build		= nftnl_expr_connlimit_build,
	.snprintf	= nftnl_expr_connlimit_snprintf,
};
