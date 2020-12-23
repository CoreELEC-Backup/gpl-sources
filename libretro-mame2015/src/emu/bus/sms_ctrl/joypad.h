/**********************************************************************

    Sega Master System "Control Pad"/generic joystick emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __SMS_JOYPAD__
#define __SMS_JOYPAD__


#include "emu.h"
#include "smsctrl.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> sms_joypad_device

class sms_joypad_device : public device_t,
							public device_sms_control_port_interface
{
public:
	// construction/destruction
	sms_joypad_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual ioport_constructor device_input_ports() const;

protected:
	// device-level overrides
	virtual void device_start();

	// device_sms_control_port_interface overrides
	virtual UINT8 peripheral_r();

private:
	required_ioport m_joypad;
};


// device type definition
extern const device_type SMS_JOYPAD;


#endif
