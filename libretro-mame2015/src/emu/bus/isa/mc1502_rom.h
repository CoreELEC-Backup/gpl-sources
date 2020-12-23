// license:BSD-3-Clause
// copyright-holders:XXX
/**********************************************************************

    MC-1502 ROM cartridge device

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __MC1502_ROM__
#define __MC1502_ROM__

#include "emu.h"
#include "isa.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class mc1502_rom_device : public device_t,
	public device_isa8_card_interface
{
public:
	// construction/destruction
	mc1502_rom_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual const rom_entry *device_rom_region() const;

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();
};


// device type definition
extern const device_type MC1502_ROM;


#endif
