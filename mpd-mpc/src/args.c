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

#include "args.h"
#include "charset.h"
#include "list.h"
#include "options.h"

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <sys/param.h>

int
stdinToArgArray(char ***array)
{
	List *list = makeList();
	char buffer[4096];

	while (fgets(buffer, sizeof(buffer), stdin)) {
		char *sp;
		if((sp = strchr(buffer,'\n'))) *sp = '\0';
		insertInListWithoutKey(list,strdup(buffer));
	}

	const unsigned size = list->numberOfNodes;
	*array = malloc((sizeof(char *))*size);
	unsigned i = 0;
	ListNode * node = list->firstNode;
	while(node) {
		(*array)[i++] = (char *)node->data;
		node = node->nextNode;
	}
	assert(i==size);

	freeList(list);

	return size;
}

void
free_pipe_array(unsigned max, char ** array)
{
	for (unsigned i = 0 ; i < max; ++i)
		free(array[i]);
}

bool
contains_absolute_path(unsigned argc, char **argv)
{
	for (unsigned i = 0; i < argc; ++i)
		if (argv[i][0] == '/')
			return true;

	return false;
}

gcc_pure
static bool
uri_has_scheme(const char *uri)
{
	return strstr(uri, "://") != NULL;
}

void
strip_trailing_slash(char *s)
{
	if (uri_has_scheme(s))
		/* strip slashes only if this string is relative to
		   the music directory; absolute URLs are not, for
		   sure */
		return;

	size_t len = strlen(s);

	if (len == 0)
		return;
	--len;

	if (s[len] == '/')
		s[len] = '\0';

	return;
}

int
get_boolean(const char *arg)
{
	static const struct _bool_table {
		const char * on;
		const char * off;
	} bool_table [] = {
		{ "on", "off" },
		{ "1", "0" },
		{ "true", "false" },
		{ "yes", "no" },
		{ .on = NULL }
	};

	for (unsigned i = 0; bool_table[i].on != NULL; ++i) {
		if (strcasecmp(arg,bool_table[i].on) == 0)
			return 1;
		else if (strcasecmp(arg,bool_table[i].off) == 0)
			return 0;
	}

	fprintf(stderr,"\"%s\" is not a boolean value: <",arg);

	for (unsigned i = 0; bool_table[i].on != NULL; ++i) {
		fprintf(stderr,"%s|%s%s", bool_table[i].on,
			bool_table[i].off,
			( bool_table[i+1].off ? "|" : ">\n"));
	}

	return -1;
}

bool
parse_int(const char *str, int *ret)
{
	char *test;
	int temp = strtol(str, &test, 10);

	if (*test != '\0')
		return false; /* failure */

	*ret = temp;
	return true; /* success */
}

bool
parse_float(const char *str, float *ret)
{
	char *test;
	float temp = strtof(str, &test);

	if (*test != '\0')
		return false; /* failure */

	*ret = temp;
	return true; /* success */
}

bool
parse_songnum(const char *str, int *ret)
{
	if (!str)
		return false;
	if (*str == '#')
		str++;

	char *endptr;
	int song = strtol(str, &endptr, 10);

	if (str == endptr || (*endptr != ')' && *endptr != '\0') || song < 1)
		return false;

	*ret = song;
	return true;
}

bool
parse_int_value_change(const char *str, struct int_value_change *ret)
{
	const size_t len = strlen(str);
	if (len < 1)
		return false;

	int relative = 0;
	if (*str == '+')
		relative = 1;
	else if (*str == '-')
		relative = -1;

	int change;
	if (!parse_int(str, &change))
		return false;

	ret->value = change;
	ret->is_relative = (relative != 0);
	return true;
}
