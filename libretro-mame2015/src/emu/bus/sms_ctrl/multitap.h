/**********************************************************************

    Furrtek's homemade multitap emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __SMS_MULTITAP__
#define __SMS_MULTITAP__


#include "emu.h"
#include "smsctrl.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> sms_multitap_device

class sms_multitap_device : public device_t,
							public device_sms_control_port_interface
{
public:
	// construction/destruction
	sms_multitap_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	DECLARE_READ32_MEMBER(pixel_r);

protected:
	// device-level overrides
	virtual void device_start();
	virtual machine_config_constructor device_mconfig_additions() const;

	// device_sms_control_port_interface overrides
	virtual UINT8 peripheral_r();
	virtual void peripheral_w(UINT8 data);

private:
	required_device<sms_control_port_device> m_subctrl1_port;
	required_device<sms_control_port_device> m_subctrl2_port;
	required_device<sms_control_port_device> m_subctrl3_port;
	required_device<sms_control_port_device> m_subctrl4_port;

	UINT8 m_read_state;
	UINT8 m_last_data;
};


// device type definition
extern const device_type SMS_MULTITAP;


#endif
