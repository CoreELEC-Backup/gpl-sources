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

struct nftnl_expr_fwd {
	enum nft_registers	sreg_dev;
	enum nft_registers	sreg_addr;
	uint32_t		nfproto;
};

static int nftnl_expr_fwd_set(struct nftnl_expr *e, uint16_t type,
				  const void *data, uint32_t data_len)
{
	struct nftnl_expr_fwd *fwd = nftnl_expr_data(e);

	switch (type) {
	case NFTNL_EXPR_FWD_SREG_DEV:
		memcpy(&fwd->sreg_dev, data, sizeof(fwd->sreg_dev));
		break;
	case NFTNL_EXPR_FWD_SREG_ADDR:
		memcpy(&fwd->sreg_addr, data, sizeof(fwd->sreg_addr));
		break;
	case NFTNL_EXPR_FWD_NFPROTO:
		memcpy(&fwd->nfproto, data, sizeof(fwd->nfproto));
		break;
	default:
		return -1;
	}
	return 0;
}

static const void *nftnl_expr_fwd_get(const struct nftnl_expr *e,
				      uint16_t type, uint32_t *data_len)
{
	struct nftnl_expr_fwd *fwd = nftnl_expr_data(e);

	switch (type) {
	case NFTNL_EXPR_FWD_SREG_DEV:
		*data_len = sizeof(fwd->sreg_dev);
		return &fwd->sreg_dev;
	case NFTNL_EXPR_FWD_SREG_ADDR:
		*data_len = sizeof(fwd->sreg_addr);
		return &fwd->sreg_addr;
	case NFTNL_EXPR_FWD_NFPROTO:
		*data_len = sizeof(fwd->nfproto);
		return &fwd->nfproto;
	}
	return NULL;
}

static int nftnl_expr_fwd_cb(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = data;
	int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, NFTA_FWD_MAX) < 0)
		return MNL_CB_OK;

	switch (type) {
	case NFTA_FWD_SREG_DEV:
	case NFTA_FWD_SREG_ADDR:
	case NFTA_FWD_NFPROTO:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0)
			abi_breakage();
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

static void nftnl_expr_fwd_build(struct nlmsghdr *nlh,
				 const struct nftnl_expr *e)
{
	struct nftnl_expr_fwd *fwd = nftnl_expr_data(e);

	if (e->flags & (1 << NFTNL_EXPR_FWD_SREG_DEV))
		mnl_attr_put_u32(nlh, NFTA_FWD_SREG_DEV, htonl(fwd->sreg_dev));
	if (e->flags & (1 << NFTNL_EXPR_FWD_SREG_ADDR))
		mnl_attr_put_u32(nlh, NFTA_FWD_SREG_ADDR, htonl(fwd->sreg_addr));
	if (e->flags & (1 << NFTNL_EXPR_FWD_NFPROTO))
		mnl_attr_put_u32(nlh, NFTA_FWD_NFPROTO, htonl(fwd->nfproto));
}

static int nftnl_expr_fwd_parse(struct nftnl_expr *e, struct nlattr *attr)
{
	struct nftnl_expr_fwd *fwd = nftnl_expr_data(e);
	struct nlattr *tb[NFTA_FWD_MAX + 1] = {};
	int ret = 0;

	if (mnl_attr_parse_nested(attr, nftnl_expr_fwd_cb, tb) < 0)
		return -1;

	if (tb[NFTA_FWD_SREG_DEV]) {
		fwd->sreg_dev = ntohl(mnl_attr_get_u32(tb[NFTA_FWD_SREG_DEV]));
		e->flags |= (1 << NFTNL_EXPR_FWD_SREG_DEV);
	}
	if (tb[NFTA_FWD_SREG_ADDR]) {
		fwd->sreg_addr = ntohl(mnl_attr_get_u32(tb[NFTA_FWD_SREG_ADDR]));
		e->flags |= (1 << NFTNL_EXPR_FWD_SREG_ADDR);
	}
	if (tb[NFTA_FWD_NFPROTO]) {
		fwd->nfproto = ntohl(mnl_attr_get_u32(tb[NFTA_FWD_NFPROTO]));
		e->flags |= (1 << NFTNL_EXPR_FWD_NFPROTO);
	}

	return ret;
}

static int nftnl_expr_fwd_snprintf_default(char *buf, size_t len,
					   const struct nftnl_expr *e,
					   uint32_t flags)
{
	int remain = len, offset = 0, ret;
	struct nftnl_expr_fwd *fwd = nftnl_expr_data(e);

	if (e->flags & (1 << NFTNL_EXPR_FWD_SREG_DEV)) {
		ret = snprintf(buf + offset, remain, "sreg_dev %u ",
			       fwd->sreg_dev);
		SNPRINTF_BUFFER_SIZE(ret, remain, offset);
	}
	if (e->flags & (1 << NFTNL_EXPR_FWD_SREG_ADDR)) {
		ret = snprintf(buf + offset, remain, "sreg_addr %u ",
			       fwd->sreg_addr);
		SNPRINTF_BUFFER_SIZE(ret, remain, offset);
	}
	if (e->flags & (1 << NFTNL_EXPR_FWD_NFPROTO)) {
		ret = snprintf(buf + offset, remain, "nfproto %u ",
			       fwd->nfproto);
		SNPRINTF_BUFFER_SIZE(ret, remain, offset);
	}

	return offset;
}

static int nftnl_expr_fwd_snprintf(char *buf, size_t len, uint32_t type,
				   uint32_t flags, const struct nftnl_expr *e)
{
	switch (type) {
	case NFTNL_OUTPUT_DEFAULT:
		return nftnl_expr_fwd_snprintf_default(buf, len, e, flags);
	case NFTNL_OUTPUT_XML:
	case NFTNL_OUTPUT_JSON:
	default:
		break;
	}
	return -1;
}

struct expr_ops expr_ops_fwd = {
	.name		= "fwd",
	.alloc_len	= sizeof(struct nftnl_expr_fwd),
	.max_attr	= NFTA_FWD_MAX,
	.set		= nftnl_expr_fwd_set,
	.get		= nftnl_expr_fwd_get,
	.parse		= nftnl_expr_fwd_parse,
	.build		= nftnl_expr_fwd_build,
	.snprintf	= nftnl_expr_fwd_snprintf,
};
