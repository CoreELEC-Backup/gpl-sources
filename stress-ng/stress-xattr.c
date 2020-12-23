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
	{ NULL,	"xattr N",	"start N workers stressing file extended attributes" },
	{ NULL,	"xattr-ops N",	"stop after N bogo xattr operations" },
	{ NULL,	NULL,		NULL }
};

#if (defined(HAVE_SYS_XATTR_H) ||	\
     defined(HAVE_ATTR_XATTR_H)) &&	\
    defined(HAVE_FGETXATTR) &&		\
    defined(HAVE_FLISTXATTR) &&		\
    defined(HAVE_FREMOVEXATTR) &&	\
    defined(HAVE_FSETXATTR) &&		\
    defined(HAVE_GETXATTR) &&		\
    defined(HAVE_LISTXATTR) &&		\
    defined(HAVE_SETXATTR)

#define MAX_XATTRS		(4096)

/*
 *  stress_xattr
 *	stress the xattr operations
 */
static int stress_xattr(const stress_args_t *args)
{
	int ret, fd, rc = EXIT_FAILURE;
	const int bad_fd = stress_get_bad_fd();
	char filename[PATH_MAX];

	ret = stress_temp_dir_mk_args(args);
	if (ret < 0)
		return exit_status(-ret);

	(void)stress_temp_filename_args(args, filename, sizeof(filename), stress_mwc32());
	if ((fd = open(filename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) < 0) {
		rc = exit_status(errno);
		pr_fail("%s: open %s failed, errno=%d (%s)\n",
			args->name, filename, errno, strerror(errno));
		goto out;
	}

	do {
		int i, j;
		char attrname[32];
		char value[32];
		char tmp[sizeof(value)];
		ssize_t sz;
		char *buffer;

		for (i = 0; i < MAX_XATTRS; i++) {
			(void)snprintf(attrname, sizeof(attrname), "user.var_%d", i);
			(void)snprintf(value, sizeof(value), "orig-value-%d", i);

			ret = shim_fsetxattr(fd, attrname, value, strlen(value), XATTR_CREATE);
			if (ret < 0) {
				if (errno == ENOTSUP) {
					pr_inf("%s stressor will be "
						"skipped, filesystem does not "
						"support xattr.\n", args->name);
				}
				if (errno == ENOSPC || errno == EDQUOT)
					break;
				pr_fail("%s: fsetxattr failed, errno=%d (%s)\n",
					args->name, errno, strerror(errno));
				goto out_close;
			}
		}
		/*
		 *  Exercise bad/invalid fd
		 */
		ret = shim_fsetxattr(bad_fd, attrname, value, strlen(value), XATTR_CREATE);
		(void)ret;

		for (j = 0; j < i; j++) {
			(void)snprintf(attrname, sizeof(attrname), "user.var_%d", j);
			(void)snprintf(value, sizeof(value), "value-%d", j);

			ret = shim_fsetxattr(fd, attrname, value, strlen(value),
				XATTR_REPLACE);
			if (ret < 0) {
				if (errno == ENOSPC || errno == EDQUOT)
					break;
				pr_fail("%s: fsetxattr failed, errno=%d (%s)\n",
					args->name, errno, strerror(errno));
				goto out_close;
			}

			/* ..and do it again using setxattr */
			ret = shim_setxattr(filename, attrname, value, strlen(value),
				XATTR_REPLACE);
			if (ret < 0) {
				if (errno == ENOSPC || errno == EDQUOT)
					break;
				pr_fail("%s: setxattr failed, errno=%d (%s)\n",
					args->name, errno, strerror(errno));
				goto out_close;
			}

#if defined(HAVE_LSETXATTR)
			/* Although not a link, it's good to exercise this call */
			ret = shim_lsetxattr(filename, attrname, value, strlen(value),
				XATTR_REPLACE);
			if (ret < 0) {
				if (errno == ENOSPC || errno == EDQUOT)
					break;
				pr_fail("%s: lsetxattr failed, errno=%d (%s)\n",
					args->name, errno, strerror(errno));
				goto out_close;
			}
#endif
		}
		for (j = 0; j < i; j++) {
			(void)snprintf(attrname, sizeof(attrname), "user.var_%d", j);
			(void)snprintf(value, sizeof(value), "value-%d", j);

			(void)memset(tmp, 0, sizeof(tmp));
			ret = shim_fgetxattr(fd, attrname, tmp, sizeof(tmp));
			if (ret < 0) {
				pr_fail("%s: fgetxattr failed, errno=%d (%s)\n",
					args->name, errno, strerror(errno));
				goto out_close;
			}
			if (strncmp(value, tmp, ret)) {
				pr_fail("%s: fgetxattr values different %.*s vs %.*s\n",
					args->name, ret, value, ret, tmp);
				goto out_close;
			}

			ret = shim_getxattr(filename, attrname, tmp, sizeof(tmp));
			if (ret < 0) {
				pr_fail("%s: getxattr failed, errno=%d (%s)\n",
					args->name, errno, strerror(errno));
				goto out_close;
			}
			if (strncmp(value, tmp, ret)) {
				pr_fail("%s: getxattr values different %.*s vs %.*s\n",
					args->name, ret, value, ret, tmp);
				goto out_close;
			}

#if defined(HAVE_LGETXATTR)
			ret = shim_lgetxattr(filename, attrname, tmp, sizeof(tmp));
			if (ret < 0) {
				pr_fail("%s: lgetxattr failed, errno=%d (%s)\n",
					args->name, errno, strerror(errno));
				goto out_close;
			}
			if (strncmp(value, tmp, ret)) {
				pr_fail("%s: lgetxattr values different %.*s vs %.*s\n",
					args->name, ret, value, ret, tmp);
				goto out_close;
			}
#endif
		}

		/*
		 *  Exercise bad/invalid fd
		 */
		ret = shim_fgetxattr(bad_fd, "user.var_bad", tmp, sizeof(tmp));
		(void)ret;

		/* Determine how large a buffer we required... */
		sz = shim_flistxattr(fd, NULL, 0);
		if (sz < 0) {
			pr_fail("%s: flistxattr failed, errno=%d (%s)\n",
				args->name, errno, strerror(errno));
			goto out_close;
		}
		buffer = malloc(sz);
		if (buffer) {
			/* ...and fetch */
			sz = shim_listxattr(filename, buffer, sz);
			free(buffer);

			if (sz < 0) {
				pr_fail("%s: listxattr failed, errno=%d (%s)\n",
					args->name, errno, strerror(errno));
				goto out_close;
			}
		}

		/*
		 *  Exercise bad/invalid fd
		 */
		ret = shim_flistxattr(bad_fd, NULL, 0);
		(void)ret;

		for (j = 0; j < i; j++) {
			char *errmsg;

			(void)snprintf(attrname, sizeof(attrname), "user.var_%d", j);

			switch (j % 3) {
			case 0:
				ret = shim_fremovexattr(fd, attrname);
				errmsg = "fremovexattr";
				break;
#if defined(HAVE_LREMOVEXATTR)
			case 1:
				ret = shim_lremovexattr(filename, attrname);
				errmsg = "lremovexattr";
				break;
#endif
			default:
				ret = shim_removexattr(filename, attrname);
				errmsg = "removexattr";
				break;
			}
			if (ret < 0) {
				pr_fail("%s: %s failed, errno=%d (%s)\n",
					args->name, errmsg, errno, strerror(errno));
				goto out_close;
			}
		}

		/*
		 *  Exercise bad/invalid fd
		 */
		ret = shim_fremovexattr(bad_fd, "user.var_bad");
		(void)ret;

#if defined(HAVE_LLISTXATTR)
		sz = shim_llistxattr(filename, NULL, 0);
		if (sz < 0) {
			pr_fail("%s: llistxattr failed, errno=%d (%s)\n",
				args->name, errno, strerror(errno));
			goto out_close;
		}
#endif
		inc_counter(args);
	} while (keep_stressing());

	rc = EXIT_SUCCESS;
out_close:
	(void)close(fd);
out:
	(void)unlink(filename);
	(void)stress_temp_dir_rm_args(args);
	return rc;
}

stressor_info_t stress_xattr_info = {
	.stressor = stress_xattr,
	.class = CLASS_FILESYSTEM | CLASS_OS,
	.help = help
};
#else
stressor_info_t stress_xattr_info = {
	.stressor = stress_not_implemented,
	.class = CLASS_FILESYSTEM | CLASS_OS,
	.help = help
};
#endif
