/*
 * (C) 2017 Red Hat GmbH
 * Author: Florian Westphal <fw@strlen.de>
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

static int nftnl_obj_ct_helper_set(struct nftnl_obj *e, uint16_t type,
				   const void *data, uint32_t data_len)
{
	struct nftnl_obj_ct_helper *helper = nftnl_obj_data(e);

	switch (type) {
	case NFTNL_OBJ_CT_HELPER_NAME:
		snprintf(helper->name, sizeof(helper->name), "%s", (const char *)data);
		break;
	case NFTNL_OBJ_CT_HELPER_L3PROTO:
		memcpy(&helper->l3proto, data, sizeof(helper->l3proto));
		break;
	case NFTNL_OBJ_CT_HELPER_L4PROTO:
		memcpy(&helper->l4proto, data, sizeof(helper->l4proto));
		break;
	default:
		return -1;
	}
	return 0;
}

static const void *nftnl_obj_ct_helper_get(const struct nftnl_obj *e,
					   uint16_t type, uint32_t *data_len)
{
	struct nftnl_obj_ct_helper *helper = nftnl_obj_data(e);

	switch (type) {
	case NFTNL_OBJ_CT_HELPER_NAME:
		*data_len = strlen(helper->name);
		return helper->name;
	case NFTNL_OBJ_CT_HELPER_L3PROTO:
		*data_len = sizeof(helper->l3proto);
		return &helper->l3proto;
	case NFTNL_OBJ_CT_HELPER_L4PROTO:
		*data_len = sizeof(helper->l4proto);
		return &helper->l4proto;
	}
	return NULL;
}

static int nftnl_obj_ct_helper_cb(const struct nlattr *attr, void *data)
{
	const struct nftnl_obj_ct_helper *helper = NULL;
	int type = mnl_attr_get_type(attr);
	const struct nlattr **tb = data;

	if (mnl_attr_type_valid(attr, NFTA_CT_HELPER_MAX) < 0)
		return MNL_CB_OK;

	switch (type) {
	case NFTA_CT_HELPER_NAME:
		if (mnl_attr_validate(attr, MNL_TYPE_STRING) < 0)
			abi_breakage();
		if (mnl_attr_get_payload_len(attr) >= sizeof(helper->name))
			abi_breakage();
		break;
	case NFTA_CT_HELPER_L3PROTO:
		if (mnl_attr_validate(attr, MNL_TYPE_U16) < 0)
			abi_breakage();
		break;
	case NFTA_CT_HELPER_L4PROTO:
		if (mnl_attr_validate(attr, MNL_TYPE_U8) < 0)
			abi_breakage();
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

static void
nftnl_obj_ct_helper_build(struct nlmsghdr *nlh, const struct nftnl_obj *e)
{
	struct nftnl_obj_ct_helper *helper = nftnl_obj_data(e);

	if (e->flags & (1 << NFTNL_OBJ_CT_HELPER_NAME))
		mnl_attr_put_str(nlh, NFTA_CT_HELPER_NAME, helper->name);
	if (e->flags & (1 << NFTNL_OBJ_CT_HELPER_L3PROTO))
		mnl_attr_put_u16(nlh, NFTA_CT_HELPER_L3PROTO, htons(helper->l3proto));
	if (e->flags & (1 << NFTNL_OBJ_CT_HELPER_L4PROTO))
		mnl_attr_put_u8(nlh, NFTA_CT_HELPER_L4PROTO, helper->l4proto);
}

static int
nftnl_obj_ct_helper_parse(struct nftnl_obj *e, struct nlattr *attr)
{
	struct nftnl_obj_ct_helper *helper = nftnl_obj_data(e);
	struct nlattr *tb[NFTA_CT_HELPER_MAX + 1] = {};

	if (mnl_attr_parse_nested(attr, nftnl_obj_ct_helper_cb, tb) < 0)
		return -1;

	if (tb[NFTA_CT_HELPER_NAME]) {
		snprintf(helper->name, sizeof(helper->name), "%s",
			 mnl_attr_get_str(tb[NFTA_CT_HELPER_NAME]));
		e->flags |= (1 << NFTNL_OBJ_CT_HELPER_NAME);
	}
	if (tb[NFTA_CT_HELPER_L3PROTO]) {
		helper->l3proto = ntohs(mnl_attr_get_u16(tb[NFTA_CT_HELPER_L3PROTO]));
		e->flags |= (1 << NFTNL_OBJ_CT_HELPER_L3PROTO);
	}
	if (tb[NFTA_CT_HELPER_L4PROTO]) {
		helper->l4proto = mnl_attr_get_u8(tb[NFTA_CT_HELPER_L4PROTO]);
		e->flags |= (1 << NFTNL_OBJ_CT_HELPER_L4PROTO);
	}

	return 0;
}

static int nftnl_obj_ct_helper_snprintf_default(char *buf, size_t len,
					       const struct nftnl_obj *e)
{
	struct nftnl_obj_ct_helper *helper = nftnl_obj_data(e);

	return snprintf(buf, len, "name %s family %d protocol %d ",
			helper->name, helper->l3proto, helper->l4proto);
}

static int nftnl_obj_ct_helper_snprintf(char *buf, size_t len, uint32_t type,
				       uint32_t flags,
				       const struct nftnl_obj *e)
{
	if (len)
		buf[0] = '\0';

	switch (type) {
	case NFTNL_OUTPUT_DEFAULT:
		return nftnl_obj_ct_helper_snprintf_default(buf, len, e);
	case NFTNL_OUTPUT_JSON:
	default:
		break;
	}
	return -1;
}

struct obj_ops obj_ops_ct_helper = {
	.name		= "ct_helper",
	.type		= NFT_OBJECT_CT_HELPER,
	.alloc_len	= sizeof(struct nftnl_obj_ct_helper),
	.max_attr	= NFTA_CT_HELPER_MAX,
	.set		= nftnl_obj_ct_helper_set,
	.get		= nftnl_obj_ct_helper_get,
	.parse		= nftnl_obj_ct_helper_parse,
	.build		= nftnl_obj_ct_helper_build,
	.snprintf	= nftnl_obj_ct_helper_snprintf,
};
