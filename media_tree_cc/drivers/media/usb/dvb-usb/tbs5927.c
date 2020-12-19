/*
 * TurboSight TBS 5927 DVB-S2 driver
 *
 * Copyright (c) 2016 TBSDTV
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, version 2.
 *
 */

#include <linux/version.h>
#include "tbs5927.h"
#include "stv6120.h"
#include "stv091x.h"

#define TBS5927_READ_MSG 0
#define TBS5927_WRITE_MSG 1
#define TBS5927_LED_CTRL (0x1b00)

/* on my own*/
#define TBS5927_RC_QUERY (0x1a00)
#define TBS5927_VOLTAGE_CTRL (0x1800)

struct tbs5927_state {
	u8 initialized;
};

/* struct tbs5927_rc_keys {
	u32 keycode;
	u32 event;
}; */

/* debug */
static int dvb_usb_tbs5927_debug;
module_param_named(debug, dvb_usb_tbs5927_debug, int, 0644);
MODULE_PARM_DESC(debug, "set debugging level (1=info 2=xfer (or-able))."
							DVB_USB_DEBUG_STATUS);

DVB_DEFINE_MOD_OPT_ADAPTER_NR(adapter_nr);

static int tbs5927_op_rw(struct usb_device *dev, u8 request, u16 value,
			u16 index, u8 * data, u16 len, int flags)
{
	int ret;
	void *u8buf;

	unsigned int pipe = (flags == TBS5927_READ_MSG) ?
			usb_rcvctrlpipe(dev, 0) : usb_sndctrlpipe(dev, 0);
	u8 request_type = (flags == TBS5927_READ_MSG) ? USB_DIR_IN : USB_DIR_OUT;
	u8buf = kmalloc(len, GFP_KERNEL);
	if (!u8buf)
		return -ENOMEM;

	if (flags == TBS5927_WRITE_MSG)
		memcpy(u8buf, data, len);
	ret = usb_control_msg(dev, pipe, request, request_type | USB_TYPE_VENDOR,
				value, index , u8buf, len, 2000);

	if (flags == TBS5927_READ_MSG)
		memcpy(data, u8buf, len);
	kfree(u8buf);
	return ret;
}

/* I2C */
static int tbs5927_i2c_transfer(struct i2c_adapter *adap, struct i2c_msg msg[],
		int num)
{
struct dvb_usb_device *d = i2c_get_adapdata(adap);
	int i = 0;
	u8 buf6[32];
	u8 inbuf[32];

	if (!d)
		return -ENODEV;
	if (mutex_lock_interruptible(&d->i2c_mutex) < 0)
		return -EAGAIN;

	switch (num) {
	case 2:
		buf6[0]=msg[1].len;//lenth
		buf6[1]=msg[0].addr<<1;//demod addr
		//register
		memcpy(buf6+2,msg[0].buf,msg[0].len);

		tbs5927_op_rw(d->udev, msg[0].addr < 0x68 ? 0x90 : 0x92, 0, 0,
					buf6, msg[0].len+2, TBS5927_WRITE_MSG);
		//msleep(5);
		tbs5927_op_rw(d->udev, 0x91, 0, 0,
					inbuf, msg[1].len, TBS5927_READ_MSG);
		memcpy(msg[1].buf, inbuf, msg[1].len);
		break;
	case 1:
		switch (msg[0].addr) {
		case 0x68:
		case 0x61:
		case 0x60:
			if (msg[0].flags == 0) {
				buf6[0] = msg[0].len+1;//lenth
				buf6[1] = msg[0].addr<<1;//addr
				for(i=0;i<msg[0].len;i++) {
					buf6[2+i] = msg[0].buf[i];//register
				}
				tbs5927_op_rw(d->udev, 0x80, 0, 0,
							buf6, msg[0].len+2, TBS5927_WRITE_MSG);
			} else {
				buf6[0] = msg[0].len;//length
				buf6[1] = msg[0].addr<<1;//addr
				buf6[2] = 0x00;
				tbs5927_op_rw(d->udev, 0x90, 0, 0,
							buf6, 3, TBS5927_WRITE_MSG);
				//msleep(5);
				tbs5927_op_rw(d->udev, 0x91, 0, 0,
							inbuf, buf6[0], TBS5927_READ_MSG);
				memcpy(msg[0].buf, inbuf, msg[0].len);
			}
			//msleep(3);
			break;
		case (TBS5927_RC_QUERY):
			tbs5927_op_rw(d->udev, 0xb8, 0, 0,
					buf6, 4, TBS5927_READ_MSG);
			msg[0].buf[0] = buf6[2];
			msg[0].buf[1] = buf6[3];
			msleep(3);
			/* info("TBS5927_RC_QUERY %x %x %x %x\n",buf6[0],buf6[1],buf6[2],buf6[3]); */
			break;
		case (TBS5927_VOLTAGE_CTRL):
			buf6[0] = 1;
			buf6[1] = msg[0].buf[0] < 2;
			tbs5927_op_rw(d->udev, 0x8a, 0, 0,
					buf6, 2, TBS5927_WRITE_MSG);
			msleep(5);
			if (msg[0].buf[0] < 2) {
				buf6[0] = 3;
				buf6[1] = msg[0].buf[0];
				tbs5927_op_rw(d->udev, 0x8a, 0, 0,
					buf6, 2, TBS5927_WRITE_MSG);
			}
			break;
		case (TBS5927_LED_CTRL):
			buf6[0] = 5;
			buf6[1] = msg[0].buf[0];
			tbs5927_op_rw(d->udev, 0x8a, 0, 0,
					buf6, 2, TBS5927_WRITE_MSG);
			break;
		}

		break;
	}

	mutex_unlock(&d->i2c_mutex);
	return num;
}

static u32 tbs5927_i2c_func(struct i2c_adapter *adapter)
{
	return I2C_FUNC_I2C;
}

static void tbs5927_led_ctrl(struct dvb_frontend *fe, int offon)
{
	static u8 led_off[] = { 0 };
	static u8 led_on[] = { 1 };
	struct i2c_msg msg = {
		.addr = TBS5927_LED_CTRL,
		.flags = 0,
		.buf = led_off,
		.len = 1
	};
	struct dvb_usb_adapter *udev_adap =
		(struct dvb_usb_adapter *)(fe->dvb->priv);

	if (offon)
		msg.buf = led_on;
	
	/* info("tbs5927_led_ctrl %d", msg.buf[0]); */
	i2c_transfer(&udev_adap->dev->i2c_adap, &msg, 1);
}

static struct stv091x_cfg tbs5927_stv0910_cfg = {
	.adr      = 0x68,
	.parallel = 1,
	.rptlvl   = 3,
	.clk      = 30000000,
	.dual_tuner = 1,
	.set_lock_led = tbs5927_led_ctrl,
};

static struct stv6120_cfg tbs5927_stv6120_cfg = {
	.adr			= 0x60,
	.xtal			= 30000,
	.Rdiv			= 2,
};

static struct i2c_algorithm tbs5927_i2c_algo = {
	.master_xfer = tbs5927_i2c_transfer,
	.functionality = tbs5927_i2c_func,
};

static int tbs5927_read_mac_address(struct dvb_usb_device *d, u8 mac[6])
{
	int i,ret;
	u8 ibuf[3] = {0, 0,0};
	u8 eeprom[256], eepromline[16];

	for (i = 0; i < 256; i++) {
		ibuf[0]=1;//lenth
		ibuf[1]=0xa0;//eeprom addr
		ibuf[2]=i;//register
		ret = tbs5927_op_rw(d->udev, 0x90, 0, 0,
					ibuf, 3, TBS5927_WRITE_MSG);
		ret = tbs5927_op_rw(d->udev, 0x91, 0, 0,
					ibuf, 1, TBS5927_READ_MSG);
			if (ret < 0) {
				err("read eeprom failed.");
				return -1;
			} else {
				eepromline[i%16] = ibuf[0];
				eeprom[i] = ibuf[0];
			}
			
			if ((i % 16) == 15) {
				deb_xfer("%02x: ", i - 15);
				debug_dump(eepromline, 16, deb_xfer);
			}
	}
	memcpy(mac, eeprom + 16, 6);
	return 0;
};

static int tbs5927_set_voltage(struct dvb_frontend *fe, 
						enum fe_sec_voltage voltage)
{
	static u8 cmd[1] = {0x00};
 
	struct i2c_msg msg[] = {
		{.addr = TBS5927_VOLTAGE_CTRL, .flags = 0,
			.buf = cmd, .len = 1},
	};
	
	struct dvb_usb_adapter *udev_adap =
		(struct dvb_usb_adapter *)(fe->dvb->priv);
	switch (voltage) {
	  case SEC_VOLTAGE_13:
		cmd[0] = 0;
		break;
	  case SEC_VOLTAGE_18:
		cmd[0] = 1;
		break;
	  case SEC_VOLTAGE_OFF:
		cmd[0] = 2;
		break;
	}

	/*info("tbs5927_set_voltage %d", voltage);*/
	i2c_transfer(&udev_adap->dev->i2c_adap, msg, 1);
	return 0;
}

static int tbs5927_tuner_attach(struct dvb_usb_adapter *adap)
{
	if (!dvb_attach(stv6120_attach, adap->fe_adap->fe,
		      &adap->dev->i2c_adap, &tbs5927_stv6120_cfg, 0))
		return -EIO;

	return 0;
}

static struct dvb_usb_device_properties tbs5927_properties;

static int tbs5927_frontend_attach(struct dvb_usb_adapter *d)
{
	u8 buf[20];

	buf[0] = 6; /* I2C speed */
	buf[1] = 0; /* 100KHz */
	tbs5927_op_rw(d->dev->udev, 0x8a, 0, 0,
			buf, 2, TBS5927_WRITE_MSG);

	if (tbs5927_properties.adapter->fe->tuner_attach == &tbs5927_tuner_attach) {
		d->fe_adap->fe = dvb_attach(stv091x_attach, &d->dev->i2c_adap, 
					    &tbs5927_stv0910_cfg, 1 );
		if (d->fe_adap->fe != NULL) {
			d->fe_adap->fe->ops.set_voltage = tbs5927_set_voltage;

			buf[0] = 1; /* LNB power disable */
			buf[1] = 0;
			tbs5927_op_rw(d->dev->udev, 0x8a, 0, 0,
					buf, 2, TBS5927_WRITE_MSG);

			buf[0] = 7; /* IR RC enable */
			buf[1] = 1;
			tbs5927_op_rw(d->dev->udev, 0x8a, 0, 0,
					buf, 2, TBS5927_WRITE_MSG);

			return 0;
		}
	}

	return -EIO;
}

static int tbs5927_rc_query(struct dvb_usb_device *d)
{
	u8 key[2];
	struct i2c_msg msg = {
		.addr = TBS5927_RC_QUERY,
		.flags = I2C_M_RD,
		.buf = key,
		.len = 2
	};

	if (d->props.i2c_algo->master_xfer(&d->i2c_adap, &msg, 1) == 1) {
		if (key[1] != 0xff) {
			deb_xfer("RC code: 0x%02X !\n", key[1]);
			rc_keydown(d->rc_dev, RC_PROTO_UNKNOWN, key[1],
				   0);
		}
	}

	return 0;
}

static struct usb_device_id tbs5927_table[] = {
	{USB_DEVICE(0x734c, 0x5927)},
	{ }
};

MODULE_DEVICE_TABLE(usb, tbs5927_table);

static int tbs5927_load_firmware(struct usb_device *dev,
			const struct firmware *frmwr)
{
	u8 *b, *p;
	int ret = 0, i;
	u8 reset;
	const struct firmware *fw;
	switch (dev->descriptor.idProduct) {
	case 0x5927:
		ret = request_firmware(&fw, tbs5927_properties.firmware, &dev->dev);
		if (ret != 0) {
			err("did not find the firmware file. (%s) "
			"Please see linux/Documentation/dvb/ for more details "
			"on firmware-problems.", tbs5927_properties.firmware);
			return ret;
		}
		break;
	default:
		fw = frmwr;
		break;
	}
	info("start downloading TBS5927 firmware");
	p = kmalloc(fw->size, GFP_KERNEL);
	reset = 1;
	/*stop the CPU*/
	tbs5927_op_rw(dev, 0xa0, 0x7f92, 0, &reset, 1, TBS5927_WRITE_MSG);
	tbs5927_op_rw(dev, 0xa0, 0xe600, 0, &reset, 1, TBS5927_WRITE_MSG);

	if (p != NULL) {
		memcpy(p, fw->data, fw->size);
		for (i = 0; i < fw->size; i += 0x40) {
			b = (u8 *) p + i;
			if (tbs5927_op_rw(dev, 0xa0, i, 0, b , 0x40,
					TBS5927_WRITE_MSG) != 0x40) {
				err("error while transferring firmware");
				ret = -EINVAL;
				break;
			}
		}
		/* restart the CPU */
		reset = 0;
		if (ret || tbs5927_op_rw(dev, 0xa0, 0x7f92, 0, &reset, 1,
					TBS5927_WRITE_MSG) != 1) {
			err("could not restart the USB controller CPU.");
			ret = -EINVAL;
		}
		if (ret || tbs5927_op_rw(dev, 0xa0, 0xe600, 0, &reset, 1,
					TBS5927_WRITE_MSG) != 1) {
			err("could not restart the USB controller CPU.");
			ret = -EINVAL;
		}

		msleep(100);
		kfree(p);
	}
	return ret;
}

static struct dvb_usb_device_properties tbs5927_properties = {
	.caps = DVB_USB_IS_AN_I2C_ADAPTER,
	.usb_ctrl = DEVICE_SPECIFIC,
	.firmware = "dvb-usb-tbsqbox-id5927.fw",
	.size_of_priv = sizeof(struct tbs5927_state),
	.no_reconnect = 1,

	.i2c_algo = &tbs5927_i2c_algo,
	.rc.core = {
		.rc_interval = 150,
		.rc_codes = RC_MAP_TBS_NEC,
		.module_name = KBUILD_MODNAME,
		.allowed_protos   = RC_PROTO_BIT_NEC,
		.rc_query = tbs5927_rc_query,
	},

	.generic_bulk_ctrl_endpoint = 0x81,
	/* parameter for the MPEG2-data transfer */
	.num_adapters = 1,
	.download_firmware = tbs5927_load_firmware,
	.read_mac_address = tbs5927_read_mac_address,
	.adapter = {{
		.num_frontends = 1,
		.fe = {{
			.frontend_attach = tbs5927_frontend_attach,
			.streaming_ctrl = NULL,
			.tuner_attach = tbs5927_tuner_attach,
			.stream = {
				.type = USB_BULK,
				.count = 8,
				.endpoint = 0x82,
				.u = {
					.bulk = {
						.buffersize = 4096,
					}
				}
			},
		} },
	} },

	.num_device_descs = 1,
	.devices = {
		{"TBS 5927 DVB-S2 USB2.0",
			{&tbs5927_table[0], NULL},
			{NULL},
		}
	}
};

static int tbs5927_probe(struct usb_interface *intf,
		const struct usb_device_id *id)
{
	if (0 == dvb_usb_device_init(intf, &tbs5927_properties,
			THIS_MODULE, NULL, adapter_nr)) {
		return 0;
	}
	return -ENODEV;
}

static struct usb_driver tbs5927_driver = {
	.name = "tbs5927",
	.probe = tbs5927_probe,
	.disconnect = dvb_usb_device_exit,
	.id_table = tbs5927_table,
};

static int __init tbs5927_module_init(void)
{
	int ret =  usb_register(&tbs5927_driver);
	if (ret)
		err("usb_register failed. Error number %d", ret);

	return ret;
}

static void __exit tbs5927_module_exit(void)
{
	usb_deregister(&tbs5927_driver);
}

module_init(tbs5927_module_init);
module_exit(tbs5927_module_exit);

MODULE_AUTHOR("TBSDTV");
MODULE_DESCRIPTION("TurboSight TBS 5927 DVB-S2 driver");
MODULE_VERSION("1.0");
MODULE_LICENSE("GPL");
