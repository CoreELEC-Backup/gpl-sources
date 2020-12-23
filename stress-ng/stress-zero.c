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
	{ NULL,	"zero N",	"start N workers reading /dev/zero" },
	{ NULL,	"zero-ops N",	"stop after N /dev/zero bogo read operations" },
	{ NULL,	NULL,		NULL }
};

static inline bool stress_is_not_zero(char *buffer, size_t len)
{
	register const char *end = buffer + len;
	register char *ptr = buffer;

	while (ptr < end) {
		if (*ptr)
			return true;
		ptr++;
	}
	return false;
}

/*
 *  stress_zero
 *	stress reading of /dev/zero
 */
static int stress_zero(const stress_args_t *args)
{
	int fd;
	const size_t page_size = args->page_size;
#if defined(__minix__)
	const int flags = O_RDONLY;
#else
	const int flags = O_RDWR;
#endif
	char wr_buffer[page_size];

	if ((fd = open("/dev/zero", flags)) < 0) {
		pr_fail("%s: open /dev/zero failed, errno=%d (%s)\n",
			args->name, errno, strerror(errno));
		return EXIT_FAILURE;
	}

	(void)memset(wr_buffer, 0, sizeof wr_buffer);

	do {
		char rd_buffer[page_size];
		ssize_t ret;
#if defined(__linux__)
		int32_t *ptr;
#endif

		ret = read(fd, rd_buffer, sizeof(rd_buffer));
		if (ret < 0) {
			if ((errno == EAGAIN) || (errno == EINTR))
				continue;
			pr_fail("%s: read failed, errno=%d (%s)\n",
				args->name, errno, strerror(errno));
			(void)close(fd);
			return EXIT_FAILURE;
		}
		if (stress_is_not_zero(rd_buffer, (size_t)ret)) {
			pr_fail("%s: non-zero value from a read of /dev/zero\n",
				args->name);
		}

#if !defined(__minix__)
		/* One can also write to /dev/zero w/o failure */
		ret = write(fd, wr_buffer, sizeof(wr_buffer));
		if (ret < 0) {
			if ((errno == EAGAIN) || (errno == EINTR))
				continue;
			pr_fail("%s: write failed, errno=%d (%s)\n",
				args->name, errno, strerror(errno));
			(void)close(fd);
			return EXIT_FAILURE;
		}
#endif

#if defined(__linux__)
		/*
		 *  check if we can mmap /dev/zero
		 */
		ptr = mmap(NULL, page_size, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS,
			fd, page_size * stress_mwc16());
		if (ptr == MAP_FAILED) {
			if ((errno == ENOMEM) || (errno == EAGAIN))
				continue;
			pr_fail("%s: mmap /dev/zero failed, errno=%d (%s)\n",
				args->name, errno, strerror(errno));
			(void)close(fd);
			return EXIT_FAILURE;
		}
		if (stress_is_not_zero(rd_buffer, (size_t)ret)) {
			pr_fail("%s: memory mapped page of /dev/zero is not zero\n",
				args->name);
		}
		(void)munmap(ptr, page_size);
#endif


		/*
		 *  lseek on /dev/zero just because we can
		 */
		(void)lseek(fd, SEEK_SET, 0);
		(void)lseek(fd, SEEK_END, 0);
		(void)lseek(fd, SEEK_CUR, 0);

		inc_counter(args);
	} while (keep_stressing());
	(void)close(fd);

	return EXIT_SUCCESS;
}

stressor_info_t stress_zero_info = {
	.stressor = stress_zero,
	.class = CLASS_DEV | CLASS_MEMORY | CLASS_OS,
	.help = help
};
