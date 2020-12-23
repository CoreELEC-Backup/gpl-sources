/////////////////////////////////////////////////////////////////////////////
//
// @file ddcitssend.cpp @brief Digital Devices Common Interface plugin for VDR.
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

#include "ddcitssend.h"
#include "ddcicommon.h"
#include "ddciadapter.h"
#include "logging.h"

#include <vdr/tools.h>

static const int CNT_SND_DBG_MAX = 100;

//------------------------------------------------------------------------

void DdCiTsSend::CleanUp()
{
	LOG_FUNCTION_ENTER;

	if (fd != -1) {
		close( fd );
		fd = -1;
	}

	LOG_FUNCTION_EXIT;
}

//------------------------------------------------------------------------

bool DdCiTsSend::PutAndCheck(const uchar *data, int &count)
{
	bool ret = true;

	int written = rb.Put( data, count );
	pkgCntW += written / TS_SIZE;
	if (written != count) {
		L_ERR_LINE( "Couldn't write previously checked free data ?!?" );
		ret = false;
	}
	count = written;
	return ret;
}

//------------------------------------------------------------------------

DdCiTsSend::DdCiTsSend( DdCiAdapter &the_adapter, int ci_fdw, cString &devNameCi )
: cThread()
, adapter( the_adapter )
, fd( ci_fdw )
, ciDevName( devNameCi )
, rb( CalcRbBufSz(), BUF_MARGIN, STAT_DDCITSSENDBUF, "DDCI CAM Send" )
, pkgCntR( 0 )
, pkgCntW( 0 )
, pkgCntRL( 0 )
, pkgCntWL( 0 )
, clear( false )
, cntSndDbg( 0 )
{
	LOG_FUNCTION_ENTER;

	// don't use adapter in this function,, unless you know what you are doing!

	SetDescription( "DDCI Send (%s)", *ciDevName );
	L_DBG_M( LDM_D, "DdCiTsSend for %s created", *ciDevName );

	LOG_FUNCTION_EXIT;
}

//------------------------------------------------------------------------

DdCiTsSend::~DdCiTsSend()
{
	LOG_FUNCTION_ENTER;

	Cancel( 3 );
	CleanUp();

	LOG_FUNCTION_EXIT;
}

//------------------------------------------------------------------------

bool DdCiTsSend::Start()
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

void DdCiTsSend::Cancel( int waitSec )
{
	LOG_FUNCTION_ENTER;

	cThread::Cancel( waitSec );

	LOG_FUNCTION_EXIT;
}

//------------------------------------------------------------------------

void DdCiTsSend::ClrBuffer()
{
	LOG_FUNCTION_ENTER;

	clear = true;

	LOG_FUNCTION_EXIT;
}

//------------------------------------------------------------------------

int DdCiTsSend::Write( const uchar *data, int count )
{
	cMutexLock MutexLockW( &mtxWrite );

	DDCI_RB_CLR_MTX_LOCK( &mtxClear )
	int free = rb.Free();
	if (free > count)
		free = count;
	free -= free % TS_SIZE;  // only whole TS frames must be written
	if (free > 0)
		PutAndCheck( data, free );

	return free;
}

//------------------------------------------------------------------------

bool DdCiTsSend::WriteAll( const uchar *data, int count )
{
	cMutexLock MutexLockW( &mtxWrite );

	if (count % TS_SIZE)  // have to be a multiple of TS_SIZE
		return false;

	DDCI_RB_CLR_MTX_LOCK( &mtxClear )
	int free = rb.Free();
	if (free < count)  // all the packets need to be written at once
		return false;

	return PutAndCheck( data, count );
}

//------------------------------------------------------------------------

void DdCiTsSend::Action()
{
	const int run_check_tmo = CfgGetSleepTmo();

	LOG_FUNCTION_ENTER;

	rb.SetTimeouts( 0, run_check_tmo );
	cTimeMs t (DBG_PKG_TMO);

	while (Running()) {
		if (clear) {
			DDCI_RB_CLR_MTX_LOCK( &mtxClear )
			rb.Clear();
			pkgCntW = 0;
			pkgCntR = 0;
			clear = false;
			cntSndDbg = 0;
		}

		int cnt = 0;
		uchar *data = rb.Get( cnt );
		if (data && cnt >= TS_SIZE) {
			int skipped;
			uchar *frame = CheckTsSync( data, cnt, skipped );
			if (skipped) {
				L_ERR_LINE( "skipped %d bytes to sync on start of TS packet", skipped );
				rb.Del( skipped );
			}

			int len = cnt - skipped;
			len -= (len % TS_SIZE);     // only whole TS frames must be written
			if (len >= TS_SIZE) {
				int w = WriteAllOrNothing( fd, frame, len, 5 * run_check_tmo, run_check_tmo );
				if (w >= 0) {
					int remain = len - w;
					if (remain > 0) {
						L_ERR_LINE( "couldn't write all data to CAM %s", *ciDevName );
						len -= remain;
					}
					if (cntSndDbg < CNT_SND_DBG_MAX) {
						++cntSndDbg;
						L_DBG_M( LDM_CRW, "DdCiTsSend for %s wrote data to CAM ###", *ciDevName );
					}
				} else {
					L_ERR_LINE( "couldn't write to CAM %s:%m", *ciDevName );
					break;
				}
				rb.Del( w );
				pkgCntR += w / TS_SIZE;
			}
		}

		if (t.TimedOut()) {
			if ((pkgCntR != pkgCntRL) || (pkgCntW != pkgCntWL)) {
				L_DBG_M( LDM_CBS, "DdCiTsSend for %s CAM buff rd(-> CAM):%d, wr:%d", *ciDevName, pkgCntR, pkgCntW );
				pkgCntRL = pkgCntR;
				pkgCntWL = pkgCntW;
			}
			t.Set(DBG_PKG_TMO);
		}
	}

	CleanUp();

	LOG_FUNCTION_EXIT;
}
