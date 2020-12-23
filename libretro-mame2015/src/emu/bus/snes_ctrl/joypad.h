/**********************************************************************

    Nintendo Super Famicom & SNES Joypad

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __SNES_JOYPAD__
#define __SNES_JOYPAD__


#include "emu.h"
#include "ctrl.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> snes_joypad_device

class snes_joypad_device : public device_t,
							public device_snes_control_port_interface
{
public:
	// construction/destruction
	snes_joypad_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual ioport_constructor device_input_ports() const;

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	// device_sms_control_port_interface overrides
	virtual UINT8 read_pin4();
	virtual void write_strobe(UINT8 data);
	virtual void port_poll();

private:
	required_ioport m_joypad;
	int m_strobe;
	UINT32 m_latch;
};


// device type definition
extern const device_type SNES_JOYPAD;


#endif
