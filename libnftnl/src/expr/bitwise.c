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

#include "internal.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h> /* for memcpy */
#include <arpa/inet.h>
#include <errno.h>
#include <libmnl/libmnl.h>
#include <linux/netfilter/nf_tables.h>
#include <libnftnl/expr.h>
#include <libnftnl/rule.h>

struct nftnl_expr_bitwise {
	enum nft_registers	sreg;
	enum nft_registers	dreg;
	unsigned int		len;
	union nftnl_data_reg	mask;
	union nftnl_data_reg	xor;
};

static int
nftnl_expr_bitwise_set(struct nftnl_expr *e, uint16_t type,
			  const void *data, uint32_t data_len)
{
	struct nftnl_expr_bitwise *bitwise = nftnl_expr_data(e);

	switch(type) {
	case NFTNL_EXPR_BITWISE_SREG:
		memcpy(&bitwise->sreg, data, sizeof(bitwise->sreg));
		break;
	case NFTNL_EXPR_BITWISE_DREG:
		memcpy(&bitwise->dreg, data, sizeof(bitwise->dreg));
		break;
	case NFTNL_EXPR_BITWISE_LEN:
		memcpy(&bitwise->len, data, sizeof(bitwise->len));
		break;
	case NFTNL_EXPR_BITWISE_MASK:
		memcpy(&bitwise->mask.val, data, data_len);
		bitwise->mask.len = data_len;
		break;
	case NFTNL_EXPR_BITWISE_XOR:
		memcpy(&bitwise->xor.val, data, data_len);
		bitwise->xor.len = data_len;
		break;
	default:
		return -1;
	}
	return 0;
}

static const void *
nftnl_expr_bitwise_get(const struct nftnl_expr *e, uint16_t type,
			  uint32_t *data_len)
{
	struct nftnl_expr_bitwise *bitwise = nftnl_expr_data(e);

	switch(type) {
	case NFTNL_EXPR_BITWISE_SREG:
		*data_len = sizeof(bitwise->sreg);
		return &bitwise->sreg;
	case NFTNL_EXPR_BITWISE_DREG:
		*data_len = sizeof(bitwise->dreg);
		return &bitwise->dreg;
	case NFTNL_EXPR_BITWISE_LEN:
		*data_len = sizeof(bitwise->len);
		return &bitwise->len;
	case NFTNL_EXPR_BITWISE_MASK:
		*data_len = bitwise->mask.len;
		return &bitwise->mask.val;
	case NFTNL_EXPR_BITWISE_XOR:
		*data_len = bitwise->xor.len;
		return &bitwise->xor.val;
	}
	return NULL;
}

static int nftnl_expr_bitwise_cb(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = data;
	int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, NFTA_BITWISE_MAX) < 0)
		return MNL_CB_OK;

	switch(type) {
	case NFTA_BITWISE_SREG:
	case NFTA_BITWISE_DREG:
	case NFTA_BITWISE_LEN:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0)
			abi_breakage();
		break;
	case NFTA_BITWISE_MASK:
	case NFTA_BITWISE_XOR:
		if (mnl_attr_validate(attr, MNL_TYPE_BINARY) < 0)
			abi_breakage();
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

static void
nftnl_expr_bitwise_build(struct nlmsghdr *nlh, const struct nftnl_expr *e)
{
	struct nftnl_expr_bitwise *bitwise = nftnl_expr_data(e);

	if (e->flags & (1 << NFTNL_EXPR_BITWISE_SREG))
		mnl_attr_put_u32(nlh, NFTA_BITWISE_SREG, htonl(bitwise->sreg));
	if (e->flags & (1 << NFTNL_EXPR_BITWISE_DREG))
		mnl_attr_put_u32(nlh, NFTA_BITWISE_DREG, htonl(bitwise->dreg));
	if (e->flags & (1 << NFTNL_EXPR_BITWISE_LEN))
		mnl_attr_put_u32(nlh, NFTA_BITWISE_LEN, htonl(bitwise->len));
	if (e->flags & (1 << NFTNL_EXPR_BITWISE_MASK)) {
		struct nlattr *nest;

		nest = mnl_attr_nest_start(nlh, NFTA_BITWISE_MASK);
		mnl_attr_put(nlh, NFTA_DATA_VALUE, bitwise->mask.len,
				bitwise->mask.val);
		mnl_attr_nest_end(nlh, nest);
	}
	if (e->flags & (1 << NFTNL_EXPR_BITWISE_XOR)) {
		struct nlattr *nest;

		nest = mnl_attr_nest_start(nlh, NFTA_BITWISE_XOR);
		mnl_attr_put(nlh, NFTA_DATA_VALUE, bitwise->xor.len,
				bitwise->xor.val);
		mnl_attr_nest_end(nlh, nest);
	}
}

static int
nftnl_expr_bitwise_parse(struct nftnl_expr *e, struct nlattr *attr)
{
	struct nftnl_expr_bitwise *bitwise = nftnl_expr_data(e);
	struct nlattr *tb[NFTA_BITWISE_MAX+1] = {};
	int ret = 0;

	if (mnl_attr_parse_nested(attr, nftnl_expr_bitwise_cb, tb) < 0)
		return -1;

	if (tb[NFTA_BITWISE_SREG]) {
		bitwise->sreg = ntohl(mnl_attr_get_u32(tb[NFTA_BITWISE_SREG]));
		e->flags |= (1 << NFTNL_EXPR_BITWISE_SREG);
	}
	if (tb[NFTA_BITWISE_DREG]) {
		bitwise->dreg = ntohl(mnl_attr_get_u32(tb[NFTA_BITWISE_DREG]));
		e->flags |= (1 << NFTNL_EXPR_BITWISE_DREG);
	}
	if (tb[NFTA_BITWISE_LEN]) {
		bitwise->len = ntohl(mnl_attr_get_u32(tb[NFTA_BITWISE_LEN]));
		e->flags |= (1 << NFTNL_EXPR_BITWISE_LEN);
	}
	if (tb[NFTA_BITWISE_MASK]) {
		ret = nftnl_parse_data(&bitwise->mask, tb[NFTA_BITWISE_MASK], NULL);
		e->flags |= (1 << NFTA_BITWISE_MASK);
	}
	if (tb[NFTA_BITWISE_XOR]) {
		ret = nftnl_parse_data(&bitwise->xor, tb[NFTA_BITWISE_XOR], NULL);
		e->flags |= (1 << NFTA_BITWISE_XOR);
	}

	return ret;
}

static int nftnl_expr_bitwise_snprintf_default(char *buf, size_t size,
					       const struct nftnl_expr *e)
{
	struct nftnl_expr_bitwise *bitwise = nftnl_expr_data(e);
	int remain = size, offset = 0, ret;

	ret = snprintf(buf, remain, "reg %u = (reg=%u & ",
		       bitwise->dreg, bitwise->sreg);
	SNPRINTF_BUFFER_SIZE(ret, remain, offset);

	ret = nftnl_data_reg_snprintf(buf + offset, remain, &bitwise->mask,
				    NFTNL_OUTPUT_DEFAULT, 0, DATA_VALUE);
	SNPRINTF_BUFFER_SIZE(ret, remain, offset);

	ret = snprintf(buf + offset, remain, ") ^ ");
	SNPRINTF_BUFFER_SIZE(ret, remain, offset);

	ret = nftnl_data_reg_snprintf(buf + offset, remain, &bitwise->xor,
				    NFTNL_OUTPUT_DEFAULT, 0, DATA_VALUE);
	SNPRINTF_BUFFER_SIZE(ret, remain, offset);

	return offset;
}

static int
nftnl_expr_bitwise_snprintf(char *buf, size_t size, uint32_t type,
			    uint32_t flags, const struct nftnl_expr *e)
{
	switch (type) {
	case NFTNL_OUTPUT_DEFAULT:
		return nftnl_expr_bitwise_snprintf_default(buf, size, e);
	default:
		break;
	}
	return -1;
}

struct expr_ops expr_ops_bitwise = {
	.name		= "bitwise",
	.alloc_len	= sizeof(struct nftnl_expr_bitwise),
	.max_attr	= NFTA_BITWISE_MAX,
	.set		= nftnl_expr_bitwise_set,
	.get		= nftnl_expr_bitwise_get,
	.parse		= nftnl_expr_bitwise_parse,
	.build		= nftnl_expr_bitwise_build,
	.snprintf	= nftnl_expr_bitwise_snprintf,
};
