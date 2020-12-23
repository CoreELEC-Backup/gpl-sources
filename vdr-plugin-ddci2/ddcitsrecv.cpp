/////////////////////////////////////////////////////////////////////////////
//
// @file ddcitsrecv.cpp @brief Digital Devices Common Interface plugin for VDR.
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
#include "ddcicommon.h"
#include "ddciadapter.h"
#include "logging.h"

#include <vdr/tools.h>

static const int CNT_REC_DBG_MAX = 100;

//------------------------------------------------------------------------

void DdCiTsRecv::CleanUp()
{
	LOG_FUNCTION_ENTER;

	if (fd != -1) {
		close( fd );
		fd = -1;
	}

	LOG_FUNCTION_EXIT;
}

//------------------------------------------------------------------------

DdCiTsRecv::DdCiTsRecv( DdCiAdapter &the_adapter, int ci_fdr, cString &devNameCi )
: cThread()
, adapter( the_adapter )
, fd( ci_fdr )
, ciDevName( devNameCi )
, rb( CalcRbBufSz(), BUF_MARGIN, STAT_DDCITSRECVBUF, "DDCI CAM Recv" )
, pkgCntR( 0 )
, pkgCntW( 0 )
, pkgCntRL( 0 )
, pkgCntWL( 0 )
, clear( false )
, retry( 0 )
, cntRecDbg( 0 )
, tsdeliver( *this, devNameCi )
{
	LOG_FUNCTION_ENTER;

	// don't use adapter in this function, unless you know what you are doing!

	SetDescription( "DDCI Recv (%s)", *ciDevName );
	L_DBG_M( LDM_D, "DdCiTsRecv for %s created", *ciDevName );

	LOG_FUNCTION_EXIT;
}

//------------------------------------------------------------------------

DdCiTsRecv::~DdCiTsRecv()
{
	LOG_FUNCTION_ENTER;

	Cancel( 3 );
	CleanUp();

	LOG_FUNCTION_EXIT;
}

//------------------------------------------------------------------------

bool DdCiTsRecv::Start()
{
	LOG_FUNCTION_ENTER;

	if (fd == -1) {
		L_ERR_LINE( "Invalid file handle" );
		return false;
	}

	LOG_FUNCTION_EXIT;

	return cThread::Start();
}

//------------------------------------------------------------------------

void DdCiTsRecv::Cancel( int waitSec )
{
	LOG_FUNCTION_ENTER;

	cThread::Cancel( waitSec );

	LOG_FUNCTION_EXIT;
}

//------------------------------------------------------------------------

void DdCiTsRecv::ClrBuffer()
{
	LOG_FUNCTION_ENTER;

	clear = true;

	LOG_FUNCTION_EXIT;
}

//------------------------------------------------------------------------

void DdCiTsRecv::Deliver()
{
	while (Running()) {
		if (clear) {
			DDCI_RB_CLR_MTX_LOCK( &mtxClear )
			rb.Clear();
			pkgCntW = 0;
			pkgCntR = 0;
			clear = false;
			retry = 0;
			cntRecDbg = 0;
		}

		int cnt = 0;
		uchar *data = rb.Get( cnt );
		if (!data || cnt < TS_SIZE)
			continue;

		int skipped;
		uchar *frame = CheckTsSync( data, cnt, skipped );
		if (skipped) {
			L_ERR_LINE( "skipped %d bytes to sync on start of TS packet", skipped );
			rb.Del( skipped );
			cnt -= skipped;
		}

		if (cnt < TS_SIZE)
			continue;

		/* write only whole packets, because CheckTsSync will fail in next round otherwise */
		cnt -= cnt % TS_SIZE;

		int written = adapter.DataRecv( frame, cnt );
		if (written != 0) {
			rb.Del( written );
			retry = 0;
			pkgCntR += written / TS_SIZE;
		} else {
			if (retry++ < 3) {
				/* The receive buffer of the adapter is full, so we need to wait a little bit. */
				cCondWait::SleepMs( CfgGetSleepTmo() );
			} else {
				L_ERR( "Can't write packet VDR CamSlot for CI adapter (%s)", adapter.GetCaDevName() );
				rb.Del( TS_SIZE );
				retry = 0;
			}
		}
	}
}

//------------------------------------------------------------------------

void DdCiTsRecv::Action()
{
	LOG_FUNCTION_ENTER;

	cPoller Poller( fd );

	if (!tsdeliver.Start()) {
		L_ERR_LINE( "Couldn't start deliver thread" );
		return;
	}

	rb.SetTimeouts( 0, CfgGetSleepTmo() );
	cTimeMs t( DBG_PKG_TMO );

	while (Running()) {
		if (Poller.Poll( CfgGetSleepTmo() )) {
			errno = 0;
			DDCI_RB_CLR_MTX_LOCK( &mtxClear )
			int r = rb.Read( fd );
			if ((r < 0) && FATALERRNO) {
				if (errno == EOVERFLOW)
					L_ERR_LINE( "Driver buffer overflow on file %s:%m", *ciDevName );
				else {
					L_ERROR();
					break;
				}
			}
			if (r > 0) {
				if (cntRecDbg < CNT_REC_DBG_MAX) {
					++cntRecDbg;
					L_DBG_M( LDM_CRW, "DdCiTsRecv for %s received data from CAM ###", *ciDevName );
				}
				pkgCntW += r / TS_SIZE;
			}
		}

		if (t.TimedOut()) {
			if ((pkgCntR != pkgCntRL) || (pkgCntW != pkgCntWL)) {
				L_DBG_M( LDM_CBS, "DdCiTsRecv for %s CAM buff wr(CAM ->):%d, rd:%d", *ciDevName, pkgCntW, pkgCntR );
				pkgCntRL = pkgCntR;
				pkgCntWL = pkgCntW;
			}
			t.Set(DBG_PKG_TMO);
		}
	}

	tsdeliver.Cancel( 3 );
	CleanUp();

	LOG_FUNCTION_EXIT;
}
