// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Western Digital WD1002A-WX1 Winchester Disk Controller emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __ISA_WD1002A_WX1__
#define __ISA_WD1002A_WX1__

#include "emu.h"
#include "isa.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> isa16_ide_device

class isa8_wd1002a_wx1_device : public device_t,
								public device_isa8_card_interface
{
public:
	// construction/destruction
	isa8_wd1002a_wx1_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual const rom_entry *device_rom_region() const;

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();
};


// device type definition
extern const device_type ISA8_WD1002A_WX1;


#endif
