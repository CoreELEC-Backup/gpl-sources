#ifndef __SAT_SLOT_H
#define __SAT_SLOT_H

/***************************************************************************
 TYPE DEFINITIONS
 ***************************************************************************/


// ======================> device_sat_cart_interface

class device_sat_cart_interface : public device_slot_card_interface
{
public:
	// construction/destruction
	device_sat_cart_interface(const machine_config &mconfig, device_t &device);
	virtual ~device_sat_cart_interface();

	// reading from ROM
	virtual DECLARE_READ32_MEMBER(read_rom) { return 0xffffffff; }
	// reading and writing to Extended DRAM chips
	virtual DECLARE_READ32_MEMBER(read_ext_dram0) { return 0xffffffff; }
	virtual DECLARE_WRITE32_MEMBER(write_ext_dram0) {}
	virtual DECLARE_READ32_MEMBER(read_ext_dram1) { return 0xffffffff; }
	virtual DECLARE_WRITE32_MEMBER(write_ext_dram1) {}
	// reading and writing to Extended BRAM chip
	virtual DECLARE_READ32_MEMBER(read_ext_bram) { return 0xffffffff; }
	virtual DECLARE_WRITE32_MEMBER(write_ext_bram) {}

	virtual int get_cart_type() { return m_cart_type; };


	void rom_alloc(UINT32 size, const char *tag);
	void bram_alloc(UINT32 size);
	void dram0_alloc(UINT32 size);
	void dram1_alloc(UINT32 size);
	UINT32* get_rom_base() { return m_rom; }
	UINT32* get_ext_dram0_base() { return m_ext_dram0; }
	UINT32* get_ext_dram1_base() { return m_ext_dram1; }
	UINT8*  get_ext_bram_base() { return m_ext_bram; }
	UINT32  get_rom_size() { return m_rom_size; }
	UINT32  get_ext_dram0_size() { return m_ext_dram0.bytes(); }
	UINT32  get_ext_dram1_size() { return m_ext_dram1.bytes(); }
	UINT32  get_ext_bram_size() { return m_ext_bram.bytes(); }

protected:
	int m_cart_type;

	// internal state
	UINT32 *m_rom;
	UINT32 m_rom_size;
	dynamic_array<UINT32> m_ext_dram0;
	dynamic_array<UINT32> m_ext_dram1;
	dynamic_buffer m_ext_bram;
};


// ======================> sat_cart_slot_device

class sat_cart_slot_device : public device_t,
								public device_image_interface,
								public device_slot_interface
{
public:
	// construction/destruction
	sat_cart_slot_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	virtual ~sat_cart_slot_device();

	// device-level overrides
	virtual void device_start();
	virtual void device_config_complete();

	// image-level overrides
	virtual bool call_load();
	virtual void call_unload();
	virtual bool call_softlist_load(software_list_device &swlist, const char *swname, const rom_entry *start_entry);

	int get_cart_type();

	virtual iodevice_t image_type() const { return IO_CARTSLOT; }
	virtual bool is_readable()  const { return 1; }
	virtual bool is_writeable() const { return 0; }
	virtual bool is_creatable() const { return 0; }
	virtual bool must_be_loaded() const { return 0; }
	virtual bool is_reset_on_load() const { return 1; }
	virtual const option_guide *create_option_guide() const { return NULL; }
	virtual const char *image_interface() const { return "sat_cart"; }
	virtual const char *file_extensions() const { return "bin"; }

	// slot interface overrides
	virtual void get_default_card_software(astring &result);

	// reading and writing
	virtual DECLARE_READ32_MEMBER(read_rom);
	virtual DECLARE_READ32_MEMBER(read_ext_dram0);
	virtual DECLARE_WRITE32_MEMBER(write_ext_dram0);
	virtual DECLARE_READ32_MEMBER(read_ext_dram1);
	virtual DECLARE_WRITE32_MEMBER(write_ext_dram1);
	virtual DECLARE_READ32_MEMBER(read_ext_bram);
	virtual DECLARE_WRITE32_MEMBER(write_ext_bram);

private:
	device_sat_cart_interface*       m_cart;
};


// device type definition
extern const device_type SATURN_CART_SLOT;


/***************************************************************************
 DEVICE CONFIGURATION MACROS
 ***************************************************************************/

#define SATSLOT_ROM_REGION_TAG ":cart:rom"

#define MCFG_SATURN_CARTRIDGE_ADD(_tag,_slot_intf,_def_slot) \
	MCFG_DEVICE_ADD(_tag, SATURN_CART_SLOT, 0)  \
	MCFG_DEVICE_SLOT_INTERFACE(_slot_intf, _def_slot, false)


#endif
