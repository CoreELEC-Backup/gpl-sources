/*
 * (C) 2012-2013 by Pablo Neira Ayuso <pablo@netfilter.org>
 * (C) 2013 by Arturo Borrero Gonzalez <arturo@debian.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <internal.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <errno.h>
#include <inttypes.h>

#include <libnftnl/common.h>

#include <linux/netfilter.h>
#include <linux/netfilter/nf_tables.h>

static const char *const nftnl_family_str[NFPROTO_NUMPROTO] = {
	[NFPROTO_INET]		= "inet",
	[NFPROTO_IPV4]		= "ip",
	[NFPROTO_ARP]		= "arp",
	[NFPROTO_NETDEV]	= "netdev",
	[NFPROTO_BRIDGE]	= "bridge",
	[NFPROTO_IPV6]		= "ip6",
};

const char *nftnl_family2str(uint32_t family)
{
	if (family >= NFPROTO_NUMPROTO || !nftnl_family_str[family])
		return "unknown";

	return nftnl_family_str[family];
}

int nftnl_str2family(const char *family)
{
	int i;

	for (i = 0; i < NFPROTO_NUMPROTO; i++) {
		if (nftnl_family_str[i] == NULL)
			continue;

		if (strcmp(nftnl_family_str[i], family) == 0)
			return i;
	}

	errno = EAFNOSUPPORT;
	return -1;
}

static struct {
	int len;
	int64_t min;
	uint64_t max;
} basetype[] = {
	[NFTNL_TYPE_U8]	 = { .len = sizeof(uint8_t), .max = UINT8_MAX },
	[NFTNL_TYPE_U16] = { .len = sizeof(uint16_t), .max = UINT16_MAX },
	[NFTNL_TYPE_U32] = { .len = sizeof(uint32_t), .max = UINT32_MAX },
	[NFTNL_TYPE_U64] = { .len = sizeof(uint64_t), .max = UINT64_MAX },
	[NFTNL_TYPE_S8]  = { .len = sizeof(int8_t), .min = INT8_MIN, .max = INT8_MAX },
	[NFTNL_TYPE_S16] = { .len = sizeof(int16_t), .min = INT16_MIN, .max = INT16_MAX },
	[NFTNL_TYPE_S32] = { .len = sizeof(int32_t), .min = INT32_MIN, .max = INT32_MAX },
	[NFTNL_TYPE_S64] = { .len = sizeof(int64_t), .min = INT64_MIN, .max = INT64_MAX },
};

int nftnl_get_value(enum nftnl_type type, void *val, void *out)
{
	union {
		uint8_t u8;
		uint16_t u16;
		uint32_t u32;
		int8_t s8;
		int16_t s16;
		int32_t s32;
	} values;
	void *valuep = NULL;
	int64_t sval;
	uint64_t uval;

	switch (type) {
	case NFTNL_TYPE_U8:
	case NFTNL_TYPE_U16:
	case NFTNL_TYPE_U32:
	case NFTNL_TYPE_U64:
		memcpy(&uval, val, sizeof(uval));
		if (uval > basetype[type].max) {
			errno = ERANGE;
			return -1;
		}
		break;
	case NFTNL_TYPE_S8:
	case NFTNL_TYPE_S16:
	case NFTNL_TYPE_S32:
	case NFTNL_TYPE_S64:
		memcpy(&sval, val, sizeof(sval));
		if (sval < basetype[type].min ||
		    sval > (int64_t)basetype[type].max) {
			errno = ERANGE;
			return -1;
		}
		break;
	}

	switch (type) {
	case NFTNL_TYPE_U8:
		values.u8 = uval;
		valuep = &values.u8;
		break;
	case NFTNL_TYPE_U16:
		values.u16 = uval;
		valuep = &values.u16;
		break;
	case NFTNL_TYPE_U32:
		values.u32 = uval;
		valuep = &values.u32;
		break;
	case NFTNL_TYPE_U64:
		valuep = &uval;
		break;
	case NFTNL_TYPE_S8:
		values.s8 = sval;
		valuep = &values.s8;
		break;
	case NFTNL_TYPE_S16:
		values.s16 = sval;
		valuep = &values.s16;
		break;
	case NFTNL_TYPE_S32:
		values.s32 = sval;
		valuep = &values.s32;
		break;
	case NFTNL_TYPE_S64:
		valuep = &sval;
		break;
	}
	memcpy(out, valuep, basetype[type].len);
	return 0;
}

int nftnl_strtoi(const char *string, int base, void *out, enum nftnl_type type)
{
	int ret;
	int64_t sval = 0;
	uint64_t uval = -1;
	char *endptr;

	switch (type) {
	case NFTNL_TYPE_U8:
	case NFTNL_TYPE_U16:
	case NFTNL_TYPE_U32:
	case NFTNL_TYPE_U64:
		uval = strtoll(string, &endptr, base);
		ret = nftnl_get_value(type, &uval, out);
		break;
	case NFTNL_TYPE_S8:
	case NFTNL_TYPE_S16:
	case NFTNL_TYPE_S32:
	case NFTNL_TYPE_S64:
		sval = strtoull(string, &endptr, base);
		ret = nftnl_get_value(type, &sval, out);
		break;
	default:
		errno = EINVAL;
		return -1;
	}

	if (*endptr) {
		errno = EINVAL;
		return -1;
	}

	return ret;
}

const char *nftnl_verdict2str(uint32_t verdict)
{
	switch (verdict) {
	case NF_ACCEPT:
		return "accept";
	case NF_DROP:
		return "drop";
	case NF_STOLEN:
		return "stolen";
	case NF_QUEUE:
		return "queue";
	case NF_REPEAT:
		return "repeat";
	case NF_STOP:
		return "stop";
	case NFT_RETURN:
		return "return";
	case NFT_JUMP:
		return "jump";
	case NFT_GOTO:
		return "goto";
	case NFT_CONTINUE:
		return "continue";
	case NFT_BREAK:
		return "break";
	default:
		return "unknown";
	}
}

int nftnl_str2verdict(const char *verdict, int *verdict_num)
{
	if (strcmp(verdict, "accept") == 0) {
		*verdict_num = NF_ACCEPT;
		return 0;
	} else if (strcmp(verdict, "drop") == 0) {
		*verdict_num = NF_DROP;
		return 0;
	} else if (strcmp(verdict, "return") == 0) {
		*verdict_num = NFT_RETURN;
		return 0;
	} else if (strcmp(verdict, "jump") == 0) {
		*verdict_num = NFT_JUMP;
		return 0;
	} else if (strcmp(verdict, "goto") == 0) {
		*verdict_num = NFT_GOTO;
		return 0;
	}

	return -1;
}

enum nftnl_cmd_type nftnl_flag2cmd(uint32_t flags)
{
	if (flags & NFTNL_OF_EVENT_NEW)
		return NFTNL_CMD_ADD;
	else if (flags & NFTNL_OF_EVENT_DEL)
		return NFTNL_CMD_DELETE;

	return NFTNL_CMD_UNSPEC;
}

static const char *cmd2tag[NFTNL_CMD_MAX] = {
	[NFTNL_CMD_ADD]			= ADD,
	[NFTNL_CMD_INSERT]		= INSERT,
	[NFTNL_CMD_DELETE]		= DELETE,
	[NFTNL_CMD_REPLACE]		= REPLACE,
	[NFTNL_CMD_FLUSH]			= FLUSH,
};

const char *nftnl_cmd2tag(enum nftnl_cmd_type cmd)
{
	if (cmd >= NFTNL_CMD_MAX)
		return "unknown";

	return cmd2tag[cmd];
}

uint32_t nftnl_str2cmd(const char *cmd)
{
	if (strcmp(cmd, ADD) == 0)
		return NFTNL_CMD_ADD;
	else if (strcmp(cmd, INSERT) == 0)
		return NFTNL_CMD_INSERT;
	else if (strcmp(cmd, DELETE) == 0)
		return NFTNL_CMD_DELETE;
	else if (strcmp(cmd, REPLACE) == 0)
		return NFTNL_CMD_REPLACE;
	else if (strcmp(cmd, FLUSH) == 0)
		return NFTNL_CMD_FLUSH;

	return NFTNL_CMD_UNSPEC;
}

int nftnl_fprintf(FILE *fp, const void *obj, uint32_t cmd, uint32_t type,
		  uint32_t flags,
		  int (*snprintf_cb)(char *buf, size_t bufsiz, const void *obj,
				     uint32_t cmd, uint32_t type,
				     uint32_t flags))
{
	char _buf[NFTNL_SNPRINTF_BUFSIZ];
	char *buf = _buf;
	size_t bufsiz = sizeof(_buf);
	int ret;

	ret = snprintf_cb(buf, bufsiz, obj, cmd, type, flags);
	if (ret <= 0)
		goto out;

	if (ret >= NFTNL_SNPRINTF_BUFSIZ) {
		bufsiz = ret + 1;

		buf = malloc(bufsiz);
		if (buf == NULL)
			return -1;

		ret = snprintf_cb(buf, bufsiz, obj, cmd, type, flags);
		if (ret <= 0)
			goto out;
	}

	ret = fprintf(fp, "%s", buf);

out:
	if (buf != _buf)
		xfree(buf);

	return ret;
}

void __nftnl_assert_attr_exists(uint16_t attr, uint16_t attr_max,
				const char *filename, int line)
{
	fprintf(stderr, "libnftnl: attribute %d > %d (maximum) assertion failed in %s:%d\n",
		attr, attr_max, filename, line);
	exit(EXIT_FAILURE);
}

void __nftnl_assert_fail(uint16_t attr, const char *filename, int line)
{
	fprintf(stderr, "libnftnl: attribute %d assertion failed in %s:%d\n",
		attr, filename, line);
	exit(EXIT_FAILURE);
}

void __noreturn __abi_breakage(const char *file, int line, const char *reason)
{
       fprintf(stderr, "nf_tables kernel ABI is broken, contact your vendor.\n"
		       "%s:%d reason: %s\n", file, line, reason);
       exit(EXIT_FAILURE);
}
