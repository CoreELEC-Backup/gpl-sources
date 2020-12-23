/**********************************************************************

    Nintendo Family Computer - Bandai Family Trainer Mat

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __NES_FTRAINER__
#define __NES_FTRAINER__


#include "emu.h"
#include "ctrl.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> nes_ftrainer_device

class nes_ftrainer_device : public device_t,
							public device_nes_control_port_interface
{
public:
	// construction/destruction
	nes_ftrainer_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	virtual ioport_constructor device_input_ports() const;

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	virtual UINT8 read_exp(offs_t offset);
	virtual void write(UINT8 data);

private:
	required_ioport_array<4> m_trainer;
	UINT8 m_row_scan;
};


// device type definition
extern const device_type NES_FTRAINER;


#endif
