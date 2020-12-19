/*
 * (C) 2012-2014 Pablo Neira Ayuso <pablo@netfilter.org>
 * (C) 2012 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Authors:
 * 	Tomasz Bursztyka <tomasz.bursztyka@linux.intel.com>
 */

#include "internal.h"

#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <libmnl/libmnl.h>
#include <linux/netfilter/nf_tables.h>
#include <libnftnl/expr.h>
#include <libnftnl/rule.h>

struct nftnl_expr_nat {
	enum nft_registers sreg_addr_min;
	enum nft_registers sreg_addr_max;
	enum nft_registers sreg_proto_min;
	enum nft_registers sreg_proto_max;
	int                family;
	enum nft_nat_types type;
	uint32_t	   flags;
};

static int
nftnl_expr_nat_set(struct nftnl_expr *e, uint16_t type,
		      const void *data, uint32_t data_len)
{
	struct nftnl_expr_nat *nat = nftnl_expr_data(e);

	switch(type) {
	case NFTNL_EXPR_NAT_TYPE:
		memcpy(&nat->type, data, sizeof(nat->type));
		break;
	case NFTNL_EXPR_NAT_FAMILY:
		memcpy(&nat->family, data, sizeof(nat->family));
		break;
	case NFTNL_EXPR_NAT_REG_ADDR_MIN:
		memcpy(&nat->sreg_addr_min, data, sizeof(nat->sreg_addr_min));
		break;
	case NFTNL_EXPR_NAT_REG_ADDR_MAX:
		memcpy(&nat->sreg_addr_max, data, sizeof(nat->sreg_addr_max));
		break;
	case NFTNL_EXPR_NAT_REG_PROTO_MIN:
		memcpy(&nat->sreg_proto_min, data, sizeof(nat->sreg_proto_min));
		break;
	case NFTNL_EXPR_NAT_REG_PROTO_MAX:
		memcpy(&nat->sreg_proto_max, data, sizeof(nat->sreg_proto_max));
		break;
	case NFTNL_EXPR_NAT_FLAGS:
		memcpy(&nat->flags, data, sizeof(nat->flags));
		break;
	default:
		return -1;
	}

	return 0;
}

static const void *
nftnl_expr_nat_get(const struct nftnl_expr *e, uint16_t type,
		      uint32_t *data_len)
{
	struct nftnl_expr_nat *nat = nftnl_expr_data(e);

	switch(type) {
	case NFTNL_EXPR_NAT_TYPE:
		*data_len = sizeof(nat->type);
		return &nat->type;
	case NFTNL_EXPR_NAT_FAMILY:
		*data_len = sizeof(nat->family);
		return &nat->family;
	case NFTNL_EXPR_NAT_REG_ADDR_MIN:
		*data_len = sizeof(nat->sreg_addr_min);
		return &nat->sreg_addr_min;
	case NFTNL_EXPR_NAT_REG_ADDR_MAX:
		*data_len = sizeof(nat->sreg_addr_max);
		return &nat->sreg_addr_max;
	case NFTNL_EXPR_NAT_REG_PROTO_MIN:
		*data_len = sizeof(nat->sreg_proto_min);
		return &nat->sreg_proto_min;
	case NFTNL_EXPR_NAT_REG_PROTO_MAX:
		*data_len = sizeof(nat->sreg_proto_max);
		return &nat->sreg_proto_max;
	case NFTNL_EXPR_NAT_FLAGS:
		*data_len = sizeof(nat->flags);
		return &nat->flags;
	}
	return NULL;
}

static int nftnl_expr_nat_cb(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = data;
	int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, NFTA_NAT_MAX) < 0)
		return MNL_CB_OK;

	switch(type) {
	case NFTA_NAT_TYPE:
	case NFTA_NAT_FAMILY:
	case NFTA_NAT_REG_ADDR_MIN:
	case NFTA_NAT_REG_ADDR_MAX:
	case NFTA_NAT_REG_PROTO_MIN:
	case NFTA_NAT_REG_PROTO_MAX:
	case NFTA_NAT_FLAGS:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0)
			abi_breakage();
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

static int
nftnl_expr_nat_parse(struct nftnl_expr *e, struct nlattr *attr)
{
	struct nftnl_expr_nat *nat = nftnl_expr_data(e);
	struct nlattr *tb[NFTA_NAT_MAX+1] = {};

	if (mnl_attr_parse_nested(attr, nftnl_expr_nat_cb, tb) < 0)
		return -1;

	if (tb[NFTA_NAT_TYPE]) {
		nat->type = ntohl(mnl_attr_get_u32(tb[NFTA_NAT_TYPE]));
		e->flags |= (1 << NFTNL_EXPR_NAT_TYPE);
	}
	if (tb[NFTA_NAT_FAMILY]) {
		nat->family = ntohl(mnl_attr_get_u32(tb[NFTA_NAT_FAMILY]));
		e->flags |= (1 << NFTNL_EXPR_NAT_FAMILY);
	}
	if (tb[NFTA_NAT_REG_ADDR_MIN]) {
		nat->sreg_addr_min =
			ntohl(mnl_attr_get_u32(tb[NFTA_NAT_REG_ADDR_MIN]));
		e->flags |= (1 << NFTNL_EXPR_NAT_REG_ADDR_MIN);
	}
	if (tb[NFTA_NAT_REG_ADDR_MAX]) {
		nat->sreg_addr_max =
			ntohl(mnl_attr_get_u32(tb[NFTA_NAT_REG_ADDR_MAX]));
		e->flags |= (1 << NFTNL_EXPR_NAT_REG_ADDR_MAX);
	}
	if (tb[NFTA_NAT_REG_PROTO_MIN]) {
		nat->sreg_proto_min =
			ntohl(mnl_attr_get_u32(tb[NFTA_NAT_REG_PROTO_MIN]));
		e->flags |= (1 << NFTNL_EXPR_NAT_REG_PROTO_MIN);
	}
	if (tb[NFTA_NAT_REG_PROTO_MAX]) {
		nat->sreg_proto_max =
			ntohl(mnl_attr_get_u32(tb[NFTA_NAT_REG_PROTO_MAX]));
		e->flags |= (1 << NFTNL_EXPR_NAT_REG_PROTO_MAX);
	}
	if (tb[NFTA_NAT_FLAGS]) {
		nat->flags = ntohl(mnl_attr_get_u32(tb[NFTA_NAT_FLAGS]));
		e->flags |= (1 << NFTNL_EXPR_NAT_FLAGS);
	}

	return 0;
}

static void
nftnl_expr_nat_build(struct nlmsghdr *nlh, const struct nftnl_expr *e)
{
	struct nftnl_expr_nat *nat = nftnl_expr_data(e);

	if (e->flags & (1 << NFTNL_EXPR_NAT_TYPE))
		mnl_attr_put_u32(nlh, NFTA_NAT_TYPE, htonl(nat->type));
	if (e->flags & (1 << NFTNL_EXPR_NAT_FAMILY))
		mnl_attr_put_u32(nlh, NFTA_NAT_FAMILY, htonl(nat->family));
	if (e->flags & (1 << NFTNL_EXPR_NAT_REG_ADDR_MIN))
		mnl_attr_put_u32(nlh, NFTA_NAT_REG_ADDR_MIN,
				 htonl(nat->sreg_addr_min));
	if (e->flags & (1 << NFTNL_EXPR_NAT_REG_ADDR_MAX))
		mnl_attr_put_u32(nlh, NFTA_NAT_REG_ADDR_MAX,
				 htonl(nat->sreg_addr_max));
	if (e->flags & (1 << NFTNL_EXPR_NAT_REG_PROTO_MIN))
		mnl_attr_put_u32(nlh, NFTA_NAT_REG_PROTO_MIN,
				 htonl(nat->sreg_proto_min));
	if (e->flags & (1 << NFTNL_EXPR_NAT_REG_PROTO_MAX))
		mnl_attr_put_u32(nlh, NFTA_NAT_REG_PROTO_MAX,
				 htonl(nat->sreg_proto_max));
	if (e->flags & (1 << NFTNL_EXPR_NAT_FLAGS))
		mnl_attr_put_u32(nlh, NFTA_NAT_FLAGS, htonl(nat->flags));
}

static inline const char *nat2str(uint16_t nat)
{
	switch (nat) {
	case NFT_NAT_SNAT:
		return "snat";
	case NFT_NAT_DNAT:
		return "dnat";
	default:
		return "unknown";
	}
}

static inline int nftnl_str2nat(const char *nat)
{
	if (strcmp(nat, "snat") == 0)
		return NFT_NAT_SNAT;
	else if (strcmp(nat, "dnat") == 0)
		return NFT_NAT_DNAT;
	else {
		errno = EINVAL;
		return -1;
	}
}

static int
nftnl_expr_nat_snprintf_default(char *buf, size_t size,
				const struct nftnl_expr *e)
{
	struct nftnl_expr_nat *nat = nftnl_expr_data(e);
	int remain = size, offset = 0, ret = 0;

	ret = snprintf(buf, remain, "%s ", nat2str(nat->type));
	SNPRINTF_BUFFER_SIZE(ret, remain, offset);

	ret = snprintf(buf + offset, remain, "%s ",
		       nftnl_family2str(nat->family));
	SNPRINTF_BUFFER_SIZE(ret, remain, offset);

	if (e->flags & (1 << NFTNL_EXPR_NAT_REG_ADDR_MIN)) {
		ret = snprintf(buf + offset, remain,
			       "addr_min reg %u addr_max reg %u ",
			       nat->sreg_addr_min, nat->sreg_addr_max);
		SNPRINTF_BUFFER_SIZE(ret, remain, offset);
	}

	if (e->flags & (1 << NFTNL_EXPR_NAT_REG_PROTO_MIN)) {
		ret = snprintf(buf + offset, remain,
			       "proto_min reg %u proto_max reg %u ",
			       nat->sreg_proto_min, nat->sreg_proto_max);
		SNPRINTF_BUFFER_SIZE(ret, remain, offset);
	}

	if (e->flags & (1 << NFTNL_EXPR_NAT_FLAGS)) {
		ret = snprintf(buf + offset, remain, "flags %u", nat->flags);
		SNPRINTF_BUFFER_SIZE(ret, remain, offset);
	}

	return offset;
}

static int
nftnl_expr_nat_snprintf(char *buf, size_t size, uint32_t type,
			uint32_t flags, const struct nftnl_expr *e)
{
	switch (type) {
	case NFTNL_OUTPUT_DEFAULT:
		return nftnl_expr_nat_snprintf_default(buf, size, e);
	case NFTNL_OUTPUT_XML:
	case NFTNL_OUTPUT_JSON:
	default:
		break;
	}
	return -1;
}

struct expr_ops expr_ops_nat = {
	.name		= "nat",
	.alloc_len	= sizeof(struct nftnl_expr_nat),
	.max_attr	= NFTA_NAT_MAX,
	.set		= nftnl_expr_nat_set,
	.get		= nftnl_expr_nat_get,
	.parse		= nftnl_expr_nat_parse,
	.build		= nftnl_expr_nat_build,
	.snprintf	= nftnl_expr_nat_snprintf,
};
