// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    ColecoVision standard cartridge emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __COLECOVISION_STANDARD_CARTRIDGE__
#define __COLECOVISION_STANDARD_CARTRIDGE__

#include "exp.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> colecovision_standard_cartridge_device

class colecovision_standard_cartridge_device : public device_t,
												public device_colecovision_cartridge_interface
{
public:
	// construction/destruction
	colecovision_standard_cartridge_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

protected:
	// device-level overrides
	virtual void device_start();

	// device_colecovision_expansion_card_interface overrides
	virtual UINT8 bd_r(address_space &space, offs_t offset, UINT8 data, int _8000, int _a000, int _c000, int _e000);
};


// device type definition
extern const device_type COLECOVISION_STANDARD;


#endif
