/**********************************************************************

    Sega Master System "Paddle Control" emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __SMS_PADDLE__
#define __SMS_PADDLE__


#include "emu.h"
#include "smsctrl.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> sms_paddle_device

class sms_paddle_device : public device_t,
							public device_sms_control_port_interface
{
public:
	// construction/destruction
	sms_paddle_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual ioport_constructor device_input_ports() const;

	DECLARE_CUSTOM_INPUT_MEMBER( dir_pins_r );
	DECLARE_CUSTOM_INPUT_MEMBER( tr_pin_r );

protected:
	// device-level overrides
	virtual void device_start();

	// device_sms_control_port_interface overrides
	virtual UINT8 peripheral_r();

private:
	required_ioport m_paddle_pins;
	required_ioport m_paddle_x;

	UINT8 m_read_state;
	attotime m_start_time;
	const attotime m_interval;
};


// device type definition
extern const device_type SMS_PADDLE;


#endif
