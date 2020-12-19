/*
 * (C) 2012 by Pablo Neira Ayuso <pablo@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This code has been sponsored by Sophos Astaro <http://www.sophos.com>
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

struct nftnl_expr_counter {
	uint64_t	pkts;
	uint64_t	bytes;
};

static int
nftnl_expr_counter_set(struct nftnl_expr *e, uint16_t type,
			  const void *data, uint32_t data_len)
{
	struct nftnl_expr_counter *ctr = nftnl_expr_data(e);

	switch(type) {
	case NFTNL_EXPR_CTR_BYTES:
		memcpy(&ctr->bytes, data, sizeof(ctr->bytes));
		break;
	case NFTNL_EXPR_CTR_PACKETS:
		memcpy(&ctr->pkts, data, sizeof(ctr->pkts));
		break;
	default:
		return -1;
	}
	return 0;
}

static const void *
nftnl_expr_counter_get(const struct nftnl_expr *e, uint16_t type,
			  uint32_t *data_len)
{
	struct nftnl_expr_counter *ctr = nftnl_expr_data(e);

	switch(type) {
	case NFTNL_EXPR_CTR_BYTES:
		*data_len = sizeof(ctr->bytes);
		return &ctr->bytes;
	case NFTNL_EXPR_CTR_PACKETS:
		*data_len = sizeof(ctr->pkts);
		return &ctr->pkts;
	}
	return NULL;
}

static int nftnl_expr_counter_cb(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = data;
	int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, NFTA_COUNTER_MAX) < 0)
		return MNL_CB_OK;

	switch(type) {
	case NFTA_COUNTER_BYTES:
	case NFTA_COUNTER_PACKETS:
		if (mnl_attr_validate(attr, MNL_TYPE_U64) < 0)
			abi_breakage();
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

static void
nftnl_expr_counter_build(struct nlmsghdr *nlh, const struct nftnl_expr *e)
{
	struct nftnl_expr_counter *ctr = nftnl_expr_data(e);

	if (e->flags & (1 << NFTNL_EXPR_CTR_BYTES))
		mnl_attr_put_u64(nlh, NFTA_COUNTER_BYTES, htobe64(ctr->bytes));
	if (e->flags & (1 << NFTNL_EXPR_CTR_PACKETS))
		mnl_attr_put_u64(nlh, NFTA_COUNTER_PACKETS, htobe64(ctr->pkts));
}

static int
nftnl_expr_counter_parse(struct nftnl_expr *e, struct nlattr *attr)
{
	struct nftnl_expr_counter *ctr = nftnl_expr_data(e);
	struct nlattr *tb[NFTA_COUNTER_MAX+1] = {};

	if (mnl_attr_parse_nested(attr, nftnl_expr_counter_cb, tb) < 0)
		return -1;

	if (tb[NFTA_COUNTER_BYTES]) {
		ctr->bytes = be64toh(mnl_attr_get_u64(tb[NFTA_COUNTER_BYTES]));
		e->flags |= (1 << NFTNL_EXPR_CTR_BYTES);
	}
	if (tb[NFTA_COUNTER_PACKETS]) {
		ctr->pkts = be64toh(mnl_attr_get_u64(tb[NFTA_COUNTER_PACKETS]));
		e->flags |= (1 << NFTNL_EXPR_CTR_PACKETS);
	}

	return 0;
}

static int nftnl_expr_counter_snprintf_default(char *buf, size_t len,
					       const struct nftnl_expr *e)
{
	struct nftnl_expr_counter *ctr = nftnl_expr_data(e);

	return snprintf(buf, len, "pkts %"PRIu64" bytes %"PRIu64" ",
			ctr->pkts, ctr->bytes);
}

static int nftnl_expr_counter_snprintf(char *buf, size_t len, uint32_t type,
				       uint32_t flags,
				       const struct nftnl_expr *e)
{
	switch (type) {
	case NFTNL_OUTPUT_DEFAULT:
		return nftnl_expr_counter_snprintf_default(buf, len, e);
	case NFTNL_OUTPUT_XML:
	case NFTNL_OUTPUT_JSON:
	default:
		break;
	}
	return -1;
}

struct expr_ops expr_ops_counter = {
	.name		= "counter",
	.alloc_len	= sizeof(struct nftnl_expr_counter),
	.max_attr	= NFTA_COUNTER_MAX,
	.set		= nftnl_expr_counter_set,
	.get		= nftnl_expr_counter_get,
	.parse		= nftnl_expr_counter_parse,
	.build		= nftnl_expr_counter_build,
	.snprintf	= nftnl_expr_counter_snprintf,
};
