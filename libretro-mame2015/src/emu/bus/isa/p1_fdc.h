// license:BSD-3-Clause
// copyright-holders:XXX
/**********************************************************************

    Poisk-1 FDC device (model B504)

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __P1_FDC__
#define __P1_FDC__

#include "emu.h"

#include "imagedev/flopdrv.h"
#include "isa.h"
#include "machine/wd_fdc.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class p1_fdc_device : public device_t,
	public device_isa8_card_interface
{
public:
	// construction/destruction
	p1_fdc_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual const rom_entry *device_rom_region() const;

	DECLARE_FLOPPY_FORMATS( floppy_formats );
	DECLARE_READ8_MEMBER(p1_fdc_r);
	DECLARE_WRITE8_MEMBER(p1_fdc_w);
	DECLARE_WRITE_LINE_MEMBER( p1_fdc_irq_drq );
protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

private:
	required_device<fd1793_t> m_fdc;

public:
	void p1_wd17xx_aux_w(int data);
	UINT8 p1_wd17xx_aux_r();
	UINT8 p1_wd17xx_motor_r();
};


// device type definition
extern const device_type P1_FDC;


#endif
