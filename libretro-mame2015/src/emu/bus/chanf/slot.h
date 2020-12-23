// license:BSD-3-Clause
// copyright-holders:etabeta
#ifndef __CHANF_SLOT_H
#define __CHANF_SLOT_H

/***************************************************************************
 TYPE DEFINITIONS
 ***************************************************************************/


/* PCB */
enum
{
	CF_STD = 0,
	CF_MAZE,
	CF_HANGMAN,
	CF_CHESS,
	CF_MULTI_OLD,
	CF_MULTI
};


// ======================> device_channelf_cart_interface

class device_channelf_cart_interface : public device_slot_card_interface
{
public:
	// device_channelf_cart_interface/destruction
	device_channelf_cart_interface(const machine_config &mconfig, device_t &device);
	virtual ~device_channelf_cart_interface();

	// reading and writing
	virtual DECLARE_READ8_MEMBER(read_rom) { return 0xff; }
	virtual DECLARE_READ8_MEMBER(read_ram) { return 0xff; }
	virtual DECLARE_WRITE8_MEMBER(write_ram) {}
	virtual DECLARE_WRITE8_MEMBER(write_bank) {}

	void rom_alloc(UINT32 size, const char *tag);
	void ram_alloc(UINT32 size);
	UINT8* get_rom_base() { return m_rom; }
	UINT8* get_ram_base() { return m_ram; }
	UINT32 get_rom_size() { return m_rom_size; }
	UINT32 get_ram_size() { return m_ram.count(); }

	void save_ram() { device().save_item(NAME(m_ram)); }

protected:
	// internal state
	UINT8 *m_rom;
	UINT32 m_rom_size;
	dynamic_buffer m_ram;
};


// ======================> channelf_cart_slot_device

class channelf_cart_slot_device : public device_t,
								public device_image_interface,
								public device_slot_interface
{
public:
	// construction/destruction
	channelf_cart_slot_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	virtual ~channelf_cart_slot_device();

	// device-level overrides
	virtual void device_start();
	virtual void device_config_complete();

	// image-level overrides
	virtual bool call_load();
	virtual void call_unload() {}
	virtual bool call_softlist_load(software_list_device &swlist, const char *swname, const rom_entry *start_entry);

	int get_type() { return m_type; }

	void save_ram() { if (m_cart && m_cart->get_ram_size()) m_cart->save_ram(); }

	virtual iodevice_t image_type() const { return IO_CARTSLOT; }
	virtual bool is_readable()  const { return 1; }
	virtual bool is_writeable() const { return 0; }
	virtual bool is_creatable() const { return 0; }
	virtual bool must_be_loaded() const { return 0; }
	virtual bool is_reset_on_load() const { return 1; }
	virtual const option_guide *create_option_guide() const { return NULL; }
	virtual const char *image_interface() const { return "channelf_cart"; }
	virtual const char *file_extensions() const { return "bin,chf"; }

	// slot interface overrides
	virtual void get_default_card_software(astring &result);

	// reading and writing
	virtual DECLARE_READ8_MEMBER(read_rom);
	virtual DECLARE_READ8_MEMBER(read_ram);
	virtual DECLARE_WRITE8_MEMBER(write_ram);
	virtual DECLARE_WRITE8_MEMBER(write_bank);

protected:

	int m_type;
	device_channelf_cart_interface*       m_cart;
};



// device type definition
extern const device_type CHANF_CART_SLOT;


/***************************************************************************
 DEVICE CONFIGURATION MACROS
 ***************************************************************************/

#define CHANFSLOT_ROM_REGION_TAG ":cart:rom"

#define MCFG_CHANNELF_CARTRIDGE_ADD(_tag,_slot_intf,_def_slot) \
	MCFG_DEVICE_ADD(_tag, CHANF_CART_SLOT, 0) \
	MCFG_DEVICE_SLOT_INTERFACE(_slot_intf, _def_slot, false)
#endif
