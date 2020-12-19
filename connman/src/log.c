/*
 *
 *  Connection Manager
 *
 *  Copyright (C) 2007-2013  Intel Corporation. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <dlfcn.h>
#include <signal.h>

#include "connman.h"

static const char *program_exec;
static const char *program_path;

/* This makes sure we always have a __debug section. */
CONNMAN_DEBUG_DEFINE(dummy);

/**
 * connman_info:
 * @format: format string
 * @Varargs: list of arguments
 *
 * Output general information
 */
void connman_info(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);

	vsyslog(LOG_INFO, format, ap);

	va_end(ap);
}

/**
 * connman_warn:
 * @format: format string
 * @Varargs: list of arguments
 *
 * Output warning messages
 */
void connman_warn(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);

	vsyslog(LOG_WARNING, format, ap);

	va_end(ap);
}

/**
 * connman_error:
 * @format: format string
 * @varargs: list of arguments
 *
 * Output error messages
 */
void connman_error(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);

	vsyslog(LOG_ERR, format, ap);

	va_end(ap);
}

/**
 * connman_debug:
 * @format: format string
 * @varargs: list of arguments
 *
 * Output debug message
 */
void connman_debug(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);

	vsyslog(LOG_DEBUG, format, ap);

	va_end(ap);
}

static void signal_handler(int signo)
{
	connman_error("Aborting (signal %d) [%s]", signo, program_exec);

	print_backtrace(program_path, program_exec, 2);

	exit(EXIT_FAILURE);
}

static void signal_setup(sighandler_t handler)
{
	struct sigaction sa;
	sigset_t mask;

	sigemptyset(&mask);
	sa.sa_handler = handler;
	sa.sa_mask = mask;
	sa.sa_flags = 0;
	sigaction(SIGBUS, &sa, NULL);
	sigaction(SIGILL, &sa, NULL);
	sigaction(SIGFPE, &sa, NULL);
	sigaction(SIGSEGV, &sa, NULL);
	sigaction(SIGABRT, &sa, NULL);
	sigaction(SIGPIPE, &sa, NULL);
}

extern struct connman_debug_desc __start___debug[];
extern struct connman_debug_desc __stop___debug[];

static gchar **enabled = NULL;

static bool is_enabled(struct connman_debug_desc *desc)
{
	int i;

	if (!enabled)
		return false;

	for (i = 0; enabled[i]; i++) {
		if (desc->name && g_pattern_match_simple(enabled[i],
							desc->name))
			return true;
		if (desc->file && g_pattern_match_simple(enabled[i],
							desc->file))
			return true;
	}

	return false;
}

void __connman_log_enable(struct connman_debug_desc *start,
					struct connman_debug_desc *stop)
{
	struct connman_debug_desc *desc;
	const char *name = NULL, *file = NULL;

	if (!start || !stop)
		return;

	for (desc = start; desc < stop; desc++) {
		if (desc->flags & CONNMAN_DEBUG_FLAG_ALIAS) {
			file = desc->file;
			name = desc->name;
			continue;
		}

		if (file || name) {
			if (g_strcmp0(desc->file, file) == 0) {
				if (!desc->name)
					desc->name = name;
			} else
				file = NULL;
		}

		if (is_enabled(desc))
			desc->flags |= CONNMAN_DEBUG_FLAG_PRINT;
	}
}

int __connman_log_init(const char *program, const char *debug,
		gboolean detach, gboolean backtrace,
		const char *program_name, const char *program_version)
{
	static char path[PATH_MAX];
	int option = LOG_NDELAY | LOG_PID;

	program_exec = program;
	program_path = getcwd(path, sizeof(path));

	if (debug)
		enabled = g_strsplit_set(debug, ":, ", 0);

	__connman_log_enable(__start___debug, __stop___debug);

	if (!detach)
		option |= LOG_PERROR;

	if (backtrace)
		signal_setup(signal_handler);

	openlog(basename(program), option, LOG_DAEMON);

	syslog(LOG_INFO, "%s version %s", program_name, program_version);

	return 0;
}

void __connman_log_cleanup(gboolean backtrace)
{
	syslog(LOG_INFO, "Exit");

	closelog();

	if (backtrace)
		signal_setup(SIG_DFL);

	g_strfreev(enabled);
}
