/*
 *  megatools - Mega.nz client library and tools
 *  Copyright (C) 2013  Ondřej Jirman <megous@megous.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __MEGA_HTTP_H
#define __MEGA_HTTP_H

#include <glib.h>

#define HTTP_ERROR http_error_quark()

enum {
	HTTP_ERROR_NO_RESPONSE,
	HTTP_ERROR_TIMEOUT,
	HTTP_ERROR_SERVER_BUSY,
	HTTP_ERROR_COMM_FAILURE,
	HTTP_ERROR_BANDWIDTH_LIMIT,
	HTTP_ERROR_OTHER
};

typedef gsize (*http_data_fn)(gpointer buf, gsize len, gpointer user_data);
typedef gboolean (*http_progress_fn)(goffset dltotal, goffset dlnow, goffset ultotal, goffset ulnow,
				     gpointer user_data);

// functions

struct http *http_new(void);

void http_set_max_connects(struct http *h, long max);
void http_expect_short_running(struct http *h);
void http_set_content_type(struct http *h, const gchar *type);
void http_set_content_length(struct http *h, goffset len);
void http_set_header(struct http *h, const gchar *name, const gchar *value);
void http_set_progress_callback(struct http *h, http_progress_fn cb, gpointer data);
void http_set_speed(struct http *h, gint max_ul, gint max_dl);
void http_set_proxy(struct http *h, const gchar *proxy);

GString *http_post(struct http *h, const gchar *url, const gchar *body, gssize body_len, GError **err);
GString *http_post_stream_upload(struct http *h, const gchar *url, goffset len, http_data_fn read_cb,
				 gpointer user_data, GError **err);
gboolean http_post_stream_download(struct http *h, const gchar *url, http_data_fn write_cb, gpointer user_data,
				   GError **err);

void http_free(struct http *h);

void http_cleanup(void);

GQuark http_error_quark(void);

#endif
