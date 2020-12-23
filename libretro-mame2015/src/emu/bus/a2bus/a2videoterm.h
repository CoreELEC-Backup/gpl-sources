/*********************************************************************

    a2videoterm.h

    Implementation of the Apple II Memory Expansion Card

*********************************************************************/

#ifndef __A2BUS_VIDEOTERM__
#define __A2BUS_VIDEOTERM__

#include "emu.h"
#include "a2bus.h"
#include "video/mc6845.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class a2bus_videx80_device:
	public device_t,
	public device_a2bus_card_interface
{
public:
	// construction/destruction
	a2bus_videx80_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const;

	DECLARE_WRITE_LINE_MEMBER(vsync_changed);
	MC6845_UPDATE_ROW(crtc_update_row);

	UINT8 *m_rom, *m_chrrom;
	UINT8 m_ram[512*4];
	int m_framecnt;

protected:
	virtual void device_start();
	virtual void device_reset();

	// overrides of standard a2bus slot functions
	virtual UINT8 read_c0nx(address_space &space, UINT8 offset);
	virtual void write_c0nx(address_space &space, UINT8 offset, UINT8 data);
	virtual UINT8 read_cnxx(address_space &space, UINT8 offset);
	virtual void write_cnxx(address_space &space, UINT8 offset, UINT8 data);
	virtual UINT8 read_c800(address_space &space, UINT16 offset);
	virtual void write_c800(address_space &space, UINT16 offset, UINT8 data);

	required_device<mc6845_device> m_crtc;

private:
	int m_rambank;
public:
	required_device<palette_device> m_palette;
};

class a2bus_videoterm_device : public a2bus_videx80_device
{
public:
	a2bus_videoterm_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	virtual const rom_entry *device_rom_region() const;
};

class a2bus_ap16_device : public a2bus_videx80_device
{
public:
	a2bus_ap16_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	virtual const rom_entry *device_rom_region() const;

	virtual UINT8 read_cnxx(address_space &space, UINT8 offset);
};


class a2bus_ap16alt_device : public a2bus_videx80_device
{
public:
	a2bus_ap16alt_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	virtual const rom_entry *device_rom_region() const;

	virtual UINT8 read_cnxx(address_space &space, UINT8 offset);
};

class a2bus_vtc1_device : public a2bus_videx80_device
{
public:
	a2bus_vtc1_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	virtual const rom_entry *device_rom_region() const;
};

class a2bus_vtc2_device : public a2bus_videx80_device
{
public:
	a2bus_vtc2_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	virtual const rom_entry *device_rom_region() const;
};

class a2bus_aevm80_device : public a2bus_videx80_device
{
public:
	a2bus_aevm80_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	virtual const rom_entry *device_rom_region() const;
};

// device type definition
extern const device_type A2BUS_VIDEOTERM;
extern const device_type A2BUS_IBSAP16;
extern const device_type A2BUS_IBSAP16ALT;
extern const device_type A2BUS_VTC1;
extern const device_type A2BUS_VTC2;
extern const device_type A2BUS_AEVIEWMASTER80;

#endif /* __A2BUS_VIDEOTERM__ */
