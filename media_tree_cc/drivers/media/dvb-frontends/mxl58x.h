#ifndef _MXL58X_H_
#define _MXL58X_H_

#include <linux/types.h>
#include <linux/i2c.h>

#include <media/dvb_frontend.h>

struct mxl58x_cfg {
	u8   adr;
	u8   type;
	u32  cap;
	u32  clk;

	int (*set_voltage)(struct i2c_adapter *i2c,
		enum fe_sec_voltage voltage, u8 rf_in);
};


#if IS_REACHABLE(CONFIG_DVB_MXL5XX)
extern struct dvb_frontend *mxl58x_attach(struct i2c_adapter *i2c,
					  struct mxl58x_cfg *cfg,
					  u32 demod);
#else

static inline struct dvb_frontend *mxl58x_attach(struct i2c_adapter *i2c,
						 struct mxl58x_cfg *cfg,
						 u32 demod)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
	return NULL;
}

#endif

#endif
