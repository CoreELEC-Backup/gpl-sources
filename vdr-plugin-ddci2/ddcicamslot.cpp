/////////////////////////////////////////////////////////////////////////////
//
// @file ddcicamslot.cpp @brief Digital Devices Common Interface plugin for VDR.
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

#include "ddcicamslot.h"
#include "ddciadapter.h"
#include "ddcitssend.h"
#include "ddci2.h"
#include "logging.h"

#include <vdr/remux.h>

static const int SCT_DBG_TMO = 2000;   // 2 seconds
static const int CNT_SCT_DBG_MAX = 20;


//------------------------------------------------------------------------

void DdCiCamSlot::StopIt()
{
	cMutexLock MutexLock(&mtxRun);
	active = false;
	clear_rBuffer = true;
	delivered = false;
	cntSctPkt = 0;
	cntSctClrPkt = 0;
	cntSctDbg = 0;
	timSctDbg.Set(SCT_DBG_TMO);

	theAdapter.ClrBuffers();
}

//------------------------------------------------------------------------

DdCiCamSlot::DdCiCamSlot( DdCiAdapter &adapter, DdCiTsSend &sendCi )
: cCamSlot( &adapter, true )
, theAdapter( adapter )
, ciSend( sendCi )
, clear_rBuffer( false )
, delivered( false )
, active( false )
, cntSctPkt( 0 )
, cntSctPktL( 0 )
, cntSctClrPkt( 0 )
, cntSctDbg( 0 )
{
	LOG_FUNCTION_ENTER;

#if DDCI_MTD
	MtdEnable();
#endif

	LOG_FUNCTION_EXIT;
}

//------------------------------------------------------------------------

DdCiCamSlot::~DdCiCamSlot()
{
	LOG_FUNCTION_ENTER;

	StopIt();

	LOG_FUNCTION_EXIT;
}

//------------------------------------------------------------------------

bool DdCiCamSlot::Reset()
{
	LOG_FUNCTION_ENTER;

	L_FUNC_NAME();

	bool ret = cCamSlot::Reset();
	if (ret)
		StopIt();

	LOG_FUNCTION_EXIT;

	return ret;
}

//------------------------------------------------------------------------

void DdCiCamSlot::StartDecrypting()
{
	LOG_FUNCTION_ENTER;

	L_FUNC_NAME();

	// to lock the processing against StopIt
	mtxRun.Lock();

	active = true;

	// need to unlock it before base class call to avoid deadlock
	mtxRun.Unlock();

#if DDCI_MTD
	if (!MtdActive())
#endif
		cCamSlot::StartDecrypting();

	LOG_FUNCTION_EXIT;
}

//------------------------------------------------------------------------

void DdCiCamSlot::StopDecrypting()
{
	LOG_FUNCTION_ENTER;

	L_FUNC_NAME();

	cCamSlot::StopDecrypting();

	StopIt();

	LOG_FUNCTION_EXIT;
}

//------------------------------------------------------------------------

uchar *DdCiCamSlot::Decrypt( uchar *Data, int &Count )
{
	/* Normally we would need to lock mtxRun. But because we only write the
	 * packet into a send buffer or ignore them during the state change, it is
	 * not worth to lock here.
	 */

	if (Data) {
#if DDCI_DECRYPT_MORE
		Count -= (Count % TS_SIZE);  // we write only whole TS frames
#else
		Count = TS_SIZE;
#endif
	}

	if (!(active || CfgIgnAct())) {
		return 0;
	}

	/*
	 * WRITE
	 */
	if (Data)
		Count = ciSend.Write( Data, Count );

#if DDCI_MTD
	/*
	 * MTD
	 *
	 * With MTD support active, decrypted TS packets are sent to the individual
	 * MTD CAM slots in DataRecv().
	 */

	if (MtdActive())
		return 0;
#endif

	/*
	 * READ
	 */

	/* Decrypt is called for each frame and we need to return the decoded
	 * frame. But there is no "I_have_the_frame_consumed" function, so the
	 * only chance we have is to delete now the last sent frame from the
	 * buffer.
	 */
	if (delivered) {
		rBuffer.Del( TS_SIZE );
		delivered = false;
	}

	if (clear_rBuffer) {
		DDCI_RB_CLR_MTX_LOCK( &mtxClear );
		rBuffer.Clear();
		clear_rBuffer = false;
	}

	int cnt = 0;
	uchar *data = rBuffer.Get( cnt );
	if (!data || (cnt < TS_SIZE)) {
		data = 0;
	}
	else {
		if (TsIsScrambled( data )) {
			++cntSctPkt;

			// remove the scrambling bit?
			if (CfgIsClrSct()) {
				data[3] &= ~TS_SCRAMBLING_CONTROL;
				++cntSctClrPkt;
			}
		}

		if ((cntSctPkt != cntSctPktL) && (cntSctDbg < CNT_SCT_DBG_MAX) && timSctDbg.TimedOut()) {
			cntSctPktL = cntSctPkt;
			++cntSctDbg;
			L_DBG_M( LDM_SCT, "DdCiCamSlot for %s got %d scrambled packets from CAM"
				   , ciSend.GetCiDevName(), cntSctPkt );
			L_DBG_M( LDM_SCT, "DdCiCamSlot for %s clr %d scrambling control bits"
				   , ciSend.GetCiDevName(), cntSctClrPkt );
			timSctDbg.Set(SCT_DBG_TMO);
		}

		delivered = true;
	}

	return data;
}

//------------------------------------------------------------------------

bool DdCiCamSlot::Inject(uchar *Data, int Count)
{
	return ciSend.WriteAll( Data, Count );
}

//------------------------------------------------------------------------

int DdCiCamSlot::DataRecv( uchar *data, int count )
{
	if (!(active || CfgIgnAct())) {
		return count;   // not active, eat all the data
	}

	int written;

#if DDCI_MTD
	if (MtdActive()) {
		written = MtdPutData(data, count);
	} else
#endif
	{
		DDCI_RB_CLR_MTX_LOCK( &mtxClear );
		int free = rBuffer.Free();
		free -= free % TS_SIZE;   // write only whole packets
		if (free >= TS_SIZE) {
			if (free < count)
				count = free;
			written = rBuffer.Put( data, count );
			if (written != count)
				L_ERR_LINE( "Couldn't write previously checked free data ?!?" );
		}
		else
			written = 0;
	}
	return written;
}
