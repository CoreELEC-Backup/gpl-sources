/***************************************************************************

    cr589.h

    Matsushita CR589

    Copyright Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

***************************************************************************/

#pragma once

#ifndef __CR589_H__
#define __CR589_H__

#include "atapicdr.h"
#include "t10mmc.h"

class matsushita_cr589_device : public atapi_cdrom_device,
	public device_nvram_interface
{
public:
	matsushita_cr589_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	virtual void ExecCommand();
	virtual void WriteData( UINT8 *data, int dataLength );
	virtual void ReadData( UINT8 *data, int dataLength );

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	// device_nvram_interface overrides
	virtual void nvram_default();
	virtual void nvram_read(emu_file &file);
	virtual void nvram_write(emu_file &file);

private:
	int download;
	UINT8 buffer[ 65536 ];
	int bufferOffset;
};

// device type definition
extern const device_type CR589;

#endif
