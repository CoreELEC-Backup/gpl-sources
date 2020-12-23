/*********************************************************************

    a2diskiing.h

    Apple II Disk II Controller Card, new floppy

*********************************************************************/

#ifndef __A2BUS_DISKIING__
#define __A2BUS_DISKIING__

#include "emu.h"
#include "a2bus.h"
#include "imagedev/floppy.h"
#include "formats/flopimg.h"
#include "machine/wozfdc.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************


class a2bus_diskiing_device:
	public device_t,
	public device_a2bus_card_interface
{
public:
	// construction/destruction
	a2bus_diskiing_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual const rom_entry *device_rom_region() const;

	DECLARE_FLOPPY_FORMATS( floppy_formats );

protected:
	virtual void device_start();
	virtual void device_reset();

	// overrides of standard a2bus slot functions
	virtual UINT8 read_c0nx(address_space &space, UINT8 offset);
	virtual void write_c0nx(address_space &space, UINT8 offset, UINT8 data);
	virtual UINT8 read_cnxx(address_space &space, UINT8 offset);

private:
	required_device<diskii_fdc> m_wozfdc;
	required_device<floppy_connector> floppy0;
	required_device<floppy_connector> floppy1;

	const UINT8 *m_rom;
};

// device type definition
extern const device_type A2BUS_DISKIING;

#endif  /* __A2BUS_DISKIING__ */
