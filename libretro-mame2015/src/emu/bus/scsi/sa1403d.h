// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Shugart SA1403D Winchester Disk Controller emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __SA1403D__
#define __SA1403D__

#include "scsihd.h"
#include "imagedev/harddriv.h"

class sa1403d_device  : public scsihd_device
{
public:
	// construction/destruction
	sa1403d_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual const rom_entry *device_rom_region() const;
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual ioport_constructor device_input_ports() const;

	virtual void ExecCommand();
	virtual void WriteData( UINT8 *data, int dataLength );
};


// device type definition
extern const device_type SA1403D;

#endif
