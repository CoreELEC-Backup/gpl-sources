/////////////////////////////////////////////////////////////////////////////
//
// @file ddcicamslot.h @brief Digital Devices Common Interface plugin for VDR.
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

#ifndef __DDCICAMSLOT_H
#define __DDCICAMSLOT_H

#include "ddcirecvbuf.h"

#include <vdr/ci.h>

// forward declarations
class DdCiAdapter;
class DdCiTsSend;

/**
 * This class implements the logical interface to one slot of the CAM device.
 */
class DdCiCamSlot: public cCamSlot
{
private:
	DdCiAdapter &theAdapter; //< the adapter of this CAM slot
	cMutex mtxRun;           //< the synchronization mutex for Start/StopDecrypting
	DdCiTsSend &ciSend;      //< the CAM TS sender
	DdCiRecvBuf rBuffer;     //< the receive buffer
	bool clear_rBuffer;      //< true, when the receive buffer shall be cleared
	bool delivered;          //< true, if Decrypt did deliver data at last call
	bool active;             //< true, if this slot does decrypting
	int cntSctPkt;           //< number of scrambled packets got from CAM
	int cntSctPktL;          //< number of scrambled packets got from CAM last
	int cntSctClrPkt;        //< number of cleared scrambling control bits
	int cntSctDbg;           //< counter for scrambling control debugging
	cTimeMs timSctDbg;       //< timer for scrambling control debugging
	DDCI_RB_CLR_MTX_DECL(mtxClear);

	void StopIt();

public:
	/**
	 * Constructor.
	 * Creates a new CAM slot for the given adapter.
	 * The adapter will take care of deleting the CAM slot, so the caller must
	 * not delete it!
	 * @param adapter the CAM adapter this slot is associated to
	 * @param the buffer for the TS packets
	 **/
	DdCiCamSlot(DdCiAdapter &adapter, DdCiTsSend &sendCi);

	/// Destructor.
	virtual ~DdCiCamSlot();

	/* see file ci.h in the VDR include directory for the description of
	 * the following functions
	 */

	/*
	virtual eModuleStatus ModuleStatus(void);
	virtual const char *GetCamName(void);
	virtual bool Ready(void);
	virtual bool HasMMI(void);
	virtual bool HasUserIO(void);
	virtual bool EnterMenu(void);
	virtual cCiMenu *GetMenu(void);
	virtual cCiEnquiry *GetEnquiry(void);
	virtual bool ProvidesCa(const int *CaSystemIds);
	virtual void AddPid(int ProgramNumber, int Pid, int StreamType);
	virtual void SetPid(int Pid, bool Active);
	virtual void AddChannel(const cChannel *Channel);
	virtual bool CanDecrypt(const cChannel *Channel);
	virtual bool IsDecrypting(void);
	*/

	virtual bool Reset(void);
	virtual void StartDecrypting(void);
	virtual void StopDecrypting(void);

	/**
	 * For a detailed description have a look to file ci.h in the VDR include
	 * directory.
	 * This function will copy the given TS packet(s) to the CAM TS send
	 * buffer and return the next decrypted TS packet from the CAM TS
	 * receive buffer.
	 *
	 * @param Data the TS packet(s) to decrypt
	 * @param Count the number of bytes in Data (should be a multiple of
	 *        TS_SIZE). On function return it is set to the number of bytes
	 *        consumed from Data. 0 in case the CAM send buffer is full.
	 * @return A pointer to the first TS packet in the CAM receive buffer, or
	 *        0, if the CAM receive buffer buffer is empty.
	 **/
	virtual uchar *Decrypt(uchar *Data, int &Count);

	/**
	 * For a detailed description have a look to file ci.h in the VDR include
	 * directory.
	 * This function will copy the given TS packet to the CAM TS send
	 * buffer and return true if this was possible or false. In the latter case
	 * nothing is copied at all.
	 * @param Data the TS packet(s) to sent to the CAM; it have to point to the
	 *        beginning of a packet (start with TS_SYNC_BYTE).
	 * @param count the number of bytes in data (shall be at least TS_SIZE);
	 *        have to be a multiple of TS_SIZE.
	 * @return true ... Data copied
	 *         false .. Nothing written to send buffer
	 */
	virtual bool Inject(uchar *Data, int Count);

	/**
	 * Deliver the received CAM TS Data to the CAM slot.
	 * @param data the received TS packet(s) from the CAM; it have to point to
	 *        the beginning of a packet (start with TS_SYNC_BYTE).
	 * @param count the number of bytes in data (shall be at least TS_SIZE).
	 * @return the number of bytes actually processed
	 */
	int DataRecv( uchar *data, int count );
};

#endif //__DDCICAMSLOT_H
