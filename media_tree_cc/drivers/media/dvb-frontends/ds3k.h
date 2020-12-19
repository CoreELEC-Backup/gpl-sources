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

#ifndef DS3K_H
#define DS3K_H

#include <linux/kconfig.h>
#include <linux/dvb/frontend.h>

/* AC coupling control*/
#define MT_FE_ENABLE_AC_COUPLING            1        /*1: AC coupling (recommended in reference design) 0: DC coupling*/

#define MT_FE_MCLK_KHZ_SERIAL_S2            144000	//96000


#define MT_FE_ENHANCE_TS_PIN_LEVEL_PARALLEL_CI    0    // Parallel Mode or Common Interface Mode
#define MT_FE_ENHANCE_TS_PIN_LEVEL_SERIAL         1    // Serial Mode

/* CLOCK OUTPUT TO DECODER*/
#define MT_FE_ENABLE_27MHZ_CLOCK_OUT        0
#define MT_FE_ENABLE_13_P_5_MHZ_CLOCK_OUT   0

/******************** LNB and DISEQC DEFINES ************************/
/*  Maybe user need change the defines according to reference design  */
#define LNB_ENABLE_WHEN_LNB_EN_HIGH         0
#define LNB_13V_WHEN_VSEL_HIGH              1
#define LNB_VSEL_STANDBY_HIGH               1
#define LNB_DISEQC_OUT_FORCE_HIGH           0
#define LNB_DISEQC_OUT_ONLY_OUTPUT          0

/******************** GPIO DEFINES **************************************/
#define ENABLE_OLF_AS_GPIO                  0
#define GPIO_OLF_AS_INPUT                   0
#define ENABLE_MERR_AS_GPIO                 0
#define GPIO_MERR_AS_INPUT                  0
#define ENABLE_ITLOCK_AS_GPIO               0
#define GPIO_ITLOCK_AS_INPUT                0
#define ENABLE_CLKOUT_AS_GPIO               0
#define GPIO_CLKOUT_AS_INPUT                0


#define MtFeTsOutMode_Serial        1
#define MtFeTsOutMode_Parallel      2
#define MtFeTsOutMode_Common        3

#define MT_FE_TS_OUTPUT_MODE        MtFeTsOutMode_Common



#define MT_FE_TS_PIN_ORDER_D0_D7        0        // 0: D0, 1: D7

#define MT_FE_TS_CLOCK_AUTO_SET_FOR_SERIAL_MODE 1

#if(MT_FE_TS_OUTPUT_MODE == MtFeTsOutMode_Parallel)
/***************************************************************
In parallel mode, user can select the max clock out frequency
according to the decoder's max clock frequency.

Four Options:
MtFeTSOut_Max_Clock_12_MHz;         MtFeTSOut_Max_Clock_16_MHz
MtFeTSOut_Max_Clock_19_p_2_MHz;     MtFeTSOut_Max_Clock_24_MHz;

Default setting is 24MHz.
***************************************************************/
#define MtFeTSOut_Max_Clock_12_MHz          0
#define MtFeTSOut_Max_Clock_16_MHz          0
#define MtFeTSOut_Max_Clock_19_p_2_MHz      0
#define MtFeTSOut_Max_Clock_24_MHz          0
#define MT_FE_TS_CLOCK_AUTO_SET_FOR_CI_MODE 1

#elif (MT_FE_TS_OUTPUT_MODE == MtFeTsOutMode_Common)
/***********************  TS Clock Auto Set  ************************
** MT_FE_TS_CLOCK_AUTO_SET_FOR_CI_MODE == 1  Automatically set TS clock
**
**       TS clock will be automatically set just according to the
** modulation mode/code rate/symbol rate after TP locked.
*********************************************************************/
#define MT_FE_TS_CLOCK_AUTO_SET_FOR_CI_MODE 1
#endif

struct ds3k_config {
	/* the demodulator's i2c address */
	u8 demod_address;
	u8 ci_mode;
	u8 output_mode;
	/* Set device param to start dma */
	int (*set_ts_params)(struct dvb_frontend *fe, int is_punctured);
	/* Hook for Lock LED */
	void (*set_lock_led)(struct dvb_frontend *fe, int offon);
};

#if IS_REACHABLE(CONFIG_DVB_DS3K)
extern struct dvb_frontend *ds3k_attach(const struct ds3k_config *config,
					struct i2c_adapter *i2c);
#else
static inline
struct dvb_frontend *ds3k_attach(const struct ds3k_config *config,
					struct i2c_adapter *i2c)
{
	printk(KERN_WARNING "%s: driver disabled by Kconfig\n", __func__);
	return NULL;
}
#endif /* CONFIG_DVB_DS3K */
#endif /* DS3K_H */
