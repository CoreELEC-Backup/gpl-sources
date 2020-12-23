// license:BSD-3-Clause
// copyright-holders:Sandro Ronco
#pragma once

#ifndef __KC_D002_H__
#define __KC_D002_H__

#include "emu.h"
#include "kc.h"
#include "ram.h"
#include "rom.h"
#include "d004.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> kc_d002_device

class kc_d002_device :
		public device_t,
		public device_kcexp_interface
{
public:
	// construction/destruction
	kc_d002_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual ioport_constructor device_input_ports() const;

	// interface callbacks
	DECLARE_WRITE_LINE_MEMBER( out_irq_w );
	DECLARE_WRITE_LINE_MEMBER( out_nmi_w );
	DECLARE_WRITE_LINE_MEMBER( out_halt_w );

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	// kcexp_interface overrides
	virtual void read(offs_t offset, UINT8 &data);
	virtual void write(offs_t offset, UINT8 data);
	virtual void io_read(offs_t offset, UINT8 &data);
	virtual void io_write(offs_t offset, UINT8 data);
	virtual DECLARE_WRITE_LINE_MEMBER( mei_w );

private:
	kcexp_slot_device *m_slot;

	// internal state
	kcexp_slot_device *m_expansions[5];
};


// device type definition
extern const device_type KC_D002;

#endif  /* __KC_D002_H__ */
