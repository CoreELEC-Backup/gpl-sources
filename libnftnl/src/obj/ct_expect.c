/*
 * (C) 2019 by Stéphane Veyret <sveyret@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <arpa/inet.h>
#include <errno.h>

#include <libmnl/libmnl.h>

#include "obj.h"

static int nftnl_obj_ct_expect_set(struct nftnl_obj *e, uint16_t type,
				   const void *data, uint32_t data_len)
{
	struct nftnl_obj_ct_expect *exp = nftnl_obj_data(e);

	switch (type) {
	case NFTNL_OBJ_CT_EXPECT_L3PROTO:
		memcpy(&exp->l3proto, data, sizeof(exp->l3proto));
		break;
	case NFTNL_OBJ_CT_EXPECT_L4PROTO:
		memcpy(&exp->l4proto, data, sizeof(exp->l4proto));
		break;
	case NFTNL_OBJ_CT_EXPECT_DPORT:
		memcpy(&exp->dport, data, sizeof(exp->dport));
		break;
	case NFTNL_OBJ_CT_EXPECT_TIMEOUT:
		memcpy(&exp->timeout, data, sizeof(exp->timeout));
		break;
	case NFTNL_OBJ_CT_EXPECT_SIZE:
		memcpy(&exp->size, data, sizeof(exp->size));
		break;
	default:
		return -1;
	}
	return 0;
}

static const void *nftnl_obj_ct_expect_get(const struct nftnl_obj *e,
					   uint16_t type, uint32_t *data_len)
{
	struct nftnl_obj_ct_expect *exp = nftnl_obj_data(e);

	switch (type) {
	case NFTNL_OBJ_CT_EXPECT_L3PROTO:
		*data_len = sizeof(exp->l3proto);
		return &exp->l3proto;
	case NFTNL_OBJ_CT_EXPECT_L4PROTO:
		*data_len = sizeof(exp->l4proto);
		return &exp->l4proto;
	case NFTNL_OBJ_CT_EXPECT_DPORT:
		*data_len = sizeof(exp->dport);
		return &exp->dport;
	case NFTNL_OBJ_CT_EXPECT_TIMEOUT:
		*data_len = sizeof(exp->timeout);
		return &exp->timeout;
	case NFTNL_OBJ_CT_EXPECT_SIZE:
		*data_len = sizeof(exp->size);
		return &exp->size;
	}
	return NULL;
}

static int nftnl_obj_ct_expect_cb(const struct nlattr *attr, void *data)
{
	int type = mnl_attr_get_type(attr);
	const struct nlattr **tb = data;

	if (mnl_attr_type_valid(attr, NFTA_CT_EXPECT_MAX) < 0)
		return MNL_CB_OK;

	switch (type) {
	case NFTA_CT_EXPECT_L3PROTO:
		if (mnl_attr_validate(attr, MNL_TYPE_U16) < 0)
			abi_breakage();
		break;
	case NFTA_CT_EXPECT_L4PROTO:
		if (mnl_attr_validate(attr, MNL_TYPE_U8) < 0)
			abi_breakage();
		break;
	case NFTA_CT_EXPECT_DPORT:
		if (mnl_attr_validate(attr, MNL_TYPE_U16) < 0)
			abi_breakage();
		break;
	case NFTA_CT_EXPECT_TIMEOUT:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0)
			abi_breakage();
		break;
	case NFTA_CT_EXPECT_SIZE:
		if (mnl_attr_validate(attr, MNL_TYPE_U8) < 0)
			abi_breakage();
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

static void
nftnl_obj_ct_expect_build(struct nlmsghdr *nlh, const struct nftnl_obj *e)
{
	struct nftnl_obj_ct_expect *exp = nftnl_obj_data(e);

	if (e->flags & (1 << NFTNL_OBJ_CT_EXPECT_L3PROTO))
		mnl_attr_put_u16(nlh, NFTA_CT_EXPECT_L3PROTO, htons(exp->l3proto));
	if (e->flags & (1 << NFTNL_OBJ_CT_EXPECT_L4PROTO))
		mnl_attr_put_u8(nlh, NFTA_CT_EXPECT_L4PROTO, exp->l4proto);
	if (e->flags & (1 << NFTNL_OBJ_CT_EXPECT_DPORT))
		mnl_attr_put_u16(nlh, NFTA_CT_EXPECT_DPORT, htons(exp->dport));
	if (e->flags & (1 << NFTNL_OBJ_CT_EXPECT_TIMEOUT))
		mnl_attr_put_u32(nlh, NFTA_CT_EXPECT_TIMEOUT, exp->timeout);
	if (e->flags & (1 << NFTNL_OBJ_CT_EXPECT_SIZE))
		mnl_attr_put_u8(nlh, NFTA_CT_EXPECT_SIZE, exp->size);
}

static int
nftnl_obj_ct_expect_parse(struct nftnl_obj *e, struct nlattr *attr)
{
	struct nftnl_obj_ct_expect *exp = nftnl_obj_data(e);
	struct nlattr *tb[NFTA_CT_EXPECT_MAX + 1] = {};

	if (mnl_attr_parse_nested(attr, nftnl_obj_ct_expect_cb, tb) < 0)
		return -1;

	if (tb[NFTA_CT_EXPECT_L3PROTO]) {
		exp->l3proto = ntohs(mnl_attr_get_u16(tb[NFTA_CT_EXPECT_L3PROTO]));
		e->flags |= (1 << NFTNL_OBJ_CT_EXPECT_L3PROTO);
	}
	if (tb[NFTA_CT_EXPECT_L4PROTO]) {
		exp->l4proto = mnl_attr_get_u8(tb[NFTA_CT_EXPECT_L4PROTO]);
		e->flags |= (1 << NFTNL_OBJ_CT_EXPECT_L4PROTO);
	}
	if (tb[NFTA_CT_EXPECT_DPORT]) {
		exp->dport = ntohs(mnl_attr_get_u16(tb[NFTA_CT_EXPECT_DPORT]));
		e->flags |= (1 << NFTNL_OBJ_CT_EXPECT_DPORT);
	}
	if (tb[NFTA_CT_EXPECT_TIMEOUT]) {
		exp->timeout = mnl_attr_get_u32(tb[NFTA_CT_EXPECT_TIMEOUT]);
		e->flags |= (1 << NFTNL_OBJ_CT_EXPECT_TIMEOUT);
	}
	if (tb[NFTA_CT_EXPECT_SIZE]) {
		exp->size = mnl_attr_get_u8(tb[NFTA_CT_EXPECT_SIZE]);
		e->flags |= (1 << NFTNL_OBJ_CT_EXPECT_SIZE);
	}

	return 0;
}

static int nftnl_obj_ct_expect_snprintf_default(char *buf, size_t len,
						const struct nftnl_obj *e)
{
	int ret = 0;
	int offset = 0, remain = len;
	struct nftnl_obj_ct_expect *exp = nftnl_obj_data(e);

	if (e->flags & (1 << NFTNL_OBJ_CT_EXPECT_L3PROTO)) {
		ret = snprintf(buf + offset, len, "family %d ", exp->l3proto);
		SNPRINTF_BUFFER_SIZE(ret, remain, offset);
	}
	if (e->flags & (1 << NFTNL_OBJ_CT_EXPECT_L4PROTO)) {
		ret = snprintf(buf + offset, len, "protocol %d ", exp->l4proto);
		SNPRINTF_BUFFER_SIZE(ret, remain, offset);
	}
	if (e->flags & (1 << NFTNL_OBJ_CT_EXPECT_DPORT)) {
		ret = snprintf(buf + offset, len, "dport %d ", exp->dport);
		SNPRINTF_BUFFER_SIZE(ret, remain, offset);
	}
	if (e->flags & (1 << NFTNL_OBJ_CT_EXPECT_TIMEOUT)) {
		ret = snprintf(buf + offset, len, "timeout %d ", exp->timeout);
		SNPRINTF_BUFFER_SIZE(ret, remain, offset);
	}
	if (e->flags & (1 << NFTNL_OBJ_CT_EXPECT_SIZE)) {
		ret = snprintf(buf + offset, len, "size %d ", exp->size);
		SNPRINTF_BUFFER_SIZE(ret, remain, offset);
	}

	buf[offset] = '\0';
	return offset;
}

static int nftnl_obj_ct_expect_snprintf(char *buf, size_t len, uint32_t type,
					uint32_t flags,
					const struct nftnl_obj *e)
{
	if (len)
		buf[0] = '\0';

	switch (type) {
	case NFTNL_OUTPUT_DEFAULT:
		return nftnl_obj_ct_expect_snprintf_default(buf, len, e);
	case NFTNL_OUTPUT_JSON:
	default:
		break;
	}
	return -1;
}

struct obj_ops obj_ops_ct_expect = {
	.name		= "ct_expect",
	.type		= NFT_OBJECT_CT_EXPECT,
	.alloc_len	= sizeof(struct nftnl_obj_ct_expect),
	.max_attr	= NFTA_CT_EXPECT_MAX,
	.set		= nftnl_obj_ct_expect_set,
	.get		= nftnl_obj_ct_expect_get,
	.parse		= nftnl_obj_ct_expect_parse,
	.build		= nftnl_obj_ct_expect_build,
	.snprintf	= nftnl_obj_ct_expect_snprintf,
};
