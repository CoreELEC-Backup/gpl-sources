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
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/*
 * Single Unix Specification Version 3 headers.
 */
#include <signal.h>       /* C89 */
#include <stdbool.h>      /* C99 */
#include <stdio.h>        /* C89 */
#include <stdlib.h>       /* C89 */
#include <string.h>       /* C89 */
#include <sys/stat.h>     /* POSIX */
#include <syslog.h>       /* XSI */
#include <unistd.h>       /* POSIX */
/*
 * Misc headers.
 */
#include <getopt.h>
#include <sysexits.h>
/*
 * eventlircd headers.
 */
#include "input.h"
#include "lircd.h"
#include "monitor.h"

int main(int argc,char **argv)
{
    static struct option longopts[] =
    {
        {"help",no_argument,NULL,'h'},
        {"version",no_argument,NULL,'V'},
        {"verbose",no_argument,NULL,'v'},
        {"foreground",no_argument,NULL,'f'},
        {"evmap",required_argument,NULL,'e'},
        {"socket",required_argument,NULL,'s'},
        {"mode",required_argument,NULL,'m'},
        {"repeat-filter",no_argument,NULL,'R'},
        {"release",required_argument,NULL,'r'},
        {0, 0, 0, 0}
    };
    const char *progname = NULL;
    int verbose = 0;
    bool foreground = false;
    const char *input_device_evmap_dir = EVMAP_DIR;
    const char *lircd_socket_path = LIRCD_SOCKET;
    mode_t lircd_socket_mode = S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IWOTH | S_IROTH;
    bool input_repeat_filter = false;
    const char *lircd_release_suffix = NULL;
    int opt;

    for (progname = argv[0] ; strchr(progname, '/') != NULL ; progname = strchr(progname, '/') + 1);

    openlog(progname, LOG_CONS | LOG_PERROR | LOG_PID, LOG_DAEMON);

    while((opt = getopt_long(argc, argv, "hVvfe:s:m:Rr:", longopts, NULL)) != -1)
    {
        switch(opt)
        {
            case 'h':
		fprintf(stdout, "Usage: %s [options]\n", progname);
		fprintf(stdout, "    -h --help              print this help message and exit\n");
		fprintf(stdout, "    -V --version           print the program version and exit\n");
		fprintf(stdout, "    -v --verbose           increase the output message verbosity (-v, -vv or -vvv)\n");
		fprintf(stdout, "    -f --foreground        run in the foreground\n");
		fprintf(stdout, "    -e --evmap=<dir>       directory containing input device event map files (default is '%s')\n",
                                                            input_device_evmap_dir);
		fprintf(stdout, "    -s --socket=<socket>   lircd socket (default is '%s')\n",
                                                            lircd_socket_path);
		fprintf(stdout, "    -m --mode=<mode>       lircd socket mode (default is '%04o')\n",
                                                            lircd_socket_mode);
		fprintf(stdout, "    -R --repeat-filter     enable repeat filtering (default is '%s')\n",
                                                            input_repeat_filter ? "false" : "true");
		fprintf(stdout, "    -r --release=<suffix>  generate key release events suffixed with <suffix>\n");
                exit(EX_OK);
                break;
            case 'V':
                fprintf(stdout, PACKAGE_STRING "\n");
                break;
            case 'v':
                if (verbose < 3)
                {
                    verbose++;
                }
                else
                {
                    syslog(LOG_WARNING, "the highest verbosity level is -vvv\n");
                }
                break;
            case 'f':
                foreground = true;
                break;
            case 'e':
                input_device_evmap_dir = optarg;
                break;
            case 's':
                lircd_socket_path = optarg;
                break;
            case 'm':
                lircd_socket_mode = (mode_t)atol(optarg);
                break;
            case 'R':
                input_repeat_filter = true;
                break;
            case 'r':
                lircd_release_suffix = optarg;
                break;
            default:
                fprintf(stderr, "error: unknown option: %c\n", opt);
                exit(EX_USAGE);
        }
    }

    if      (verbose == 0)
    {
        setlogmask(0 | LOG_DEBUG | LOG_INFO | LOG_NOTICE);
    }
    else if (verbose == 1)
    {
        setlogmask(0 | LOG_DEBUG | LOG_INFO);
    }
    else if (verbose == 2)
    {
        setlogmask(0 | LOG_DEBUG);
    }
    else
    {
        setlogmask(0);
    }

    signal(SIGPIPE, SIG_IGN);

    if (monitor_init() != 0)
    {
        exit(EXIT_FAILURE);
    }

    /* Initialize the lircd socket before daemonizing in order to ensure that programs
       started after it damonizes will have an lircd socket with which to connect. */
    if (lircd_init(lircd_socket_path, lircd_socket_mode, lircd_release_suffix) != 0)
    {
        monitor_exit();
        exit(EXIT_FAILURE);
    }

    if (foreground != true)
    {
        if (daemon(0, 0) != 0)
        {
            monitor_exit();
            lircd_exit();
            exit(EXIT_FAILURE);
        }
    }

    if (input_init(input_device_evmap_dir, input_repeat_filter) != 0)
    {
        monitor_exit();
        lircd_exit();
        exit(EXIT_FAILURE);
    }

    monitor_run();

    if (input_exit() != 0)
    {
        monitor_exit();
        lircd_exit();
        exit(EXIT_FAILURE);
    }

    if (lircd_exit() != 0)
    {
        monitor_exit();
        exit(EXIT_FAILURE);
    }

    if (monitor_exit() != 0)
    {
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
