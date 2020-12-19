/*
 * (C) 2012-2013 by Pablo Neira Ayuso <pablo@netfilter.org>
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
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <arpa/inet.h>
#include <errno.h>
#include <libmnl/libmnl.h>

#include <linux/netfilter/nf_tables.h>

#include <libnftnl/expr.h>
#include <libnftnl/rule.h>

#ifndef IPPROTO_MH
#define IPPROTO_MH 135
#endif

struct nftnl_expr_exthdr {
	enum nft_registers	dreg;
	enum nft_registers	sreg;
	uint32_t		offset;
	uint32_t		len;
	uint8_t			type;
	uint32_t		op;
	uint32_t		flags;
};

static int
nftnl_expr_exthdr_set(struct nftnl_expr *e, uint16_t type,
			  const void *data, uint32_t data_len)
{
	struct nftnl_expr_exthdr *exthdr = nftnl_expr_data(e);

	switch(type) {
	case NFTNL_EXPR_EXTHDR_DREG:
		memcpy(&exthdr->dreg, data, sizeof(exthdr->dreg));
		break;
	case NFTNL_EXPR_EXTHDR_TYPE:
		memcpy(&exthdr->type, data, sizeof(exthdr->type));
		break;
	case NFTNL_EXPR_EXTHDR_OFFSET:
		memcpy(&exthdr->offset, data, sizeof(exthdr->offset));
		break;
	case NFTNL_EXPR_EXTHDR_LEN:
		memcpy(&exthdr->len, data, sizeof(exthdr->len));
		break;
	case NFTNL_EXPR_EXTHDR_OP:
		memcpy(&exthdr->op, data, sizeof(exthdr->op));
		break;
	case NFTNL_EXPR_EXTHDR_FLAGS:
		memcpy(&exthdr->flags, data, sizeof(exthdr->flags));
		break;
	case NFTNL_EXPR_EXTHDR_SREG:
		memcpy(&exthdr->sreg, data, sizeof(exthdr->sreg));
		break;
	default:
		return -1;
	}
	return 0;
}

static const void *
nftnl_expr_exthdr_get(const struct nftnl_expr *e, uint16_t type,
			 uint32_t *data_len)
{
	struct nftnl_expr_exthdr *exthdr = nftnl_expr_data(e);

	switch(type) {
	case NFTNL_EXPR_EXTHDR_DREG:
		*data_len = sizeof(exthdr->dreg);
		return &exthdr->dreg;
	case NFTNL_EXPR_EXTHDR_TYPE:
		*data_len = sizeof(exthdr->type);
		return &exthdr->type;
	case NFTNL_EXPR_EXTHDR_OFFSET:
		*data_len = sizeof(exthdr->offset);
		return &exthdr->offset;
	case NFTNL_EXPR_EXTHDR_LEN:
		*data_len = sizeof(exthdr->len);
		return &exthdr->len;
	case NFTNL_EXPR_EXTHDR_OP:
		*data_len = sizeof(exthdr->op);
		return &exthdr->op;
	case NFTNL_EXPR_EXTHDR_FLAGS:
		*data_len = sizeof(exthdr->flags);
		return &exthdr->flags;
	case NFTNL_EXPR_EXTHDR_SREG:
		*data_len = sizeof(exthdr->sreg);
		return &exthdr->sreg;
	}
	return NULL;
}

static int nftnl_expr_exthdr_cb(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = data;
	int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, NFTA_EXTHDR_MAX) < 0)
		return MNL_CB_OK;

	switch(type) {
	case NFTA_EXTHDR_TYPE:
		if (mnl_attr_validate(attr, MNL_TYPE_U8) < 0)
			abi_breakage();
		break;
	case NFTA_EXTHDR_DREG:
	case NFTA_EXTHDR_SREG:
	case NFTA_EXTHDR_OFFSET:
	case NFTA_EXTHDR_LEN:
	case NFTA_EXTHDR_OP:
	case NFTA_EXTHDR_FLAGS:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0)
			abi_breakage();
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

static void
nftnl_expr_exthdr_build(struct nlmsghdr *nlh, const struct nftnl_expr *e)
{
	struct nftnl_expr_exthdr *exthdr = nftnl_expr_data(e);

	if (e->flags & (1 << NFTNL_EXPR_EXTHDR_DREG))
		mnl_attr_put_u32(nlh, NFTA_EXTHDR_DREG, htonl(exthdr->dreg));
	if (e->flags & (1 << NFTNL_EXPR_EXTHDR_SREG))
		mnl_attr_put_u32(nlh, NFTA_EXTHDR_SREG, htonl(exthdr->sreg));
	if (e->flags & (1 << NFTNL_EXPR_EXTHDR_TYPE))
		mnl_attr_put_u8(nlh, NFTA_EXTHDR_TYPE, exthdr->type);
	if (e->flags & (1 << NFTNL_EXPR_EXTHDR_OFFSET))
		mnl_attr_put_u32(nlh, NFTA_EXTHDR_OFFSET, htonl(exthdr->offset));
	if (e->flags & (1 << NFTNL_EXPR_EXTHDR_LEN))
		mnl_attr_put_u32(nlh, NFTA_EXTHDR_LEN, htonl(exthdr->len));
	if (e->flags & (1 << NFTNL_EXPR_EXTHDR_OP))
		mnl_attr_put_u32(nlh, NFTA_EXTHDR_OP, htonl(exthdr->op));
	if (e->flags & (1 << NFTNL_EXPR_EXTHDR_FLAGS))
		mnl_attr_put_u32(nlh, NFTA_EXTHDR_FLAGS, htonl(exthdr->flags));
}

static int
nftnl_expr_exthdr_parse(struct nftnl_expr *e, struct nlattr *attr)
{
	struct nftnl_expr_exthdr *exthdr = nftnl_expr_data(e);
	struct nlattr *tb[NFTA_EXTHDR_MAX+1] = {};

	if (mnl_attr_parse_nested(attr, nftnl_expr_exthdr_cb, tb) < 0)
		return -1;

	if (tb[NFTA_EXTHDR_DREG]) {
		exthdr->dreg = ntohl(mnl_attr_get_u32(tb[NFTA_EXTHDR_DREG]));
		e->flags |= (1 << NFTNL_EXPR_EXTHDR_DREG);
	}
	if (tb[NFTA_EXTHDR_SREG]) {
		exthdr->sreg = ntohl(mnl_attr_get_u32(tb[NFTA_EXTHDR_SREG]));
		e->flags |= (1 << NFTNL_EXPR_EXTHDR_SREG);
	}
	if (tb[NFTA_EXTHDR_TYPE]) {
		exthdr->type = mnl_attr_get_u8(tb[NFTA_EXTHDR_TYPE]);
		e->flags |= (1 << NFTNL_EXPR_EXTHDR_TYPE);
	}
	if (tb[NFTA_EXTHDR_OFFSET]) {
		exthdr->offset = ntohl(mnl_attr_get_u32(tb[NFTA_EXTHDR_OFFSET]));
		e->flags |= (1 << NFTNL_EXPR_EXTHDR_OFFSET);
	}
	if (tb[NFTA_EXTHDR_LEN]) {
		exthdr->len = ntohl(mnl_attr_get_u32(tb[NFTA_EXTHDR_LEN]));
		e->flags |= (1 << NFTNL_EXPR_EXTHDR_LEN);
	}
	if (tb[NFTA_EXTHDR_OP]) {
		exthdr->op = ntohl(mnl_attr_get_u32(tb[NFTA_EXTHDR_OP]));
		e->flags |= (1 << NFTNL_EXPR_EXTHDR_OP);
	}
	if (tb[NFTA_EXTHDR_FLAGS]) {
		exthdr->flags = ntohl(mnl_attr_get_u32(tb[NFTA_EXTHDR_FLAGS]));
		e->flags |= (1 << NFTNL_EXPR_EXTHDR_FLAGS);
	}

	return 0;
}

static const char *op2str(uint8_t op)
{
	switch (op) {
	case NFT_EXTHDR_OP_TCPOPT:
		return " tcpopt";
	case NFT_EXTHDR_OP_IPV6:
		return " ipv6";
	case NFT_EXTHDR_OP_IPV4:
		return " ipv4";
	default:
		return "";
	}
}

static inline int str2exthdr_op(const char* str)
{
	if (!strcmp(str, "tcpopt"))
		return NFT_EXTHDR_OP_TCPOPT;
	if (!strcmp(str, "ipv4"))
		return NFT_EXTHDR_OP_IPV4;

	/* if str == "ipv6" or anything else */
	return NFT_EXTHDR_OP_IPV6;
}

static inline int str2exthdr_type(const char *str)
{
	if (strcmp(str, "hopopts") == 0)
		return IPPROTO_HOPOPTS;
	else if (strcmp(str, "routing") == 0)
		return IPPROTO_ROUTING;
	else if (strcmp(str, "fragment") == 0)
		return IPPROTO_FRAGMENT;
	else if (strcmp(str, "dstopts") == 0)
		return IPPROTO_DSTOPTS;
	else if (strcmp(str, "mh") == 0)
		return IPPROTO_MH;

	return -1;
}

static int nftnl_expr_exthdr_snprintf_default(char *buf, size_t len,
					      const struct nftnl_expr *e)
{
	struct nftnl_expr_exthdr *exthdr = nftnl_expr_data(e);

	if (e->flags & (1 << NFTNL_EXPR_EXTHDR_DREG))
		return snprintf(buf, len, "load%s %ub @ %u + %u%s => reg %u ",
				op2str(exthdr->op), exthdr->len, exthdr->type,
				exthdr->offset,
				exthdr->flags & NFT_EXTHDR_F_PRESENT ? " present" : "",
				exthdr->dreg);
	else
		return snprintf(buf, len, "write%s reg %u => %ub @ %u + %u ",
				op2str(exthdr->op), exthdr->sreg, exthdr->len, exthdr->type,
				exthdr->offset);

}

static int
nftnl_expr_exthdr_snprintf(char *buf, size_t len, uint32_t type,
			   uint32_t flags, const struct nftnl_expr *e)
{
	switch (type) {
	case NFTNL_OUTPUT_DEFAULT:
		return nftnl_expr_exthdr_snprintf_default(buf, len, e);
	case NFTNL_OUTPUT_XML:
	case NFTNL_OUTPUT_JSON:
	default:
		break;
	}
	return -1;
}

struct expr_ops expr_ops_exthdr = {
	.name		= "exthdr",
	.alloc_len	= sizeof(struct nftnl_expr_exthdr),
	.max_attr	= NFTA_EXTHDR_MAX,
	.set		= nftnl_expr_exthdr_set,
	.get		= nftnl_expr_exthdr_get,
	.parse		= nftnl_expr_exthdr_parse,
	.build		= nftnl_expr_exthdr_build,
	.snprintf	= nftnl_expr_exthdr_snprintf,
};
