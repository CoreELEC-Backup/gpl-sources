/*
 * Copyright (C) 2009-2010 Paul Bender.
 *
 * This file is part of eventlircd.
 *
 * eventlircd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * eventlircd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with eventlircd.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Single Unix Specification Version 3 headers.
 */
#include <errno.h>        /* C89 */
#include <signal.h>       /* C89 */
#include <stdbool.h>      /* C99 */
#include <stdio.h>        /* C89 */
#include <stdlib.h>       /* C89 */
#include <string.h>       /* C89 */
#include <unistd.h>       /* POSIX */
#include <sys/select.h>   /* POSIX */
#include <syslog.h>       /* XSI */
/*
 * eventlircd headers.
 */
#include "input.h"
#include "lircd.h"
#include "monitor.h"

struct monitor_client {
	int fd;
	int (*handler)(void *id);
	void *id;
	struct monitor_client *next;
};

struct {
	struct monitor_client *client_list;
} eventlircd_monitor = {
	.client_list = NULL
};

static bool monitor_sigterm_active = false;
static int  monitor_sigterm_signal = 0;

static int monitor_client_close(struct monitor_client *client)
{
	if (client == NULL) {
		errno = EINVAL;
		return -1;
	}

	client->fd = -1;
	client->handler = NULL;
	client->id = NULL;

	return 0;
}

static int monitor_client_purge()
{
	struct monitor_client **client_ptr;
	struct monitor_client *client;
	int return_code;

	return_code = 0;

	client_ptr = &(eventlircd_monitor.client_list);
	while (*client_ptr != NULL) {
		client = *client_ptr;
		if (client->fd == -1) {
			*client_ptr = client->next;
			if (monitor_client_close(client) != 0) {
				free(client);
				return_code = -1;
			}
		} else {
			client_ptr = &((*client_ptr)->next);
		}
	}

	return return_code;
}

int monitor_client_remove(int fd)
{
	struct monitor_client *client;
	int return_code;

	return_code = 0;

	for (client = eventlircd_monitor.client_list ; client != NULL ; client = client->next) {
		if (client->fd == fd) {
			if (monitor_client_close(client) != 0) {
				return_code = -1;
			}
		}
	}
	if (monitor_client_purge() != 0) {
		return_code = -1;
	}

	return return_code;
}

int monitor_client_add(int fd, int (*handler)(void *id), void *id)
{
	struct monitor_client *client;

	if (fd < 0) {
		errno = EINVAL;
		return -1;
	}
	if (handler == NULL) {
		errno = EINVAL;
		return -1;
	}

	if ((client = (struct monitor_client *)malloc(sizeof(struct monitor_client))) == NULL) {
		syslog(LOG_ERR,
		       "failed to allocate memory for monitor client: %s\n",
		       strerror(errno));
		return -1;
	}

	client->fd = fd;
	client->handler = handler;
	client->id = id;

	client->next = eventlircd_monitor.client_list;
	eventlircd_monitor.client_list = client;

	return 0;
}

int monitor_exit()
{
	struct monitor_client *client;
	int return_code;

	return_code = 0;

	for (client = eventlircd_monitor.client_list ; client != NULL ; client = client->next) {
		if (monitor_client_close(client) != 0) {
			return_code = -1;
		}
	}
	if (monitor_client_purge() != 0) {
		return_code = -1;
	}

	return return_code;
}

int monitor_init()
{
	eventlircd_monitor.client_list = NULL;

	return 0;
}

static void monitor_sigterm_handler(int signal)
{
	if (monitor_sigterm_active == true) {
		return;
	}
	monitor_sigterm_active = true;
	monitor_sigterm_signal = signal;
}

int monitor_run()
{
	struct sigaction signal_action;
	struct monitor_client *client;
	fd_set fdset;
	int nfds;

	signal_action.sa_handler = monitor_sigterm_handler;
	sigfillset(&signal_action.sa_mask);
	signal_action.sa_flags=SA_RESTART;
	sigaction(SIGTERM, &signal_action, NULL);
	sigaction(SIGINT,  &signal_action, NULL);

	while (true) {
		if (monitor_sigterm_active == true) {
			input_exit();
			lircd_exit();
			monitor_exit();
			signal(monitor_sigterm_signal, SIG_DFL);
			raise(monitor_sigterm_signal);
			return 0;
		}

		FD_ZERO(&fdset);
		nfds = 0;
		for (client = eventlircd_monitor.client_list ; client != NULL ; client = client->next) {
			if (client->fd < 0) {
				continue;
			}
			FD_SET(client->fd, &fdset);
			if (nfds < client->fd) {
				nfds = client->fd;
			}
		}
		nfds++;

		if (select(nfds, &fdset, NULL, NULL, NULL) < 0) {
			if (errno == EINTR) {
				continue;
			}
			return -1;
		}

		for (client = eventlircd_monitor.client_list ; client != NULL ; client = client->next) {
			if (client->fd == -1) {
				continue;
			}
			if (FD_ISSET(client->fd, &fdset) == 0) {
				continue;
			}
			client->handler(client->id);
		}
	}
	return 0;
}
