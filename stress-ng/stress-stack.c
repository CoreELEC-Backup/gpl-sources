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

static sigjmp_buf jmp_env;

static const stress_help_t help[] = {
	{ NULL,	"stack N",	"start N workers generating stack overflows" },
	{ NULL,	"stack-ops N",	"stop after N bogo stack overflows" },
	{ NULL,	"stack-fill",	"fill stack, touches all new pages " },
	{ NULL,	NULL,		NULL }
};

static int stress_set_stack_fill(const char *opt)
{
	bool stack_fill = true;

	(void)opt;
	return stress_set_setting("stack-fill", TYPE_ID_BOOL, &stack_fill);
}

static const stress_opt_set_func_t opt_set_funcs[] = {
	{ OPT_stack_fill,	stress_set_stack_fill },
	{ 0,			NULL }
};

/*
 *  stress_segvhandler()
 *	SEGV handler
 */
static void MLOCKED_TEXT stress_segvhandler(int signum)
{
	(void)signum;

	siglongjmp(jmp_env, 1);		/* Ugly, bounce back */
}

/*
 *  stress_stack_alloc()
 *	eat up stack. The default is to eat up lots of pages
 *	but only have 25% of the pages actually in memory
 *	so we a large stack with lots of pages not physically
 *	resident.
 */
static void stress_stack_alloc(const stress_args_t *args, const bool stack_fill)
{
	const size_t sz = 256 * KB;
	const size_t page_size4 = (args->page_size << 2);
	uint8_t data[sz];

	if (stack_fill) {
		(void)memset(data, 0, sz);
	} else {
		register size_t i;

		/*
		 *  Touch 25% of the pages, ensure data
		 *  is random and non-zero to avoid
		 *  kernel same page merging
		 */
		for (i = 0; i < sz; i += page_size4) {
			uint32_t *ptr = (uint32_t *)(data + i);

			*ptr = stress_mwc32();
			*(ptr + 1) = stress_mwc32() | 1;
		}
	}

	inc_counter(args);

	if (keep_stressing())
		stress_stack_alloc(args, stack_fill);
}

static int stress_stack_child(const stress_args_t *args, void *context)
{
	char *start_ptr = shim_sbrk(0);
	void *altstack;
	ssize_t altstack_size = (SIGSTKSZ +
				 STACK_ALIGNMENT +
				 args->page_size) & ~(args->page_size -1);
	bool stack_fill = false;

	(void)context;

	(void)stress_get_setting("stack-fill", &stack_fill);

	/*
	 *  Allocate altstack on heap rather than an
	 *  autoexpanding stack that may trip a segfault
	 *  if there is no memory to back it later. Stack
	 *  must be privately mapped.
	 */
	altstack = mmap(NULL, altstack_size, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (altstack == MAP_FAILED)
		return EXIT_NO_RESOURCE;
	(void)stress_mincore_touch_pages(altstack, altstack_size);

	/*
	 *  We need to create an alternative signal
	 *  stack so when a segfault occurs we use
	 *  this already allocated signal stack rather
	 *  than try to push onto an already overflowed
	 *  stack
	 */
	if (stress_sigaltstack(altstack, SIGSTKSZ) < 0) {
		(void)munmap(altstack, altstack_size);
		return EXIT_NO_RESOURCE;
	}

	(void)setpgid(0, g_pgrp);
	stress_parent_died_alarm();

	if (start_ptr == (void *) -1) {
		pr_err("%s: sbrk(0) failed: errno=%d (%s)\n",
			args->name, errno, strerror(errno));
		return EXIT_FAILURE;
	}

	/* Make sure this is killable by OOM killer */
	stress_set_oom_adjustment(args->name, true);

	for (;;) {
		struct sigaction new_action;
		int ret;

		if (!keep_stressing())
			break;

		(void)memset(&new_action, 0, sizeof new_action);
		new_action.sa_handler = stress_segvhandler;
		(void)sigemptyset(&new_action.sa_mask);
		new_action.sa_flags = SA_ONSTACK;

		if (sigaction(SIGSEGV, &new_action, NULL) < 0) {
			pr_fail("%s: sigaction on SIGSEGV failed, errno=%d (%s)\n",
				args->name, errno, strerror(errno));
			return EXIT_FAILURE;
		}
		if (sigaction(SIGBUS, &new_action, NULL) < 0) {
			pr_fail("%s: sigaction on SIGBUS failed, errno=%d (%s)\n",
				args->name, errno, strerror(errno));
			return EXIT_FAILURE;
		}
		ret = sigsetjmp(jmp_env, 1);
		/*
		 * We return here if we segfault, so
		 * first check if we need to terminate
		 */
		if (!keep_stressing())
			break;

		if (ret) {
			/* We end up here after handling the fault */
			inc_counter(args);
		} else {
			/* Expand the stack and cause a fault */
			stress_stack_alloc(args, stack_fill);
		}
	}
	(void)munmap(altstack, altstack_size);

	return EXIT_SUCCESS;
}

/*
 *  stress_stack
 *	stress by forcing stack overflows
 */
static int stress_stack(const stress_args_t *args)
{
	return stress_oomable_child(args, NULL, stress_stack_child, STRESS_OOMABLE_NORMAL);
}

stressor_info_t stress_stack_info = {
	.stressor = stress_stack,
	.class = CLASS_VM | CLASS_MEMORY,
	.opt_set_funcs = opt_set_funcs,
	.help = help
};
