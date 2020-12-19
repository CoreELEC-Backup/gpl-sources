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

static gchar *opt_remote_path;
static gchar *opt_local_path;
static gboolean opt_download;
static gboolean opt_noprogress;
static gboolean opt_dryrun;
static gboolean opt_nofollow;
static struct mega_session *s;

static GOptionEntry entries[] = {
	{ "remote", 'r', 0, G_OPTION_ARG_STRING, &opt_remote_path, "Remote directory", "PATH" },
	{ "local", 'l', 0, G_OPTION_ARG_STRING, &opt_local_path, "Local directory", "PATH" },
	{ "download", 'd', 0, G_OPTION_ARG_NONE, &opt_download, "Download files from mega", NULL },
	{ "no-progress", '\0', 0, G_OPTION_ARG_NONE, &opt_noprogress, "Disable progress bar", NULL },
	{ "no-follow", '\0', 0, G_OPTION_ARG_NONE, &opt_nofollow, "Don't follow symbolic links", NULL },
	{ "dryrun", 'n', 0, G_OPTION_ARG_NONE, &opt_dryrun, "Don't perform any actual changes", NULL },
	{ NULL }
};

static gchar *cur_file = NULL;

static void status_callback(struct mega_status_data *data, gpointer userdata)
{
	if (!opt_noprogress && data->type == MEGA_STATUS_PROGRESS)
		tool_show_progress(cur_file, data);
}

// upload operation

static gboolean up_sync_file(GFile *root, GFile *file, const gchar *remote_path)
{
	GError *local_err = NULL;

	struct mega_node *node = mega_session_stat(s, remote_path);
	if (node) {
		g_printerr("ERROR: File already exists at %s\n", remote_path);
		return FALSE;
	}

	g_print("F %s\n", remote_path);

	if (!opt_dryrun) {
		g_free(cur_file);
		cur_file = g_file_get_basename(file);
		gc_free gchar* local_path = g_file_get_path(file);

		if (!mega_session_put_compat(s, remote_path, local_path, &local_err)) {
			if (!opt_noprogress && tool_is_stdout_tty())
				g_print("\r" ESC_CLREOL);

			g_printerr("ERROR: Upload failed for %s: %s\n", remote_path, local_err->message);
			g_clear_error(&local_err);
			return FALSE;
		}

		if (!opt_noprogress && tool_is_stdout_tty())
			g_print("\r" ESC_CLREOL);
	}

	return TRUE;
}

static gboolean up_sync_dir(GFile *root, GFile *file, const gchar *remote_path)
{
	GError *local_err = NULL;
	GFileInfo *i;

	if (root != file) {
		struct mega_node *node = mega_session_stat(s, remote_path);
		if (node && node->type == MEGA_NODE_FILE) {
			g_printerr("ERROR: File already exists at %s\n", remote_path);
			return FALSE;
		}

		if (!node) {
			g_print("D %s\n", remote_path);

			if (!opt_dryrun) {
				if (!mega_session_mkdir(s, remote_path, &local_err)) {
					g_printerr("ERROR: Can't create remote directory %s: %s\n", remote_path,
						   local_err->message);
					g_clear_error(&local_err);
					return FALSE;
				}
			}
		}
	}

	// sync children
	gc_object_unref GFileEnumerator *e =
		g_file_enumerate_children(file, "standard::*",
					  opt_nofollow ? G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS : G_FILE_QUERY_INFO_NONE,
					  NULL, &local_err);
	if (!e) {
		g_printerr("ERROR: Can't read local directory %s: %s\n", g_file_get_relative_path(root, file),
			   local_err->message);
		g_clear_error(&local_err);
		return FALSE;
	}

	gboolean status = TRUE;
	while ((i = g_file_enumerator_next_file(e, NULL, NULL))) {
		const gchar *name = g_file_info_get_name(i);
		gc_object_unref GFile *child = g_file_get_child(file, name);
		GFileType type = g_file_query_file_type(child, 0, NULL);
		gc_free gchar *child_remote_path = g_strconcat(remote_path, "/", name, NULL);

		if (type == G_FILE_TYPE_DIRECTORY) {
			if (!up_sync_dir(root, child, child_remote_path))
				status = FALSE;
		} else if (type == G_FILE_TYPE_REGULAR) {
			if (!up_sync_file(root, child, child_remote_path))
				status = FALSE;
		} else {
			gc_free gchar* rel_path = g_file_get_relative_path(root, file);

			g_printerr("WARNING: Skipping special file %s\n", rel_path);
		}

		g_object_unref(i);
	}

	return status;
}

// download operation

static gboolean dl_sync_file(struct mega_node *node, GFile *file, const gchar *remote_path)
{
	GError *local_err = NULL;
	gchar *local_path = g_file_get_path(file);

	if (g_file_query_exists(file, NULL)) {
		g_printerr("ERROR: File already exists at %s\n", local_path);
		return FALSE;
	}

	g_print("F %s\n", local_path);

	if (!opt_dryrun) {
		g_free(cur_file);
		cur_file = g_file_get_basename(file);

		if (!mega_session_get_compat(s, g_file_get_path(file), remote_path, &local_err)) {
			if (!opt_noprogress && tool_is_stdout_tty())
				g_print("\r" ESC_CLREOL);

			g_printerr("ERROR: Download failed for %s: %s\n", remote_path, local_err->message);
			g_clear_error(&local_err);
			return FALSE;
		}

		if (!opt_noprogress && tool_is_stdout_tty())
			g_print("\r" ESC_CLREOL);
	}

	return TRUE;
}

static gboolean dl_sync_dir(struct mega_node *node, GFile *file, const gchar *remote_path)
{
	GError *local_err = NULL;
	gc_free gchar *local_path = g_file_get_path(file);

	if (!g_file_query_exists(file, NULL)) {
		g_print("D %s\n", local_path);

		if (!opt_dryrun) {
			if (!g_file_make_directory(file, NULL, &local_err)) {
				g_printerr("ERROR: Can't create local directory %s: %s\n", local_path,
					   local_err->message);
				g_clear_error(&local_err);
				return FALSE;
			}
		}
	} else {
		if (g_file_query_file_type(file, 0, NULL) != G_FILE_TYPE_DIRECTORY) {
			g_printerr("ERROR: Can't create local directory %s: file exists\n", local_path);
			return FALSE;
		}
	}

	// sync children
	gboolean status = TRUE;
	GSList *children = mega_session_get_node_chilren(s, node), *i;
	for (i = children; i; i = i->next) {
		struct mega_node *child = i->data;
		gc_free gchar *child_remote_path = g_strconcat(remote_path, "/", child->name, NULL);
		gc_object_unref GFile *child_file = g_file_get_child(file, child->name);

		if (child->type == MEGA_NODE_FILE) {
			if (!dl_sync_file(child, child_file, child_remote_path))
				status = FALSE;
		} else {
			if (!dl_sync_dir(child, child_file, child_remote_path))
				status = FALSE;
		}
	}

	g_slist_free(children);
	return status;
}

// main program

int main(int ac, char *av[])
{
	gc_object_unref GFile *local_file = NULL;
	gint status = 0;

	tool_init(&ac, &av, "- synchronize local and remote mega.nz directories", entries,
		  TOOL_INIT_AUTH | TOOL_INIT_UPLOAD_OPTS | TOOL_INIT_DOWNLOAD_OPTS);

	if (!opt_local_path || !opt_remote_path) {
		g_printerr("ERROR: You must specify local and remote paths\n");
		goto err;
	}

	s = tool_start_session(TOOL_SESSION_OPEN);
	if (!s)
		goto err;

	mega_session_watch_status(s, status_callback, NULL);

	// check remote dir existence
	struct mega_node *remote_dir = mega_session_stat(s, opt_remote_path);
	if (!remote_dir) {
		g_printerr("ERROR: Remote directory not found %s\n", opt_remote_path);
		goto err;
	} else if (!mega_node_is_container(remote_dir)) {
		g_printerr("ERROR: Remote path must be a folder: %s\n", opt_remote_path);
		goto err;
	}

	// check local dir existence
	local_file = g_file_new_for_path(opt_local_path);

	if (opt_download) {
		if (!dl_sync_dir(remote_dir, local_file, opt_remote_path))
			goto err;
	} else {
		if (g_file_query_file_type(local_file, 0, NULL) != G_FILE_TYPE_DIRECTORY) {
			g_printerr("ERROR: Local directory not found %s\n", opt_local_path);
			goto err;
		}

		if (!up_sync_dir(local_file, local_file, opt_remote_path))
			status = 1;

		mega_session_save(s, NULL);
	}

	g_free(cur_file);
	tool_fini(s);
	return status;

err:
	g_free(cur_file);
	tool_fini(s);
	return 1;
}
