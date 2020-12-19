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
#include <fcntl.h>        /* POSIX */
#include <stdio.h>        /* C89 */
#include <stdlib.h>       /* C89 */
#include <sys/socket.h>   /* POSIX */
#include <sys/stat.h>     /* POSIX */
#include <sys/types.h>    /* POSIX */
#include <sys/un.h>       /* XSI */
#include <syslog.h>       /* XSI */
#include <unistd.h>       /* POSIX */
/*
 * Linux header files.
 */
#include <linux/input.h>  /* */
#include <linux/limits.h> /* */
/*
 * eventlircd headers.
 */
#include "lircd.h"
#include "monitor.h"

/*
 * The lircd_handler does not use the id parameter, so we need to let gcc's
 * -Wused know that it is ok.
 */
#ifdef UNUSED
# error cannot define UNUSED because it is already defined
#endif
#if defined(__GNUC__)
# define UNUSED(x) x __attribute__((unused))
#else
# define UNUSED(x) x
#endif

/*
 * The 'lircd' structure contains the information associated with the lircd
 * socket. In particular, it contains a linked list of 'lircd_client'
 * structures, each of which contains information associated with one connected
 * lirc client.
 */
struct lircd_client {
	int fd;
	struct lircd_client *next;
};

struct {
	int fd;
	char *path;
	mode_t mode;
	char *release_suffix;
	struct lircd_client *client_list;
} eventlircd_lircd = {
	.fd = -1,
	.path = NULL,
	.mode = 0,
	.release_suffix = NULL,
	.client_list = NULL
};

static int lircd_client_add()
{
	struct lircd_client *client;
	int flags;

	if (eventlircd_lircd.fd == -1) {
		return -1;
	}

	if ((client = (struct lircd_client *)malloc(sizeof(struct lircd_client))) == NULL) {
		syslog(LOG_ERR,
		       "failed to allocate memory for lircd client: %s\n",
		       strerror(errno));
		return -1;
	}

	client->fd = accept(eventlircd_lircd.fd, NULL, NULL);

	if  (client->fd < 0) {
		syslog(LOG_ERR,
		       "error during accept(): %s",
		       strerror(errno));
		free(client);
		return -1;
	}

	flags = fcntl(client->fd, F_GETFL);
	fcntl(client->fd, F_SETFL, flags | O_NONBLOCK);

	client->next = eventlircd_lircd.client_list;
	eventlircd_lircd.client_list = client;

	return 0;
}

static int lircd_handler(void* UNUSED(id))
{
	if (lircd_client_add() != 0) {
		return -1;
	}

	return 0;
}

static int lircd_client_close(struct lircd_client *client)
{
	if (client == NULL) {
		return -1;
	}

	if (client->fd >= 0) {
		shutdown(client->fd, 2);
		close(client->fd);
		client->fd = -1;
	}

	return 0;
}

static int lircd_client_purge()
{
	struct lircd_client **client_ptr;
	struct lircd_client *client;
	int return_code;

	return_code = 0;

	client_ptr = &(eventlircd_lircd.client_list);
	while (*client_ptr != NULL) {
		client = *client_ptr;
		if (client->fd == -1) {
			*client_ptr = client->next;
			if (lircd_client_close(client) != 0) {
				return_code = -1;
			}
			free(client);
		} else {
			client_ptr = &((*client_ptr)->next);
		}
	}
	return return_code;
}

int lircd_exit()
{
	struct lircd_client *client;
	int return_code;

	return_code = 0;

	if (eventlircd_lircd.fd >= 0) {
		if (monitor_client_remove(eventlircd_lircd.fd) != 0) {
			return_code = -1;
		}
		close(eventlircd_lircd.fd);
		eventlircd_lircd.fd = -1;
	}

	for (client = eventlircd_lircd.client_list ; client != NULL ; client = client->next) {
		if (lircd_client_close(client) != 0) {
			return_code = -1;
		}
	}
	if (lircd_client_purge() != 0) {
		return_code = -1;
	}

	if (eventlircd_lircd.path != NULL) {
		unlink(eventlircd_lircd.path);
		free(eventlircd_lircd.path);
		eventlircd_lircd.path = NULL;
	}

	return return_code;
}

int lircd_init(const char *path, mode_t mode, const char *release_suffix)
{
	struct sockaddr_un addr;

	eventlircd_lircd.fd = -1;
	eventlircd_lircd.path = NULL;
	eventlircd_lircd.mode = 0;
	eventlircd_lircd.release_suffix = NULL;
	eventlircd_lircd.client_list = NULL;

	if (path == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (strnlen(path, PATH_MAX + 1) >= PATH_MAX + 1) {
		errno = ENAMETOOLONG;
		syslog(LOG_ERR,
		       "lircd socket path: %s\n",
		       strerror(errno));
		return -1;
	}

	if ((eventlircd_lircd.path = strndup(path, PATH_MAX)) == NULL) {
		syslog(LOG_ERR,
		       "failed to allocate memory for the lircd device %s: %s\n",
		       path,
		       strerror(errno));
		lircd_exit();
		return -1;
	}

	eventlircd_lircd.mode = mode;

	if (release_suffix != NULL) {
		if ((eventlircd_lircd.release_suffix = strndup(release_suffix, 128)) == NULL) {
			syslog(LOG_ERR,
			       "failed to allocate memory for the lircd device %s: %s\n",
			       path,
			       strerror(errno));
			lircd_exit();
			return -1;
		}
	}

	if (unlink(eventlircd_lircd.path) != 0) {
		if (errno != ENOENT) {
			syslog(LOG_ERR,
			       "failed to remove existing lircd socket %s: %s\n",
			       path,
			       strerror(errno));
			lircd_exit();
			return -1;
		}
	}

	eventlircd_lircd.fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (eventlircd_lircd.fd < 0) {
		syslog(LOG_ERR,
		       "failed to create Unix socket for lircd output: %s\n",
		       strerror(errno));
		lircd_exit();
		return -1;
	}

	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, eventlircd_lircd.path);
	if(bind(eventlircd_lircd.fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		syslog(LOG_ERR,
		       "failed to bind to Unix socket needed for lircd output; %s\n",
		       strerror(errno));
		lircd_exit();
		return -1;
	}

	chmod(eventlircd_lircd.path, mode);

	if (listen(eventlircd_lircd.fd, 3) < 0) {
		syslog(LOG_ERR,
		       "failed to listen on Unix socket needed for lircd output: %s\n",
		       strerror(errno));
		lircd_exit();
		return -1;
	}

	if (monitor_client_add(eventlircd_lircd.fd, &lircd_handler, NULL) != 0) {
		syslog(LOG_ERR,
		       "failed to add lircd to the monitor client list: %s\n",
		       strerror(errno));
		lircd_exit();
		return -1;
	}

	return 0;
}

int lircd_send(const struct input_event *event, const char *name, unsigned int repeat_count, const char *remote)
{
	char message[1000];
	int message_len;
	struct lircd_client *client;

	if (event == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (name == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (remote == NULL) {
		errno = EINVAL;
		return -1;
	}

	if ((event->value == 0) && (eventlircd_lircd.release_suffix == NULL)) {
		return 0;
	}

	/*
	 * If this is a key release, so append the key release suffix.
	 */
	if (event->value == 0) {
		message_len = snprintf(message,
		                       sizeof message,
		                       "%x %x %s%s %s\n",
		                       (unsigned int)event->code,
		                       repeat_count,
		                       name,
		                       eventlircd_lircd.release_suffix,
		                       remote);
	} else {
		message_len = snprintf(message, sizeof message, "%x %x %s %s\n",
		                       (unsigned int)event->code,
		                       repeat_count,
		                       name,
		                       remote);
	}

	if (message_len > 0) {
		for(client = eventlircd_lircd.client_list ; client != NULL ; client = client->next) {
			if (write(client->fd, message, (size_t)message_len) != (ssize_t)message_len) {
				if (lircd_client_close(client) != 0) {
					return -1;
				}
			}
		}
		if (lircd_client_purge() != 0) {
			return -1;
		}
	}

	return 0;
}
