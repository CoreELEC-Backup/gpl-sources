// license:BSD-3-Clause
// copyright-holders:etabeta
#ifndef __VC4000_ROM_H
#define __VC4000_ROM_H

#include "slot.h"


// ======================> vc4000_rom_device

class vc4000_rom_device : public device_t,
						public device_vc4000_cart_interface
{
public:
	// construction/destruction
	vc4000_rom_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source);
	vc4000_rom_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start() {}
	virtual void device_reset() {}

	// reading and writing
	virtual DECLARE_READ8_MEMBER(read_rom);
};

// ======================> vc4000_rom4k_device

class vc4000_rom4k_device : public vc4000_rom_device
{
public:
	// construction/destruction
	vc4000_rom4k_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
};

// ======================> vc4000_ram1k_device

class vc4000_ram1k_device : public vc4000_rom_device
{
public:
	// construction/destruction
	vc4000_ram1k_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// reading and writing
	virtual DECLARE_READ8_MEMBER(read_ram);
	virtual DECLARE_WRITE8_MEMBER(write_ram);
};

// ======================> vc4000_chess2_device

class vc4000_chess2_device : public vc4000_rom_device
{
public:
	// construction/destruction
	vc4000_chess2_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// reading and writing
	virtual DECLARE_READ8_MEMBER(extra_rom);
	virtual DECLARE_READ8_MEMBER(read_ram);
	virtual DECLARE_WRITE8_MEMBER(write_ram);
};





// device type definition
extern const device_type VC4000_ROM_STD;
extern const device_type VC4000_ROM_ROM4K;
extern const device_type VC4000_ROM_RAM1K;
extern const device_type VC4000_ROM_CHESS2;


#endif
