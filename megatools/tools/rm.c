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

#include "tools.h"

static GOptionEntry entries[] = { { NULL } };

int main(int ac, char *av[])
{
	gc_error_free GError *local_err = NULL;
	static struct mega_session *s;

	tool_init(&ac, &av, "- remove files from mega.nz", entries, TOOL_INIT_AUTH);

	if (ac < 2) {
		g_printerr("ERROR: No files specified for removal!\n");
		tool_fini(NULL);
		return 1;
	}

	s = tool_start_session(TOOL_SESSION_OPEN);
	if (!s) {
		tool_fini(NULL);
		return 1;
	}

	gint i, status = 0;
	for (i = 1; i < ac; i++) {
		gc_free gchar *path = tool_convert_filename(av[i], FALSE);

		if (!mega_session_rm(s, path, &local_err)) {
			g_printerr("ERROR: Can't remove %s: %s\n", path, local_err->message);
			g_clear_error(&local_err);
			status = 1;
		}
	}

	mega_session_save(s, NULL);

	tool_fini(s);
	return status;
}
