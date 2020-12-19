/*
 * Copyright (c) 2017 Pablo M. Bermudo Garay <pablombg@gmail.com>
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
#include <libnftnl/object.h>

#include "obj.h"

static int nftnl_obj_limit_set(struct nftnl_obj *e, uint16_t type,
			       const void *data, uint32_t data_len)
{
	struct nftnl_obj_limit *limit = nftnl_obj_data(e);

	switch (type) {
	case NFTNL_OBJ_LIMIT_RATE:
		memcpy(&limit->rate, data, sizeof(limit->rate));
		break;
	case NFTNL_OBJ_LIMIT_UNIT:
		memcpy(&limit->unit, data, sizeof(limit->unit));
		break;
	case NFTNL_OBJ_LIMIT_BURST:
		memcpy(&limit->burst, data, sizeof(limit->burst));
		break;
	case NFTNL_OBJ_LIMIT_TYPE:
		memcpy(&limit->type, data, sizeof(limit->type));
		break;
	case NFTNL_OBJ_LIMIT_FLAGS:
		memcpy(&limit->flags, data, sizeof(limit->flags));
		break;
	default:
		return -1;
	}
	return 0;
}

static const void *nftnl_obj_limit_get(const struct nftnl_obj *e,
				       uint16_t type, uint32_t *data_len)
{
	struct nftnl_obj_limit *limit = nftnl_obj_data(e);

	switch (type) {
	case NFTNL_OBJ_LIMIT_RATE:
		*data_len = sizeof(limit->rate);
		return &limit->rate;
	case NFTNL_OBJ_LIMIT_UNIT:
		*data_len = sizeof(limit->unit);
		return &limit->unit;
	case NFTNL_OBJ_LIMIT_BURST:
		*data_len = sizeof(limit->burst);
		return &limit->burst;
	case NFTNL_OBJ_LIMIT_TYPE:
		*data_len = sizeof(limit->type);
		return &limit->type;
	case NFTNL_OBJ_LIMIT_FLAGS:
		*data_len = sizeof(limit->flags);
		return &limit->flags;
	}
	return NULL;
}

static int nftnl_obj_limit_cb(const struct nlattr *attr, void *data)
{
	int type = mnl_attr_get_type(attr);
	const struct nlattr **tb = data;

	if (mnl_attr_type_valid(attr, NFTA_LIMIT_MAX) < 0)
		return MNL_CB_OK;

	switch (type) {
	case NFTA_LIMIT_RATE:
	case NFTA_LIMIT_UNIT:
		if (mnl_attr_validate(attr, MNL_TYPE_U64) < 0)
			abi_breakage();
		break;
	case NFTA_LIMIT_BURST:
	case NFTA_LIMIT_TYPE:
	case NFTA_LIMIT_FLAGS:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0)
			abi_breakage();
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

static void nftnl_obj_limit_build(struct nlmsghdr *nlh,
				  const struct nftnl_obj *e)
{
	struct nftnl_obj_limit *limit = nftnl_obj_data(e);

	if (e->flags & (1 << NFTNL_OBJ_LIMIT_RATE))
		mnl_attr_put_u64(nlh, NFTA_LIMIT_RATE, htobe64(limit->rate));
	if (e->flags & (1 << NFTNL_OBJ_LIMIT_UNIT))
		mnl_attr_put_u64(nlh, NFTA_LIMIT_UNIT, htobe64(limit->unit));
	if (e->flags & (1 << NFTNL_OBJ_LIMIT_BURST))
		mnl_attr_put_u32(nlh, NFTA_LIMIT_BURST, htonl(limit->burst));
	if (e->flags & (1 << NFTNL_OBJ_LIMIT_TYPE))
		mnl_attr_put_u32(nlh, NFTA_LIMIT_TYPE, htonl(limit->type));
	if (e->flags & (1 << NFTNL_OBJ_LIMIT_FLAGS))
		mnl_attr_put_u32(nlh, NFTA_LIMIT_FLAGS, htonl(limit->flags));
}

static int nftnl_obj_limit_parse(struct nftnl_obj *e, struct nlattr *attr)
{
	struct nftnl_obj_limit *limit = nftnl_obj_data(e);
	struct nlattr *tb[NFTA_LIMIT_MAX + 1] = {};

	if (mnl_attr_parse_nested(attr, nftnl_obj_limit_cb, tb) < 0)
		return -1;

	if (tb[NFTA_LIMIT_RATE]) {
		limit->rate = be64toh(mnl_attr_get_u64(tb[NFTA_LIMIT_RATE]));
		e->flags |= (1 << NFTNL_OBJ_LIMIT_RATE);
	}
	if (tb[NFTA_LIMIT_UNIT]) {
		limit->unit = be64toh(mnl_attr_get_u64(tb[NFTA_LIMIT_UNIT]));
		e->flags |= (1 << NFTNL_OBJ_LIMIT_UNIT);
	}
	if (tb[NFTA_LIMIT_BURST]) {
		limit->burst = ntohl(mnl_attr_get_u32(tb[NFTA_LIMIT_BURST]));
		e->flags |= (1 << NFTNL_OBJ_LIMIT_BURST);
	}
	if (tb[NFTA_LIMIT_TYPE]) {
		limit->type = ntohl(mnl_attr_get_u32(tb[NFTA_LIMIT_TYPE]));
		e->flags |= (1 << NFTNL_OBJ_LIMIT_TYPE);
	}
	if (tb[NFTA_LIMIT_FLAGS]) {
		limit->flags = ntohl(mnl_attr_get_u32(tb[NFTA_LIMIT_FLAGS]));
		e->flags |= (1 << NFTNL_OBJ_LIMIT_FLAGS);
	}

	return 0;
}

static int nftnl_obj_limit_snprintf_default(char *buf, size_t len,
					    const struct nftnl_obj *e)
{
	struct nftnl_obj_limit *limit = nftnl_obj_data(e);

	return snprintf(buf, len, "rate %"PRIu64" unit %"PRIu64" burst %u "
			"type %u flags %u ", limit->rate, limit->unit,
			limit->burst, limit->type, limit->flags);
}

static int nftnl_obj_limit_snprintf(char *buf, size_t len, uint32_t type,
				    uint32_t flags,
				    const struct nftnl_obj *e)
{
	if (len)
		buf[0] = '\0';

	switch (type) {
	case NFTNL_OUTPUT_DEFAULT:
		return nftnl_obj_limit_snprintf_default(buf, len, e);
	case NFTNL_OUTPUT_XML:
	case NFTNL_OUTPUT_JSON:
	default:
		break;
	}
	return -1;
}

struct obj_ops obj_ops_limit = {
	.name		= "limit",
	.type		= NFT_OBJECT_LIMIT,
	.alloc_len	= sizeof(struct nftnl_obj_limit),
	.max_attr	= NFTA_LIMIT_MAX,
	.set		= nftnl_obj_limit_set,
	.get		= nftnl_obj_limit_get,
	.parse		= nftnl_obj_limit_parse,
	.build		= nftnl_obj_limit_build,
	.snprintf	= nftnl_obj_limit_snprintf,
};
