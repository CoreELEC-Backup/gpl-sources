/**********************************************************************

    Nintendo Super Famicom - Epoch Barcode Battler

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __SNES_BCBATTLE__
#define __SNES_BCBATTLE__


#include "emu.h"
#include "ctrl.h"
#include "machine/bcreader.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> snes_bcbattle_device

class snes_bcbattle_device : public device_t,
							public device_snes_control_port_interface
{
public:
	// construction/destruction
	snes_bcbattle_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);
	virtual machine_config_constructor device_mconfig_additions() const;

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	virtual UINT8 read_pin4();
	virtual void write_strobe(UINT8 data);
	virtual void port_poll();

	int read_current_bit();

private:

	static const device_timer_id TIMER_BATTLER = 1;
	required_device<barcode_reader_device> m_reader;
	UINT8 m_current_barcode[20];
	int m_pending_code, m_new_code, m_transmitting, m_cur_bit, m_cur_byte;
	emu_timer *battler_timer;

	int m_strobe, m_idx;
};

// device type definition
extern const device_type SNES_BARCODE_BATTLER;

#endif
