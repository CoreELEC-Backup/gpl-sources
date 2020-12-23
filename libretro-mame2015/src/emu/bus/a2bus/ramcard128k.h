// license:BSD-3-Clause
// copyright-holders:R. Belmont
/*********************************************************************

    ramcard128k.h

    Implemention of the Saturn Systems 128K extended language card

*********************************************************************/

#ifndef __A2BUS_RAMCARD128K__
#define __A2BUS_RAMCARD128K__

#include "emu.h"
#include "a2bus.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class a2bus_ssramcard_device:
	public device_t,
	public device_a2bus_card_interface
{
public:
	// construction/destruction
	a2bus_ssramcard_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source);
	a2bus_ssramcard_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

protected:
	virtual void device_start();
	virtual void device_reset();

	// overrides of standard a2bus slot functions
	virtual UINT8 read_c0nx(address_space &space, UINT8 offset);
	virtual void write_c0nx(address_space &space, UINT8 offset, UINT8 data);
	virtual UINT8 read_inh_rom(address_space &space, UINT16 offset);
	virtual void write_inh_rom(address_space &space, UINT16 offset, UINT8 data);
	virtual UINT16 inh_start() { return 0xd000; }
	virtual UINT16 inh_end() { return 0xffff; }
	virtual int inh_type();

private:
	void do_io(int offset);

	int m_inh_state;
	int m_last_offset;
	int m_dxxx_bank;
	int m_main_bank;
	UINT8 m_ram[128*1024];
};

// device type definition
extern const device_type A2BUS_RAMCARD128K;

#endif /* __A2BUS_RAMCARD128K__ */
