/**********************************************************************

    Nintendo Family Computer & Entertainment System Joypads

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __NES_JOYPAD__
#define __NES_JOYPAD__


#include "emu.h"
#include "ctrl.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> nes_joypad_device

class nes_joypad_device : public device_t,
							public device_nes_control_port_interface
{
public:
	// construction/destruction
	nes_joypad_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source);
	nes_joypad_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	virtual ioport_constructor device_input_ports() const;

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	virtual UINT8 read_bit0();
	virtual void write(UINT8 data);

	required_ioport m_joypad;
	UINT32 m_latch;
};

// ======================> nes_fcpad2_device

class nes_fcpad2_device : public nes_joypad_device
{
public:
	// construction/destruction
	nes_fcpad2_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	virtual ioport_constructor device_input_ports() const;

protected:
	virtual UINT8 read_exp(offs_t offset);
	virtual void write(UINT8 data);
};

// ======================> nes_ccpadl_device

class nes_ccpadl_device : public nes_joypad_device
{
public:
	// construction/destruction
	nes_ccpadl_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	virtual ioport_constructor device_input_ports() const;
};

// ======================> nes_ccpadr_device

class nes_ccpadr_device : public nes_joypad_device
{
public:
	// construction/destruction
	nes_ccpadr_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	virtual ioport_constructor device_input_ports() const;
};

// ======================> nes_arcstick_device

class nes_arcstick_device : public nes_joypad_device
{
public:
	// construction/destruction
	nes_arcstick_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	virtual ioport_constructor device_input_ports() const;
	virtual machine_config_constructor device_mconfig_additions() const;

protected:
	virtual UINT8 read_bit0() { return 0; }
	virtual UINT8 read_exp(offs_t offset);
	virtual void write(UINT8 data);

	required_device<nes_control_port_device> m_daisychain;
	required_ioport m_cfg;
};


// device type definition
extern const device_type NES_JOYPAD;
extern const device_type NES_FCPAD_P2;
extern const device_type NES_CCPAD_LEFT;
extern const device_type NES_CCPAD_RIGHT;
extern const device_type NES_ARCSTICK;

#endif
