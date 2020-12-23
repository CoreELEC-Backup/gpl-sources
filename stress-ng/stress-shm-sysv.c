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
	{ NULL,	"shm-sysv N",		"start N workers that exercise System V shared memory" },
	{ NULL,	"shm-sysv-ops N",	"stop after N shared memory bogo operations" },
	{ NULL,	"shm-sysv-bytes N",	"allocate and free N bytes of shared memory per loop" },
	{ NULL,	"shm-sysv-segs N",	"allocate N shared memory segments per iteration" },
	{ NULL,	NULL,			NULL }
};

#if defined(HAVE_SYS_IPC_H) &&	\
    defined(HAVE_SYS_SHM_H) &&	\
    defined(HAVE_SHM_SYSV)

#define KEY_GET_RETRIES		(40)
#define BITS_PER_BYTE		(8)
#define NUMA_LONG_BITS		(sizeof(unsigned long) * BITS_PER_BYTE)
#define MPOL_F_ADDR		(1 << 1)
#define MPOL_DEFAULT		(0)

/*
 *  Note, running this test with the --maximize option on
 *  low memory systems with many instances can trigger the
 *  OOM killer fairly easily.  The test tries hard to reap
 *  reap shared memory segments that are left over if the
 *  child is killed, however if the OOM killer kills the
 *  parent that does the reaping, then one can be left with
 *  a system with many shared segments still reserved and
 *  little free memory.
 */
typedef struct {
	int	index;
	int	shm_id;
} stress_shm_msg_t;

static const int shm_flags[] = {
#if defined(SHM_HUGETLB)
	SHM_HUGETLB,
#endif
#if defined(SHM_HUGE_2MB)
	SHM_HUGE_2MB,
#endif
#if defined(SHM_HUGE_1GB)
	SHM_HUGE_1GB,
#endif
/* This will segv if no backing, so don't use it for now */
/*
#if defined(SHM_NO_RESERVE)
	SHM_NO_RESERVE
#endif
*/
	0
};
#endif

static int stress_set_shm_sysv_bytes(const char *opt)
{
	size_t shm_sysv_bytes;

	shm_sysv_bytes = (size_t)stress_get_uint64_byte(opt);
	stress_check_range_bytes("shm-sysv-bytes", shm_sysv_bytes,
		MIN_SHM_SYSV_BYTES, MAX_MEM_LIMIT);
	return stress_set_setting("shm-sysv-bytes", TYPE_ID_SIZE_T, &shm_sysv_bytes);
}

static int stress_set_shm_sysv_segments(const char *opt)
{
	size_t shm_sysv_segments;

	shm_sysv_segments = (size_t)stress_get_uint64(opt);
	stress_check_range("shm-sysv-segs", shm_sysv_segments,
		MIN_SHM_SYSV_SEGMENTS, MAX_SHM_SYSV_SEGMENTS);
	return stress_set_setting("shm-sysv-segs", TYPE_ID_SIZE_T, &shm_sysv_segments);
}

static const stress_opt_set_func_t opt_set_funcs[] = {
	{ OPT_shm_bytes,		stress_set_shm_sysv_bytes },
	{ OPT_shm_sysv_segments,	stress_set_shm_sysv_segments },
	{ 0,				NULL }
};

#if defined(HAVE_SHM_SYSV)
/*
 *  stress_shm_sysv_check()
 *	simple check if shared memory is sane
 */
static int stress_shm_sysv_check(
	uint8_t *buf,
	const size_t sz,
	const size_t page_size)
{
	uint8_t *ptr, *end = buf + sz;
	uint8_t val;

	for (val = 0, ptr = buf; ptr < end; ptr += page_size, val++) {
		*ptr = val;
	}

	for (val = 0, ptr = buf; ptr < end; ptr += page_size, val++) {
		if (*ptr != val)
			return -1;

	}
	return 0;
}

#if defined(__linux__)
/*
 *  stress_shm_get_procinfo()
 *	exercise /proc/sysvipc/shm
 */
static void stress_shm_get_procinfo(bool *get_procinfo)
{
	int fd;
	static int count;

	if (++count & 0x3f)
		return;
	count = 0;

	fd = open("/proc/sysvipc/shm", O_RDONLY);
	if (fd < 0) {
		*get_procinfo = false;
		return;
	}
	for (;;) {
		ssize_t ret;
		char buffer[1024];

		ret = read(fd, buffer, sizeof(buffer));
		if (ret <= 0)
			break;
	}
	(void)close(fd);
}
#endif

/*
 *  stress_shm_sysv_child()
 * 	stress out the shm allocations. This can be killed by
 *	the out of memory killer, so we need to keep the parent
 *	informed of the allocated shared memory ids so these can
 *	be reaped cleanly if this process gets prematurely killed.
 */
static int stress_shm_sysv_child(
	const stress_args_t *args,
	const int fd,
	const size_t max_sz,
	const size_t page_size,
	const size_t shm_sysv_segments)
{
	void *addrs[MAX_SHM_SYSV_SEGMENTS];
	key_t keys[MAX_SHM_SYSV_SEGMENTS];
	int shm_ids[MAX_SHM_SYSV_SEGMENTS];
	stress_shm_msg_t msg;
	size_t i;
	int rc = EXIT_SUCCESS;
	bool ok = true;
	int mask = ~0;
	int32_t instances = args->num_instances;

	if (stress_sig_stop_stressing(args->name, SIGALRM) < 0)
		return EXIT_FAILURE;

	(void)memset(addrs, 0, sizeof(addrs));
	(void)memset(keys, 0, sizeof(keys));
	for (i = 0; i < MAX_SHM_SYSV_SEGMENTS; i++)
		shm_ids[i] = -1;

	/* Make sure this is killable by OOM killer */
	stress_set_oom_adjustment(args->name, true);

	/* Should never happen, but be safe */
	if (instances < 1)
		instances = 1;

	do {
		size_t sz = max_sz;
		pid_t pid = -1;

		for (i = 0; i < shm_sysv_segments; i++) {
			int shm_id, count = 0;
			void *addr;
			key_t key;
			size_t shmall, freemem, totalmem, freeswap;

			/* Try hard not to overcommit at this current time */
			stress_get_memlimits(&shmall, &freemem, &totalmem, &freeswap);
			shmall /= instances;
			freemem /= instances;
			if ((shmall > page_size) && sz > shmall)
				sz = shmall;
			if ((freemem > page_size) && sz > freemem)
				sz = freemem;
			if (!keep_stressing_flag())
				goto reap;

			for (count = 0; count < KEY_GET_RETRIES; count++) {
				bool unique = true;
				const int rnd =
					stress_mwc32() % SIZEOF_ARRAY(shm_flags);
				const int rnd_flag = shm_flags[rnd] & mask;

				if (sz < page_size)
					goto reap;

				/* Get a unique key */
				do {
					size_t j;

					if (!keep_stressing_flag())
						goto reap;

					/* Get a unique random key */
					key = (key_t)stress_mwc16();
					for (j = 0; j < i; j++) {
						if (key == keys[j]) {
							unique = false;
							break;
						}
					}
					if (!keep_stressing_flag())
						goto reap;

				} while (!unique);

				shm_id = shmget(key, sz,
					IPC_CREAT | IPC_EXCL |
					S_IRUSR | S_IWUSR | rnd_flag);
				if (shm_id >= 0)
					break;
				if (errno == EINTR)
					goto reap;
				if (errno == EPERM) {
					/* ignore using the flag again */
					mask &= ~rnd_flag;
				}
				if ((errno == EINVAL) || (errno == ENOMEM)) {
					/*
					 * On some systems we may need
					 * to reduce the size
					 */
					sz = sz / 2;
				}
			}
			if (shm_id < 0) {
				ok = false;
				pr_fail("%s: shmget failed, errno=%d (%s)\n",
					args->name, errno, strerror(errno));
				rc = EXIT_FAILURE;
				goto reap;
			}

			/* Inform parent of the new shm ID */
			msg.index = i;
			msg.shm_id = shm_id;
			if (write(fd, &msg, sizeof(msg)) < 0) {
				pr_err("%s: write failed: errno=%d: (%s)\n",
					args->name, errno, strerror(errno));
				rc = EXIT_FAILURE;
				goto reap;
			}

			addr = shmat(shm_id, NULL, 0);
			if (addr == (char *) -1) {
				ok = false;
				pr_fail("%s: shmat failed, errno=%d (%s)\n",
					args->name, errno, strerror(errno));
				rc = EXIT_FAILURE;
				goto reap;
			}
			addrs[i] = addr;
			shm_ids[i] = shm_id;
			keys[i] = key;

			if (!keep_stressing())
				goto reap;
			(void)stress_mincore_touch_pages(addr, sz);
			(void)shim_msync(addr, sz, stress_mwc1() ? MS_ASYNC : MS_SYNC);

			if (!keep_stressing())
				goto reap;
			(void)stress_madvise_random(addr, sz);

			if (!keep_stressing())
				goto reap;
			if (stress_shm_sysv_check(addr, sz, page_size) < 0) {
				ok = false;
				pr_fail("%s: memory check failed\n", args->name);
				rc = EXIT_FAILURE;
				goto reap;
			}
#if defined(SHM_LOCK) &&	\
    defined(SHM_UNLOCK)
			{
				int ret;

				ret = shmctl(shm_id, SHM_LOCK, NULL);
				if (ret == 0) {
					ret = shmctl(shm_id, SHM_UNLOCK, NULL);
					(void)ret;
				}
			}
#endif
#if defined(IPC_STAT) &&	\
    defined(HAVE_SHMID_DS)
			{
				struct shmid_ds ds;

				if (shmctl(shm_id, IPC_STAT, &ds) < 0)
					pr_fail("%s: shmctl IPC_STAT failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
#if defined(SHM_SET)
				else {
					int ret;

					ret = shmctl(shm_id, SHM_SET, &ds);
					(void)ret;
				}
#endif
			}
#endif
#if defined(IPC_INFO) &&	\
    defined(HAVE_SHMINFO)
			{
				struct shminfo s;

				if (shmctl(shm_id, IPC_INFO, (struct shmid_ds *)&s) < 0)
					pr_fail("%s: shmctl IPC_INFO failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
			}
#endif
#if defined(SHM_INFO) &&	\
    defined(HAVE_SHMINFO)
			{
				struct shm_info s;

				if (shmctl(shm_id, SHM_INFO, (struct shmid_ds *)&s) < 0)
					pr_fail("%s: shmctl SHM_INFO failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
			}
#endif
#if defined(SHM_LOCK) &&	\
    defined(SHM_UNLOCK)
			if (shmctl(shm_id, SHM_LOCK, (struct shmid_ds *)NULL) < 0) {
				int ret;

				ret = shmctl(shm_id, SHM_UNLOCK, (struct shmid_ds *)NULL);
				(void)ret;

			}
#endif
		/*
		 *  Exercise NUMA mem_policy on shm
		 */
#if defined(__NR_get_mempolicy) &&	\
    defined(__NR_set_mempolicy)
			{
				int ret, mode;
				unsigned long node_mask[NUMA_LONG_BITS];

				ret = shim_get_mempolicy(&mode, node_mask, 1,
					(unsigned long)addrs[i], MPOL_F_ADDR);
				if (ret == 0) {
					ret = shim_set_mempolicy(MPOL_DEFAULT, NULL, 1);
					(void)ret;
				}
				(void)ret;
			}
#endif

			inc_counter(args);
		}

		pid = fork();
		if (pid == 0) {
			for (i = 0; i < shm_sysv_segments; i++) {
				int ret;
#if defined(IPC_STAT) &&	\
    defined(HAVE_SHMID_DS)

				if (shm_ids[i] >= 0) {
					struct shmid_ds ds;
					ret = shmctl(shm_ids[i], IPC_STAT, &ds);
					(void)ret;
				}
#endif
				ret = shmdt(addrs[i]);
				(void)ret;
			}
			_exit(EXIT_SUCCESS);
		}
reap:
		for (i = 0; i < shm_sysv_segments; i++) {
			if (addrs[i]) {
				if (shmdt(addrs[i]) < 0) {
					pr_fail("%s: shmdt failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
				}
			}
			if (shm_ids[i] >= 0) {
				if (shmctl(shm_ids[i], IPC_RMID, NULL) < 0) {
					if ((errno != EIDRM) && (errno != EINVAL))
						pr_fail("%s: shmctl IPC_RMID failed, errno=%d (%s)\n",
							args->name, errno, strerror(errno));
				}
			}

			/* Inform parent shm ID is now free */
			msg.index = i;
			msg.shm_id = -1;
			if (write(fd, &msg, sizeof(msg)) < 0) {
				pr_dbg("%s: write failed: errno=%d: (%s)\n",
					args->name, errno, strerror(errno));
				ok = false;
			}
			addrs[i] = NULL;
			shm_ids[i] = -1;
			keys[i] = 0;
		}

		if (pid >= 0) {
			int status;

			(void)waitpid(pid, &status, 0);
		}
	} while (ok && keep_stressing());

	/* Inform parent of end of run */
	msg.index = -1;
	msg.shm_id = -1;
	if (write(fd, &msg, sizeof(msg)) < 0) {
		pr_err("%s: write failed: errno=%d: (%s)\n",
			args->name, errno, strerror(errno));
		rc = EXIT_FAILURE;
	}

	return rc;
}

/*
 *  stress_shm_sysv()
 *	stress SYSTEM V shared memory
 */
static int stress_shm_sysv(const stress_args_t *args)
{
	const size_t page_size = args->page_size;
	size_t orig_sz, sz;
	int pipefds[2];
	int rc = EXIT_SUCCESS;
	ssize_t i;
	pid_t pid;
	bool retry = true;
	uint32_t restarts = 0;
	size_t shm_sysv_bytes = DEFAULT_SHM_SYSV_BYTES;
	size_t shm_sysv_segments = DEFAULT_SHM_SYSV_SEGMENTS;

	if (!stress_get_setting("shm-sysv-bytes", &shm_sysv_bytes)) {
		if (g_opt_flags & OPT_FLAGS_MAXIMIZE)
			shm_sysv_bytes = MAX_SHM_SYSV_BYTES;
		if (g_opt_flags & OPT_FLAGS_MINIMIZE)
			shm_sysv_bytes = MIN_SHM_SYSV_BYTES;
	}
	if (shm_sysv_bytes < page_size)
		shm_sysv_bytes = page_size;

	if (!stress_get_setting("shm-sysv-segs", &shm_sysv_segments)) {
		if (g_opt_flags & OPT_FLAGS_MAXIMIZE)
			shm_sysv_segments = MAX_SHM_SYSV_SEGMENTS;
		if (g_opt_flags & OPT_FLAGS_MINIMIZE)
			shm_sysv_segments = MIN_SHM_SYSV_SEGMENTS;
	}
	shm_sysv_segments /= args->num_instances;
	if (shm_sysv_segments < 1)
		shm_sysv_segments = 1;

	orig_sz = sz = shm_sysv_bytes & ~(page_size - 1);

	while (keep_stressing_flag() && retry) {
		if (pipe(pipefds) < 0) {
			pr_fail("%s: pipe failed, errno=%d (%s)\n",
				args->name, errno, strerror(errno));
			return EXIT_FAILURE;
		}
fork_again:
		pid = fork();
		if (pid < 0) {
			/* Can't fork, retry? */
			if (errno == EAGAIN)
				goto fork_again;
			pr_err("%s: fork failed: errno=%d: (%s)\n",
				args->name, errno, strerror(errno));
			(void)close(pipefds[0]);
			(void)close(pipefds[1]);

			/* Nope, give up! */
			return EXIT_FAILURE;
		} else if (pid > 0) {
			/* Parent */
			int status, shm_ids[MAX_SHM_SYSV_SEGMENTS];
#if defined(__linux__)
			bool get_procinfo = true;
#endif

			(void)setpgid(pid, g_pgrp);
			stress_set_oom_adjustment(args->name, false);
			(void)close(pipefds[1]);

			for (i = 0; i < (ssize_t)MAX_SHM_SYSV_SEGMENTS; i++)
				shm_ids[i] = -1;

			while (keep_stressing_flag()) {
				stress_shm_msg_t msg;
				ssize_t n;

				/*
				 *  Blocking read on child shm ID info
				 *  pipe.  We break out if pipe breaks
				 *  on child death, or child tells us
				 *  about its demise.
				 */
				n = read(pipefds[0], &msg, sizeof(msg));
				if (n <= 0) {
					if ((errno == EAGAIN) || (errno == EINTR))
						continue;
					if (errno) {
						pr_fail("%s: read failed, errno=%d (%s)\n",
							args->name, errno, strerror(errno));
						break;
					}
					pr_fail("%s: zero bytes read\n", args->name);
					break;
				}
				if ((msg.index < 0) ||
				    (msg.index >= MAX_SHM_SYSV_SEGMENTS)) {
					retry = false;
					break;
				}
				shm_ids[msg.index] = msg.shm_id;
#if defined(__linux__)
				if (get_procinfo)
					stress_shm_get_procinfo(&get_procinfo);
#endif
			}
			(void)kill(pid, SIGALRM);
			(void)shim_waitpid(pid, &status, 0);
			if (WIFSIGNALED(status)) {
				if ((WTERMSIG(status) == SIGKILL) ||
				    (WTERMSIG(status) == SIGBUS)) {
					stress_log_system_mem_info();
					pr_dbg("%s: assuming killed by OOM killer, "
						"restarting again (instance %d)\n",
						args->name, args->instance);
					restarts++;
				}
			}
			(void)close(pipefds[0]);
			/*
			 *  The child may have been killed by the OOM killer or
			 *  some other way, so it may have left the shared
			 *  memory segment around.  At this point the child
			 *  has died, so we should be able to remove the
			 *  shared memory segment.
			 */
			for (i = 0; i < (ssize_t)shm_sysv_segments; i++) {
				if (shm_ids[i] != -1)
					(void)shmctl(shm_ids[i], IPC_RMID, NULL);
			}
		} else if (pid == 0) {
			/* Child, stress memory */
			(void)setpgid(0, g_pgrp);
			stress_parent_died_alarm();
			(void)sched_settings_apply(true);

			/*
			 * Nicing the child may OOM it first as this
			 * doubles the OOM score
			 */
			if (nice(5) < 0)
				pr_dbg("%s: nice of child failed, "
					"(instance %d)\n", args->name, args->instance);

			(void)close(pipefds[0]);
			rc = stress_shm_sysv_child(args, pipefds[1], sz, page_size, shm_sysv_segments);
			(void)close(pipefds[1]);
			_exit(rc);
		}
	}
	if (orig_sz != sz)
		pr_dbg("%s: reduced shared memory size from "
			"%zu to %zu bytes\n", args->name, orig_sz, sz);
	if (restarts) {
		pr_dbg("%s: OOM restarts: %" PRIu32 "\n",
			args->name, restarts);
	}
	return rc;
}

stressor_info_t stress_shm_sysv_info = {
	.stressor = stress_shm_sysv,
	.class = CLASS_VM | CLASS_OS,
	.opt_set_funcs = opt_set_funcs,
	.help = help
};
#else
stressor_info_t stress_shm_sysv_info = {
	.stressor = stress_not_implemented,
	.class = CLASS_VM | CLASS_OS,
	.opt_set_funcs = opt_set_funcs,
	.help = help
};
#endif
