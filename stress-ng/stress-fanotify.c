/*
 * Copyright (C) 2012-2020 Canonical, Ltd.
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
 */
#include "stress-ng.h"

static const stress_help_t help[] = {
	{ NULL,	"fanotify N",	  "start N workers exercising fanotify events" },
	{ NULL,	"fanotify-ops N", "stop fanotify workers after N bogo operations" },
	{ NULL,	NULL,		  NULL }
};

#if defined(HAVE_MNTENT_H) &&		\
    defined(HAVE_SYS_SELECT_H) && 	\
    defined(HAVE_SYS_FANOTIFY_H) &&	\
    defined(HAVE_FANOTIFY)

#define MAX_MNTS	(4096)

#define BUFFER_SIZE	(4096)

/* fanotify stats */
typedef struct {
	uint64_t	open;
	uint64_t	close_write;
	uint64_t	close_nowrite;
	uint64_t	access;
	uint64_t	modify;
} stress_fanotify_account_t;

#endif

#if defined(HAVE_FANOTIFY) &&	\
    defined(HAVE_SYS_SELECT_H)

static const int fan_stress_settings[] = {
#if defined(FAN_ACCESS)
	FAN_ACCESS,
#endif
#if defined(FAN_ACCESS_PERM)
	FAN_ACCESS_PERM,
#endif
#if defined(FAN_ATTRIB)
	FAN_ATTRIB,
#endif
#if defined(FAN_CLOSE)
	FAN_CLOSE,
#endif
#if defined(FAN_CLOSE_NOWRITE)
	FAN_CLOSE_NOWRITE,
#endif
#if defined(FAN_CLOSE_WRITE)
	FAN_CLOSE_WRITE,
#endif
#if defined(FAN_CREATE)
	FAN_CREATE,
#endif
#if defined(FAN_DELETE)
	FAN_DELETE,
#endif
#if defined(FAN_DELETE_SELF)
	FAN_DELETE_SELF,
#endif
#if defined(FAN_DIR_MODIFY)
	FAN_DIR_MODIFY,
#endif
#if defined(FAN_EVENT_ON_CHILD)
	FAN_EVENT_ON_CHILD,
#endif
#if defined(FAN_MODIFY)
	FAN_MODIFY,
#endif
#if defined(FAN_MOVE)
	FAN_MOVE,
#endif
#if defined(FAN_MOVED_FROM)
	FAN_MOVED_FROM,
#endif
#if defined(FAN_MOVE_SELF)
	FAN_MOVE_SELF,
#endif
#if defined(FAN_MOVED_TO)
	FAN_MOVED_TO,
#endif
#if defined(FAN_ONDIR)
	FAN_ONDIR,
#endif
#if defined(FAN_OPEN)
	FAN_OPEN,
#endif
#if defined(FAN_OPEN_EXEC)
	FAN_OPEN_EXEC,
#endif
#if defined(FAN_OPEN_EXEC_PERM)
	FAN_OPEN_EXEC_PERM,
#endif
#if defined(FAN_OPEN_PERM)
	FAN_OPEN_PERM,
#endif
#if defined(FAN_Q_OVERFLOW)
	FAN_Q_OVERFLOW,
#endif
	0
};

static char *mnts[MAX_MNTS];
static int n_mnts;

/*
 *  stress_fanotify_supported()
 *      check if we can run this as root
 */
static int stress_fanotify_supported(const char *name)
{
	int fan_fd;
	static const char skipped[] =
		"stressor will be skipped, ";
	static const char noperm[] =
		"need to be running with CAP_SYS_ADMIN "
		"rights for this stressor";
	static const char noresource[] =
		"no resources (out of descriptors or memory)";
	static const char nosyscall[] =
		"system call not supported";

	if (!stress_check_capability(SHIM_CAP_SYS_ADMIN)) {
		pr_inf("%s%s\n", skipped, noperm);
		return -1;
	}
	fan_fd = fanotify_init(0, 0);
	if (fan_fd < 0) {
		int rc = -1;

		switch (errno) {
		case EPERM:
			pr_inf("%s %s%s\n", name, skipped, noperm);
			break;
		case EMFILE:
		case ENOMEM:
			pr_inf("%s %s%s\n", name, skipped, noresource);
			break;
		case ENOSYS:
			pr_inf("%s %s%s\n", name, skipped, nosyscall);
			break;
		default:
			rc = 0;
			break;
		}
		return rc;
	}
	(void)close(fan_fd);

	return 0;
}

/*
 *  fanotify_event_init()
 *	initialize fanotify
 */
static int fanotify_event_init(const char *name)
{
	int fan_fd, count = 0, i;

	(void)memset(mnts, 0, sizeof(mnts));

	fan_fd = fanotify_init(0, 0);
	if (fan_fd < 0) {
		pr_err("%s: cannot initialize fanotify, errno=%d (%s)\n",
			name, errno, strerror(errno));
		return -1;
	}

	/* do all mount points */
	n_mnts = stress_mount_get(mnts, MAX_MNTS);
	if (n_mnts < 1) {
		pr_err("%s: setmntent cannot get mount points from "
			"/proc/self/mounts, errno=%d (%s)\n",
			name, errno, strerror(errno));
		(void)close(fan_fd);
		return -1;
	}

	/*
	 *  Gather all mounted file systems and monitor them
	 */
	for (i = 0; i < n_mnts; i++) {
		size_t j;
#if defined(FAN_MARK_MOUNT) || defined(FAN_MARK_FILESYSTEM)
		int ret;
#endif

		for (j = 0; j < SIZEOF_ARRAY(fan_stress_settings); j++) {
#if defined(FAN_MARK_MOUNT)
			ret = fanotify_mark(fan_fd, FAN_MARK_ADD | FAN_MARK_MOUNT,
				fan_stress_settings[j], AT_FDCWD, mnts[i]);
			if (ret == 0)
				count++;
#endif

#if defined(FAN_MARK_FILESYSTEM)
			ret = fanotify_mark(fan_fd, FAN_MARK_ADD | FAN_MARK_FILESYSTEM,
				fan_stress_settings[j], AT_FDCWD, mnts[i]);
			if (ret == 0)
				count++;
#endif
		}
	}

	/* This really should not happen, / is always mounted */
	if (!count) {
		pr_err("%s: no mount points could be monitored\n",
			name);
		(void)close(fan_fd);
		return -1;
	}
	return fan_fd;
}

/*
 *  fanotify_event_clear()
 *	exercise remove and flush fanotify_marking
 */
static void fanotify_event_clear(const int fan_fd)
{
#if defined(FAN_MARK_REMOVE)
	int i;

	/*
	 *  Gather all mounted file systems and monitor them
	 */
	for (i = 0; i < n_mnts; i++) {
		size_t j;
#if defined(FAN_MARK_MOUNT) || defined(FAN_MARK_FILESYSTEM)
		int ret;
#endif

		for (j = 0; j < SIZEOF_ARRAY(fan_stress_settings); j++) {
#if defined(FAN_MARK_MOUNT)
			ret = fanotify_mark(fan_fd, FAN_MARK_REMOVE | FAN_MARK_MOUNT,
				fan_stress_settings[j], AT_FDCWD, mnts[i]);
			(void)ret;
#endif

#if defined(FAN_MARK_FILESYSTEM)
			ret = fanotify_mark(fan_fd, FAN_MARK_REMOVE | FAN_MARK_FILESYSTEM,
				fan_stress_settings[j], AT_FDCWD, mnts[i]);
			(void)ret;
#endif
		}
#if defined(FAN_MARK_FLUSH) && defined(FAN_MARK_MOUNT)
		ret = fanotify_mark(fan_fd, FAN_MARK_FLUSH | FAN_MARK_MOUNT,
				0, AT_FDCWD, mnts[i]);
		(void)ret;
#endif
#if defined(FAN_MARK_FLUSH) && defined(FAN_MARK_FILESYSTEM)
		ret = fanotify_mark(fan_fd, FAN_MARK_FLUSH | FAN_MARK_FILESYSTEM,
				0, AT_FDCWD, mnts[i]);
		(void)ret;
#endif
	}
#endif
}

/*
 *  stress_fanotify()
 *	stress fanotify
 */
static int stress_fanotify(const stress_args_t *args)
{
	char pathname[PATH_MAX - 16], filename[PATH_MAX];
	int ret, fan_fd, pid, rc = EXIT_SUCCESS;
	stress_fanotify_account_t account;

	(void)memset(&account, 0, sizeof(account));

	stress_temp_dir_args(args, pathname, sizeof(pathname));
	(void)snprintf(filename, sizeof(filename), "%s/%s", pathname, "fanotify_file");
	ret = stress_temp_dir_mk_args(args);
	if (ret < 0)
		return exit_status(-ret);

	pid = fork();
	if (pid < 0) {
		pr_err("%s: fork failed: errno=%d (%s)\n",
			args->name, errno, strerror(errno));
		rc = EXIT_NO_RESOURCE;
		goto tidy;
	} else if (pid == 0) {
		/* Child */

		(void)sched_settings_apply(true);

		do {
			int fd;
			ssize_t n;
			char buffer[64];

			/* Force FAN_CLOSE_NOWRITE */
			fd = creat(filename, S_IRUSR | S_IWUSR);
			if (fd < 0) {
				pr_fail("%s: creat %s failed, errno=%d (%s)\n",
					args->name, filename, errno, strerror(errno));
				(void)kill(args->ppid, SIGALRM);
				_exit(EXIT_FAILURE);
			}
			(void)close(fd);

			/* Force FAN_CLOSE_WRITE */
			fd = open(filename, O_WRONLY, S_IRUSR | S_IWUSR);
			if (fd < 0) {
				pr_fail("%s: open %s O_WRONLY failed, errno=%d (%s)\n",
					args->name, filename, errno, strerror(errno));
				(void)kill(args->ppid, SIGALRM);
				_exit(EXIT_FAILURE);
			}
			n = write(fd, "test", 4);
			(void)n;
			(void)close(fd);

			/* Force FAN_ACCESS */
			fd = open(filename, O_RDONLY, S_IRUSR | S_IWUSR);
			if (fd < 0) {
				pr_fail("%s: open %s O_RDONLY failed, errno=%d (%s)\n",
					args->name, filename, errno, strerror(errno));
				(void)kill(args->ppid, SIGALRM);
				_exit(EXIT_FAILURE);
			}
			n = read(fd, buffer, sizeof(buffer));
			(void)n;
			(void)close(fd);

			/* Force remove */
			(void)unlink(filename);
		} while (keep_stressing());

		_exit(EXIT_SUCCESS);
	} else {
		void *buffer;

		ret = posix_memalign(&buffer, BUFFER_SIZE, BUFFER_SIZE);
		if (ret != 0 || buffer == NULL) {
			pr_err("%s: posix_memalign: cannot allocate 4K "
				"aligned buffer\n", args->name);
			rc = EXIT_NO_RESOURCE;
			goto tidy;
		}

		fan_fd = fanotify_event_init(args->name);
		if (fan_fd < 0) {
			free(buffer);
			rc = EXIT_FAILURE;
			goto tidy;
		}

		do {
			fd_set rfds;
			ssize_t len;

			FD_ZERO(&rfds);
			FD_SET(fan_fd, &rfds);
			ret = select(fan_fd + 1, &rfds, NULL, NULL, NULL);
			if (ret == -1) {
				if (errno == EINTR)
					continue;
				pr_fail("%s: select failed, errno=%d (%s)\n",
					args->name, errno, strerror(errno));
				continue;
			}
			if (ret == 0)
				continue;

#if defined(FIONREAD)
			{
				int isz;

				/*
				 *  Force kernel to determine number
				 *  of bytes that are ready to be read
				 *  for some extra stress
				 */
				ret = ioctl(fan_fd, FIONREAD, &isz);
				(void)ret;
			}
#endif
			if ((len = read(fan_fd, (void *)buffer, BUFFER_SIZE)) > 0) {
				struct fanotify_event_metadata *metadata;
				metadata = (struct fanotify_event_metadata *)buffer;

				while (FAN_EVENT_OK(metadata, len)) {
					if (!keep_stressing_flag())
						break;
					if ((metadata->fd != FAN_NOFD) && (metadata->fd >= 0)) {
#if defined(FAN_OPEN)
						if (metadata->mask & FAN_OPEN)
							account.open++;
#endif
#if defined(FAN_CLOSE_WRITE)
						if (metadata->mask & FAN_CLOSE_WRITE)
							account.close_write++;
#endif
#if defined(FAN_CLOSE_NOWRITE)
						if (metadata->mask & FAN_CLOSE_NOWRITE)
							account.close_nowrite++;
#endif
#if defined(FAN_ACCESS)
						if (metadata->mask & FAN_ACCESS)
							account.access++;
#endif
#if defined(FAN_MODIFY)
						if (metadata->mask & FAN_MODIFY)
							account.modify++;
#endif
						inc_counter(args);
						(void)close(metadata->fd);
					}
					metadata = FAN_EVENT_NEXT(metadata, len);
				}
			}
		} while (keep_stressing());

		free(buffer);
		fanotify_event_clear(fan_fd);
		(void)close(fan_fd);
		pr_inf("%s: "
			"%" PRIu64 " open, "
			"%" PRIu64 " close write, "
			"%" PRIu64 " close nowrite, "
			"%" PRIu64 " access, "
			"%" PRIu64 " modify\n",
			args->name,
			account.open,
			account.close_write,
			account.close_nowrite,
			account.access,
			account.modify);
	}
tidy:
	if (pid > 0) {
		int status;

		(void)kill(pid, SIGKILL);
		(void)shim_waitpid(pid, &status, 0);
	}
	(void)unlink(filename);
	(void)stress_temp_dir_rm_args(args);
	stress_mount_free(mnts, n_mnts);

	return rc;
}

stressor_info_t stress_fanotify_info = {
	.stressor = stress_fanotify,
	.supported = stress_fanotify_supported,
	.class = CLASS_FILESYSTEM | CLASS_SCHEDULER | CLASS_OS,
	.help = help
};
#else
stressor_info_t stress_fanotify_info = {
	.stressor = stress_not_implemented,
	.class = CLASS_FILESYSTEM | CLASS_SCHEDULER | CLASS_OS,
	.help = help
};
#endif
