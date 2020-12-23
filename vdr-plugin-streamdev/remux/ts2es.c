#include "remux/ts2es.h"
#include "server/streamer.h"
#include "common.h"
#include <vdr/device.h>

// from VDR's remux.c
#define MAXNONUSEFULDATA (10*1024*1024)

namespace Streamdev {

class cTS2ES: public ipack {
	friend void PutES(uint8_t *Buffer, int Size, void *Data);

private:
	cRingBufferLinear *m_ResultBuffer;

public:
	cTS2ES(cRingBufferLinear *ResultBuffer);
	~cTS2ES();

	void PutTSPacket(const uint8_t *Buffer);
};

void PutES(uint8_t *Buffer, int Size, void *Data) 
{
	cTS2ES *This = (cTS2ES*)Data;
	uint8_t payl = Buffer[8] + 9 + This->start - 1;
	int count = Size - payl;

	int n = This->m_ResultBuffer->Put(Buffer + payl, count);
	if (n != count)
		esyslog("ERROR: result buffer overflow, dropped %d out of %d byte", count - n, count);
	This->start = 1;
}

} // namespace Streamdev
using namespace Streamdev;

cTS2ES::cTS2ES(cRingBufferLinear *ResultBuffer) 
{
	m_ResultBuffer = ResultBuffer;

	init_ipack(this, IPACKS, PutES, 0);
	data = (void*)this;
}

cTS2ES::~cTS2ES() 
{
	free_ipack(this);
}

void cTS2ES::PutTSPacket(const uint8_t *Buffer) {
  if (!Buffer)
     return;

  if (Buffer[1] & 0x80) { // ts error
		// TODO
	}

  if (Buffer[1] & 0x40) { // payload start
		if (plength == MMAX_PLENGTH - 6) {
    	plength = found - 6;
      found = 0;
      send_ipack(this);
      reset_ipack(this);
    }
  }

	uint8_t off = 0;

  if (Buffer[3] & 0x20) {  // adaptation field?
		off = Buffer[4] + 1;
    if (off + 4 > TS_SIZE - 1)
      return;
  }

  instant_repack((uint8_t*)(Buffer + 4 + off), TS_SIZE - 4 - off, this);
}

cTS2ESRemux::cTS2ESRemux(int Pid):
		m_Pid(Pid),
		m_ResultBuffer(new cStreamdevBuffer(WRITERBUFSIZE, IPACKS)),
		m_Remux(new cTS2ES(m_ResultBuffer))
{
	m_ResultBuffer->SetTimeouts(100, 100);
}

cTS2ESRemux::~cTS2ESRemux() 
{
	delete m_Remux;
	delete m_ResultBuffer;
}

int cTS2ESRemux::Put(const uchar *Data, int Count) 
{
	int used = 0;

	// Make sure we are looking at a TS packet:

	while (Count > TS_SIZE) {
		if (Data[0] == TS_SYNC_BYTE && Data[TS_SIZE] == TS_SYNC_BYTE)
			break;
		Data++;
		Count--;
		used++;
	}

	if (used)
		esyslog("ERROR: skipped %d byte to sync on TS packet", used);

	// Convert incoming TS data into ES:

	for (int i = 0; i < Count; i += TS_SIZE) {
		if (Count - i < TS_SIZE)
			break;
		if (Data[i] != TS_SYNC_BYTE)
			break;
		if (m_ResultBuffer->Free() < 2 * IPACKS) {
			m_ResultBuffer->WaitForPut();
			break; // A cTS2ES might write one full packet and also a small rest
		}
		int pid = cTSRemux::GetPid(Data + i + 1);
		if (Data[i + 3] & 0x10) { // got payload
			if (m_Pid == pid)
				m_Remux->PutTSPacket(Data + i);
		}
		used += TS_SIZE;
	}

/*
  // Check if we're getting anywhere here:
  if (!synced && skipped >= 0) {
     if (skipped > MAXNONUSEFULDATA) {
        esyslog("ERROR: no useful data seen within %d byte of video stream", skipped);
        skipped = -1;
        if (exitOnFailure)
           cThread::EmergencyExit(true);
        }
     else
        skipped += used;
     }
*/

	return used;
}

