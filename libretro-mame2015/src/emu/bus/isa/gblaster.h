#pragma once

#ifndef __ISA_GAME_BLASTER_H__
#define __ISA_GAME_BLASTER_H__

#include "emu.h"
#include "isa.h"
#include "sound/saa1099.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> isa8_gblaster_device

class isa8_gblaster_device :
		public device_t,
		public device_isa8_card_interface
{
public:
		// construction/destruction
		isa8_gblaster_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

		// optional information overrides
		virtual machine_config_constructor device_mconfig_additions() const;

		DECLARE_READ8_MEMBER(saa1099_16_r);
		DECLARE_WRITE8_MEMBER(saa1099_1_16_w);
		DECLARE_WRITE8_MEMBER(saa1099_2_16_w);
protected:
		// device-level overrides
		virtual void device_start();
		virtual void device_reset();
private:
		// internal state
		required_device<saa1099_device> m_saa1099_1;
		required_device<saa1099_device> m_saa1099_2;
};


// device type definition
extern const device_type ISA8_GAME_BLASTER;

#endif  /* __ISA_GAME_BLASTER_H__ */
