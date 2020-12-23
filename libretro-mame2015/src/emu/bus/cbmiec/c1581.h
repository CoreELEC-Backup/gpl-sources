// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Commodore 1581/1563 Single Disk Drive emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __C1581__
#define __C1581__

#include "emu.h"
#include "cbmiec.h"
#include "cpu/m6502/m6502.h"
#include "formats/d81_dsk.h"
#include "machine/mos6526.h"
#include "machine/wd_fdc.h"



//**************************************************************************
//  MACROS / CONSTANTS
//**************************************************************************

#define C1581_TAG           "c1581"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> c1581_device

class c1581_device :  public device_t,
						public device_cbm_iec_interface
{
public:
	// construction/destruction
	c1581_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source);
	c1581_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual const rom_entry *device_rom_region() const;
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual ioport_constructor device_input_ports() const;

	// not really public
	DECLARE_WRITE_LINE_MEMBER( cnt_w );
	DECLARE_WRITE_LINE_MEMBER( sp_w );
	DECLARE_READ8_MEMBER( cia_pa_r );
	DECLARE_WRITE8_MEMBER( cia_pa_w );
	DECLARE_READ8_MEMBER( cia_pb_r );
	DECLARE_WRITE8_MEMBER( cia_pb_w );

	DECLARE_FLOPPY_FORMATS( floppy_formats );

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	// device_cbm_iec_interface overrides
	virtual void cbm_iec_srq(int state);
	virtual void cbm_iec_atn(int state);
	virtual void cbm_iec_data(int state);
	virtual void cbm_iec_reset(int state);

	enum
	{
		LED_POWER = 0,
		LED_ACT
	};

	void update_iec();

	required_device<cpu_device> m_maincpu;
	required_device<mos6526_device> m_cia;
	required_device<wd1772_t> m_fdc;
	required_device<floppy_image_device> m_floppy;
	required_ioport m_address;

	int m_data_out;             // serial data out
	int m_atn_ack;              // attention acknowledge
	int m_fast_ser_dir;         // fast serial direction
	int m_sp_out;               // fast serial data out
	int m_cnt_out;              // fast serial clock out
};


// ======================> c1563_device

class c1563_device :  public c1581_device
{
public:
	// construction/destruction
	c1563_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual const rom_entry *device_rom_region() const;
};


// device type definition
extern const device_type C1563;
extern const device_type C1581;



#endif
