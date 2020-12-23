/*
 * Data definitions
 * Copyright (C) 2011 Unix Solutions Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License (COPYING file) for more details.
 *
 */
#ifndef DATA_H
#define DATA_H

#include <pthread.h>
#include <limits.h>
#include <stdbool.h>

#include <openssl/aes.h>
#include <openssl/des.h>
#include <openssl/md5.h>

#include "libfuncs/libfuncs.h"
#include "libtsfuncs/tsfuncs.h"

// 7 * 188
#define FRAME_SIZE 1316

// How much seconds to assume the key is valid
#define KEY_VALID_TIME 30

#define EMM_QUEUE_HARD_LIMIT 10000
#define EMM_QUEUE_SOFT_LIMIT 1000

#define ECM_QUEUE_HARD_LIMIT 10
#define ECM_QUEUE_SOFT_LIMIT 3

// 64k should be enough for everybody
#define THREAD_STACK_SIZE (64 * 1024)

struct notify {
	pthread_t	thread;				/* Thread handle */
	QUEUE		*notifications;		/* Notification queue */
	char		ident[512];			/* tsdecrypt ident (set by -i) */
	char		program[512];		/* What program to exec */
};

#define CODEWORD_LENGTH 16
#define BISSKEY_LENGTH 6

typedef void csakey_t;

struct key {
	uint8_t				cw[CODEWORD_LENGTH];
	csakey_t			*csakey;
	int					is_valid_cw;
	time_t					ts;				// At what time the key is set
	struct timeval			ts_keyset;		// At what time the key is set
};

// 4 auth header, 20 header size, 256 max data size, 16 potential padding
#define CAMD35_HDR_LEN (20)
#define CAMD35_BUF_LEN (4 + CAMD35_HDR_LEN + 256 + 16)

// When this limit is reached invalid_cw flag is set.
#define ECM_RECV_ERRORS_LIMIT 10

// When this limit is reached camd_reconnect is called.
#define EMM_RECV_ERRORS_LIMIT 100

struct camd;
struct ts;

enum msg_type { EMM_MSG, ECM_MSG };

struct camd_msg {
	enum msg_type	type;
	uint16_t		ca_id;
	uint16_t		service_id;
	uint8_t			data_len;
	uint8_t			data[255];
	struct ts		*ts;
};

enum camd_proto {
	CAMD_CS378X,
	CAMD_NEWCAMD,
};

struct camd_ops {
	char *ident;
	enum camd_proto proto;
	int (*connect)(struct camd *c);
	void (*disconnect)(struct camd *c);
	int (*reconnect)(struct camd *c);
	int (*do_emm)(struct camd *c, struct camd_msg *msg);
	int (*do_ecm)(struct camd *c, struct camd_msg *msg);
	int (*get_cw)(struct camd *c, uint16_t *ca_id, uint16_t *idx, uint8_t *cw);
};

struct cs378x {
	// cs378x private data
	uint8_t			buf[CAMD35_BUF_LEN];
	AES_KEY			aes_encrypt_key;
	AES_KEY			aes_decrypt_key;
	uint32_t		auth_token;
	uint16_t		msg_id;
};

#define DESKEY_LENGTH     28
#define NEWCAMD_MSG_SIZE  400
#define NEWCAMD_MAXPROV   32

typedef struct {
	DES_key_schedule ks1;
	DES_key_schedule ks2;
	uint8_t des_key[16];
} triple_des_t;

struct newcamd {
	// newcamd private data
	uint8_t			buf[NEWCAMD_MSG_SIZE];
	char			hex_des_key[DESKEY_LENGTH + 1];
	uint8_t			bin_des_key[DESKEY_LENGTH / 2];	// Decoded des_key
	triple_des_t	td_key;
	uint16_t		msg_id;
	// Initialized from CARD INFO command
	int				caid;
	uint8_t			ua[8];
	uint8_t			num_of_provs;
	uint8_t			provs_ident[NEWCAMD_MAXPROV][3];
	uint8_t			provs_id[NEWCAMD_MAXPROV][8];
	uint8_t			prov_ident_manual;
	char			*crypt_passwd;
};

struct camd {
	int				server_fd;
	char			*hostname;
	char			*service;
	char			*user;
	char			*pass;

	unsigned int	ecm_recv_errors; // Error counter, reset on successful send/recv
	unsigned int	emm_recv_errors; // Error counter, reset on successful send/recv

	unsigned int	no_reconnect;
	unsigned int	check_emm_errors;

	struct key		*key;
	unsigned int	constant_codeword; // The codeword is set on the command line once, no ecm processing is done.

	pthread_t		thread;
	QUEUE			*req_queue;
	QUEUE			*ecm_queue;
	QUEUE			*emm_queue;

	struct camd_ops	ops;
	struct cs378x	cs378x;
	struct newcamd	newcamd;
};

enum io_type {
	FILE_IO,
	NET_IO,
	WTF_IO
};

struct io {
	int					fd;
	enum io_type		type;
	char				*fname;
	char				*hostname;
	char				*service;
	// Used only for output
	int					ttl;
	int					tos;
	struct in_addr		intf;
	int					v6_if_index;
	// Used for input
	struct in_addr		isrc;
};

struct packet_buf {
	int64_t		time;
	uint8_t		data[188];
};

#define MAX_FILTERS     16
#define MAX_FILTER_NAME 32
#define MAX_FILTER_LEN  16

enum filter_action {
	FILTER_NO_MATCH   = 0,
	FILTER_ACCEPT_ALL = 1,
	FILTER_REJECT_ALL = 2,
	FILTER_ACCEPT     = 3,
	FILTER_REJECT     = 4,
};

enum filter_type {
	FILTER_TYPE_DATA = 0,				// Compare data at offset X
	FILTER_TYPE_MASK = 1,				// Compare data + mask
	FILTER_TYPE_LENGTH = 2,				// Compare section length /EMM[2]/)
};

struct filter {
	enum filter_action action;
	enum filter_type type;
	uint8_t		offset;					// Offset into EMM
	uint8_t		filter_len;				// Filter length
	uint8_t		data[MAX_FILTER_LEN];	// Data | Matched bytes
	uint8_t		mask[MAX_FILTER_LEN];	// Mask bytes
	char		name[MAX_FILTER_NAME];	// Filter name (default: NO_NAME)
};

struct chid {
	int			seen;
	uint16_t	chid;
};

#define MAX_PIDS 8192

struct ts {
	// Stream handling
	struct ts_pat		*pat, *curpat;
	struct ts_pat		*genpat;
	uint8_t				genpat_cc;
	struct ts_cat		*cat, *curcat;
	struct ts_pmt		*pmt, *curpmt;
	struct ts_sdt		*sdt, *cursdt;
	struct ts_privsec	*emm, *last_emm;
	struct ts_privsec	*ecm, *last_ecm;
	struct ts_privsec	*tmp_emm;
	struct ts_privsec	*tmp_ecm;
	uint16_t			pmt_pid;
	uint16_t			service_id;
	uint16_t			forced_service_id;
	uint16_t			emm_caid, emm_pid;
	uint16_t			ecm_caid, ecm_pid;
	uint16_t			forced_caid;
	uint16_t			forced_emm_pid;
	uint16_t			forced_ecm_pid;
	pidmap_t			pidmap;
	pidmap_t			cc; // Continuity counters
	pidmap_t			pid_seen;

	// Stats
	unsigned int		emm_input_count;
	unsigned int		emm_seen_count;
	unsigned int		emm_processed_count;
	unsigned int		emm_skipped_count;
	unsigned int		emm_report_interval;
	time_t				emm_last_report;

	unsigned int		ecm_seen_count;
	unsigned int		ecm_processed_count;
	unsigned int		ecm_duplicate_count;
	unsigned int		ecm_report_interval;
	time_t				ecm_last_report;

	unsigned int		cw_warn_sec;
	time_t				cw_last_warn;
	time_t				cw_next_warn;
	struct timeval		ecm_change_time;

	unsigned int		pid_report;
	unsigned int		pid_stats[MAX_PIDS];

	// CAMD handling
	struct key			key;
	struct camd			camd;

	// Config
	char				*ident;

	char				*syslog_host;
	int					syslog_port;
	int					syslog_active;
	int					syslog_remote;

	char				*pidfile;

	enum CA_system		req_CA_sys;

	bool				output_stream;		// Decode and output the decoded stream
	bool				process_ecm;		// Process ECM packets (and send them to CAMD)
	bool				process_emm;		// Process EMM packets (and send them to CAMD)

	int					pid_filter;
	int					eit_passthrough;
	int					tdt_passthrough;
	int					nit_passthrough;

	uint8_t				irdeto_ecm_idx;
	uint16_t			irdeto_ecm_chid;

	enum {
		IRDETO_FILTER_IDX,
		IRDETO_FILTER_CHID,
	} irdeto_ecm_filter_type;

	struct chid			irdeto_chid[0xff];
	uint8_t				irdeto_max_chids;

	int					ecm_cw_log;

	int					rtp_input;
	int					rtp_output;
	uint32_t			rtp_ssrc;
	uint16_t			rtp_seqnum;

	struct io			input;
	struct io			output;

	FILE				*input_dump_file;
	char				*input_dump_filename;

	int					debug_level;
	int					ts_discont;

	int					camd_stop;
	int					is_cw_error;
	int					no_output_on_error;

	int					threaded;

	pthread_attr_t		thread_attr;

	int					decode_stop;
	pthread_t			decode_thread;
	CBUF				*decode_buf;

	int					write_stop;
	pthread_t			write_thread;
	CBUF				*write_buf;

	struct notify		*notify;
	char				*notify_program;

	unsigned int		input_buffer_time;
	LIST				*input_buffer;

	int					emm_filters_num;
	struct filter		emm_filters[MAX_FILTERS];
};

void data_init(struct ts *ts);
void data_free(struct ts *ts);

#endif
