/***************************************************************************

  nubus_image.c - synthetic NuBus card to allow reading/writing "raw"
  HFS images, including floppy images (DD and HD) and vMac/Basilisk HDD
  volumes up to 256 MB in size.

***************************************************************************/

#include "emu.h"
#include "nubus_image.h"

#define IMAGE_ROM_REGION    "image_rom"
#define IMAGE_DISK0_TAG     "nb_disk"

#define MESSIMG_DISK_SECTOR_SIZE (512)


// messimg_disk_image_device

class messimg_disk_image_device :   public device_t,
								public device_image_interface
{
public:
	// construction/destruction
	messimg_disk_image_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// image-level overrides
	virtual iodevice_t image_type() const { return IO_QUICKLOAD; }

	virtual bool is_readable()  const { return 1; }
	virtual bool is_writeable() const { return 1; }
	virtual bool is_creatable() const { return 0; }
	virtual bool must_be_loaded() const { return 0; }
	virtual bool is_reset_on_load() const { return 0; }
	virtual const char *image_interface() const { return NULL; }
	virtual const char *file_extensions() const { return "img"; }
	virtual const option_guide *create_option_guide() const { return NULL; }

	virtual bool call_load();
	virtual void call_unload();

	protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();
	virtual void device_reset();
public:
	UINT32 m_size;
	UINT8 *m_data;
	bool m_ejected;
};


// device type definition
extern const device_type MESSIMG_DISK;

const device_type MESSIMG_DISK = &device_creator<messimg_disk_image_device>;

messimg_disk_image_device::messimg_disk_image_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, MESSIMG_DISK, "Mac image", tag, owner, clock, "messimg_disk_image", __FILE__),
		device_image_interface(mconfig, *this)
{
}

void messimg_disk_image_device::device_config_complete()
{
	update_names(MESSIMG_DISK, "disk", "disk");
};


/*-------------------------------------------------
    device start callback
-------------------------------------------------*/

void messimg_disk_image_device::device_start()
{
	m_data = (UINT8 *)NULL;

	if (exists() && fseek(0, SEEK_END) == 0)
	{
		m_size = (UINT32)ftell();
	}
}

bool messimg_disk_image_device::call_load()
{
	fseek(0, SEEK_END);
	m_size = (UINT32)ftell();
	if (m_size > (256*1024*1024))
	{
		printf("Mac image too large: must be 256MB or less!\n");
		m_size = 0;
		return IMAGE_INIT_FAIL;
	}

	m_data = (UINT8 *)auto_alloc_array_clear(machine(), UINT32, m_size/sizeof(UINT32));
	fseek(0, SEEK_SET);
	fread(m_data, m_size);
	m_ejected = false;

	return IMAGE_INIT_PASS;
}

void messimg_disk_image_device::call_unload()
{
	// TODO: track dirty sectors and only write those
	fseek(0, SEEK_SET);
	fwrite(m_data, m_size);
	m_size = 0;
	//free(m_data);
}

/*-------------------------------------------------
    device reset callback
-------------------------------------------------*/

void messimg_disk_image_device::device_reset()
{
}

MACHINE_CONFIG_FRAGMENT( image )
	MCFG_DEVICE_ADD(IMAGE_DISK0_TAG, MESSIMG_DISK, 0)
MACHINE_CONFIG_END

ROM_START( image )
	ROM_REGION(0x2000, IMAGE_ROM_REGION, 0)
	ROM_LOAD( "nb_fake.bin",  0x000000, 0x002000, CRC(9264bac5) SHA1(540c2ce3c90382b2da6e1e21182cdf8fc3f0c930) )
ROM_END

//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

const device_type NUBUS_IMAGE = &device_creator<nubus_image_device>;


//-------------------------------------------------
//  machine_config_additions - device-specific
//  machine configurations
//-------------------------------------------------

machine_config_constructor nubus_image_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( image );
}

//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const rom_entry *nubus_image_device::device_rom_region() const
{
	return ROM_NAME( image );
}

//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  nubus_image_device - constructor
//-------------------------------------------------

nubus_image_device::nubus_image_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
		device_t(mconfig, NUBUS_IMAGE, "Disk Image Pseudo-Card", tag, owner, clock, "nb_image", __FILE__),
		device_nubus_card_interface(mconfig, *this)
{
}

nubus_image_device::nubus_image_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source) :
		device_t(mconfig, type, name, tag, owner, clock, shortname, source),
		device_nubus_card_interface(mconfig, *this)
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void nubus_image_device::device_start()
{
	UINT32 slotspace;
	UINT32 superslotspace;

	// set_nubus_device makes m_slot valid
	set_nubus_device();
	install_declaration_rom(this, IMAGE_ROM_REGION);

	slotspace = get_slotspace();
	superslotspace = get_super_slotspace();

//  printf("[image %p] slotspace = %x, super = %x\n", this, slotspace, superslotspace);

	m_nubus->install_device(slotspace, slotspace+3, read32_delegate(FUNC(nubus_image_device::image_r), this), write32_delegate(FUNC(nubus_image_device::image_w), this));
	m_nubus->install_device(slotspace+4, slotspace+7, read32_delegate(FUNC(nubus_image_device::image_status_r), this), write32_delegate(FUNC(nubus_image_device::image_status_w), this));
	m_nubus->install_device(superslotspace, superslotspace+((256*1024*1024)-1), read32_delegate(FUNC(nubus_image_device::image_super_r), this), write32_delegate(FUNC(nubus_image_device::image_super_w), this));

	m_image = subdevice<messimg_disk_image_device>(IMAGE_DISK0_TAG);
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void nubus_image_device::device_reset()
{
}

WRITE32_MEMBER( nubus_image_device::image_status_w )
{
	m_image->m_ejected = true;
}

READ32_MEMBER( nubus_image_device::image_status_r )
{
	if(m_image->m_ejected) {
		return 0;
	}

	if(m_image->m_size) {
		return 1;
	}
	return 0;
}

WRITE32_MEMBER( nubus_image_device::image_w )
{
}

READ32_MEMBER( nubus_image_device::image_r )
{
	return m_image->m_size;
}

WRITE32_MEMBER( nubus_image_device::image_super_w )
{
	UINT32 *image = (UINT32*)m_image->m_data;
	data = ((data & 0xff) << 24) | ((data & 0xff00) << 8) | ((data & 0xff0000) >> 8) | ((data & 0xff000000) >> 24);
	mem_mask = ((mem_mask & 0xff) << 24) | ((mem_mask & 0xff00) << 8) | ((mem_mask & 0xff0000) >> 8) | ((mem_mask & 0xff000000) >> 24);

	COMBINE_DATA(&image[offset]);
}

READ32_MEMBER( nubus_image_device::image_super_r )
{
	UINT32 *image = (UINT32*)m_image->m_data;
	UINT32 data = image[offset];
	return ((data & 0xff) << 24) | ((data & 0xff00) << 8) | ((data & 0xff0000) >> 8) | ((data & 0xff000000) >> 24);
}
