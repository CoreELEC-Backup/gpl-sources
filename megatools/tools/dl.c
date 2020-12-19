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
#ifdef G_OS_WIN32
#include <io.h>
#include <fcntl.h>
#endif

static gchar *opt_path = ".";
static gboolean opt_stream = FALSE;
static gboolean opt_noprogress = FALSE;
static gboolean opt_print_names = FALSE;
static gboolean opt_choose_files = FALSE;

static GOptionEntry entries[] = {
	{ "path", '\0', 0, G_OPTION_ARG_FILENAME, &opt_path, "Local directory or file name, to save data to", "PATH" },
	{ "no-progress", '\0', 0, G_OPTION_ARG_NONE, &opt_noprogress, "Disable progress bar", NULL },
	{ "print-names", '\0', 0, G_OPTION_ARG_NONE, &opt_print_names, "Print names of downloaded files", NULL },
	{ "choose-files", '\0', 0, G_OPTION_ARG_NONE, &opt_choose_files, "Choose which files to download when downloading folders (interactive)", NULL },
	{ NULL }
};

static gchar *cur_file;
static struct mega_session *s;

static void status_callback(struct mega_status_data *data, gpointer userdata)
{
	if (opt_stream && data->type == MEGA_STATUS_DATA) {
		fwrite(data->data.buf, data->data.size, 1, stdout);
		fflush(stdout);
	}

	if (data->type == MEGA_STATUS_FILEINFO) {
		g_free(cur_file);
		cur_file = g_strdup(data->fileinfo.name);
	}

	if (!opt_noprogress && data->type == MEGA_STATUS_PROGRESS)
		tool_show_progress(cur_file, data);
}

// download operation

static gboolean dl_sync_file(struct mega_node *node, GFile *file)
{
	gc_error_free GError *local_err = NULL;
	gc_object_unref GFile *parent = g_file_get_parent(file);
	gc_free gchar *local_path = g_file_get_path(file);
	gc_free gchar *parent_path = g_file_get_path(parent);

	if (g_file_query_exists(file, NULL)) {
		g_printerr("ERROR: File already exists at %s\n", local_path);
		return FALSE;
	}

	if (!g_file_query_exists(parent, NULL)) {
		if (!g_file_make_directory_with_parents(parent, NULL, &local_err)) {
			g_printerr("ERROR: Can't create local directory %s: %s\n", parent_path, local_err->message);
			return FALSE;
		}
	} else {
		if (g_file_query_file_type(parent, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, NULL) != G_FILE_TYPE_DIRECTORY) {
			g_printerr("ERROR: Can't create local directory %s: a file exists there!\n", parent_path);
			return FALSE;
		}
	}

	if (!opt_noprogress)
		g_print("F %s\n", local_path);

	if (!mega_session_get(s, file, node, &local_err)) {
		if (!opt_noprogress && tool_is_stdout_tty())
			g_print("\r" ESC_CLREOL);
		gc_free gchar* remote_path = mega_node_get_path_dup(node);
		g_printerr("ERROR: Download failed for %s: %s\n", remote_path, local_err->message);
		return FALSE;
	}

	if (!opt_noprogress && tool_is_stdout_tty())
		g_print("\r" ESC_CLREOL);

	if (opt_print_names)
		g_print("%s\n", local_path);

	return TRUE;
}

static gboolean dl_sync_dir(struct mega_node *node, GFile *file)
{
	gc_error_free GError *local_err = NULL;
	gc_free gchar *local_path = g_file_get_path(file);

	// sync children
	GSList *children = mega_session_get_node_chilren(s, node), *i;
	gboolean status = TRUE;
	for (i = children; i; i = i->next) {
		struct mega_node *child = i->data;
		gc_object_unref GFile *child_file = g_file_get_child(file, child->name);

		if (child->type == MEGA_NODE_FILE) {
			if (!dl_sync_file(child, child_file))
				status = FALSE;
		} else {
			if (!dl_sync_dir(child, child_file))
				status = FALSE;
		}
	}

	g_slist_free(children);
	return status;
}

static gint* parse_number_list(const gchar* input, gint* count)
{
	*count = 0;

	gc_regex_unref GRegex* re = g_regex_new("\\s+", 0, 0, NULL);
	if (re == NULL)
		return NULL;

	gchar** tokens = g_regex_split(re, input, 0);
	guint i = 0, n_tokens = g_strv_length(tokens);
	gint* nums = g_new0(gint, n_tokens);

	*count = 0;
        while (i < n_tokens) {
		if (g_regex_match_simple("^\\d{1,7}$", tokens[i], 0, 0)) {
			nums[*count] = atoi(tokens[i]);
			*count += 1;
		} else {
			g_printerr("WARNING: Skipping non-numeric value '%s'\n", tokens[i]);
		}

		i++;
	}

	g_strfreev(tokens);
	return nums;
}

static GSList *prompt_and_filter_nodes(GSList *nodes)
{
	GSList *chosen_nodes = NULL;

        gc_free gchar* input = tool_prompt_input();
	if (input == NULL)
		return g_slist_copy(nodes);

	if (g_regex_match_simple("all", input, G_REGEX_CASELESS, 0))
		return g_slist_copy(nodes);

	gint n_nums;
	gc_free gint* nums = parse_number_list(input, &n_nums);

	gint max_index = g_slist_length(nodes);

	for (int i = 0; i < n_nums; i++) {
		if (nums[i] <= max_index && nums[i] >= 1) {
			struct mega_node* node = g_slist_nth_data(nodes, nums[i] - 1);

			if (!g_slist_find(chosen_nodes, node))
				chosen_nodes = g_slist_prepend(chosen_nodes, node);
			else
				g_printerr("WARNING: Index %d given multiple times\n", nums[i]);
		} else
			g_printerr("WARNING: Index %d out of range\n", nums[i]);
	}

	return chosen_nodes;
}

static gint compare_node(struct mega_node *a, struct mega_node *b)
{
	gchar path1[4096];
	gchar path2[4096];

	if (mega_node_get_path(a, path1, 4096) && mega_node_get_path(b, path2, 4096)) {
		if (mega_node_is_container(a)) {
			int pos = strlen(path1);
			if(pos < 4095) {
				path1[pos] = '/';
				path1[pos+1] = '\0';
			}
		}
		if (mega_node_is_container(b)) {
			int pos = strlen(path2);
			if(pos < 4095) {
				path2[pos] = '/';
				path2[pos+1] = '\0';
			}
		}
		return strcmp(path1, path2);
	}
	return 0;
}

GSList* prune_children(GSList* nodes)
{
	GSList* pruned = NULL, *it, *it2;

	// not the most optimal, but the working set is small
	for (it = nodes; it; it = it->next) {
		struct mega_node *node = it->data;

		// check if there's ancestor of node in the list
		for (it2 = nodes; it2; it2 = it2->next) {
			struct mega_node *node2 = it2->data;

			if (mega_node_has_ancestor(node, node2)) {
				gchar path[4096];
				if (mega_node_get_path(node, path, sizeof path))
					g_printerr("WARNING: skipping already included path %s\n", path);

				goto prune_node;
			}
		}

		pruned = g_slist_prepend(pruned, node);

prune_node:;
	}

	g_slist_free(nodes);

	return g_slist_reverse(pruned);
}

GSList* pick_nodes(void)
{
	GSList *nodes = mega_session_ls(s, "/", TRUE), *it, *chosen_nodes;
	int position = 2;

	nodes = g_slist_sort(nodes, (GCompareFunc)compare_node);

	struct mega_node *parent = nodes->data;
	int indent = 0;

	g_print("1. %s/\n", ((struct mega_node*)nodes->data)->name);

	for (it = nodes->next; it; it = it->next) {
		struct mega_node *node = it->data;
		while(parent != node->parent && indent != 0) {
			indent--;
			parent = parent->parent;
		}

		g_print("%*s" ESC_GRAY "|--" ESC_NORMAL "%d. %s", indent*3, "", position, node->name);
		if (mega_node_is_container(node)) {
			g_print("/");
			indent++;
			parent = node;
		}
		g_print("\n");

		position++;
	}

	g_print("Enter numbers of files or folders to download separated by spaces (or type 'all' to download everything):\n> ");

	chosen_nodes = prompt_and_filter_nodes(nodes);

	g_slist_free(nodes);

	// now, user may have chosen both directories and files contained in
	// them, prune the list

	chosen_nodes = g_slist_sort(chosen_nodes, (GCompareFunc)compare_node);

	return prune_children(chosen_nodes);
}

static gboolean dl_sync_dir_choose(GFile *local_dir)
{
	gc_error_free GError *local_err = NULL;
	GSList* chosen_nodes = pick_nodes(), *it;
	gboolean status = TRUE;

	if (chosen_nodes == NULL)
		g_printerr("WARNING: Nothing was selected\n");

	for (it = chosen_nodes; it; it = it->next) {
		struct mega_node *node = it->data;
		gchar remote_path[4096];
		if (!mega_node_get_path(node, remote_path, sizeof remote_path))
			continue;

		gc_object_unref GFile *file = g_file_get_child(local_dir, remote_path + 1);

		if (node->type == MEGA_NODE_FILE) {
			if (!dl_sync_file(node, file))
				status = FALSE;
		} else {
			if (!dl_sync_dir(node, file))
				status = FALSE;
		}
	}

	g_slist_free(chosen_nodes);
	return status;
}

int main(int ac, char *av[])
{
	gc_error_free GError *local_err = NULL;
	gc_regex_unref GRegex *file_regex = NULL, *folder_regex = NULL;
	gc_regex_unref GRegex *file_regex2 = NULL, *folder_regex2 = NULL;;
	gint i;
	int status = 0;

	tool_init(&ac, &av, "- download exported files from mega.nz", entries,
		  TOOL_INIT_AUTH_OPTIONAL | TOOL_INIT_DOWNLOAD_OPTS);

	if (!strcmp(opt_path, "-")) {
		opt_noprogress = opt_stream = TRUE;

		// see https://github.com/megous/megatools/issues/38
#ifdef G_OS_WIN32
		setmode(fileno(stdout), O_BINARY);
#endif
	}

	if (ac < 2) {
		g_printerr("ERROR: No links specified for download!\n");
		tool_fini(NULL);
		return 1;
	}

	if (opt_stream && ac != 2) {
		g_printerr("ERROR: Can't stream from multiple files!\n");
		tool_fini(NULL);
		return 1;
	}

	// prepare link parsers

	file_regex = g_regex_new("^https?://mega(?:\\.co)?\\.nz/#!([a-z0-9_-]{8})!([a-z0-9_-]{43})$", G_REGEX_CASELESS,
				 0, NULL);
	g_assert(file_regex != NULL);

	file_regex2 = g_regex_new("^https?://mega\\.nz/file/([a-z0-9_-]{8})#([a-z0-9_-]{43})$", G_REGEX_CASELESS,
                                  0, NULL);
	g_assert(file_regex2 != NULL);

	folder_regex =
		g_regex_new("^https?://mega(?:\\.co)?\\.nz/#F!([a-z0-9_-]{8})!([a-z0-9_-]{22})(![a-z0-9_-]{8})?$",
			    G_REGEX_CASELESS, 0, NULL);
	g_assert(folder_regex != NULL);

	folder_regex2 =
		g_regex_new("^https?://mega\\.nz/folder/([a-z0-9_-]{8})#([a-z0-9_-]{22})$",
			    G_REGEX_CASELESS, 0, NULL);
	g_assert(folder_regex2 != NULL);

	// create session

	s = tool_start_session(TOOL_SESSION_OPEN | TOOL_SESSION_AUTH_ONLY | TOOL_SESSION_AUTH_OPTIONAL);
	if (!s) {
		tool_fini(NULL);
		return 1;
	}

	mega_session_watch_status(s, status_callback, NULL);

	// process links
	for (i = 1; i < ac; i++) {
		gc_match_info_unref GMatchInfo *m1 = NULL;
		gc_match_info_unref GMatchInfo *m2 = NULL;
		gc_free gchar *key = NULL;
		gc_free gchar *handle = NULL;
		gc_free gchar *specific = NULL;
		gc_free gchar *link = tool_convert_filename(av[i], FALSE);

		if (g_regex_match(file_regex, link, 0, &m1) ||
                    g_regex_match(file_regex2, link, 0, &m1)) {
			handle = g_match_info_fetch(m1, 1);
			key = g_match_info_fetch(m1, 2);

			// perform download
			if (!mega_session_dl_compat(s, handle, key, opt_stream ? NULL : opt_path, &local_err)) {
				if (!opt_noprogress && tool_is_stdout_tty())
					g_print("\r" ESC_CLREOL "\n");
				g_printerr("ERROR: Download failed for '%s': %s\n", link, local_err->message);
				g_clear_error(&local_err);
				status = 1;
			} else {
				if (!opt_noprogress) {
					if (tool_is_stdout_tty())
						g_print("\r" ESC_CLREOL);
					g_print("Downloaded %s\n", cur_file);
				}

				if (opt_print_names)
					g_print("%s\n", cur_file);
			}
		} else if (g_regex_match(folder_regex, link, 0, &m2) ||
                           g_regex_match(folder_regex2, link, 0, &m2)) {
			if (opt_stream) {
				g_printerr("ERROR: Can't stream from a directory!\n");
				tool_fini(s);
				return 1;
			}

			handle = g_match_info_fetch(m2, 1);
			key = g_match_info_fetch(m2, 2);
			specific = g_match_info_fetch(m2, 3);

			// remove first char of |specific| since it's an '!'
			if (specific)
				memmove(&specific[0], &specific[1], strlen(specific));

			// perform download
			if (!mega_session_open_exp_folder(s, handle, key, specific, &local_err)) {
				g_printerr("ERROR: Can't open folder '%s': %s\n", link, local_err->message);
				g_clear_error(&local_err);
				status = 1;
			} else {
				mega_session_watch_status(s, status_callback, NULL);

				GSList *l = mega_session_ls(s, "/", FALSE);
				if (g_slist_length(l) == 1) {
					struct mega_node *root_node = l->data;

					gc_object_unref GFile *local_dir = g_file_new_for_path(opt_path);
					if (g_file_query_file_type(local_dir, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
								   NULL) == G_FILE_TYPE_DIRECTORY) {
						if (opt_choose_files) {
							if (!dl_sync_dir_choose(local_dir))
								status = 1;
						} else {
							if (!dl_sync_dir(root_node, local_dir))
								status = 1;
						}
					} else {
						g_printerr("ERROR: %s must be a directory\n", opt_path);
						status = 1;
					}
				} else {
					g_printerr("ERROR: EXP folder fs has multiple toplevel nodes? Weird!\n");
					status = 1;
				}

				g_slist_free(l);
			}
		} else {
			g_printerr("WARNING: Skipping invalid Mega download link: %s\n", link);
		}
	}

	tool_fini(s);
	return status;
}
