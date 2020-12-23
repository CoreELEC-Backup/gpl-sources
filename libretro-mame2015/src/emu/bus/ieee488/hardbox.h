// license:BSD-3-Clause
// copyright-holders:Curt Coder, Mike Naberezny
/**********************************************************************

    SSE HardBox emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __PET_HARDBOX__
#define __PET_HARDBOX__

#include "ieee488.h"
#include "cpu/z80/z80.h"
#include "machine/i8251.h"
#include "machine/i8255.h"
#include "imagedev/harddriv.h"
#include "machine/corvushd.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> hardbox_device

class hardbox_device :  public device_t,
						public device_ieee488_interface
{
public:
	// construction/destruction
	hardbox_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual const rom_entry *device_rom_region() const;
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual ioport_constructor device_input_ports() const;

	DECLARE_READ8_MEMBER( ppi0_pa_r );
	DECLARE_WRITE8_MEMBER( ppi0_pb_w );
	DECLARE_READ8_MEMBER( ppi0_pc_r );

	DECLARE_READ8_MEMBER( ppi1_pa_r );
	DECLARE_WRITE8_MEMBER( ppi1_pb_w );
	DECLARE_READ8_MEMBER( ppi1_pc_r );
	DECLARE_WRITE8_MEMBER( ppi1_pc_w );

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset_after_children();

	// device_ieee488_interface overrides
	virtual void ieee488_ifc(int state);

private:
	enum
	{
		LED_A,
		LED_B,
		LED_READY
	};

	required_device<cpu_device> m_maincpu;
	required_device<corvus_hdc_t> m_hdc;

	int m_ifc;  // Tracks previous state of IEEE-488 IFC line
};

// device type definition
extern const device_type HARDBOX;



#endif
