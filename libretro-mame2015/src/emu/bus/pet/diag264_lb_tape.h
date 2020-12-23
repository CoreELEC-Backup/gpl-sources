// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Diag264 Cassette Loop Back Connector emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __DIAG264_CASSETTE_LOOPBACK__
#define __DIAG264_CASSETTE_LOOPBACK__

#include "emu.h"
#include "cass.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> diag264_cassette_loopback_device

class diag264_cassette_loopback_device :  public device_t,
											public device_pet_datassette_port_interface
{
public:
	// construction/destruction
	diag264_cassette_loopback_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

protected:
	// device-level overrides
	virtual void device_start();

	// device_pet_datassette_port_interface overrides
	virtual int datassette_read();
	virtual void datassette_write(int state);
	virtual int datassette_sense();
	virtual void datassette_motor(int state);

private:
	int m_read;
	int m_sense;
};


// device type definition
extern const device_type DIAG264_CASSETTE_LOOPBACK;



#endif
