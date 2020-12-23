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
	{ NULL,	"mknod N",	"start N workers that exercise mknod" },
	{ NULL,	"mknod-ops N",	"stop after N mknod bogo operations" },
	{ NULL,	NULL,		NULL }
};

#if defined(__linux__)

static const int modes[] = {
#if defined(S_IFIFO)
	S_IFIFO,	/* FIFO */
#endif
#if defined(S_IFREG)
	S_IFREG,	/* Regular file */
#endif
#if defined(S_IFSOCK)
	S_IFSOCK	/* named socket */
#endif
};

/*
 *  stress_mknod_tidy()
 *	remove all files
 */
static void stress_mknod_tidy(
	const stress_args_t *args,
	const uint64_t n)
{
	uint64_t i;

	for (i = 0; i < n; i++) {
		char path[PATH_MAX];
		const uint64_t gray_code = (i >> 1) ^ i;

		(void)stress_temp_filename_args(args,
			path, sizeof(path), gray_code);
		(void)unlink(path);
	}
}

/*
 *  stress_mknod
 *	stress mknod creates
 */
static int stress_mknod(const stress_args_t *args)
{
	const size_t num_nodes = SIZEOF_ARRAY(modes);
	int ret;

	if (num_nodes == 0) {
		pr_err("%s: aborting, no valid mknod modes.\n",
			args->name);
		return EXIT_FAILURE;
	}
	ret = stress_temp_dir_mk_args(args);
	if (ret < 0)
		return exit_status(-ret);

	do {
		uint64_t i, n = DEFAULT_DIRS;

		for (i = 0; i < n; i++) {
			char path[PATH_MAX];
			const uint64_t gray_code = (i >> 1) ^ i;
			int mode = modes[stress_mwc32() % num_nodes];

			(void)stress_temp_filename_args(args,
				path, sizeof(path), gray_code);
			if (mknod(path, mode | S_IRUSR | S_IWUSR, 0) < 0) {
				if ((errno == ENOSPC) || (errno == ENOMEM))
					continue;	/* Try again */
				pr_fail("%s: mknod %s failed, errno=%d (%s)\n",
					args->name, path, errno, strerror(errno));
				n = i;
				break;
			}

			if (!keep_stressing())
				goto abort;

			inc_counter(args);
		}
		stress_mknod_tidy(args, n);
		if (!keep_stressing_flag())
			break;
		(void)sync();
	} while (keep_stressing());

abort:
	/* force unlink of all files */
	pr_tidy("%s: removing %" PRIu32 " nodes\n", args->name, DEFAULT_DIRS);
	stress_mknod_tidy(args, DEFAULT_DIRS);
	(void)stress_temp_dir_rm_args(args);

	return EXIT_SUCCESS;
}

stressor_info_t stress_mknod_info = {
	.stressor = stress_mknod,
	.class = CLASS_FILESYSTEM | CLASS_OS,
	.help = help
};
#else
stressor_info_t stress_mknod_info = {
	.stressor = stress_not_implemented,
	.class = CLASS_FILESYSTEM | CLASS_OS,
	.help = help
};
#endif
