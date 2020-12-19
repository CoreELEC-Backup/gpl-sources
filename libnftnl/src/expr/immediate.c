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
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include "internal.h"
#include <libmnl/libmnl.h>
#include <linux/netfilter/nf_tables.h>
#include <libnftnl/expr.h>
#include <libnftnl/rule.h>

struct nftnl_expr_immediate {
	union nftnl_data_reg	data;
	enum nft_registers	dreg;
};

static int
nftnl_expr_immediate_set(struct nftnl_expr *e, uint16_t type,
			    const void *data, uint32_t data_len)
{
	struct nftnl_expr_immediate *imm = nftnl_expr_data(e);

	switch(type) {
	case NFTNL_EXPR_IMM_DREG:
		memcpy(&imm->dreg, data, sizeof(imm->dreg));
		break;
	case NFTNL_EXPR_IMM_DATA:
		memcpy(&imm->data.val, data, data_len);
		imm->data.len = data_len;
		break;
	case NFTNL_EXPR_IMM_VERDICT:
		memcpy(&imm->data.verdict, data, sizeof(imm->data.verdict));
		break;
	case NFTNL_EXPR_IMM_CHAIN:
		if (e->flags & (1 << NFTNL_EXPR_IMM_CHAIN))
			xfree(imm->data.chain);

		imm->data.chain = strdup(data);
		if (!imm->data.chain)
			return -1;
		break;
	default:
		return -1;
	}
	return 0;
}

static const void *
nftnl_expr_immediate_get(const struct nftnl_expr *e, uint16_t type,
			    uint32_t *data_len)
{
	struct nftnl_expr_immediate *imm = nftnl_expr_data(e);

	switch(type) {
	case NFTNL_EXPR_IMM_DREG:
		*data_len = sizeof(imm->dreg);
		return &imm->dreg;
	case NFTNL_EXPR_IMM_DATA:
		*data_len = imm->data.len;
		return &imm->data.val;
	case NFTNL_EXPR_IMM_VERDICT:
		*data_len = sizeof(imm->data.verdict);
		return &imm->data.verdict;
	case NFTNL_EXPR_IMM_CHAIN:
		*data_len = strlen(imm->data.chain)+1;
		return imm->data.chain;
	}
	return NULL;
}

static int nftnl_expr_immediate_cb(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = data;
	int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, NFTA_IMMEDIATE_MAX) < 0)
		return MNL_CB_OK;

	switch(type) {
	case NFTA_IMMEDIATE_DREG:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0)
			abi_breakage();
		break;
	case NFTA_IMMEDIATE_DATA:
		if (mnl_attr_validate(attr, MNL_TYPE_BINARY) < 0)
			abi_breakage();
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

static void
nftnl_expr_immediate_build(struct nlmsghdr *nlh, const struct nftnl_expr *e)
{
	struct nftnl_expr_immediate *imm = nftnl_expr_data(e);

	if (e->flags & (1 << NFTNL_EXPR_IMM_DREG))
		mnl_attr_put_u32(nlh, NFTA_IMMEDIATE_DREG, htonl(imm->dreg));

	/* Sane configurations allows you to set ONLY one of these two below */
	if (e->flags & (1 << NFTNL_EXPR_IMM_DATA)) {
		struct nlattr *nest;

		nest = mnl_attr_nest_start(nlh, NFTA_IMMEDIATE_DATA);
		mnl_attr_put(nlh, NFTA_DATA_VALUE, imm->data.len, imm->data.val);
		mnl_attr_nest_end(nlh, nest);

	} else if (e->flags & (1 << NFTNL_EXPR_IMM_VERDICT)) {
		struct nlattr *nest1, *nest2;

		nest1 = mnl_attr_nest_start(nlh, NFTA_IMMEDIATE_DATA);
		nest2 = mnl_attr_nest_start(nlh, NFTA_DATA_VERDICT);
		mnl_attr_put_u32(nlh, NFTA_VERDICT_CODE, htonl(imm->data.verdict));
		if (e->flags & (1 << NFTNL_EXPR_IMM_CHAIN))
			mnl_attr_put_strz(nlh, NFTA_VERDICT_CHAIN, imm->data.chain);

		mnl_attr_nest_end(nlh, nest1);
		mnl_attr_nest_end(nlh, nest2);
	}
}

static int
nftnl_expr_immediate_parse(struct nftnl_expr *e, struct nlattr *attr)
{
	struct nftnl_expr_immediate *imm = nftnl_expr_data(e);
	struct nlattr *tb[NFTA_IMMEDIATE_MAX+1] = {};
	int ret = 0;

	if (mnl_attr_parse_nested(attr, nftnl_expr_immediate_cb, tb) < 0)
		return -1;

	if (tb[NFTA_IMMEDIATE_DREG]) {
		imm->dreg = ntohl(mnl_attr_get_u32(tb[NFTA_IMMEDIATE_DREG]));
		e->flags |= (1 << NFTNL_EXPR_IMM_DREG);
	}
	if (tb[NFTA_IMMEDIATE_DATA]) {
		int type;

		ret = nftnl_parse_data(&imm->data, tb[NFTA_IMMEDIATE_DATA], &type);
		if (ret < 0)
			return ret;

		switch(type) {
		case DATA_VALUE:
			/* real immediate data to be loaded to destination */
			e->flags |= (1 << NFTNL_EXPR_IMM_DATA);
			break;
		case DATA_VERDICT:
			/* NF_ACCEPT, NF_DROP, NF_QUEUE and NFTNL_RETURN case */
			e->flags |= (1 << NFTNL_EXPR_IMM_VERDICT);
			break;
		case DATA_CHAIN:
			/* NFTNL_GOTO and NFTNL_JUMP case */
			e->flags |= (1 << NFTNL_EXPR_IMM_VERDICT) |
				    (1 << NFTNL_EXPR_IMM_CHAIN);
			break;
		}
	}

	return ret;
}

static int
nftnl_expr_immediate_snprintf_default(char *buf, size_t len,
				      const struct nftnl_expr *e,
				      uint32_t flags)
{
	int remain = len, offset = 0, ret;
	struct nftnl_expr_immediate *imm = nftnl_expr_data(e);

	ret = snprintf(buf, remain, "reg %u ", imm->dreg);
	SNPRINTF_BUFFER_SIZE(ret, remain, offset);

	if (e->flags & (1 << NFTNL_EXPR_IMM_DATA)) {
		ret = nftnl_data_reg_snprintf(buf + offset, remain, &imm->data,
					NFTNL_OUTPUT_DEFAULT, flags, DATA_VALUE);
		SNPRINTF_BUFFER_SIZE(ret, remain, offset);

	} else if (e->flags & (1 << NFTNL_EXPR_IMM_VERDICT)) {
		ret = nftnl_data_reg_snprintf(buf + offset, remain, &imm->data,
				NFTNL_OUTPUT_DEFAULT, flags, DATA_VERDICT);
		SNPRINTF_BUFFER_SIZE(ret, remain, offset);

	} else if (e->flags & (1 << NFTNL_EXPR_IMM_CHAIN)) {
		ret = nftnl_data_reg_snprintf(buf + offset, remain, &imm->data,
					NFTNL_OUTPUT_DEFAULT, flags, DATA_CHAIN);
		SNPRINTF_BUFFER_SIZE(ret, remain, offset);
	}

	return offset;
}

static int
nftnl_expr_immediate_snprintf(char *buf, size_t len, uint32_t type,
			      uint32_t flags, const struct nftnl_expr *e)
{
	switch(type) {
	case NFTNL_OUTPUT_DEFAULT:
		return nftnl_expr_immediate_snprintf_default(buf, len, e, flags);
	case NFTNL_OUTPUT_XML:
	case NFTNL_OUTPUT_JSON:
	default:
		break;
	}
	return -1;
}

static void nftnl_expr_immediate_free(const struct nftnl_expr *e)
{
	struct nftnl_expr_immediate *imm = nftnl_expr_data(e);

	if (e->flags & (1 << NFTNL_EXPR_IMM_VERDICT))
		nftnl_free_verdict(&imm->data);
}

struct expr_ops expr_ops_immediate = {
	.name		= "immediate",
	.alloc_len	= sizeof(struct nftnl_expr_immediate),
	.max_attr	= NFTA_IMMEDIATE_MAX,
	.free		= nftnl_expr_immediate_free,
	.set		= nftnl_expr_immediate_set,
	.get		= nftnl_expr_immediate_get,
	.parse		= nftnl_expr_immediate_parse,
	.build		= nftnl_expr_immediate_build,
	.snprintf	= nftnl_expr_immediate_snprintf,
};
