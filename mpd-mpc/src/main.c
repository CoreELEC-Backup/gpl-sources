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

#include "list.h"
#include "charset.h"
#include "password.h"
#include "util.h"
#include "args.h"
#include "status.h"
#include "command.h"
#include "queue.h"
#include "output.h"
#include "sticker.h"
#include "tab.h"
#include "idle.h"
#include "message.h"
#include "mount.h"
#include "neighbors.h"
#include "search.h"
#include "mpc.h"
#include "options.h"

#include <mpd/client.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/param.h>

static const struct command {
	const char *command;
	const int min, max;   /* min/max arguments allowed, -1 = unlimited */
	int pipe;             /**
	                       * 1: implicit pipe read, `-' optional as argv[2]
	                       * 2: explicit pipe read, `-' needed as argv[2]
	                       */
	cmdhandler handler;
	const char *usage;

	/** NULL means they won't be shown in help */
	const char *help;
} mpc_table [] = {
	/* command,     min, max, pipe, handler,         usage, help */
	{"add",         0,   -1,  1,    cmd_add,         "<uri>", "Add a song to the queue"},
	{"crop",        0,   0,   0,    cmd_crop,        "", "Remove all but the currently playing song"},
	{ "current", 0, 0, 0, cmd_current,
	  "", "Show the currently playing song"},
	{"del",         0,   -1,  1,    cmd_del,         "<position>", "Remove a song from the queue"},
	{"play",        0,   1,   2,    cmd_play,        "[<position>]", "Start playing at <position>"},
	{"next",        0,   0,   0,    cmd_next,        "", "Play the next song in the queue"},
	{"prev",        0,   0,   0,    cmd_prev,        "", "Play the previous song in the queue"},
	{"pause",       0,   0,   0,    cmd_pause,       "", "Pauses the currently playing song"},
	{"pause-if-playing",
	                0,   0,   0,    cmd_pause_if_playing, "", "Pauses the currently playing song; exits with failure if not playing"},
	{"toggle",      0,   0,   0,    cmd_toggle,      "", "Toggles Play/Pause, plays if stopped"},
	{"cdprev",      0,   0,   0,    cmd_cdprev,      "", "Compact disk player-like previous command"},
	{"stop",        0,   0,   0,    cmd_stop,        "", "Stop playback"},
	{"seek",        1,   1,   0,    cmd_seek,        "[+-][HH:MM:SS]|<0-100>%", "Seeks to the specified position"},
	{"seekthrough", 1,   1,   0,    cmd_seek_through, "[+-][HH:MM:SS]", "Seeks by an amount of time within the song and playlist"},
	{"clear",       0,   0,   0,    cmd_clear,       "", "Clear the queue"},
	{"outputs",     0,   0,   0,    cmd_outputs,     "", "Show the current outputs"},
	{"enable",      1,   -1,  0,    cmd_enable,      "[only] <output # or name> [...]", "Enable output(s)"},
	{"disable",     1,   -1,  0,    cmd_disable,     "[only] <output # or name> [...]", "Disable output(s)"},
	{"toggleoutput", 1, -1,   0,    cmd_toggle_output, "<output # or name> [...]", "Toggle output(s)"},
#if LIBMPDCLIENT_CHECK_VERSION(2,14,0)
	{"outputset",   2,   2,   0,    cmd_outputset,   "<output # or name> <name>=<value>", "Set output attributes"},
#endif
	{ "queued",	0,   0,   0,    cmd_queued,  "", "Show the next queued song"},
	{"shuffle",     0,   0,   0,    cmd_shuffle,     "", "Shuffle the queue"},
	{"move",        2,   2,   0,    cmd_move,        "<from> <to>", "Move song in queue"},
        /* mv is an alias for move */
	{"mv",          2,   2,   0,    cmd_move,        "<from> <to>", NULL},
	{"playlist",    0,   1,   0,    cmd_playlist,    "[<playlist>]", "Print <playlist>"},
	{"listall",     0,   -1,  2,    cmd_listall,     "[<file>]", "List all songs in the music dir"},
	{"ls",          0,   -1,  2,    cmd_ls,          "[<directory>]", "List the contents of <directory>"},
	{"lsplaylists", 0,   -1,  2,    cmd_lsplaylists, "", "List currently available playlists"},
	{"load",        0,   -1,  1,    cmd_load,        "<file>", "Load <file> into the queue"},
	{"insert",      0,   -1,  1,    cmd_insert,      "<uri>", "Insert a song to the queue after the current track"},

	{"prio",        2,   -1,  2,    cmd_prio,        "<prio> <position/range> ...", "Change song priorities in the queue"},

	{"save",        1,   1,   0,    cmd_save,        "<file>", "Save a queue as <file>"},
	{"rm",          1,   1,   0,    cmd_rm,          "<file>", "Remove a playlist"},
	{"volume",      0,   1,   0,    cmd_volume,      "[+-]<num>", "Set volume to <num> or adjusts by [+-]<num>"},
	{"repeat",      0,   1,   0,    cmd_repeat,      "<on|off>", "Toggle repeat mode, or specify state"},
	{"random",      0,   1,   0,    cmd_random,      "<on|off>", "Toggle random mode, or specify state"},
	{"single",      0,   1,   0,    cmd_single,      "<on|off>", "Toggle single mode, or specify state"},
	{"consume",     0,   1,   0,    cmd_consume,     "<on|off>", "Toggle consume mode, or specify state"},
	{"search",      1,   -1,  0,    cmd_search,      "<type> <query>", "Search for a song"},
	{"searchadd",   1,   -1,  0,    cmd_searchadd,   "<type> <query>", "Search songs and add them to the queue"},
	{"find",        1,   -1,  0,    cmd_find,        "<type> <query>", "Find a song (exact match)"},
	{"findadd",     1,   -1,  0,    cmd_findadd,     "<type> <query>", "Find songs and add them to the queue"},
	{"searchplay",  1,   -1,  0,    cmd_searchplay,  "<pattern>", "Find and play a song in the queue"},
	{"list",        1,   -1,  0,    cmd_list,        "<type> [<type> <query>]", "Show all tags of <type>"},
	{"crossfade",   0,   1,   0,    cmd_crossfade,   "[<seconds>]", "Set and display crossfade settings"},
	{"clearerror",  0,   0,   0,    cmd_clearerror,  "", "Clear the current error"},
	{"mixrampdb",   0,   1,   0,    cmd_mixrampdb,   "[<dB>]", "Set and display mixrampdb settings"},
	{"mixrampdelay",0,   1,   0,    cmd_mixrampdelay,"[<seconds>]", "Set and display mixrampdelay settings"},
	{"update",      0,   -1,  2,    cmd_update,      "[<path>]", "Scan music directory for updates"},
	{"rescan",      0,   -1,  2,    cmd_rescan,      "[<path>]", "Rescan music directory (including unchanged files)"},
	{"sticker",     2,   -1,  0,    cmd_sticker,     "<uri> <get|set|list|delete|find> [args..]", "Sticker management"},
	{"stats",       0,   -1,  0,    cmd_stats,       "", "Display statistics about MPD"},
	{"version",     0,   0,   0,    cmd_version,     "", "Report version of MPD"},
	/* loadtab, lstab, and tab used for completion-scripting only */
	{"loadtab",     1,   1,   0,    cmd_loadtab,     "<directory>", NULL},
	{"lstab",       1,   1,   0,    cmd_lstab,       "<directory>", NULL},
	{"tab",         1,   1,   0,    cmd_tab,         "<path>", NULL},
	/* status was added for pedantic reasons */
	{"status",      0,   -1,  0,    cmd_status,      "", NULL},
	{ "idle", 0, -1, 0, cmd_idle, "[events]",
	  "Idle until an event occurs" },
	{ "idleloop", 0, -1, 0, cmd_idleloop, "[events]",
	  "Continuously idle until an event occurs" },
	{ "replaygain", 0, -1, 0, cmd_replaygain, "[off|track|album]",
	  "Set or display the replay gain mode" },

	{ "channels", 0, 0, 0, cmd_channels, "",
	  "List the channels that other clients have subscribed to." },
	{ "sendmessage", 2, 2, 0, cmd_sendmessage, "<channel> <message>",
	  "Send a message to the specified channel." },
	{ "waitmessage", 1, 1, 0, cmd_waitmessage, "<channel>",
	  "Wait for at least one message on the specified channel." },
	{ "subscribe", 1, 1, 0, cmd_subscribe, "<channel>",
	  "Subscribe to the specified channel and continuously receive messages." },

	{ "listneighbors", 0, 2, 0, cmd_listneighbors, "", "List neighbors." },

#if LIBMPDCLIENT_CHECK_VERSION(2,16,0)
	{ "mount", 0, 2, 0, cmd_mount, "[<uri> <storage>]", "List mounts or add a new mount." },
	{ "unmount", 1, 1, 0, cmd_unmount, "<uri>", "Remove a mount." },
#endif

	/* don't remove this, when mpc_table[i].command is NULL it will terminate the loop */
	{ .command = NULL }
};

static void
print_usage(FILE *outfp, const char *progname)
{
	fprintf(outfp,"Usage: %s [options] <command> [<arguments>]\n"
		"mpc version: "VERSION"\n",progname);
}

static int
print_help(const char *progname, const char *command)
{
	if (command && strcmp(command, "help")) {
		fprintf(stderr,"unknown command \"%s\"\n",command);
		print_usage(stderr, progname);
		fprintf(stderr,"See man 1 mpc or use 'mpc help' for more help.\n");
		return EXIT_FAILURE;
	}

	print_usage(stdout, progname);
	printf("\n");

	printf("Options:\n");
	print_option_help();
	printf("\n");

	printf("Commands:\n");

	unsigned max = 0;
	for (unsigned i = 0; mpc_table[i].command != NULL; ++i) {
		if (mpc_table[i].help) {
			unsigned tmp = strlen(mpc_table[i].command) +
				strlen(mpc_table[i].usage);
			max = (tmp > max) ? tmp : max;
		}
	}

	printf("  %s %*s  Display status\n",progname,max," ");

	for (unsigned i = 0; mpc_table[i].command != NULL; ++i) {
		if (!mpc_table[i].help)
			continue ;

		int spaces = max-(strlen(mpc_table[i].command)+strlen(mpc_table[i].usage));
		spaces += !spaces ? 0 : 1;

		printf("  %s %s %s%*s%s\n",progname,
			mpc_table[i].command,mpc_table[i].usage,
			spaces," ",mpc_table[i].help);

	}
	printf("\nSee man 1 mpc for more information about mpc commands and options\n");
	return EXIT_SUCCESS;
}

static struct mpd_connection *
setup_connection(void)
{
	struct mpd_connection *conn = mpd_connection_new(options.host, options.port, 0);
	if (conn == NULL) {
		fputs("Out of memory\n", stderr);
		exit(EXIT_FAILURE);
	}

	if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS)
		printErrorAndExit(conn);

	if(options.password)
		send_password(options.password, conn);

	return conn;
}

static const struct command *
find_command(const char *name)
{
	unsigned n_matches = 0;
	size_t name_length = strlen(name);
	const struct command *command = NULL;

	for (unsigned i = 0; mpc_table[i].command != NULL; ++i) {
		if (memcmp(name, mpc_table[i].command, name_length) == 0) {
			command = &mpc_table[i];
			++n_matches;

			if (command->command[name_length] == 0)
				/* exact match */
				return &mpc_table[i];
		}
	}

	return n_matches == 1
		? command
		: /* ambiguous or nonexistent */ NULL;
}

/**
 * Set to true when "argv" contains an allocated list which must be
 * freed before exiting.
 */
static bool pipe_array_used = false;

/* check arguments to see if they are valid */
static char **
check_args(const struct command *command, int * argc, char ** argv)
{
	char ** array;

	if ((command->pipe == 1 &&
		(2==*argc || (3==*argc && 0==strcmp(argv[2],STDIN_SYMBOL) )))
	    || (command->pipe == 2 && (3 == *argc &&
				       0 == strcmp(argv[2],STDIN_SYMBOL)))){
		*argc = stdinToArgArray(&array);
		pipe_array_used = true;
	} else {
		*argc -= 2;
		array = malloc( (*argc * (sizeof(char *))));
		for(int i = 0; i < *argc; ++i) {
			array[i]=argv[i+2];
		}
	}
	if ((-1 != command->min && *argc < command->min) ||
	    (-1 != command->max && *argc > command->max)) {
		fprintf(stderr,"usage: %s %s %s\n", argv[0], command->command,
			command->usage);
			exit (EXIT_FAILURE);
	}
	return array;
}

static int
run(const struct command *command, int argc, char **array)
{
	struct mpd_connection *conn = setup_connection();

	if (mpd_connection_cmp_server_version(conn, 0, 19, 0) < 0)
		fprintf(stderr, "warning: MPD 0.19 required\n");

	int ret = command->handler(argc, array, conn);
	if (ret != 0 && options.verbosity > V_QUIET) {
		print_status(conn);
	}

	mpd_connection_free(conn);
	return (ret >= 0) ? EXIT_SUCCESS : -ret;
}

int main(int argc, char ** argv)
{
	parse_options(&argc, argv);

	/* parse command and arguments */

	const char *command_name;
	if (argc >= 2)
		command_name = argv[1];
	else {
		command_name = "status";

		/* this is a hack, so check_args() won't complain; the
		   arguments won't we used anyway, so this is quite
		   safe */
		argc = 2;
	}

	const struct command *command = find_command(command_name);
	if (command == NULL)
		return print_help("mpc", argv[1]);

	argv = check_args(command, &argc, argv);

	/* initialization */

	charset_init(true, true);

	/* run */

	int ret = run(command, argc, argv);

	/* cleanup */

	charset_deinit();

	if (pipe_array_used)
		free_pipe_array(argc, argv);
	free(argv);

	return ret;
}
