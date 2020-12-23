#ifndef __NES_SUNSOFT_DCS_H
#define __NES_SUNSOFT_DCS_H

#include "sunsoft.h"


//-----------------------------------------------
//
//  Nantettate!! Baseball Cartslot implementation
//
//-----------------------------------------------

// ======================> ntb_cart_interface

class ntb_cart_interface : public device_slot_card_interface
{
public:
	// construction/destruction
	ntb_cart_interface(const machine_config &mconfig, device_t &device);
	virtual ~ntb_cart_interface();

	// reading and writing
	virtual DECLARE_READ8_MEMBER(read) { return m_rom[offset]; }

	UINT8 *get_cart_base() { return m_rom; }

protected:
	// internal state
	UINT8 *m_rom;
};

// ======================> nes_ntb_slot_device

class nes_ntb_slot_device : public device_t,
								public device_image_interface,
								public device_slot_interface
{
public:
	// construction/destruction
	nes_ntb_slot_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	virtual ~nes_ntb_slot_device();

	// device-level overrides
	virtual void device_start();
	virtual void device_config_complete() { update_names(); }

	// image-level overrides
	virtual bool call_load();
	virtual bool call_softlist_load(software_list_device &swlist, const char *swname, const rom_entry *start_entry);

	virtual iodevice_t image_type() const { return IO_CARTSLOT; }
	virtual bool is_readable()  const { return 1; }
	virtual bool is_writeable() const { return 0; }
	virtual bool is_creatable() const { return 0; }
	virtual bool must_be_loaded() const { return 0; }
	virtual bool is_reset_on_load() const { return 1; }
	virtual const char *image_interface() const { return "ntb_cart"; }
	virtual const char *file_extensions() const { return "bin"; }
	virtual const option_guide *create_option_guide() const { return NULL; }

	// slot interface overrides
	virtual void get_default_card_software(astring &result);

	virtual DECLARE_READ8_MEMBER(read);

	ntb_cart_interface*      m_cart;
};

// device type definition
extern const device_type NES_NTB_SLOT;


#define MCFG_NTB_MINICART_ADD(_tag, _slot_intf) \
		MCFG_DEVICE_ADD(_tag, NES_NTB_SLOT, 0) \
		MCFG_DEVICE_SLOT_INTERFACE(_slot_intf, NULL, false)


//-----------------------------------------------
//
//  Nantettate!! Baseball Minicart implementation
//
//-----------------------------------------------

// ======================> nes_ntb_rom_device

class nes_ntb_rom_device : public device_t,
							public ntb_cart_interface
{
public:
	// construction/destruction
	nes_ntb_rom_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual const rom_entry *device_rom_region() const;
	virtual UINT8* get_cart_base();

protected:
	// device-level overrides
	virtual void device_start();
};

// device type definition
extern const device_type NES_NTB_ROM;



//------------------------------------------------
//
//  Nantettate!! Baseball base cart implementation
//  a.k.a. Sunsoft Dual Cassette System
//  (variant of Sunsoft-4 PCB)
//
//------------------------------------------------

// ======================> nes_sunsoft_dcs_device

class nes_sunsoft_dcs_device : public nes_sunsoft_4_device
{
public:
	// construction/destruction
	nes_sunsoft_dcs_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);
	virtual DECLARE_READ8_MEMBER(read_m);
	virtual DECLARE_READ8_MEMBER(read_h);
	virtual DECLARE_WRITE8_MEMBER(write_m);
	virtual DECLARE_WRITE8_MEMBER(write_h);

	virtual void pcb_reset();

private:
	int m_timer_on, m_exrom_enable;
	required_device<nes_ntb_slot_device> m_subslot;

	static const device_timer_id TIMER_PROTECT = 0;
	emu_timer *ntb_enable_timer;
	attotime timer_freq;
};



// device type definition
extern const device_type NES_SUNSOFT_DCS;

#endif
