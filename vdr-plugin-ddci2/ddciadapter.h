/////////////////////////////////////////////////////////////////////////////
//
// @file ddciadapter.h @brief Digital Devices Common Interface plugin for VDR.
//
// Copyright (c) 2013 - 2014 by Jasmin Jessich.  All Rights Reserved.
//
// Contributor(s):
//
// License: GPLv2
//
// This file is part of vdr_plugin_ddci2.
//
// vdr_plugin_ddci2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// vdr_plugin_ddci2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with vdr_plugin_ddci2.  If not, see <http://www.gnu.org/licenses/>.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __DDCIADAPTER_H
#define __DDCIADAPTER_H

#include "ddcitssend.h"
#include "ddcitsrecv.h"

#include <vdr/ci.h>

// forward declarations
class DdCiCamSlot;

/**
 * This class implements the physical interface to the CAM device.
 */
class DdCiAdapter: public cCiAdapter
{
private:
	int fd;             //< .../adapterX/caY device file handle
	cString caDevName;  //< .../adapterX/caY device path
	DdCiTsSend ciSend;  //< the CAM TS sender
	DdCiTsRecv ciRecv;  //< the CAM TS receiver

	// FIXME: after VDR base class change, this is not necessary
	DdCiCamSlot *camSlot;  //< the one and only slot of a DD CI adapter

	void CleanUp();

protected:
	/* see file ci.h in the VDR include directory for the description of
	 * the following functions
	 */
	virtual void Action();
	virtual int Read( uint8_t *Buffer, int MaxLength );
	virtual void Write( const uint8_t *Buffer, int Length );
	virtual bool Reset( int Slot );
	virtual eModuleStatus ModuleStatus( int Slot );
	virtual bool Assign( cDevice *Device, bool Query = false );

public:
	/**
	 * Constructor.
	 * Checks for the available slots of the CAM and starts the
	 * controlling thread.
	 * @param ca_fd the file handle for the .../adapterX/caY device
	 * @param ci_fdw the write file handle for the .../adapterX/ciY device
	 * @param ci_fdr the read file handle for the .../adapterX/ciY device
	 * @param devNameCa the name of the device (.../adapterX/caY)
	 * @param devNameCi the name of the device (.../adapterX/ciY)
	 **/
	DdCiAdapter( int ca_fd, int ci_fdw, int ci_fdr, cString &devNameCa, cString &devNameCi );

	/// Destructor.
	virtual ~DdCiAdapter();

	/**
	 * Deliver the received CAM TS Data to the receive buffer.
	 * @param data the received TS packet(s) from the CAM; it have to point to
	 *        the beginning of a packet (start with TS_SYNC_BYTE).
	 * @param count the number of bytes in data (shall be at least TS_SIZE).
	 * @return the number of bytes actually processed
	 */
	int DataRecv( uchar *data, int count );

	/// get the caX device name
	const char *GetCaDevName() { return caDevName; };

	/// clear the CAM send and receive buffer
	void ClrBuffers();

	/// stop the thread
	void Cancel( int waitSec = 0 );
};

#endif //__DDCIADAPTER_H
