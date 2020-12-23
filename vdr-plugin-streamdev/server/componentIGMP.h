/*
 *  $Id: componentIGMP.h,v 1.1 2009/02/13 10:39:22 schmirl Exp $
 */
 
#ifndef VDR_STREAMDEV_IGMPSERVER_H
#define VDR_STREAMDEV_IGMPSERVER_H

#include <sys/time.h>
#include <time.h>
#include <vdr/thread.h>
#include "server/component.h"
#include "../common.h"

class cMulticastGroup;

class cComponentIGMP: public cServerComponent, public cThread {
private:
	char m_ReadBuffer[2048];
	cList<cMulticastGroup> m_Groups;
	in_addr_t m_BindIp;
	int m_MaxChannelNumber;
	struct timeval m_GeneralQueryTimer;
	int m_StartupQueryCount;
	bool m_Querier;
	cCondWait m_CondWait;

#if APIVERSNUM >= 20300
	cMulticastGroup* FindGroup(in_addr_t Group);
#else
	cMulticastGroup* FindGroup(in_addr_t Group) const;
#endif

	/* Add or remove local host to multicast group */
	bool IGMPMembership(in_addr_t Group, bool Add = true);
	void IGMPSendQuery(in_addr_t Group, int Timeout);

	cServerConnection* ProcessMessage(struct igmp *Igmp, in_addr_t Group, in_addr_t Sender);

	void IGMPStartGeneralQueryTimer();
	void IGMPStartOtherQuerierPresentTimer();
	void IGMPSendGeneralQuery();

	void IGMPStartTimer(cMulticastGroup* Group, in_addr_t Member);
	void IGMPStartV1HostTimer(cMulticastGroup* Group);
	void IGMPStartTimerAfterLeave(cMulticastGroup* Group, unsigned int MaxResponseTime);
	void IGMPStartRetransmitTimer(cMulticastGroup* Group);
	void IGMPClearRetransmitTimer(cMulticastGroup* Group);
	void IGMPSendGroupQuery(cMulticastGroup* Group);
	cServerConnection* IGMPStartMulticast(cMulticastGroup* Group);
	void IGMPStopMulticast(cMulticastGroup* Group);

	virtual void Action();

protected:
	virtual cServerConnection *NewClient(void);

public:
	virtual bool Initialize(void);
	virtual void Destruct(void);
	virtual cServerConnection* Accept(void);

	cComponentIGMP(void);
	~cComponentIGMP(void);
};

#endif // VDR_STREAMDEV_IGMPSERVER_H
