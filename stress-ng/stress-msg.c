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
	{ NULL,	"msg N",	"start N workers stressing System V messages" },
	{ NULL,	"msg-ops N",	"stop msg workers after N bogo messages" },
	{ NULL, "msg-types N",	"enable N different message types" },
	{ NULL,	NULL,		NULL }
};

static int stress_set_msg_types(const char *opt) {
	int32_t msg_types;

	msg_types = stress_get_int32(opt);
	stress_check_range("msg-types", msg_types, 0, 100);
	return stress_set_setting("msg-types", TYPE_ID_INT32, &msg_types);
}

static const stress_opt_set_func_t opt_set_funcs[] = {
	{ OPT_msg_types,	stress_set_msg_types },
	{ 0,                    NULL },
};

#if defined(HAVE_SYS_IPC_H) &&	\
    defined(HAVE_SYS_MSG_H) &&	\
    defined(HAVE_MQ_SYSV)

typedef struct {
	long mtype;
	uint32_t value;
} stress_msg_t;

static int stress_msg_get_stats(const stress_args_t *args, const int msgq_id)
{
	struct msqid_ds buf;

	if (msgctl(msgq_id, IPC_STAT, &buf) < 0) {
		pr_fail("%s: msgctl IPC_STAT failed, errno=%d (%s)\n",
			args->name, errno, strerror(errno));
		return -errno;
	}

#if defined(MSG_STAT_ANY)
	/*
	 * select random msgq index numbers, we may hit
	 * some that are in use. Ignore failures
	 */
	(void)msgctl(stress_mwc8() % (msgq_id + 1), MSG_STAT_ANY, &buf);
#endif

#if defined(IPC_INFO) &&	\
    defined(HAVE_MSGINFO)
	{
		struct msginfo info;

		if (msgctl(msgq_id, IPC_INFO, (struct msqid_ds *)&info) < 0) {
			pr_fail("%s: msgctl IPC_INFO failed, errno=%d (%s)\n",
				args->name, errno, strerror(errno));
			return -errno;
		}
	}
#endif
#if defined(MSG_INFO) &&	\
    defined(HAVE_MSGINFO)
	{
		struct msginfo info;

		if (msgctl(msgq_id, MSG_INFO, (struct msqid_ds *)&info) < 0) {
			pr_fail("%s: msgctl MSG_INFO failed, errno=%d (%s)\n",
				args->name, errno, strerror(errno));
			return -errno;
		}
	}
#endif

	return 0;
}

#if defined(__linux__)
/*
 *  stress_msg_get_procinfo()
 *	exercise /proc/sysvipc/msg
 */
static void stress_msg_get_procinfo(bool *get_procinfo)
{
	int fd;

	fd = open("/proc/sysvipc/msg", O_RDONLY);
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
 *  stress_msg
 *	stress by message queues
 */
static int stress_msg(const stress_args_t *args)
{
	pid_t pid;
	int msgq_id;
	int32_t msg_types = 0;
#if defined(__linux__)
	bool get_procinfo = true;
#endif

	(void)stress_get_setting("msg-types", &msg_types);

	msgq_id = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL);
	if (msgq_id < 0) {
		pr_fail("%s: msgget failed, errno=%d (%s)\n",
			args->name, errno, strerror(errno));
		return exit_status(errno);
	}
	pr_dbg("%s: System V message queue created, id: %d\n", args->name, msgq_id);

again:
	pid = fork();
	if (pid < 0) {
		if (keep_stressing_flag() &&
		    ((errno == EAGAIN) || (errno == ENOMEM)))
			goto again;
		pr_fail("%s: fork failed, errno=%d (%s)\n",
			args->name, errno, strerror(errno));
		return EXIT_FAILURE;
	} else if (pid == 0) {
		(void)setpgid(0, g_pgrp);
		stress_parent_died_alarm();
		(void)sched_settings_apply(true);

		while (keep_stressing()) {
			stress_msg_t ALIGN64 msg;
			register uint32_t i;
			register const long mtype = msg_types == 0 ? 0 : -(msg_types + 1);

			for (i = 0; keep_stressing(); i++) {
#if defined(MSG_COPY) &&	\
    defined(IPC_NOWAIT)
				/*
				 *  Very occasionally peek with a MSG_COPY, ignore
				 *  the return as we just want to exercise the flag
				 *  and we don't care if it succeeds or not
				 */
				if ((i & 0xfff) == 0) {
					int rc;

					rc = msgrcv(msgq_id, &msg, sizeof(msg.value), mtype,
						MSG_COPY | IPC_NOWAIT);
					(void)rc;
				}
#endif
				if (msgrcv(msgq_id, &msg, sizeof(msg.value), mtype, 0) < 0) {
					pr_fail("%s: msgrcv failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
					break;
				}
				/*
				 *  Only when msg_types is not set can we fetch
				 *  data in an ordered FIFO to sanity check data
				 *  ordering.
				 */
				if ((msg_types == 0) && (g_opt_flags & OPT_FLAGS_VERIFY)) {
					if (msg.value != i)
						pr_fail("%s: msgrcv: expected msg containing 0x%" PRIx32
							" but received 0x%" PRIx32 " instead\n",
							 args->name, i, msg.value);
				}
			}
			_exit(EXIT_SUCCESS);
		}
	} else {
		stress_msg_t ALIGN64 msg;
		int status;

		/* Parent */
		(void)setpgid(pid, g_pgrp);

		msg.value = 0;

		do {
			msg.mtype = (msg_types) ? (stress_mwc8() % msg_types) + 1 : 1;
			if (msgsnd(msgq_id, &msg, sizeof(msg.value), 0) < 0) {
				if (errno != EINTR)
					pr_fail("%s: msgsnd failed, errno=%d (%s)\n",
						args->name, errno, strerror(errno));
				break;
			}
			msg.value++;
			inc_counter(args);
			if ((msg.value & 0xff) == 0) {
				if (stress_msg_get_stats(args, msgq_id) < 0)
					break;
#if defined(__NetBSD__)
				/*
				 *  NetBSD can shove loads of messages onto
				 *  a queue before it blocks, so force
				 *  a scheduling yield every so often so that
				 *  consumer can read them.
				 */
				(void)shim_sched_yield();
#endif
			}

#if defined(__linux__)
			/*
			 *  Periodically read /proc/sysvipc/msg to exercise
			 *  this interface if it exists
			 */
			if (get_procinfo && ((msg.value & 0xffff) == 0))
				stress_msg_get_procinfo(&get_procinfo);
#endif

		} while (keep_stressing());

		(void)kill(pid, SIGKILL);
		(void)shim_waitpid(pid, &status, 0);

		if (msgctl(msgq_id, IPC_RMID, NULL) < 0)
			pr_fail("%s: msgctl failed, errno=%d (%s)\n",
				args->name, errno, strerror(errno));
		else
			pr_dbg("%s: System V message queue deleted, id: %d\n", args->name, msgq_id);
	}
	return EXIT_SUCCESS;
}

stressor_info_t stress_msg_info = {
	.stressor = stress_msg,
	.class = CLASS_SCHEDULER | CLASS_OS,
	.opt_set_funcs = opt_set_funcs,
	.help = help
};
#else
stressor_info_t stress_msg_info = {
	.stressor = stress_not_implemented,
	.class = CLASS_SCHEDULER | CLASS_OS,
	.opt_set_funcs = opt_set_funcs,
	.help = help
};
#endif
