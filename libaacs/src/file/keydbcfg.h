/*
 * This file is part of libaacs
 * Copyright (C) 2010  gates
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef KEYDBCFG_H
#define KEYDBCFG_H

#include "util/attributes.h"

#include <stdint.h>
#include <stddef.h> /* size_t */

/* struct holding a digit and key pair for <ENTRY NUMBER> - <ENTRY> entries */
typedef struct digit_key_pair_t digit_key_pair;
struct digit_key_pair_t
{
  unsigned int digit;
  char *key;
};

/* list of digit_key_pair struct used in title entry */
typedef struct digit_key_pair_list_t digit_key_pair_list;
struct digit_key_pair_list_t
{
  digit_key_pair key_pair;
  digit_key_pair_list *next;
};

/* date entry struct */
#if 0
typedef struct date_entry_t date_entry;
struct date_entry_t
{
  unsigned int year;
  unsigned int month;
  unsigned int day;
};
#endif

/* dk entry */
typedef struct dk_entry dk_list;
struct dk_entry
{
  uint8_t key[16];
  unsigned long node;
  dk_list *next;

  /* optional, can be calculated */
  uint32_t uv;
  uint8_t  u_mask_shift;
};

/* pk entry */
typedef struct pk_entry pk_list;
struct pk_entry
{
  uint8_t key[16];
  pk_list *next;
};

/* certificate entry */
typedef struct cert_entry cert_list;
struct cert_entry
{
  uint8_t host_priv_key[20];
  uint8_t host_cert[92];
  cert_list *next;
};

/* title entry struct */
typedef struct title_entry_t title_entry;
struct title_entry_t
{
  uint8_t discid[20];
  //char *title;
  //date_entry date;
  char *mek;
  char *vid;
  //digit_key_pair_list *bn;
  uint8_t vuk[16];
  //digit_key_pair_list *pak;
  //digit_key_pair_list *tk;
  digit_key_pair_list *uk;
};

/* main struct for title entries, held in a list structure */
typedef struct title_entry_list_t title_entry_list;
struct title_entry_list_t
{
  title_entry entry;
  title_entry_list *next;
};

/* struct representing the contents of a config file */
typedef struct config_file_t config_file;
struct config_file_t
{
  dk_list *dkl;
  pk_list *pkl;
  cert_list *host_cert_list;
  title_entry_list *list;
};

/* Functions used throughout the parser */
BD_PRIVATE int keydbcfg_parse_config(config_file *cfgfile, const char *path);
BD_PRIVATE config_file *keydbcfg_new_config_file(void);
BD_PRIVATE int keydbcfg_config_file_close(config_file *cfgfile);

/* */

BD_PRIVATE config_file *keydbcfg_config_load(const char *configfile_path);

BD_PRIVATE int   keycache_save(const char *type, const uint8_t *disc_id,
                                 const uint8_t *key, unsigned int len);
BD_PRIVATE int   keycache_find(const char *type, const uint8_t *disc_id,
                                 uint8_t *key, unsigned int len);

BD_PRIVATE int cache_get(const char *name, uint32_t *version, uint32_t *len, void *buf, size_t buf_size); /* use buf=NULL to get version and size */
BD_PRIVATE int cache_save(const char *name, uint32_t version, const void *data, uint32_t len);
BD_PRIVATE int cache_remove(const char *name);

BD_PRIVATE int config_get(const char *name, uint32_t *len, void *buf); /* use buf=NULL to get size */
BD_PRIVATE int config_save(const char *name, const void *data, uint32_t len);

#endif
