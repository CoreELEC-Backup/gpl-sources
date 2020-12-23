#include "remux/ts2ps.h"
#include "remux/ts2pes.h"
#include "remux/ts2es.h"
#include "remux/extern.h"

#include <vdr/ringbuffer.h>
#include "server/recstreamer.h"
#include "server/connection.h"
#include "common.h"

using namespace Streamdev;

// --- cStreamdevRecStreamer -------------------------------------------------

cStreamdevRecStreamer::cStreamdevRecStreamer(const cServerConnection *Connection, RecPlayer *RecPlayer, eStreamType StreamType, int64_t StartOffset, const int *Apid, const int *Dpid):
		cStreamdevStreamer("streamdev-recstreaming", Connection),
		m_RecPlayer(RecPlayer),
		m_StartOffset(StartOffset),
		m_From(0L)
{
	Dprintf("New rec streamer\n");
	m_To = (int64_t) m_RecPlayer->getLengthBytes() - StartOffset - 1;

	const cPatPmtParser *parser = RecPlayer->getPatPmtData();
	const int *Apids = Apid ? Apid : parser->Apids();
	const int *Dpids = Dpid ? Dpid : parser->Dpids();
	switch (StreamType) {
	case stES:
		{
			int pid = parser->Vpid();
			if (Apid && Apid[0])
				pid = Apid[0];
			else if (Dpid && Dpid[0])
				pid = Dpid[0];
			SetRemux(new cTS2ESRemux(pid));
		}
		break;
	case stPES:
		if (!m_RecPlayer->getCurrentRecording()->IsPesRecording())
			SetRemux(new cTS2PESRemux(parser->Vpid(), Apids, Dpids, parser->Spids()));
		break;
#ifdef STREAMDEV_PS
	case stPS:
		SetRemux(new cTS2PSRemux(parser->Vpid(), Apids, Dpids, parser->Spids()));
		break;
#endif
	case stEXT:
		SetRemux(new cExternRemux(Connection, parser, Apids, Dpids));
                break;
	default:
		break;
	}
}

cStreamdevRecStreamer::~cStreamdevRecStreamer() 
{
	Dprintf("Desctructing rec streamer\n");
	Stop();
}

int64_t cStreamdevRecStreamer::SetRange(int64_t &From, int64_t &To)
{
	int64_t l = (int64_t) GetLength();
	if (From < 0L) {
		From += l;
		if (From < 0L)
			From = 0L;
		To = l - 1;
	}
	else {
		if (To < 0L)
			To += l;
		else if (To >= l)
			To = l - 1;
		if (From > To) {
			// invalid range - return whole content
			From = 0L;
			To = l - 1;
		}
	}
	m_From = From;
	m_To = To;
	return m_To - m_From + 1;
}

uchar* cStreamdevRecStreamer::GetFromReceiver(int &Count)
{
	if (m_From <= m_To) {
		Count = (int) m_RecPlayer->getBlock(m_Buffer, m_StartOffset + m_From, sizeof(m_Buffer));
		return m_Buffer;
	}
	return NULL;
}

cString cStreamdevRecStreamer::ToText() const
{
	return "REPLAY";
}
