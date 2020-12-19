/*
 * Copyright © 2019 Red Hat, Inc.
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
#include "test-common.h"

START_TEST(test_info)
{
	struct libevdev *d = libevdev_new();

	libevdev_set_name(d, "some name");
	ck_assert_str_eq(libevdev_get_name(d), "some name");
	libevdev_set_phys(d, "physical");
	ck_assert_str_eq(libevdev_get_phys(d), "physical");
	libevdev_set_uniq(d, "very unique");
	ck_assert_str_eq(libevdev_get_uniq(d), "very unique");

	libevdev_set_id_bustype(d, 1);
	libevdev_set_id_vendor(d, 2);
	libevdev_set_id_product(d, 3);
	libevdev_set_id_version(d, 4);
	ck_assert_int_eq(libevdev_get_id_bustype(d), 1);
	ck_assert_int_eq(libevdev_get_id_vendor(d), 2);
	ck_assert_int_eq(libevdev_get_id_product(d), 3);
	ck_assert_int_eq(libevdev_get_id_version(d), 4);

	libevdev_free(d);
}
END_TEST

START_TEST(test_properties)
{
	for (unsigned prop = 0; prop < INPUT_PROP_CNT; prop++) {
		struct libevdev *d = libevdev_new();

		ck_assert(!libevdev_has_property(d, prop));
		libevdev_enable_property(d, prop);
		ck_assert(libevdev_has_property(d, prop));
		libevdev_free(d);
	}
}
END_TEST

START_TEST(test_bits)
{
	for (unsigned type = 1; type < EV_CNT; type++) {
		unsigned max = libevdev_event_type_get_max(type);

		if((int)max == -1)
			continue;

		for (unsigned code = 0; code <= max; code++) {
			struct libevdev *d = libevdev_new();
			const struct input_absinfo abs = {
				10, 20, 30, 40, 50
			};
			const void *data = NULL;

			if (type == EV_ABS || type == EV_REP)
				data = &abs;

			ck_assert(!libevdev_has_event_code(d, type, code));
			libevdev_enable_event_code(d, type, code, data);
			ck_assert(libevdev_has_event_code(d, type, code));
			libevdev_free(d);
		}
	}
}
END_TEST

START_TEST(test_mt_slots_enable_disable)
{
	struct libevdev *d = libevdev_new();
	struct input_absinfo abs = {0};

	abs.maximum = 5;
	libevdev_enable_event_code(d, EV_ABS, ABS_MT_SLOT, &abs);
	ck_assert(libevdev_has_event_code(d, EV_ABS, ABS_MT_SLOT));
	ck_assert_int_eq(libevdev_get_num_slots(d), 6);

	libevdev_disable_event_code(d, EV_ABS, ABS_MT_SLOT);
	ck_assert(!libevdev_has_event_code(d, EV_ABS, ABS_MT_SLOT));
	ck_assert_int_eq(libevdev_get_num_slots(d), -1);

	abs.maximum = 2;
	libevdev_enable_event_code(d, EV_ABS, ABS_MT_SLOT, &abs);
	ck_assert(libevdev_has_event_code(d, EV_ABS, ABS_MT_SLOT));
	ck_assert_int_eq(libevdev_get_num_slots(d), 3);

	libevdev_free(d);
}
END_TEST

START_TEST(test_mt_slots_increase_decrease)
{
	struct libevdev *d = libevdev_new();
	struct input_absinfo abs = {0};

	abs.maximum = 5;
	libevdev_enable_event_code(d, EV_ABS, ABS_MT_SLOT, &abs);
	ck_assert(libevdev_has_event_code(d, EV_ABS, ABS_MT_SLOT));
	ck_assert_int_eq(libevdev_get_num_slots(d), 6);

	abs.maximum = 2;
	libevdev_enable_event_code(d, EV_ABS, ABS_MT_SLOT, &abs);
	ck_assert(libevdev_has_event_code(d, EV_ABS, ABS_MT_SLOT));
	ck_assert_int_eq(libevdev_get_num_slots(d), 3);

	abs.maximum = 6;
	libevdev_enable_event_code(d, EV_ABS, ABS_MT_SLOT, &abs);
	ck_assert(libevdev_has_event_code(d, EV_ABS, ABS_MT_SLOT));
	ck_assert_int_eq(libevdev_get_num_slots(d), 7);

	abs.maximum = 10;
	libevdev_enable_event_code(d, EV_ABS, ABS_MT_SLOT, &abs);
	ck_assert(libevdev_has_event_code(d, EV_ABS, ABS_MT_SLOT));
	ck_assert_int_eq(libevdev_get_num_slots(d), 11);

	libevdev_free(d);
}
END_TEST

START_TEST(test_mt_tracking_id)
{
	struct libevdev *d = libevdev_new();
	struct input_absinfo abs = { .maximum = 5 };

	libevdev_enable_event_code(d, EV_ABS, ABS_MT_SLOT, &abs);

	/* Not yet enabled, so 0. This is technically undefined */
	for (int slot = 0; slot < 5; slot++)
		ck_assert_int_eq(libevdev_get_slot_value(d, 0, ABS_MT_TRACKING_ID), 0);

	libevdev_enable_event_code(d, EV_ABS, ABS_MT_TRACKING_ID, &abs);

	for (int slot = 0; slot < 5; slot++)
		ck_assert_int_eq(libevdev_get_slot_value(d, 0, ABS_MT_TRACKING_ID), -1);

	libevdev_free(d);
}
END_TEST

TEST_SUITE(event_name_suite)
{
	Suite *s = suite_create("Context manipulation");
	TCase *tc;

	tc = tcase_create("Device info");
	tcase_add_test(tc, test_info);
	tcase_add_test(tc, test_properties);
	tcase_add_test(tc, test_bits);
	tcase_add_test(tc, test_mt_slots_enable_disable);
	tcase_add_test(tc, test_mt_slots_increase_decrease);
	tcase_add_test(tc, test_mt_tracking_id);
	suite_add_tcase(s, tc);

	return s;
}
