/*
 * Copyright © 2013 Red Hat, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#include <config.h>
#include <check.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libevdev/libevdev.h>

#include "test-common.h"

static int
is_debugger_attached(void)
{
	int status;
	int rc;
	int pid = fork();

	if (pid == -1)
		return 0;

	if (pid == 0) {
		int ppid = getppid();
		if (ptrace(PTRACE_ATTACH, ppid, NULL, NULL) == 0) {
			waitpid(ppid, NULL, 0);
			ptrace(PTRACE_CONT, NULL, NULL);
			ptrace(PTRACE_DETACH, ppid, NULL, NULL);
			rc = 0;
		} else
			rc = 1;
		_exit(rc);
	} else {
		waitpid(pid, &status, 0);
		rc = WEXITSTATUS(status);
	}

	return rc;
}

static bool
device_nodes_exist(void)
{
	struct stat st;
	int rc;

	rc = stat("/dev/uinput", &st);
	if (rc == -1 && errno == ENOENT)
		return false;

	rc = stat("/dev/input", &st);
	if (rc == -1 && errno == ENOENT)
		return false;

	/* Any issues but ENOENT we just let the test suite blow up later */
	return true;
}

extern const struct libevdev_test __start_test_section, __stop_test_section;

int main(void)
{
	const struct libevdev_test *t;
	const struct rlimit corelimit = {0, 0};
	int failed;

	for (t = &__start_test_section; t < &__stop_test_section; t++) {
		if (t->needs_root_privileges) {
			if (getenv("LIBEVDEV_SKIP_ROOT_TESTS"))
				return 77;

			if (getuid() != 0) {
				fprintf(stderr, "This test needs to run as root\n");
				return 77;
			}
			if (!device_nodes_exist()) {
				fprintf(stderr, "This test needs /dev/input and /dev/uinput to exist\n");
				return 77;
			}

			break;
		}
	}

	if (is_debugger_attached())
		setenv("CK_FORK", "no", 0);

	if (setrlimit(RLIMIT_CORE, &corelimit) != 0)
		perror("WARNING: Core dumps not disabled. Reason");

	libevdev_set_log_function(test_logfunc_abort_on_error, NULL);

	SRunner *sr = srunner_create(NULL);
	for (t = &__start_test_section; t < &__stop_test_section; t++) {
		srunner_add_suite(sr, t->setup());
	}

	srunner_run_all(sr, CK_NORMAL);

	failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return failed;
}
