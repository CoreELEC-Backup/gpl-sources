/*
 * (C) 2015 Pablo Neira Ayuso <pablo@netfilter.org>
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
#include "internal.h"
#include <libmnl/libmnl.h>
#include <linux/netfilter/nf_tables.h>
#include <libnftnl/expr.h>
#include <libnftnl/rule.h>
#include "expr_ops.h"
#include "data_reg.h"
#include <buffer.h>

struct nftnl_expr_dup {
	enum nft_registers	sreg_addr;
	enum nft_registers	sreg_dev;
};

static int nftnl_expr_dup_set(struct nftnl_expr *e, uint16_t type,
			      const void *data, uint32_t data_len)
{
	struct nftnl_expr_dup *dup = nftnl_expr_data(e);

	switch (type) {
	case NFTNL_EXPR_DUP_SREG_ADDR:
		memcpy(&dup->sreg_addr, data, sizeof(dup->sreg_addr));
		break;
	case NFTNL_EXPR_DUP_SREG_DEV:
		memcpy(&dup->sreg_dev, data, sizeof(dup->sreg_dev));
		break;
	default:
		return -1;
	}
	return 0;
}

static const void *nftnl_expr_dup_get(const struct nftnl_expr *e,
				      uint16_t type, uint32_t *data_len)
{
	struct nftnl_expr_dup *dup = nftnl_expr_data(e);

	switch (type) {
	case NFTNL_EXPR_DUP_SREG_ADDR:
		*data_len = sizeof(dup->sreg_addr);
		return &dup->sreg_addr;
	case NFTNL_EXPR_DUP_SREG_DEV:
		*data_len = sizeof(dup->sreg_dev);
		return &dup->sreg_dev;
	}
	return NULL;
}

static int nftnl_expr_dup_cb(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = data;
	int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, NFTA_DUP_MAX) < 0)
		return MNL_CB_OK;

	switch (type) {
	case NFTA_DUP_SREG_ADDR:
	case NFTA_DUP_SREG_DEV:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0)
			abi_breakage();
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

static void nftnl_expr_dup_build(struct nlmsghdr *nlh,
				 const struct nftnl_expr *e)
{
	struct nftnl_expr_dup *dup = nftnl_expr_data(e);

	if (e->flags & (1 << NFTNL_EXPR_DUP_SREG_ADDR))
		mnl_attr_put_u32(nlh, NFTA_DUP_SREG_ADDR, htonl(dup->sreg_addr));
	if (e->flags & (1 << NFTNL_EXPR_DUP_SREG_DEV))
		mnl_attr_put_u32(nlh, NFTA_DUP_SREG_DEV, htonl(dup->sreg_dev));
}

static int nftnl_expr_dup_parse(struct nftnl_expr *e, struct nlattr *attr)
{
	struct nftnl_expr_dup *dup = nftnl_expr_data(e);
	struct nlattr *tb[NFTA_DUP_MAX + 1] = {};
	int ret = 0;

	if (mnl_attr_parse_nested(attr, nftnl_expr_dup_cb, tb) < 0)
		return -1;

	if (tb[NFTA_DUP_SREG_ADDR]) {
		dup->sreg_addr = ntohl(mnl_attr_get_u32(tb[NFTA_DUP_SREG_ADDR]));
		e->flags |= (1 << NFTNL_EXPR_DUP_SREG_ADDR);
	}
	if (tb[NFTA_DUP_SREG_DEV]) {
		dup->sreg_dev = ntohl(mnl_attr_get_u32(tb[NFTA_DUP_SREG_DEV]));
		e->flags |= (1 << NFTNL_EXPR_DUP_SREG_DEV);
	}

	return ret;
}

static int nftnl_expr_dup_snprintf_default(char *buf, size_t len,
					   const struct nftnl_expr *e,
					   uint32_t flags)
{
	int remain = len, offset = 0, ret;
	struct nftnl_expr_dup *dup = nftnl_expr_data(e);

	if (e->flags & (1 << NFTNL_EXPR_DUP_SREG_ADDR)) {
		ret = snprintf(buf + offset, len, "sreg_addr %u ", dup->sreg_addr);
		SNPRINTF_BUFFER_SIZE(ret, remain, offset);
	}

	if (e->flags & (1 << NFTNL_EXPR_DUP_SREG_DEV)) {
		ret = snprintf(buf + offset, len, "sreg_dev %u ", dup->sreg_dev);
		SNPRINTF_BUFFER_SIZE(ret, remain, offset);
	}

	return offset;
}

static int nftnl_expr_dup_snprintf(char *buf, size_t len, uint32_t type,
				   uint32_t flags, const struct nftnl_expr *e)
{
	switch (type) {
	case NFTNL_OUTPUT_DEFAULT:
		return nftnl_expr_dup_snprintf_default(buf, len, e, flags);
	case NFTNL_OUTPUT_XML:
	case NFTNL_OUTPUT_JSON:
	default:
		break;
	}
	return -1;
}

struct expr_ops expr_ops_dup = {
	.name		= "dup",
	.alloc_len	= sizeof(struct nftnl_expr_dup),
	.max_attr	= NFTA_DUP_MAX,
	.set		= nftnl_expr_dup_set,
	.get		= nftnl_expr_dup_get,
	.parse		= nftnl_expr_dup_parse,
	.build		= nftnl_expr_dup_build,
	.snprintf	= nftnl_expr_dup_snprintf,
};
