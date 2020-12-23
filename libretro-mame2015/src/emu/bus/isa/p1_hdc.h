// license:BSD-3-Clause
// copyright-holders:XXX
/**********************************************************************

    Poisk-1 HDC device (model B942)

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __P1_HDC__
#define __P1_HDC__

#include "emu.h"

#include "imagedev/harddriv.h"
#include "isa.h"
#include "machine/wd2010.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class p1_hdc_device : public device_t,
	public device_isa8_card_interface
{
public:
	// construction/destruction
	p1_hdc_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual const rom_entry *device_rom_region() const;

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

private:
	required_device<wd2010_device> m_hdc;

	//UINT8 m_ram[0x800];

public:
	DECLARE_READ8_MEMBER(p1_HDC_r);
	DECLARE_WRITE8_MEMBER(p1_HDC_w);
};


// device type definition
extern const device_type P1_HDC;


#endif
