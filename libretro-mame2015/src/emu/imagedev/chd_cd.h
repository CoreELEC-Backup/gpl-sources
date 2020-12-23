/*********************************************************************

    chd_cd.h

    Interface to the CHD CDROM code

*********************************************************************/

#ifndef CHD_CD_H
#define CHD_CD_H

#include "cdrom.h"

/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

// ======================> cdrom_image_device

class cdrom_image_device :  public device_t,
							public device_image_interface
{
public:
	// construction/destruction
	cdrom_image_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	cdrom_image_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source);
	virtual ~cdrom_image_device();

	static void static_set_interface(device_t &device, const char *_interface) { downcast<cdrom_image_device &>(device).m_interface = _interface; }

	// image-level overrides
	virtual bool call_load();
	virtual void call_unload();
	virtual bool call_softlist_load(software_list_device &swlist, const char *swname, const rom_entry *start_entry) { load_software_part_region(*this, swlist, swname, start_entry ); return TRUE; }

	virtual iodevice_t image_type() const { return IO_CDROM; }

	virtual bool is_readable()  const { return 1; }
	virtual bool is_writeable() const { return 0; }
	virtual bool is_creatable() const { return 0; }
	virtual bool must_be_loaded() const { return 0; }
	virtual bool is_reset_on_load() const { return 0; }
	virtual const char *image_interface() const { return m_interface; }
	virtual const char *file_extensions() const { return m_extension_list; }
	virtual const option_guide *create_option_guide() const;

	// specific implementation
	cdrom_file *get_cdrom_file() { return m_cdrom_handle; }
protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();
	virtual void device_stop();

	chd_file    m_self_chd;
	cdrom_file  *m_cdrom_handle;
	const char  *m_extension_list;
	const char  *m_interface;
};

// device type definition
extern const device_type CDROM;

/***************************************************************************
    DEVICE CONFIGURATION MACROS
***************************************************************************/


#define MCFG_CDROM_ADD(_tag) \
	MCFG_DEVICE_ADD(_tag, CDROM, 0)

#define MCFG_CDROM_INTERFACE(_interface)                         \
	cdrom_image_device::static_set_interface(*device, _interface);

#endif /* CHD_CD_H */
