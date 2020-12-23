/*********************************************************************

    swim.h

    Implementation of the Apple SWIM FDC controller; used on (less)
    early Macs

*********************************************************************/

#ifndef __SWIM_H__
#define __SWIM_H__

#include "machine/applefdc.h"


/***************************************************************************
    DEVICE
***************************************************************************/

extern const device_type SWIM;

class swim_device : public applefdc_base_device
{
public:
	swim_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// read/write
	virtual UINT8 read(UINT8 offset);
	virtual void write(UINT8 offset, UINT8 data);

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	// other overrides
	virtual void iwm_modereg_w(UINT8 data);

private:
	UINT8       m_swim_mode;
	UINT8       m_swim_magic_state;
	UINT8       m_parm_offset;
	UINT8       m_ism_regs[8];
	UINT8       m_parms[16];
};



/***************************************************************************
    DEVICE CONFIGURATION MACROS
***************************************************************************/

#define MCFG_SWIM_ADD(_tag, _intrf) \
	MCFG_DEVICE_ADD(_tag, SWIM, 0) \
	MCFG_DEVICE_CONFIG(_intrf)

#define MCFG_SWIM_MODIFY(_tag, _intrf) \
	MCFG_DEVICE_MODIFY(_tag)          \
	MCFG_DEVICE_CONFIG(_intrf)

#endif // __SWIM_H__
