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

#include "mount.h"
#include "util.h"
#include "charset.h"
#include "Compiler.h"

#include <mpd/client.h>

#include <stdio.h>

#if LIBMPDCLIENT_CHECK_VERSION(2,16,0)

static int
cmd_list_mounts(struct mpd_connection *connection)
{
	if (!mpd_send_list_mounts(connection))
		printErrorAndExit(connection);

	struct mpd_mount *m;
	while ((m = mpd_recv_mount(connection)) != NULL) {
		printf("%s\t", charset_from_utf8(mpd_mount_get_uri(m)));

		const char *storage = mpd_mount_get_storage(m);
		if (storage != NULL)
			storage = charset_from_utf8(storage);
		else
			storage = "[unknown]";

		printf("%s\n", storage);
		mpd_mount_free(m);
	}

	my_finishCommand(connection);
	return 0;
}

int
cmd_mount(int argc, char **argv,
	  struct mpd_connection *connection)
{
	if (argc == 0)
		return cmd_list_mounts(connection);

	if (argc != 2) {
		fprintf(stderr, "Usage: mount URI STORAGE\n");
		return -1;
	}

	if (!mpd_run_mount(connection, charset_to_utf8(argv[0]), argv[1]))
		printErrorAndExit(connection);

	return 0;
}

int
cmd_unmount(gcc_unused int argc, char **argv,
	    struct mpd_connection *connection)
{
	const char *uri = charset_to_utf8(argv[0]);
	if (!mpd_run_unmount(connection, uri))
		printErrorAndExit(connection);

	return 0;
}

#endif
