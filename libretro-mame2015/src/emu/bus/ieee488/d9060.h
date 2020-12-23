// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Commodore D9060/D9090 Hard Disk Drive emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __D9060__
#define __D9060__

#include "emu.h"
#include "ieee488.h"
#include "cpu/m6502/m6502.h"
#include "machine/6522via.h"
#include "machine/6532riot.h"
#include "bus/scsi/scsi.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************


// ======================> base_d9060_device

class base_d9060_device :  public device_t,
							public device_ieee488_interface
{
public:
	enum
	{
		TYPE_9060,
		TYPE_9090
	};

	// construction/destruction
	base_d9060_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, UINT32 variant, const char *shortname, const char *source);

	// optional information overrides
	virtual const rom_entry *device_rom_region() const;
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual ioport_constructor device_input_ports() const;

	// not really public
	DECLARE_READ8_MEMBER( dio_r );
	DECLARE_WRITE8_MEMBER( dio_w );
	DECLARE_READ8_MEMBER( riot1_pa_r );
	DECLARE_WRITE8_MEMBER( riot1_pa_w );
	DECLARE_READ8_MEMBER( riot1_pb_r );
	DECLARE_WRITE8_MEMBER( riot1_pb_w );
	DECLARE_READ8_MEMBER( via_pb_r );
	DECLARE_WRITE8_MEMBER( via_pb_w );
	DECLARE_WRITE_LINE_MEMBER( ack_w );
	DECLARE_WRITE_LINE_MEMBER( enable_w );
	DECLARE_WRITE8_MEMBER( scsi_data_w );

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	// device_ieee488_interface overrides
	void ieee488_atn(int state);
	void ieee488_ifc(int state);

private:
	inline void update_ieee_signals();

	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_hdccpu;
	required_device<riot6532_device> m_riot0;
	required_device<riot6532_device> m_riot1;
	required_device<via6522_device> m_via;
	required_device<SCSI_PORT_DEVICE> m_sasibus;
	required_device<output_latch_device> m_sasi_data_out;
	required_ioport m_address;

	// IEEE-488 bus
	int m_rfdo;                         // not ready for data output
	int m_daco;                         // not data accepted output
	int m_atna;                         // attention acknowledge
	int m_ifc;

	// SASI bus
	int m_enable;
	UINT8 m_data;

	int m_variant;
};


// ======================> d9060_device

class d9060_device :  public base_d9060_device
{
public:
	// construction/destruction
	d9060_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
};


// ======================> d9090_device

class d9090_device :  public base_d9060_device
{
public:
	// construction/destruction
	d9090_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
};


// device type definition
extern const device_type D9060;
extern const device_type D9090;



#endif
