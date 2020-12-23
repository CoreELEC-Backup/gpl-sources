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

#include "status.h"
#include "charset.h"
#include "util.h"
#include "mpc.h"

#include <mpd/client.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>

static unsigned
elapsed_percent(const struct mpd_status *status)
{
	unsigned total = mpd_status_get_total_time(status);
	if (total == 0)
		return 0;

	unsigned elapsed = mpd_status_get_elapsed_time(status);
	if (elapsed >= total)
		return 100;

	return (elapsed * 100) / total;
}

void
print_status(struct mpd_connection *conn)
{
	if (!mpd_command_list_begin(conn, true) ||
	    !mpd_send_status(conn) ||
	    !mpd_send_current_song(conn) ||
	    !mpd_command_list_end(conn))
		printErrorAndExit(conn);

	struct mpd_status *status = mpd_recv_status(conn);
	if (status == NULL)
		printErrorAndExit(conn);

	if (mpd_status_get_state(status) == MPD_STATE_PLAY ||
	    mpd_status_get_state(status) == MPD_STATE_PAUSE) {
		if (!mpd_response_next(conn))
			printErrorAndExit(conn);

		struct mpd_song *song = mpd_recv_song(conn);
		if (song != NULL) {
			pretty_print_song(song);
			printf("\n");

			mpd_song_free(song);
		}

		if (mpd_status_get_state(status) == MPD_STATE_PLAY)
			printf("[playing]");
		else
			printf("[paused] ");

		printf(" #%i/%u %3i:%02i/%i:%02i (%u%%)\n",
		       mpd_status_get_song_pos(status) + 1,
		       mpd_status_get_queue_length(status),
		       mpd_status_get_elapsed_time(status) / 60,
		       mpd_status_get_elapsed_time(status) % 60,
		       mpd_status_get_total_time(status) / 60,
		       mpd_status_get_total_time(status) % 60,
		       elapsed_percent(status));
	}

	if (mpd_status_get_update_id(status) > 0)
		printf("Updating DB (#%u) ...\n",
		       mpd_status_get_update_id(status));

	if (mpd_status_get_volume(status) >= 0)
		printf("volume:%3i%c   ", mpd_status_get_volume(status), '%');
	else {
		printf("volume: n/a   ");
	}

	printf("repeat: ");
	if (mpd_status_get_repeat(status))
		printf("on    ");
	else printf("off   ");

	printf("random: ");
	if (mpd_status_get_random(status))
		printf("on    ");
	else printf("off   ");

	printf("single: ");
	if (mpd_status_get_single(status))
		printf("on    ");
	else printf("off   ");

	printf("consume: ");
	if (mpd_status_get_consume(status))
		printf("on \n");
	else printf("off\n");

	if (mpd_status_get_error(status) != NULL)
		printf("ERROR: %s\n",
		       charset_from_utf8(mpd_status_get_error(status)));

	mpd_status_free(status);

	my_finishCommand(conn);
}

