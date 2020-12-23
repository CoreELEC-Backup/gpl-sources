// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Commodore VIC-10 Standard 8K/16K ROM Cartridge emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __VIC10_STD__
#define __VIC10_STD__

#include "emu.h"
#include "exp.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> vic10_standard_cartridge_device

class vic10_standard_cartridge_device :  public device_t,
											public device_vic10_expansion_card_interface
{
public:
	// construction/destruction
	vic10_standard_cartridge_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

protected:
	// device-level overrides
	virtual void device_start();

	// device_vic10_expansion_card_interface overrides
	virtual UINT8 vic10_cd_r(address_space &space, offs_t offset, UINT8 data, int lorom, int uprom, int exram);
	virtual void vic10_cd_w(address_space &space, offs_t offset, UINT8 data, int lorom, int uprom, int exram);
};


// device type definition
extern const device_type VIC10_STD;



#endif
