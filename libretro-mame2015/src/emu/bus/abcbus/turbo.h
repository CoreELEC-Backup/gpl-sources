// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    MyAB Turbo-Kontroller disk controller emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

*********************************************************************/

#pragma once

#ifndef __TURBO_KONTROLLER__
#define __TURBO_KONTROLLER__

#include "emu.h"
#include "abcbus.h"
#include "cpu/z80/z80.h"
#include "cpu/z80/z80daisy.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> turbo_kontroller_device

class turbo_kontroller_device :  public device_t,
									public device_abcbus_card_interface
{
public:
	// construction/destruction
	turbo_kontroller_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual const rom_entry *device_rom_region() const;
	virtual machine_config_constructor device_mconfig_additions() const;

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	// device_abcbus_interface overrides
	virtual void abcbus_cs(UINT8 data);

private:
	required_device<cpu_device> m_maincpu;
};


// device type definition
extern const device_type TURBO_KONTROLLER;



#endif
