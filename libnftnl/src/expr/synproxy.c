#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <linux/netfilter/nf_tables.h>

#include "internal.h"
#include <libmnl/libmnl.h>
#include <libnftnl/expr.h>
#include <libnftnl/rule.h>

struct nftnl_expr_synproxy {
	uint16_t	mss;
	uint8_t		wscale;
	uint32_t	flags;
};

static int nftnl_expr_synproxy_set(struct nftnl_expr *e, uint16_t type,
				   const void *data, uint32_t data_len)
{
	struct nftnl_expr_synproxy *synproxy = nftnl_expr_data(e);

	switch(type) {
	case NFTNL_EXPR_SYNPROXY_MSS:
		memcpy(&synproxy->mss, data, sizeof(synproxy->mss));
		break;
	case NFTNL_EXPR_SYNPROXY_WSCALE:
		memcpy(&synproxy->wscale, data, sizeof(synproxy->wscale));
		break;
	case NFTNL_EXPR_SYNPROXY_FLAGS:
		memcpy(&synproxy->flags, data, sizeof(synproxy->flags));
		break;
	}
	return 0;
}

static const void *
nftnl_expr_synproxy_get(const struct nftnl_expr *e, uint16_t type,
			uint32_t *data_len)
{
	struct nftnl_expr_synproxy *synproxy = nftnl_expr_data(e);

	switch(type) {
	case NFTNL_EXPR_SYNPROXY_MSS:
		*data_len = sizeof(synproxy->mss);
		return &synproxy->mss;
	case NFTNL_EXPR_SYNPROXY_WSCALE:
		*data_len = sizeof(synproxy->wscale);
		return &synproxy->wscale;
	case NFTNL_EXPR_SYNPROXY_FLAGS:
		*data_len = sizeof(synproxy->flags);
		return &synproxy->flags;
	}
	return NULL;
}

static int nftnl_expr_synproxy_cb(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = data;
	int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, NFTA_SYNPROXY_MAX) < 0)
		return MNL_CB_OK;

	switch(type) {
	case NFTNL_EXPR_SYNPROXY_MSS:
		if (mnl_attr_validate(attr, MNL_TYPE_U16) < 0)
			abi_breakage();
		break;

	case NFTNL_EXPR_SYNPROXY_WSCALE:
		if (mnl_attr_validate(attr, MNL_TYPE_U8) < 0)
			abi_breakage();
		break;

	case NFTNL_EXPR_SYNPROXY_FLAGS:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0)
			abi_breakage();
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

static void
nftnl_expr_synproxy_build(struct nlmsghdr *nlh, const struct nftnl_expr *e)
{
	struct nftnl_expr_synproxy *synproxy = nftnl_expr_data(e);

	if (e->flags & (1 << NFTNL_EXPR_SYNPROXY_MSS))
		mnl_attr_put_u16(nlh, NFTNL_EXPR_SYNPROXY_MSS,
				 htons(synproxy->mss));
	if (e->flags & (1 << NFTNL_EXPR_SYNPROXY_WSCALE))
		mnl_attr_put_u8(nlh, NFTNL_EXPR_SYNPROXY_WSCALE,
				synproxy->wscale);
	if (e->flags & (1 << NFTNL_EXPR_SYNPROXY_FLAGS))
		mnl_attr_put_u32(nlh, NFTNL_EXPR_SYNPROXY_FLAGS,
				 htonl(synproxy->flags));
}

static int
nftnl_expr_synproxy_parse(struct nftnl_expr *e, struct nlattr *attr)
{
	struct nftnl_expr_synproxy *synproxy = nftnl_expr_data(e);
	struct nlattr *tb[NFTA_SYNPROXY_MAX + 1] = {};

	if (mnl_attr_parse_nested(attr, nftnl_expr_synproxy_cb, tb) < 0)
		return -1;

	if (tb[NFTA_SYNPROXY_MSS]) {
		synproxy->mss = ntohs(mnl_attr_get_u16(tb[NFTA_SYNPROXY_MSS]));
		e->flags |= (1 << NFTNL_EXPR_SYNPROXY_MSS);
	}

	if (tb[NFTA_SYNPROXY_WSCALE]) {
		synproxy->wscale = mnl_attr_get_u8(tb[NFTA_SYNPROXY_WSCALE]);
		e->flags |= (1 << NFTNL_EXPR_SYNPROXY_WSCALE);
	}

	if (tb[NFTA_SYNPROXY_FLAGS]) {
		synproxy->flags = ntohl(mnl_attr_get_u32(tb[NFTA_SYNPROXY_FLAGS]));
		e->flags |= (1 << NFTNL_EXPR_SYNPROXY_FLAGS);
	}

	return 0;
}

static int nftnl_expr_synproxy_snprintf_default(char *buf, size_t size,
						const struct nftnl_expr *e)
{
	struct nftnl_expr_synproxy *synproxy = nftnl_expr_data(e);
	int ret, offset = 0, len = size;

	if (e->flags & (1 << NFTNL_EXPR_SYNPROXY_MSS) &&
	    e->flags & (1 << NFTNL_EXPR_SYNPROXY_WSCALE)) {
		ret = snprintf(buf, len, "mss %u wscale %u ", synproxy->mss,
			       synproxy->wscale);
		SNPRINTF_BUFFER_SIZE(ret, len, offset);
	}

	return offset;
}

static int
nftnl_expr_synproxy_snprintf(char *buf, size_t len, uint32_t type,
			     uint32_t flags, const struct nftnl_expr *e)
{
	switch(type) {
	case NFTNL_OUTPUT_DEFAULT:
		return nftnl_expr_synproxy_snprintf_default(buf, len, e);
	case NFTNL_OUTPUT_XML:
	case NFTNL_OUTPUT_JSON:
	default:
		break;
	}
	return -1;
}

struct expr_ops expr_ops_synproxy = {
	.name		= "synproxy",
	.alloc_len	= sizeof(struct nftnl_expr_synproxy),
	.max_attr	= NFTA_SYNPROXY_MAX,
	.set		= nftnl_expr_synproxy_set,
	.get		= nftnl_expr_synproxy_get,
	.parse		= nftnl_expr_synproxy_parse,
	.build		= nftnl_expr_synproxy_build,
	.snprintf	= nftnl_expr_synproxy_snprintf,
};
