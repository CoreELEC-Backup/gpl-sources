#ifdef STREAMDEV_PS

#include "remux/ts2ps.h"
#include "server/streamer.h"
#include <vdr/channels.h>
#include <vdr/device.h>

namespace Streamdev {

class cTS2PS {
	friend void PutPES(uint8_t *Buffer, int Size, void *Data);

private:
	ipack m_Ipack;
	int m_Pid;
	cRingBufferLinear *m_ResultBuffer;

public:
	cTS2PS(cRingBufferLinear *ResultBuffer, int Pid, uint8_t AudioCid = 0x00);
	~cTS2PS();

	void PutTSPacket(const uint8_t *Buffer);

	int Pid(void) const { return m_Pid; }
};

void PutPES(uint8_t *Buffer, int Size, void *Data) 
{
	cTS2PS *This = (cTS2PS*)Data;
	int n = This->m_ResultBuffer->Put(Buffer, Size);
	if (n != Size)
		esyslog("ERROR: result buffer overflow, dropped %d out of %d byte", Size - n, Size);
}

} // namespace Streamdev
using namespace Streamdev;

cTS2PS::cTS2PS(cRingBufferLinear *ResultBuffer, int Pid, uint8_t AudioCid)
{
	m_ResultBuffer = ResultBuffer;
	m_Pid = Pid;

	init_ipack(&m_Ipack, IPACKS, PutPES, false);
	m_Ipack.cid = AudioCid;
	m_Ipack.data = (void*)this;
}

cTS2PS::~cTS2PS() 
{
	free_ipack(&m_Ipack);
}

void cTS2PS::PutTSPacket(const uint8_t *Buffer) 
{
  if (!Buffer)
     return;

  if (Buffer[1] & 0x80) { // ts error
		// TODO
	}

  if (Buffer[1] & 0x40) { // payload start
		if (m_Ipack.plength == MMAX_PLENGTH - 6 && m_Ipack.found > 6) {
    	m_Ipack.plength = m_Ipack.found - 6;
      m_Ipack.found = 0;
      send_ipack(&m_Ipack);
      reset_ipack(&m_Ipack);
    }
  }

	uint8_t off = 0;

  if (Buffer[3] & 0x20) {  // adaptation field?
		off = Buffer[4] + 1;
    if (off + 4 > TS_SIZE - 1)
      return;
  }

  instant_repack((uint8_t*)(Buffer + 4 + off), TS_SIZE - 4 - off, &m_Ipack);
}

cTS2PSRemux::cTS2PSRemux(int VPid, const int *APids, const int *DPids, const int *SPids):
		m_NumTracks(0),
		m_ResultBuffer(new cStreamdevBuffer(WRITERBUFSIZE, IPACKS)),
		m_ResultSkipped(0),
		m_Skipped(0),
		m_Synced(false),
		m_IsRadio(VPid == 0 || VPid == 1 || VPid == 0x1FFF)
{
	m_ResultBuffer->SetTimeouts(100, 100);

	if (VPid)
		m_Remux[m_NumTracks++] = new cTS2PS(m_ResultBuffer, VPid);
	if (APids) {
		int n = 0;
		while (*APids && m_NumTracks < MAXTRACKS && n < MAXAPIDS)
			m_Remux[m_NumTracks++] = new cTS2PS(m_ResultBuffer, *APids++, 0xC0 + n++);
	}
	if (DPids) {
		int n = 0;
		while (*DPids && m_NumTracks < MAXTRACKS && n < MAXDPIDS)
			m_Remux[m_NumTracks++] = new cTS2PS(m_ResultBuffer, *DPids++, 0x80 + n++);
	}
}

cTS2PSRemux::~cTS2PSRemux() {
	for (int i = 0; i < m_NumTracks; ++i)
		delete m_Remux[i];
	delete m_ResultBuffer;
}

int cTS2PSRemux::Put(const uchar *Data, int Count)
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
		esyslog("ERROR: m_Skipped %d byte to sync on TS packet", used);

	// Convert incoming TS data into multiplexed PS:

	for (int i = 0; i < Count; i += TS_SIZE) {
		if (Count - i < TS_SIZE)
			break;
		if (Data[i] != TS_SYNC_BYTE)
			break;
		if (m_ResultBuffer->Free() < 2 * IPACKS) {
			m_ResultBuffer->WaitForPut();
			break; // A cTS2PS might write one full packet and also a small rest
		}
		int pid = GetPid(Data + i + 1);
		if (Data[i + 3] & 0x10) { // got payload
			for (int t = 0; t < m_NumTracks; t++) {
				if (m_Remux[t]->Pid() == pid) {
					m_Remux[t]->PutTSPacket(Data + i);
					break;
				}
			}
		}
		used += TS_SIZE;
	}

	// Check if we're getting anywhere here:
	if (!m_Synced && m_Skipped >= 0)
		m_Skipped += used;

	return used;
}

uchar *cTS2PSRemux::Get(int &Count)
{
	// Remove any previously skipped data from the result buffer:

	if (m_ResultSkipped > 0) {
		m_ResultBuffer->Del(m_ResultSkipped);
		m_ResultSkipped = 0;
	}

	// Special VPID case to enable recording radio channels:
	if (m_IsRadio) {
		// Force syncing of radio channels to avoid "no useful data" error
		m_Synced = true;
		return m_ResultBuffer->Get(Count);
	}

	// Check for frame borders:
	Count = 0;
	uchar *resultData = NULL;
	int resultCount = 0;
	uchar *data = m_ResultBuffer->Get(resultCount);
	if (data) {
		for (int i = 0; i < resultCount - 3; i++) {
			if (data[i] == 0 && data[i + 1] == 0 && data[i + 2] == 1) {
				int l = 0;
				uchar StreamType = data[i + 3];
				if (VIDEO_STREAM_S <= StreamType && StreamType <= VIDEO_STREAM_E) {
					uchar pt = NO_PICTURE;
					l = ScanVideoPacket(data, resultCount, i, pt);
					if (l < 0)
						return resultData;
					if (pt != NO_PICTURE) {
						if (pt < I_FRAME || B_FRAME < pt) {
							esyslog("ERROR: unknown picture type '%d'", pt);
						}
						else if (!m_Synced) {
							if (pt == I_FRAME) {
								m_ResultSkipped = i; // will drop everything before this position
								SetBrokenLink(data + i, l);
								m_Synced = true;
							}
						}
						else if (Count)
							return resultData;
					}
				} else {
					l = GetPacketLength(data, resultCount, i);
					if (l < 0)
						return resultData;
				}
				if (m_Synced) {
					if (!Count)
						resultData = data + i;
					Count += l;
				} else
					m_ResultSkipped = i + l;
				if (l > 0)
					i += l - 1; // the loop increments, too
			}
		}
	}
	return resultData;
}

#endif
