/*
 * Copyright (C) 2013-2020 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * This code is a complete clean re-write of the stress tool by
 * Colin Ian King <colin.king@canonical.com> and attempts to be
 * backwardly compatible with the stress tool by Amos Waterland
 * <apw@rossby.metr.ou.edu> but has more stress tests and more
 * functionality.
 *
 */
#include "stress-ng.h"

static const stress_help_t help[] = {
	{ NULL,	"pty N",	"start N workers that exercise pseudoterminals" },
	{ NULL,	"pty-ops N",	"stop pty workers after N pty bogo operations" },
	{ NULL,	"pty-max N",	"attempt to open a maximum of N ptys" },
	{ NULL,	NULL,          NULL }
};

#if defined(HAVE_TERMIOS_H) &&	\
    defined(HAVE_TERMIO_H) && \
    defined(HAVE_PTSNAME)

typedef struct {
	char *slavename;
	int master;
	int slave;
} stress_pty_info_t;

#endif

/*
 *  stress_set_pty_max()
 *	set ptr maximum
 */
static int stress_set_pty_max(const char *opt)
{
	uint64_t pty_max;

	pty_max = stress_get_uint64(opt);
	stress_check_range("pty-max", pty_max,
		MIN_PTYS, MAX_PTYS);
	return stress_set_setting("pty-max", TYPE_ID_UINT64, &pty_max);
}

static const stress_opt_set_func_t opt_set_funcs[] = {
	{ OPT_pty_max,	stress_set_pty_max },
	{ 0,		NULL }
};

#if defined(HAVE_TERMIOS_H) &&	\
    defined(HAVE_TERMIO_H) &&	\
    defined(HAVE_PTSNAME)

/*
 *  stress_pty
 *	stress pty handling
 */
static int stress_pty(const stress_args_t *args)
{
	uint64_t pty_max = DEFAULT_PTYS;
	stress_pty_info_t *ptys;

	(void)stress_get_setting("pty-max", &pty_max);

	ptys = calloc(pty_max, sizeof(*ptys));
	if (!ptys) {
		pr_inf("%s: allocation of pty array failed: %d (%s)\n",
			args->name, errno, strerror(errno));
		return EXIT_NO_RESOURCE;
	}

	do {
		size_t i, n;


		for (n = 0; n < pty_max; n++) {
			ptys[n].slave = -1;
			ptys[n].master = open("/dev/ptmx", O_RDWR);
			if (ptys[n].master < 0) {
				if ((errno != ENOMEM) &&
				    (errno != ENOSPC) &&
				    (errno != EIO) &&
				    (errno != EMFILE)) {
					pr_fail("%s: open /dev/ptmx failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
					goto clean;
				}
			} else {
				ptys[n].slavename = ptsname(ptys[n].master);
				if (!ptys[n].slavename) {
					pr_fail("%s: ptsname failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
					goto clean;
				}
				if (grantpt(ptys[n].master) < 0) {
					pr_fail("%s: grantpt failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
					goto clean;
				}
				if (unlockpt(ptys[n].master) < 0) {
					pr_fail("%s: unlockpt failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
					goto clean;
				}
				ptys[n].slave = open(ptys[n].slavename, O_RDWR);
				if (ptys[n].slave < 0) {
					if (errno != EMFILE) {
						pr_fail("%s: open %s failed, errno=%d (%s)\n",
							args->name, ptys[n].slavename, errno, strerror(errno));
						goto clean;
					}
				}
			}
			if (!keep_stressing_flag())
				goto clean;
		}
		/*
		 *  ... and exercise ioctls ...
		 */
		for (i = 0; i < n; i++) {
			if ((ptys[i].master < 0) || (ptys[i].slave < 0))
				continue;

#if defined(DHAVE_TCGETATTR)
			{
				struct termios ios;

				if (tcgetattr(ptys[i].master, &ios) < 0) {
					pr_fail("%s: tcgetattr on master pty failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
				}
			}
#endif
#if defined(HAVE_TCDRAIN)
			{
				if (tcdrain(ptys[i].slave) < 0) {
					pr_fail("%s: tcdrain on slave pty failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
				}
			}
#endif
#if defined(HAVE_TCFLUSH)
			{
#if defined(TCIFLUSH)
				if (tcflush(ptys[i].slave, TCIFLUSH) < 0) {
					pr_fail("%s: tcflush TCIFLUSH on slave pty failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
				}
#endif
#if defined(TCOFLUSH)
				if (tcflush(ptys[i].slave, TCOFLUSH) < 0) {
					pr_fail("%s: tcflush TCOFLUSH on slave pty failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
				}
#endif
#if defined(TCIOFLUSH)
				if (tcflush(ptys[i].slave, TCIOFLUSH) < 0) {
					pr_fail("%s: tcflush TCOOFLUSH on slave pty failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
				}
#endif
			}
#endif
#if defined(HAVE_TCFLOW)
#if defined(TCOOFF) && \
    defined(TCOON)
			{
				if (tcflow(ptys[i].slave, TCOOFF) < 0) {
					pr_fail("%s: tcflow TCOOFF on slave pty failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
				}
				if (tcflow(ptys[i].slave, TCOON) < 0) {
					pr_fail("%s: tcflow TCOON on slave pty failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
				}
			}
#endif
#if defined(TCIOFF) && \
    defined(TCION)
			{
				if (tcflow(ptys[i].slave, TCIOFF) < 0) {
					pr_fail("%s: tcflow TCIOFF on slave pty failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
				}
				if (tcflow(ptys[i].slave, TCION) < 0) {
					pr_fail("%s: tcflow TCION on slave pty failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
				}
			}
#endif
#endif
#if defined(TCGETS)
			{
				struct termios ios;

				if ((ioctl(ptys[i].slave, TCGETS, &ios) < 0) &&
				    (errno != EINTR))
					pr_fail("%s: ioctl TCGETS on slave pty failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
			}
#endif
#if defined(TCSETS)
			{
				struct termios ios;

				if ((ioctl(ptys[i].slave, TCSETS, &ios) < 0) &&
				    (errno != EINTR))
					pr_fail("%s: ioctl TCSETS on slave pty failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
			}
#endif
#if defined(TCSETSW)
			{
				struct termios ios;

				if ((ioctl(ptys[i].slave, TCSETSW, &ios) < 0) &&
				    (errno != EINTR))
					pr_fail("%s: ioctl TCSETSW on slave pty failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
			}
#endif
#if defined(TCSETSF)
			{
				struct termios ios;

				if ((ioctl(ptys[i].slave, TCSETSF, &ios) < 0) &&
				    (errno != EINTR))
					pr_fail("%s: ioctl TCSETSF on slave pty failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
			}
#endif
#if defined(TCGETA)
			{
				struct termio io;

				if ((ioctl(ptys[i].slave, TCGETA, &io) < 0) &&
				    (errno != EINTR))
					pr_fail("%s: ioctl TCGETA on slave pty failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
			}
#endif
#if defined(TCSETA)
			{
				struct termio io;

				if ((ioctl(ptys[i].slave, TCSETA, &io) < 0) &&
				    (errno != EINTR))
					pr_fail("%s: ioctl TCSETA on slave pty failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
			}
#endif
#if defined(TCSETAW)
			{
				struct termio io;

				if ((ioctl(ptys[i].slave, TCSETAW, &io) < 0) &&
				    (errno != EINTR))
					pr_fail("%s: ioctl TCSETAW on slave pty failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
			}
#endif
#if defined(TCSETAF)
			{
				struct termio io;

				if ((ioctl(ptys[i].slave, TCSETAF, &io) < 0) &&
				    (errno != EINTR))
					pr_fail("%s: ioctl TCSETAF on slave pty failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
			}
#endif
#if defined(TIOCGLCKTRMIOS)
			{
				struct termios ios;

				if ((ioctl(ptys[i].slave, TIOCGLCKTRMIOS, &ios) < 0) &&
				    (errno != EINTR))
					pr_fail("%s: ioctl TIOCGLCKTRMIOS on slave pty failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
			}
#endif
#if defined(TIOCGWINSZ)
			{
				struct winsize ws;

				if ((ioctl(ptys[i].slave, TIOCGWINSZ, &ws) < 0) &&
				    (errno != EINTR))
					pr_fail("%s: ioctl TIOCGWINSZ on slave pty failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
			}
#endif
#if defined(TIOCSWINSZ)
			{
				struct winsize ws;

				if ((ioctl(ptys[i].slave, TIOCSWINSZ, &ws) < 0) &&
				    (errno != EINTR))
					pr_fail("%s: ioctl TIOCSWINSZ on slave pty failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
			}
#endif
#if defined(FIONREAD)
			{
				int arg;

				if ((ioctl(ptys[i].slave, FIONREAD, &arg) < 0) &&
				    (errno != EINTR))
					pr_fail("%s: ioctl FIONREAD on slave pty failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
			}
#endif
#if defined(TIOCINQ)
			{
				int arg;

				if ((ioctl(ptys[i].slave, TIOCINQ, &arg) < 0) &&
				    (errno != EINTR))
					pr_fail("%s: ioctl TIOCINQ on slave pty failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
			}
#endif
#if defined(TIOCOUTQ)
			{
				int arg;

				if ((ioctl(ptys[i].slave, TIOCOUTQ, &arg) < 0) &&
				    (errno != EINTR))
					pr_fail("%s: ioctl TIOCOUTQ on slave pty failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
			}
#endif

			if (!keep_stressing_flag())
				goto clean;
		}
clean:
		/*
		 *  and close
		 */
		for (i = 0; i < n; i++) {
			if (ptys[i].slave != -1)
				(void)close(ptys[i].slave);
			if (ptys[i].master != -1)
				(void)close(ptys[i].master);
		}
		inc_counter(args);
	} while (keep_stressing());

	free(ptys);

	return EXIT_SUCCESS;
}

stressor_info_t stress_pty_info = {
	.stressor = stress_pty,
	.class = CLASS_OS,
	.opt_set_funcs = opt_set_funcs,
	.help = help
};
#else
stressor_info_t stress_pty_info = {
	.stressor = stress_not_implemented,
	.class = CLASS_OS,
	.opt_set_funcs = opt_set_funcs,
	.help = help
};
#endif
