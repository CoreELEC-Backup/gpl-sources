/**********************************************************************

    Nintendo Super Famicom & SNES Multitap Adapter

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __SNES_MULTITAP__
#define __SNES_MULTITAP__


#include "emu.h"
#include "ctrl.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> snes_multitap_device

class snes_multitap_device : public device_t,
							public device_snes_control_port_interface
{
public:
	// construction/destruction
	snes_multitap_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual ioport_constructor device_input_ports() const;
	virtual machine_config_constructor device_mconfig_additions() const;

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	// device_sms_control_port_interface overrides
	virtual UINT8 read_pin4();
	virtual UINT8 read_pin5();
	virtual void write_strobe(UINT8 data);
	virtual void write_pin6(UINT8 data);
	virtual void port_poll();

private:
	required_device<snes_control_port_device> m_port1;
	required_device<snes_control_port_device> m_port2;
	required_device<snes_control_port_device> m_port3;
	required_device<snes_control_port_device> m_port4;
	required_ioport m_cfg;
	int m_select;
};


// device type definition
extern const device_type SNES_MULTITAP;


#endif
