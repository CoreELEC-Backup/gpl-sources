/**********************************************************************

    Apricot keyboard emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

*********************************************************************/

#pragma once

#ifndef __APRICOT_KEYBOARD__
#define __APRICOT_KEYBOARD__

#include "emu.h"



//**************************************************************************
//  MACROS / CONSTANTS
//**************************************************************************

#define APRICOT_KEYBOARD_TAG    "aprikb"



//**************************************************************************
//  INTERFACE CONFIGURATION MACROS
//**************************************************************************

#define MCFG_APRICOT_KEYBOARD_TXD_CALLBACK(_write) \
	devcb = &apricot_keyboard_device::set_tcd_wr_callback(*device, DEVCB_##_write);



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> apricot_keyboard_device

class apricot_keyboard_device :  public device_t
{
public:
	// construction/destruction
	apricot_keyboard_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	template<class _Object> static devcb_base &set_txd_wr_callback(device_t &device, _Object object) { return downcast<apricot_keyboard_device &>(device).m_write_txd.set_callback(object); }

	// optional information overrides
	virtual const rom_entry *device_rom_region() const;
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual ioport_constructor device_input_ports() const;

	UINT8 read_keyboard();

	DECLARE_READ8_MEMBER( kb_lo_r );
	DECLARE_READ8_MEMBER( kb_hi_r );
	DECLARE_READ8_MEMBER( kb_p6_r );
	DECLARE_WRITE8_MEMBER( kb_p3_w );
	DECLARE_WRITE8_MEMBER( kb_y0_w );
	DECLARE_WRITE8_MEMBER( kb_y4_w );
	DECLARE_WRITE8_MEMBER( kb_y8_w );
	DECLARE_WRITE8_MEMBER( kb_yc_w );

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

private:
	devcb_write_line   m_write_txd;

	required_ioport m_y0;
	required_ioport m_y1;
	required_ioport m_y2;
	required_ioport m_y3;
	required_ioport m_y4;
	required_ioport m_y5;
	required_ioport m_y6;
	required_ioport m_y7;
	required_ioport m_y8;
	required_ioport m_y9;
	required_ioport m_ya;
	required_ioport m_yb;
	required_ioport m_yc;
	required_ioport m_modifiers;

	UINT16 m_kb_y;
};


// device type definition
extern const device_type APRICOT_KEYBOARD;



#endif
