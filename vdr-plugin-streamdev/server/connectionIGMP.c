/*
 *  $Id: connectionIGMP.c,v 1.3 2010/08/03 10:46:41 schmirl Exp $
 */

#include <ctype.h>
 
#include "server/connectionIGMP.h"
#include "server/server.h"
#include "server/setup.h"
#include <vdr/channels.h>

cConnectionIGMP::cConnectionIGMP(const char* Name, int ClientPort, eStreamType StreamType) :
		cServerConnection(Name, SOCK_DGRAM),
		m_ClientPort(ClientPort),
		m_StreamType(StreamType),
		m_Channel(NULL)
{
}

cConnectionIGMP::~cConnectionIGMP() 
{
}

#if APIVERSNUM >= 20300
bool cConnectionIGMP::SetChannel(const cChannel *Channel, in_addr_t Dst)
#else
bool cConnectionIGMP::SetChannel(cChannel *Channel, in_addr_t Dst)
#endif
{
	if (Channel) {
		m_Channel = Channel;
		struct in_addr ip;
		ip.s_addr = Dst;
		if (Connect(inet_ntoa(ip), m_ClientPort))
			return true;
		else
			esyslog("streamdev-server IGMP: Connect failed: %m");
		return false;
	}
	else
		esyslog("streamdev-server IGMP: Channel not found");
	return false;
}

void cConnectionIGMP::Welcome()
{
	if (cStreamdevLiveStreamer::ProvidesChannel(m_Channel, StreamdevServerSetup.IGMPPriority)) {
		cStreamdevLiveStreamer * liveStreamer = new cStreamdevLiveStreamer(this, m_Channel, StreamdevServerSetup.IGMPPriority, m_StreamType);
		if (liveStreamer->GetDevice()) {
			SetStreamer(liveStreamer);
			if (!SetDSCP())
				LOG_ERROR_STR("unable to set DSCP sockopt");
			Dprintf("streamer start\n");
			liveStreamer->Start(this);
		}
		else {
			SetStreamer(NULL);
			delete liveStreamer;
			esyslog("streamdev-server IGMP: SetChannel failed");
		}
	}
	else
		esyslog("streamdev-server IGMP: SwitchDevice failed");
}

bool cConnectionIGMP::Close()
{
	if (Streamer())
		Streamer()->Stop();
	return cServerConnection::Close();
}

cString cConnectionIGMP::ToText(char Delimiter) const
{
	cString str = cServerConnection::ToText(Delimiter);
	return Streamer() ? cString::sprintf("%s%c%s", *str, Delimiter, *Streamer()->ToText()) : str;
}
