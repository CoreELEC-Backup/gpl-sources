/*
 * (C) 2016 by Pablo Neira Ayuso <pablo@netfilter.org>
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
#include <libnftnl/expr.h>
#include <libnftnl/rule.h>

struct nftnl_expr_objref {
	struct {
		uint32_t	type;
		const char	*name;
	} imm;
	struct {
		uint32_t	sreg;
		const char	*name;
		uint32_t	id;
	} set;
};

static int nftnl_expr_objref_set(struct nftnl_expr *e, uint16_t type,
				 const void *data, uint32_t data_len)
{
	struct nftnl_expr_objref *objref = nftnl_expr_data(e);

	switch(type) {
	case NFTNL_EXPR_OBJREF_IMM_TYPE:
		memcpy(&objref->imm.type, data, sizeof(objref->imm.type));
		break;
	case NFTNL_EXPR_OBJREF_IMM_NAME:
		objref->imm.name = strdup(data);
		if (!objref->imm.name)
			return -1;
		break;
	case NFTNL_EXPR_OBJREF_SET_SREG:
		memcpy(&objref->set.sreg, data, sizeof(objref->set.sreg));
		break;
	case NFTNL_EXPR_OBJREF_SET_NAME:
		objref->set.name = strdup(data);
		if (!objref->set.name)
			return -1;
		break;
	case NFTNL_EXPR_OBJREF_SET_ID:
		memcpy(&objref->set.id, data, sizeof(objref->set.id));
		break;
	default:
		return -1;
	}
	return 0;
}

static const void *nftnl_expr_objref_get(const struct nftnl_expr *e,
					 uint16_t type, uint32_t *data_len)
{
	struct nftnl_expr_objref *objref = nftnl_expr_data(e);

	switch(type) {
	case NFTNL_EXPR_OBJREF_IMM_TYPE:
		*data_len = sizeof(objref->imm.type);
		return &objref->imm.type;
	case NFTNL_EXPR_OBJREF_IMM_NAME:
		*data_len = strlen(objref->imm.name) + 1;
		return objref->imm.name;
	case NFTNL_EXPR_OBJREF_SET_SREG:
		*data_len = sizeof(objref->set.sreg);
		return &objref->set.sreg;
	case NFTNL_EXPR_OBJREF_SET_NAME:
		*data_len = strlen(objref->set.name) + 1;
		return objref->set.name;
	case NFTNL_EXPR_OBJREF_SET_ID:
		*data_len = sizeof(objref->set.id);
		return &objref->set.id;
	}
	return NULL;
}

static int nftnl_expr_objref_cb(const struct nlattr *attr, void *data)
{
	int type = mnl_attr_get_type(attr);
	const struct nlattr **tb = data;

	if (mnl_attr_type_valid(attr, NFTA_OBJREF_MAX) < 0)
		return MNL_CB_OK;

	switch(type) {
	case NFTA_OBJREF_IMM_TYPE:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0)
			abi_breakage();
		break;
	case NFTA_OBJREF_IMM_NAME:
	case NFTA_OBJREF_SET_NAME:
		if (mnl_attr_validate(attr, MNL_TYPE_STRING) < 0)
			abi_breakage();
		break;
	case NFTA_OBJREF_SET_SREG:
	case NFTA_OBJREF_SET_ID:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0)
			abi_breakage();
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

static void nftnl_expr_objref_build(struct nlmsghdr *nlh,
				    const struct nftnl_expr *e)
{
	struct nftnl_expr_objref *objref = nftnl_expr_data(e);

	if (e->flags & (1 << NFTNL_EXPR_OBJREF_IMM_TYPE))
		mnl_attr_put_u32(nlh, NFTA_OBJREF_IMM_TYPE,
				 htonl(objref->imm.type));
	if (e->flags & (1 << NFTNL_EXPR_OBJREF_IMM_NAME))
		mnl_attr_put_str(nlh, NFTA_OBJREF_IMM_NAME, objref->imm.name);
	if (e->flags & (1 << NFTNL_EXPR_OBJREF_SET_SREG))
		mnl_attr_put_u32(nlh, NFTA_OBJREF_SET_SREG,
				 htonl(objref->set.sreg));
	if (e->flags & (1 << NFTNL_EXPR_OBJREF_SET_NAME))
		mnl_attr_put_str(nlh, NFTA_OBJREF_SET_NAME, objref->set.name);
	if (e->flags & (1 << NFTNL_EXPR_OBJREF_SET_ID))
		mnl_attr_put_u32(nlh, NFTA_OBJREF_SET_ID,
				 htonl(objref->set.id));
}

static int nftnl_expr_objref_parse(struct nftnl_expr *e, struct nlattr *attr)
{
	struct nftnl_expr_objref *objref = nftnl_expr_data(e);
	struct nlattr *tb[NFTA_OBJREF_MAX + 1] = {};

	if (mnl_attr_parse_nested(attr, nftnl_expr_objref_cb, tb) < 0)
		return -1;

	if (tb[NFTA_OBJREF_IMM_TYPE]) {
		objref->imm.type =
			ntohl(mnl_attr_get_u32(tb[NFTA_OBJREF_IMM_TYPE]));
		e->flags |= (1 << NFTNL_EXPR_OBJREF_IMM_TYPE);
	}
	if (tb[NFTA_OBJREF_IMM_NAME]) {
		objref->imm.name =
			strdup(mnl_attr_get_str(tb[NFTA_OBJREF_IMM_NAME]));
		e->flags |= (1 << NFTNL_EXPR_OBJREF_IMM_NAME);
	}
	if (tb[NFTA_OBJREF_SET_SREG]) {
		objref->set.sreg =
			ntohl(mnl_attr_get_u32(tb[NFTA_OBJREF_SET_SREG]));
		e->flags |= (1 << NFTNL_EXPR_OBJREF_SET_SREG);
	}
	if (tb[NFTA_OBJREF_SET_NAME]) {
		objref->set.name =
			strdup(mnl_attr_get_str(tb[NFTA_OBJREF_SET_NAME]));
		e->flags |= (1 << NFTNL_EXPR_OBJREF_SET_NAME);
	}
	if (tb[NFTA_OBJREF_SET_ID]) {
		objref->set.id =
			ntohl(mnl_attr_get_u32(tb[NFTA_OBJREF_SET_ID]));
		e->flags |= (1 << NFTNL_EXPR_OBJREF_SET_ID);
	}

	return 0;
}

static int nftnl_expr_objref_snprintf_default(char *buf, size_t len,
					      const struct nftnl_expr *e)
{
	struct nftnl_expr_objref *objref = nftnl_expr_data(e);

	if (e->flags & (1 << NFTNL_EXPR_OBJREF_SET_SREG))
		return snprintf(buf, len, "sreg %u set %s ",
				objref->set.sreg, objref->set.name);
	else
		return snprintf(buf, len, "type %u name %s ",
				objref->imm.type, objref->imm.name);
}

static int nftnl_expr_objref_snprintf(char *buf, size_t len, uint32_t type,
				      uint32_t flags,
				      const struct nftnl_expr *e)
{
	switch (type) {
	case NFTNL_OUTPUT_DEFAULT:
		return nftnl_expr_objref_snprintf_default(buf, len, e);
	case NFTNL_OUTPUT_XML:
	case NFTNL_OUTPUT_JSON:
	default:
		break;
	}
	return -1;
}

struct expr_ops expr_ops_objref = {
	.name		= "objref",
	.alloc_len	= sizeof(struct nftnl_expr_objref),
	.max_attr	= NFTA_OBJREF_MAX,
	.set		= nftnl_expr_objref_set,
	.get		= nftnl_expr_objref_get,
	.parse		= nftnl_expr_objref_parse,
	.build		= nftnl_expr_objref_build,
	.snprintf	= nftnl_expr_objref_snprintf,
};
