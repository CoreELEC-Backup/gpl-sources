/////////////////////////////////////////////////////////////////////////////
//
// @file ddcitsrecvdeliver.cpp @brief Digital Devices Common Interface plugin for VDR.
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

#include "ddcitsrecv.h"
#include "logging.h"

//------------------------------------------------------------------------

DdCiTsRecvDeliver::DdCiTsRecvDeliver(  DdCiTsRecv &the_tsrecv, cString &devNameCi )
: cThread()
, tsrecv( the_tsrecv )
, ciDevName( devNameCi )
{
	LOG_FUNCTION_ENTER;

	// don't use tsrecv in this function, unless you know what you are doing!

	SetDescription( "DDCI Recv Deliver (%s)", *ciDevName );
	L_DBG( "DdCiTsRecvDeliver for %s created", *ciDevName );

	LOG_FUNCTION_EXIT;
}

//------------------------------------------------------------------------

DdCiTsRecvDeliver::~DdCiTsRecvDeliver()
{
	LOG_FUNCTION_ENTER;

	Cancel( 3 );

	LOG_FUNCTION_EXIT;
}

//------------------------------------------------------------------------

bool DdCiTsRecvDeliver::Start()
{
	LOG_FUNCTION_ENTER;

	LOG_FUNCTION_EXIT;

	return cThread::Start();
}

//------------------------------------------------------------------------

void DdCiTsRecvDeliver::Cancel( int waitSec )
{
	LOG_FUNCTION_ENTER;

	cThread::Cancel( waitSec );

	LOG_FUNCTION_EXIT;
}

//------------------------------------------------------------------------

void DdCiTsRecvDeliver::Action()
{
	LOG_FUNCTION_ENTER;

	while (Running()) {
		tsrecv.Deliver();
	}

	LOG_FUNCTION_EXIT;
}
