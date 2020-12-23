// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Structured Basic cartridge emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __STRUCTURED_BASIC__
#define __STRUCTURED_BASIC__


#include "emu.h"
#include "exp.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> c64_structured_basic_cartridge_device

class c64_structured_basic_cartridge_device : public device_t,
												public device_c64_expansion_card_interface
{
public:
	// construction/destruction
	c64_structured_basic_cartridge_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	// device_c64_expansion_card_interface overrides
	virtual UINT8 c64_cd_r(address_space &space, offs_t offset, UINT8 data, int sphi2, int ba, int roml, int romh, int io1, int io2);
	virtual void c64_cd_w(address_space &space, offs_t offset, UINT8 data, int sphi2, int ba, int roml, int romh, int io1, int io2);

private:
	UINT8 m_bank;
};


// device type definition
extern const device_type C64_STRUCTURED_BASIC;



#endif
