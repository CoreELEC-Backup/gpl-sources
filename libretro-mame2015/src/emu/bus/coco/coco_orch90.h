#pragma once

#ifndef __COCO_ORCH90_H__
#define __COCO_ORCH90_H__

#include "emu.h"
#include "sound/dac.h"
#include "cococart.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> coco_orch90_device

class coco_orch90_device :
		public device_t,
		public device_cococart_interface
{
public:
		// construction/destruction
		coco_orch90_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

		// optional information overrides
		virtual machine_config_constructor device_mconfig_additions() const;
protected:
		// device-level overrides
		virtual void device_start();
		virtual DECLARE_WRITE8_MEMBER(write);
private:
		// internal state
		dac_device *m_left_dac;
		dac_device *m_right_dac;
};


// device type definition
extern const device_type COCO_ORCH90;

#endif  /* __COCO_ORCH90_H__ */
