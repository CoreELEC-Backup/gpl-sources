/*
 * Copyright © 2014 Red Hat, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "config.h"

#include "litest.h"
#include "litest-int.h"

static struct input_event proximity_in[] = {
	{ .type = EV_ABS, .code = ABS_X, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_Y, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_DISTANCE, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_PRESSURE, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_TILT_X, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_TILT_Y, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_MISC, .value = 1050626 },
	{ .type = EV_MSC, .code = MSC_SERIAL, .value = 578837976 },
	{ .type = EV_KEY, .code = BTN_TOOL_PEN, .value = 1 },
	{ .type = EV_SYN, .code = SYN_REPORT, .value = 0 },
	{ .type = -1, .code = -1 },
};

static struct input_event proximity_out[] = {
	{ .type = EV_ABS, .code = ABS_X, .value = 0 },
	{ .type = EV_ABS, .code = ABS_Y, .value = 0 },
	{ .type = EV_ABS, .code = ABS_DISTANCE, .value = 0 },
	{ .type = EV_ABS, .code = ABS_TILT_X, .value = 0 },
	{ .type = EV_ABS, .code = ABS_TILT_Y, .value = 0 },
	{ .type = EV_ABS, .code = ABS_MISC, .value = 0 },
	{ .type = EV_MSC, .code = MSC_SERIAL, .value = 578837976 },
	{ .type = EV_KEY, .code = BTN_TOOL_PEN, .value = 0 },
	{ .type = EV_SYN, .code = SYN_REPORT, .value = 0 },
	{ .type = -1, .code = -1 },
};

static struct input_event motion[] = {
	{ .type = EV_ABS, .code = ABS_X, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_Y, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_DISTANCE, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_PRESSURE, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_TILT_X, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_TILT_Y, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_MSC, .code = MSC_SERIAL, .value = 578837976 },
	{ .type = EV_SYN, .code = SYN_REPORT, .value = 0 },
	{ .type = -1, .code = -1 },
};

static int
get_axis_default(struct litest_device *d, unsigned int evcode, int32_t *value)
{
	switch (evcode) {
	case ABS_TILT_X:
	case ABS_TILT_Y:
		*value = 0;
		return 0;
	case ABS_PRESSURE:
		*value = 100;
		return 0;
	case ABS_DISTANCE:
		*value = 0;
		return 0;
	}
	return 1;
}

static struct litest_device_interface interface = {
	.tablet_proximity_in_events = proximity_in,
	.tablet_proximity_out_events = proximity_out,
	.tablet_motion_events = motion,

	.get_axis_default = get_axis_default,
};

static struct input_absinfo absinfo[] = {
	{ ABS_X, 0, 44704, 4, 0, 200 },
	{ ABS_Y, 0, 27940, 4, 0, 200 },
	{ ABS_Z, -900, 899, 0, 0, 0 },
	{ ABS_THROTTLE, -1023, 1023, 0, 0, 0 },
	{ ABS_WHEEL, 0, 1023, 0, 0, 0 },
	{ ABS_PRESSURE, 0, 2047, 0, 0, 0 },
	{ ABS_DISTANCE, 0, 63, 0, 0, 0 },
	{ ABS_TILT_X, 0, 127, 0, 0, 0 },
	{ ABS_TILT_Y, 0, 127, 0, 0, 0 },
	{ ABS_MISC, 0, 0, 0, 0, 0 },
	{ .value = -1 },
};

static struct input_id input_id = {
	.bustype = 0x3,
	.vendor = 0x56a,
	.product = 0x27,
};

static int events[] = {
	EV_KEY, BTN_0,
	EV_KEY, BTN_1,
	EV_KEY, BTN_2,
	EV_KEY, BTN_3,
	EV_KEY, BTN_4,
	EV_KEY, BTN_5,
	EV_KEY, BTN_6,
	EV_KEY, BTN_7,
	EV_KEY, BTN_8,
	EV_KEY, BTN_LEFT,
	EV_KEY, BTN_RIGHT,
	EV_KEY, BTN_MIDDLE,
	EV_KEY, BTN_SIDE,
	EV_KEY, BTN_EXTRA,
	EV_KEY, BTN_TOOL_PEN,
	EV_KEY, BTN_TOOL_RUBBER,
	EV_KEY, BTN_TOOL_BRUSH,
	EV_KEY, BTN_TOOL_PENCIL,
	EV_KEY, BTN_TOOL_AIRBRUSH,
	EV_KEY, BTN_TOOL_MOUSE,
	EV_KEY, BTN_TOOL_LENS,
	EV_KEY, BTN_TOUCH,
	EV_KEY, BTN_STYLUS,
	EV_KEY, BTN_STYLUS2,
	EV_REL, REL_WHEEL,
	EV_MSC, MSC_SERIAL,
	INPUT_PROP_MAX, INPUT_PROP_POINTER,
	-1, -1,
};

TEST_DEVICE("wacom-intuos5-tablet",
	.type = LITEST_WACOM_INTUOS,
	.features = LITEST_TABLET | LITEST_DISTANCE | LITEST_TOOL_SERIAL | LITEST_TILT | LITEST_TOOL_MOUSE | LITEST_HOVER,
	.interface = &interface,

	.name = "Wacom Intuos5 touch M Pen",
	.id = &input_id,
	.events = events,
	.absinfo = absinfo,
	.udev_properties = {
		{ "LIBINPUT_DEVICE_GROUP", "wacom-i5-group" },
		{ NULL },
	},
)
