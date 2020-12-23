// license:BSD-3-Clause
// copyright-holders:etabeta
/***********************************************************************************************************

    Interton Electronic VC 4000 cart emulation
    (through slot devices)

 ***********************************************************************************************************/


#include "emu.h"
#include "slot.h"

//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

const device_type VC4000_CART_SLOT = &device_creator<vc4000_cart_slot_device>;

//**************************************************************************
//    VC4000 Cartridges Interface
//**************************************************************************

//-------------------------------------------------
//  device_vc4000_cart_interface - constructor
//-------------------------------------------------

device_vc4000_cart_interface::device_vc4000_cart_interface(const machine_config &mconfig, device_t &device)
	: device_slot_card_interface(mconfig, device),
		m_rom(NULL),
		m_rom_size(0)
{
}


//-------------------------------------------------
//  ~device_vc4000_cart_interface - destructor
//-------------------------------------------------

device_vc4000_cart_interface::~device_vc4000_cart_interface()
{
}

//-------------------------------------------------
//  rom_alloc - alloc the space for the cart
//-------------------------------------------------

void device_vc4000_cart_interface::rom_alloc(UINT32 size, const char *tag)
{
	if (m_rom == NULL)
	{
		astring tempstring(tag);
		tempstring.cat(VC4000SLOT_ROM_REGION_TAG);
		m_rom = device().machine().memory().region_alloc(tempstring, size, 1, ENDIANNESS_LITTLE)->base();
		m_rom_size = size;
	}
}


//-------------------------------------------------
//  ram_alloc - alloc the space for the ram
//-------------------------------------------------

void device_vc4000_cart_interface::ram_alloc(UINT32 size)
{
	m_ram.resize(size);
}


//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  vc4000_cart_slot_device - constructor
//-------------------------------------------------
vc4000_cart_slot_device::vc4000_cart_slot_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
						device_t(mconfig, VC4000_CART_SLOT, "Interton VC 4000 Cartridge Slot", tag, owner, clock, "vc4000_cart_slot", __FILE__),
						device_image_interface(mconfig, *this),
						device_slot_interface(mconfig, *this),
						m_type(VC4000_STD)
{
}


//-------------------------------------------------
//  vc4000_cart_slot_device - destructor
//-------------------------------------------------

vc4000_cart_slot_device::~vc4000_cart_slot_device()
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void vc4000_cart_slot_device::device_start()
{
	m_cart = dynamic_cast<device_vc4000_cart_interface *>(get_card_device());
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void vc4000_cart_slot_device::device_config_complete()
{
	// set brief and instance name
	update_names();
}


//-------------------------------------------------
//  VC4000 PCB
//-------------------------------------------------

struct vc4000_slot
{
	int                     pcb_id;
	const char              *slot_option;
};

// Here, we take the feature attribute from .xml (i.e. the PCB name) and we assign a unique ID to it
static const vc4000_slot slot_list[] =
{
	{ VC4000_STD,     "std" },
	{ VC4000_ROM4K,   "rom4k" },
	{ VC4000_RAM1K,   "ram1k" },
	{ VC4000_CHESS2,  "chess2" }
};

static int vc4000_get_pcb_id(const char *slot)
{
	for (int i = 0; i < ARRAY_LENGTH(slot_list); i++)
	{
		if (!core_stricmp(slot_list[i].slot_option, slot))
			return slot_list[i].pcb_id;
	}

	return 0;
}

static const char *vc4000_get_slot(int type)
{
	for (int i = 0; i < ARRAY_LENGTH(slot_list); i++)
	{
		if (slot_list[i].pcb_id == type)
			return slot_list[i].slot_option;
	}

	return "std";
}


/*-------------------------------------------------
 call load
 -------------------------------------------------*/

bool vc4000_cart_slot_device::call_load()
{
	if (m_cart)
	{
		UINT32 size = (software_entry() == NULL) ? length() : get_software_region_length("rom");

		if (size > 0x1800)
		{
			seterror(IMAGE_ERROR_UNSPECIFIED, "Image extends beyond the expected size for a VC4000 cart");
			return IMAGE_INIT_FAIL;
		}

		m_cart->rom_alloc(size, tag());

		if (software_entry() == NULL)
			fread(m_cart->get_rom_base(), size);
		else
			memcpy(m_cart->get_rom_base(), get_software_region("rom"), size);

		if (software_entry() == NULL)
		{
			m_type = VC4000_STD;
			// attempt to identify the non-standard types
			if (size > 0x1000)  // 6k rom + 1k ram - Chess2 only
				m_type = VC4000_CHESS2;
			else if (size > 0x0800) // some 4k roms have 1k of mirrored ram (those who don't still work with RAM emulated luckily)
				m_type = VC4000_RAM1K;

			if (m_type == VC4000_RAM1K || m_type == VC4000_CHESS2)
				m_cart->ram_alloc(0x400);
		}
		else
		{
			const char *pcb_name = get_feature("slot");
			if (pcb_name)
				m_type = vc4000_get_pcb_id(pcb_name);

			if (get_software_region("ram"))
				m_cart->ram_alloc(get_software_region_length("ram"));
		}

		//printf("Type: %s\n", vc4000_get_slot(m_type));

		return IMAGE_INIT_PASS;
	}

	return IMAGE_INIT_PASS;
}


/*-------------------------------------------------
 call softlist load
 -------------------------------------------------*/

bool vc4000_cart_slot_device::call_softlist_load(software_list_device &swlist, const char *swname, const rom_entry *start_entry)
{
	load_software_part_region(*this, swlist, swname, start_entry);
	return TRUE;
}


/*-------------------------------------------------
 get default card software
 -------------------------------------------------*/

void vc4000_cart_slot_device::get_default_card_software(astring &result)
{
	if (open_image_file(mconfig().options()))
	{
		const char *slot_string = "std";
		UINT32 size = core_fsize(m_file);
		int type = VC4000_STD;

		// attempt to identify the non-standard types
		if (size > 0x1000)  // 6k rom + 1k ram - Chess2 only
			type = VC4000_CHESS2;
		else if (size > 0x0800) // some 4k roms have 1k of mirrored ram
			type = VC4000_RAM1K;

		slot_string = vc4000_get_slot(type);

		//printf("type: %s\n", slot_string);
		clear();

		result.cpy(slot_string);
		return;
	}

	software_get_default_slot(result, "std");
}

/*-------------------------------------------------
 read
 -------------------------------------------------*/

READ8_MEMBER(vc4000_cart_slot_device::read_rom)
{
	if (m_cart)
		return m_cart->read_rom(space, offset);
	else
		return 0xff;
}

/*-------------------------------------------------
 read
 -------------------------------------------------*/

READ8_MEMBER(vc4000_cart_slot_device::extra_rom)
{
	if (m_cart)
		return m_cart->extra_rom(space, offset);
	else
		return 0xff;
}

/*-------------------------------------------------
 read
 -------------------------------------------------*/

READ8_MEMBER(vc4000_cart_slot_device::read_ram)
{
	if (m_cart)
		return m_cart->read_ram(space, offset);
	else
		return 0xff;
}

/*-------------------------------------------------
 write
 -------------------------------------------------*/

WRITE8_MEMBER(vc4000_cart_slot_device::write_ram)
{
	if (m_cart)
		m_cart->write_ram(space, offset, data);
}
