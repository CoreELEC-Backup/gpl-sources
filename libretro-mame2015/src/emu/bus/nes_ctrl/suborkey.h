/**********************************************************************

    Nintendo Family Computer Subor Keyboard (used by some Famiclones)

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __NES_SUBORKEY__
#define __NES_SUBORKEY__


#include "emu.h"
#include "ctrl.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> nes_suborkey_device

class nes_suborkey_device : public device_t,
							public device_nes_control_port_interface
{
public:
	// construction/destruction
	nes_suborkey_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	virtual ioport_constructor device_input_ports() const;

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	virtual UINT8 read_exp(offs_t offset);
	virtual void write(UINT8 data);

private:
	required_ioport_array<13> m_kbd;
	UINT8 m_fck_scan, m_fck_mode;
};


// device type definition
extern const device_type NES_SUBORKEYBOARD;


#endif
