/*
 *  $Id: connectionIGMP.h,v 1.1 2009/02/13 10:39:22 schmirl Exp $
 */
 
#ifndef VDR_STREAMDEV_SERVERS_CONNECTIONIGMP_H
#define VDR_STREAMDEV_SERVERS_CONNECTIONIGMP_H

#include "connection.h"
#include "server/livestreamer.h"

#include <tools/select.h>

#define MULTICAST_PRIV_MIN ((uint32_t) 0xefff0000)
#define MULTICAST_PRIV_MAX ((uint32_t) 0xeffffeff)

class cStreamdevLiveStreamer;

class cConnectionIGMP: public cServerConnection {
private:
	int                               m_ClientPort;
	eStreamType                       m_StreamType;
#if APIVERSNUM >= 20300
	const cChannel                   *m_Channel;
#else
	cChannel                         *m_Channel;
#endif

public:
	cConnectionIGMP(const char* Name, int ClientPort, eStreamType StreamType);
	virtual ~cConnectionIGMP();

#if APIVERSNUM >= 20300
	bool SetChannel(const cChannel *Channel, in_addr_t Dst);
#else
	bool SetChannel(cChannel *Channel, in_addr_t Dst);
#endif
	virtual void Welcome(void);
	virtual cString ToText(char Delimiter = ' ') const;

	/* Not used here */
	virtual bool Command(char *Cmd) { return false; }

	virtual bool Close(void);

	virtual bool Abort(void) const;
};

inline bool cConnectionIGMP::Abort(void) const
{
	return !IsOpen() || !Streamer() || Streamer()->Abort();
}

#endif // VDR_STREAMDEV_SERVERS_CONNECTIONIGMP_H
