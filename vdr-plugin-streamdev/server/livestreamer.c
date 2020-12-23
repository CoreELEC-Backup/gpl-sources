#include <assert.h>

#include <libsi/section.h>
#include <libsi/descriptor.h>

#include "remux/ts2ps.h"
#include "remux/ts2pes.h"
#include "remux/ts2es.h"
#include "remux/extern.h"

#include <vdr/transfer.h>

#include "server/livestreamer.h"
#include "server/setup.h"
#include "common.h"

using namespace Streamdev;

// device occupied timeout to prevent VDR main loop to immediately switch back
// when streamdev switched the live TV channel.
// Note that there is still a gap between the GetDevice() and SetOccupied()
// calls where the VDR main loop could strike
#define STREAMDEVTUNETIMEOUT 5

// --- cStreamdevLiveReceiver -------------------------------------------------

class cStreamdevLiveReceiver: public cReceiver {
	friend class cStreamdevStreamer;

private:
	cStreamdevLiveStreamer *m_Streamer;

protected:
#if APIVERSNUM >= 20300
	virtual void Receive(const uchar *Data, int Length);
#else
	virtual void Receive(uchar *Data, int Length);
#endif

public:
	cStreamdevLiveReceiver(cStreamdevLiveStreamer *Streamer, const cChannel *Channel, int Priority, const int *Pids);
	virtual ~cStreamdevLiveReceiver();
};

cStreamdevLiveReceiver::cStreamdevLiveReceiver(cStreamdevLiveStreamer *Streamer, const cChannel *Channel, int Priority, const int *Pids):
		cReceiver(Channel, Priority),
		m_Streamer(Streamer)
{
		// clears all PIDs but channel remains set
		SetPids(NULL);
		AddPids(Pids);
}

cStreamdevLiveReceiver::~cStreamdevLiveReceiver() 
{
	Dprintf("Killing live receiver\n");
	Detach();
}

#if APIVERSNUM >= 20300
void cStreamdevLiveReceiver::Receive(const uchar *Data, int Length) {
#else
void cStreamdevLiveReceiver::Receive(uchar *Data, int Length) {
#endif
	m_Streamer->Receive(Data, Length);
}

// --- cStreamdevPatFilter ----------------------------------------------------

class cStreamdevPatFilter : public cFilter {
private:
	int    pmtPid;
	int    pmtSid;
	int    pmtVersion;
	uchar  tspat_buf[TS_SIZE];
	cStreamdevBuffer siBuffer;

	const cChannel *m_Channel;
	cStreamdevLiveStreamer *m_Streamer;

	virtual void Process(u_short Pid, u_char Tid, const u_char *Data, int Length);

	int GetPid(SI::PMT::Stream& stream);
public:
	cStreamdevPatFilter(cStreamdevLiveStreamer *Streamer, const cChannel *Channel);
	uchar* Get(int &Count) { return siBuffer.Get(Count); }
	void Del(int Count) { return siBuffer.Del(Count); }
};

cStreamdevPatFilter::cStreamdevPatFilter(cStreamdevLiveStreamer *Streamer, const cChannel *Channel): siBuffer(10 * TS_SIZE, TS_SIZE)
{
	Dprintf("cStreamdevPatFilter(\"%s\")\n", Channel->Name());
	assert(Streamer);
	m_Channel = Channel;
	m_Streamer = Streamer;
	pmtPid = 0;
	pmtSid = 0;
	pmtVersion = -1;
	Set(0x00, 0x00);  // PAT
	// initialize PAT buffer. Only some values are dynamic (see comments)
	memset(tspat_buf, 0xff, TS_SIZE);
	tspat_buf[0] = TS_SYNC_BYTE;          // Transport packet header sunchronization byte (1000011 = 0x47h)
	tspat_buf[1] = 0x40;                  // Set payload unit start indicator bit
	tspat_buf[2] = 0x0;                   // PID
	tspat_buf[3] = 0x10;                  // Set payload flag, DYNAMIC: Continuity counter
	tspat_buf[4] = 0x0;                   // SI pointer field
	tspat_buf[5] = 0x0;                   // PAT table id
	tspat_buf[6] = 0xb0;                  // Section syntax indicator bit and reserved bits set
	tspat_buf[7] = 12 + 1;                // Section length (12 bit): PAT_TABLE_LEN + 1
	tspat_buf[8] = 0;                     // DYNAMIC: Transport stream ID (bits 8-15)
	tspat_buf[9] = 0;                     // DYNAMIC: Transport stream ID (bits 0-7)
	tspat_buf[10] = 0xc0;                 // Reserved, DYNAMIC: Version number, DYNAMIC: Current next indicator
	tspat_buf[11] = 0x0;                  // Section number
	tspat_buf[12] = 0x0;                  // Last section number
	tspat_buf[13] = 0;                    // DYNAMIC: Program number (bits 8-15)
	tspat_buf[14] = 0;                    // DYNAMIC: Program number (bits 0-7)
	tspat_buf[15] = 0xe0;                 // Reserved, DYNAMIC: Network ID (bits 8-12)
	tspat_buf[16] = 0;                    // DYNAMIC: Network ID (bits 0-7)
	tspat_buf[17] = 0;                    // DYNAMIC: Checksum
	tspat_buf[18] = 0;                    // DYNAMIC: Checksum
	tspat_buf[19] = 0;                    // DYNAMIC: Checksum
	tspat_buf[20] = 0;                    // DYNAMIC: Checksum
}

static const char * const psStreamTypes[] = {
	"UNKNOWN", 
	"ISO/IEC 11172 Video", 
	"ISO/IEC 13818-2 Video", 
	"ISO/IEC 11172 Audio", 
	"ISO/IEC 13818-3 Audio",
	"ISO/IEC 13818-1 Privete sections", 
	"ISO/IEC 13818-1 Private PES data",
	"ISO/IEC 13512 MHEG", 
	"ISO/IEC 13818-1 Annex A DSM CC",
	"0x09",
	"ISO/IEC 13818-6 Multiprotocol encapsulation",
	"ISO/IEC 13818-6 DSM-CC U-N Messages",
	"ISO/IEC 13818-6 Stream Descriptors",
	"ISO/IEC 13818-6 Sections (any type, including private data)",
	"ISO/IEC 13818-1 auxiliary",
	"ISO/IEC 13818-7 Audio with ADTS transport sytax",
	"ISO/IEC 14496-2 Visual (MPEG-4)",
	"ISO/IEC 14496-3 Audio with LATM transport syntax",
	"0x12", "0x13", "0x14", "0x15", "0x16", "0x17", "0x18", "0x19", "0x1a",
	"ISO/IEC 14496-10 Video (MPEG-4 part 10/AVC, aka H.264)",
	"0x1c", "0x1d", "0x1e", "0x1f", "0x20", "0x21", "0x22","0x23",
	"HEVC aka H.265",
	"",
};

int cStreamdevPatFilter::GetPid(SI::PMT::Stream& stream)
{
	SI::Descriptor *d;

	if (!stream.getPid()) 
		return 0;

	switch (stream.getStreamType()) {
	case 0x01: // ISO/IEC 11172 Video
	case 0x02: // ISO/IEC 13818-2 Video
	case 0x03: // ISO/IEC 11172 Audio
	case 0x04: // ISO/IEC 13818-3 Audio
#if 0
	case 0x07: // ISO/IEC 13512 MHEG 
	case 0x08: // ISO/IEC 13818-1 Annex A  DSM CC
	case 0x0a: // ISO/IEC 13818-6 Multiprotocol encapsulation
	case 0x0b: // ISO/IEC 13818-6 DSM-CC U-N Messages
	case 0x0c: // ISO/IEC 13818-6 Stream Descriptors
	case 0x0d: // ISO/IEC 13818-6 Sections (any type, including private data)
	case 0x0e: // ISO/IEC 13818-1 auxiliary
#endif
	case 0x0f: // ISO/IEC 13818-7 Audio with ADTS transport syntax
	case 0x10: // ISO/IEC 14496-2 Visual (MPEG-4)
	case 0x11: // ISO/IEC 14496-3 Audio with LATM transport syntax
	case 0x1b: // ISO/IEC 14496-10 Video (MPEG-4 part 10/AVC, aka H.264)
	case 0x24: // HEVC aka H.265
		Dprintf("cStreamdevPatFilter PMT scanner adding PID %d (%s)\n",
			stream.getPid(), psStreamTypes[stream.getStreamType()]);
		return stream.getPid();
	case 0x05: // ISO/IEC 13818-1 private sections
	case 0x06: // ISO/IEC 13818-1 PES packets containing private data
		for (SI::Loop::Iterator it; (d = stream.streamDescriptors.getNext(it)); ) {
			switch (d->getDescriptorTag()) {
			case SI::AC3DescriptorTag:
			case SI::EnhancedAC3DescriptorTag:
				Dprintf("cStreamdevPatFilter PMT scanner: adding PID %d (%s) %s\n",
					stream.getPid(), psStreamTypes[stream.getStreamType()], "AC3");
				delete d;
				return stream.getPid();
			case SI::TeletextDescriptorTag:
				Dprintf("cStreamdevPatFilter PMT scanner: adding PID %d (%s) %s\n",
					stream.getPid(), psStreamTypes[stream.getStreamType()], "Teletext");
				delete d;
				return stream.getPid();
			case SI::SubtitlingDescriptorTag:
				Dprintf("cStreamdevPatFilter PMT scanner: adding PID %d (%s) %s\n",
					stream.getPid(), psStreamTypes[stream.getStreamType()], "DVBSUB");
				delete d;
				return stream.getPid();
			default:
				Dprintf("cStreamdevPatFilter PMT scanner: NOT adding PID %d (%s) %s\n",
					stream.getPid(), psStreamTypes[stream.getStreamType()], "UNKNOWN");
				break;
			}
			delete d;
		}
		break;
	default:
		/* This following section handles all the cases where the audio track 
		 * info is stored in PMT user info with stream id >= 0x80
		 * we check the registration format identifier to see if it 
		 * holds "AC-3"
		 */
		if (stream.getStreamType() >= 0x80) {
			bool found = false;
			for (SI::Loop::Iterator it; (d = stream.streamDescriptors.getNext(it)); ) {
				switch (d->getDescriptorTag()) {
				case SI::RegistrationDescriptorTag:
					/* unfortunately libsi does not implement RegistrationDescriptor */
					if (d->getLength() >= 4) {
						found = true;
						SI::CharArray rawdata = d->getData();
						if (/*rawdata[0] == 5 && rawdata[1] >= 4 && */
						    rawdata[2] == 'A' && rawdata[3] == 'C' &&
						    rawdata[4] == '-' && rawdata[5] == '3') {
							isyslog("cStreamdevPatFilter PMT scanner:"
								"Adding pid %d (type 0x%x) RegDesc len %d (%c%c%c%c)", 
								stream.getPid(), stream.getStreamType(), 
								d->getLength(), rawdata[2], rawdata[3], 
								rawdata[4], rawdata[5]);
							delete d;
							return stream.getPid();
						}
					}
					break;
				default: 
					break;
				}
				delete d;
			}
			if(!found) {
				isyslog("Adding pid %d (type 0x%x) RegDesc not found -> assume AC-3", 
					stream.getPid(), stream.getStreamType());
				return stream.getPid();
			}
		}
		Dprintf("cStreamdevPatFilter PMT scanner: NOT adding PID %d (%s) %s\n",
			stream.getPid(), psStreamTypes[stream.getStreamType()<0x1c?stream.getStreamType():0], "UNKNOWN");
		break;
	}
	return 0;
}

void cStreamdevPatFilter::Process(u_short Pid, u_char Tid, const u_char *Data, int Length)
{
	if (Pid == 0x00) {
		if (Tid == 0x00) {
			SI::PAT pat(Data, false);
			if (!pat.CheckCRCAndParse())
				return;
			SI::PAT::Association assoc;
			for (SI::Loop::Iterator it; pat.associationLoop.getNext(assoc, it); ) {
				if (!assoc.isNITPid()) {
#if APIVERSNUM >= 20300
					LOCK_CHANNELS_READ;
					const cChannel *Channel =  Channels->GetByServiceID(Source(), Transponder(), assoc.getServiceId());
#else
					const cChannel *Channel =  Channels.GetByServiceID(Source(), Transponder(), assoc.getServiceId());
#endif
					if (Channel && (Channel == m_Channel)) {
						int prevPmtPid = pmtPid;
						if (0 != (pmtPid = assoc.getPid())) {
							Dprintf("cStreamdevPatFilter: PMT pid for channel %s: %d\n", Channel->Name(), pmtPid);
							pmtSid = assoc.getServiceId();
							// repack PAT to TS frame and send to client
							int ts_id;
							unsigned int crc, i, len;
							uint8_t *tmp;
							static uint8_t ccounter = 0;
							ccounter = (ccounter + 1) % 16;
							ts_id = Channel->Tid();               // Get transport stream id of the channel
							tspat_buf[3] = 0x10 | ccounter;       // Set payload flag, Continuity counter
							tspat_buf[8] = (ts_id >> 8);          // Transport stream ID (bits 8-15)
							tspat_buf[9] = (ts_id & 0xff);        // Transport stream ID (bits 0-7)
							tspat_buf[10] = 0xc0 | ((pat.getVersionNumber() << 1) & 0x3e) |
								pat.getCurrentNextIndicator();// Version number, Current next indicator
							tspat_buf[13] = (pmtSid >> 8);        // Program number (bits 8-15)
							tspat_buf[14] = (pmtSid & 0xff);      // Program number (bits 0-7)
							tspat_buf[15] = 0xe0 | (pmtPid >> 8); // Network ID (bits 8-12)
							tspat_buf[16] = (pmtPid & 0xff);      // Network ID (bits 0-7)
							crc = 0xffffffff;
							len = 12;                             // PAT_TABLE_LEN
							tmp = &tspat_buf[4 + 1];              // TS_HDR_LEN + 1
							while (len--) {
								crc ^= *tmp++ << 24;
								for (i = 0; i < 8; i++)
									crc = (crc << 1) ^ ((crc & 0x80000000) ? 0x04c11db7 : 0); // CRC32POLY
							}
							tspat_buf[17] = crc >> 24 & 0xff;     // Checksum
							tspat_buf[18] = crc >> 16 & 0xff;     // Checksum
							tspat_buf[19] = crc >>  8 & 0xff;     // Checksum
							tspat_buf[20] = crc & 0xff;           // Checksum
							int written = siBuffer.PutTS(tspat_buf, TS_SIZE);
							if (written != TS_SIZE)
								siBuffer.ReportOverflow(TS_SIZE - written);
							if (pmtPid != prevPmtPid) {
								m_Streamer->SetPid(pmtPid, true);
								Add(pmtPid, 0x02);
								pmtVersion = -1;
							}
							return;
						}
					}
				}
			}
		}
	} else if (Pid == pmtPid && Tid == SI::TableIdPMT && Source() && Transponder()) {
		SI::PMT pmt(Data, false);
		if (!pmt.CheckCRCAndParse())
			return;
		if (pmt.getServiceId() != pmtSid)
			return; // skip broken PMT records
		if (pmtVersion != -1) {
			if (pmtVersion != pmt.getVersionNumber()) {
				Dprintf("cStreamdevPatFilter: PMT version changed, detaching all pids\n");
				cFilter::Del(pmtPid, 0x02);
				pmtPid = 0; // this triggers PAT scan
			}
			return;
		}
		pmtVersion = pmt.getVersionNumber();

		SI::PMT::Stream stream;
		int pids[MAXRECEIVEPIDS + 1], npids = 0;
		pids[npids++] = pmtPid;
#if 0
		pids[npids++] = 0x10;  // pid 0x10, tid 0x40: NIT 
#endif
		pids[npids++] = 0x11;  // pid 0x11, tid 0x42: SDT 
		pids[npids++] = 0x14;  // pid 0x14, tid 0x70: TDT 
		pids[npids++] = 0x12;  // pid 0x12, tid 0x4E...0x6F: EIT 
		for (SI::Loop::Iterator it; pmt.streamLoop.getNext(stream, it); )
			if (0 != (pids[npids] = GetPid(stream)) && npids < MAXRECEIVEPIDS)
				npids++;

		pids[npids] = 0;
		m_Streamer->SetPids(pmt.getPCRPid(), pids);
	}
}

// --- cStreamdevLiveStreamer -------------------------------------------------

cStreamdevLiveStreamer::cStreamdevLiveStreamer(const cServerConnection *Connection, const cChannel *Channel, int Priority, eStreamType StreamType, const int* Apid, const int *Dpid) :
		cStreamdevStreamer("streamdev-livestreaming", Connection),
		m_Priority(Priority),
		m_NumPids(0),
		m_Channel(Channel),
		m_Device(NULL),
		m_Receiver(NULL),
		m_PatFilter(NULL),
		m_SwitchLive(false)
{
		m_ReceiveBuffer = new cStreamdevBuffer(LIVEBUFSIZE, TS_SIZE *2, true, "streamdev-livestreamer"),
		m_ReceiveBuffer->SetTimeouts(0, 100);
		if (Priority == IDLEPRIORITY) {
			SetChannel(StreamType, Apid, Dpid);
		}
		else {
			m_Device = SwitchDevice(Channel, Priority);
			if (m_Device)
				SetChannel(StreamType, Apid, Dpid);
			memcpy(m_Caids,Channel->Caids(),sizeof(m_Caids));
		}
}

cStreamdevLiveStreamer::~cStreamdevLiveStreamer() 
{
	Dprintf("Desctructing Live streamer\n");
	Stop();
	DELETENULL(m_PatFilter);
	DELETENULL(m_Receiver);
	delete m_ReceiveBuffer;
}

bool cStreamdevLiveStreamer::HasPid(int Pid) 
{
	int idx;
	for (idx = 0; idx < m_NumPids; ++idx)
		if (m_Pids[idx] == Pid)
			return true;
	return false;
}

bool cStreamdevLiveStreamer::SetPid(int Pid, bool On) 
{
	int idx;

	if (Pid == 0)
		return true;
	
	if (On) {
		for (idx = 0; idx < m_NumPids; ++idx) {
			if (m_Pids[idx] == Pid)
				return true; // No change needed
		}

		if (m_NumPids == MAXRECEIVEPIDS) {
			esyslog("ERROR: Streamdev: No free slot to receive pid %d\n", Pid);
			return false;
		}

		m_Pids[m_NumPids++] = Pid;
		m_Pids[m_NumPids] = 0;
	} else {
		for (idx = 0; idx < m_NumPids; ++idx) {
			if (m_Pids[idx] == Pid) {
				--m_NumPids;
				memmove(&m_Pids[idx], &m_Pids[idx + 1], sizeof(int) * (m_NumPids - idx));
			}
		}
	}

	StartReceiver();
	return true;
}

bool cStreamdevLiveStreamer::SetPids(int Pid, const int *Pids1, const int *Pids2, const int *Pids3)
{
	m_NumPids = 0;

	if (Pid)
		m_Pids[m_NumPids++] = Pid;

	if (Pids1)
		for ( ; *Pids1 && m_NumPids < MAXRECEIVEPIDS; Pids1++)
			if (!HasPid(*Pids1))
				m_Pids[m_NumPids++] = *Pids1;

	if (Pids2)
		for ( ; *Pids2 && m_NumPids < MAXRECEIVEPIDS; Pids2++)
			if (!HasPid(*Pids2))
				m_Pids[m_NumPids++] = *Pids2;

	if (Pids3)
		for ( ; *Pids3 && m_NumPids < MAXRECEIVEPIDS; Pids3++)
			if (!HasPid(*Pids3))
				m_Pids[m_NumPids++] = *Pids3;

	if (m_NumPids >= MAXRECEIVEPIDS) {
		esyslog("ERROR: Streamdev: No free slot to receive pid %d\n", Pid);
		return false;
	}

	m_Pids[m_NumPids] = 0;
	StartReceiver();
	return true;
}

void cStreamdevLiveStreamer::SetPriority(int Priority)
{
	m_Priority = Priority;
#if VDRVERSNUM >= 20104
	cThreadLock ThreadLock(m_Device);
	if (m_Receiver)
		m_Receiver->SetPriority(Priority);
	else
#endif
		StartReceiver();
}

void cStreamdevLiveStreamer::GetSignal(int *DevNum, int *Strength, int *Quality) const
{
	if (m_Device) {
		*DevNum = m_Device->DeviceNumber() + 1;
		*Strength = m_Device->SignalStrength();
		*Quality = m_Device->SignalQuality();
	}
}

cString cStreamdevLiveStreamer::ToText() const
{
	if (m_Device && m_Channel) {
		return cString::sprintf("DVB%-2d %3d %s", m_Device->DeviceNumber() + 1, m_Channel->Number(), m_Channel->Name());
	}
        return cString("");
}

bool cStreamdevLiveStreamer::IsReceiving(void) const
{
	cThreadLock ThreadLock(m_Device);
	return m_Receiver && m_Receiver->IsAttached();
}

void cStreamdevLiveStreamer::StartReceiver(bool Force)
{
	if (m_NumPids > 0 || Force) {
		Dprintf("Creating Receiver to respect changed pids\n");
		cReceiver *current = m_Receiver;
		cThreadLock ThreadLock(m_Device);
		m_Receiver = new cStreamdevLiveReceiver(this, m_Channel, m_Priority, m_Pids);
		if (IsRunning())
			Attach();
		delete current;
	}
	else
		DELETENULL(m_Receiver);
}

bool cStreamdevLiveStreamer::SetChannel(eStreamType StreamType, const int* Apid, const int *Dpid) 
{
	Dprintf("Initializing Remuxer for full channel transfer\n");
	//printf("ca pid: %d\n", Channel->Ca());

	const int *Apids = Apid ? Apid : m_Channel->Apids();
	const int *Dpids = Dpid ? Dpid : m_Channel->Dpids();

	switch (StreamType) {
	case stES: 
		{
			int pid = ISRADIO(m_Channel) ? m_Channel->Apid(0) : m_Channel->Vpid();
			if (Apid && Apid[0])
				pid = Apid[0];
			else if (Dpid && Dpid[0])
				pid = Dpid[0];
			SetRemux(new cTS2ESRemux(pid));
			return SetPids(pid);
		}

	case stPES: 
		SetRemux(new cTS2PESRemux(m_Channel->Vpid(), Apids, Dpids, m_Channel->Spids()));
		return SetPids(m_Channel->Vpid(), Apids, Dpids, m_Channel->Spids());

#ifdef STREAMDEV_PS
	case stPS:  
		SetRemux(new cTS2PSRemux(m_Channel->Vpid(), Apids, Dpids, m_Channel->Spids()));
		return SetPids(m_Channel->Vpid(), Apids, Dpids, m_Channel->Spids());
#endif

	case stEXT:
		SetRemux(new cExternRemux(Connection(), m_Channel, Apids, Dpids));
		// fall through
	case stTS:
		// This should never happen, but ...
		if (m_PatFilter) {
			Detach();
			DELETENULL(m_PatFilter);
		}
		// Set pids from cChannel
		SetPids(m_Channel->Vpid(), Apids, Dpids, m_Channel->Spids());
		if (m_Channel->Vpid() != m_Channel->Ppid())
			SetPid(m_Channel->Ppid(), true);
		// Set pids from PMT
		m_PatFilter = new cStreamdevPatFilter(this, m_Channel);
		return true;

	case stTSPIDS:
		Dprintf("pid streaming mode\n");
		// No PIDs requested yet. Start receiver anyway to occupy device
		StartReceiver(true);
		return true;
	default:
		return false;
	}
}

#if APIVERSNUM >= 20300                 
void cStreamdevLiveStreamer::Receive(const uchar *Data, int Length)
#else
void cStreamdevLiveStreamer::Receive(uchar *Data, int Length)
#endif
{
	int p = m_ReceiveBuffer->PutTS(Data, Length);
	if (p != Length)
		m_ReceiveBuffer->ReportOverflow(Length - p);
}

void cStreamdevLiveStreamer::Action(void)
{
	if (StreamdevServerSetup.LiveBufferMs) {
		// wait for first data block
		int count = 0;
		while (Running()) {
			if (m_ReceiveBuffer->Get(count) != NULL) {
				cCondWait::SleepMs(StreamdevServerSetup.LiveBufferMs);
				break;
			}
		}
	}
	cStreamdevStreamer::Action();
}

int cStreamdevLiveStreamer::Put(const uchar *Data, int Count) 
{
	// insert si data
	if (m_PatFilter) {
		int siCount;
		uchar *siData = m_PatFilter->Get(siCount);
		if (siData) {
			siCount = cStreamdevStreamer::Put(siData, siCount);
			if (siCount)
				m_PatFilter->Del(siCount);
		}
	}
	return cStreamdevStreamer::Put(Data, Count);
}

void cStreamdevLiveStreamer::Attach(void) 
{ 
	Dprintf("cStreamdevLiveStreamer::Attach()\n");
	if (m_Device) {
		if (m_Receiver) {
			if (m_Receiver->IsAttached())
				m_Device->Detach(m_Receiver); 
			m_Device->AttachReceiver(m_Receiver); 
		}
		if (m_PatFilter) {
			m_Device->Detach(m_PatFilter); 
			m_Device->AttachFilter(m_PatFilter); 
		}
	}
}

void cStreamdevLiveStreamer::Detach(void) 
{ 
	Dprintf("cStreamdevLiveStreamer::Detach()\n");
	if (m_Device) {
		if (m_Receiver)
			m_Device->Detach(m_Receiver); 
		if (m_PatFilter)
			m_Device->Detach(m_PatFilter); 
	}
}

bool cStreamdevLiveStreamer::UsedByLiveTV(cDevice *device)
{
	return device == cTransferControl::ReceiverDevice() ||
		(device->IsPrimaryDevice() && device->HasDecoder() && !device->Replaying());
}

cDevice *cStreamdevLiveStreamer::SwitchDevice(const cChannel *Channel, int Priority) 
{
	cDevice *device = cDevice::GetDevice(Channel, Priority, false);
	if (!device) {
		dsyslog("streamdev: GetDevice failed for channel %d (%s) at priority %d (PrimaryDevice=%d, ActualDevice=%d)", Channel->Number(), Channel->Name(), Priority, cDevice::PrimaryDevice()->CardIndex(), cDevice::ActualDevice()->CardIndex());
	}
	else if (!device->IsTunedToTransponder(Channel) && UsedByLiveTV(device)) {
		// make sure VDR main loop doesn't switch back
		device->SetOccupied(STREAMDEVTUNETIMEOUT);
		if (device->SwitchChannel(Channel, false)) {
			// switched away live TV
			m_SwitchLive = true;
		}
		else {
			dsyslog("streamdev: SwitchChannel (live) failed for channel %d (%s) at priority %d (PrimaryDevice=%d, ActualDevice=%d, device=%d)", Channel->Number(), Channel->Name(), Priority, cDevice::PrimaryDevice()->CardIndex(), cDevice::ActualDevice()->CardIndex(), device->CardIndex());
			device->SetOccupied(0);
			device = NULL;
		}
	}
	else if (!device->SwitchChannel(Channel, false)) {
		dsyslog("streamdev: SwitchChannel failed for channel %d (%s) at priority %d (PrimaryDevice=%d, ActualDevice=%d, device=%d)", Channel->Number(), Channel->Name(), Priority, cDevice::PrimaryDevice()->CardIndex(), cDevice::ActualDevice()->CardIndex(), device->CardIndex());
		device = NULL;
	}
	return device;
}

bool cStreamdevLiveStreamer::ProvidesChannel(const cChannel *Channel, int Priority) 
{
	cDevice *device = cDevice::GetDevice(Channel, Priority, false, true);
	if (!device)
		dsyslog("streamdev: No device provides channel %d (%s) at priority %d", Channel->Number(), Channel->Name(), Priority);
	return device;
}

void cStreamdevLiveStreamer::ChannelChange(const cChannel *Channel)
{
	if (Running() && m_Device && m_Channel == Channel) {
		// Check whether the Caids actually changed
		// If not, no need to re-tune, probably just an Audio PID update
		if (!memcmp(m_Caids, Channel->Caids(), sizeof(m_Caids))) {
			dsyslog("streamdev: channel %d (%s) changed, but caids remained the same, not re-tuning", Channel->Number(), Channel->Name());
		}
		else {
			Detach();
			if (m_Device->SwitchChannel(m_Channel, false)) {
				Attach();
				dsyslog("streamdev: channel %d (%s) changed, re-tuned", Channel->Number(), Channel->Name());
				memcpy(m_Caids, Channel->Caids(), sizeof(m_Caids));
			}
			else
				isyslog("streamdev: failed to re-tune after channel %d (%s) changed", Channel->Number(), Channel->Name());
		}
	}
}

void cStreamdevLiveStreamer::MainThreadHook()
{
	if (!m_SwitchLive && Running() && m_Device && !m_Device->IsTunedToTransponder(m_Channel) && !IsReceiving()) {
		cDevice *dev = SwitchDevice(m_Channel, m_Priority);
		if (dev) {
			dsyslog("streamdev: Lost channel %d (%s) on device %d. Continuing on device %d.", m_Channel->Number(), m_Channel->Name(), m_Device->CardIndex(), dev->CardIndex());
			m_Device = dev;
			StartReceiver();
		}
		else {
			isyslog("streamdev: Lost channel %d (%s) on device %d.", m_Channel->Number(), m_Channel->Name(), m_Device->CardIndex());
			Stop();
		}
	}
	if (m_SwitchLive) {
		// switched away live TV. Try previous channel on other device first
#if APIVERSNUM >= 20300                 
		LOCK_CHANNELS_READ;
		if (!Channels->SwitchTo(cDevice::CurrentChannel())) {
			// switch to streamdev channel otherwise
			Channels->SwitchTo(m_Channel->Number());
#else
		if (!Channels.SwitchTo(cDevice::CurrentChannel())) {
			// switch to streamdev channel otherwise
			Channels.SwitchTo(m_Channel->Number());
#endif
			Skins.Message(mtInfo, tr("Streaming active"));
		}
		if (m_Device)
			m_Device->SetOccupied(0);
		m_SwitchLive = false;
	}
}

std::string cStreamdevLiveStreamer::Report(void) 
{
	std::string result;

	if (m_Device != NULL)
		result += (std::string)"+- Device is " + (const char*)itoa(m_Device->CardIndex()) + "\n";
	if (m_Receiver != NULL)
		result += "+- Receiver is allocated\n";
		
	result += "+- Pids are ";
	for (int i = 0; i < MAXRECEIVEPIDS; ++i) 
		if (m_Pids[i] != 0)
			result += (std::string)(const char*)itoa(m_Pids[i]) + ", ";
	result += "\n";
	return result;
}
