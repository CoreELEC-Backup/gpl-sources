#ifndef __SEGA8_MGEAR_H
#define __SEGA8_MGEAR_H

#include "sega8_slot.h"
#include "rom.h"


// ======================> sega8_mgear_device

class sega8_mgear_device : public sega8_rom_device
{
public:
	// construction/destruction
	sega8_mgear_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	// reading and writing
	virtual DECLARE_READ8_MEMBER(read_cart) { return m_subslot->read_cart(space, offset); }
	virtual DECLARE_WRITE8_MEMBER(write_cart) { m_subslot->write_cart(space, offset, data); }
	virtual DECLARE_WRITE8_MEMBER(write_mapper) { m_subslot->write_mapper(space, offset, data); }

	virtual machine_config_constructor device_mconfig_additions() const;

protected:
	required_device<sega8_cart_slot_device> m_subslot;
};


// device type definition
extern const device_type SEGA8_ROM_MGEAR;


#endif
