// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Triton QD TDOS cartridge emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __TDOS__
#define __TDOS__

#include "emu.h"
#include "exp.h"
#include "machine/mc6852.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> c64_tdos_cartridge_device

class c64_tdos_cartridge_device : public device_t,
									public device_c64_expansion_card_interface
{
public:
	// construction/destruction
	c64_tdos_cartridge_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual ioport_constructor device_input_ports() const;

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	// device_c64_expansion_card_interface overrides
	virtual UINT8 c64_cd_r(address_space &space, offs_t offset, UINT8 data, int sphi2, int ba, int roml, int romh, int io1, int io2);
	virtual void c64_cd_w(address_space &space, offs_t offset, UINT8 data, int sphi2, int ba, int roml, int romh, int io1, int io2);
	virtual int c64_game_r(offs_t offset, int sphi2, int ba, int rw);
	virtual int c64_exrom_r(offs_t offset, int sphi2, int ba, int rw);

private:
	required_device<mc6852_device> m_ssda;
	required_device<c64_expansion_slot_device> m_exp;
	required_ioport m_sw1;

	bool m_enabled;
};


// device type definition
extern const device_type C64_TDOS;


#endif
