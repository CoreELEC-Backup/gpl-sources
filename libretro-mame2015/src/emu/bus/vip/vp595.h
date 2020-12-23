// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    RCA VIP Simple Sound Board VP595 emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __VP595__
#define __VP595__

#include "emu.h"
#include "exp.h"
#include "sound/cdp1863.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> vp595_device

class vp595_device : public device_t,
						public device_vip_expansion_card_interface
{
public:
	// construction/destruction
	vp595_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const;

protected:
	// device-level overrides
	virtual void device_start();

	// device_vip_expansion_card_interface overrides
	virtual void vip_io_w(address_space &space, offs_t offset, UINT8 data);
	virtual void vip_q_w(int state);

private:
	required_device<cdp1863_device> m_pfg;
};


// device type definition
extern const device_type VP595;


#endif
