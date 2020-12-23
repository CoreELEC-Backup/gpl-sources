/*
 * music player command (mpc)
 * Copyright 2003-2018 The Music Player Daemon Project
 * http://www.musicpd.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "util.h"
#include "song_format.h"
#include "charset.h"
#include "list.h"
#include "options.h"

#include <mpd/client.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

void
printErrorAndExit(struct mpd_connection *conn)
{
	assert(mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS);

	const char *message = mpd_connection_get_error_message(conn);
	if (mpd_connection_get_error(conn) == MPD_ERROR_SERVER)
		/* messages received from the server are UTF-8; the
		   rest is either US-ASCII or locale */
		message = charset_from_utf8(message);

	fprintf(stderr, "MPD error: %s\n", message);
	mpd_connection_free(conn);
	exit(EXIT_FAILURE);
}

void
my_finishCommand(struct mpd_connection *conn)
{
	if (!mpd_response_finish(conn))
		printErrorAndExit(conn);
}

struct mpd_status *
getStatus(struct mpd_connection *conn)
{
	struct mpd_status *ret = mpd_run_status(conn);
	if (ret == NULL)
		printErrorAndExit(conn);

	return ret;
}

static void
print_formatted_song(const struct mpd_song *song, const char * format)
{
	char * str = format_song(song, format);

	if(str) {
		printf("%s", str);
		free(str);
	}
}

void
pretty_print_song(const struct mpd_song *song)
{
	print_formatted_song(song, options.format);
}

void
print_entity_list(struct mpd_connection *c, enum mpd_entity_type filter_type,
		  bool pretty)
{
	struct mpd_entity *entity;
	while ((entity = mpd_recv_entity(c)) != NULL) {
		const struct mpd_directory *dir;
		const struct mpd_song *song;
		const struct mpd_playlist *playlist;

		enum mpd_entity_type type = mpd_entity_get_type(entity);
		if (filter_type != MPD_ENTITY_TYPE_UNKNOWN &&
		    type != filter_type)
			type = MPD_ENTITY_TYPE_UNKNOWN;

		switch (type) {
		case MPD_ENTITY_TYPE_UNKNOWN:
			break;

		case MPD_ENTITY_TYPE_DIRECTORY:
			dir = mpd_entity_get_directory(entity);
			printf("%s\n", charset_from_utf8(mpd_directory_get_path(dir)));
			break;

		case MPD_ENTITY_TYPE_SONG:
			song = mpd_entity_get_song(entity);
			if (pretty) {
				pretty_print_song(song);
				puts("");
			} else
				printf("%s\n", charset_from_utf8(mpd_song_get_uri(song)));
			break;

		case MPD_ENTITY_TYPE_PLAYLIST:
			playlist = mpd_entity_get_playlist(entity);
			printf("%s\n", charset_from_utf8(mpd_playlist_get_path(playlist)));
			break;
		}

		mpd_entity_free(entity);
	}
}

void
print_filenames(struct mpd_connection *conn)
{
	struct mpd_song *song;

	while ((song = mpd_recv_song(conn)) != NULL) {
		printf("%s\n", charset_from_utf8(mpd_song_get_uri(song)));
		mpd_song_free(song);
	}

	if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS)
		printErrorAndExit(conn);
}
