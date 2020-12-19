/*
    TBS ECP3 FPGA based cards PCIe driver

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "tbsecp3.h"

#include "tas2101.h"
#include "av201x.h"

#include "si2168.h"
#include "si2157.h"

#include "mxl58x.h"

#include "si2183.h"

#include "stv091x.h"
#include "stv6120.h"

#include "stid135.h"

DVB_DEFINE_MOD_OPT_ADAPTER_NR(adapter_nr);

struct sec_priv {
	struct tbsecp3_adapter *adap;
	int (*set_voltage)(struct dvb_frontend *fe,
			   enum fe_sec_voltage voltage);
};

static int tbsecp3_set_voltage(struct dvb_frontend* fe,
		enum fe_sec_voltage voltage)
{
	struct sec_priv *priv = fe->sec_priv;
	struct tbsecp3_gpio_config *cfg = &priv->adap->cfg->gpio;
	struct tbsecp3_dev *dev = priv->adap->dev;

	dev_dbg(&dev->pci_dev->dev, "%s() %s\n", __func__,
		voltage == SEC_VOLTAGE_13 ? "SEC_VOLTAGE_13" :
		voltage == SEC_VOLTAGE_18 ? "SEC_VOLTAGE_18" :
		"SEC_VOLTAGE_OFF");

	switch (voltage) {
		case SEC_VOLTAGE_13:
			tbsecp3_gpio_set_pin(dev, &cfg->lnb_power, 1);
			tbsecp3_gpio_set_pin(dev, &cfg->lnb_voltage, 0);
			break;
		case SEC_VOLTAGE_18:
			tbsecp3_gpio_set_pin(dev, &cfg->lnb_power, 1);
			tbsecp3_gpio_set_pin(dev, &cfg->lnb_voltage, 1);
			break;
		default: /* OFF */
			tbsecp3_gpio_set_pin(dev, &cfg->lnb_power, 0);
			break;
	}

	if (priv->set_voltage)
		return priv->set_voltage(fe, voltage);
	else
		return 0;
}

static void tbsecp3_release_sec(struct dvb_frontend* fe)
{
	struct sec_priv *priv;

	if (fe == NULL)
		return;

	priv = fe->sec_priv;
	if (priv == NULL)
		return;

	fe->ops.set_voltage = priv->set_voltage;
	fe->sec_priv = NULL;
	kfree(priv);
}

static struct dvb_frontend *tbsecp3_attach_sec(struct tbsecp3_adapter *adap, struct dvb_frontend *fe)
{
	struct sec_priv *priv;

	priv = kzalloc(sizeof(struct sec_priv), GFP_KERNEL);
	if (!priv)
		return NULL;

	priv->set_voltage = fe->ops.set_voltage;
	priv->adap = adap;

//	fe->ops.release_sec = tbsecp3_release_sec;
	fe->ops.set_voltage = tbsecp3_set_voltage;
	fe->sec_priv = priv;

	return fe;
}

static int set_mac_address(struct tbsecp3_adapter *adap)
{
	struct tbsecp3_dev *dev = adap->dev;
	u8 eeprom_bus_nr = dev->info->eeprom_i2c;
	struct i2c_adapter *i2c = &dev->i2c_bus[eeprom_bus_nr].i2c_adap;
	u8 eep_addr;
	int ret;

	struct i2c_msg msg[] = {
		{ .addr = 0x50, .flags = 0,
		  .buf = &eep_addr, .len = 1 },
		{ .addr = 0x50, .flags = I2C_M_RD,
		  .buf = adap->dvb_adapter.proposed_mac, .len = 6 }
	};

	if (dev->info->eeprom_addr)
		eep_addr = dev->info->eeprom_addr;
	else
		eep_addr = 0xa0;
	eep_addr += 0x10 * adap->nr;
	ret = i2c_transfer(i2c, msg, 2);
	if (ret != 2) {
		dev_warn(&dev->pci_dev->dev,
			"error reading MAC address for adapter %d\n",
			adap->nr);
	} else {
		/* Set fake MAC when EEPROM not set */
		if (adap->dvb_adapter.proposed_mac[0] == 0xff && adap->dvb_adapter.proposed_mac[1] == 0xff && adap->dvb_adapter.proposed_mac[2] == 0xff &&
		    adap->dvb_adapter.proposed_mac[3] == 0xff && adap->dvb_adapter.proposed_mac[4] == 0xff && adap->dvb_adapter.proposed_mac[5] == 0xff) {
			adap->dvb_adapter.proposed_mac[0] = 0;
			adap->dvb_adapter.proposed_mac[1] = 0x22;
			adap->dvb_adapter.proposed_mac[2] = 0xAB;
			adap->dvb_adapter.proposed_mac[3] = 0x69;
			adap->dvb_adapter.proposed_mac[4] = 0x69;
			adap->dvb_adapter.proposed_mac[5] = dev->mac_num++;
		}
		dev_info(&dev->pci_dev->dev,
			"MAC address %pM\n", adap->dvb_adapter.proposed_mac);
	}
	
	return 0;
};

static int start_feed(struct dvb_demux_feed *dvbdmxfeed)
{
	struct dvb_demux *dvbdmx = dvbdmxfeed->demux;
	struct tbsecp3_adapter *adapter = dvbdmx->priv;

	if (!adapter->feeds)
		tbsecp3_dma_enable(adapter);

	return ++adapter->feeds;
}

static int stop_feed(struct dvb_demux_feed *dvbdmxfeed)
{
	struct dvb_demux *dvbdmx = dvbdmxfeed->demux;
	struct tbsecp3_adapter *adapter = dvbdmx->priv;

	if (--adapter->feeds)
		return adapter->feeds;

	tbsecp3_dma_disable(adapter);
	return 0;
}

static void reset_demod(struct tbsecp3_adapter *adapter)
{
	struct tbsecp3_dev *dev = adapter->dev;
	struct tbsecp3_gpio_pin *reset = &adapter->cfg->gpio.demod_reset;

	tbsecp3_gpio_set_pin(dev, reset, 1);
	usleep_range(10000, 20000);

	tbsecp3_gpio_set_pin(dev, reset, 0);
	usleep_range(50000, 100000);
}


static struct tas2101_config tbs6902_demod_cfg[] = {
	{
		.i2c_address   = 0x60,
		.id            = ID_TAS2101,
		.init          = {0xb0, 0x32, 0x81, 0x57, 0x64, 0x9a, 0x33},
		.init2         = 0,
	},
	{
		.i2c_address   = 0x68,
		.id            = ID_TAS2101,
		.init          = {0xb0, 0x32, 0x81, 0x57, 0x64, 0x9a, 0x33}, // 0xb1
		.init2         = 0,
	}
};

static struct av201x_config tbs6902_av201x_cfg = {
		.i2c_address = 0x62,
		.id 		 = ID_AV2012,
		.xtal_freq	 = 27000,		/* kHz */
};

static struct tas2101_config tbs6904_demod_cfg[] = {
	{
		.i2c_address   = 0x60,
		.id            = ID_TAS2101,
		.init          = {0xb0, 0x32, 0x81, 0x57, 0x64, 0x9a, 0x33}, // 0xb1
		.init2         = 0,
	},
	{
		.i2c_address   = 0x68,
		.id            = ID_TAS2101,
		.init          = {0xb0, 0x32, 0x81, 0x57, 0x64, 0x9a, 0x33},
		.init2         = 0,
	},
	{
		.i2c_address   = 0x60,
		.id            = ID_TAS2101,
		.init          = {0xb0, 0x32, 0x81, 0x57, 0x64, 0x9a, 0x33},
		.init2         = 0,
	},
	{
		.i2c_address   = 0x68,
		.id            = ID_TAS2101,
		.init          = {0xb0, 0x32, 0x81, 0x57, 0x64, 0x9a, 0x33},
		.init2         = 0,
	}
};

static struct av201x_config tbs6904_av201x_cfg = {
	.i2c_address = 0x63,
	.id          = ID_AV2012,
	.xtal_freq   = 27000,		/* kHz */
};


static struct tas2101_config tbs6910_demod_cfg[] = {
	{
		.i2c_address   = 0x68,
		.id            = ID_TAS2101,
		.init          = {0x21, 0x43, 0x65, 0xb0, 0xa8, 0x97, 0xb1},
		.init2         = 0,
	},
	{
		.i2c_address   = 0x60,
		.id            = ID_TAS2101,
		.init          = {0xb0, 0xa8, 0x21, 0x43, 0x65, 0x97, 0xb1},
		.init2         = 0,
	},
};

static struct av201x_config tbs6910_av201x_cfg = {
	.i2c_address = 0x62,
	.id          = ID_AV2018,
	.xtal_freq   = 27000,		/* kHz */
};

static int max_set_voltage(struct i2c_adapter *i2c,
		enum fe_sec_voltage voltage, u8 rf_in)
{
	struct tbsecp3_i2c *i2c_adap = i2c_get_adapdata(i2c);
	struct tbsecp3_dev *dev = i2c_adap->dev;

	u32 val, reg;

	//printk("set voltage on %u = %d\n", rf_in, voltage);
	
	if (rf_in > 3)
		return -EINVAL;

	reg = rf_in * 4;
	val = tbs_read(TBSECP3_GPIO_BASE, reg) & ~4;

	switch (voltage) {
	case SEC_VOLTAGE_13:
		val &= ~2;
		break;
	case SEC_VOLTAGE_18:
		val |= 2;
		break;
	case SEC_VOLTAGE_OFF:
	default:
		val |= 4;
		break;
	}

	tbs_write(TBSECP3_GPIO_BASE, reg, val);
	return 0;
}

static struct mxl58x_cfg tbs6909_mxl58x_cfg = {
	.adr		= 0x60,
	.type		= 0x01,
	.clk		= 24000000,
	.cap		= 12,
	.set_voltage	= max_set_voltage,
};

static struct stv091x_cfg tbs6903_stv0910_cfg = {
	.adr      = 0x68,
	.parallel = 1,
	.rptlvl   = 3,
	.clk      = 30000000,
	.dual_tuner = 1,
};

static struct stv6120_cfg tbs6903_stv6120_cfg = {
	.adr			= 0x60,
	.xtal			= 30000,
	.Rdiv			= 2,
};

static struct av201x_config tbs6522_av201x_cfg[] = {
	{
		.i2c_address = 0x63,
		.id          = ID_AV2018,
		.xtal_freq   = 27000,
	},
	{
		.i2c_address = 0x62,
		.id          = ID_AV2018,
		.xtal_freq   = 27000,
	},
};

static struct stid135_cfg tbs6903x_stid135_cfg = {
	.adr		= 0x68,
	.clk		= 27,
	.ts_mode	= TS_2PAR,
	.set_voltage	= NULL,
};

static struct stid135_cfg tbs6909x_stid135_cfg = {
	.adr		= 0x68,
	.clk		= 27,
	.ts_mode	= TS_STFE,
	.set_voltage	= max_set_voltage,
};

static void RF_switch(struct i2c_adapter *i2c,u8 rf_in,u8 flag)//flag : 0: dvbs/s2 signal 1:Terrestrial and cable signal 
{
	struct tbsecp3_i2c *i2c_adap = i2c_get_adapdata(i2c);
	struct tbsecp3_dev *dev = i2c_adap->dev;
	u32 val ,reg;

	if (flag)
		tbsecp3_gpio_set_pin(dev, &dev->adapter[rf_in].cfg->gpio.lnb_power, 0);

	reg = 0x8 + rf_in*4;
	
	val = tbs_read(TBSECP3_GPIO_BASE, reg);
	if(flag)
		val |= 2;
	else
		val &= ~2;
		
	tbs_write(TBSECP3_GPIO_BASE, reg, val);
}

static int tbsecp3_frontend_attach(struct tbsecp3_adapter *adapter)
{
	struct tbsecp3_dev *dev = adapter->dev;

	struct si2168_config si2168_config;
	struct si2183_config si2183_config;
	struct si2157_config si2157_config;

	struct i2c_board_info info;
	struct i2c_adapter *i2c = &adapter->i2c->i2c_adap;

	adapter->fe = NULL;
	adapter->fe2 = NULL;
	adapter->i2c_client_demod = NULL;
	adapter->i2c_client_tuner = NULL;

	reset_demod(adapter);

	set_mac_address(adapter);

	switch (dev->info->board_id) {
	case TBSECP3_BOARD_TBS6205:
	case TBSECP3_BOARD_TBS6281SE:
		/* attach demod */
		memset(&si2168_config, 0, sizeof(si2168_config));
		si2168_config.i2c_adapter = &i2c;
		si2168_config.fe = &adapter->fe;
		si2168_config.ts_mode = SI2168_TS_PARALLEL;
		si2168_config.ts_clock_gapped = true;

		adapter->i2c_client_demod = dvb_module_probe("si2168", NULL,
						   i2c, 0x64, &si2168_config);
		if (!adapter->i2c_client_demod)
			goto frontend_atach_fail;

		/* attach tuner */
		memset(&si2157_config, 0, sizeof(si2157_config));
		si2157_config.fe = adapter->fe;
		si2157_config.if_port = 1;

		adapter->i2c_client_tuner = dvb_module_probe("si2157", "si2157",
							  i2c, 0x60, &si2157_config);
		if (!adapter->i2c_client_tuner)
			goto frontend_atach_fail;
		break;

	case TBSECP3_BOARD_TBS6290SE:
		/* attach demod */
		memset(&si2168_config, 0, sizeof(si2168_config));
		si2168_config.i2c_adapter = &i2c;
		si2168_config.fe = &adapter->fe;
		si2168_config.ts_mode = SI2168_TS_SERIAL;//zc2016/07/20
		si2168_config.ts_clock_gapped = true;
		//si2168_config.ts_clock_inv=1;//zc2016/07/20

		adapter->i2c_client_demod = dvb_module_probe("si2168", NULL,
						   i2c, 0x64, &si2168_config);
		if (!adapter->i2c_client_demod)
			goto frontend_atach_fail;

		/* attach tuner */
		memset(&si2157_config, 0, sizeof(si2157_config));
		si2157_config.fe = adapter->fe;
		si2157_config.if_port = 1;

		adapter->i2c_client_tuner = dvb_module_probe("si2157", "si2157",
							  i2c, 0x60, &si2157_config);
		if (!adapter->i2c_client_tuner)
			goto frontend_atach_fail;

		tbsecp3_ca_init(adapter, adapter->nr);
		break;

	case TBSECP3_BOARD_TBS6209:
		/* attach demod */
		memset(&si2183_config, 0, sizeof(si2183_config));
		si2183_config.i2c_adapter = &i2c;
		si2183_config.fe = &adapter->fe;
		si2183_config.ts_mode = SI2183_TS_SERIAL;
		si2183_config.ts_clock_gapped = true;
		si2183_config.fef_pin = (adapter->nr%2) ? SI2183_MP_B : SI2183_MP_A;
		si2183_config.fef_inv = 0;
		si2183_config.agc_pin = (adapter->nr%2) ? SI2183_MP_D : SI2183_MP_C;
		si2183_config.ter_agc_inv = 0;
		si2183_config.sat_agc_inv = 1;
		si2183_config.rf_in = adapter->nr;
		si2183_config.RF_switch = NULL;

		adapter->i2c_client_demod = dvb_module_probe("si2183", NULL,
						   i2c, (adapter->nr%2)? 0x67 : 0x64, &si2183_config);
		if (!adapter->i2c_client_demod)
			goto frontend_atach_fail;

		/* terrestrial tuner */
		memset(adapter->fe->ops.delsys, 0, MAX_DELSYS);
		adapter->fe->ops.delsys[0] = SYS_DVBT;
		adapter->fe->ops.delsys[1] = SYS_DVBT2;
		adapter->fe->ops.delsys[2] = SYS_DVBC_ANNEX_A;
		adapter->fe->ops.delsys[3] = SYS_ISDBT;
		adapter->fe->ops.delsys[4] = SYS_DVBC_ANNEX_B;

		/* attach tuner */
		memset(&si2157_config, 0, sizeof(si2157_config));
		si2157_config.fe = adapter->fe;
		si2157_config.if_port = 1;

		adapter->i2c_client_tuner = dvb_module_probe("si2157", "si2157",
							  i2c, (adapter->nr %2)? 0x60 : 0x63, &si2157_config);
		if (!adapter->i2c_client_tuner)
			goto frontend_atach_fail;
		break;

	case TBSECP3_BOARD_TBS6522:
		/* attach demod */
		memset(&si2183_config, 0, sizeof(si2183_config));
		si2183_config.i2c_adapter = &i2c;
		si2183_config.fe = &adapter->fe;
		si2183_config.ts_mode = SI2183_TS_PARALLEL;
		si2183_config.ts_clock_gapped = true;
		si2183_config.fef_pin = adapter->nr ? SI2183_MP_A : SI2183_MP_B;
		si2183_config.fef_inv = 0;
		si2183_config.agc_pin = adapter->nr ? SI2183_MP_C : SI2183_MP_D;
		si2183_config.ter_agc_inv = 0;
		si2183_config.sat_agc_inv = 1;
		si2183_config.rf_in = adapter->nr;
		si2183_config.RF_switch = NULL;

		adapter->i2c_client_demod = dvb_module_probe("si2183", NULL,
						   i2c, adapter->nr ? 0x64 : 0x67, &si2183_config);
		if (!adapter->i2c_client_demod)
			goto frontend_atach_fail;

		/* dvb core doesn't support 2 tuners for 1 demod so
		   we split the adapter in 2 frontends */
		adapter->fe2 = &adapter->_fe2;
		memcpy(adapter->fe2, adapter->fe, sizeof(struct dvb_frontend));

		/* sattelite tuner */
		memset(adapter->fe->ops.delsys, 0, MAX_DELSYS);
		adapter->fe->ops.delsys[0] = SYS_DVBS;
		adapter->fe->ops.delsys[1] = SYS_DVBS2;
		adapter->fe->ops.delsys[2] = SYS_DSS;
		
		if (dvb_attach(av201x_attach, adapter->fe, &tbs6522_av201x_cfg[adapter->nr],
				i2c) == NULL) {
			dev_err(&dev->pci_dev->dev,
				"frontend %d tuner attach failed\n",
				adapter->nr);
			goto frontend_atach_fail;
		}
		if (tbsecp3_attach_sec(adapter, adapter->fe) == NULL) {
			dev_warn(&dev->pci_dev->dev,
				"error attaching lnb control on adapter %d\n",
				adapter->nr);
		}

		/* terrestrial tuner */
		memset(adapter->fe2->ops.delsys, 0, MAX_DELSYS);
		adapter->fe2->ops.delsys[0] = SYS_DVBT;
		adapter->fe2->ops.delsys[1] = SYS_DVBT2;
		adapter->fe2->ops.delsys[2] = SYS_ISDBT;
		adapter->fe2->ops.delsys[3] = SYS_DVBC_ANNEX_A;
		adapter->fe2->ops.delsys[4] = SYS_DVBC_ANNEX_B;
		adapter->fe2->id = 1;

		/* attach tuner */
		memset(&si2157_config, 0, sizeof(si2157_config));
		si2157_config.fe = adapter->fe2;
		si2157_config.if_port = 1;

		adapter->i2c_client_tuner = dvb_module_probe("si2157", "si2157",
							  i2c, adapter->nr ? 0x61 : 0x60, &si2157_config);
		if (!adapter->i2c_client_tuner)
			goto frontend_atach_fail;
		break;

	case TBSECP3_BOARD_TBS6528:
	case TBSECP3_BOARD_TBS6590:
		/* attach demod */
		memset(&si2183_config, 0, sizeof(si2183_config));
		si2183_config.i2c_adapter = &i2c;
		si2183_config.fe = &adapter->fe;
		si2183_config.ts_mode = dev->info->board_id == TBSECP3_BOARD_TBS6528 ? SI2183_TS_PARALLEL : SI2183_TS_SERIAL;
		si2183_config.ts_clock_gapped = true;
		si2183_config.rf_in = adapter->nr;
		si2183_config.RF_switch = RF_switch;

		if(dev->info->board_id == TBSECP3_BOARD_TBS6528) {
			info.addr = 0x67;
			si2183_config.fef_pin = SI2183_MP_B;
			si2183_config.fef_inv = 0;
			si2183_config.agc_pin = SI2183_MP_D;
			si2183_config.ter_agc_inv = 0;
			si2183_config.sat_agc_inv = 1;
			adapter->i2c_client_demod = dvb_module_probe("si2183", NULL,
							  i2c, 0x67, &si2183_config);
		}
		else {
			si2183_config.fef_pin = adapter->nr ? SI2183_MP_B : SI2183_MP_A;
			si2183_config.fef_inv = 0;
			si2183_config.agc_pin = adapter->nr ? SI2183_MP_D : SI2183_MP_C;
			si2183_config.ter_agc_inv = 0;
			si2183_config.sat_agc_inv = 1;
			adapter->i2c_client_demod = dvb_module_probe("si2183", NULL,
							  i2c, adapter->nr ? 0x67 : 0x64, &si2183_config);
		}

		if (!adapter->i2c_client_demod)
			goto frontend_atach_fail;

		/* dvb core doesn't support 2 tuners for 1 demod so
		   we split the adapter in 2 frontends */
		adapter->fe2 = &adapter->_fe2;
		memcpy(adapter->fe2, adapter->fe, sizeof(struct dvb_frontend));

		/* sattelite tuner */
		memset(adapter->fe->ops.delsys, 0, MAX_DELSYS);
		adapter->fe->ops.delsys[0] = SYS_DVBS;
		adapter->fe->ops.delsys[1] = SYS_DVBS2;
		adapter->fe->ops.delsys[2] = SYS_DSS;

		if (dev->info->board_id == TBSECP3_BOARD_TBS6528) {
			if (dvb_attach(av201x_attach, adapter->fe, &tbs6522_av201x_cfg[1],
					i2c) == NULL) {
				dev_err(&dev->pci_dev->dev,
				"frontend %d tuner attach failed\n",
				adapter->nr);
				goto frontend_atach_fail;
			}
		} else if (dvb_attach(av201x_attach, adapter->fe, &tbs6522_av201x_cfg[adapter->nr],
					i2c) == NULL) {
				dev_err(&dev->pci_dev->dev,
					"frontend %d tuner attach failed\n",
					adapter->nr);
				goto frontend_atach_fail;
		}
		if (tbsecp3_attach_sec(adapter, adapter->fe) == NULL) {
			dev_warn(&dev->pci_dev->dev,
				"error attaching lnb control on adapter %d\n",
				adapter->nr);
		}

		/* terrestrial tuner */
		memset(adapter->fe2->ops.delsys, 0, MAX_DELSYS);
		adapter->fe2->ops.delsys[0] = SYS_DVBT;
		adapter->fe2->ops.delsys[1] = SYS_DVBT2;
		adapter->fe2->ops.delsys[2] = SYS_DVBC_ANNEX_A;
		adapter->fe2->ops.delsys[3] = SYS_ISDBT;
		adapter->fe2->ops.delsys[4] = SYS_DVBC_ANNEX_B;
		adapter->fe2->id = 1;

		/* attach tuner */
		memset(&si2157_config, 0, sizeof(si2157_config));
		si2157_config.fe = adapter->fe2;
		si2157_config.if_port = 1;

		if (dev->info->board_id == TBSECP3_BOARD_TBS6528)
			adapter->i2c_client_tuner = dvb_module_probe("si2157", "si2157",
								  i2c, 0x61, &si2157_config);
		else
			adapter->i2c_client_tuner = dvb_module_probe("si2157", "si2157",
								  i2c, adapter->nr ? 0x61 : 0x60, &si2157_config);

		if (!adapter->i2c_client_tuner)
			goto frontend_atach_fail;

		tbsecp3_ca_init(adapter, adapter->nr);
		break;

	case TBSECP3_BOARD_TBS6902:
		adapter->fe = dvb_attach(tas2101_attach, &tbs6902_demod_cfg[adapter->nr], i2c);
		if (adapter->fe == NULL)
			goto frontend_atach_fail;

		if (dvb_attach(av201x_attach, adapter->fe, &tbs6902_av201x_cfg,
				tas2101_get_i2c_adapter(adapter->fe, 2)) == NULL) {
			dvb_frontend_detach(adapter->fe);
			adapter->fe = NULL;
			dev_err(&dev->pci_dev->dev,
				"TBS_PCIE frontend %d tuner attach failed\n",
				adapter->nr);
			goto frontend_atach_fail;
		}

		if (tbsecp3_attach_sec(adapter, adapter->fe) == NULL) {
			dev_warn(&dev->pci_dev->dev,
				"error attaching lnb control on adapter %d\n",
				adapter->nr);
		}
		break;

	case TBSECP3_BOARD_TBS6903:
	case TBSECP3_BOARD_TBS6905:
	case TBSECP3_BOARD_TBS6908:
		adapter->fe = dvb_attach(stv091x_attach, i2c,
				&tbs6903_stv0910_cfg, adapter->nr & 1);
		if (adapter->fe == NULL)
			goto frontend_atach_fail;

		if (dvb_attach(stv6120_attach, adapter->fe, i2c,
				&tbs6903_stv6120_cfg,1-(adapter->nr & 1)) == NULL) {
			dvb_frontend_detach(adapter->fe);
			adapter->fe = NULL;
			dev_err(&dev->pci_dev->dev,
				"TBS_PCIE frontend %d tuner attach failed\n",
				adapter->nr);
			goto frontend_atach_fail;
		}

		if (tbsecp3_attach_sec(adapter, adapter->fe) == NULL) {
			dev_warn(&dev->pci_dev->dev,
				"error attaching lnb control on adapter %d\n",
				adapter->nr);
		}

		break;

	case TBSECP3_BOARD_TBS6904:
		adapter->fe = dvb_attach(tas2101_attach, &tbs6904_demod_cfg[adapter->nr], i2c);
		if (adapter->fe == NULL)
			goto frontend_atach_fail;

		if (dvb_attach(av201x_attach, adapter->fe, &tbs6904_av201x_cfg,
				tas2101_get_i2c_adapter(adapter->fe, 2)) == NULL) {
			dvb_frontend_detach(adapter->fe);
			adapter->fe = NULL;
			dev_err(&dev->pci_dev->dev,
				"TBS_PCIE frontend %d tuner attach failed\n",
				adapter->nr);
			goto frontend_atach_fail;
		}

		if (tbsecp3_attach_sec(adapter, adapter->fe) == NULL) {
			dev_warn(&dev->pci_dev->dev,
				"error attaching lnb control on adapter %d\n",
				adapter->nr);
		}

		break;

	case TBSECP3_BOARD_TBS6909:
		adapter->fe = dvb_attach(mxl58x_attach, i2c,
				&tbs6909_mxl58x_cfg, adapter->nr);
		if (adapter->fe == NULL)
			goto frontend_atach_fail;

		break;

	case TBSECP3_BOARD_TBS6910:
		adapter->fe = dvb_attach(tas2101_attach, &tbs6910_demod_cfg[adapter->nr], i2c);
		if (adapter->fe == NULL)
			goto frontend_atach_fail;

		if (dvb_attach(av201x_attach, adapter->fe, &tbs6910_av201x_cfg,
				tas2101_get_i2c_adapter(adapter->fe, 2)) == NULL) {
			dvb_frontend_detach(adapter->fe);
			adapter->fe = NULL;
			dev_err(&dev->pci_dev->dev,
				"TBS_PCIE frontend %d tuner attach failed\n",
				adapter->nr);
			goto frontend_atach_fail;
		}
		if (tbsecp3_attach_sec(adapter, adapter->fe) == NULL) {
			dev_warn(&dev->pci_dev->dev,
				"error attaching lnb control on adapter %d\n",
				adapter->nr);
		}

		tbsecp3_ca_init(adapter, adapter->nr);
		break;

	case TBSECP3_BOARD_TBS6909X:
		adapter->fe = dvb_attach(stid135_attach, i2c,
				&tbs6909x_stid135_cfg, adapter->nr, adapter->nr/2);
		if (adapter->fe == NULL)
			goto frontend_atach_fail;

		break;

	case TBSECP3_BOARD_TBS6903X:
		adapter->fe = dvb_attach(stid135_attach, i2c,
				&tbs6903x_stid135_cfg, adapter->nr ? 2 : 0, adapter->nr ? 3 : 0);
		if (adapter->fe == NULL)
			goto frontend_atach_fail;

		if (tbsecp3_attach_sec(adapter, adapter->fe) == NULL) {
			dev_warn(&dev->pci_dev->dev,
				"error attaching lnb control on adapter %d\n",
				adapter->nr);
		}

		break;
	case TBSECP3_BOARD_TBS6904X:
		memset(&si2183_config, 0, sizeof(si2183_config));
		si2183_config.i2c_adapter = &i2c;
		si2183_config.fe = &adapter->fe;
		si2183_config.ts_mode =  SI2183_TS_PARALLEL ;
		si2183_config.ts_clock_gapped = true;
		si2183_config.start_clk_mode = 1;
		si2183_config.agc_pin = (adapter->nr%2) ? SI2183_MP_C : SI2183_MP_D;
		si2183_config.sat_agc_inv = 1;
		si2183_config.rf_in = adapter->nr;
		si2183_config.RF_switch = NULL;
	
		adapter->i2c_client_demod = dvb_module_probe("si2183", NULL,
						   i2c, (adapter->nr %2)? 0x64 : 0x67, &si2183_config);
		if (!adapter->i2c_client_demod)
			goto frontend_atach_fail;

		memset(adapter->fe->ops.delsys, 0, MAX_DELSYS);
		adapter->fe->ops.delsys[0] = SYS_DVBS;
		adapter->fe->ops.delsys[1] = SYS_DVBS2;
		adapter->fe->ops.delsys[2] = SYS_DSS;

		if (dvb_attach(av201x_attach, adapter->fe, &tbs6522_av201x_cfg[(adapter->nr%2)],
			    i2c) == NULL) {
		    dvb_frontend_detach(adapter->fe);
		    adapter->fe = NULL;
		    dev_err(&dev->pci_dev->dev,
			    "frontend %d tuner attach failed\n",
			    adapter->nr);
		    goto frontend_atach_fail;
		}
		if (tbsecp3_attach_sec(adapter, adapter->fe) == NULL) {
		    dev_warn(&dev->pci_dev->dev,
			    "error attaching lnb control on adapter %d\n",
			    adapter->nr);
		}
		break;
	default:
		dev_warn(&dev->pci_dev->dev, "unknonw card\n");
		return -ENODEV;
		break;
	}

	return 0;

frontend_atach_fail:
	tbsecp3_i2c_remove_clients(adapter);
	if (adapter->fe != NULL)
		dvb_frontend_detach(adapter->fe);
	adapter->fe = NULL;
	dev_err(&dev->pci_dev->dev, "TBSECP3 frontend %d attach failed\n",
		adapter->nr);

	return -ENODEV;
}

int tbsecp3_dvb_init(struct tbsecp3_adapter *adapter)
{
	struct tbsecp3_dev *dev = adapter->dev;
	struct dvb_adapter *adap = &adapter->dvb_adapter;
	struct dvb_demux *dvbdemux = &adapter->demux;
	struct dmxdev *dmxdev;
	struct dmx_frontend *fe_hw;
	struct dmx_frontend *fe_mem;
	int ret;

	ret = dvb_register_adapter(adap, "TBSECP3 DVB Adapter",
					THIS_MODULE,
					&adapter->dev->pci_dev->dev,
					adapter_nr);
	if (ret < 0) {
		dev_err(&dev->pci_dev->dev, "error registering adapter\n");
		if (ret == -ENFILE)
			dev_err(&dev->pci_dev->dev,
				"increase DVB_MAX_ADAPTERS (%d)\n",
				DVB_MAX_ADAPTERS);
		return ret;
	}

	adap->priv = adapter;
	dvbdemux->priv = adapter;
	dvbdemux->filternum = 256;
	dvbdemux->feednum = 256;
	dvbdemux->start_feed = start_feed;
	dvbdemux->stop_feed = stop_feed;
	dvbdemux->write_to_decoder = NULL;
	dvbdemux->dmx.capabilities = (DMX_TS_FILTERING |
				      DMX_SECTION_FILTERING |
				      DMX_MEMORY_BASED_FILTERING);

	ret = dvb_dmx_init(dvbdemux);
	if (ret < 0) {
		dev_err(&dev->pci_dev->dev, "dvb_dmx_init failed\n");
		goto err0;
	}

	dmxdev = &adapter->dmxdev;

	dmxdev->filternum = 256;
	dmxdev->demux = &dvbdemux->dmx;
	dmxdev->capabilities = 0;

	ret = dvb_dmxdev_init(dmxdev, adap);
	if (ret < 0) {
		dev_err(&dev->pci_dev->dev, "dvb_dmxdev_init failed\n");
		goto err1;
	}

	fe_hw = &adapter->fe_hw;
	fe_mem = &adapter->fe_mem;

	fe_hw->source = DMX_FRONTEND_0;
	ret = dvbdemux->dmx.add_frontend(&dvbdemux->dmx, fe_hw);
	if ( ret < 0) {
		dev_err(&dev->pci_dev->dev, "dvb_dmx_init failed");
		goto err2;
	}

	fe_mem->source = DMX_MEMORY_FE;
	ret = dvbdemux->dmx.add_frontend(&dvbdemux->dmx, fe_mem);
	if (ret  < 0) {
		dev_err(&dev->pci_dev->dev, "dvb_dmx_init failed");
		goto err3;
	}

	ret = dvbdemux->dmx.connect_frontend(&dvbdemux->dmx, fe_hw);
	if (ret < 0) {
		dev_err(&dev->pci_dev->dev, "dvb_dmx_init failed");
		goto err4;
	}

	ret = dvb_net_init(adap, &adapter->dvbnet, adapter->dmxdev.demux);
	if (ret < 0) {
		dev_err(&dev->pci_dev->dev, "dvb_net_init failed");
		goto err5;
	}

	tbsecp3_frontend_attach(adapter);
	if (adapter->fe == NULL) {
		dev_err(&dev->pci_dev->dev, "frontend attach failed\n");
		ret = -ENODEV;
		goto err6;
	}

	ret = dvb_register_frontend(adap, adapter->fe);
	if (ret < 0) {
		dev_err(&dev->pci_dev->dev, "frontend register failed\n");
		goto err7;
	}

	if (adapter->fe2 != NULL) {
		ret = dvb_register_frontend(adap, adapter->fe2);
		if (ret < 0) {
			dev_err(&dev->pci_dev->dev, "frontend2 register failed\n");
		}
	}


	return ret;

err7:
	dvb_frontend_detach(adapter->fe);
err6:
	tbsecp3_release_sec(adapter->fe);

	dvb_net_release(&adapter->dvbnet);
err5:
	dvbdemux->dmx.close(&dvbdemux->dmx);
err4:
	dvbdemux->dmx.remove_frontend(&dvbdemux->dmx, fe_mem);
err3:
	dvbdemux->dmx.remove_frontend(&dvbdemux->dmx, fe_hw);
err2:
	dvb_dmxdev_release(dmxdev);
err1:
	dvb_dmx_release(dvbdemux);
err0:
	dvb_unregister_adapter(adap);
	return ret;
}

void tbsecp3_dvb_exit(struct tbsecp3_adapter *adapter)
{
	struct dvb_adapter *adap = &adapter->dvb_adapter;
	struct dvb_demux *dvbdemux = &adapter->demux;

	if (adapter->fe) {
		tbsecp3_ca_release(adapter);
		dvb_unregister_frontend(adapter->fe);
		tbsecp3_release_sec(adapter->fe);
		dvb_frontend_detach(adapter->fe);
		adapter->fe = NULL;

		if (adapter->fe2 != NULL) {
			dvb_unregister_frontend(adapter->fe2);
			tbsecp3_release_sec(adapter->fe2);
			dvb_frontend_detach(adapter->fe2);
			adapter->fe2 = NULL;
		}
	}
	dvb_net_release(&adapter->dvbnet);
	dvbdemux->dmx.close(&dvbdemux->dmx);
	dvbdemux->dmx.remove_frontend(&dvbdemux->dmx, &adapter->fe_mem);
	dvbdemux->dmx.remove_frontend(&dvbdemux->dmx, &adapter->fe_hw);
	dvb_dmxdev_release(&adapter->dmxdev);
	dvb_dmx_release(&adapter->demux);
	dvb_unregister_adapter(adap);
}
