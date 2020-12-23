// license:MAME, GPL-2.0+
// copyright-holders:Dirk Best
/**********************************************************************

    Epson EX-800 dot matrix printer emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __EPSON_EX800__
#define __EPSON_EX800__

#include "emu.h"
#include "ctronics.h"
#include "cpu/upd7810/upd7810.h"
#include "sound/beep.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> epson_ex800_t

class epson_ex800_t :  public device_t,
						public device_centronics_peripheral_interface
{
public:
	// construction/destruction
	epson_ex800_t(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	epson_ex800_t(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source);

	// optional information overrides
	virtual const rom_entry *device_rom_region() const;
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual ioport_constructor device_input_ports() const;

	DECLARE_READ8_MEMBER(porta_r);
	DECLARE_READ8_MEMBER(portb_r);
	DECLARE_READ8_MEMBER(portc_r);
	DECLARE_WRITE8_MEMBER(porta_w);
	DECLARE_WRITE8_MEMBER(portb_w);
	DECLARE_WRITE8_MEMBER(portc_w);
	DECLARE_READ8_MEMBER(devsel_r);
	DECLARE_WRITE8_MEMBER(devsel_w);
	DECLARE_READ8_MEMBER(gate5a_r);
	DECLARE_WRITE8_MEMBER(gate5a_w);
	DECLARE_READ8_MEMBER(iosel_r);
	DECLARE_WRITE8_MEMBER(iosel_w);
	DECLARE_READ8_MEMBER(gate7a_r);
	DECLARE_WRITE8_MEMBER(gate7a_w);

	DECLARE_INPUT_CHANGED_MEMBER(online_switch);

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

private:
	required_device<cpu_device> m_maincpu;
	required_device<beep_device> m_beeper;

	int m_irq_state;
};



// device type definition
extern const device_type EPSON_EX800;



#endif
