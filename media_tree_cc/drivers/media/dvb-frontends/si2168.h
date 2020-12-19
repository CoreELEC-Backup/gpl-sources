/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Silicon Labs Si2168 DVB-T/T2/C demodulator driver
 *
 * Copyright (C) 2014 Antti Palosaari <crope@iki.fi>
 */

#ifndef SI2168_H
#define SI2168_H

#include <linux/dvb/frontend.h>
/*
 * I2C address
 * 0x64
 */
struct si2168_config {
	/*
	 * frontend
	 * returned by driver
	 */
	struct dvb_frontend **fe;

	/*
	 * tuner I2C adapter
	 * returned by driver
	 */
	struct i2c_adapter **i2c_adapter;

	/* TS mode */
#define SI2168_TS_PARALLEL	0x06
#define SI2168_TS_SERIAL	0x03
#define SI2168_TS_TRISTATE	0x00
	u8 ts_mode;

	/* TS clock mode */
#define SI2168_TS_CLK_AUTO_FIXED	0
#define SI2168_TS_CLK_AUTO_ADAPT	1
#define SI2168_TS_CLK_MANUAL		2
	u8 ts_clock_mode;

	/* TS clock inverted */
	bool ts_clock_inv;

	/* TS clock gapped */
	bool ts_clock_gapped;

	/* Tuner control pins */
#define SI2168_MP_NOT_USED	1
#define SI2168_MP_A		2
#define SI2168_MP_B		3
#define SI2168_MP_C		4
#define SI2168_MP_D		5
	int agc_pin;
	bool agc_inv;
	int fef_pin;
	bool fef_inv;

	/* Inverted spectrum */
	bool spectral_inversion;
};

#endif
