/*
    Montage Technology TS2020/TS2022/DS300x/DS3103 - DVBS/S2 Demodulator/Tuner driver
    Copyright (C) 2015 Geniatech

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/firmware.h>

#include <media/dvb_frontend.h>
#include "ds3k.h"
#include "ds3k_priv.h"

static int debug = 0;
static int debugI2c = 0;


#define dprintk(args...) \
	do { \
		if (debug) \
			printk(args); \
	} while (0)

#define I2Cprintk(args...) \
	do { \
		if (debugI2c) \
			printk(args); \
	} while (0)


/* as of March 2009 current DS3000 firmware version is 1.78 */
/* DS3000 FW v1.78 MD5: a32d17910c4f370073f9346e71d34b80 */
#define DS3000_DEFAULT_FIRMWARE "dvb-fe-ds3000.fw"

#define DS300X_DEFAULT_FIRMWARE "dvb-fe-ds300x.fw"
#define DS3103_DEFAULT_FIRMWARE "dvb-fe-ds3103.fw"

#define TUNER_M88TS2020 0x2020
#define TUNER_M88TS2022 0x2022
#define TUNER_UNKNOW 0xFFFF

typedef enum _MT_FE_DMD_ID
{
    FeDmdId_Undef,
    FeDmdId_DS300X,
    FeDmdId_DS3002B,
    FeDmdId_DS3103,
    FeDmdId_DS3103B,
    FeDmdId_UNKNOW
}MT_FE_DMD_ID;

#define DS3000_SAMPLE_RATE 96000 /* in kHz */
#define DS3000_XTAL_FREQ   27000 /* in kHz */
#define MT_FE_CRYSTAL_KHZ  27000 /* Crystal Frequency of M88TS2022 used, unit: KHz , range: 16000 - 32000 */


/* Register values to initialise the demod in DVB-S mode */
static u8 ds3000_dvbs_init_tab[] = {
	0x23, 0x05,
	0x08, 0x03,
	0x0c, 0x02,
	0x21, 0x54,
	//0x25, 0x82,
	//0x27, 0x31,
	0x30, 0x08,
	0x31, 0x40,
	0x32, 0x32,
	0x33, 0x35,
	0x35, 0xff,
	0x3a, 0x00,
	0x37, 0x10,
	0x38, 0x10,
	0x39, 0x02,
	0x42, 0x60,
	0x4a, 0x40,
	0x4b, 0x04,
	0x4d, 0x91,
	0x5d, 0xc8,
	0x50, 0x77,
	0x51, 0x77,
	0x52, 0x36,
	0x53, 0x36,
	0x56, 0x01,
	0x63, 0x47,
	0x64, 0x30,
	0x65, 0x40,
	0x68, 0x26,
	0x69, 0x4c,
	0x70, 0x20,
	0x71, 0x70,
	0x72, 0x04,
	0x73, 0x00,
	0x70, 0x40,
	0x71, 0x70,
	0x72, 0x04,
	0x73, 0x00,
	0x70, 0x60,
	0x71, 0x70,
	0x72, 0x04,
	0x73, 0x00,
	0x70, 0x80,
	0x71, 0x70,
	0x72, 0x04,
	0x73, 0x00,
	0x70, 0xa0,
	0x71, 0x70,
	0x72, 0x04,
	0x73, 0x00,
	0x70, 0x1f,
	0x76, 0x00,
	0x77, 0xd1,
	0x78, 0x0c,
	0x79, 0x80,
	0x7f, 0x04,
	0x7c, 0x00,
	0x80, 0x86,
	0x81, 0xa6,
	0x85, 0x04,
	0xcd, 0xf4,
	0x90, 0x33,
	0xa0, 0x44,
	0xc0, 0x18,
	0xc3, 0x10,
	0xc4, 0x08,
	0xc5, 0x80,
	0xc6, 0x80,
	0xc7, 0x0a,
	0xc8, 0x1a,
	0xc9, 0x80,
	0xfe, 0xb6,
	0xe0, 0xf8,
	0xe6, 0x8b,
	0xd0, 0x40,
	0xf8, 0x20,
	0xfa, 0x0f,
	0xad, 0x20,
	0xae, 0x07,
	0xb8, 0x00
};
static u8 ds310x_dvbs_init_tab[] = {
	0x23, 0x07,
	0x08, 0x03,
	0x0c, 0x02,
	0x21, 0x54,
	//0x25, 0x82,
	//0x27, 0x31,
	0x30, 0x08,
	0x31, 0x40,
	0x32, 0x32,
	0x33, 0x35,
	0x35, 0xff,
	0x3a, 0x00,
	0x37, 0x10,
	0x38, 0x10,
	0x39, 0x02,
	0x42, 0x60,
	0x4a, 0x80,
	0x4b, 0x04,
	0x4d, 0x91,
	0x5d, 0xc8,
	0x50, 0x36,
	0x51, 0x36,
	0x52, 0x36,
	0x53, 0x36,
	0x63, 0x0f,
	0x64, 0x30,
	0x65, 0x40,
	0x68, 0x26,
	0x69, 0x4c,
	0x70, 0x20,
	0x71, 0x70,
	0x72, 0x04,
	0x73, 0x00,
	0x70, 0x40,
	0x71, 0x70,
	0x72, 0x04,
	0x73, 0x00,
	0x70, 0x60,
	0x71, 0x70,
	0x72, 0x04,
	0x73, 0x00,
	0x70, 0x80,
	0x71, 0x70,
	0x72, 0x04,
	0x73, 0x00,
	0x70, 0xa0,
	0x71, 0x70,
	0x72, 0x04,
	0x73, 0x00,
	0x70, 0x1f,
	0x76, 0x38,
	0x77, 0xa6,
	0x78, 0x0c,
	0x79, 0x80,
	0x7f, 0x14,
	0x7c, 0x00,
	0xae, 0x82,
	0x80, 0x64,
	0x81, 0x66,
	0x82, 0x44,
	0x85, 0x04,
	0xcd, 0xf4,
	0x90, 0x33,
	0xa0, 0x44,
	0xbe, 0x00,
	0xc0, 0x08,
	0xc3, 0x10,
	0xc4, 0x08,
	0xc5, 0xf0,
	0xc6, 0xff,
	0xc7, 0x00,
	0xc8, 0x1a,
	0xc9, 0x80,
	0xe0, 0xf8,
	0xe6, 0x8b,
	0xd0, 0x40,
	0xf8, 0x20,
	0xfa, 0x0f,
	0x00, 0x00,
	0xbd, 0x01,
	0xb8, 0x00
};

/* Register values to initialise the demod in DVB-S2 mode */
static u8 ds3000_dvbs2_init_tab[] = {
	0x23, 0x0f,
	0x08, 0x07,
	0x0c, 0x02,
	0x21, 0x54,
	//0x25, 0x82,
	//0x27, 0x31,
	0x30, 0x08,
	0x31, 0x32,
	0x32, 0x32,
	0x33, 0x35,
	0x35, 0xff,
	0x3a, 0x00,
	0x37, 0x10,
	0x38, 0x10,
	0x39, 0x02,
	0x42, 0x60,
	0x4a, 0x80,
	0x4b, 0x04,
	0x4d, 0x91,
	0x5d, 0x88,
	0x50, 0x36,
	0x51, 0x36,
	0x52, 0x36,
	0x53, 0x36,
	0x63, 0x60,
	0x64, 0x10,
	0x65, 0x10,
	0x68, 0x04,
	0x69, 0x29,
	0x70, 0x20,
	0x71, 0x70,
	0x72, 0x04,
	0x73, 0x00,
	0x70, 0x40,
	0x71, 0x70,
	0x72, 0x04,
	0x73, 0x00,
	0x70, 0x60,
	0x71, 0x70,
	0x72, 0x04,
	0x73, 0x00,
	0x70, 0x80,
	0x71, 0x70,
	0x72, 0x04,
	0x73, 0x00,
	0x70, 0xa0,
	0x71, 0x70,
	0x72, 0x04,
	0x73, 0x00,
	0x70, 0x1f,
	0xa0, 0x44,
	0xc0, 0x08,
	0xc1, 0x10,
	0xc2, 0x08,
	0xc3, 0x10,
	0xc4, 0x08,
	0xc5, 0xf0,
	0xc6, 0xf0,
	0xc7, 0x0a,
	0xc8, 0x1a,
	0xc9, 0x80,
	0xca, 0x23,
	0xcb, 0x24,
	0xce, 0x74,
	0x56, 0x01,
	0x90, 0x03,
	0x76, 0x80,
	0x77, 0x42,
	0x78, 0x0a,
	0x79, 0x80,
	0xad, 0x40,
	0xae, 0x07,
	0x7f, 0xd4,
	0x7c, 0x00,
	0x80, 0xa8,
	0x81, 0xda,
	0x7c, 0x01,
	0x80, 0xda,
	0x81, 0xec,
	0x7c, 0x02,
	0x80, 0xca,
	0x81, 0xeb,
	0x7c, 0x03,
	0x80, 0xba,
	0x81, 0xdb,
	0x85, 0x08,
	0x86, 0x00,
	0x87, 0x02,
	0x89, 0x80,
	0x8b, 0x44,
	0x8c, 0xaa,
	0x8a, 0x10,
	0xba, 0x00,
	0xf5, 0x04,
	0xd2, 0x32,
	0xb8, 0x00
};
static u8 ds310x_dvbs2_init_tab[] = {
	0x23, 0x07,
	0x08, 0x07,
	0x0c, 0x02,
	0x21, 0x54,
	//0x25, 0x82,
	//0x27, 0x31,
	0x30, 0x08,
	0x32, 0x32,
	0x33, 0x35,
	0x35, 0xff,
	0x3a, 0x00,
	0x37, 0x10,
	0x38, 0x10,
	0x39, 0x02,
	0x42, 0x60,
	0x4a, 0x80,
	0x4b, 0x04,
	0x4d, 0x91,
	0x5d, 0xc8,
	0x50, 0x36,
	0x51, 0x36,
	0x52, 0x36,
	0x53, 0x36,
	0x63, 0x0f,
	0x64, 0x10,
	0x65, 0x20,
	0x68, 0x46,
	0x69, 0xcd,
	0x70, 0x20,
	0x71, 0x70,
	0x72, 0x04,
	0x73, 0x00,
	0x70, 0x40,
	0x71, 0x70,
	0x72, 0x04,
	0x73, 0x00,
	0x70, 0x60,
	0x71, 0x70,
	0x72, 0x04,
	0x73, 0x00,
	0x70, 0x80,
	0x71, 0x70,
	0x72, 0x04,
	0x73, 0x00,
	0x70, 0xa0,
	0x71, 0x70,
	0x72, 0x04,
	0x73, 0x00,
	0x70, 0x1f,
	0x76, 0x38,
	0x77, 0xa6,
	0x78, 0x0c,
	0x79, 0x80,
	0x7f, 0x14,
	0x85, 0x08,
	0xcd, 0xf4,
	0x90, 0x33,
	0x86, 0x00,
	0x87, 0x0f,
	0x89, 0x00,
	0x8b, 0x44,
	0x8c, 0x66,
	0x9d, 0xc1,
	0x8a, 0x10,
	0xad, 0x40,
	0xa0, 0x44,
	0xbe, 0x00,
	0xc0, 0x08,
	0xc1, 0x10,
	0xc2, 0x08,
	0xc3, 0x10,
	0xc4, 0x08,
	0xc5, 0xf0,
	0xc6, 0xff,
	0xc7, 0x00,
	0xc8, 0x1a,
	0xc9, 0x80,
	0xca, 0x23,
	0xcb, 0x24,
	0xcc, 0xf4,
	0xce, 0x74,
	0x00, 0x00,
	0xbd, 0x01,
	0xb8, 0x00
};

struct ds3k_state {
	struct i2c_adapter *i2c;
	struct ds3k_config *config;
	struct dvb_frontend frontend;
	u8 skip_fw_load;
	/* previous uncorrected block counter for DVB-S2 */
	u16 prevUCBS2;
	u16 chip_ID;
	u16 tuner_ID;
	u8	dt_addr;  //For DS3103B
	u8 	cur_type;
	int mclk_khz;
	u32	cur_symbol_rate;
	u32 cur_freqKhz;
};

static int ds3k_writereg(struct ds3k_state *state, int reg, int data)
{
	u8 buf[] = { reg, data };
	struct i2c_msg msg = {
		.addr = state->config->demod_address,
		.flags = 0,
		.buf = buf,
		.len = 2
	};
	int err;

	I2Cprintk("%s: write reg 0x%02x, value 0x%02x\n", __func__, reg, data);

	err = i2c_transfer(state->i2c, &msg, 1);
	if (err != 1) {
		printk(KERN_ERR "%s: writereg error(err == %i, reg == 0x%02x, value == 0x%02x)\n",
		       __func__, err, reg, data);
		return -EREMOTEIO;
	}

	return 0;
}


/* I2C write for 8k firmware load */
static int ds3k_writeFW(struct ds3k_state *state, int reg, const u8 *data, u16 len)
{
	int i, ret = 0;
	struct i2c_msg msg;
	u8 *buf;

	buf = kmalloc(33, GFP_KERNEL);
	if (buf == NULL) {
		printk(KERN_ERR "Unable to kmalloc\n");
		return -ENOMEM;
	}

	*(buf) = reg;

	msg.addr = state->config->demod_address;
	msg.flags = 0;
	msg.buf = buf;
	msg.len = 33;

	for (i = 0; i < len; i += 32) {
		memcpy(buf + 1, data + i, 32);

		I2Cprintk("%s: write reg 0x%02x, len = %d\n", __func__, reg, len);

		ret = i2c_transfer(state->i2c, &msg, 1);
		if (ret != 1) {
			printk(KERN_ERR "%s: write error(err == %i, reg == 0x%02x\n", __func__, ret, reg);
			ret = -EREMOTEIO;
			goto error;
		}
	}
	ret = 0;

error:
	kfree(buf);

	return ret;
}

static int ds3k_readreg(struct ds3k_state *state, u8 reg)
{
	int ret;
	u8 b0[] = { reg };
	u8 b1[] = { 0 };
	struct i2c_msg msg[] = {
		{
			.addr = state->config->demod_address,
			.flags = 0,
			.buf = b0,
			.len = 1
		}, {
			.addr = state->config->demod_address,
			.flags = I2C_M_RD,
			.buf = b1,
			.len = 1
		}
	};

	ret = i2c_transfer(state->i2c, msg, 2);

	if (ret != 2) {
		printk(KERN_ERR "%s: reg=0x%x(error=%d)\n", __func__, reg, ret);
		return ret;
	}

	I2Cprintk("%s: read reg 0x%02x, value 0x%02x\n", __func__, reg, b1[0]);

	return b1[0];
}

static int ds3k_tuner_writereg(struct ds3k_state *state, int reg, int data)
{
	u8 buf[] = { reg, data };
	struct i2c_msg msg = {
		.addr = 0x60, .flags = 0, .buf = buf, .len = 2
	};
	int err;

	I2Cprintk("%s: write reg 0x%02x, value 0x%02x\n", __func__, reg, data);

	ds3k_writereg(state, 0x03, (ds3k_readreg(state, 0x03)&0xf8)|0x11);
	err = i2c_transfer(state->i2c, &msg, 1);
	if (err != 1) {
		printk("%s: writereg error(err == %i, reg == 0x%02x,"
			 " value == 0x%02x)\n", __func__, err, reg, data);
		return -EREMOTEIO;
	}

	return 0;
}

static int ds3k_tuner_readreg(struct ds3k_state *state, u8 reg)
{
	int ret;
	u8 b0[] = { reg };
	u8 b1[] = { 0 };
	struct i2c_msg msg[] = {
		{
			.addr = 0x60,
			.flags = 0,
			.buf = b0,
			.len = 1
		},
		{
			.addr = 0x60,
			.flags = I2C_M_RD,
			.buf = b1,
			.len = 1
		}
	};

	ds3k_writereg(state, 0x03, (ds3k_readreg(state, 0x03) & 0xf8) | 0x11);
	ret = i2c_transfer(state->i2c, msg, 2);
	if (ret != 2) {
		printk(KERN_ERR "%s: reg=0x%x(error=%d)\n", __func__, reg, ret);
		return ret;
	}

	I2Cprintk("%s: read reg 0x%02x, value 0x%02x\n", __func__, reg, b1[0]);

	return b1[0];
}

static int ds3k_dt_write(struct ds3k_state *state, int reg, int data)
{
	int err;
	u8 buf[] = {reg, data};
	u8 val, tmp;
	struct i2c_msg msg = {
		.addr = state->dt_addr, .flags = 0, .buf = buf, .len = 2
	};

	tmp = ds3k_readreg(state, 0x11);
	tmp &= ~0x01;
	ds3k_writereg(state, 0x11, tmp);

	val = 0x11;
	ds3k_writereg(state, 0x03, val);
	err = i2c_transfer(state->i2c, &msg, 1);
	if (err != 1) {
		printk(KERN_ERR "%s: writereg error(err == %i, reg == 0x%02x,"
			 " value == 0x%02x)\n", __func__, err, reg, data);
		return -EREMOTEIO;
	}
	tmp |= 0x01;
	ds3k_writereg(state, 0x11, tmp);

	return 0;
}

static int ds3k_dt_read(struct ds3k_state *state, u8 reg)
{
	u8 val, tmp;
	int ret;
	u8 b0[] = { reg };
	u8 b1[] = { 0 };
	struct i2c_msg msg[] = {
		{
			.addr = state->dt_addr,
			.flags = 0,
			.buf = b0,
			.len = 1
		},
		{
			.addr = state->dt_addr,
			.flags = I2C_M_RD,
			.buf = b1,
			.len = 1
		}
	};

	tmp = ds3k_readreg(state, 0x11);
	tmp &= ~0x01;
	ds3k_writereg(state, 0x11, tmp);

	val = 0x12;
	ds3k_writereg(state, 0x03, val);

	ret = i2c_transfer(state->i2c, msg, 2);
	if (ret != 2) {
		printk(KERN_ERR "%s: reg=0x%x(error=%d)\n", __func__, reg, ret);
		return -EREMOTEIO;;
	}
	tmp |= 0x01;
	ds3k_writereg(state, 0x11, tmp);

	return b1[0];
}


static int ds3k_load_firmware(struct dvb_frontend *fe,
					const struct firmware *fw);

static int ds3k_firmware_ondemand(struct dvb_frontend *fe)
{
	struct ds3k_state *state = fe->demodulator_priv;
	const struct firmware ds300x_fw = {ds300x_firmware_size, ds300x_firmware};
	const struct firmware ds3103_fw = {ds3103_firmware_size, ds3103_firmware};
	const struct firmware *fw;
	int ret = 0;

	dprintk("%s()\n", __func__);

//xsj	if(ds3k_readreg(state, 0xb2) <= 0)
//			return ret;

	if (state->skip_fw_load)
		return 0;
	/* Load firmware */
	if ((state->chip_ID == FeDmdId_DS3002B) || (state->chip_ID == FeDmdId_DS3103)
			|| (state->chip_ID == FeDmdId_DS3103B))
	{
		ds3k_writereg(state, 0x07, 0xE0);		// global reset, global diseqc reset, golbal fec reset
		ds3k_writereg(state, 0x07, 0x00);

		/* request the firmware, this will block until someone uploads it */
		printk(KERN_INFO "%s: Waiting for firmware upload (%s)...\n", __func__,
					DS3103_DEFAULT_FIRMWARE);
		fw = &ds3103_fw;
		printk(KERN_INFO "%s: Waiting for firmware upload(2)...\n", __func__);
	}
	else if(state->chip_ID==FeDmdId_DS300X)
	{
		/* request the firmware, this will block until someone uploads it */
		printk(KERN_INFO "%s: Waiting for firmware upload (%s)...\n", __func__,
					DS300X_DEFAULT_FIRMWARE);

		fw = &ds300x_fw;
		printk(KERN_INFO "%s: Waiting for firmware upload(2)...\n", __func__);

	}
	else
	{
		printk(KERN_INFO "%s: unknow chip ID...\n", __func__);
		return ret;
	}

	/* Make sure we don't recurse back through here during loading */
	state->skip_fw_load = 1;

	ret = ds3k_load_firmware(fe, fw);
	if (ret)
		printk("%s: Writing firmware to device failed\n", __func__);

	//release_firmware(fw);

	dprintk("%s: Firmware upload %s\n", __func__, ret == 0 ? "complete" : "failed");

	/* Ensure firmware is always loaded if required */
	state->skip_fw_load = 0;

	return ret;
}

static int ds3k_load_firmware(struct dvb_frontend *fe,
					const struct firmware *fw)
{
	struct ds3k_state *state = fe->demodulator_priv;

	dprintk("%s\n", __func__);
	dprintk("Firmware is %zu bytes (%02x %02x .. %02x %02x)\n",
			fw->size,
			fw->data[0],
			fw->data[1],
			fw->data[fw->size - 2],
			fw->data[fw->size - 1]);

	/* Begin the firmware load process */
	ds3k_writereg(state, 0xb2, 0x01);
	/* write the entire firmware */
	ds3k_writeFW(state, 0xb0, fw->data, fw->size);
	ds3k_writereg(state, 0xb2, 0x00);

	return 0;
}



/**************************************************************************
* Function to Set the M88TS2022
* fPLL:    Frequency         			unit: MHz	from 950 to 2150
* fSym:    SymbolRate         			unit: KS/s  from 1000 to 45000
* lpfOffset: Set the low pass filter offset when the demodulator set the PLL offset at low symbolrate  unit: KHz
* gainHold:  The flag of AGC gain hold, the tuner gain is hold when gainHold == 1 , default please set gainHold = 0
* return:	 Frequency offset of PLL 	 	unit: KHz
**************************************************************************/

int set_freq_ts2022(struct ds3k_state *state)
{

	u8 buf, capCode, div4, changePLL, K, lpf_mxdiv, divMax, divMin, RFgain = 0;
	u32 gdiv28;
	u32 N, lpf_gm, f3dB, fREF, divN, lpf_coeff = 3200;
	s32 freqOffset;

	u32 fPLL = (state->cur_freqKhz + 500) / 1000;
	u32 fSym =	state->cur_symbol_rate / 1000;
	u16 lpfOffset = 0;
	u8 gainHold = 0;


	//Initialize the tuner
	//InitialTuner();

	//Set the PLL
	if(fSym < 5000)
		lpfOffset = 3000;

	if(state->tuner_ID == TUNER_M88TS2020) {
		ds3k_tuner_writereg(state, 0x10, 0x00);
	} else if(state->tuner_ID == TUNER_M88TS2022) {
		ds3k_tuner_writereg(state, 0x10, 0x0b);
		ds3k_tuner_writereg(state, 0x11, 0x40);
	} else {
		printk(KERN_ERR "%s: Unable check tuner version\n", __func__);
		return 1;			//Error, maybe other tuner ICs,please do action at top level application
	}

	div4 = 0;
	changePLL = 0;
	K = 0;
	divN = 0;
	N = 0;
	fREF = 2;

	if(state->tuner_ID == TUNER_M88TS2020) {
		K = (MT_FE_CRYSTAL_KHZ / 1000 + 1) / 2 - 8;
		if(fPLL < 1146) {
			ds3k_tuner_writereg(state, 0x10, 0x11);
			div4 = 1;
			divN = fPLL * (K + 8) * 4000 / MT_FE_CRYSTAL_KHZ;
		} else {
			ds3k_tuner_writereg(state, 0x10, 0x01);
			divN = fPLL * (K + 8) * 2000 / MT_FE_CRYSTAL_KHZ;
		}

		divN =divN + divN % 2;
		N = divN - 1024;
		buf = (N >> 8) & 0x0f;
		ds3k_tuner_writereg(state, 0x01, buf);

		buf = N & 0xff;
		ds3k_tuner_writereg(state, 0x02, buf);

		buf = K;
		ds3k_tuner_writereg(state, 0x03, buf);
	} else if(state->tuner_ID == TUNER_M88TS2022) {
		if(fREF == 1)
			K = MT_FE_CRYSTAL_KHZ / 1000 - 8;
		else
			K = (MT_FE_CRYSTAL_KHZ / 1000 + 1) / 2 - 8;

		if (fPLL < 1103) {
			ds3k_tuner_writereg(state, 0x10, 0x1b);
			div4 = 1;
			divN = fPLL * (K+8) * 4000 / MT_FE_CRYSTAL_KHZ;
		} else {
			divN = fPLL * (K+8) * 2000 / MT_FE_CRYSTAL_KHZ;
		}

		divN = divN + divN % 2;

		if (divN < 4095) {
			N = divN - 1024;
		} else if (divN < 6143) {
			N = divN + 1024;
		} else {
			N = divN + 3072;
		}

		buf = (N >> 8) & 0x3f;
		ds3k_tuner_writereg(state, 0x01, buf);

		buf = N & 0xff;
		ds3k_tuner_writereg(state, 0x02, buf);

		buf = K;
		ds3k_tuner_writereg(state, 0x03, buf);
	}

	ds3k_tuner_writereg(state, 0x51, 0x0f);
	ds3k_tuner_writereg(state, 0x51, 0x1f);
	ds3k_tuner_writereg(state, 0x50, 0x10);
	ds3k_tuner_writereg(state, 0x50, 0x00);
	msleep(5);

	buf = ds3k_tuner_readreg(state, 0x15);
	if((buf & 0x40) != 0x40) {
		ds3k_tuner_writereg(state, 0x51, 0x0f);
		ds3k_tuner_writereg(state, 0x51, 0x1f);
		ds3k_tuner_writereg(state, 0x50, 0x10);
		ds3k_tuner_writereg(state, 0x50, 0x00);
		msleep(5);
	}

	if(state->tuner_ID == TUNER_M88TS2020) {
		buf = ds3k_tuner_readreg(state, 0x66);
		changePLL = (((buf & 0x80) >> 7) != div4);

		if(changePLL)
		{
			ds3k_tuner_writereg(state, 0x10, 0x11);

			div4 = 1;

			divN = fPLL * (K + 8) * 4000 / MT_FE_CRYSTAL_KHZ;
			divN = divN + divN % 2;
			N = divN - 1024;

			buf = (N >>8) & 0x0f;
			ds3k_tuner_writereg(state, 0x01, buf);

			buf = N & 0xff;
			ds3k_tuner_writereg(state, 0x02, buf);

			ds3k_tuner_writereg(state, 0x51, 0x0f);
			ds3k_tuner_writereg(state, 0x51, 0x1f);
			ds3k_tuner_writereg(state, 0x50, 0x10);
			ds3k_tuner_writereg(state, 0x50, 0x00);
			msleep(5);
		}
	} else if(state->tuner_ID == TUNER_M88TS2022) {
		buf = ds3k_tuner_readreg(state, 0x14);
		buf &= 0x7f;
		if(buf < 64) {
			buf = ds3k_tuner_readreg(state, 0x10);
			buf |= 0x80;
			ds3k_tuner_writereg(state, 0x10, buf);
			ds3k_tuner_writereg(state, 0x11, 0x6f);

			ds3k_tuner_writereg(state, 0x51, 0x0f);
			ds3k_tuner_writereg(state, 0x51, 0x1f);
			ds3k_tuner_writereg(state, 0x50, 0x10);
			ds3k_tuner_writereg(state, 0x50, 0x00);
			msleep(5);

			buf = ds3k_tuner_readreg(state, 0x15);
			if((buf & 0x40) != 0x40) {
				ds3k_tuner_writereg(state, 0x51, 0x0f);
				ds3k_tuner_writereg(state, 0x51, 0x1f);
				ds3k_tuner_writereg(state, 0x50, 0x10);
				ds3k_tuner_writereg(state, 0x50, 0x00);
				msleep(5);
			}
		}

		buf = ds3k_tuner_readreg(state, 0x14);
		buf &= 0x1f;
		if(buf > 19) {
			buf = ds3k_tuner_readreg(state, 0x10);
			buf &= 0xfd;
			ds3k_tuner_writereg(state, 0x10, buf);
		}
	}

	freqOffset = (s32)(divN * MT_FE_CRYSTAL_KHZ / (K + 8)/(div4 + 1) / 2 - fPLL * 1000);

	// set the RF gain
	if(state->tuner_ID == TUNER_M88TS2020) {
		ds3k_tuner_writereg(state, 0x60, 0x79);
	}

	ds3k_tuner_writereg(state, 0x51, 0x17);
	ds3k_tuner_writereg(state, 0x51, 0x1f);
	ds3k_tuner_writereg(state, 0x50, 0x08);
	ds3k_tuner_writereg(state, 0x50, 0x00);
	msleep(5);

	buf = ds3k_tuner_readreg(state, 0x3c);
	if(buf == 0) {
		ds3k_tuner_writereg(state, 0x51, 0x17);
		ds3k_tuner_writereg(state, 0x51, 0x1f);
		ds3k_tuner_writereg(state, 0x50, 0x08);
		ds3k_tuner_writereg(state, 0x50, 0x00);
		msleep(5);
	}

	if(state->tuner_ID == TUNER_M88TS2020) {
		buf = ds3k_tuner_readreg(state, 0x3d);
		RFgain = buf & 0x0f;

		if(RFgain < 15) {
			if(RFgain < 4)
				RFgain = 0;
			else
				RFgain = RFgain - 3;

			buf = ((RFgain << 3) | 0x01) & 0x79;
			ds3k_tuner_writereg(state, 0x60, buf);

			ds3k_tuner_writereg(state, 0x51, 0x17);
			ds3k_tuner_writereg(state, 0x51, 0x1f);
			ds3k_tuner_writereg(state, 0x50, 0x08);
			ds3k_tuner_writereg(state, 0x50, 0x00);
			msleep(5);
		}
	}

	// set the LPF
	if(state->tuner_ID == TUNER_M88TS2022) {
		ds3k_tuner_writereg(state, 0x25, 0x00);
		ds3k_tuner_writereg(state, 0x27, 0x70);
		ds3k_tuner_writereg(state, 0x41, 0x09);

		ds3k_tuner_writereg(state, 0x08, 0x0b);
	}

	f3dB = fSym * 135 / 200 + 2000;

	f3dB += lpfOffset;

	if(f3dB < 7000)		f3dB = 7000;
	if(f3dB > 40000)	f3dB = 40000;

	gdiv28 = (MT_FE_CRYSTAL_KHZ / 1000 * 1694 + 500) / 1000;

	buf = (u8)gdiv28;
	ds3k_tuner_writereg(state, 0x04, buf);

	ds3k_tuner_writereg(state, 0x51, 0x1b);
	ds3k_tuner_writereg(state, 0x51, 0x1f);
	ds3k_tuner_writereg(state, 0x50, 0x04);
	ds3k_tuner_writereg(state, 0x50, 0x00);
	msleep(2);

	buf = ds3k_tuner_readreg(state, 0x26);
	if(buf == 0x00) {
		ds3k_tuner_writereg(state, 0x51, 0x1b);
		ds3k_tuner_writereg(state, 0x51, 0x1f);
		ds3k_tuner_writereg(state, 0x50, 0x04);
		ds3k_tuner_writereg(state, 0x50, 0x00);
		msleep(2);

		buf = ds3k_tuner_readreg(state, 0x26);
	}

	capCode = buf & 0x3f;
	if(state->tuner_ID == TUNER_M88TS2022) {
		ds3k_tuner_writereg(state, 0x41, 0x0d);

		ds3k_tuner_writereg(state, 0x51, 0x1b);
		ds3k_tuner_writereg(state, 0x51, 0x1f);
		ds3k_tuner_writereg(state, 0x50, 0x04);
		ds3k_tuner_writereg(state, 0x50, 0x00);
		msleep(2);
		buf = ds3k_tuner_readreg(state, 0x26);
		if(buf == 0x00) {
			ds3k_tuner_writereg(state, 0x51, 0x1b);
			ds3k_tuner_writereg(state, 0x51, 0x1f);
			ds3k_tuner_writereg(state, 0x50, 0x04);
			ds3k_tuner_writereg(state, 0x50, 0x00);
			msleep(2);
			buf = ds3k_tuner_readreg(state, 0x26);
		}

		buf &= 0x3f;
		capCode = (capCode + buf) / 2;
	}

	gdiv28 = gdiv28 * 207 / (capCode * 2 + 151);

	divMax = gdiv28 * 135 / 100;
	divMin = gdiv28 * 78 / 100;

	if(divMax > 63)
		divMax = 63;

	if(state->tuner_ID == TUNER_M88TS2020) {
		lpf_coeff = 2766;
	} else if(state->tuner_ID == TUNER_M88TS2022) {
		lpf_coeff = 3200;
	}

	lpf_gm = (f3dB * gdiv28 * 2 / lpf_coeff / (MT_FE_CRYSTAL_KHZ / 1000) + 1) / 2;

	if (lpf_gm > 23)
		lpf_gm = 23;
	if (lpf_gm < 1)
		lpf_gm = 1;

	lpf_mxdiv = (lpf_gm * (MT_FE_CRYSTAL_KHZ / 1000) * lpf_coeff * 2 / f3dB + 1) / 2;

	if (lpf_mxdiv < divMin) {
		lpf_gm++;
		lpf_mxdiv = (lpf_gm * (MT_FE_CRYSTAL_KHZ / 1000) * lpf_coeff * 2 / f3dB + 1) / 2;
	}

	if (lpf_mxdiv > divMax) {
		lpf_mxdiv = divMax;
	}

	buf = lpf_mxdiv;
	ds3k_tuner_writereg(state, 0x04, buf);

	buf = lpf_gm;
	ds3k_tuner_writereg(state, 0x06, buf);

	ds3k_tuner_writereg(state, 0x51, 0x1b);
	ds3k_tuner_writereg(state, 0x51, 0x1f);
	ds3k_tuner_writereg(state, 0x50, 0x04);
	ds3k_tuner_writereg(state, 0x50, 0x00);
	msleep(2);

	buf = ds3k_tuner_readreg(state, 0x26);
	if(buf == 0x00) {
		ds3k_tuner_writereg(state, 0x51, 0x1b);
		ds3k_tuner_writereg(state, 0x51, 0x1f);
		ds3k_tuner_writereg(state, 0x50, 0x04);
		ds3k_tuner_writereg(state, 0x50, 0x00);
		msleep(2);

		buf = ds3k_tuner_readreg(state, 0x26);
	}

	if(state->tuner_ID == TUNER_M88TS2022)
	{
		capCode = buf & 0x3f;

		ds3k_tuner_writereg(state, 0x41, 0x09);

		ds3k_tuner_writereg(state, 0x51, 0x1b);
		ds3k_tuner_writereg(state, 0x51, 0x1f);
		ds3k_tuner_writereg(state, 0x50, 0x04);
		ds3k_tuner_writereg(state, 0x50, 0x00);
		msleep(2);

		buf = ds3k_tuner_readreg(state, 0x26);
		if(buf == 0x00) {
			ds3k_tuner_writereg(state, 0x51, 0x1b);
			ds3k_tuner_writereg(state, 0x51, 0x1f);
			ds3k_tuner_writereg(state, 0x50, 0x04);
			ds3k_tuner_writereg(state, 0x50, 0x00);
			msleep(2);

			buf = ds3k_tuner_readreg(state, 0x26);
		}

		buf &= 0x3f;
		capCode = (capCode + buf) / 2;

		buf = capCode | 0x80;
		ds3k_tuner_writereg(state, 0x25, buf);
		ds3k_tuner_writereg(state, 0x27, 0x30);

		ds3k_tuner_writereg(state, 0x08, 0x09);
	}

	// Set the BB gain
	// default should set gainHold = 0;
	// except when the AGC of demodulator is hold, for example application at blind scan use Haier demodulator

	if(gainHold == 0) {
		ds3k_tuner_writereg(state, 0x51, 0x1e);
		ds3k_tuner_writereg(state, 0x51, 0x1f);
		ds3k_tuner_writereg(state, 0x50, 0x01);
		ds3k_tuner_writereg(state, 0x50, 0x00);
		msleep(20);

		buf = ds3k_tuner_readreg(state, 0x21);
		if(buf == 0x00) {
			ds3k_tuner_writereg(state, 0x51, 0x1e);
			ds3k_tuner_writereg(state, 0x51, 0x1f);
			ds3k_tuner_writereg(state, 0x50, 0x01);
			ds3k_tuner_writereg(state, 0x50, 0x00);
			msleep(20);
		}

		if(state->tuner_ID == TUNER_M88TS2020) {
			if(RFgain == 15) {
				msleep(20);
				buf = ds3k_tuner_readreg(state, 0x21);
				buf &= 0x0f;
				if(buf < 3) {
					ds3k_tuner_writereg(state, 0x60, 0x61);

					ds3k_tuner_writereg(state, 0x51, 0x17);
					ds3k_tuner_writereg(state, 0x51, 0x1f);
					ds3k_tuner_writereg(state, 0x50, 0x08);
					ds3k_tuner_writereg(state, 0x50, 0x00);
					msleep(20);
				}
			}
		}

		//User should delay 100ms here to wait the Tuner gain stable before checking the chip lock status
		//If there have delay time at the function of setting demodulator, you can take out it to reduce the lock time;
		msleep(40);
	}

	return freqOffset;	// return the frequency offset : KHz
}

int _mt_fe_dmd_ds3k_select_mclk(struct ds3k_state *state)
{
	u32 adc_Freq_MHz[3] = {96, 93, 99};
	u8  reg16_list[3] = {96, 92, 100}, reg16, reg15;
	u32 offset_MHz[3];
	u32 max_offset = 0;
	u32 tuner_freq_MHz = state->cur_freqKhz / 1000;

	u8 i;

	char bBigSymbol = 0;

	bBigSymbol = (state->cur_symbol_rate > 45010000) ? 1 : 0;

	if(bBigSymbol) {
		#if 0
		adc_Freq_MHz[0] = 216;
		adc_Freq_MHz[1] = 222;
		adc_Freq_MHz[2] = 219;

		reg16_list[0] = 112;
		reg16_list[1] = 116;
		reg16_list[2] = 114;
		#endif

		reg16 = 115;			//147 - 32;
		state->mclk_khz = 110250;
	} else {
		adc_Freq_MHz[0] = 96;
		adc_Freq_MHz[1] = 93;
		adc_Freq_MHz[2] = 99;

		reg16_list[0] = 96;
		reg16_list[1] = 92;
		reg16_list[2] = 100;

		reg16 = 96;

		for(i = 0; i < 3; i++)
		{
			offset_MHz[i] = tuner_freq_MHz % adc_Freq_MHz[i];

			if(offset_MHz[i] > (adc_Freq_MHz[i] / 2))
				offset_MHz[i] = adc_Freq_MHz[i] - offset_MHz[i];

			if(offset_MHz[i] > max_offset)
			{
				max_offset = offset_MHz[i];
				reg16 = reg16_list[i];
				state->mclk_khz= adc_Freq_MHz[i] * 1000;

				if(bBigSymbol)
					state->mclk_khz /= 2;
			}
		}
	}

	if(state->mclk_khz == 93000)
		ds3k_writereg(state, 0xA0, 0x42);
	else if(state->mclk_khz == 96000)
		ds3k_writereg(state, 0xA0, 0x44);
	else if(state->mclk_khz == 99000)
		ds3k_writereg(state, 0xA0, 0x46);
	else if(state->mclk_khz == 110250)
		ds3k_writereg(state, 0xA0, 0x4E);
	else
		ds3k_writereg(state, 0xA0, 0x44);

	reg15 = ds3k_dt_read(state, 0x15);

	ds3k_dt_write(state, 0x05, 0x40);
	ds3k_dt_write(state, 0x11, 0x08);

	if(bBigSymbol) {
		reg15 |= 0x02;
	} else {
		reg15 &= ~0x02;
	}
	ds3k_dt_write(state, 0x15, reg15);

	ds3k_dt_write(state, 0x16, reg16);

	msleep(5);

	ds3k_dt_write(state, 0x05, 0x00);
	ds3k_dt_write(state, 0x11, (u8)(bBigSymbol ? 0x0E : 0x0A));
	msleep(5);

	return 0;
}

int _mt_fe_dmd_ds3k_set_mclk(struct ds3k_state *state, u32 MCLK_KHz)
{
	u8 tmp3 = 0, tmp4 = 0;
	struct ds3k_config *cfg = state->config;

	if((state->chip_ID == FeDmdId_DS3002B) || (state->chip_ID == FeDmdId_DS3103)) {
		// 0x22 bit[7:6] clkxM_d
		tmp3 = ds3k_readreg(state, 0x22);
		// 0x24 bit[7:6] clkxM_sel
		tmp4 = ds3k_readreg(state, 0x24);

		switch(MCLK_KHz) {
			case 192000:		// 4b'0011 MCLK = 192M
				tmp3 |= 0xc0;		// 0x22 bit[7:6] = 2b'11
				tmp4 &= 0x3f;		// 0x24 bit[7:6] = 2b'00
				break;

			case 144000:		// 4b'0100 MCLK = 144M
				tmp3 &= 0x3f;		// 0x22 bit[7:6] = 2b'00
				tmp4 &= 0x7f;		// 0x24 bit[7:6] = 2b'01
				tmp4 |= 0x40;
				break;

			case 115200:		// 4b'0101 MCLK = 115.2M
				tmp3 &= 0x7f;		// 0x22 bit[7:6] = 2b'01
				tmp3 |= 0x40;
				tmp4 &= 0x7f;		// 0x24 bit[7:6] = 2b'01
				tmp4 |= 0x40;
				break;

			case 72000:			// 4b'1100 MCLK = 72M
				tmp4 |= 0xc0;		// 0x24 bit[7:6] = 2b'11
				tmp3 &= 0x3f;		// 0x22 bit[7:6] = 2b'00
				break;

			case 96000:			// 4b'0110 MCLK = 96M
			default:
				tmp3 &= 0xbf;		// 0x22 bit[7:6] = 2b'10
				tmp3 |= 0x80;

				tmp4 &= 0x7f;		// 0x24 bit[7:6] = 2b'01
				tmp4 |= 0x40;
				break;
		}

		ds3k_writereg(state, 0x22, tmp3);
		ds3k_writereg(state, 0x24, tmp4);
	} else if(state->chip_ID == FeDmdId_DS3103B) {
		u8 reg11 = 0x0A, reg15, reg16, reg1D, reg1E, reg1F, tmp;
		u8 sm, f0 = 0, f1 = 0, f2 = 0, f3 = 0, pll_ldpc_mode;
		u16 pll_div_fb, N;
		u32 div;

		reg15 = ds3k_dt_read(state, 0x15);
		reg16 = ds3k_dt_read(state, 0x16);
		reg1D = ds3k_dt_read(state, 0x1D);


		if(cfg->output_mode != MtFeTsOutMode_Serial) {
			#if 1		// 130603
			if(reg16 == 92)
			{
				tmp = 93;
			}
			else if(reg16 == 100)
			{
				tmp = 99;
			}
			else // if(reg16 == 96)
			{
				tmp = 96;
			}

			MCLK_KHz *= tmp;
			MCLK_KHz /= 96;
			#endif
		}


		pll_ldpc_mode = (reg15 >> 1) & 0x01;

		//pll_div_fb = (pll_ldpc_mode == 1) ? 147 : 128;
		//pll_div_fb -= 32;
		pll_div_fb = (reg15 & 0x01) << 8;
		pll_div_fb += reg16;
		pll_div_fb += 32;

		div = 9000 * pll_div_fb * 4;
		div /= MCLK_KHz;


		if(cfg->output_mode == MtFeTsOutMode_Serial) {
			reg11 |= 0x02;

			if(div <= 32)
			{
				N = 2;

				f0 = 0;
				f1 = div / N;
				f2 = div - f1;
				f3 = 0;
			}
			else if(div <= 34)
			{
				N = 3;

				f0 = div / N;
				f1 = (div - f0) / (N - 1);
				f2 = div - f0 - f1;
				f3 = 0;
			}
			else if(div <= 64)
			{
				N = 4;

				f0 = div / N;
				f1 = (div - f0) / (N - 1);
				f2 = (div - f0 - f1) / (N - 2);
				f3 = div - f0 - f1 - f2;
			}
			else
			{
				N = 4;

				f0 = 16;
				f1 = 16;
				f2 = 16;
				f3 = 16;
			}


			if(f0 == 16)
				f0 = 0;
			else if((f0 < 8) && (f0 != 0))
				f0 = 8;

			if(f1 == 16)
				f1 = 0;
			else if((f1 < 8) && (f1 != 0))
				f1 = 8;

			if(f2 == 16)
				f2 = 0;
			else if((f2 < 8) && (f2 != 0))
				f2 = 8;

			if(f3 == 16)
				f3 = 0;
			else if((f3 < 8) && (f3 != 0))
				f3 = 8;
		}
		else
		{
			reg11 &= ~0x02;

			if(div <= 32)
			{
				N = 2;

				f0 = 0;
				f1 = div / N;
				f2 = div - f1;
				f3 = 0;
			}
			else if(div <= 48)
			{
				N = 3;

				f0 = div / N;
				f1 = (div - f0) / (N - 1);
				f2 = div - f0 - f1;
				f3 = 0;
			}
			else if(div <= 64)
			{
				N = 4;

				f0 = div / N;
				f1 = (div - f0) / (N - 1);
				f2 = (div - f0 - f1) / (N - 2);
				f3 = div - f0 - f1 - f2;
			}
			else
			{
				N = 4;

				f0 = 16;
				f1 = 16;
				f2 = 16;
				f3 = 16;
			}

			if(f0 == 16)
				f0 = 0;
			else if((f0 < 9) && (f0 != 0))
				f0 = 9;

			if(f1 == 16)
				f1 = 0;
			else if((f1 < 9) && (f1 != 0))
				f1 = 9;

			if(f2 == 16)
				f2 = 0;
			else if((f2 < 9) && (f2 != 0))
				f2 = 9;

			if(f3 == 16)
				f3 = 0;
			else if((f3 < 9) && (f3 != 0))
				f3 = 9;
		}

		sm = N - 1;

		/* Write to registers */
		//reg15 &= 0x01;
		//reg15 |= (pll_div_fb >> 8) & 0x01;

		//reg16 = pll_div_fb & 0xFF;

		reg1D &= ~0x03;
		reg1D |= sm;
		reg1D |= 0x80;

		reg1E = ((f3 << 4) + f2) & 0xFF;
		reg1F = ((f1 << 4) + f0) & 0xFF;

		ds3k_dt_write(state, 0x05, 0x40);
		ds3k_dt_write(state, 0x11, 0x08);
		ds3k_dt_write(state, 0x1D, reg1D);
		ds3k_dt_write(state, 0x1E, reg1E);
		ds3k_dt_write(state, 0x1F, reg1F);

		ds3k_dt_write(state, 0x17, 0xc1);
		ds3k_dt_write(state, 0x17, 0x81);
		msleep(5);

		ds3k_dt_write(state, 0x05, 0x00);
		ds3k_dt_write(state, 0x11, 0x0A);
		msleep(5);
	}
	else if(state->chip_ID == FeDmdId_DS300X)
	{
		return -4;
	}
	else
	{
		return -6;
	}

	return 0;
}

static int ds3k_set_voltage(struct dvb_frontend *fe, enum fe_sec_voltage voltage)
{
	struct ds3k_state *state = fe->demodulator_priv;
	u8 data;

	dprintk("%s(%d)\n", __func__, voltage);

	data = ds3k_readreg(state, 0xa2);
	data |= 0x03; /* bit0 V/H, bit1 off/on */

	switch (voltage) {
	case SEC_VOLTAGE_18:
		data &= ~0x03;
		break;
	case SEC_VOLTAGE_13:
		data &= ~0x03;
		data |= 0x01;
		break;
	case SEC_VOLTAGE_OFF:
		break;
	}

	ds3k_writereg(state, 0xa2, data);

	return 0;
}

static int ds3k_read_status(struct dvb_frontend *fe, enum fe_status *status)
{
	struct ds3k_state *state = fe->demodulator_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	int lock;

	*status = 0;

	switch (c->delivery_system) {
	case SYS_DVBS:
		lock = ds3k_readreg(state, 0xd1);
		if ((lock & 0x07) == 0x07)
			*status = FE_HAS_SIGNAL | FE_HAS_CARRIER |
				FE_HAS_VITERBI | FE_HAS_SYNC |
				FE_HAS_LOCK;

		break;
	case SYS_DVBS2:
		lock = ds3k_readreg(state, 0x0d);
		if ((lock & 0x8f) == 0x8f)
			*status = FE_HAS_SIGNAL | FE_HAS_CARRIER |
				FE_HAS_VITERBI | FE_HAS_SYNC |
				FE_HAS_LOCK;

		break;
	default:
		return 1;
	}

	if (state->config->set_lock_led)
		state->config->set_lock_led(fe, *status == 0 ? 0 : 1);

	dprintk("%s: lock = 0x%02x, status = 0x%02x\n", __func__, lock, *status);

	return 0;
}

/* read DS3000 BER value */
static int ds3k_read_ber(struct dvb_frontend *fe, u32* ber)
{
	struct ds3k_state *state = fe->demodulator_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	u8 data;
	u32 ber_reading, lpdc_frames;

	dprintk("%s()\n", __func__);

	switch (c->delivery_system) {
	case SYS_DVBS:
		/* set the number of bytes checked during
		BER estimation */
		ds3k_writereg(state, 0xf9, 0x04);
		/* read BER estimation status */
		data = ds3k_readreg(state, 0xf8);
		/* check if BER estimation is ready */
		if ((data & 0x10) == 0) {
			/* this is the number of error bits,
			to calculate the bit error rate
			divide to 8388608 */
			*ber = (ds3k_readreg(state, 0xf7) << 8) |
				ds3k_readreg(state, 0xf6);
			/* start counting error bits */
			/* need to be set twice
			otherwise it fails sometimes */
			data |= 0x10;
			ds3k_writereg(state, 0xf8, data);
			ds3k_writereg(state, 0xf8, data);
		} else
			/* used to indicate that BER estimation
			is not ready, i.e. BER is unknown */
			*ber = 0xffffffff;
		break;
	case SYS_DVBS2:
		/* read the number of LPDC decoded frames */
		lpdc_frames = (ds3k_readreg(state, 0xd7) << 16) |
				(ds3k_readreg(state, 0xd6) << 8) |
				ds3k_readreg(state, 0xd5);
		/* read the number of packets with bad CRC */
		ber_reading = (ds3k_readreg(state, 0xf8) << 8) |
				ds3k_readreg(state, 0xf7);
		if (lpdc_frames > 750) {
			/* clear LPDC frame counters */
			ds3k_writereg(state, 0xd1, 0x01);
			/* clear bad packets counter */
			ds3k_writereg(state, 0xf9, 0x01);
			/* enable bad packets counter */
			ds3k_writereg(state, 0xf9, 0x00);
			/* enable LPDC frame counters */
			ds3k_writereg(state, 0xd1, 0x00);
			*ber = ber_reading;
		} else
			/* used to indicate that BER estimation is not ready,
			i.e. BER is unknown */
			*ber = 0xffffffff;
		break;
	default:
		return 1;
	}

	return 0;
}

/* read TS2020 signal strength */
static int ds3k_read_signal_strength(struct dvb_frontend *fe,
						u16 *signal_strength)
{
	struct ds3k_state *state = fe->demodulator_priv;
	int sig_reading = 0;
	u8 rfgain, bbgain, nngain;
	u8 rfagc;
	u32 gain = 0;
	dprintk("%s()\n", __func__);

	rfgain = ds3k_tuner_readreg(state, 0x3d) & 0x1f;
	bbgain = ds3k_tuner_readreg(state, 0x21) & 0x1f;
	rfagc = ds3k_tuner_readreg(state, 0x3f);

	if(state->tuner_ID == TUNER_M88TS2020)
	{
		//TUNER 2020
	      sig_reading = rfagc * 20 - 1166;
	      if(sig_reading<0) sig_reading =0;
	      if(rfgain < 0)		rfgain = 0;
	      if(rfgain > 15)		rfgain = 15;
	      if(bbgain < 0 )		bbgain = 0;
	      if(bbgain > 13)		bbgain = 13;

	      if(sig_reading < 400)		sig_reading = 400;
	      if(sig_reading > 1100)		sig_reading = 1100;

	      gain = (u16) rfgain * 233 + (u16) bbgain * 350 + sig_reading * 24 / 10 + 1000;

	}
	else if(state->tuner_ID == TUNER_M88TS2022)
	{
		//TUNER 2022
	      sig_reading = rfagc * 16 - 670;
	      if(sig_reading<0) sig_reading = 0;
	      nngain =ds3k_tuner_readreg(state, 0x66);
	      nngain = (nngain >> 3) & 0x07;

	      if(rfgain < 0)	rfgain = 0;
	      if(rfgain > 15)	rfgain = 15;
	      if(bbgain < 2)	bbgain = 2;
	      if(bbgain > 16)	bbgain = 16;
	      if(nngain < 0)	nngain = 0;
	      if(nngain > 6)	nngain = 6;

	      if(sig_reading < 600)	sig_reading = 600;
	      if(sig_reading > 1600)	sig_reading = 1600;

	      gain = (u16) rfgain * 265 + (u16) bbgain * 338 + (u16) nngain * 285 + sig_reading * 176 / 100 - 3000;
	}


	*signal_strength = gain*100;

	dprintk("%s: raw / cooked = 0x%04x / 0x%04x\n", __func__, sig_reading, *signal_strength);

	return 0;
}

/* calculate DS3000 snr value in dB */
static int ds3k_read_snr(struct dvb_frontend *fe, u16 *snr)
{
	struct ds3k_state *state = fe->demodulator_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	u8 snr_reading, snr_value;
	u32 dvbs2_signal_reading, dvbs2_noise_reading, tmp;
	static const u16 dvbs_snr_tab[] = { /* 20 x Table (rounded up) */
		0x0000, 0x1b13, 0x2aea, 0x3627, 0x3ede, 0x45fe, 0x4c03,
		0x513a, 0x55d4, 0x59f2, 0x5dab, 0x6111, 0x6431, 0x6717,
		0x69c9, 0x6c4e, 0x6eac, 0x70e8, 0x7304, 0x7505
	};
	static const u16 dvbs2_snr_tab[] = { /* 80 x Table (rounded up) */
		0x0000, 0x0bc2, 0x12a3, 0x1785, 0x1b4e, 0x1e65, 0x2103,
		0x2347, 0x2546, 0x2710, 0x28ae, 0x2a28, 0x2b83, 0x2cc5,
		0x2df1, 0x2f09, 0x3010, 0x3109, 0x31f4, 0x32d2, 0x33a6,
		0x3470, 0x3531, 0x35ea, 0x369b, 0x3746, 0x37ea, 0x3888,
		0x3920, 0x39b3, 0x3a42, 0x3acc, 0x3b51, 0x3bd3, 0x3c51,
		0x3ccb, 0x3d42, 0x3db6, 0x3e27, 0x3e95, 0x3f00, 0x3f68,
		0x3fcf, 0x4033, 0x4094, 0x40f4, 0x4151, 0x41ac, 0x4206,
		0x425e, 0x42b4, 0x4308, 0x435b, 0x43ac, 0x43fc, 0x444a,
		0x4497, 0x44e2, 0x452d, 0x4576, 0x45bd, 0x4604, 0x4649,
		0x468e, 0x46d1, 0x4713, 0x4755, 0x4795, 0x47d4, 0x4813,
		0x4851, 0x488d, 0x48c9, 0x4904, 0x493f, 0x4978, 0x49b1,
		0x49e9, 0x4a20, 0x4a57
	};

	dprintk("%s()\n", __func__);

	switch (c->delivery_system) {
	case SYS_DVBS:
		snr_reading = ds3k_readreg(state, 0xff);
		snr_reading /= 8;
		if (snr_reading == 0)
			*snr = 0x0000;
		else {
			if (snr_reading > 20)
				snr_reading = 20;
			snr_value = dvbs_snr_tab[snr_reading - 1] * 10 / 23026;
			/* cook the value to be suitable for szap-s2
			human readable output */
			*snr = snr_value * 8 * 655;
		}
		dprintk("%s: raw / cooked = 0x%02x / 0x%04x\n", __func__,
				snr_reading, *snr);
		break;
	case SYS_DVBS2:
		dvbs2_noise_reading = (ds3k_readreg(state, 0x8c) & 0x3f) +
				(ds3k_readreg(state, 0x8d) << 4);
		dvbs2_signal_reading = ds3k_readreg(state, 0x8e);
		tmp = dvbs2_signal_reading * dvbs2_signal_reading >> 1;
		if (tmp == 0) {
			*snr = 0x0000;
			return 0;
		}
		if (dvbs2_noise_reading == 0) {
			snr_value = 0x0013;
			/* cook the value to be suitable for szap-s2
			human readable output */
			*snr = 0xffff;
			return 0;
		}
		if (tmp > dvbs2_noise_reading) {
			snr_reading = tmp / dvbs2_noise_reading;
			if (snr_reading > 80)
				snr_reading = 80;
			snr_value = dvbs2_snr_tab[snr_reading - 1] / 1000;
			/* cook the value to be suitable for szap-s2
			human readable output */
			*snr = snr_value * 5 * 655;
		} else {
			snr_reading = dvbs2_noise_reading / tmp;
			if (snr_reading > 80)
				snr_reading = 80;
			*snr = -(dvbs2_snr_tab[snr_reading] / 1000);
		}
		dprintk("%s: raw / cooked = 0x%02x / 0x%04x\n", __func__,
				snr_reading, *snr);
		break;
	default:
		return 1;
	}

	return 0;
}

/* read DS3000 uncorrected blocks */
static int ds3k_read_ucblocks(struct dvb_frontend *fe, u32 *ucblocks)
{
	struct ds3k_state *state = fe->demodulator_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	u8 data;
	u16 _ucblocks;

	dprintk("%s()\n", __func__);

	switch (c->delivery_system) {
	case SYS_DVBS:
		*ucblocks = (ds3k_readreg(state, 0xf5) << 8) |
				ds3k_readreg(state, 0xf4);
		data = ds3k_readreg(state, 0xf8);
		/* clear packet counters */
		data &= ~0x20;
		ds3k_writereg(state, 0xf8, data);
		/* enable packet counters */
		data |= 0x20;
		ds3k_writereg(state, 0xf8, data);
		break;
	case SYS_DVBS2:
		_ucblocks = (ds3k_readreg(state, 0xe2) << 8) |
				ds3k_readreg(state, 0xe1);
		if (_ucblocks > state->prevUCBS2)
			*ucblocks = _ucblocks - state->prevUCBS2;
		else
			*ucblocks = state->prevUCBS2 - _ucblocks;
		state->prevUCBS2 = _ucblocks;
		break;
	default:
		return 1;
	}

	return 0;
}

static int ds3k_set_tone(struct dvb_frontend *fe, enum fe_sec_tone_mode tone)
{
	struct ds3k_state *state = fe->demodulator_priv;
	u8 data;

	dprintk("%s(%d)\n", __func__, tone);
	if ((tone != SEC_TONE_ON) && (tone != SEC_TONE_OFF)) {
		printk(KERN_ERR "%s: Invalid, tone=%d\n", __func__, tone);
		return -EINVAL;
	}

	data = ds3k_readreg(state, 0xa2);
	data &= ~0xc0;
	ds3k_writereg(state, 0xa2, data);

	switch (tone) {
	case SEC_TONE_ON:
		dprintk("%s: setting tone on\n", __func__);
		data = ds3k_readreg(state, 0xa1);
		data &= ~0x43;
		data |= 0x04;
		ds3k_writereg(state, 0xa1, data);
		break;
	case SEC_TONE_OFF:
		dprintk("%s: setting tone off\n", __func__);
		data = ds3k_readreg(state, 0xa2);
		data |= 0x80;
		ds3k_writereg(state, 0xa2, data);
		break;
	}

	return 0;
}

static int ds3k_send_diseqc_msg(struct dvb_frontend *fe,
				struct dvb_diseqc_master_cmd *d)
{
	struct ds3k_state *state = fe->demodulator_priv;
	int i;
	u8 data;

	/* Dump DiSEqC message */
	dprintk("%s(", __func__);
	for (i = 0 ; i < d->msg_len;) {
		dprintk("0x%02x", d->msg[i]);
		if (++i < d->msg_len)
			dprintk(", ");
	}

	/* enable DiSEqC message send pin */
	data = ds3k_readreg(state, 0xa2);
	data &= ~0xc0;
	data &= ~0x20;
	ds3k_writereg(state, 0xa2, data);

	/* DiSEqC message */
	for (i = 0; i < d->msg_len; i++)
		ds3k_writereg(state, 0xa3 + i, d->msg[i]);

	data = ds3k_readreg(state, 0xa1);
	/* clear DiSEqC message length and status,
	enable DiSEqC message send */
	data &= ~0xf8;
	/* set DiSEqC mode, modulation active during 33 pulses,
	set DiSEqC message length */
	data |= ((d->msg_len - 1) << 3) | 0x07;
	ds3k_writereg(state, 0xa1, data);

	/* wait up to 150ms for DiSEqC transmission to complete */
	for (i = 0; i < 15; i++) {
		data = ds3k_readreg(state, 0xa1);
		if ((data & 0x40) == 0)
			break;
		msleep(10);
	}

	/* DiSEqC timeout after 150ms */
	if (i == 15) {
		data = ds3k_readreg(state, 0xa1);
		data &= ~0x80;
		data |= 0x40;
		ds3k_writereg(state, 0xa1, data);

		data = ds3k_readreg(state, 0xa2);
		data &= ~0xc0;
		data |= 0x80;
		ds3k_writereg(state, 0xa2, data);

		return 1;
	}

	data = ds3k_readreg(state, 0xa2);
	data &= ~0xc0;
	data |= 0x80;
	ds3k_writereg(state, 0xa2, data);

	return 0;
}

/* Send DiSEqC burst */
static int ds3k_diseqc_send_burst(struct dvb_frontend *fe, enum fe_sec_mini_cmd burst)
{
	struct ds3k_state *state = fe->demodulator_priv;
	int i;
	u8 data;

	dprintk("%s()\n", __func__);

	data = ds3k_readreg(state, 0xa2);
	data &= ~0xc0;
	data &= ~0x20;
	ds3k_writereg(state, 0xa2, data);

	/* DiSEqC burst */
	if (burst == SEC_MINI_A)
		/* Unmodulated tone burst */
		ds3k_writereg(state, 0xa1, 0x02);
	else if (burst == SEC_MINI_B)
		/* Modulated tone burst */
		ds3k_writereg(state, 0xa1, 0x01);
	else
		return -EINVAL;

	msleep(13);
	for (i = 0; i < 5; i++) {
		data = ds3k_readreg(state, 0xa1);
		if ((data & 0x40) == 0)
			break;
		msleep(1);
	}

	if (i == 5) {
		data = ds3k_readreg(state, 0xa1);
		data &= ~0x80;
		data |= 0x40;
		ds3k_writereg(state, 0xa1, data);

		data = ds3k_readreg(state, 0xa2);
		data &= ~0xc0;
		data |= 0x80;
		ds3k_writereg(state, 0xa2, data);

		return 1;
	}

	data = ds3k_readreg(state, 0xa2);
	data &= ~0xc0;
	data |= 0x80;
	ds3k_writereg(state, 0xa2, data);

	return 0;
}

static void ds3k_release(struct dvb_frontend *fe)
{
	struct ds3k_state *state = fe->demodulator_priv;

	if (state->config->set_lock_led)
		state->config->set_lock_led(fe, 0);

	dprintk("%s\n", __func__);
	kfree(state);
}

static struct dvb_frontend_ops ds3k_ops;

struct dvb_frontend *ds3k_attach(const struct ds3k_config *config,
				    struct i2c_adapter *i2c)
{
	struct ds3k_state *state = NULL;
	int ret;
	u8 val_00, val_01, val_02, val_b2;
	int i;


	dprintk("%s\n", __func__);

	/* allocate memory for the internal state */
	state = kzalloc(sizeof(struct ds3k_state), GFP_KERNEL);
	if (state == NULL) {
		printk(KERN_ERR "Unable to kmalloc\n");
		goto error2;
	}

	state->config = config;
	state->i2c = i2c;
	state->prevUCBS2 = 0;
	state->dt_addr = 0x42;
	state->cur_type = SYS_DVBS;

	/* check if the demod is present */
	for(i = 0x68; i <= 0x6b; i++) {
		state->config->demod_address = i;
		ret = ds3k_readreg(state, 0x00) & 0xfe;
		if((ret == 0xE0) || (ret == 0xE8 ))
			break;
	}
	if((i == 0x6b ) && (ret != 0xE0) && (ret != 0xE8 )) {
		printk(KERN_ERR "Invalid probe, probably not a DS3000\n");
		goto error3;
	}

	printk(KERN_INFO "DS3000 chip version: %d.%d attached.\n",
			ds3k_readreg(state, 0x02),
			ds3k_readreg(state, 0x01));

	/* check demod chip ID */
	val_00 = ds3k_readreg(state, 0x00);
	val_01 = ds3k_readreg(state, 0x01);
	val_02 = ds3k_readreg(state, 0x02);
	val_b2 = ds3k_readreg(state, 0xb2);
	if((val_02 == 0x00) && (val_01 == 0xC0))
	{
		state->chip_ID = FeDmdId_DS300X;
		printk("\tChip ID = [DS300X]!\n");
	}
	else if((val_02 == 0x00) && (val_01 == 0xD0) && ((val_b2 & 0xC0) == 0x00))
	{
		state->chip_ID = FeDmdId_DS3002B;
		printk("\tChip ID = [DS3002B]!\n");
	}
	else if((val_02 == 0x00) && (val_01 == 0xD0) && ((val_b2 & 0xC0) == 0xC0))
	{
		state->chip_ID = FeDmdId_DS3103;
		printk("\tChip ID = [DS3103]!\n");
	}
	else if((val_02 == 0x00) && ((val_01 == 0xA0) || (val_01 == 0xA1))
				&& ((val_b2 & 0xC0) == 0x00) && (val_00 == 0xE8))
	{
		state->chip_ID = FeDmdId_DS3103B;
		printk("\tChip ID = [DS3103B]!\n");
	}
	else
	{
		state->chip_ID = FeDmdId_UNKNOW;
		printk("\tChip ID = unknow!\n");
	}

	if(state->chip_ID == FeDmdId_DS3103B) {
		val_00 = ds3k_readreg(state, 0x29);
		state->dt_addr = ((val_00 & 0x80) == 0) ? 0x42 : 0x40;
		printk("dt addr is 0x%02x", state->dt_addr);
	}

	memcpy(&state->frontend.ops, &ds3k_ops, sizeof(struct dvb_frontend_ops));
	state->frontend.demodulator_priv = state;
	return &state->frontend;

error3:
	kfree(state);
error2:
	return NULL;
}
EXPORT_SYMBOL(ds3k_attach);

#if 0
static int ds3k_set_property(struct dvb_frontend *fe,
	struct dtv_property *tvp)
{
	dprintk("%s(..)\n", __func__);
	return 0;
}

static int ds3k_get_property(struct dvb_frontend *fe,
	struct dtv_property *tvp)
{
	dprintk("%s(..)\n", __func__);
	return 0;
}
#endif

static int ds3k_set_carrier_offset(struct dvb_frontend *fe,
					s32 carrier_offset_khz)
{
	struct ds3k_state *state = fe->demodulator_priv;
	s32 tmp;

	tmp = carrier_offset_khz;
	tmp *= 65536;
	tmp = (2 * tmp + DS3000_SAMPLE_RATE) / (2 * DS3000_SAMPLE_RATE);

	if (tmp < 0)
		tmp += 65536;

	ds3k_writereg(state, 0x5f, tmp >> 8);
	ds3k_writereg(state, 0x5e, tmp & 0xff);

	return 0;
}

static int ds3k_setTSdiv(struct ds3k_state *state, int type, u8 tmp1, u8 tmp2)
{
	u8 buf;
	if(type == SYS_DVBS) {
		if(state->chip_ID == FeDmdId_DS300X) {
			tmp1 &= 0x07;
			tmp2 &= 0x07;
			buf = ds3k_readreg(state, 0xfe);
			buf &= 0xc0;
			buf |= ((u8)(((tmp1<<3) + tmp2)) & 0x3f);
			ds3k_writereg(state, 0xfe, buf);
		} else if((state->chip_ID == FeDmdId_DS3002B) || (state->chip_ID == FeDmdId_DS3103)
					|| (state->chip_ID == FeDmdId_DS3103B)) {
			tmp1 -= 1;
			tmp2 -= 1;

			tmp1 &= 0x3f;
			tmp2 &= 0x3f;

			buf = ds3k_readreg(state, 0xfe);
			buf &= 0xF0;
			buf |= (tmp1 >> 2) & 0x0f;
			ds3k_writereg(state, 0xfe, buf);

			buf = (u8)((tmp1 & 0x03) << 6);
			buf |= tmp2;
			ds3k_writereg(state, 0xea, buf);
		} else {
			return -1;
		}
	} else if(type == SYS_DVBS2) {
		if(state->chip_ID == FeDmdId_DS300X) {
			tmp1 &= 0x0f;
			tmp2 &= 0x0f;
			buf = (u8)((tmp1<<4) + tmp2);
			ds3k_writereg(state, 0xfe, buf);
		} else if((state->chip_ID == FeDmdId_DS3002B) || (state->chip_ID == FeDmdId_DS3103)
						|| (state->chip_ID == FeDmdId_DS3103B)) {
			tmp1 -= 1;
			tmp2 -= 1;

			tmp1 &= 0x3f;
			tmp2 &= 0x3f;


			buf = ds3k_readreg(state, 0xfe);
			buf &= 0xF0;						// bits[3:0]
			buf |= (tmp1 >> 2) & 0x0f;
			ds3k_writereg(state, 0xfe, buf);

			buf = (u8)((tmp1 & 0x03) << 6);		// ci_divrange_h_0 bits[1:0]
			buf |= tmp2;						// ci_divrange_l   bits[5:0]
			ds3k_writereg(state, 0xea, buf);
		} else {
			return -1;
		}
	}
	return 0;
}
static int ds3k_set_frontend(struct dvb_frontend *fe)
{
	struct ds3k_state *state = fe->demodulator_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	struct ds3k_config *cfg = state->config;

	int i;
	enum fe_status status;
	s32 offset_khz;
	u16 value;
	u32 tmp;
	u8 tmp1, tmp2;
	u32 target_mclk = 0;
	u32 ts_clk = 24000;
	u16 divide_ratio;

	state->cur_type = c->delivery_system;
	state->cur_symbol_rate = c->symbol_rate;
	state->cur_freqKhz = c->frequency;

	dprintk("%s() frec=%d symb=%d", __func__, c->frequency, c->symbol_rate);

	if (state->config->set_ts_params)
		state->config->set_ts_params(fe, 0);
	//
	if(state->chip_ID == FeDmdId_DS300X) {
		value = ds3k_readreg(state, 0xb2);
		if(value == 0x01) {
			ds3k_writereg(state, 0x05, 0x00);
			ds3k_writereg(state, 0xb2, 0x00);
		}
	} else if((state->chip_ID == FeDmdId_DS3002B) || (state->chip_ID == FeDmdId_DS3103)
					|| (state->chip_ID == FeDmdId_DS3103B)) {
		value = ds3k_readreg(state, 0xb2);
		if(value == 0x01) {
			ds3k_writereg(state, 0x00, 0x00);
			ds3k_writereg(state, 0xb2, 0x00);
		}
	} else {
		printk(KERN_ERR "%s: Unable check tuner version\n", __func__);
		return -1;			//Error, maybe other tuner ICs,please do action at top level application
	}


	if(state->chip_ID == FeDmdId_DS3103B) {
		if(state->cur_type == SYS_DVBS2) {
			target_mclk = 144000;
		} else {
			target_mclk = 96000;
		}

		ds3k_writereg(state, 0x06, 0xe0);
		_mt_fe_dmd_ds3k_select_mclk(state);
		_mt_fe_dmd_ds3k_set_mclk(state, target_mclk);
		ds3k_writereg(state, 0x06, 0x00);
		msleep(10);

	}

	offset_khz = set_freq_ts2022(state) + 500;

	dprintk("%s---cfg->output_mode----->>%d\n", __func__, cfg->output_mode);
	//demod
	ds3k_writereg(state, 0xb2, 0x01);
	if((state->chip_ID == FeDmdId_DS3002B) || (state->chip_ID == FeDmdId_DS3103)
				|| (state->chip_ID == FeDmdId_DS3103B)) {
		ds3k_writereg(state, 0x00, 0x01);
	}

	value = ds3k_readreg(state, 0x08);
	switch (c->delivery_system) {
	case SYS_DVBS:
		/* initialise the demod in DVB-S mode */
		value &= ~0x04;
		ds3k_writereg(state, 0x08, value);
		if(state->chip_ID == FeDmdId_DS300X) {
			for(i = 0; i < sizeof(ds3000_dvbs_init_tab); i += 2)
				ds3k_writereg(state, ds3000_dvbs_init_tab[i], ds3000_dvbs_init_tab[i + 1]);
		} else if((state->chip_ID == FeDmdId_DS3002B) || (state->chip_ID == FeDmdId_DS3103)
					|| (state->chip_ID == FeDmdId_DS3103B)) {
			for (i = 0; i < sizeof(ds310x_dvbs_init_tab); i += 2)
				ds3k_writereg(state, ds310x_dvbs_init_tab[i], ds310x_dvbs_init_tab[i + 1]);
		}

		if(cfg->output_mode == MtFeTsOutMode_Common) {
			ts_clk = 6000; //8000;
		} else if(cfg->output_mode == MtFeTsOutMode_Parallel) {
			ts_clk = 24000;
		} else {
			ts_clk = 0;
		}

		target_mclk = 96000;
		if((state->chip_ID == FeDmdId_DS3002B) || (state->chip_ID == FeDmdId_DS3103)
						|| (state->chip_ID == FeDmdId_DS3103B)) {
			value = ds3k_readreg(state, 0x4d);
			value &= ~0x02;
			ds3k_writereg(state, 0x4d, value);
			value = ds3k_readreg(state, 0x30);
			value &= ~0x10;
			ds3k_writereg(state, 0x30, value);
		}
		if(state->chip_ID == FeDmdId_DS3103B) {
			// S mode, disable 192M LDPC clock, Reg. 29H bit4 = 1
			value = ds3k_readreg(state, 0x29);
			value |= 0x10;
			ds3k_writereg(state, 0x29, value);
		}

		break;
	case SYS_DVBS2:
		/* initialise the demod in DVB-S2 mode */
		value |= 0x04;
		ds3k_writereg(state, 0x08, value);
		if(state->chip_ID == FeDmdId_DS300X)
		{
			for (i = 0; i < sizeof(ds3000_dvbs2_init_tab); i += 2)
				ds3k_writereg(state, ds3000_dvbs2_init_tab[i], ds3000_dvbs2_init_tab[i + 1]);
		} else if((state->chip_ID == FeDmdId_DS3002B) || (state->chip_ID == FeDmdId_DS3103)
						|| (state->chip_ID == FeDmdId_DS3103B)) {
			for (i = 0; i < sizeof(ds310x_dvbs2_init_tab); i += 2)
				ds3k_writereg(state, ds310x_dvbs2_init_tab[i], ds310x_dvbs2_init_tab[i + 1]);
		}
		//ts_clk = 18000;//zf8471;

		if(cfg->output_mode == MtFeTsOutMode_Common) {
			ts_clk = 6000;	//8471;
		} else if(cfg->output_mode == MtFeTsOutMode_Parallel) {
			ts_clk = 24000;
		} else {
			ts_clk = 0;
		}

		if(state->chip_ID == FeDmdId_DS300X) {
			target_mclk = 144000;
		} else if((state->chip_ID == FeDmdId_DS3002B) || (state->chip_ID == FeDmdId_DS3103)
							|| (state->chip_ID == FeDmdId_DS3103B)) {
			value = ds3k_readreg(state, 0x4d);
			value &= ~0x02;
			ds3k_writereg(state, 0x4d, value);
			value = ds3k_readreg(state, 0x30);
			value &= ~0x10;
			ds3k_writereg(state, 0x30, value);
			if((cfg->output_mode == MtFeTsOutMode_Parallel) || (cfg->output_mode == MtFeTsOutMode_Common)) {
				if(c->symbol_rate > 18000000) {
					target_mclk = 144000;
				} else {
					target_mclk = 96000;
				}
			} else {
#if (MT_FE_TS_CLOCK_AUTO_SET_FOR_SERIAL_MODE != 0)
				if(c->symbol_rate > 18000000) {
					target_mclk = 144000;
				} else {
					target_mclk = 96000;
				}
#else
				target_mclk = MT_FE_MCLK_KHZ_SERIAL_S2;
#endif
			}
		}

		if(state->chip_ID == FeDmdId_DS3103B) {
			// S2 mode, enable 192M LDPC clock, Reg. 29H bit4 = 0
			value = ds3k_readreg(state, 0x29);
			value &= ~0x10;
			ds3k_writereg(state, 0x29, value);
		}

		if(c->symbol_rate <= 5000000)
		{
			ds3k_writereg(state, 0xc0, 0x04);
			ds3k_writereg(state, 0x8a, 0x09);
			ds3k_writereg(state, 0x8b, 0x22);
			ds3k_writereg(state, 0x8c, 0x88);
		}

		break;
	default:
		return 1;
	}

	if(state->chip_ID == FeDmdId_DS3103B) {
		value = ds3k_readreg(state, 0x9d);
		value |= 0x08;
		ds3k_writereg(state, 0x9d, value);

		tmp1 = ds3k_dt_read(state, 0x15);
		tmp2 = ds3k_dt_read(state, 0x16);

		if(c->symbol_rate > 45000000) {
			tmp1 &= ~0x03;
			tmp1 |= 0x02;
			tmp1 |= ((147 - 32) >> 8) & 0x01;
			tmp2 = (147 - 32) & 0xFF;

			state->mclk_khz = 110250;
		} else {
			tmp1 &= ~0x03;
			tmp1 |= ((128 - 32) >> 8) & 0x01;
			tmp2 = (128 - 32) & 0xFF;

			state->mclk_khz = 96000;
		}
		ds3k_dt_write(state, 0x15, tmp1);
		ds3k_dt_write(state, 0x16, tmp2);

		value = ds3k_readreg(state, 0x30);
		value &= ~0x80;
		ds3k_writereg(state, 0x30, value);
	}

	if(ts_clk != 0) {
		divide_ratio = (target_mclk + ts_clk - 1) / ts_clk;

		if(divide_ratio > 128)
			divide_ratio = 128;

		if(divide_ratio < 2)
			divide_ratio = 2;

		tmp1 = (u8)(divide_ratio / 2);
		tmp2 = (u8)(divide_ratio / 2);

		if((divide_ratio % 2) != 0)
			tmp2 += 1;
	} else {
		divide_ratio = 0;
		tmp1 = 0;
		tmp2 = 0;
	}

	ds3k_setTSdiv(state, c->delivery_system, tmp1, tmp2);
	_mt_fe_dmd_ds3k_set_mclk(state, target_mclk);

	value = ds3k_readreg(state, 0xfd);
	tmp = ds3k_readreg(state, 0xca);
	tmp &= 0xFE;
	tmp |= (value >> 3) & 0x01;
	ds3k_writereg(state, 0xca, tmp);

	ds3k_writereg(state, 0x33, 0x99);

	if(state->chip_ID == FeDmdId_DS3103B) {
		tmp = ds3k_readreg(state, 0xC9);
		tmp |= 0x08;
		ds3k_writereg(state, 0xC9, tmp);
	}

	/* enhance symbol rate performance */
	if(c->symbol_rate <= 3000000)
	{
		ds3k_writereg(state, 0xc3, 0x08); // 8 * 32 * 100 / 64 = 400
		ds3k_writereg(state, 0xc8, 0x20);
		ds3k_writereg(state, 0xc4, 0x08); // 8 * 0 * 100 / 128 = 0
		ds3k_writereg(state, 0xc7, 0x00);
	}
	else if(c->symbol_rate <= 10000000)
	{
		ds3k_writereg(state, 0xc3, 0x08); // 8 * 16 * 100 / 64 = 200
		ds3k_writereg(state, 0xc8, 0x10);
		ds3k_writereg(state, 0xc4, 0x08); // 8 * 0 * 100 / 128 = 0
		ds3k_writereg(state, 0xc7, 0x00);
	}
	else
	{
		ds3k_writereg(state, 0xc3, 0x08); // 8 * 6 * 100 / 64 = 75
		ds3k_writereg(state, 0xc8, 0x06);
		ds3k_writereg(state, 0xc4, 0x08); // 8 * 0 * 100 / 128 = 0
		ds3k_writereg(state, 0xc7, 0x00);
	}

	/* normalized symbol rate rounded to the closest integer */

	dprintk("%s----state->mclk_khz----->>%d\n", __func__, state->mclk_khz);
	tmp = (u32)((((c->symbol_rate / 1000) << 15) + (state->mclk_khz / 4)) / (state->mclk_khz / 2));

	ds3k_writereg(state, 0x61, tmp & 0x00ff);
	ds3k_writereg(state, 0x62, (tmp & 0xff00) >> 8);

	/* co-channel interference cancellation disabled */
	value = ds3k_readreg(state, 0x56);
	value &= ~0x01;
	ds3k_writereg(state, 0x56, value);
	/* equalizer disabled */
	value = ds3k_readreg(state, 0x76);
	value &= ~0x80;
	ds3k_writereg(state, 0x76, value);

	//offset
	if ((c->symbol_rate / 1000) < 5000)
		offset_khz += 3000;
	ds3k_set_carrier_offset(fe, offset_khz);

	if((state->chip_ID == FeDmdId_DS3002B) || (state->chip_ID == FeDmdId_DS3103)
				|| state->chip_ID == FeDmdId_DS3103B) {
		/* ds3000 out of software reset */
		ds3k_writereg(state, 0x00, 0x00);
	}
	/* start ds3000 build-in uC */
	ds3k_writereg(state, 0xb2, 0x00);


	for (i = 0; i < 30 ; i++) {
		ds3k_read_status(fe, &status);
		if (status && FE_HAS_LOCK)
			break;

		msleep(10);
	}

	return 0;
}

static int ds3k_tune(struct dvb_frontend *fe,
			bool re_tune,
			unsigned int mode_flags,
			unsigned int *delay,
			enum fe_status *status)
{
	if (re_tune) {
		int ret = ds3k_set_frontend(fe);
		if (ret)
			return ret;
	}

	*delay = HZ / 5;

	return ds3k_read_status(fe, status);
}

static enum dvbfe_algo ds3k_get_algo(struct dvb_frontend *fe)
{
	//dprintk("%s()\n", __func__);
	return DVBFE_ALGO_HW;
}

/*
 * Initialise or wake up device
 *
 * Power config will reset and load initial firmware if required
 */
static int ds3k_initfe(struct dvb_frontend *fe)
{
	struct ds3k_state *state = fe->demodulator_priv;
	struct ds3k_config *cfg = state->config;
	int ret;
	u8 buf;
	u8 val_08;

	state->mclk_khz = DS3000_SAMPLE_RATE;

	dprintk("%s()\n", __func__);
	/* hard reset */
	if(state->chip_ID==FeDmdId_DS300X)
	{
	      buf = ds3k_readreg(state, 0xb2);
		if(buf == 0x01)
		{
			ds3k_writereg(state, 0x05, 0x00);
			ds3k_writereg(state, 0xb2, 0x00);
		}
	}
	else if ((state->chip_ID == FeDmdId_DS3002B) || (state->chip_ID == FeDmdId_DS3103)
				|| (state->chip_ID == FeDmdId_DS3103B))
	{
		buf = ds3k_readreg(state, 0xb2);
		if(buf == 0x01)
		{
			ds3k_writereg(state, 0x00, 0x00);
			ds3k_writereg(state, 0xb2, 0x00);
		}
	}
	else
	{
		printk(KERN_ERR "%s: unknow demod version\n", __func__);
		return -1;			//Error, maybe other tuner ICs,please do action at top level application
	}
	ds3k_writereg(state, 0x07, 0x80);
	ds3k_writereg(state, 0x07, 0x00);
	ds3k_writereg(state, 0xb2, 0x00);
	msleep(1);
	ds3k_writereg(state, 0x08, 0x01 | ds3k_readreg(state, 0x08));

	msleep(1);
	if (state->chip_ID == FeDmdId_DS3103B) {
		ds3k_writereg(state, 0x11, 0x01 | ds3k_readreg(state, 0x11));
	}

	//check tuner version
	// Wake Up the tuner
	buf = ds3k_tuner_readreg(state, 0x00);
	buf &= 0x03;
	if(buf == 0x00)
	{
		ds3k_tuner_writereg(state, 0x00, 0x01);
		msleep(2);
	}
	ds3k_tuner_writereg(state, 0x00, 0x03);
	msleep(2);

	//Check the tuner version

	buf = ds3k_tuner_readreg(state, 0x00);

	if((buf == 0x01) || (buf == 0x41) || (buf == 0x81))
	{
		dprintk("FIND TUNER M88TS2020");		//A0 or A4 or A5
		/* TS2020 init */
		state->tuner_ID = TUNER_M88TS2020;
		ds3k_tuner_writereg(state, 0x62, 0xfd);
		ds3k_tuner_writereg(state, 0x42, 0x63);
		ds3k_tuner_writereg(state, 0x07, 0x02);
		ds3k_tuner_writereg(state, 0x08, 0x01);
	}
	else if((buf == 0xc3)|| (buf == 0x83) || (buf == 0xc1)) //yh add c1
	{
		dprintk("FIND TUNER_M88TS2022");		//C0 or C0A
		/* TS2022 init */
		state->tuner_ID = TUNER_M88TS2022;

		ds3k_tuner_writereg(state, 0x62, 0xec);
		msleep(2);
		ds3k_tuner_writereg(state, 0x42, 0x6c);
		msleep(2);

		ds3k_tuner_writereg(state, 0x7d, 0x9d);
		ds3k_tuner_writereg(state, 0x7c, 0x9a);
		ds3k_tuner_writereg(state, 0x7a, 0x76);

		ds3k_tuner_writereg(state, 0x3b, 0x01);
		ds3k_tuner_writereg(state, 0x63, 0x88);

		ds3k_tuner_writereg(state, 0x61, 0x85);
		ds3k_tuner_writereg(state, 0x22, 0x30);
		ds3k_tuner_writereg(state, 0x30, 0x40);
		ds3k_tuner_writereg(state, 0x20, 0x23);
		ds3k_tuner_writereg(state, 0x24, 0x02);
		ds3k_tuner_writereg(state, 0x12, 0xa0);
	}
	else
	{
		state->tuner_ID = TUNER_UNKNOW;
		printk(KERN_ERR "%s: Unable check tuner version\n", __func__);
		return -1;			//Error, maybe other tuner ICs,please do action at top level application
	}

	if(state->chip_ID == FeDmdId_DS3103B) {
		ds3k_dt_write(state, 0x21, 0x92);
		ds3k_dt_write(state, 0x15, 0x6C);
		ds3k_dt_write(state, 0x17, 0xC1);
		ds3k_dt_write(state, 0x17, 0x81);
	}

	/* Load the firmware if required */
	ret = ds3k_firmware_ondemand(fe);
	if (ret != 0) {
		printk(KERN_ERR "%s: Unable initialize firmware\n", __func__);
		return ret;
	}
	//TS mode
	val_08 = ds3k_readreg(state, 0x08);
	if(state->chip_ID == FeDmdId_DS3103B) {
		buf = ds3k_readreg(state, 0x29);
		buf &= ~0x01;
		ds3k_writereg(state, 0x29, buf);
	} else {
		buf = ds3k_readreg(state, 0x27);
		buf &= ~0x01;
		ds3k_writereg(state, 0x27, buf);
	}

	//dvbs
	buf = val_08 & (~0x04) ;
	ds3k_writereg(state, 0x08, buf);
	ds3k_setTSdiv(state, SYS_DVBS, 6, 6);


//lxg	if(state->chip_ID == FeDmdId_DS300X) {
		buf = ds3k_readreg(state, 0xfd);
		if(cfg->output_mode == MtFeTsOutMode_Parallel) {
			buf &= ~0x80;
			buf &= ~0x40;
		} else if(cfg->output_mode == MtFeTsOutMode_Serial) {
			buf &= ~0x80;
			buf |= 0x40;
		} else {
			buf |= 0x80;
			buf &= ~0x40;
		}

		if(cfg->ci_mode)
			buf |= 0x20;
		else
			buf &= ~0x20;
		buf &= ~0x1f;
		ds3k_writereg(state, 0xfd, buf);
//	}

	//S2
	buf = val_08 | 0x04 ;
	ds3k_writereg(state, 0x08, buf);
//	if(state->chip_ID == FeDmdId_DS3103B)
//		ds3k_setTSdiv(state, SYS_DVBS2, 8, 8);
//	else
		ds3k_setTSdiv(state, SYS_DVBS2, 8, 9);
	buf = ds3k_readreg(state, 0xfd);
	if(cfg->output_mode == MtFeTsOutMode_Parallel) {
		buf &= ~0x01;
		buf &= ~0x04;
	} else if(cfg->output_mode == MtFeTsOutMode_Serial) {
		buf &= ~0x01;
		buf |= 0x04;
	} else {
		buf |= 0x01;
		buf &= ~0x04;
	}
	if(cfg->ci_mode) {
		buf &= ~0xba;
		buf |= 0x40;
	} else {
		buf &= ~0xb8;
		buf &= ~0x42;	//lxg buf |= 0x42;
	}
	ds3k_writereg(state, 0xfd, buf);

	ds3k_writereg(state, 0x08, val_08);

	if(state->chip_ID == FeDmdId_DS3103B) {
		u8 val = 0, tmp;
		if(cfg->output_mode != MtFeTsOutMode_Serial) {
			tmp = MT_FE_ENHANCE_TS_PIN_LEVEL_PARALLEL_CI;

			val |= tmp & 0x03;
			val |= (tmp << 2) & 0x0C;
			val |= (tmp << 4) & 0x30;
			val |= (tmp << 6) & 0xC0;
		} else {
			tmp = MT_FE_ENHANCE_TS_PIN_LEVEL_SERIAL;

			val |= tmp & 0x03;
			val |= (tmp << 2) & 0x0C;
			val |= (tmp << 4) & 0x30;
			val |= (tmp << 6) & 0xC0;
		}
		ds3k_writereg(state, 0x20, val);
	} else {
		buf = ds3k_readreg(state, 0x27);
		buf |= 0x01;
		if(((MT_FE_ENHANCE_TS_PIN_LEVEL_PARALLEL_CI != 0) && (cfg->output_mode != MtFeTsOutMode_Serial))
		 		|| ((MT_FE_ENHANCE_TS_PIN_LEVEL_SERIAL != 0) && (cfg->output_mode == MtFeTsOutMode_Serial))) {
			buf &= ~0x10;
		} else {
			buf |= 0x10;
		}
		ds3k_writereg(state, 0x27, buf);
	}

	buf = ds3k_readreg(state, 0x29);
	if(state->chip_ID == FeDmdId_DS3103B) {
		if(MT_FE_TS_PIN_ORDER_D0_D7 != 0) {
			buf |= 0x20;
		} else {
			buf &= ~0x20;
		}

		buf |= 0x01;
	} else {
		if((MT_FE_TS_PIN_ORDER_D0_D7 != 0) && (cfg->output_mode == MtFeTsOutMode_Serial)) {
			buf |= 0x20;
		} else {
			buf &= ~0x20;
		}
	}
	ds3k_writereg(state, 0x29, buf);
	// Ts mode end


	if(state->chip_ID != FeDmdId_DS3103B) {
		buf = ds3k_readreg(state, 0x29);
#if MT_FE_ENABLE_27MHZ_CLOCK_OUT
		buf &= ~0x80;
#if MT_FE_ENABLE_13_P_5_MHZ_CLOCK_OUT
		buf |= 0x10;
#else
		buf &= ~0x10;
#endif
#else
		buf |= 0x80;
#endif
		ds3k_writereg(state, 0x29, buf);
	} else {
		buf = ds3k_readreg(state, 0x11);
#if (MT_FE_ENABLE_27MHZ_CLOCK_OUT && (ENABLE_CLKOUT_AS_GPIO == 0))
		buf &= ~0x08;
#elif((MT_FE_ENABLE_27MHZ_CLOCK_OUT == 0) && ENABLE_CLKOUT_AS_GPIO)
		buf |= 0x08;
#if GPIO_CLKOUT_AS_INPUT
		buf |= 0x04;
#else
		buf &= ~0x04;
#endif
#endif

#if ENABLE_ITLOCK_AS_GPIO
		buf |= 0x80;
#if GPIO_ITLOCK_AS_INPUT
		buf |= 0x40;
#else
		buf &= ~0x40;
#endif
#else
		buf &= ~0x80;
#endif
		ds3k_writereg(state, 0x11, buf);
	}

#if MT_FE_ENABLE_AC_COUPLING
	buf = ds3k_readreg(state, 0x25);
	buf |= 0x08;
	ds3k_writereg(state, 0x25, buf);
#endif

	//
	if((state->chip_ID == FeDmdId_DS3002B) || (state->chip_ID == FeDmdId_DS3103)
			|| (state->chip_ID == FeDmdId_DS3103B)) {
		buf = ds3k_readreg(state, 0x4d);
		buf &= ~0x02;
		ds3k_writereg(state, 0x4d, buf);
		buf = ds3k_readreg(state, 0x30);
		buf &= ~0x10;
		ds3k_writereg(state, 0x30, buf);

		ds3k_writereg(state, 0xf1, 0x01);
		if(state->chip_ID == FeDmdId_DS3103B) {
			buf = ds3k_readreg(state, 0x29);
			if(LNB_DISEQC_OUT_ONLY_OUTPUT == 1) {
				buf |= 0x40;
			} else {
				buf &= ~0x40;
			}
			ds3k_writereg(state, 0x29, buf);

			buf = ds3k_readreg(state, 0x9d);
			buf |= 0x08;
			ds3k_writereg(state, 0x9d, buf);
		}
	}
	return 0;
}

/* Put device to sleep */
static int ds3k_sleep(struct dvb_frontend *fe)
{
	struct ds3k_state *state = fe->demodulator_priv;

	if (state->config->set_lock_led)
		state->config->set_lock_led(fe, 0);

	dprintk("%s()\n", __func__);
	return 0;
}

static struct dvb_frontend_ops ds3k_ops = {
	.delsys = { SYS_DVBS, SYS_DVBS2 },
	.info = {
		.name = "Montage Technology DS3000/TS2020",
		.frequency_min_hz =  950 * MHz,
		.frequency_max_hz = 2150 * MHz,
		.frequency_stepsize_hz = 1011 * kHz,
		.frequency_tolerance_hz = 5 * MHz,
		.symbol_rate_min = 1000000,
		.symbol_rate_max = 45000000,
		.caps = FE_CAN_INVERSION_AUTO |
			FE_CAN_FEC_1_2 | FE_CAN_FEC_2_3 | FE_CAN_FEC_3_4 |
			FE_CAN_FEC_4_5 | FE_CAN_FEC_5_6 | FE_CAN_FEC_6_7 |
			FE_CAN_FEC_7_8 | FE_CAN_FEC_AUTO |
			FE_CAN_2G_MODULATION |
			FE_CAN_QPSK | FE_CAN_RECOVER
	},

	.release = ds3k_release,

	.init = ds3k_initfe,
	.sleep = ds3k_sleep,
	.read_status = ds3k_read_status,
	.read_ber = ds3k_read_ber,
	.read_signal_strength = ds3k_read_signal_strength,
	.read_snr = ds3k_read_snr,
	.read_ucblocks = ds3k_read_ucblocks,
	.set_voltage = ds3k_set_voltage,
	.set_tone = ds3k_set_tone,
	.diseqc_send_master_cmd = ds3k_send_diseqc_msg,
	.diseqc_send_burst = ds3k_diseqc_send_burst,
	.get_frontend_algo = ds3k_get_algo,

	//.set_property = ds3k_set_property,
	//.get_property = ds3k_get_property,
	.set_frontend = ds3k_set_frontend,
	.tune = ds3k_tune,
};

module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "Activates frontend debugging (default:0)");

MODULE_DESCRIPTION("DVB Frontend module for Montage Technology "
			"TS2020/TS2022/DS300x/DS3103 hardware");
MODULE_AUTHOR("Geniatech");
MODULE_LICENSE("GPL");
