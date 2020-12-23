/*
 * Data functions
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "csa.h"
#include "camd.h"

void data_init(struct ts *ts) {
	memset(ts, 0, sizeof(struct ts));
	// Stream
	ts->pat	     = ts_pat_alloc();
	ts->curpat   = ts_pat_alloc();
	ts->genpat   = ts_pat_alloc();

	ts->cat      = ts_cat_alloc();
	ts->curcat   = ts_cat_alloc();

	ts->pmt      = ts_pmt_alloc();
	ts->curpmt   = ts_pmt_alloc();

	ts->sdt      = ts_sdt_alloc();
	ts->cursdt   = ts_sdt_alloc();

	ts->emm      = ts_privsec_alloc();
	ts->last_emm = ts_privsec_alloc();
	ts->tmp_emm  = ts_privsec_alloc();

	ts->ecm      = ts_privsec_alloc();
	ts->last_ecm = ts_privsec_alloc();
	ts->tmp_ecm  = ts_privsec_alloc();

	pidmap_clear(&ts->pidmap);
	pidmap_clear(&ts->cc);
	pidmap_clear(&ts->pid_seen);

	// Key
	memset(&ts->key, 0, sizeof(ts->key));
	ts->key.csakey = csa_key_alloc();
	gettimeofday(&ts->key.ts_keyset, NULL);

	// CAMD
	memset(&ts->camd, 0, sizeof(ts->camd));
	ts->camd.server_fd    = -1;
	ts->camd.service      = "2233";
	ts->camd.key          = &ts->key;
	ts->camd.user         = "user";
	ts->camd.pass         = "pass";
	strcpy(ts->camd.newcamd.hex_des_key, "0102030405060708091011121314");

	camd_proto_cs378x(&ts->camd.ops);

	// Config
	ts->syslog_port = 514;

	ts->ts_discont  = 1;
	ts->ecm_cw_log  = 1;

	ts->debug_level = 0;
	ts->req_CA_sys  = CA_CONAX;

	ts->process_ecm = 1;
	ts->process_emm = 0;
	ts->output_stream = 1;

	ts->pid_filter  = 1;

	ts->emm_report_interval = 60;
	ts->emm_last_report     = time(NULL);

	ts->ecm_report_interval = 60;
	ts->ecm_last_report     = time(NULL);

	ts->irdeto_ecm_idx         = 0;
	ts->irdeto_ecm_filter_type = IRDETO_FILTER_IDX;

	ts->cw_warn_sec = 60;
	ts->cw_last_warn= time(NULL);
	ts->cw_last_warn= ts->cw_last_warn + ts->cw_warn_sec;
	ts->key.ts      = time(NULL);

	ts->input.fd    = 0; // STDIN
	ts->input.type  = FILE_IO;

	ts->output.fd   = 1; // STDOUT
	ts->output.type = FILE_IO;
	ts->output.ttl  = 1;
	ts->output.tos  = -1;
	ts->output.v6_if_index = -1;

	ts->decode_buf  = cbuf_init((7 * csa_get_batch_size() * 188) * 16, "decode"); // ~658Kb
	ts->write_buf   = cbuf_init((7 * csa_get_batch_size() * 188) *  8, "write");  // ~324Kb

	ts->input_buffer= list_new("input");

	pthread_attr_init(&ts->thread_attr);
	size_t stack_size;
	pthread_attr_getstacksize(&ts->thread_attr, &stack_size);
	if (stack_size > THREAD_STACK_SIZE)
		pthread_attr_setstacksize(&ts->thread_attr, THREAD_STACK_SIZE);
}

void data_free(struct ts *ts) {
	ts_pat_free(&ts->pat);
	ts_pat_free(&ts->curpat);
	ts_pat_free(&ts->genpat);
	ts_cat_free(&ts->cat);
	ts_cat_free(&ts->curcat);
	ts_pmt_free(&ts->pmt);
	ts_pmt_free(&ts->curpmt);
	ts_sdt_free(&ts->sdt);
	ts_sdt_free(&ts->cursdt);
	ts_privsec_free(&ts->emm);
	ts_privsec_free(&ts->last_emm);
	ts_privsec_free(&ts->tmp_emm);
	ts_privsec_free(&ts->ecm);
	ts_privsec_free(&ts->last_ecm);
	ts_privsec_free(&ts->tmp_ecm);

	csa_key_free(&ts->key.csakey);

	cbuf_free(&ts->decode_buf);
	cbuf_free(&ts->write_buf);

	list_free(&ts->input_buffer, free, NULL);

	// glibc's crypt function allocates static buffer on first crypt() call.
	// Since newcamd uses crypt(), the result is saved in c->newcamd.crypt_passwd
	// and in order to avoid leaking 43 bytes of memory on exit which makes valgrind
	// unhappy it is a good idea to free the memory (ONCE!).
	FREE(ts->camd.newcamd.crypt_passwd);

	pthread_attr_destroy(&ts->thread_attr);
}
