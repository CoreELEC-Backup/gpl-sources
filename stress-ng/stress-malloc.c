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
	{ NULL,	"malloc N",		"start N workers exercising malloc/realloc/free" },
	{ NULL,	"malloc-bytes N",	"allocate up to N bytes per allocation" },
	{ NULL,	"malloc-max N",		"keep up to N allocations at a time" },
	{ NULL,	"malloc-ops N",		"stop after N malloc bogo operations" },
	{ NULL,	"malloc-thresh N",	"threshold where malloc uses mmap instead of sbrk" },
	{ NULL,	NULL,			NULL }
};

static int stress_set_malloc_bytes(const char *opt)
{
	size_t malloc_bytes;

	malloc_bytes = (size_t)stress_get_uint64_byte_memory(opt, 1);
	stress_check_range_bytes("malloc-bytes", malloc_bytes,
		MIN_MALLOC_BYTES, MAX_MEM_LIMIT);
	return stress_set_setting("malloc-bytes", TYPE_ID_SIZE_T, &malloc_bytes);
}

static int stress_set_malloc_max(const char *opt)
{
	size_t malloc_max;

	malloc_max = (size_t)stress_get_uint64_byte(opt);
	stress_check_range("malloc-max", malloc_max,
		MIN_MALLOC_MAX, MAX_MALLOC_MAX);
	return stress_set_setting("malloc-max", TYPE_ID_SIZE_T, &malloc_max);
}

static int stress_set_malloc_threshold(const char *opt)
{
	size_t malloc_threshold;

	malloc_threshold = (size_t)stress_get_uint64_byte(opt);
	stress_check_range("malloc-threshold", malloc_threshold,
		MIN_MALLOC_THRESHOLD, MAX_MALLOC_THRESHOLD);
	return stress_set_setting("malloc-threshold", TYPE_ID_SIZE_T, &malloc_threshold);
}

/*
 *  stress_alloc_size()
 *	get a new allocation size, ensuring
 *	it is never zero bytes.
 */
static inline size_t stress_alloc_size(const size_t malloc_bytes)
{
	const size_t len = stress_mwc64() % malloc_bytes;

	return len ? len : 1;
}

static int stress_malloc_child(const stress_args_t *args, void *context)
{
	size_t j;
	size_t malloc_bytes = DEFAULT_MALLOC_BYTES;
	size_t malloc_max = DEFAULT_MALLOC_MAX;
#if defined(__GNUC__) && defined(M_MMAP_THRESHOLD) && defined(HAVE_MALLOPT)
	size_t malloc_threshold = DEFAULT_MALLOC_THRESHOLD;
#endif
	void **addr;

	(void)context;

	if (!stress_get_setting("malloc-bytes", &malloc_bytes)) {
		if (g_opt_flags & OPT_FLAGS_MAXIMIZE)
			malloc_bytes = MAX_32;
		if (g_opt_flags & OPT_FLAGS_MINIMIZE)
			malloc_bytes = MIN_MALLOC_BYTES;
	}

	malloc_bytes /= args->num_instances;
	if (malloc_bytes < MIN_MALLOC_BYTES)
		malloc_bytes = MIN_MALLOC_BYTES;

	if (!stress_get_setting("malloc-max", &malloc_max)) {
		if (g_opt_flags & OPT_FLAGS_MAXIMIZE)
			malloc_max = MAX_MALLOC_MAX;
		if (g_opt_flags & OPT_FLAGS_MINIMIZE)
			malloc_max = MIN_MALLOC_MAX;
	}

#if defined(__GNUC__) && defined(M_MMAP_THRESHOLD) && defined(HAVE_MALLOPT)
	if (stress_get_setting("malloc-threshold", &malloc_threshold))
		(void)mallopt(M_MMAP_THRESHOLD, (int)malloc_threshold);
#endif
	addr = (void **)calloc(malloc_max, sizeof(void *));
	if (!addr) {
		pr_dbg("%s: cannot allocate address buffer: %d (%s)\n",
			args->name, errno, strerror(errno));
		return EXIT_NO_RESOURCE;
	}

	do {
		const unsigned int rnd = stress_mwc32();
		const unsigned int i = rnd % malloc_max;
		const unsigned int action = (rnd >> 12) & 1;
		const unsigned int do_calloc = (rnd >> 14) & 0x1f;

		/*
		 * With many instances running it is wise to
		 * double check before the next allocation as
		 * sometimes process start up is delayed for
		 * some time and we should bail out before
		 * exerting any more memory pressure
		 */
		if (!keep_stressing_flag())
			goto abort;

		if (addr[i]) {
			/* 50% free, 50% realloc */
			if (action) {
				free(addr[i]);
				addr[i] = NULL;
				inc_counter(args);
			} else {
				void *tmp;
				const size_t len = stress_alloc_size(malloc_bytes);

				tmp = realloc(addr[i], len);
				if (tmp) {
					addr[i] = tmp;
					(void)stress_mincore_touch_pages_interruptible(addr[i], len);
					inc_counter(args);
				}
			}
		} else {
			/* 50% free, 50% alloc */
			if (action) {
				size_t len = stress_alloc_size(malloc_bytes);

				if (do_calloc == 0) {
					size_t n = ((rnd >> 15) % 17) + 1;
					addr[i] = calloc(n, len / n);
					len = n * (len / n);
				} else {
					addr[i] = malloc(len);
				}
				if (addr[i]) {
					inc_counter(args);
					(void)stress_mincore_touch_pages_interruptible(addr[i], len);
				}
			}
		}
	} while (keep_stressing());
abort:
	for (j = 0; j < malloc_max; j++) {
		free(addr[j]);
	}
	free(addr);

	return EXIT_SUCCESS;
}

/*
 *  stress_malloc()
 *	stress malloc by performing a mix of
 *	allocation and frees
 */
static int stress_malloc(const stress_args_t *args)
{
	return stress_oomable_child(args, NULL, stress_malloc_child, STRESS_OOMABLE_NORMAL);
}

static const stress_opt_set_func_t opt_set_funcs[] = {
	{ OPT_malloc_threshold,	stress_set_malloc_threshold },
	{ OPT_malloc_max,	stress_set_malloc_max },
	{ OPT_malloc_bytes,	stress_set_malloc_bytes },
	{ 0,		NULL }
};

stressor_info_t stress_malloc_info = {
	.stressor = stress_malloc,
	.class = CLASS_CPU_CACHE | CLASS_MEMORY | CLASS_VM | CLASS_OS,
	.opt_set_funcs = opt_set_funcs,
	.help = help
};
