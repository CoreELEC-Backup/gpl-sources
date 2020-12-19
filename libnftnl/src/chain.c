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

#include <time.h>
#include <endian.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>
#include <inttypes.h>

#include <libmnl/libmnl.h>
#include <linux/netfilter/nfnetlink.h>
#include <linux/netfilter/nf_tables.h>
#include <linux/netfilter.h>
#include <linux/netfilter_arp.h>

#include <libnftnl/chain.h>
#include <libnftnl/rule.h>
#include <buffer.h>

struct nftnl_chain {
	struct list_head head;
	struct hlist_node hnode;

	const char	*name;
	const char	*type;
	const char	*table;
	const char	*dev;
	const char	**dev_array;
	int		dev_array_len;
	uint32_t	family;
	uint32_t	policy;
	uint32_t	hooknum;
	int32_t		prio;
	uint32_t	use;
	uint64_t	packets;
	uint64_t	bytes;
	uint64_t	handle;
	uint32_t	flags;

	struct list_head rule_list;
};

static const char *nftnl_hooknum2str(int family, int hooknum)
{
	switch (family) {
	case NFPROTO_IPV4:
	case NFPROTO_IPV6:
	case NFPROTO_INET:
	case NFPROTO_BRIDGE:
		switch (hooknum) {
		case NF_INET_PRE_ROUTING:
			return "prerouting";
		case NF_INET_LOCAL_IN:
			return "input";
		case NF_INET_FORWARD:
			return "forward";
		case NF_INET_LOCAL_OUT:
			return "output";
		case NF_INET_POST_ROUTING:
			return "postrouting";
		}
		break;
	case NFPROTO_ARP:
		switch (hooknum) {
		case NF_ARP_IN:
			return "input";
		case NF_ARP_OUT:
			return "output";
		case NF_ARP_FORWARD:
			return "forward";
		}
		break;
	case NFPROTO_NETDEV:
		switch (hooknum) {
		case NF_NETDEV_INGRESS:
			return "ingress";
		}
		break;
	}
	return "unknown";
}

EXPORT_SYMBOL(nftnl_chain_alloc);
struct nftnl_chain *nftnl_chain_alloc(void)
{
	struct nftnl_chain *c;

	c = calloc(1, sizeof(struct nftnl_chain));
	if (c == NULL)
		return NULL;

	INIT_LIST_HEAD(&c->rule_list);

	return c;
}

EXPORT_SYMBOL(nftnl_chain_free);
void nftnl_chain_free(const struct nftnl_chain *c)
{
	struct nftnl_rule *r, *tmp;
	int i;

	list_for_each_entry_safe(r, tmp, &c->rule_list, head)
		nftnl_rule_free(r);

	if (c->flags & (1 << NFTNL_CHAIN_NAME))
		xfree(c->name);
	if (c->flags & (1 << NFTNL_CHAIN_TABLE))
		xfree(c->table);
	if (c->flags & (1 << NFTNL_CHAIN_TYPE))
		xfree(c->type);
	if (c->flags & (1 << NFTNL_CHAIN_DEV))
		xfree(c->dev);
	if (c->flags & (1 << NFTNL_CHAIN_DEVICES)) {
		for (i = 0; i < c->dev_array_len; i++)
			xfree(c->dev_array[i]);

		xfree(c->dev_array);
	}
	xfree(c);
}

EXPORT_SYMBOL(nftnl_chain_is_set);
bool nftnl_chain_is_set(const struct nftnl_chain *c, uint16_t attr)
{
	return c->flags & (1 << attr);
}

EXPORT_SYMBOL(nftnl_chain_unset);
void nftnl_chain_unset(struct nftnl_chain *c, uint16_t attr)
{
	int i;

	if (!(c->flags & (1 << attr)))
		return;

	switch (attr) {
	case NFTNL_CHAIN_NAME:
		xfree(c->name);
		break;
	case NFTNL_CHAIN_TABLE:
		xfree(c->table);
		break;
	case NFTNL_CHAIN_USE:
		break;
	case NFTNL_CHAIN_TYPE:
		xfree(c->type);
		break;
	case NFTNL_CHAIN_HOOKNUM:
	case NFTNL_CHAIN_PRIO:
	case NFTNL_CHAIN_POLICY:
	case NFTNL_CHAIN_BYTES:
	case NFTNL_CHAIN_PACKETS:
	case NFTNL_CHAIN_HANDLE:
	case NFTNL_CHAIN_FAMILY:
		break;
	case NFTNL_CHAIN_DEV:
		xfree(c->dev);
		break;
	case NFTNL_CHAIN_DEVICES:
		for (i = 0; i < c->dev_array_len; i++)
			xfree(c->dev_array[i]);
		xfree(c->dev_array);
		break;
	default:
		return;
	}

	c->flags &= ~(1 << attr);
}

static uint32_t nftnl_chain_validate[NFTNL_CHAIN_MAX + 1] = {
	[NFTNL_CHAIN_HOOKNUM]	= sizeof(uint32_t),
	[NFTNL_CHAIN_PRIO]		= sizeof(int32_t),
	[NFTNL_CHAIN_POLICY]		= sizeof(uint32_t),
	[NFTNL_CHAIN_BYTES]		= sizeof(uint64_t),
	[NFTNL_CHAIN_PACKETS]	= sizeof(uint64_t),
	[NFTNL_CHAIN_HANDLE]		= sizeof(uint64_t),
	[NFTNL_CHAIN_FAMILY]		= sizeof(uint32_t),
};

EXPORT_SYMBOL(nftnl_chain_set_data);
int nftnl_chain_set_data(struct nftnl_chain *c, uint16_t attr,
			 const void *data, uint32_t data_len)
{
	const char **dev_array;
	int len = 0, i;

	nftnl_assert_attr_exists(attr, NFTNL_CHAIN_MAX);
	nftnl_assert_validate(data, nftnl_chain_validate, attr, data_len);

	switch(attr) {
	case NFTNL_CHAIN_NAME:
		if (c->flags & (1 << NFTNL_CHAIN_NAME))
			xfree(c->name);

		c->name = strdup(data);
		if (!c->name)
			return -1;
		break;
	case NFTNL_CHAIN_TABLE:
		if (c->flags & (1 << NFTNL_CHAIN_TABLE))
			xfree(c->table);

		c->table = strdup(data);
		if (!c->table)
			return -1;
		break;
	case NFTNL_CHAIN_HOOKNUM:
		memcpy(&c->hooknum, data, sizeof(c->hooknum));
		break;
	case NFTNL_CHAIN_PRIO:
		memcpy(&c->prio, data, sizeof(c->prio));
		break;
	case NFTNL_CHAIN_POLICY:
		memcpy(&c->policy, data, sizeof(c->policy));
		break;
	case NFTNL_CHAIN_USE:
		memcpy(&c->use, data, sizeof(c->use));
		break;
	case NFTNL_CHAIN_BYTES:
		memcpy(&c->bytes, data, sizeof(c->bytes));
		break;
	case NFTNL_CHAIN_PACKETS:
		memcpy(&c->packets, data, sizeof(c->packets));
		break;
	case NFTNL_CHAIN_HANDLE:
		memcpy(&c->handle, data, sizeof(c->handle));
		break;
	case NFTNL_CHAIN_FAMILY:
		memcpy(&c->family, data, sizeof(c->family));
		break;
	case NFTNL_CHAIN_TYPE:
		if (c->flags & (1 << NFTNL_CHAIN_TYPE))
			xfree(c->type);

		c->type = strdup(data);
		if (!c->type)
			return -1;
		break;
	case NFTNL_CHAIN_DEV:
		if (c->flags & (1 << NFTNL_CHAIN_DEV))
			xfree(c->dev);

		c->dev = strdup(data);
		if (!c->dev)
			return -1;
		break;
	case NFTNL_CHAIN_DEVICES:
		dev_array = (const char **)data;
		while (dev_array[len] != NULL)
			len++;

		if (c->flags & (1 << NFTNL_CHAIN_DEVICES)) {
			for (i = 0; i < c->dev_array_len; i++)
				xfree(c->dev_array[i]);
			xfree(c->dev_array);
		}

		c->dev_array = calloc(len + 1, sizeof(char *));
		if (!c->dev_array)
			return -1;

		for (i = 0; i < len; i++)
			c->dev_array[i] = strdup(dev_array[i]);

		c->dev_array_len = len;
		break;
	}
	c->flags |= (1 << attr);
	return 0;
}

void nftnl_chain_set(struct nftnl_chain *c, uint16_t attr, const void *data) __visible;
void nftnl_chain_set(struct nftnl_chain *c, uint16_t attr, const void *data)
{
	nftnl_chain_set_data(c, attr, data, nftnl_chain_validate[attr]);
}

EXPORT_SYMBOL(nftnl_chain_set_u32);
void nftnl_chain_set_u32(struct nftnl_chain *c, uint16_t attr, uint32_t data)
{
	nftnl_chain_set_data(c, attr, &data, sizeof(uint32_t));
}

EXPORT_SYMBOL(nftnl_chain_set_s32);
void nftnl_chain_set_s32(struct nftnl_chain *c, uint16_t attr, int32_t data)
{
	nftnl_chain_set_data(c, attr, &data, sizeof(int32_t));
}

EXPORT_SYMBOL(nftnl_chain_set_u64);
void nftnl_chain_set_u64(struct nftnl_chain *c, uint16_t attr, uint64_t data)
{
	nftnl_chain_set_data(c, attr, &data, sizeof(uint64_t));
}

EXPORT_SYMBOL(nftnl_chain_set_u8);
void nftnl_chain_set_u8(struct nftnl_chain *c, uint16_t attr, uint8_t data)
{
	nftnl_chain_set_data(c, attr, &data, sizeof(uint8_t));
}

EXPORT_SYMBOL(nftnl_chain_set_str);
int nftnl_chain_set_str(struct nftnl_chain *c, uint16_t attr, const char *str)
{
	return nftnl_chain_set_data(c, attr, str, strlen(str) + 1);
}

EXPORT_SYMBOL(nftnl_chain_get_data);
const void *nftnl_chain_get_data(const struct nftnl_chain *c, uint16_t attr,
				 uint32_t *data_len)
{
	if (!(c->flags & (1 << attr)))
		return NULL;

	switch(attr) {
	case NFTNL_CHAIN_NAME:
		*data_len = strlen(c->name) + 1;
		return c->name;
	case NFTNL_CHAIN_TABLE:
		*data_len = strlen(c->table) + 1;
		return c->table;
	case NFTNL_CHAIN_HOOKNUM:
		*data_len = sizeof(uint32_t);
		return &c->hooknum;
	case NFTNL_CHAIN_PRIO:
		*data_len = sizeof(int32_t);
		return &c->prio;
	case NFTNL_CHAIN_POLICY:
		*data_len = sizeof(uint32_t);
		return &c->policy;
	case NFTNL_CHAIN_USE:
		*data_len = sizeof(uint32_t);
		return &c->use;
	case NFTNL_CHAIN_BYTES:
		*data_len = sizeof(uint64_t);
		return &c->bytes;
	case NFTNL_CHAIN_PACKETS:
		*data_len = sizeof(uint64_t);
		return &c->packets;
	case NFTNL_CHAIN_HANDLE:
		*data_len = sizeof(uint64_t);
		return &c->handle;
	case NFTNL_CHAIN_FAMILY:
		*data_len = sizeof(uint32_t);
		return &c->family;
	case NFTNL_CHAIN_TYPE:
		*data_len = sizeof(uint32_t);
		return c->type;
	case NFTNL_CHAIN_DEV:
		*data_len = strlen(c->dev) + 1;
		return c->dev;
	case NFTNL_CHAIN_DEVICES:
		return &c->dev_array[0];
	}
	return NULL;
}

EXPORT_SYMBOL(nftnl_chain_get);
const void *nftnl_chain_get(const struct nftnl_chain *c, uint16_t attr)
{
	uint32_t data_len;
	return nftnl_chain_get_data(c, attr, &data_len);
}

EXPORT_SYMBOL(nftnl_chain_get_str);
const char *nftnl_chain_get_str(const struct nftnl_chain *c, uint16_t attr)
{
	return nftnl_chain_get(c, attr);
}

EXPORT_SYMBOL(nftnl_chain_get_u32);
uint32_t nftnl_chain_get_u32(const struct nftnl_chain *c, uint16_t attr)
{
	uint32_t data_len;
	const uint32_t *val = nftnl_chain_get_data(c, attr, &data_len);

	nftnl_assert(val, attr, data_len == sizeof(uint32_t));

	return val ? *val : 0;
}

EXPORT_SYMBOL(nftnl_chain_get_s32);
int32_t nftnl_chain_get_s32(const struct nftnl_chain *c, uint16_t attr)
{
	uint32_t data_len;
	const int32_t *val = nftnl_chain_get_data(c, attr, &data_len);

	nftnl_assert(val, attr, data_len == sizeof(int32_t));

	return val ? *val : 0;
}

EXPORT_SYMBOL(nftnl_chain_get_u64);
uint64_t nftnl_chain_get_u64(const struct nftnl_chain *c, uint16_t attr)
{
	uint32_t data_len;
	const uint64_t *val = nftnl_chain_get_data(c, attr, &data_len);

	nftnl_assert(val, attr, data_len == sizeof(int64_t));

	return val ? *val : 0;
}

EXPORT_SYMBOL(nftnl_chain_get_u8);
uint8_t nftnl_chain_get_u8(const struct nftnl_chain *c, uint16_t attr)
{
	uint32_t data_len;
	const uint8_t *val = nftnl_chain_get_data(c, attr, &data_len);

	nftnl_assert(val, attr, data_len == sizeof(int8_t));

	return val ? *val : 0;
}

EXPORT_SYMBOL(nftnl_chain_nlmsg_build_payload);
void nftnl_chain_nlmsg_build_payload(struct nlmsghdr *nlh, const struct nftnl_chain *c)
{
	int i;

	if (c->flags & (1 << NFTNL_CHAIN_TABLE))
		mnl_attr_put_strz(nlh, NFTA_CHAIN_TABLE, c->table);
	if (c->flags & (1 << NFTNL_CHAIN_NAME))
		mnl_attr_put_strz(nlh, NFTA_CHAIN_NAME, c->name);
	if ((c->flags & (1 << NFTNL_CHAIN_HOOKNUM)) &&
	    (c->flags & (1 << NFTNL_CHAIN_PRIO))) {
		struct nlattr *nest;

		nest = mnl_attr_nest_start(nlh, NFTA_CHAIN_HOOK);
		mnl_attr_put_u32(nlh, NFTA_HOOK_HOOKNUM, htonl(c->hooknum));
		mnl_attr_put_u32(nlh, NFTA_HOOK_PRIORITY, htonl(c->prio));
		if (c->flags & (1 << NFTNL_CHAIN_DEV))
			mnl_attr_put_strz(nlh, NFTA_HOOK_DEV, c->dev);
		else if (c->flags & (1 << NFTNL_CHAIN_DEVICES)) {
			struct nlattr *nest_dev;

			nest_dev = mnl_attr_nest_start(nlh, NFTA_HOOK_DEVS);
			for (i = 0; i < c->dev_array_len; i++)
				mnl_attr_put_strz(nlh, NFTA_DEVICE_NAME,
						  c->dev_array[i]);
			mnl_attr_nest_end(nlh, nest_dev);
		}
		mnl_attr_nest_end(nlh, nest);
	}
	if (c->flags & (1 << NFTNL_CHAIN_POLICY))
		mnl_attr_put_u32(nlh, NFTA_CHAIN_POLICY, htonl(c->policy));
	if (c->flags & (1 << NFTNL_CHAIN_USE))
		mnl_attr_put_u32(nlh, NFTA_CHAIN_USE, htonl(c->use));
	if ((c->flags & (1 << NFTNL_CHAIN_PACKETS)) &&
	    (c->flags & (1 << NFTNL_CHAIN_BYTES))) {
		struct nlattr *nest;

		nest = mnl_attr_nest_start(nlh, NFTA_CHAIN_COUNTERS);
		mnl_attr_put_u64(nlh, NFTA_COUNTER_PACKETS, be64toh(c->packets));
		mnl_attr_put_u64(nlh, NFTA_COUNTER_BYTES, be64toh(c->bytes));
		mnl_attr_nest_end(nlh, nest);
	}
	if (c->flags & (1 << NFTNL_CHAIN_HANDLE))
		mnl_attr_put_u64(nlh, NFTA_CHAIN_HANDLE, be64toh(c->handle));
	if (c->flags & (1 << NFTNL_CHAIN_TYPE))
		mnl_attr_put_strz(nlh, NFTA_CHAIN_TYPE, c->type);
}

EXPORT_SYMBOL(nftnl_chain_rule_add);
void nftnl_chain_rule_add(struct nftnl_rule *rule, struct nftnl_chain *c)
{
	list_add(&rule->head, &c->rule_list);
}

EXPORT_SYMBOL(nftnl_chain_rule_del);
void nftnl_chain_rule_del(struct nftnl_rule *r)
{
	list_del(&r->head);
}

EXPORT_SYMBOL(nftnl_chain_rule_add_tail);
void nftnl_chain_rule_add_tail(struct nftnl_rule *rule, struct nftnl_chain *c)
{
	list_add_tail(&rule->head, &c->rule_list);
}

EXPORT_SYMBOL(nftnl_chain_rule_insert_at);
void nftnl_chain_rule_insert_at(struct nftnl_rule *rule, struct nftnl_rule *pos)
{
	list_add_tail(&rule->head, &pos->head);
}

EXPORT_SYMBOL(nftnl_chain_rule_append_at);
void nftnl_chain_rule_append_at(struct nftnl_rule *rule, struct nftnl_rule *pos)
{
	list_add(&rule->head, &pos->head);
}

static int nftnl_chain_parse_attr_cb(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = data;
	int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, NFTA_CHAIN_MAX) < 0)
		return MNL_CB_OK;

	switch(type) {
	case NFTA_CHAIN_NAME:
	case NFTA_CHAIN_TABLE:
	case NFTA_CHAIN_TYPE:
		if (mnl_attr_validate(attr, MNL_TYPE_STRING) < 0)
			abi_breakage();
		break;
	case NFTA_CHAIN_HOOK:
	case NFTA_CHAIN_COUNTERS:
		if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0)
			abi_breakage();
		break;
	case NFTA_CHAIN_POLICY:
	case NFTA_CHAIN_USE:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0)
			abi_breakage();
		break;
	case NFTA_CHAIN_HANDLE:
		if (mnl_attr_validate(attr, MNL_TYPE_U64) < 0)
			abi_breakage();
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

static int nftnl_chain_parse_counters_cb(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = data;
	int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, NFTA_COUNTER_MAX) < 0)
		return MNL_CB_OK;

	switch(type) {
	case NFTA_COUNTER_BYTES:
	case NFTA_COUNTER_PACKETS:
		if (mnl_attr_validate(attr, MNL_TYPE_U64) < 0)
			abi_breakage();
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

static int nftnl_chain_parse_counters(struct nlattr *attr, struct nftnl_chain *c)
{
	struct nlattr *tb[NFTA_COUNTER_MAX+1] = {};

	if (mnl_attr_parse_nested(attr, nftnl_chain_parse_counters_cb, tb) < 0)
		return -1;

	if (tb[NFTA_COUNTER_PACKETS]) {
		c->packets = be64toh(mnl_attr_get_u64(tb[NFTA_COUNTER_PACKETS]));
		c->flags |= (1 << NFTNL_CHAIN_PACKETS);
	}
	if (tb[NFTA_COUNTER_BYTES]) {
		c->bytes = be64toh(mnl_attr_get_u64(tb[NFTA_COUNTER_BYTES]));
		c->flags |= (1 << NFTNL_CHAIN_BYTES);
	}

	return 0;
}

static int nftnl_chain_parse_hook_cb(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = data;
	int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, NFTA_HOOK_MAX) < 0)
		return MNL_CB_OK;

	switch(type) {
	case NFTA_HOOK_HOOKNUM:
	case NFTA_HOOK_PRIORITY:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0)
			abi_breakage();
		break;
	case NFTA_HOOK_DEV:
		if (mnl_attr_validate(attr, MNL_TYPE_STRING) < 0)
			abi_breakage();
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

static int nftnl_chain_parse_devs(struct nlattr *nest, struct nftnl_chain *c)
{
	const char **dev_array;
	int len = 0, size = 8;
	struct nlattr *attr;

	dev_array = calloc(8, sizeof(char *));
	if (!dev_array)
		return -1;

	mnl_attr_for_each_nested(attr, nest) {
		if (mnl_attr_get_type(attr) != NFTA_DEVICE_NAME)
			goto err;
		dev_array[len++] = strdup(mnl_attr_get_str(attr));
		if (len >= size) {
			dev_array = realloc(dev_array,
					    size * 2 * sizeof(char *));
			if (!dev_array)
				goto err;

			size *= 2;
			memset(&dev_array[len], 0,
			       (size - len) * sizeof(char *));
		}
	}

	c->dev_array = dev_array;
	c->dev_array_len = len;

	return 0;
err:
	while (len--)
		xfree(dev_array[len]);
	return -1;
}

static int nftnl_chain_parse_hook(struct nlattr *attr, struct nftnl_chain *c)
{
	struct nlattr *tb[NFTA_HOOK_MAX+1] = {};
	int ret;

	if (mnl_attr_parse_nested(attr, nftnl_chain_parse_hook_cb, tb) < 0)
		return -1;

	if (tb[NFTA_HOOK_HOOKNUM]) {
		c->hooknum = ntohl(mnl_attr_get_u32(tb[NFTA_HOOK_HOOKNUM]));
		c->flags |= (1 << NFTNL_CHAIN_HOOKNUM);
	}
	if (tb[NFTA_HOOK_PRIORITY]) {
		c->prio = ntohl(mnl_attr_get_u32(tb[NFTA_HOOK_PRIORITY]));
		c->flags |= (1 << NFTNL_CHAIN_PRIO);
	}
	if (tb[NFTA_HOOK_DEV]) {
		c->dev = strdup(mnl_attr_get_str(tb[NFTA_HOOK_DEV]));
		if (!c->dev)
			return -1;
		c->flags |= (1 << NFTNL_CHAIN_DEV);
	}
	if (tb[NFTA_HOOK_DEVS]) {
		ret = nftnl_chain_parse_devs(tb[NFTA_HOOK_DEVS], c);
		if (ret < 0)
			return -1;
		c->flags |= (1 << NFTNL_CHAIN_DEVICES);
	}

	return 0;
}

EXPORT_SYMBOL(nftnl_chain_nlmsg_parse);
int nftnl_chain_nlmsg_parse(const struct nlmsghdr *nlh, struct nftnl_chain *c)
{
	struct nlattr *tb[NFTA_CHAIN_MAX+1] = {};
	struct nfgenmsg *nfg = mnl_nlmsg_get_payload(nlh);
	int ret = 0;

	if (mnl_attr_parse(nlh, sizeof(*nfg), nftnl_chain_parse_attr_cb, tb) < 0)
		return -1;

	if (tb[NFTA_CHAIN_NAME]) {
		if (c->flags & (1 << NFTNL_CHAIN_NAME))
			xfree(c->name);
		c->name = strdup(mnl_attr_get_str(tb[NFTA_CHAIN_NAME]));
		if (!c->name)
			return -1;
		c->flags |= (1 << NFTNL_CHAIN_NAME);
	}
	if (tb[NFTA_CHAIN_TABLE]) {
		if (c->flags & (1 << NFTNL_CHAIN_TABLE))
			xfree(c->table);
		c->table = strdup(mnl_attr_get_str(tb[NFTA_CHAIN_TABLE]));
		if (!c->table)
			return -1;
		c->flags |= (1 << NFTNL_CHAIN_TABLE);
	}
	if (tb[NFTA_CHAIN_HOOK]) {
		ret = nftnl_chain_parse_hook(tb[NFTA_CHAIN_HOOK], c);
		if (ret < 0)
			return ret;
	}
	if (tb[NFTA_CHAIN_POLICY]) {
		c->policy = ntohl(mnl_attr_get_u32(tb[NFTA_CHAIN_POLICY]));
		c->flags |= (1 << NFTNL_CHAIN_POLICY);
	}
	if (tb[NFTA_CHAIN_USE]) {
		c->use = ntohl(mnl_attr_get_u32(tb[NFTA_CHAIN_USE]));
		c->flags |= (1 << NFTNL_CHAIN_USE);
	}
	if (tb[NFTA_CHAIN_COUNTERS]) {
		ret = nftnl_chain_parse_counters(tb[NFTA_CHAIN_COUNTERS], c);
		if (ret < 0)
			return ret;
	}
	if (tb[NFTA_CHAIN_HANDLE]) {
		c->handle = be64toh(mnl_attr_get_u64(tb[NFTA_CHAIN_HANDLE]));
		c->flags |= (1 << NFTNL_CHAIN_HANDLE);
	}
	if (tb[NFTA_CHAIN_TYPE]) {
		if (c->flags & (1 << NFTNL_CHAIN_TYPE))
			xfree(c->type);
		c->type = strdup(mnl_attr_get_str(tb[NFTA_CHAIN_TYPE]));
		if (!c->type)
			return -1;
		c->flags |= (1 << NFTNL_CHAIN_TYPE);
	}

	c->family = nfg->nfgen_family;
	c->flags |= (1 << NFTNL_CHAIN_FAMILY);

	return ret;
}

static inline int nftnl_str2hooknum(int family, const char *hook)
{
	int hooknum;

	for (hooknum = 0; hooknum < NF_INET_NUMHOOKS; hooknum++) {
		if (strcmp(hook, nftnl_hooknum2str(family, hooknum)) == 0)
			return hooknum;
	}
	return -1;
}

static int nftnl_chain_snprintf_default(char *buf, size_t size,
					const struct nftnl_chain *c)
{
	int ret, remain = size, offset = 0, i;

	ret = snprintf(buf, remain, "%s %s %s use %u",
		       nftnl_family2str(c->family), c->table, c->name, c->use);
	SNPRINTF_BUFFER_SIZE(ret, remain, offset);

	if (c->flags & (1 << NFTNL_CHAIN_HOOKNUM)) {
		ret = snprintf(buf + offset, remain, " type %s hook %s prio %d",
			       c->type, nftnl_hooknum2str(c->family, c->hooknum),
			       c->prio);
		SNPRINTF_BUFFER_SIZE(ret, remain, offset);

		if (c->flags & (1 << NFTNL_CHAIN_POLICY)) {
			ret = snprintf(buf + offset, remain, " policy %s",
				       nftnl_verdict2str(c->policy));
			SNPRINTF_BUFFER_SIZE(ret, remain, offset);
		}

		ret = snprintf(buf + offset, remain,
			       " packets %"PRIu64" bytes %"PRIu64"",
			       c->packets, c->bytes);
		SNPRINTF_BUFFER_SIZE(ret, remain, offset);

		if (c->flags & (1 << NFTNL_CHAIN_DEV)) {
			ret = snprintf(buf + offset, remain, " dev %s ",
				       c->dev);
			SNPRINTF_BUFFER_SIZE(ret, remain, offset);
		}
		if (c->flags & (1 << NFTNL_CHAIN_DEVICES)) {
			ret = snprintf(buf + offset, remain, " dev { ");
			SNPRINTF_BUFFER_SIZE(ret, remain, offset);

			for (i = 0; i < c->dev_array_len; i++) {
				ret = snprintf(buf + offset, remain, " %s ",
					       c->dev_array[i]);
				SNPRINTF_BUFFER_SIZE(ret, remain, offset);
			}
			ret = snprintf(buf + offset, remain, " } ");
			SNPRINTF_BUFFER_SIZE(ret, remain, offset);
		}
	}

	return offset;
}

static int nftnl_chain_cmd_snprintf(char *buf, size_t size,
				    const struct nftnl_chain *c, uint32_t cmd,
				    uint32_t type, uint32_t flags)
{
	int ret, remain = size, offset = 0;

	switch (type) {
	case NFTNL_OUTPUT_DEFAULT:
		ret = nftnl_chain_snprintf_default(buf + offset, remain, c);
		SNPRINTF_BUFFER_SIZE(ret, remain, offset);
		break;
	case NFTNL_OUTPUT_XML:
	case NFTNL_OUTPUT_JSON:
	default:
		return -1;
	}

	return offset;
}

EXPORT_SYMBOL(nftnl_chain_snprintf);
int nftnl_chain_snprintf(char *buf, size_t size, const struct nftnl_chain *c,
			 uint32_t type, uint32_t flags)
{
	if (size)
		buf[0] = '\0';

	return nftnl_chain_cmd_snprintf(buf, size, c, nftnl_flag2cmd(flags),
					type, flags);
}

static int nftnl_chain_do_snprintf(char *buf, size_t size, const void *c,
				   uint32_t cmd, uint32_t type, uint32_t flags)
{
	return nftnl_chain_snprintf(buf, size, c, type, flags);
}

EXPORT_SYMBOL(nftnl_chain_fprintf);
int nftnl_chain_fprintf(FILE *fp, const struct nftnl_chain *c, uint32_t type,
			uint32_t flags)
{
	return nftnl_fprintf(fp, c, NFTNL_CMD_UNSPEC, type, flags,
			   nftnl_chain_do_snprintf);
}

EXPORT_SYMBOL(nftnl_rule_foreach);
int nftnl_rule_foreach(struct nftnl_chain *c,
                          int (*cb)(struct nftnl_rule *r, void *data),
                          void *data)
{
       struct nftnl_rule *cur, *tmp;
       int ret;

       list_for_each_entry_safe(cur, tmp, &c->rule_list, head) {
               ret = cb(cur, data);
               if (ret < 0)
                       return ret;
       }
       return 0;
}

EXPORT_SYMBOL(nftnl_rule_lookup_byindex);
struct nftnl_rule *
nftnl_rule_lookup_byindex(struct nftnl_chain *c, uint32_t index)
{
	struct nftnl_rule *r;

	list_for_each_entry(r, &c->rule_list, head) {
		if (!index)
			return r;
		index--;
	}
	return NULL;
}

struct nftnl_rule_iter {
	const struct nftnl_chain	*c;
	struct nftnl_rule		*cur;
};

static void nftnl_rule_iter_init(const struct nftnl_chain *c,
				 struct nftnl_rule_iter *iter)
{
	iter->c = c;
	if (list_empty(&c->rule_list))
		iter->cur = NULL;
	else
		iter->cur = list_entry(c->rule_list.next, struct nftnl_rule,
				       head);
}

EXPORT_SYMBOL(nftnl_rule_iter_create);
struct nftnl_rule_iter *nftnl_rule_iter_create(const struct nftnl_chain *c)
{
	struct nftnl_rule_iter *iter;

	iter = calloc(1, sizeof(struct nftnl_rule_iter));
	if (iter == NULL)
		return NULL;

	nftnl_rule_iter_init(c, iter);

	return iter;
}

EXPORT_SYMBOL(nftnl_rule_iter_next);
struct nftnl_rule *nftnl_rule_iter_next(struct nftnl_rule_iter *iter)
{
	struct nftnl_rule *rule = iter->cur;

	if (rule == NULL)
		return NULL;

	/* get next rule, if any */
	iter->cur = list_entry(iter->cur->head.next, struct nftnl_rule, head);
	if (&iter->cur->head == iter->c->rule_list.next)
		return NULL;

	return rule;
}

EXPORT_SYMBOL(nftnl_rule_iter_destroy);
void nftnl_rule_iter_destroy(struct nftnl_rule_iter *iter)
{
	xfree(iter);
}

#define CHAIN_NAME_HSIZE	512

struct nftnl_chain_list {

	struct list_head list;
	struct hlist_head name_hash[CHAIN_NAME_HSIZE];
};

EXPORT_SYMBOL(nftnl_chain_list_alloc);
struct nftnl_chain_list *nftnl_chain_list_alloc(void)
{
	struct nftnl_chain_list *list;
	int i;

	list = calloc(1, sizeof(struct nftnl_chain_list));
	if (list == NULL)
		return NULL;

	INIT_LIST_HEAD(&list->list);
	for (i = 0; i < CHAIN_NAME_HSIZE; i++)
		INIT_HLIST_HEAD(&list->name_hash[i]);

	return list;
}

EXPORT_SYMBOL(nftnl_chain_list_free);
void nftnl_chain_list_free(struct nftnl_chain_list *list)
{
	struct nftnl_chain *r, *tmp;

	list_for_each_entry_safe(r, tmp, &list->list, head) {
		list_del(&r->head);
		hlist_del(&r->hnode);
		nftnl_chain_free(r);
	}
	xfree(list);
}

EXPORT_SYMBOL(nftnl_chain_list_is_empty);
int nftnl_chain_list_is_empty(const struct nftnl_chain_list *list)
{
	return list_empty(&list->list);
}

static uint32_t djb_hash(const char *key)
{
	uint32_t i, hash = 5381;

	for (i = 0; i < strlen(key); i++)
		hash = ((hash << 5) + hash) + key[i];

	return hash;
}

EXPORT_SYMBOL(nftnl_chain_list_add);
void nftnl_chain_list_add(struct nftnl_chain *r, struct nftnl_chain_list *list)
{
	int key = djb_hash(r->name) % CHAIN_NAME_HSIZE;

	hlist_add_head(&r->hnode, &list->name_hash[key]);
	list_add(&r->head, &list->list);
}

EXPORT_SYMBOL(nftnl_chain_list_add_tail);
void nftnl_chain_list_add_tail(struct nftnl_chain *r, struct nftnl_chain_list *list)
{
	int key = djb_hash(r->name) % CHAIN_NAME_HSIZE;

	hlist_add_head(&r->hnode, &list->name_hash[key]);
	list_add_tail(&r->head, &list->list);
}

EXPORT_SYMBOL(nftnl_chain_list_del);
void nftnl_chain_list_del(struct nftnl_chain *r)
{
	list_del(&r->head);
	hlist_del(&r->hnode);
}

EXPORT_SYMBOL(nftnl_chain_list_foreach);
int nftnl_chain_list_foreach(struct nftnl_chain_list *chain_list,
			   int (*cb)(struct nftnl_chain *r, void *data),
			   void *data)
{
	struct nftnl_chain *cur, *tmp;
	int ret;

	list_for_each_entry_safe(cur, tmp, &chain_list->list, head) {
		ret = cb(cur, data);
		if (ret < 0)
			return ret;
	}
	return 0;
}

EXPORT_SYMBOL(nftnl_chain_list_lookup_byname);
struct nftnl_chain *
nftnl_chain_list_lookup_byname(struct nftnl_chain_list *chain_list,
			       const char *chain)
{
	int key = djb_hash(chain) % CHAIN_NAME_HSIZE;
	struct nftnl_chain *c;
	struct hlist_node *n;

	hlist_for_each_entry(c, n, &chain_list->name_hash[key], hnode) {
		if (!strcmp(chain, c->name))
			return c;
	}
	return NULL;
}

struct nftnl_chain_list_iter {
	const struct nftnl_chain_list	*list;
	struct nftnl_chain		*cur;
};

EXPORT_SYMBOL(nftnl_chain_list_iter_create);
struct nftnl_chain_list_iter *
nftnl_chain_list_iter_create(const struct nftnl_chain_list *l)
{
	struct nftnl_chain_list_iter *iter;

	iter = calloc(1, sizeof(struct nftnl_chain_list_iter));
	if (iter == NULL)
		return NULL;

	iter->list = l;
	if (nftnl_chain_list_is_empty(l))
		iter->cur = NULL;
	else
		iter->cur = list_entry(l->list.next, struct nftnl_chain, head);

	return iter;
}

EXPORT_SYMBOL(nftnl_chain_list_iter_next);
struct nftnl_chain *nftnl_chain_list_iter_next(struct nftnl_chain_list_iter *iter)
{
	struct nftnl_chain *r = iter->cur;

	if (r == NULL)
		return NULL;

	/* get next chain, if any */
	iter->cur = list_entry(iter->cur->head.next, struct nftnl_chain, head);
	if (&iter->cur->head == iter->list->list.next)
		return NULL;

	return r;
}

EXPORT_SYMBOL(nftnl_chain_list_iter_destroy);
void nftnl_chain_list_iter_destroy(struct nftnl_chain_list_iter *iter)
{
	xfree(iter);
}
