/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <errno.h>
#include <linux/netfilter/nf_tables.h>
#include <linux/xfrm.h>

#include "internal.h"
#include <libmnl/libmnl.h>
#include <libnftnl/expr.h>
#include <libnftnl/rule.h>

struct nftnl_expr_xfrm {
	enum nft_registers	dreg;
	enum nft_xfrm_keys	key;
	uint32_t		spnum;
	uint8_t			dir;
};

static int
nftnl_expr_xfrm_set(struct nftnl_expr *e, uint16_t type,
		    const void *data, uint32_t data_len)
{
	struct nftnl_expr_xfrm *x = nftnl_expr_data(e);

	switch(type) {
	case NFTNL_EXPR_XFRM_KEY:
		memcpy(&x->key, data, sizeof(x->key));
		break;
	case NFTNL_EXPR_XFRM_DIR:
		memcpy(&x->dir, data, sizeof(x->dir));
		break;
	case NFTNL_EXPR_XFRM_SPNUM:
		memcpy(&x->spnum, data, sizeof(x->spnum));
		break;
	case NFTNL_EXPR_XFRM_DREG:
		memcpy(&x->dreg, data, sizeof(x->dreg));
		break;
	default:
		return -1;
	}
	return 0;
}

static const void *
nftnl_expr_xfrm_get(const struct nftnl_expr *e, uint16_t type,
		    uint32_t *data_len)
{
	struct nftnl_expr_xfrm *x = nftnl_expr_data(e);

	switch(type) {
	case NFTNL_EXPR_XFRM_KEY:
		*data_len = sizeof(x->key);
		return &x->key;
	case NFTNL_EXPR_XFRM_DIR:
		*data_len = sizeof(x->dir);
		return &x->dir;
	case NFTNL_EXPR_XFRM_SPNUM:
		*data_len = sizeof(x->spnum);
		return &x->spnum;
	case NFTNL_EXPR_XFRM_DREG:
		*data_len = sizeof(x->dreg);
		return &x->dreg;
	}
	return NULL;
}

static int nftnl_expr_xfrm_cb(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = data;
	int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, NFTA_XFRM_MAX) < 0)
		return MNL_CB_OK;

	switch (type) {
	case NFTA_XFRM_DREG:
	case NFTA_XFRM_KEY:
	case NFTA_XFRM_SPNUM:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0)
			abi_breakage();
		break;
	case NFTA_XFRM_DIR:
		if (mnl_attr_validate(attr, MNL_TYPE_U8) < 0)
			abi_breakage();
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

static void
nftnl_expr_xfrm_build(struct nlmsghdr *nlh, const struct nftnl_expr *e)
{
	struct nftnl_expr_xfrm *x = nftnl_expr_data(e);

	if (e->flags & (1 << NFTNL_EXPR_XFRM_KEY))
		mnl_attr_put_u32(nlh, NFTA_XFRM_KEY, htonl(x->key));
	if (e->flags & (1 << NFTNL_EXPR_XFRM_DIR))
		mnl_attr_put_u8(nlh, NFTA_XFRM_DIR, x->dir);
	if (e->flags & (1 << NFTNL_EXPR_XFRM_SPNUM))
		mnl_attr_put_u32(nlh, NFTA_XFRM_SPNUM, htonl(x->spnum));
	if (e->flags & (1 << NFTNL_EXPR_XFRM_DREG))
		mnl_attr_put_u32(nlh, NFTA_XFRM_DREG, htonl(x->dreg));
}

static int
nftnl_expr_xfrm_parse(struct nftnl_expr *e, struct nlattr *attr)
{
	struct nftnl_expr_xfrm *x = nftnl_expr_data(e);
	struct nlattr *tb[NFTA_XFRM_MAX+1] = {};

	if (mnl_attr_parse_nested(attr, nftnl_expr_xfrm_cb, tb) < 0)
		return -1;

	if (tb[NFTA_XFRM_KEY]) {
		x->key = ntohl(mnl_attr_get_u32(tb[NFTA_XFRM_KEY]));
		e->flags |= (1 << NFTNL_EXPR_XFRM_KEY);
	}
	if (tb[NFTA_XFRM_DIR]) {
		x->dir = mnl_attr_get_u8(tb[NFTA_XFRM_DIR]);
		e->flags |= (1 << NFTNL_EXPR_XFRM_DIR);
	}
	if (tb[NFTA_XFRM_SPNUM]) {
		x->spnum = ntohl(mnl_attr_get_u32(tb[NFTA_XFRM_SPNUM]));
		e->flags |= (1 << NFTNL_EXPR_XFRM_SPNUM);
	}
	if (tb[NFTA_XFRM_DREG]) {
		x->dreg = ntohl(mnl_attr_get_u32(tb[NFTA_XFRM_DREG]));
		e->flags |= (1 << NFTNL_EXPR_XFRM_DREG);
	}
	return 0;
}

static const char *xfrmkey2str_array[] = {
	[NFT_XFRM_KEY_DADDR_IP4]	= "daddr4",
	[NFT_XFRM_KEY_SADDR_IP4]	= "saddr4",
	[NFT_XFRM_KEY_DADDR_IP6]	= "daddr6",
	[NFT_XFRM_KEY_SADDR_IP6]	= "saddr6",
	[NFT_XFRM_KEY_REQID]		= "reqid",
	[NFT_XFRM_KEY_SPI]		= "spi",
};

static const char *xfrmkey2str(uint32_t key)
{
	if (key >= sizeof(xfrmkey2str_array) / sizeof(xfrmkey2str_array[0]))
		return "unknown";

	return xfrmkey2str_array[key];
}

static const char *xfrmdir2str_array[] = {
	[XFRM_POLICY_IN]	= "in",
	[XFRM_POLICY_OUT]	= "out",
};

static const char *xfrmdir2str(uint8_t dir)
{
	if (dir >= sizeof(xfrmdir2str_array) / sizeof(xfrmdir2str_array[0]))
		return "unknown";

	return xfrmdir2str_array[dir];
}

#ifdef JSON_PARSING
static uint32_t str2xfrmkey(const char *s)
{
	int i;

	for (i = 0;
	     i < sizeof(xfrmkey2str_array) / sizeof(xfrmkey2str_array[0]);
	     i++) {
		if (strcmp(xfrmkey2str_array[i], s) == 0)
			return i;
	}
	return -1;
}

static int str2xfmrdir(const char *s)
{
	int i;

	for (i = 0;
	     i <  sizeof(xfrmdir2str_array) / sizeof(xfrmdir2str_array[0]);
	     i++) {
		if (strcmp(xfrmkey2str_array[i], s) == 0)
			return i;
	}
	return -1;
}
#endif

static int
nftnl_expr_xfrm_snprintf_default(char *buf, size_t size,
			       const struct nftnl_expr *e)
{
	struct nftnl_expr_xfrm *x = nftnl_expr_data(e);
	int ret, remain = size, offset = 0;

	if (e->flags & (1 << NFTNL_EXPR_XFRM_DREG)) {
		ret = snprintf(buf, remain, "load %s %u %s => reg %u ",
				xfrmdir2str(x->dir),
				x->spnum,
			        xfrmkey2str(x->key), x->dreg);
		SNPRINTF_BUFFER_SIZE(ret, remain, offset);
	}
	return offset;
}

static int
nftnl_expr_xfrm_snprintf(char *buf, size_t len, uint32_t type,
			 uint32_t flags, const struct nftnl_expr *e)
{
	switch (type) {
	case NFTNL_OUTPUT_DEFAULT:
		return nftnl_expr_xfrm_snprintf_default(buf, len, e);
	case NFTNL_OUTPUT_XML:
	case NFTNL_OUTPUT_JSON:
	default:
		break;
	}
	return -1;
}

struct expr_ops expr_ops_xfrm = {
	.name		= "xfrm",
	.alloc_len	= sizeof(struct nftnl_expr_xfrm),
	.max_attr	= NFTA_XFRM_MAX,
	.set		= nftnl_expr_xfrm_set,
	.get		= nftnl_expr_xfrm_get,
	.parse		= nftnl_expr_xfrm_parse,
	.build		= nftnl_expr_xfrm_build,
	.snprintf	= nftnl_expr_xfrm_snprintf,
};
