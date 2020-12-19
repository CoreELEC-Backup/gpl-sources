/*
 * Leaguer MicroElectronics LME2510 driver
 *
 * Copyright (C) 2016 Antti Palosaari <crope@iki.fi>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 */

#include "dvb_usb.h"
#include "si2168.h"
#include "si2157.h"
#include <linux/i2c.h>

DVB_DEFINE_MOD_OPT_ADAPTER_NR(adapter_nr);

#define LME2510_FIRMWARE "dvb-usb-lme2510c-0.fw"

struct lme2510_dev {
	/* Bulk USB control message buffer */
	u8 buf[64];
	struct si2168_config si2168_config;
	struct si2157_config si2157_config;
	struct i2c_client *i2c_client_demod;
	struct i2c_client *i2c_client_tuner;
	enum fe_status fe_status;
	int (*fe_read_status)(struct dvb_frontend *fe, enum fe_status *status);
	int (*fe_set_frontend)(struct dvb_frontend *fe);
};

static int lme2510_ctrl_msg(struct dvb_usb_device *d, const u8 *wbuf,
			    unsigned int wlen, u8 *rbuf, unsigned int rlen)
{
	struct lme2510_dev *dev = d_to_priv(d);
	struct usb_interface *intf = d->intf;
	int ret, actual_length;

	if (wlen > sizeof(dev->buf) || rlen > sizeof(dev->buf)) {
		ret = -EINVAL;
		goto err;
	}

	mutex_lock(&d->usb_mutex);
	memcpy(&dev->buf, wbuf, wlen);
	dev_dbg(&intf->dev, ">>> %*ph\n", wlen, dev->buf);
	ret = usb_bulk_msg(d->udev, usb_sndbulkpipe(d->udev, 0x01),
			   dev->buf, wlen, &actual_length, 1000);
	if (ret) {
		dev_err(&intf->dev, "snd usb_bulk_msg() failed %d\n", ret);
		goto err_mutex_unlock;
	}

	ret = usb_bulk_msg(d->udev, usb_rcvbulkpipe(d->udev, 0x81),
			   dev->buf, rlen, &actual_length, 1000);
	if (ret) {
		dev_err(&intf->dev, "rcv usb_bulk_msg() failed %d\n", ret);
		goto err_mutex_unlock;
	}
	dev_dbg(&intf->dev, "<<< %*ph\n", actual_length, dev->buf);
	memcpy(rbuf, dev->buf, rlen);
	mutex_unlock(&d->usb_mutex);

	return 0;
err_mutex_unlock:
	mutex_unlock(&d->usb_mutex);
err:
	dev_dbg(&intf->dev, "failed=%d\n", ret);
	return ret;
}

static int lme2510_i2c_master_xfer(struct i2c_adapter *adapter,
				   struct i2c_msg msg[], int num)
{
	struct dvb_usb_device *d = i2c_get_adapdata(adapter);
	struct lme2510_dev *dev = d_to_priv(d);
	struct usb_interface *intf = d->intf;
	int ret;
	u8 wbuf[32], rbuf[32];

#define LME2510_IS_I2C_XFER_WRITE(_msg, _num) (_num == 1 && !(_msg[0].flags & I2C_M_RD))
#define LME2510_IS_I2C_XFER_READ(_msg, _num) (_num == 1 && (_msg[0].flags & I2C_M_RD))

	if (dev->fe_status & FE_HAS_LOCK) {
		/* I2C is not allowed while streaming */
		ret = -EOPNOTSUPP;
		goto err;
	} else if (LME2510_IS_I2C_XFER_WRITE(msg, num)) {
		if (msg[0].len > 24) {
			ret = -EOPNOTSUPP;
			goto err;
		}

		wbuf[0] = 0x04;
		wbuf[1] = 1 + msg[0].len;
		wbuf[2] = msg[0].addr << 1;
		memcpy(&wbuf[3], &msg[0].buf[0], msg[0].len);
		ret = lme2510_ctrl_msg(d, wbuf, 3 + msg[0].len, rbuf, 1);
		if (ret)
			goto err;
		if (rbuf[0] != 0x88) {
			ret = -EIO;
			goto err;
		}
	} else if (LME2510_IS_I2C_XFER_READ(msg, num)) {
		if (msg[0].len > 24) {
			ret = -EOPNOTSUPP;
			goto err;
		}

		wbuf[0] = 0x86;
		wbuf[1] = 1 + msg[0].len;
		wbuf[2] = msg[0].addr << 1;
		wbuf[3] = msg[0].len;
		ret = lme2510_ctrl_msg(d, wbuf, 4, rbuf, 1 + msg[0].len);
		if (ret)
			goto err;
		if (rbuf[0] != 0x55) {
			ret = -EIO;
			goto err;
		}
		memcpy(&msg[0].buf[0], &rbuf[1], msg[0].len);
	} else {
		dev_dbg(&intf->dev, "unknown msg[0].len=%u\n", msg[0].len);
		ret = -EOPNOTSUPP;
		if (ret)
			goto err;
	}

	return num;
err:
	dev_dbg(&intf->dev, "failed=%d\n", ret);
	return ret;
}

static u32 lme2510_i2c_functionality(struct i2c_adapter *adapter)
{
	return I2C_FUNC_I2C;
}

static struct i2c_algorithm lme2510_i2c_algo = {
	.master_xfer   = lme2510_i2c_master_xfer,
	.functionality = lme2510_i2c_functionality,
};

static int lme2510_identify_state(struct dvb_usb_device *d, const char **name)
{
	struct usb_interface *intf = d->intf;
	int ret;
	u8 buf[8];

	dev_dbg(&intf->dev, "\n");

	ret = usb_set_interface(d->udev, 0, 1);
	if (ret)
		goto err;

	ret = usb_get_descriptor(d->udev, USB_DT_STRING, 0x02, buf, 8);
	if (ret < 0)
		goto err;

	if (!memcmp(&buf[2], "\x44\x45\x46\x47", 4))
		return COLD;

	return WARM;
err:
	dev_dbg(&intf->dev, "failed=%d\n", ret);
	return ret;
}

static int lme2510_download_firmware(struct dvb_usb_device *d,
				     const struct firmware *firmware)
{
	struct usb_interface *intf = d->intf;
	int ret, len, rem, i;
	u8 wbuf[64], rbuf[1];

	dev_dbg(&intf->dev, "\n");

	if (firmware->size <= 512) {
		ret = -EINVAL;
		dev_err(&intf->dev, "invalid firmware\n");
		goto err;
	}

	#define LME2510_MAX_LEN (64 - 3)
	/* Block1 - USB descriptors */
	for (rem = 512; rem > 0; rem -= LME2510_MAX_LEN) {
		len = min(LME2510_MAX_LEN, rem);
		if (rem <= LME2510_MAX_LEN)
			wbuf[0] = 0x81;
		else
			wbuf[0] = 0x01;
		wbuf[1] = len - 1;
		memcpy(&wbuf[2], &firmware->data[512 - rem], len);
		/* Checksum */
		for (wbuf[len + 2] = 0, i = 0; i < len; i++)
			wbuf[len + 2] += wbuf[2 + i];

		ret = lme2510_ctrl_msg(d, wbuf, 3 + len, rbuf, 1);
		if (ret)
			goto err;
	}

	/* Block2 - firmware */
	for (rem = firmware->size - 512; rem > 0; rem -= LME2510_MAX_LEN) {
		len = min(LME2510_MAX_LEN, rem);
		if (rem <= LME2510_MAX_LEN)
			wbuf[0] = 0x82;
		else
			wbuf[0] = 0x02;
		wbuf[1] = len - 1;
		memcpy(&wbuf[2], &firmware->data[firmware->size - rem], len);
		/* Checksum */
		for (wbuf[len + 2] = 0, i = 0; i < len; i++)
			wbuf[len + 2] += wbuf[2 + i];

		ret = lme2510_ctrl_msg(d, wbuf, 3 + len, rbuf, 1);
		if (ret)
			goto err;
	}

	/* Reboot firmware */
	ret = lme2510_ctrl_msg(d, "\x8a\x00", 2, rbuf, 1);
	if (ret)
		goto err;

	return RECONNECTS_USB;
err:
	dev_dbg(&intf->dev, "failed=%d\n", ret);
	return ret;
}

/*
 * Chip has limitation that it stops streaming after any I2C command.
 * Due to that, we cache demod status here.
 */
static int lme2510_fe_read_status(struct dvb_frontend *fe,
				   enum fe_status *status)
{
	struct dvb_usb_device *d = fe_to_d(fe);
	struct lme2510_dev *dev = d_to_priv(d);
	struct usb_interface *intf = d->intf;
	int ret;

	dev_dbg(&intf->dev, "\n");

	if (dev->fe_status & FE_HAS_LOCK) {
		*status = FE_HAS_SIGNAL | FE_HAS_CARRIER | FE_HAS_VITERBI |
			  FE_HAS_SYNC | FE_HAS_LOCK;
	} else {
		ret = dev->fe_read_status(fe, status);
		if (ret)
			return ret;
		dev->fe_status = *status;

		if (dev->fe_status & FE_HAS_LOCK) {
			u8 rbuf[1];

			dev_dbg(&intf->dev, "start streaming\n");
			ret = lme2510_ctrl_msg(d, "\x06\x00", 2, rbuf, 1);
			if (ret)
				goto err;
		}
	}

	return 0;
err:
	dev_dbg(&intf->dev, "failed=%d\n", ret);
	return ret;
}

static int lme2510_fe_set_frontend(struct dvb_frontend *fe)
{
	struct dvb_usb_device *d = fe_to_d(fe);
	struct lme2510_dev *dev = d_to_priv(d);
	struct usb_interface *intf = d->intf;

	dev_dbg(&intf->dev, "\n");

	dev->fe_status = 0;

	return dev->fe_set_frontend(fe);
}

static int lme2510_streaming_ctrl(struct dvb_frontend *fe, int onoff)
{
	struct dvb_usb_device *d = fe_to_d(fe);
	struct lme2510_dev *dev = d_to_priv(d);
	struct usb_interface *intf = d->intf;

	dev_dbg(&intf->dev, "onoff=%d\n", onoff);

	dev->fe_status = 0;

	return 0;
}

static int lme2510_frontend_attach(struct dvb_usb_adapter *adap)
{
	struct dvb_usb_device *d = adap_to_d(adap);
	struct lme2510_dev *dev = adap_to_priv(adap);
	struct usb_interface *intf = d->intf;
	struct i2c_client *client_demod, *client_tuner;
	struct i2c_board_info board_info;
	struct i2c_adapter *adapter;
	struct dvb_frontend *fe;
	int ret;

	dev_dbg(&intf->dev, "\n");

	/* Add I2C demod */
	dev->si2168_config.i2c_adapter = &adapter;
	dev->si2168_config.fe = &fe;
	dev->si2168_config.ts_mode = SI2168_TS_PARALLEL;
	dev->si2168_config.ts_clock_inv = false;
	dev->si2168_config.ts_clock_gapped = false;
	memset(&board_info, 0, sizeof(struct i2c_board_info));
	strlcpy(board_info.type, "si2168", I2C_NAME_SIZE);
	board_info.addr = 0x64;
	board_info.platform_data = &dev->si2168_config;
	request_module("%s", "si2168");
	client_demod = i2c_new_device(&d->i2c_adap, &board_info);
	if (!client_demod || !client_demod->dev.driver) {
		ret = -ENODEV;
		goto err;
	}
	if (!try_module_get(client_demod->dev.driver->owner)) {
		ret = -ENODEV;
		goto err_i2c_unregister_device_demod;
	}

	/* Add I2C tuner */
	dev->si2157_config.fe = fe;
	dev->si2157_config.if_port = 1;
	dev->si2157_config.inversion = false;
	memset(&board_info, 0, sizeof(struct i2c_board_info));
	strlcpy(board_info.type, "si2157", I2C_NAME_SIZE);
	board_info.addr = 0x60;
	board_info.platform_data = &dev->si2157_config;
	request_module("%s", "si2157");
	client_tuner = i2c_new_device(adapter, &board_info);
	if (!client_tuner || !client_tuner->dev.driver) {
		ret = -ENODEV;
		goto err_module_put_demod;
	}
	if (!try_module_get(client_tuner->dev.driver->owner)) {
		ret = -ENODEV;
		goto err_i2c_unregister_device_tuner;
	}

	dev->i2c_client_demod = client_demod;
	dev->i2c_client_tuner = client_tuner;
	adap->fe[0] = fe;

	dev->fe_read_status = adap->fe[0]->ops.read_status;
	adap->fe[0]->ops.read_status = lme2510_fe_read_status;
	dev->fe_set_frontend = adap->fe[0]->ops.set_frontend;
	adap->fe[0]->ops.set_frontend = lme2510_fe_set_frontend;

	return 0;
err_i2c_unregister_device_tuner:
	i2c_unregister_device(client_tuner);
err_module_put_demod:
	module_put(client_demod->dev.driver->owner);
err_i2c_unregister_device_demod:
	i2c_unregister_device(client_demod);
err:
	dev_dbg(&intf->dev, "failed=%d\n", ret);
	return ret;
}

static int lme2510_frontend_detach(struct dvb_usb_adapter *adap)
{
	struct dvb_usb_device *d = adap_to_d(adap);
	struct lme2510_dev *dev = d_to_priv(d);
	struct usb_interface *intf = d->intf;
	struct i2c_client *client;

	dev_dbg(&intf->dev, "\n");

	/* Remove I2C tuner */
	client = dev->i2c_client_tuner;
	if (client) {
		module_put(client->dev.driver->owner);
		i2c_unregister_device(client);
	}

	/* Remove I2C demod */
	client = dev->i2c_client_demod;
	if (client) {
		module_put(client->dev.driver->owner);
		i2c_unregister_device(client);
	}

	return 0;
}

static const struct dvb_usb_device_properties lme2510_props = {
	.driver_name = KBUILD_MODNAME,
	.owner = THIS_MODULE,
	.adapter_nr = adapter_nr,
	.size_of_priv = sizeof(struct lme2510_dev),

	.identify_state    = lme2510_identify_state,
	.firmware          = LME2510_FIRMWARE,
	.download_firmware = lme2510_download_firmware,
	.i2c_algo          = &lme2510_i2c_algo,

	.frontend_attach = lme2510_frontend_attach,
	.frontend_detach = lme2510_frontend_detach,
	.streaming_ctrl  = lme2510_streaming_ctrl,

	.num_adapters = 1,
	.adapter = {
		{
			.stream = DVB_USB_STREAM_BULK(0x88, 5, 8 * 512),
		},
	},
};

#define USB_VID_LEAGUERME 0x3344
static const struct usb_device_id lme2510_id_table[] = {
	{DVB_USB_DEVICE(USB_VID_LEAGUERME, 0x24a0, &lme2510_props,
	 "Sin Hon TDH601", NULL)},
	{}
};
MODULE_DEVICE_TABLE(usb, lme2510_id_table);

/* Usb specific object needed to register this driver with the usb subsystem */
static struct usb_driver lme2510_usb_driver = {
	.name = KBUILD_MODNAME,
	.id_table = lme2510_id_table,
	.probe = dvb_usbv2_probe,
	.disconnect = dvb_usbv2_disconnect,
	.suspend = dvb_usbv2_suspend,
	.resume = dvb_usbv2_resume,
	.reset_resume = dvb_usbv2_reset_resume,
	.no_dynamic_id = 1,
	.soft_unbind = 1,
};
module_usb_driver(lme2510_usb_driver);

MODULE_AUTHOR("Antti Palosaari <crope@iki.fi>");
MODULE_DESCRIPTION("Leaguer MicroElectronics LME2510 driver");
MODULE_LICENSE("GPL");
MODULE_FIRMWARE(LME2510_FIRMWARE);
