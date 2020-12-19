/*
 *
 *  Connection Manager
 *
 *  Copyright (C) 2007-2013  Intel Corporation. All rights reserved.
 *  Copyright (C) 2016  Yann E. MORIN <yann.morin.1998@free.fr>. All rights reserved.
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
#include <stdlib.h>
#include <string.h>
#include <execinfo.h>
#include <dlfcn.h>

#include "connman.h"

void print_backtrace(const char* program_path, const char* program_exec,
		unsigned int offset)
{
	void *frames[99];
	size_t n_ptrs;
	unsigned int i;
	int outfd[2], infd[2];
	int pathlen;
	pid_t pid;

	if (!program_exec)
		return;

	pathlen = strlen(program_path);

	n_ptrs = backtrace(frames, G_N_ELEMENTS(frames));
	if (n_ptrs < offset)
		return;

	if (pipe(outfd) < 0)
		return;

	if (pipe(infd) < 0) {
		close(outfd[0]);
		close(outfd[1]);
		return;
	}

	pid = fork();
	if (pid < 0) {
		close(outfd[0]);
		close(outfd[1]);
		close(infd[0]);
		close(infd[1]);
		return;
	}

	if (pid == 0) {
		close(outfd[1]);
		close(infd[0]);

		dup2(outfd[0], STDIN_FILENO);
		dup2(infd[1], STDOUT_FILENO);

		execlp("addr2line", "-C", "-f", "-e", program_exec, NULL);

		exit(EXIT_FAILURE);
	}

	close(outfd[0]);
	close(infd[1]);

	connman_error("++++++++ backtrace ++++++++");

	for (i = offset; i < n_ptrs - 1; i++) {
		Dl_info info;
		char addr[20], buf[PATH_MAX * 2];
		int len, written;
		char *ptr, *pos;

		dladdr(frames[i], &info);

		len = snprintf(addr, sizeof(addr), "%p\n", frames[i]);
		if (len < 0)
			break;

		written = write(outfd[1], addr, len);
		if (written < 0)
			break;

		len = read(infd[0], buf, sizeof(buf) - 1);
		if (len < 0)
			break;

		buf[len] = '\0';

		pos = strchr(buf, '\n');
		if (!pos) {
			connman_error("Error in backtrace format");
			break;
		}

		*pos++ = '\0';

		if (strcmp(buf, "??") == 0) {
			connman_error("#%-2u %p in %s", i - offset,
						frames[i], info.dli_fname);
			continue;
		}

		ptr = strchr(pos, '\n');
		if (!ptr) {
			connman_error("Error in backtrace format");
			break;
		}

		*ptr++ = '\0';

		if (strncmp(pos, program_path, pathlen) == 0)
			pos += pathlen + 1;

		connman_error("#%-2u %p in %s() at %s", i - offset,
						frames[i], buf, pos);
	}

	connman_error("+++++++++++++++++++++++++++");

	kill(pid, SIGTERM);

	close(outfd[1]);
	close(infd[0]);
}
