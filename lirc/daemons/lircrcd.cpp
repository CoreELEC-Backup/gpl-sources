/****************************************************************************
** lircrcd.c ***************************************************************
****************************************************************************
*
* lircrcd - daemon that manages current mode for all applications
*
* Copyright (C) 2004 Christoph Bartelmus <lirc@bartelmus.de>
*
*/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <getopt.h>

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>

#include <string>
#include <deque>

#include "lirc_client.h"
#include "lirc/lirc_log.h"

#define MAX_CLIENTS 100
#define WHITE_SPACE " \t"

static const logchannel_t logchannel = LOG_APP;

struct config_info {
	char*			config_string;
	struct config_info*	next;
};

struct event_info {
	char*			code;
	struct config_info*	first;
	struct event_info*	next;
};

struct client_data {
	int			fd;
	char*			ident_string;
	std::deque<std::string> pending_codes;
	std::deque<std::string> pending_strings;
	std::string             last_code;
};

struct protocol_directive {
	const char* name;
	int (*function)(int fd, char* message, char* arguments);
};

struct pfd_byname{
	struct pollfd sockfd;
	struct pollfd clis[MAX_CLIENTS];
};


static int code_func(int fd, char* message, char* arguments);
static int ident_func(int fd, char* message, char* arguments);
static int getmode_func(int fd, char* message, char* arguments);
static int setmode_func(int fd, char* message, char* arguments);
static int send_result(int fd, char* message, const char* result);
static int send_success(int fd, char* message);

const struct protocol_directive directives[] = {
	{ "CODE",    code_func	  },
	{ "IDENT",   ident_func	  },
	{ "GETMODE", getmode_func },
	{ "SETMODE", setmode_func },
	{ NULL,	     NULL	  }
	/*
	 * {"DEBUG",debug},
	 * {"DEBUG_LEVEL",debug_level},
	 */
};

enum protocol_string_num {
	P_BEGIN = 0,
	P_DATA,
	P_END,
	P_ERROR,
	P_SUCCESS,
	P_SIGHUP
};

const char* const protocol_string[] = {
	"BEGIN\n",
	"DATA\n",
	"END\n",
	"ERROR\n",
	"SUCCESS\n",
	"SIGHUP\n"
};


static sig_atomic_t term = 0;
static int termsig;
static int clin = 0;
static struct client_data clis[MAX_CLIENTS];

static int daemonized = 0;

static struct lirc_config* config;

static int send_error(int fd, char* message, const char* format_str, ...);


static int get_client_index(int fd)
{
	int i;

	for (i = 0; i < clin; i++)
		if (fd == clis[i].fd)
			return i;
	/* shouldn't ever happen */
	return -1;
}

/* cut'n'paste from fileutils-3.16: */

#define isodigit(c) ((c) >= '0' && (c) <= '7')

/* Return a positive integer containing the value of the ASCII
* octal number S.  If S is not an octal number, return -1.  */

static int oatoi(const char* s)
{
	register int i;

	if (*s == 0)
		return -1;
	for (i = 0; isodigit(*s); ++s)
		i = i * 8 + *s - '0';
	if (*s)
		return -1;
	return i;
}

/* A safer write(), since sockets might not write all but only some of the
 * bytes requested */
inline int write_socket(int fd, const char* buf, int len)
{
	int done, todo = len;

	while (todo) {
		done = write(fd, buf, todo);
		if (done <= 0)
			return done;
		buf += done;
		todo -= done;
	}
	return len;
}

inline int write_socket_len(int fd, const char* buf)
{
	int len;

	len = strlen(buf);
	if (write_socket(fd, buf, len) < len)
		return 0;
	return 1;
}

inline int read_timeout(int fd, char* buf, int len, int timeout_us)
{
	int ret, n;
	struct pollfd  pfd = {fd, POLLIN, 0}; // fd, events, revents
	int timeout = timeout_us > 0 ? timeout_us/1000 : -1;

	/* CAVEAT: (from libc documentation)
	 * Any signal will cause `select' to return immediately.  So if your
	 * program uses signals, you can't rely on `select' to keep waiting
	 * for the full time specified.  If you want to be sure of waiting
	 * for a particular amount of time, you must check for `EINTR' and
	 * repeat the `select' with a newly calculated timeout based on the
	 * current time.  See the example below.
	 *
	 * The timeout is not recalculated here although it should, we keep
	 * waiting as long as there are EINTR.
	 */

	do
		ret = curl_poll(&pfd, 1, timeout * 1000);
	while (ret == -1 && errno == EINTR);
	if (ret == -1) {
		log_perror_err("read_timeout, curl_poll() failed");
		return -1;
	};
	if (ret == 0)
		return 0;       /* timeout */
	n = read(fd, buf, len);
	if (n == -1) {
		log_perror_err("read_timeout: read() failed");
		return -1;
	}
	return n;
}

static void sigterm(int sig)
{
	/* all signals are blocked now */
	if (term)
		return;
	term = 1;
	termsig = sig;
}

static void nolinger(int sock)
{
	static struct linger linger = { 0, 0 };
	int lsize = sizeof(struct linger);

	setsockopt(sock, SOL_SOCKET, SO_LINGER, (void*)&linger, lsize);
}

static void remove_client(int i)
{
	shutdown(clis[i].fd, 2);
	close(clis[i].fd);
	if (clis[i].ident_string)
		free(clis[i].ident_string);

	log_trace("removed client");

	clin--;
	for (; i < clin; i++)
		clis[i] = clis[i + 1];
}

void add_client(int sock)
{
	int fd;
	socklen_t clilen;
	struct sockaddr client_addr;
	int flags;

	clilen = sizeof(client_addr);
	fd = accept(sock, (struct sockaddr*)&client_addr, &clilen);
	if (fd == -1) {
		log_perror_err("accept() failed for new client");
		return;
	}
	;

	if (clin >= MAX_CLIENTS) {
		log_error("connection rejected");
		shutdown(fd, 2);
		close(fd);
		return;
	}
	nolinger(fd);
	flags = fcntl(fd, F_GETFL, 0);
	if (flags != -1)
		fcntl(fd, F_SETFL, flags | O_NONBLOCK);
        log_trace2( "accepted new client");
	clis[clin].fd = fd;
	clis[clin].ident_string = NULL;
	clin++;
}


static int opensocket(const char* socket_id, const char* socketname, mode_t permission, struct sockaddr_un* addr)
{
	int sockfd;
	struct stat s;
	int new_socket = 1;
	int ret;

	/* get socket name */
	if ((socketname == NULL &&
	     lirc_getsocketname(socket_id,
				addr->sun_path, sizeof(addr->sun_path)) >
	     sizeof(addr->sun_path)) || (socketname != NULL && strlen(socketname) >= sizeof(addr->sun_path))) {
		fprintf(stderr, "%s: filename too long", progname);
		return -1;
	}
	if (socketname != NULL)
		strcpy(addr->sun_path, socketname);

	/* create socket */
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("could not create socket");
		return -1;
	}

	/*
	 * get owner, permissions, etc.
	 * so new socket can be the same since we
	 * have to delete the old socket.
	 */
	ret = stat(addr->sun_path, &s);
	if (ret == -1 && errno != ENOENT) {
		perrorf("could not get file information for %s",
			addr->sun_path);
		goto opensocket_failed;
	}

	if (ret != -1) {
		new_socket = 0;
		ret = unlink(addr->sun_path);
		if (ret == -1) {
			perrorf("could not delete %s", addr->sun_path);
			goto opensocket_failed;
		}
	}

	addr->sun_family = AF_UNIX;
	if (bind(sockfd, (struct sockaddr*)addr, sizeof(*addr)) == -1) {
		perror("could not assign address to socket");
		goto opensocket_failed;
	}

	if (new_socket ?
	    chmod(addr->sun_path, permission) :
	    (chmod(addr->sun_path, s.st_mode) == -1 || chown(addr->sun_path, s.st_uid, s.st_gid) == -1)
	    ) {
		perrorf("could not set file permissions on %s", addr->sun_path);
		goto opensocket_failed;
	}

	listen(sockfd, 3);
	nolinger(sockfd);

	return sockfd;

opensocket_failed:
	close(sockfd);
	return -1;
}


static int code_func(int fd, char* message, char* arguments)
{
	int index;

	if (arguments == NULL)
		return send_error(fd, message, "protocol error\n");
	index = get_client_index(fd);
	if (index == -1)
		return send_error(fd, message, "identify yourself first!\n");
	log_trace2("%s asking for code -%s-", clis[index].ident_string, arguments);

	if (clis[index].last_code == arguments) {
		// client checking for more strings
		if (!clis[index].pending_strings.empty()) {
			std::string s = clis[index].pending_strings.front();
			clis[index].pending_strings.pop_front();
			return send_result(fd, message, s.c_str());
		} else {
			clis[index].last_code = "A never used code";
			return send_success(fd, message);
		}
	}
	clis[index].last_code = arguments;
	clis[index].pending_codes.push_back(arguments);

	const char* const_code = clis[index].pending_codes.front().c_str();
	char* var_code = strdup(const_code);
	char* prog = clis[index].ident_string;
	char* s;
	int r;

	clis[index].pending_codes.pop_front();
	while (true) {
		r = lirc_code2charprog(config, var_code, &s, &prog);
		if ( r != 0 || s == NULL || *s == '\0')
			break;
		clis[index].pending_strings.push_back(s);
	}
	free(var_code);
	if ( r != 0 ) {
		return send_error(fd, message, "Cannor decode: %s", arguments);
	} else if (clis[index].pending_strings.size() == 0) {
		return send_success(fd, message);
	} else {
		std::string s = clis[index].pending_strings.front();
		clis[index].pending_strings.pop_front();
		return send_result(fd, message, s.c_str());
	}
}


static int ident_func(int fd, char* message, char* arguments)
{
	int index;

	if (arguments == NULL)
		return send_error(fd, message, "protocol error\n");
	log_trace1("IDENT %s", arguments);
	index = get_client_index(fd);
	if (clis[index].ident_string != NULL)
		return send_error(fd, message, "protocol error\n");
	clis[index].ident_string = strdup(arguments);
	if (clis[index].ident_string == NULL)
		return send_error(fd, message, "out of memory\n");

	log_trace("%s connected", clis[index].ident_string);
	return send_success(fd, message);
}


static int getmode_func(int fd, char* message, char* arguments)
{
	if (arguments != NULL)
		return send_error(fd, message, "protocol error\n");
	log_trace1("GETMODE");
	if (lirc_getmode(config))
		return send_result(fd, message, lirc_getmode(config));
	return send_success(fd, message);
}


static int setmode_func(int fd, char* message, char* arguments)
{
	const char* mode = NULL;

	log_trace1(arguments != NULL ? "SETMODE %s" : "SETMODE", arguments);
	mode = lirc_setmode(config, arguments);
	if (mode)
		return send_result(fd, message, mode);
	return arguments == NULL ? send_success(fd, message) : send_error(fd, message, "out of memory\n");
}


static int send_result(int fd, char* message, const char* result)
{
	const char* count = "1\n";
	char buffer[strlen(result) + 1 + 1];

	sprintf(buffer, "%s\n", result);

	if (!(write_socket_len(fd, protocol_string[P_BEGIN]) &&
	      write_socket_len(fd, message) &&
	      write_socket_len(fd, protocol_string[P_SUCCESS]) &&
	      write_socket_len(fd, protocol_string[P_DATA]) &&
	      write_socket_len(fd, count) &&
	      write_socket_len(fd, buffer) && write_socket_len(fd, protocol_string[P_END])))
		return 0;
	return 1;
}


static int send_success(int fd, char* message)
{
	if (!(write_socket_len(fd, protocol_string[P_BEGIN]) &&
	      write_socket_len(fd, message) &&
	      write_socket_len(fd, protocol_string[P_SUCCESS]) && write_socket_len(fd, protocol_string[P_END])))
		return 0;
	return 1;
}

static int send_error(int fd, char* message, const char* format_str, ...)
{
	char lines[12], buffer[PACKET_SIZE + 1];
	int i, n, len;
	va_list ap;
	char* s1;
	char* s2;

	va_start(ap, format_str);
	vsprintf(buffer, format_str, ap);
	va_end(ap);

	s1 = strrchr(message, '\n');
	s2 = strrchr(buffer, '\n');
	if (s1 != NULL)
		s1[0] = 0;
	if (s2 != NULL)
		s2[0] = 0;
	log_error("error processing command: %s", message);
	log_error("%s", buffer);
	if (s1 != NULL)
		s1[0] = '\n';
	if (s2 != NULL)
		s2[0] = '\n';

	n = 0;
	len = strlen(buffer);
	for (i = 0; i < len; i++)
		if (buffer[i] == '\n')
			n++;
	sprintf(lines, "%d\n", n);

	if (!(write_socket_len(fd, protocol_string[P_BEGIN]) &&
	      write_socket_len(fd, message) &&
	      write_socket_len(fd, protocol_string[P_ERROR]) &&
	      write_socket_len(fd, protocol_string[P_DATA]) &&
	      write_socket_len(fd, lines) &&
	      write_socket_len(fd, buffer) && write_socket_len(fd, protocol_string[P_END])))
		return 0;
	return 1;
}


static int get_command(int fd)
{
	int length;
	char buffer[PACKET_SIZE + 1], backup[PACKET_SIZE + 1];
	char* end;
	int packet_length, i;
	char* directive;

	length = read_timeout(fd, buffer, PACKET_SIZE, 0);
	packet_length = 0;
	while (length > packet_length) {
		buffer[length] = 0;
		end = strchr(buffer, '\n');
		if (end == NULL) {
			log_error("bad send packet: \"%s\"", buffer);
			/* remove clients that behave badly */
			return 0;
		}
		end[0] = 0;
		log_trace("received command: \"%s\"", buffer);
		packet_length = strlen(buffer) + 1;

		strcpy(backup, buffer);
		strcat(backup, "\n");
		directive = strtok(buffer, WHITE_SPACE);
		if (directive == NULL) {
			if (!send_error(fd, backup, "bad send packet\n"))
				return 0;
			goto skip;
		}
		for (i = 0; directives[i].name != NULL; i++) {
			if (strcasecmp(directive, directives[i].name) == 0) {
				if (!directives[i].function(fd, backup, strtok(NULL, "")))
					return 0;
				goto skip;
			}
		}

		if (!send_error(fd, backup, "unknown directive: \"%s\"\n", directive))
			return 0;
skip:
		if (length > packet_length) {
			int new_length;

			memmove(buffer, buffer + packet_length, length - packet_length + 1);
			if (strchr(buffer, '\n') == NULL) {
				new_length = read_timeout(fd, buffer + length -
							  packet_length, PACKET_SIZE - (length - packet_length), 5);
				if (new_length > 0)
					length = length - packet_length + new_length;
				else
					length = new_length;
			} else {
				length -= packet_length;
			}
			packet_length = 0;
		}
	}

	if (length == 0)        /* EOF: connection closed by client */
		return 0;
	return 1;
}



static void loop(int sockfd)
{
	static const int POLLFDS_SIZE =
		sizeof(struct pfd_byname) / sizeof(struct pollfd);
	union {
		struct pfd_byname byname;
		struct pollfd byindex[POLLFDS_SIZE];
	} poll_fds;
	int i;
	int ret;

	while (1) {
		do {
			/* handle signals */
			if (term) {
				log_notice("caught signal");
				return;
			}
			memset(&poll_fds, 0, sizeof(poll_fds));
			for (i = 0; i < POLLFDS_SIZE; i += 1)
				poll_fds.byindex[i].fd = -1;
			poll_fds.byname.sockfd.fd = sockfd;
			poll_fds.byname.sockfd.events = POLLIN;

			for (i = 0; i < clin; i++) {
				poll_fds.byname.clis[i].fd = clis[i].fd;
				poll_fds.byname.clis[i].events = POLLIN;
			}
			log_trace2("poll");
			ret = curl_poll((struct pollfd*) &poll_fds.byindex,
				         POLLFDS_SIZE,
				         0);
			if (ret == -1 && errno != EINTR) {
				log_perror_err("loop: curl_poll() failed");
				raise(SIGTERM);
				continue;
			}
		} while (ret == -1 && errno == EINTR);

		for (i = 0; i < clin; i++) {
			if (poll_fds.byname.clis[i].revents & POLLIN) {
				poll_fds.byname.clis[i].revents	= 0;
				if (get_command(clis[i].fd) == 0) {
					remove_client(i);
					i--;
					if (clin == 0) {
						log_info("last client disconnected, shutting down");
						return;
					}
				}
			}
		}
		if (poll_fds.byname.sockfd.revents & POLLIN) {
			log_trace("registering local client");
			add_client(sockfd);
		}
	}
}

int main(int argc, char** argv)
{
	char* configfile;
	const char* socketfile = NULL;
	mode_t permission = S_IRUSR | S_IWUSR;
	int socket;
	struct sigaction act;
	struct sockaddr_un addr;
	char dir[FILENAME_MAX + 1] = { 0 };

	lirc_log_open("lircrcd", 0, LIRC_NOLOG);
	while (1) {
		int c;
		static struct option long_options[] = {
			{ "help",	no_argument,	   NULL, 'h' },
			{ "version",	no_argument,	   NULL, 'v' },
			{ "permission", required_argument, NULL, 'p' },
			{ "output",	required_argument, NULL, 'o' },
			{ 0,		0,		   0,	 0   }
		};
		c = getopt_long(argc, argv, "hvp:o:", long_options, NULL);
		if (c == -1)
			break;
		switch (c) {
		case 'h':
			printf("Usage: %s [options] config-file\n", progname);
			printf("\t -h --help\t\t\tdisplay this message\n");
			printf("\t -v --version\t\t\tdisplay version\n");
			printf("\t -p --permission=mode\t\tfile permissions for socket\n");
			printf("\t -o --output=socket\t\toutput socket filename\n");
			return EXIT_SUCCESS;
		case 'v':
			printf("%s %s\n", progname, VERSION);
			return EXIT_SUCCESS;
		case 'p':
			if (oatoi(optarg) == -1) {
				fprintf(stderr, "%s: invalid mode\n", progname);
				return EXIT_FAILURE;
			}
			permission = oatoi(optarg);
			break;
		case 'o':
			socketfile = optarg;
			break;
		default:
			printf("Usage: %s [options] config-file\n", progname);
			return EXIT_FAILURE;
		}
	}
	if (optind == argc - 1) {
		configfile = argv[optind];
	} else {
		fprintf(stderr, "%s: invalid argument count\n", progname);
		return EXIT_FAILURE;
	}

	/* read config file */
	if (lirc_readconfig_only(configfile, &config, NULL) != 0) {
		lirc_deinit();
		return EXIT_FAILURE;
	}

	/* open socket */
	socket = opensocket(config->lircrc_class, socketfile, permission, &addr);
	if (socket == -1) {
		lirc_freeconfig(config);
		lirc_deinit();
		return EXIT_FAILURE;
	}

	/* fork */
	if (getcwd(dir, sizeof(dir)) == NULL) {
		lirc_freeconfig(config);
		lirc_deinit();
		perror("getcwd()");
		return EXIT_FAILURE;
	}
	if (daemon(0, 0) == -1) {
		perror("daemon() failed");
		shutdown(socket, 2);
		close(socket);
		lirc_deinit();
		lirc_freeconfig(config);
		return EXIT_FAILURE;
	}
	daemonized = 1;

	openlog(progname, LOG_CONS | LOG_PID, LOG_USER);
	umask(0);
	signal(SIGPIPE, SIG_IGN);

	act.sa_handler = sigterm;
	sigfillset(&act.sa_mask);
	act.sa_flags = SA_RESTART;      /* don't fiddle with EINTR */
	sigaction(SIGTERM, &act, NULL);
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGHUP, &act, NULL);

	log_notice("%s started", progname);
	loop(socket);

	closelog();
	shutdown(socket, 2);
	close(socket);
	if (chdir(dir) == 0)
		unlink(addr.sun_path);
	lirc_freeconfig(config);
	lirc_deinit();
	return EXIT_SUCCESS;
}
