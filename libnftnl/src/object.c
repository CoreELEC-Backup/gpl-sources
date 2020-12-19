/*
 * (C) 2012-2016 by Pablo Neira Ayuso <pablo@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "internal.h"

#include <time.h>
#include <endian.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>

#include <libmnl/libmnl.h>
#include <linux/netfilter/nfnetlink.h>
#include <linux/netfilter/nf_tables.h>

#include <libnftnl/object.h>
#include <buffer.h>
#include "obj.h"

static struct obj_ops *obj_ops[__NFT_OBJECT_MAX] = {
	[NFT_OBJECT_COUNTER]	= &obj_ops_counter,
	[NFT_OBJECT_QUOTA]	= &obj_ops_quota,
	[NFT_OBJECT_CT_HELPER]	= &obj_ops_ct_helper,
	[NFT_OBJECT_LIMIT]	= &obj_ops_limit,
	[NFT_OBJECT_TUNNEL]	= &obj_ops_tunnel,
	[NFT_OBJECT_CT_TIMEOUT] = &obj_ops_ct_timeout,
	[NFT_OBJECT_SECMARK]	= &obj_ops_secmark,
	[NFT_OBJECT_CT_EXPECT]	= &obj_ops_ct_expect,
	[NFT_OBJECT_SYNPROXY]	= &obj_ops_synproxy,
};

static struct obj_ops *nftnl_obj_ops_lookup(uint32_t type)
{
	if (type > NFT_OBJECT_MAX)
		return NULL;

	return obj_ops[type];
}

EXPORT_SYMBOL(nftnl_obj_alloc);
struct nftnl_obj *nftnl_obj_alloc(void)
{
	return calloc(1, sizeof(struct nftnl_obj));
}

EXPORT_SYMBOL(nftnl_obj_free);
void nftnl_obj_free(const struct nftnl_obj *obj)
{
	if (obj->flags & (1 << NFTNL_OBJ_TABLE))
		xfree(obj->table);
	if (obj->flags & (1 << NFTNL_OBJ_NAME))
		xfree(obj->name);

	xfree(obj);
}

EXPORT_SYMBOL(nftnl_obj_is_set);
bool nftnl_obj_is_set(const struct nftnl_obj *obj, uint16_t attr)
{
	return obj->flags & (1 << attr);
}

static uint32_t nftnl_obj_validate[NFTNL_OBJ_MAX + 1] = {
	[NFTNL_OBJ_FAMILY]	= sizeof(uint32_t),
	[NFTNL_OBJ_USE]		= sizeof(uint32_t),
	[NFTNL_OBJ_HANDLE]	= sizeof(uint64_t),
};

EXPORT_SYMBOL(nftnl_obj_set_data);
void nftnl_obj_set_data(struct nftnl_obj *obj, uint16_t attr,
			const void *data, uint32_t data_len)
{
	if (attr < NFTNL_OBJ_MAX)
		nftnl_assert_validate(data, nftnl_obj_validate, attr, data_len);

	switch (attr) {
	case NFTNL_OBJ_TABLE:
		xfree(obj->table);
		obj->table = strdup(data);
		break;
	case NFTNL_OBJ_NAME:
		xfree(obj->name);
		obj->name = strdup(data);
		break;
	case NFTNL_OBJ_TYPE:
		obj->ops = nftnl_obj_ops_lookup(*((uint32_t *)data));
		if (!obj->ops)
			return;
		break;
	case NFTNL_OBJ_FAMILY:
		memcpy(&obj->family, data, sizeof(obj->family));
		break;
	case NFTNL_OBJ_USE:
		memcpy(&obj->use, data, sizeof(obj->use));
		break;
	case NFTNL_OBJ_HANDLE:
		memcpy(&obj->handle, data, sizeof(obj->handle));
		break;
	default:
		if (obj->ops)
			obj->ops->set(obj, attr, data, data_len);
		break;
	}
	obj->flags |= (1 << attr);
}

void nftnl_obj_set(struct nftnl_obj *obj, uint16_t attr, const void *data) __visible;
void nftnl_obj_set(struct nftnl_obj *obj, uint16_t attr, const void *data)
{
	nftnl_obj_set_data(obj, attr, data, nftnl_obj_validate[attr]);
}

EXPORT_SYMBOL(nftnl_obj_set_u8);
void nftnl_obj_set_u8(struct nftnl_obj *obj, uint16_t attr, uint8_t val)
{
	nftnl_obj_set_data(obj, attr, &val, sizeof(uint8_t));
}

EXPORT_SYMBOL(nftnl_obj_set_u16);
void nftnl_obj_set_u16(struct nftnl_obj *obj, uint16_t attr, uint16_t val)
{
	nftnl_obj_set_data(obj, attr, &val, sizeof(uint16_t));
}

EXPORT_SYMBOL(nftnl_obj_set_u32);
void nftnl_obj_set_u32(struct nftnl_obj *obj, uint16_t attr, uint32_t val)
{
	nftnl_obj_set_data(obj, attr, &val, sizeof(uint32_t));
}

EXPORT_SYMBOL(nftnl_obj_set_u64);
void nftnl_obj_set_u64(struct nftnl_obj *obj, uint16_t attr, uint64_t val)
{
	nftnl_obj_set_data(obj, attr, &val, sizeof(uint64_t));
}

EXPORT_SYMBOL(nftnl_obj_set_str);
void nftnl_obj_set_str(struct nftnl_obj *obj, uint16_t attr, const char *str)
{
	nftnl_obj_set_data(obj, attr, str, 0);
}

EXPORT_SYMBOL(nftnl_obj_get_data);
const void *nftnl_obj_get_data(struct nftnl_obj *obj, uint16_t attr,
			       uint32_t *data_len)
{
	if (!(obj->flags & (1 << attr)))
		return NULL;

	switch(attr) {
	case NFTNL_OBJ_TABLE:
		return obj->table;
	case NFTNL_OBJ_NAME:
		return obj->name;
	case NFTNL_OBJ_TYPE:
		if (!obj->ops)
			return NULL;

		*data_len = sizeof(uint32_t);
		return &obj->ops->type;
	case NFTNL_OBJ_FAMILY:
		*data_len = sizeof(uint32_t);
		return &obj->family;
	case NFTNL_OBJ_USE:
		*data_len = sizeof(uint32_t);
		return &obj->use;
	case NFTNL_OBJ_HANDLE:
		*data_len = sizeof(uint64_t);
		return &obj->handle;
	default:
		if (obj->ops)
			return obj->ops->get(obj, attr, data_len);
		break;
	}
	return NULL;
}

EXPORT_SYMBOL(nftnl_obj_get);
const void *nftnl_obj_get(struct nftnl_obj *obj, uint16_t attr)
{
	uint32_t data_len;
	return nftnl_obj_get_data(obj, attr, &data_len);
}

EXPORT_SYMBOL(nftnl_obj_get_u8);
uint8_t nftnl_obj_get_u8(struct nftnl_obj *obj, uint16_t attr)
{
	const void *ret = nftnl_obj_get(obj, attr);
	return ret == NULL ? 0 : *((uint8_t *)ret);
}

EXPORT_SYMBOL(nftnl_obj_get_u16);
uint16_t nftnl_obj_get_u16(struct nftnl_obj *obj, uint16_t attr)
{
	const void *ret = nftnl_obj_get(obj, attr);
	return ret == NULL ? 0 : *((uint16_t *)ret);
}

EXPORT_SYMBOL(nftnl_obj_get_u32);
uint32_t nftnl_obj_get_u32(struct nftnl_obj *obj, uint16_t attr)
{
	const void *ret = nftnl_obj_get(obj, attr);
	return ret == NULL ? 0 : *((uint32_t *)ret);
}

EXPORT_SYMBOL(nftnl_obj_get_u64);
uint64_t nftnl_obj_get_u64(struct nftnl_obj *obj, uint16_t attr)
{
	const void *ret = nftnl_obj_get(obj, attr);
	return ret == NULL ? 0 : *((uint64_t *)ret);
}

EXPORT_SYMBOL(nftnl_obj_get_str);
const char *nftnl_obj_get_str(struct nftnl_obj *obj, uint16_t attr)
{
	return nftnl_obj_get(obj, attr);
}

EXPORT_SYMBOL(nftnl_obj_nlmsg_build_payload);
void nftnl_obj_nlmsg_build_payload(struct nlmsghdr *nlh,
				   const struct nftnl_obj *obj)
{
	if (obj->flags & (1 << NFTNL_OBJ_TABLE))
		mnl_attr_put_strz(nlh, NFTA_OBJ_TABLE, obj->table);
	if (obj->flags & (1 << NFTNL_OBJ_NAME))
		mnl_attr_put_strz(nlh, NFTA_OBJ_NAME, obj->name);
	if (obj->flags & (1 << NFTNL_OBJ_TYPE))
		mnl_attr_put_u32(nlh, NFTA_OBJ_TYPE, htonl(obj->ops->type));
	if (obj->flags & (1 << NFTNL_OBJ_HANDLE))
		mnl_attr_put_u64(nlh, NFTA_OBJ_HANDLE, htobe64(obj->handle));
	if (obj->ops) {
		struct nlattr *nest = mnl_attr_nest_start(nlh, NFTA_OBJ_DATA);

		obj->ops->build(nlh, obj);
		mnl_attr_nest_end(nlh, nest);
	}
}

static int nftnl_obj_parse_attr_cb(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = data;
	int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, NFTA_OBJ_MAX) < 0)
		return MNL_CB_OK;

	switch(type) {
	case NFTA_OBJ_TABLE:
	case NFTA_OBJ_NAME:
		if (mnl_attr_validate(attr, MNL_TYPE_STRING) < 0)
			abi_breakage();
		break;
	case NFTA_OBJ_HANDLE:
		if (mnl_attr_validate(attr, MNL_TYPE_U64) < 0)
			abi_breakage();
		break;
	case NFTA_OBJ_DATA:
		if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0)
			abi_breakage();
		break;
	case NFTA_OBJ_USE:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0)
			abi_breakage();
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

EXPORT_SYMBOL(nftnl_obj_nlmsg_parse);
int nftnl_obj_nlmsg_parse(const struct nlmsghdr *nlh, struct nftnl_obj *obj)
{
	struct nfgenmsg *nfg = mnl_nlmsg_get_payload(nlh);
	struct nlattr *tb[NFTA_OBJ_MAX + 1] = {};
	int err;

	if (mnl_attr_parse(nlh, sizeof(*nfg), nftnl_obj_parse_attr_cb, tb) < 0)
		return -1;

	if (tb[NFTA_OBJ_TABLE]) {
		obj->table = strdup(mnl_attr_get_str(tb[NFTA_OBJ_TABLE]));
		obj->flags |= (1 << NFTNL_OBJ_TABLE);
	}
	if (tb[NFTA_OBJ_NAME]) {
		obj->name = strdup(mnl_attr_get_str(tb[NFTA_OBJ_NAME]));
		obj->flags |= (1 << NFTNL_OBJ_NAME);
	}
	if (tb[NFTA_OBJ_TYPE]) {
		uint32_t type = ntohl(mnl_attr_get_u32(tb[NFTA_OBJ_TYPE]));

		obj->ops = nftnl_obj_ops_lookup(type);
		if (obj->ops)
			obj->flags |= (1 << NFTNL_OBJ_TYPE);
	}
	if (tb[NFTA_OBJ_DATA]) {
		if (obj->ops) {
			err = obj->ops->parse(obj, tb[NFTA_OBJ_DATA]);
			if (err < 0)
				return err;
		}
	}
	if (tb[NFTA_OBJ_USE]) {
		obj->use = ntohl(mnl_attr_get_u32(tb[NFTA_OBJ_USE]));
		obj->flags |= (1 << NFTNL_OBJ_USE);
	}
	if (tb[NFTA_OBJ_HANDLE]) {
		obj->handle = be64toh(mnl_attr_get_u64(tb[NFTA_OBJ_HANDLE]));
		obj->flags |= (1 << NFTNL_OBJ_HANDLE);
	}

	obj->family = nfg->nfgen_family;
	obj->flags |= (1 << NFTNL_OBJ_FAMILY);

	return 0;
}

static int nftnl_obj_do_parse(struct nftnl_obj *obj, enum nftnl_parse_type type,
			      const void *data, struct nftnl_parse_err *err,
			      enum nftnl_parse_input input)
{
	struct nftnl_parse_err perr = {};
	int ret;

	switch (type) {
	case NFTNL_PARSE_JSON:
	case NFTNL_PARSE_XML:
	default:
		ret = -1;
		errno = EOPNOTSUPP;
		break;
	}

	if (err != NULL)
		*err = perr;

	return ret;
}

EXPORT_SYMBOL(nftnl_obj_parse);
int nftnl_obj_parse(struct nftnl_obj *obj, enum nftnl_parse_type type,
		      const char *data, struct nftnl_parse_err *err)
{
	return nftnl_obj_do_parse(obj, type, data, err, NFTNL_PARSE_BUFFER);
}

EXPORT_SYMBOL(nftnl_obj_parse_file);
int nftnl_obj_parse_file(struct nftnl_obj *obj, enum nftnl_parse_type type,
			   FILE *fp, struct nftnl_parse_err *err)
{
	return nftnl_obj_do_parse(obj, type, fp, err, NFTNL_PARSE_FILE);
}

static int nftnl_obj_snprintf_dflt(char *buf, size_t size,
				   const struct nftnl_obj *obj,
				   uint32_t type, uint32_t flags)
{
	const char *name = obj->ops ? obj->ops->name : "(unknown)";
	int ret, remain = size, offset = 0;

	ret = snprintf(buf, size, "table %s name %s use %u [ %s ",
		       obj->table, obj->name, obj->use, name);
	SNPRINTF_BUFFER_SIZE(ret, remain, offset);

	if (obj->ops) {
		ret = obj->ops->snprintf(buf + offset, offset, type, flags,
					 obj);
		SNPRINTF_BUFFER_SIZE(ret, remain, offset);
	}
	ret = snprintf(buf + offset, offset, "]");
	SNPRINTF_BUFFER_SIZE(ret, remain, offset);

	return offset;
}

static int nftnl_obj_cmd_snprintf(char *buf, size_t size,
				    const struct nftnl_obj *obj, uint32_t cmd,
				    uint32_t type, uint32_t flags)
{
	int ret, remain = size, offset = 0;

	switch (type) {
	case NFTNL_OUTPUT_DEFAULT:
		ret = nftnl_obj_snprintf_dflt(buf + offset, remain, obj, type,
					      flags);
		break;
	case NFTNL_OUTPUT_JSON:
	case NFTNL_OUTPUT_XML:
	default:
		return -1;
	}
	SNPRINTF_BUFFER_SIZE(ret, remain, offset);

	return offset;
}

EXPORT_SYMBOL(nftnl_obj_snprintf);
int nftnl_obj_snprintf(char *buf, size_t size, const struct nftnl_obj *obj,
		       uint32_t type, uint32_t flags)
{
	if (size)
		buf[0] = '\0';

	return nftnl_obj_cmd_snprintf(buf, size, obj, nftnl_flag2cmd(flags),
				      type, flags);
}

static int nftnl_obj_do_snprintf(char *buf, size_t size, const void *obj,
				 uint32_t cmd, uint32_t type, uint32_t flags)
{
	return nftnl_obj_snprintf(buf, size, obj, type, flags);
}

EXPORT_SYMBOL(nftnl_obj_fprintf);
int nftnl_obj_fprintf(FILE *fp, const struct nftnl_obj *obj, uint32_t type,
		      uint32_t flags)
{
	return nftnl_fprintf(fp, obj, NFTNL_CMD_UNSPEC, type, flags,
			     nftnl_obj_do_snprintf);
}

struct nftnl_obj_list {
	struct list_head list;
};

EXPORT_SYMBOL(nftnl_obj_list_alloc);
struct nftnl_obj_list *nftnl_obj_list_alloc(void)
{
	struct nftnl_obj_list *list;

	list = calloc(1, sizeof(struct nftnl_obj_list));
	if (list == NULL)
		return NULL;

	INIT_LIST_HEAD(&list->list);

	return list;
}

EXPORT_SYMBOL(nftnl_obj_list_free);
void nftnl_obj_list_free(struct nftnl_obj_list *list)
{
	struct nftnl_obj *r, *tmp;

	list_for_each_entry_safe(r, tmp, &list->list, head) {
		list_del(&r->head);
		nftnl_obj_free(r);
	}
	xfree(list);
}

EXPORT_SYMBOL(nftnl_obj_list_is_empty);
int nftnl_obj_list_is_empty(struct nftnl_obj_list *list)
{
	return list_empty(&list->list);
}

EXPORT_SYMBOL(nftnl_obj_list_add);
void nftnl_obj_list_add(struct nftnl_obj *r, struct nftnl_obj_list *list)
{
	list_add(&r->head, &list->list);
}

EXPORT_SYMBOL(nftnl_obj_list_add_tail);
void nftnl_obj_list_add_tail(struct nftnl_obj *r,
			       struct nftnl_obj_list *list)
{
	list_add_tail(&r->head, &list->list);
}

EXPORT_SYMBOL(nftnl_obj_list_del);
void nftnl_obj_list_del(struct nftnl_obj *t)
{
	list_del(&t->head);
}

EXPORT_SYMBOL(nftnl_obj_list_foreach);
int nftnl_obj_list_foreach(struct nftnl_obj_list *table_list,
			     int (*cb)(struct nftnl_obj *t, void *data),
			     void *data)
{
	struct nftnl_obj *cur, *tmp;
	int ret;

	list_for_each_entry_safe(cur, tmp, &table_list->list, head) {
		ret = cb(cur, data);
		if (ret < 0)
			return ret;
	}
	return 0;
}

struct nftnl_obj_list_iter {
	struct nftnl_obj_list	*list;
	struct nftnl_obj	*cur;
};

EXPORT_SYMBOL(nftnl_obj_list_iter_create);
struct nftnl_obj_list_iter *
nftnl_obj_list_iter_create(struct nftnl_obj_list *l)
{
	struct nftnl_obj_list_iter *iter;

	iter = calloc(1, sizeof(struct nftnl_obj_list_iter));
	if (iter == NULL)
		return NULL;

	iter->list = l;
	if (nftnl_obj_list_is_empty(l))
		iter->cur = NULL;
	else
		iter->cur = list_entry(l->list.next, struct nftnl_obj, head);

	return iter;
}

EXPORT_SYMBOL(nftnl_obj_list_iter_next);
struct nftnl_obj *nftnl_obj_list_iter_next(struct nftnl_obj_list_iter *iter)
{
	struct nftnl_obj *r = iter->cur;

	if (r == NULL)
		return NULL;

	/* get next table, if any */
	iter->cur = list_entry(iter->cur->head.next, struct nftnl_obj, head);
	if (&iter->cur->head == iter->list->list.next)
		return NULL;

	return r;
}

EXPORT_SYMBOL(nftnl_obj_list_iter_destroy);
void nftnl_obj_list_iter_destroy(struct nftnl_obj_list_iter *iter)
{
	xfree(iter);
}
