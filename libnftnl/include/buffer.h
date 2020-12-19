#ifndef _NFTNL_BUFFER_H_
#define _NFTNL_BUFFER_H_

#include <stdint.h>
#include <stdbool.h>

struct nftnl_expr;

struct nftnl_buf {
	char		*buf;
	size_t		size;
	size_t		len;
	uint32_t	off;
	bool		fail;
};

#define NFTNL_BUF_INIT(__b, __buf, __len)			\
	struct nftnl_buf __b = {				\
		.buf	= __buf,			\
		.len	= __len,			\
	};

int nftnl_buf_update(struct nftnl_buf *b, int ret);
int nftnl_buf_done(struct nftnl_buf *b);

union nftnl_data_reg;

int nftnl_buf_open(struct nftnl_buf *b, int type, const char *tag);
int nftnl_buf_close(struct nftnl_buf *b, int type, const char *tag);

int nftnl_buf_open_array(struct nftnl_buf *b, int type, const char *tag);
int nftnl_buf_close_array(struct nftnl_buf *b, int type, const char *tag);

int nftnl_buf_u32(struct nftnl_buf *b, int type, uint32_t value, const char *tag);
int nftnl_buf_s32(struct nftnl_buf *b, int type, uint32_t value, const char *tag);
int nftnl_buf_u64(struct nftnl_buf *b, int type, uint64_t value, const char *tag);
int nftnl_buf_str(struct nftnl_buf *b, int type, const char *str, const char *tag);
int nftnl_buf_reg(struct nftnl_buf *b, int type, union nftnl_data_reg *reg,
		int reg_type, const char *tag);
int nftnl_buf_expr_open(struct nftnl_buf *b, int type);
int nftnl_buf_expr_close(struct nftnl_buf *b, int type);
int nftnl_buf_expr(struct nftnl_buf *b, int type, uint32_t flags,
		   struct nftnl_expr *expr);

#define BASE			"base"
#define BYTES			"bytes"
#define BURST			"burst"
#define CHAIN			"chain"
#define CODE			"code"
#define COMPAT_FLAGS		"compat_flags"
#define COMPAT_PROTO		"compat_proto"
#define CONSUMED		"consumed"
#define COUNT			"count"
#define DATA			"data"
#define DEVICE			"device"
#define DIR			"dir"
#define DREG			"dreg"
#define EXTHDR_TYPE		"exthdr_type"
#define FAMILY			"family"
#define FLAGS			"flags"
#define GROUP			"group"
#define HANDLE			"handle"
#define HOOKNUM			"hooknum"
#define KEY			"key"
#define LEN			"len"
#define LEVEL			"level"
#define MASK			"mask"
#define NAT_TYPE		"nat_type"
#define NAME			"name"
#define NUM			"num"
#define OFFSET			"offset"
#define OP			"op"
#define PACKETS			"packets"
#define PKTS			"pkts"
#define POLICY			"policy"
#define POSITION		"position"
#define PREFIX			"prefix"
#define PRIO			"prio"
#define QTHRESH			"qthreshold"
#define RATE			"rate"
#define RULE			"rule"
#define SET			"set"
#define SET_NAME		"set_name"
#define SIZE			"size"
#define SNAPLEN			"snaplen"
#define SREG_ADDR_MAX		"sreg_addr_max"
#define SREG_ADDR_MIN		"sreg_addr_min"
#define SREG_PROTO_MAX		"sreg_proto_max"
#define SREG_PROTO_MIN		"sreg_proto_min"
#define SREG_KEY		"sreg_key"
#define SREG_DATA		"sreg_data"
#define SREG_QNUM		"sreg_qnum"
#define SREG			"sreg"
#define TABLE			"table"
#define TOTAL			"total"
#define TYPE			"type"
#define UNIT			"unit"
#define USE			"use"
#define XOR			"xor"
#define ADD			"add"
#define INSERT			"insert"
#define DELETE			"delete"
#define REPLACE			"replace"
#define FLUSH			"flush"
#define MODULUS			"modulus"
#define SEED			"seed"
#define ID			"id"

#endif
