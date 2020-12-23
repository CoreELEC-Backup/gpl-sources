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

#define _GNU_SOURCE

#include <sys/mman.h>
#include <unistd.h>
#include <features.h>

#include "../stress-version.h"
#include <sys/syscall.h>

int main(void)
{
	int ret;

#if NEED_GLIBC(2,27,0)
        ret = pkey_mprotect((void *)0, 4096, PROT_NONE, -1);
	(void)ret;
#endif
        ret = syscall(__NR_pkey_alloc, (void *)0, 4096, PROT_NONE, -1);
	(void)ret;

	return 0;
}
