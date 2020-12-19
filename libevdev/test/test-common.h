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
#include <libevdev/libevdev.h>
#include <stdbool.h>

#include <check.h>

#ifndef _TEST_COMMON_H_
#define _TEST_COMMON_H_

struct libevdev_test {
	const char *name;
	Suite* (*setup)(void);
	bool needs_root_privileges;
} __attribute__((aligned(16)));

#define _TEST_SUITE(name, root_privs) \
	static Suite* (name##_setup)(void); \
	static const struct libevdev_test _test \
	__attribute__((used)) \
	__attribute__((section ("test_section"))) = { \
		#name, name##_setup, root_privs \
	}; \
	static Suite* (name##_setup)(void)

#define TEST_SUITE(name) \
	_TEST_SUITE(name, false)

#define TEST_SUITE_ROOT_PRIVILEGES(name) \
	_TEST_SUITE(name, true)

#define TEST_DEVICE_NAME "libevdev test device"

#include "test-common-uinput.h"

void test_create_device(struct uinput_device **uidev,
			struct libevdev **dev,
			...);
void test_create_abs_device(struct uinput_device **uidev,
			    struct libevdev **dev,
			    int nabs,
			    const struct input_absinfo *abs,
			    ...);

void test_logfunc_abort_on_error(enum libevdev_log_priority priority,
				 void *data,
				 const char *file, int line,
				 const char *func,
				 const char *format, va_list args);
void test_logfunc_ignore_error(enum libevdev_log_priority priority,
			       void *data,
			       const char *file, int line,
			       const char *func,
			       const char *format, va_list args);
#endif /* _TEST_COMMON_H_ */
