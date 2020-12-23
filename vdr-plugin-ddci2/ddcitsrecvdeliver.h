/////////////////////////////////////////////////////////////////////////////
//
// @file ddcitsrecvdeliver.h @brief Digital Devices Common Interface plugin for VDR.
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

#ifndef __DDCITSRECVDELIVER_H
#define __DDCITSRECVDELIVER_H

#include <vdr/thread.h>
#include <vdr/tools.h>

// forward declarations
class DdCiTsRecv;

/**
 * This class implements the deliver thread for the received CAM TS data.
 */
class DdCiTsRecvDeliver: public cThread
{
private:
	DdCiTsRecv &tsrecv;  //< the CAM TS receiver with the buffer
	cString ciDevName;   //< .../adapterX/ciY device path

public:
	/**
	 * Constructor.
	 * Creates a new CAM TS deliver thread object.
	 * @param the_tsrecv the
	 * @param devNameCi the name of the device (.../adapterX/ciY)
	 **/
	DdCiTsRecvDeliver( DdCiTsRecv &the_tsrecv, cString &devNameCi );

	/// Destructor.
	virtual ~DdCiTsRecvDeliver();

	bool Start();
	void Cancel( int waitSec = 0 );

	/**
	 * Executes the CAM TS receiver deliver function in a loop.
	 **/
	virtual void Action();
};

#endif //__DDCITSRECVDELIVER_H
