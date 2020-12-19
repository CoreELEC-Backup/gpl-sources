/* DVB USB framework compliant Linux driver for the
*	Geniatech CI TV USB2.0 Cards
*	Copyright (C) 2015 Geniatech 
*
*	This program is free software; you can redistribute it and/or modify it
*	under the terms of the GNU General Public License as published by the
*	Free Software Foundation, version 2.
*
* see Documentation/dvb/README.dvb-usb for more information
*/

#include "cycitv.h"
#include "ds3k.h"
#include <media/dvb_ca_en50221.h>

#ifndef USB_PID_CYCITV_COLD
#define USB_PID_CYCITV_COLD 0x613d
#endif

#ifndef USB_PID_CYCITV_WORK
#define USB_PID_CYCITV_WORK  0x613f
#endif

#ifndef USB_PID_CYCITV_COLD_2
#define USB_PID_CYCITV_COLD_2 0x614d
#endif

#ifndef USB_PID_CYCITV_WORK_2
#define USB_PID_CYCITV_WORK_2  0x614f
#endif

#ifndef USB_VID_GENIATECH
#define USB_VID_GENIATECH  0x1f4d
#endif

#define CYCITV_READ_MSG 0
#define CYCITV_WRITE_MSG 1

#define VX_RESETPIO_10 0x10
#define VX_I2CBURSTWRITE_20 0x20
#define VX_I2CBURSTREAD_21 0x21
#define VX_I2CWRITE_22 0x22
#define VX_I2CREAD_24 0x24
#define VX_RAWI2C_25 0x25
#define VX_RAWI2CREAD_26 0x26
#define VX_GETFWVERSION_27 0x27
#define VX_GPIOSET_40 0x40
#define VX_GPIOGET_41 0x41
#define VX_PCMCIA_IO_WRITE 0x60
#define VX_PCMCIA_IO_READ 0x61

#define VX_PCMCIA_MEM_WRITE 0x62
#define VX_PCMCIA_MEM_READ 0x63

#define SMITRAWI2C_START 0x01
#define SMITRAWI2C_WRITEDATA 0x02
#define SMITRAWI2C_STOP 0x04
#define SMITRAWI2C_READDATA 0x06

#define OUTPASSB 0x08
#define OUTPASSA 0x04
#define OUTLNBEN 0x02
#define OUTTURST 0x01
#define OUTCRSTA 0x80
#define OUTCRSTB 0x40

#define OUT_HIGH 0xff
#define OUT_LOW  0x00



#define INCD2A 0x08
#define INCD1A 0x04
#define INCD2B 0x02
#define INCD1B 0x01

#define CMD_I2C_WRITE  0x08
#define CMD_I2C_READ   0x09
//#define CMD_I2C_READ   0x98

#define CMD_ENABEL_IR  0x0f
#define CMD_GET_IR     0x10

#define CMD_SET_GPIO   0x1E

#define CMD_START_TS   0x36
#define CMD_STOP_TS    0x37

#define CMD_WAKEUP     0xde

#define CMD_MEM_READ    0x90
#define CMD_MEM_WRITE   0x91
#define CMD_IO_READ     0x92
#define CMD_IO_WRITE    0x93
#define CMD_GPIO_READ   0x94
#define CMD_GPIO_WRITE  0x95
#define CMD_ID_READ     0x96
#define CMD_INIT_WRITE  0x97


struct rc_map_dvb_usb_table_table {
	struct rc_map_table *rc_keys;
	int rc_keys_size;
};

struct cycitv_state {
	struct dvb_ca_en50221 ca;
	struct mutex ca_mutex;
    u8 gpio;
    u8 initialized;
//	u8 c;	   /* transaction counter, wraps around...  */
//	u8 initialized; /* set to 1 if 0x15 has been sent */
//	u16 last_rc_key;
};


/* debug */
static int dvb_usb_cycitv_debug = 0x07;
module_param_named(debug, dvb_usb_cycitv_debug, int, 0644);
MODULE_PARM_DESC(debug, "set debugging level (1=info 2=xfer 4=rc(or-able))."
						DVB_USB_DEBUG_STATUS);

/* keymaps */
static int ir_keymap;
module_param_named(keymap, ir_keymap, int, 0644);
MODULE_PARM_DESC(keymap, "set keymap 0=default 1=dvbworld 2=tevii 3=tbs  ..."
			" 256=none");

DVB_DEFINE_MOD_OPT_ADAPTER_NR(adapter_nr);


/* GPIO */
static int cycitv_gpio_set(struct dvb_usb_device *d, u8 mask, u8 val)
{
	struct cycitv_state *state = (struct cycitv_state *)d->priv;

	u8 gpio_port = state->gpio;
	u8 obuf[0x40], ibuf[0x40];

	deb_xfer("cycitv_gpio_set gpio=%02x mask=%02x val =%02x\n",gpio_port, mask, val);
	gpio_port &= ~mask;
	gpio_port |= (mask & val);

	obuf[0] = CMD_GPIO_WRITE;
	obuf[1] = gpio_port;
	if (dvb_usb_generic_rw(d, obuf, 2, ibuf, 0, 0) < 0)
		return -EREMOTEIO;
	state->gpio = gpio_port;
	return 0;
}

static int cycitv_gpio_get(struct dvb_usb_device *d, u8* val)
{
	u8 obuf[0x40], ibuf[0x40];

	obuf[0] = CMD_GPIO_READ;
	if (dvb_usb_generic_rw(d, obuf, 1, ibuf, 1, 0) < 0)
		return -EREMOTEIO;
	*val = ibuf[0];
	deb_ca("cycitv_gpio_get gpio=%02x \n", *val);
	return 0;
}

/* I2C */
static int cycitv_i2c_transfer(struct i2c_adapter *adap, struct i2c_msg msg[],
		int num)
{
	struct dvb_usb_device *d = i2c_get_adapdata(adap);
	u8 obuf[0x40], ibuf[0x40];

	if (!d)
		return -ENODEV;
	if (mutex_lock_interruptible(&d->i2c_mutex) < 0)
		return -EAGAIN;

	switch (num) {
	case 1:
	if(msg[0].flags == I2C_M_RD) {
	    obuf[0] = CMD_I2C_READ;
	    obuf[1] = 0;
	    obuf[2] = msg[0].len;
	    obuf[3] = msg[0].addr;

	    if (dvb_usb_generic_rw(d, obuf, 4,
			ibuf, msg[0].len + 1, 0) < 0)
		err("i2c transfer failed.");

	    memcpy(msg[0].buf, &ibuf[1], msg[0].len);
/*
	    if(msg[0].len==1)
	    {
		deb_xfer("cycitv_i2c_readonly --------------addr=%02x - [  ]<-[%02x]-----ret=%d\n", (msg[0].addr)<<1, msg[0].buf[0] ,ibuf[0]);
	    } else
		deb_xfer("cycitv_i2c_readonly addr=%02x --rbyte=%d---ret=%d\n", msg[0].addr, msg[0].len ,ibuf[0]);
*/
	} else {

		/* always i2c write*/
		obuf[0] = CMD_I2C_WRITE;
		obuf[1] = msg[0].addr;
		obuf[2] = msg[0].len;

		memcpy(&obuf[3], msg[0].buf, msg[0].len);

		if (dvb_usb_generic_rw(d, obuf, msg[0].len + 3,
			ibuf, 1, 0) < 0)
		err("i2c transfer failed.");
/*          if(msg[0].len==2)
	    {
		deb_xfer("cycitv_i2c_write -----------------addr=%02x - [%02x]->[%02x]-----ret=%d\n", (msg[0].addr)<<1, msg[0].buf[0] , msg[0].buf[1] ,ibuf[0]);
	    } else if(msg[0].len==11)
	    {
		deb_xfer("cycitv_i2c_write -----------------addr=%02x - [%02x%02x]->[%02x %02x %02x %02x %02x %02x %02x %02x %02x ]-----ret=%d\n",
		    (msg[0].addr)<<1, msg[0].buf[0] , msg[0].buf[1] ,
		    msg[0].buf[2], msg[0].buf[3], msg[0].buf[4], msg[0].buf[5], msg[0].buf[6], msg[0].buf[7], msg[0].buf[8], msg[0].buf[9], msg[0].buf[10]
		    ,ibuf[0]);
	  } else
	      deb_xfer("cycitv_i2c_write addr=%02x byte=%d ret=%d\n", msg[0].addr, msg[0].len ,ibuf[0]);
      */  }
	break;
	case 2:
		/* always i2c read */
		obuf[0] = CMD_I2C_READ;
		obuf[1] = msg[0].len;
		obuf[2] = msg[1].len;
		obuf[3] = msg[0].addr;
		memcpy(&obuf[4], msg[0].buf, msg[0].len);

		if (dvb_usb_generic_rw(d, obuf, msg[0].len + 4,
					ibuf, msg[1].len + 1, 0) < 0)
			err("i2c transfer failed.");

		memcpy(msg[1].buf, &ibuf[1], msg[1].len);
/*
	if((msg[0].len==1)&&(msg[1].len==1))
	{
	    deb_xfer("cycitv_i2c_readwrite -------------addr=%02x - [%02x]<-[%02x]-----ret=%d\n", (msg[0].addr)<<1, msg[0].buf[0], msg[1].buf[0],ibuf[0]);
	} else if((msg[0].len==2)&&(msg[1].len==1))
	{
	    deb_xfer("cycitv_i2c_readwrite -------------addr=%02x - [%02x%02x]<-[%02x]-----ret=%d\n", (msg[0].addr)<<1, msg[0].buf[0], msg[0].buf[1], msg[1].buf[0],ibuf[0]);
	} else
	    deb_xfer("cycitv_i2c_readwrite addr=%02x wbyte=%d--rbyte=%d---ret=%d\n", msg[0].addr, msg[0].len, msg[1].len,ibuf[0]);
*/
		break;
	default:
		warn("more than 2 i2c messages at a time is not handled yet.");
		break;
	}
	mutex_unlock(&d->i2c_mutex);
	return num;
}

static u32 cycitv_i2c_func(struct i2c_adapter *adapter) {
	return I2C_FUNC_I2C;
}

static struct i2c_algorithm cycitv_i2c_algo = {
	.master_xfer = cycitv_i2c_transfer,
	.functionality = cycitv_i2c_func,
};

static int cycitv_cam_power_ctrl(struct dvb_usb_device *d, int onoff)
{
	u8 obuf[0x40], ibuf[0x40];

	obuf[0] = CMD_SET_GPIO;
	obuf[1] = onoff ? 1:0;
	if (dvb_usb_generic_rw(d, obuf, 2, ibuf, 0, 0) < 0)
		err("cmd transfer failed.\n");
	if(onoff) {
		msleep(200);
		obuf[0] = CMD_INIT_WRITE;
		obuf[1] = 0;
		if (dvb_usb_generic_rw(d, obuf, 2, ibuf, 0, 0) < 0)
			err("cmd transfer failed.\n");
	}
	return 0;
}

struct cycitv_adapter_state {
	u8 board_type;
};

static int cycitv_streaming_ctrl(struct dvb_usb_adapter *adap, int onoff)
{
	struct cycitv_adapter_state *state = (struct cycitv_adapter_state *)adap->priv;
	u8 obuf[0x40], ibuf[0x40];
	u8 tsckinv ;
	deb_info("%s onoff=%d\n", __func__,onoff);

	tsckinv = 0;//

	obuf[0] = onoff ? CMD_START_TS:CMD_STOP_TS;
	obuf[1] = tsckinv;
	if (dvb_usb_generic_rw(adap->dev, obuf, 2, ibuf, 0, 0) < 0)
		err("cmd transfer failed.\n");
	return 0;
}

static int cycitv_ci_read_attribute_mem(struct dvb_ca_en50221 *ca,
				int slot, int address)
{
	struct dvb_usb_device *d = (struct dvb_usb_device *)ca->data;
	struct cycitv_state *state = (struct cycitv_state *)d->priv;
	int ret;
	u8 obuf[0x40], ibuf[0x40];

	if (!d)
		return -ENODEV;
	if ((0 != slot)&&(1 != slot))
		return -EINVAL;

	obuf[0] = CMD_MEM_READ;
	obuf[1] = (u8) slot;
	obuf[2] = (u8)((address>>8)&0xff);
	obuf[3] = (u8)((address)&0xff);

	mutex_lock(&state->ca_mutex);

	ret = dvb_usb_generic_rw(d, obuf, 4, ibuf, 1, 0);

	mutex_unlock(&state->ca_mutex);
	deb_ca("%s(%d) %04x -> %d 0x%02x\n",
		__func__, slot, address, ret, ibuf[0]);

	if (ret < 0)
		return ret;

	return ibuf[0];
}

static int cycitv_ci_write_attribute_mem(struct dvb_ca_en50221 *ca,
				int slot, int address, u8 value)
{
	struct dvb_usb_device *d = (struct dvb_usb_device *)ca->data;
	struct cycitv_state *state = (struct cycitv_state *)d->priv;
	int ret;
	u8 obuf[0x40], ibuf[0x40];

	if (!d)
		return -ENODEV;

	if ((0 != slot)&&(1 != slot))
		return -EINVAL;

    deb_ca("%s(%d) 0x%04x 0x%02x\n",
		__func__, slot, address, value);

    obuf[0] = CMD_MEM_WRITE;
    obuf[1] = (u8) slot;
    obuf[2] = (u8)((address>>8)&0xff);
    obuf[3] = (u8)((address)&0xff);
    obuf[4] = value;

    mutex_lock(&state->ca_mutex);

    ret = dvb_usb_generic_rw(d, obuf, 5, ibuf, 0, 0);

    mutex_unlock(&state->ca_mutex);

	if( ret < 0 ) return ret;
	return 0;
}

static int cycitv_ci_read_cam_control(struct dvb_ca_en50221 *ca,
				int			slot,
				u8			address)
{
	struct dvb_usb_device *d = (struct dvb_usb_device *)ca->data;
	struct cycitv_state *state = (struct cycitv_state *)d->priv;
	int ret;
	u8 obuf[0x40], ibuf[0x40];

	if (!d)
		return -ENODEV;
	if ((0 != slot)&&(1 != slot))
		return -EINVAL;

	obuf[0] = CMD_IO_READ;
	obuf[1] = (u8) slot;
	obuf[2] = address;

	mutex_lock(&state->ca_mutex);

	ret = dvb_usb_generic_rw(d, obuf, 3, ibuf, 1, 0);

	mutex_unlock(&state->ca_mutex);

	deb_ca("%s(%d) 0x%02x -> %d 0x%02x\n",
		__func__, slot, address, ret, ibuf[0]);

	if (ret < 0)
		return ret;

	return ibuf[0];
}

static int cycitv_ci_write_cam_control(struct dvb_ca_en50221 *ca,
				int			slot,
				u8			address,
				u8			value)
{
	struct dvb_usb_device *d = (struct dvb_usb_device *)ca->data;
	struct cycitv_state *state = (struct cycitv_state *)d->priv;
	int ret;
	u8 obuf[0x40], ibuf[0x40];

	if (!d)
		return -ENODEV;

	if ((0 != slot)&&(1 != slot))
		return -EINVAL;

	deb_ca("%s(%d) 0x%02x 0x%02x\n",
		    __func__, slot, address, value);

	obuf[0] = CMD_IO_WRITE;
	obuf[1] = (u8) slot;
	obuf[2] = address;
	obuf[3] = value;

	mutex_lock(&state->ca_mutex);

	ret = dvb_usb_generic_rw(d, obuf, 4, ibuf, 0, 0);

	mutex_unlock(&state->ca_mutex);

	if( ret < 0 ) return ret;
	return 0;
}

static int cycitv_ci_slot_reset(struct dvb_ca_en50221 *ca, int slot)
{
	struct dvb_usb_device *d = (struct dvb_usb_device *)ca->data;
	struct cycitv_state *state = (struct cycitv_state *)d->priv;
	int ret;

	deb_ca("%s %d\n", __func__, slot);
	if (!d)
		return -ENODEV;

	if ((0 != slot)&&(1 != slot))
		return -EINVAL;

	mutex_lock(&state->ca_mutex);

	if ((ret = cycitv_gpio_set(d,(slot==0)?OUTCRSTA:OUTCRSTB,OUT_HIGH)) < 0)
		goto failed;

	    msleep(200);

	if ((ret = cycitv_gpio_set(d,(slot==0)?OUTCRSTA:OUTCRSTB,OUT_LOW)) < 0)
		goto failed;

	msleep(200);

	ret = cycitv_gpio_set(d,(slot==0)?OUTPASSA:OUTPASSB,OUT_HIGH);

failed:
	mutex_unlock(&state->ca_mutex);

	return ret;
}

static int cycitv_ci_slot_shutdown(struct dvb_ca_en50221 *ca, int slot)
{
	deb_ca("%s %d\n", __func__, slot);
	return 0;
}

static int cycitv_ci_slot_ts_enable(struct dvb_ca_en50221 *ca, int slot)
{
	struct dvb_usb_device *d = (struct dvb_usb_device *)ca->data;
	struct cycitv_state *state = (struct cycitv_state *)d->priv;
	int ret = 0;

	deb_ca("%s %d\n", __func__, slot);
	if (!d)
		return -ENODEV;

	if ((0 != slot)&&(1 != slot))
		return -EINVAL;

	mutex_lock(&state->ca_mutex);

	ret = cycitv_gpio_set(d,(slot==0)?OUTPASSA:OUTPASSB,OUT_LOW);

	mutex_unlock(&state->ca_mutex);

	return ret;
}
static int cycitv_ci_poll_slot_status(struct dvb_ca_en50221 *ca,
				int			slot,
				int			open)
{
	struct dvb_usb_device *d = (struct dvb_usb_device *)ca->data;
	struct cycitv_state *state = (struct cycitv_state *)d->priv;
	int ret;
	u8 gpin;

	deb_ca("%s %d\n", __func__, slot);
	if (!d)
		return -ENODEV;

	if ((0 != slot)&&(1 != slot))
		return -EINVAL;

	mutex_lock(&state->ca_mutex);

	 ret = cycitv_gpio_get(d,&gpin);

	mutex_unlock(&state->ca_mutex);

	if(ret >= 0) {
		if(slot==0) {
			if((gpin&(INCD2A|INCD1A)) == 0)
				return DVB_CA_EN50221_POLL_CAM_PRESENT |
					DVB_CA_EN50221_POLL_CAM_READY;
			} else if(slot==1) {
				if((gpin&(INCD2B|INCD1B)) == 0)
					return DVB_CA_EN50221_POLL_CAM_PRESENT |
						DVB_CA_EN50221_POLL_CAM_READY;
		} else return -EINVAL;
	}
	return 0;
}

static void cycitv_ci_uninit(struct dvb_usb_device *d)
{
	struct cycitv_state *state;

	deb_info("%s\n", __func__);

	if (NULL == d)
		return;

	state = (struct cycitv_state *)d->priv;
	if (NULL == state)
		return;

	if (NULL == state->ca.data)
		return;

	dvb_ca_en50221_release(&state->ca);

	memset(&state->ca, 0, sizeof(state->ca));

}

static int cycitv_ci_init(struct dvb_usb_adapter *a)
{
	struct dvb_usb_device *d = a->dev;
	struct cycitv_state *state = (struct cycitv_state *)d->priv;
	int ret;

	deb_info("%s\n", __func__);

	mutex_init(&state->ca_mutex);

	state->ca.owner = THIS_MODULE;
	state->ca.read_attribute_mem = cycitv_ci_read_attribute_mem;
	state->ca.write_attribute_mem = cycitv_ci_write_attribute_mem;
	state->ca.read_cam_control = cycitv_ci_read_cam_control;
	state->ca.write_cam_control = cycitv_ci_write_cam_control;
	state->ca.slot_reset = cycitv_ci_slot_reset;
	state->ca.slot_shutdown = cycitv_ci_slot_shutdown;
	state->ca.slot_ts_enable = cycitv_ci_slot_ts_enable;
	state->ca.poll_slot_status = cycitv_ci_poll_slot_status;
	state->ca.data = d;

	ret = dvb_ca_en50221_init(&a->dvb_adap,
				  &state->ca,
				  /* flags */ 0,
				  /* n_slots */ 2);
	if (0 != ret) {
		printk(KERN_ERR "Cannot initialize CI: Error %d.\n", ret);
		memset(&state->ca, 0, sizeof(state->ca));
		return ret;
	}

	info("CI initialized.\n");

	return 0;
}

static int cycitv_power_ctrl(struct dvb_usb_device *d, int i)
{
	struct cycitv_state *state = (struct cycitv_state *)d->priv;
	u8 obuf[] = {CMD_WAKEUP, 0};

	info("%s: %d, initialized %d\n", __func__, i, state->initialized);

	if (i && !state->initialized) {
		state->initialized = 1;
		/* reset board */
		dvb_usb_generic_rw(d, obuf, 2, NULL, 0, 0);
	}

	return 0;
}
static int cycitv_identify_state(struct usb_device *udev,
				struct dvb_usb_device_properties *props,
				struct dvb_usb_device_description **desc,
				int *cold)
{
	info("%s\n", __func__);

	*cold = 0;
	return 0;
}

static int cycitv_read_mac_address(struct dvb_usb_device *d, u8 mac[6])
{
	int i;
	u8 obuf[] = { 0x1f, 0xf0 };
	u8 ibuf[] = { 0 };
	struct i2c_msg msg[] = {
		{
			.addr = 0x51,
			.flags = 0,
			.buf = obuf,
			.len = 2,
		}, {
			.addr = 0x51,
			.flags = I2C_M_RD,
			.buf = ibuf,
			.len = 1,

		}
	};

	for (i = 0; i < 6; i++) {
		obuf[1] = 0xf0 + i;
		if (i2c_transfer(&d->i2c_adap, msg, 2) != 2)
			break;
		else
			mac[i] = ibuf[0];

		debug_dump(mac, 6, printk);
	}

	return 0;
}

static struct ds3k_config su3000_ds3k_config = {
	.demod_address = 0x68,
	.ci_mode = 1,
};

static u8 read_ds3000_ID(struct dvb_usb_adapter *adap)
{
	u8 b = 00;
	u8 reg;
	struct i2c_msg msg[] = { {.addr = 0x68,.flags = 0,.buf = &b,.len = 1},
	{.addr = 0x68,.flags = I2C_M_RD,.buf = &reg,.len = 1}
	};

	if (i2c_transfer(&adap->dev->i2c_adap, msg, 2) != 2)
		reg = 0x00;

	return reg;
}

static int cycitv_frontend_attach(struct dvb_usb_adapter *d)
{
	struct dvb_usb_device *dev = d->dev;
	struct cycitv_state *s = (struct cycitv_state *)dev->priv;
	struct cycitv_adapter_state *state = (struct cycitv_adapter_state *)d->priv;
	int id;

	deb_info("cycitv_frontend_attach 00!\n");

	cycitv_cam_power_ctrl(dev,1);
	s->gpio = OUTPASSA | OUTPASSB;
	//Enable LNB power
	if (cycitv_gpio_set(d->dev,OUTLNBEN,OUT_HIGH) < 0)
		    err("gpio set failed.\n");
	msleep(200);
	//reset tuner
	if (cycitv_gpio_set(d->dev,OUTTURST,OUT_LOW) < 0)
		    err("gpio set failed.\n");
	msleep(50);
	if (cycitv_gpio_set(d->dev,OUTTURST,OUT_HIGH) < 0)
		    err("gpio set failed.\n");
	msleep(200);
	//attach
	d->fe_adap[0].fe = NULL;
	id = read_ds3000_ID(d);
	if((id & 0xfe)==0xe0) {
		if ((d->fe_adap[0].fe = dvb_attach(ds3k_attach, &su3000_ds3k_config,
						    &d->dev->i2c_adap)) != NULL) {
			deb_info("found ds3k DVB-S/S2 frontend\n");
			cycitv_ci_init(d);
			 return 0;
		}
	}
	return -EIO;
}

static struct rc_map_table rc_map_cycitv_table_mygica[] = {
	{ 0x25, KEY_POWER },	/* right-bottom Red */
	{ 0x0a, KEY_MUTE },	/* -/-- */
	{ 0x01, KEY_1 },
	{ 0x02, KEY_2 },
	{ 0x03, KEY_3 },
	{ 0x04, KEY_4 },
	{ 0x05, KEY_5 },
	{ 0x06, KEY_6 },
	{ 0x07, KEY_7 },
	{ 0x08, KEY_8 },
	{ 0x09, KEY_9 },
	{ 0x00, KEY_0 },
	{ 0x20, KEY_UP },	/* CH+ */
	{ 0x21, KEY_DOWN },	/* CH+ */
	{ 0x12, KEY_VOLUMEUP },	/* Brightness Up */
	{ 0x13, KEY_VOLUMEDOWN },/* Brightness Down */
	{ 0x1f, KEY_RECORD },
	{ 0x17, KEY_PLAY },
	{ 0x16, KEY_PAUSE },
	{ 0x0b, KEY_STOP },
	{ 0x27, KEY_FASTFORWARD },/* >> */
	{ 0x26, KEY_REWIND },	/* << */
	{ 0x0d, KEY_OK },	/* Mute */
	{ 0x11, KEY_LEFT },	/* VOL- */
	{ 0x10, KEY_RIGHT },	/* VOL+ */
	{ 0x29, KEY_BACK },	/* button under 9 */
	{ 0x2c, KEY_MENU },	/* TTX */
	{ 0x2b, KEY_EPG },	/* EPG */
	{ 0x1e, KEY_RED },	/* OSD */
	{ 0x0e, KEY_GREEN },	/* Window */
	{ 0x2d, KEY_YELLOW },	/* button under << */
	{ 0x0f, KEY_BLUE },	/* bottom yellow button */
	{ 0x14, KEY_AUDIO },	/* Snapshot */
	{ 0x38, KEY_TV },	/* TV/Radio */
	{ 0x0c, KEY_ESC }	/* upper Red buttton */
};

static struct rc_map_dvb_usb_table_table keys_tables[] = {
	{ rc_map_cycitv_table_mygica, ARRAY_SIZE(rc_map_cycitv_table_mygica) },
};

static int cycitv_rc_query(struct dvb_usb_device *d, u32 *event, int *state)
{
	struct rc_map_table *keymap = d->props.rc.legacy.rc_map_table;
	int keymap_size = d->props.rc.legacy.rc_map_size;
	u8 key[2];
	int i;
	u8 obuf[0x40], ibuf[0x40];


	/* override keymap */
	if ((ir_keymap > 0) && (ir_keymap <= ARRAY_SIZE(keys_tables))) {
		keymap = keys_tables[ir_keymap - 1].rc_keys ;
		keymap_size = keys_tables[ir_keymap - 1].rc_keys_size;
	} else if (ir_keymap > ARRAY_SIZE(keys_tables))
		return 0; /* none */

	*state = REMOTE_NO_KEY_PRESSED;

	obuf[0] = CMD_GET_IR;
	if (dvb_usb_generic_rw(d, obuf, 1, ibuf, 2, 0) > 0) {
//	if (d->props.i2c_algo->master_xfer(&d->i2c_adap, &msg, 1) == 1) {
		for (i = 0; i < keymap_size ; i++) {
			if (rc5_data(&keymap[i]) == ibuf[1]) {
				*state = REMOTE_KEY_PRESSED;
				*event = keymap[i].keycode;
				break;
			}

		}

		if ((*state) == REMOTE_KEY_PRESSED)
			deb_rc("%s: found rc key: %x, %x, event: %x\n",
					__func__, key[0], key[1], (*event));
		else if (key[0] != 0xff)
			deb_rc("%s: unknown rc key: %x, %x\n",
					__func__, key[0], key[1]);

	}

	return 0;
}
enum cycitv_table_entry {
	GENIATECH_DEV_COLD,
	GENIATECH_DEV_WORK,
	GENIATECH_DEV_COLD_2,
	GENIATECH_DEV_WORK_2,
};

static struct usb_device_id cycitv_table[] = {
	[GENIATECH_DEV_COLD] = {USB_DEVICE(USB_VID_GENIATECH, USB_PID_CYCITV_COLD)},
	[GENIATECH_DEV_WORK] = {USB_DEVICE(USB_VID_GENIATECH, USB_PID_CYCITV_WORK)},
	[GENIATECH_DEV_COLD_2] = {USB_DEVICE(USB_VID_GENIATECH, USB_PID_CYCITV_COLD_2)},
	[GENIATECH_DEV_WORK_2] = {USB_DEVICE(USB_VID_GENIATECH, USB_PID_CYCITV_WORK_2)},
	{ }
};


MODULE_DEVICE_TABLE(usb, cycitv_table);

static struct dvb_usb_device_properties cycitv_properties = {
	.caps = DVB_USB_IS_AN_I2C_ADAPTER,
	.usb_ctrl = CYPRESS_FX2,
	.size_of_priv     = sizeof(struct cycitv_state),
	.power_ctrl = cycitv_power_ctrl,
	.num_adapters = 1,
	.identify_state	= cycitv_identify_state,
	.i2c_algo = &cycitv_i2c_algo,
	.no_reconnect = 1,
/*
	.rc.legacy = {
		.rc_map_table = rc_map_cycitv_table_mygica,
		.rc_map_size = ARRAY_SIZE(rc_map_cycitv_table_mygica),
		.rc_interval = 250,
		.rc_query = cycitv_rc_query,
	},
*/
	.read_mac_address = cycitv_read_mac_address,

	.generic_bulk_ctrl_endpoint = 0x01,

	.adapter = {
		{
		.num_frontends = 1,
		.size_of_priv    = sizeof(struct cycitv_adapter_state),
		.fe = {{
			.streaming_ctrl   = cycitv_streaming_ctrl,
			.frontend_attach = cycitv_frontend_attach,
			.stream = {
				.type = USB_BULK,
				.count = 8,
				.endpoint = 0x82,
				.u = {
					.bulk = {
						.buffersize = 4096,//yh mod 8192,
					}
				}
			},
		}},
		}
	},

	/* parameter for the MPEG2-data transfer */
	.num_device_descs = 4,
	.devices = {
		{"Geniatech TVbox with CI USB2.0",
			{&cycitv_table[GENIATECH_DEV_COLD], NULL},
			{NULL},
		},
		{"TerraTec Cinergy S USB",
			{&cycitv_table[GENIATECH_DEV_WORK], NULL},
			{NULL},
		},
		{"Geniatech TVbox with CI USB2.0 II",
			{&cycitv_table[GENIATECH_DEV_COLD_2], NULL},
			{NULL},
		},
		{"TerraTec Cinergy S USB II",
			{&cycitv_table[GENIATECH_DEV_WORK_2], NULL},
			{NULL},
		},
	}
};

static void cycitv_usb_disconnect(struct usb_interface *intf)
{
	struct dvb_usb_device *d = usb_get_intfdata(intf);

	cycitv_ci_uninit(d);
	dvb_usb_device_exit(intf);
}


static int cycitv_probe(struct usb_interface *intf,
		const struct usb_device_id *id)
{
	if (0 == dvb_usb_device_init(intf, &cycitv_properties,
			THIS_MODULE, NULL, adapter_nr))
		return 0;

	return -ENODEV;
}

static struct usb_driver cycitv_driver = {
	.name = "cycitv",
	.probe = cycitv_probe,
	.disconnect = cycitv_usb_disconnect,
	.id_table = cycitv_table,
};

module_usb_driver(cycitv_driver);

MODULE_AUTHOR("Geniatech");
MODULE_DESCRIPTION("Driver for CYCITV  DVB CI USB2.0");
MODULE_VERSION("1.0");
MODULE_LICENSE("GPL");
