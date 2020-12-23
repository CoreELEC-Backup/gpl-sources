// license:BSD-3-Clause
// copyright-holders:etabeta
#ifndef __VBOY_SLOT_H
#define __VBOY_SLOT_H

/***************************************************************************
 TYPE DEFINITIONS
 ***************************************************************************/


/* PCB */
enum
{
	VBOY_STD = 0,
	VBOY_EEPROM
};


// ======================> device_vboy_cart_interface

class device_vboy_cart_interface : public device_slot_card_interface
{
public:
	// construction/destruction
	device_vboy_cart_interface(const machine_config &mconfig, device_t &device);
	virtual ~device_vboy_cart_interface();

	// reading and writing
	virtual DECLARE_READ32_MEMBER(read_cart) { return 0xffffffff; }
	virtual DECLARE_READ32_MEMBER(read_eeprom) { return 0xffffffff; }
	virtual DECLARE_WRITE32_MEMBER(write_eeprom) {}

	void rom_alloc(UINT32 size, const char *tag);
	void eeprom_alloc(UINT32 size);
	UINT32* get_rom_base() { return m_rom; }
	UINT32* get_eeprom_base() { return m_eeprom; }
	UINT32 get_rom_size() { return m_rom_size; }
	UINT32 get_eeprom_size() { return m_eeprom.count(); }

	void save_eeprom()  { device().save_item(NAME(m_eeprom)); }

protected:
	// internal state
	UINT32 *m_rom;
	UINT32 m_rom_size;
	UINT32 m_rom_mask;
	dynamic_array<UINT32> m_eeprom;
};


// ======================> vboy_cart_slot_device

class vboy_cart_slot_device : public device_t,
								public device_image_interface,
								public device_slot_interface
{
public:
	// construction/destruction
	vboy_cart_slot_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	virtual ~vboy_cart_slot_device();

	// device-level overrides
	virtual void device_start();
	virtual void device_config_complete();

	// image-level overrides
	virtual bool call_load();
	virtual void call_unload();
	virtual bool call_softlist_load(software_list_device &swlist, const char *swname, const rom_entry *start_entry);

	int get_type() { return m_type; }
	int get_cart_type(UINT8 *ROM, UINT32 len);

	void save_eeprom()  { if (m_cart && m_cart->get_eeprom_size()) m_cart->save_eeprom(); }

	virtual iodevice_t image_type() const { return IO_CARTSLOT; }
	virtual bool is_readable()  const { return 1; }
	virtual bool is_writeable() const { return 0; }
	virtual bool is_creatable() const { return 0; }
	virtual bool must_be_loaded() const { return 1; }
	virtual bool is_reset_on_load() const { return 1; }
	virtual const option_guide *create_option_guide() const { return NULL; }
	virtual const char *image_interface() const { return "vboy_cart"; }
	virtual const char *file_extensions() const { return "vb,bin"; }

	// slot interface overrides
	virtual void get_default_card_software(astring &result);

	// reading and writing
	virtual DECLARE_READ32_MEMBER(read_cart);
	virtual DECLARE_READ32_MEMBER(read_eeprom);
	virtual DECLARE_WRITE32_MEMBER(write_eeprom);

protected:

	int m_type;
	device_vboy_cart_interface*       m_cart;
};



// device type definition
extern const device_type VBOY_CART_SLOT;


/***************************************************************************
 DEVICE CONFIGURATION MACROS
 ***************************************************************************/

#define VBOYSLOT_ROM_REGION_TAG ":cart:rom"

#define MCFG_VBOY_CARTRIDGE_ADD(_tag,_slot_intf,_def_slot) \
	MCFG_DEVICE_ADD(_tag, VBOY_CART_SLOT, 0) \
	MCFG_DEVICE_SLOT_INTERFACE(_slot_intf, _def_slot, false)
#endif
