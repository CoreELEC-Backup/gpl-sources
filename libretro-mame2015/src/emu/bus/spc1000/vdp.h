#ifndef __SPC1000_VDP_H__
#define __SPC1000_VDP_H__

#include "exp.h"
#include "video/tms9928a.h"


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> spc1000_vdp_exp_device

class spc1000_vdp_exp_device : public device_t,
						public device_spc1000_card_interface
{
public:
	// construction/destruction
	spc1000_vdp_exp_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const;

public:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	virtual DECLARE_READ8_MEMBER(read);
	virtual DECLARE_WRITE8_MEMBER(write);

	DECLARE_WRITE_LINE_MEMBER(vdp_interrupt);

private:
	// internal state
	required_device<tms9928a_device>   m_vdp;
};


// device type definition
extern const device_type SPC1000_VDP_EXP;

#endif  /* __SPC1000_VDP_H__ */
