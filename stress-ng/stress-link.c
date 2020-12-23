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

static const stress_help_t hardlink_help[] = {
	{ NULL,	"link N",	 "start N workers creating hard links" },
	{ NULL,	"link-ops N",	 "stop after N link bogo operations" },
	{ NULL,	NULL,		 NULL }
};

static const stress_help_t symlink_help[] = {
	{ NULL, "symlink N",	 "start N workers creating symbolic links" },
	{ NULL, "symlink-ops N", "stop after N symbolic link bogo operations" },
	{ NULL, NULL,            NULL }
};

/*
 *  stress_link_unlink()
 *	remove all links
 */
static void stress_link_unlink(
	const stress_args_t *args,
	const uint64_t n)
{
	uint64_t i;

	for (i = 0; i < n; i++) {
		char path[PATH_MAX];

		(void)stress_temp_filename_args(args,
			path, sizeof(path), i);
		(void)unlink(path);
	}
	(void)sync();
}

/*
 *  stress_link_generic
 *	stress links, generic case
 */
static int stress_link_generic(
	const stress_args_t *args,
	int (*linkfunc)(const char *oldpath, const char *newpath),
	const char *funcname)
{
	int rc, ret, fd;
	char oldpath[PATH_MAX];
	size_t oldpathlen;
	bool symlink_func = (linkfunc == symlink);

	ret = stress_temp_dir_mk_args(args);
	if (ret < 0)
		return exit_status(-ret);
	(void)stress_temp_filename_args(args, oldpath, sizeof(oldpath), ~0);
	if ((fd = open(oldpath, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) < 0) {
		if ((errno == ENFILE) || (errno == ENOMEM) || (errno == ENOSPC))
			return EXIT_NO_RESOURCE;
		pr_fail("%s: open %s failed, errno=%d (%s)\n",
			args->name, oldpath, errno, strerror(errno));
		(void)stress_temp_dir_rm_args(args);
		return EXIT_FAILURE;
	}
	(void)close(fd);

	oldpathlen = strlen(oldpath);

	rc = EXIT_SUCCESS;
	do {
		uint64_t i, n = DEFAULT_LINKS;

		for (i = 0; i < n; i++) {
			char newpath[PATH_MAX];
			struct stat stbuf;

			(void)stress_temp_filename_args(args,
				newpath, sizeof(newpath), i);
			if (linkfunc(oldpath, newpath) < 0) {
				if ((errno == EDQUOT) ||
				    (errno == ENOMEM) ||
				    (errno == ENOSPC)) {
					/* Try again */
					continue;
				}
				rc = exit_status(errno);
				pr_fail("%s: %s failed, errno=%d (%s)\n",
					args->name, funcname, errno, strerror(errno));
				n = i;
				break;
			}
			if (symlink_func) {
				char buf[PATH_MAX];
				ssize_t rret;

				rret = readlink(newpath, buf, sizeof(buf) - 1);
				if (rret < 0) {
					rc = exit_status(errno);
					pr_fail("%s: readlink failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
				} else {
					newpath[rret] = '\0';
					if ((size_t)rret != oldpathlen)
						pr_fail("%s: readlink length error, got %zd, expected: %zd\n",
							args->name, (size_t)rret, oldpathlen);
					else
						if (strncmp(oldpath, buf, rret))
							pr_fail("%s: readlink path error, got %s, exepected %s\n",
								args->name, buf, oldpath);
				}
			}
			if (lstat(newpath, &stbuf) < 0) {
				rc = exit_status(errno);
				pr_fail("%s: lstat failed, errno=%d (%s)\n",
					args->name, errno, strerror(errno));
			}

			if (!keep_stressing())
				goto abort;

			inc_counter(args);
		}
		stress_link_unlink(args, n);
	} while (keep_stressing());

abort:
	/* force unlink of all files */
	pr_tidy("%s: removing %" PRIu32" entries\n", args->name, DEFAULT_LINKS);
	stress_link_unlink(args, DEFAULT_LINKS);
	(void)unlink(oldpath);
	(void)stress_temp_dir_rm_args(args);

	return rc;
}

#if !defined(__HAIKU__)
/*
 *  stress_link
 *	stress hard links
 */
static int stress_link(const stress_args_t *args)
{
	return stress_link_generic(args, link, "link");
}
#endif

/*
 *  stress_symlink
 *	stress symbolic links
 */
static int stress_symlink(const stress_args_t *args)
{
	return stress_link_generic(args, symlink, "symlink");
}

#if !defined(__HAIKU__)
stressor_info_t stress_link_info = {
	.stressor = stress_link,
	.class = CLASS_FILESYSTEM | CLASS_OS,
	.help = hardlink_help
};
#else
stressor_info_t stress_link_info = {
	.stressor = stress_not_implemented,
	.class = CLASS_FILESYSTEM | CLASS_OS,
	.help = hardlink_help
};
#endif

stressor_info_t stress_symlink_info = {
	.stressor = stress_symlink,
	.class = CLASS_FILESYSTEM | CLASS_OS,
	.help = symlink_help
};
