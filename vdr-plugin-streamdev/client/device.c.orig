/*
 *  $Id: device.c,v 1.27 2010/08/18 10:26:55 schmirl Exp $
 */
 
#include "client/device.h"
#include "client/setup.h"
#include "client/filter.h"

#include "tools/select.h"

#include <vdr/config.h>
#include <vdr/channels.h>
#include <vdr/ringbuffer.h>
#include <vdr/eit.h>
#include <vdr/timers.h>

#include <time.h>
#include <iostream>

using namespace std;

#ifndef LIVEPRIORITY
#define LIVEPRIORITY 0
#endif

#ifndef TRANSFERPRIORITY
#define TRANSFERPRIORITY -1
#endif

#define VIDEOBUFSIZE MEGABYTE(3)

const cChannel *cStreamdevDevice::m_DenyChannel = NULL;

cStreamdevDevice::cStreamdevDevice(void) {
	m_Disabled = false;
	m_ClientSocket = new cClientSocket();
	m_Channel = NULL;
	m_TSBuffer = NULL;

	m_Filters = new cStreamdevFilters(m_ClientSocket);
	StartSectionHandler();
	isyslog("streamdev-client: got device number %d", CardIndex() + 1);

	m_Pids = 0;
}

cStreamdevDevice::~cStreamdevDevice() {
	Dprintf("Device gets destructed\n");

	Lock();
	m_Filters->SetConnection(-1);
	m_ClientSocket->Quit();
	m_ClientSocket->Reset();
	Unlock();

	Cancel(3);

	StopSectionHandler();
	DELETENULL(m_Filters);
	DELETENULL(m_TSBuffer);
	delete m_ClientSocket;
}

#if APIVERSNUM >= 10700
int cStreamdevDevice::NumProvidedSystems(void) const
{	return StreamdevClientSetup.NumProvidedSystems; }
#endif

bool cStreamdevDevice::ProvidesSource(int Source) const {
	Dprintf("ProvidesSource, Source=%d\n", Source);
	return true;
}

bool cStreamdevDevice::ProvidesTransponder(const cChannel *Channel) const
{
	Dprintf("ProvidesTransponder\n");
	return true;
}

#if APIVERSNUM >= 10722
bool cStreamdevDevice::IsTunedToTransponder(const cChannel *Channel) const
#else
bool cStreamdevDevice::IsTunedToTransponder(const cChannel *Channel)
#endif
{
	return m_ClientSocket->DataSocket(siLive) != NULL &&
			m_Channel != NULL &&
			Channel->Transponder() == m_Channel->Transponder();
}

const cChannel *cStreamdevDevice::GetCurrentlyTunedTransponder(void) const {
	if (m_ClientSocket->DataSocket(siLive) != NULL)
		return m_Channel;
	return NULL;
}

bool cStreamdevDevice::ProvidesChannel(const cChannel *Channel, int Priority, 
		bool *NeedsDetachReceivers) const {
	if (m_Disabled || Channel == m_DenyChannel)
		return false;

	Dprintf("ProvidesChannel, Channel=%s, Priority=%d, SocketPrio=%d\n", Channel->Name(), Priority, m_ClientSocket->Priority());

	if (StreamdevClientSetup.MinPriority <= StreamdevClientSetup.MaxPriority)
	{
		if (Priority < StreamdevClientSetup.MinPriority ||
				Priority > StreamdevClientSetup.MaxPriority)
			return false;
	}
	else
	{
		if (Priority < StreamdevClientSetup.MinPriority &&
				Priority > StreamdevClientSetup.MaxPriority)
			return false;
	}

	int newPrio = Priority;
	if (Priority == LIVEPRIORITY) {
		if (m_ClientSocket->ServerVersion() >= 100 || StreamdevClientSetup.LivePriority >= 0)
			newPrio = StreamdevClientSetup.LivePriority;
	}

#if APIVERSNUM >= 10725
	bool prio = Priority == IDLEPRIORITY || newPrio >= m_ClientSocket->Priority();
#else
	bool prio = Priority < 0 || newPrio > m_ClientSocket->Priority();
#endif
	bool res = prio;
	bool ndr = false;

#if APIVERSNUM >= 10722
	if (IsTunedToTransponder(Channel)) {
#else
	if (const_cast<cStreamdevDevice*>(this)->IsTunedToTransponder(Channel)) {
#endif
		if (Channel->Ca() < CA_ENCRYPTED_MIN ||
				(Channel->Vpid() && HasPid(Channel->Vpid())) ||
				(Channel->Apid(0) && HasPid(Channel->Apid(0))))
			res = true;
		else
			ndr = true;
	}
	else if (prio) {
		if (Priority == LIVEPRIORITY && m_ClientSocket->ServerVersion() >= 100)
			UpdatePriority(true);

		res = m_ClientSocket->ProvidesChannel(Channel, newPrio);
		ndr = Receiving();

		if (m_ClientSocket->ServerVersion() >= 100)
			UpdatePriority(false);
	}

	if (NeedsDetachReceivers)
		*NeedsDetachReceivers = ndr;
	Dprintf("prov res = %d, ndr = %d\n", res, ndr);
	return res;
}

bool cStreamdevDevice::SetChannelDevice(const cChannel *Channel, 
		bool LiveView) {
	bool res;
	Dprintf("SetChannelDevice Channel: %s, LiveView: %s\n", Channel->Name(),
			LiveView ? "true" : "false");
	LOCK_THREAD;

	if (LiveView)
		return false;

	if (Receiving() && IsTunedToTransponder(Channel) && (
			Channel->Ca() < CA_ENCRYPTED_MIN ||
			(Channel->Vpid() && HasPid(Channel->Vpid())) ||
			(Channel->Apid(0) && HasPid(Channel->Apid(0))))) {
		res = true;
	}
	else {
		DetachAllReceivers();
		m_Channel = Channel;
		// Old servers delete cStreamdevLiveStreamer in ABRT.
		// Delete it now or it will happen after we tuned to new channel
		if (m_ClientSocket->ServerVersion() < 100)
			CloseDvr();
		res = m_ClientSocket->SetChannelDevice(m_Channel);
	}
	Dprintf("setchanneldevice res=%d\n", res);
	return res;
}

bool cStreamdevDevice::SetPid(cPidHandle *Handle, int Type, bool On) {
	Dprintf("SetPid, Pid=%d, Type=%d, On=%d, used=%d\n", Handle->pid, Type, On, Handle->used);
	LOCK_THREAD;

	bool res = true; 
	if (Handle->pid && (On || !Handle->used)) {
		res = m_ClientSocket->SetPid(Handle->pid, On);

		m_Pids += (!res) ? 0 : On ? 1 : -1;
		if (m_Pids < 0) 
			m_Pids = 0;
	}
	return res;
}

bool cStreamdevDevice::OpenDvr(void) {
	Dprintf("OpenDvr\n");
	LOCK_THREAD;

	CloseDvr();
	if (m_ClientSocket->CreateDataConnection(siLive)) {
		m_TSBuffer = new cTSBuffer(*m_ClientSocket->DataSocket(siLive), MEGABYTE(2), CardIndex() + 1);
	}
	else {
		esyslog("cStreamdevDevice::OpenDvr(): DVR connection FAILED");
	}
	return m_TSBuffer != NULL;
}


void cStreamdevDevice::CloseDvr(void) {
	Dprintf("CloseDvr\n");
	LOCK_THREAD;

	m_ClientSocket->CloseDvr();
	DELETENULL(m_TSBuffer);
}

bool cStreamdevDevice::GetTSPacket(uchar *&Data) {
	if (m_TSBuffer) {
		Data = m_TSBuffer->Get();
#if 1 // TODO: this should be fixed in vdr cTSBuffer
		// simple disconnect detection
		static int m_TSFails = 0;
		if (!Data) {
			LOCK_THREAD;
			if(!m_ClientSocket->DataSocket(siLive)) {
				return false; // triggers CloseDvr() + OpenDvr() in cDevice
                        }
			cPoller Poller(*m_ClientSocket->DataSocket(siLive));
			errno = 0;
			if (Poller.Poll() && !errno) {
				char tmp[1];
				if (recv(*m_ClientSocket->DataSocket(siLive), tmp, 1, MSG_PEEK) == 0 && !errno) {
esyslog("cStreamDevice::GetTSPacket: GetChecked: NOTHING (%d)", m_TSFails);
					m_TSFails++; 
					if (m_TSFails > 10) {
						isyslog("cStreamdevDevice::GetTSPacket(): disconnected");
						m_Pids = 0;
						CloseDvr();
						m_TSFails = 0;
						return false;
					}
					return true;
				}
			}
			m_TSFails = 0;
		}
#endif
		return true;
	}
	return false;
}

int cStreamdevDevice::OpenFilter(u_short Pid, u_char Tid, u_char Mask) {
	Dprintf("OpenFilter\n");

	if (!StreamdevClientSetup.StreamFilters)
		return -1;


	if (!m_ClientSocket->DataSocket(siLiveFilter)) {
		if (m_ClientSocket->CreateDataConnection(siLiveFilter)) {
			m_Filters->SetConnection(*m_ClientSocket->DataSocket(siLiveFilter));
		} else {
			isyslog("cStreamdevDevice::OpenFilter: connect failed: %m");
			return -1;
		}
	}

	if (m_ClientSocket->SetFilter(Pid, Tid, Mask, true))
		return m_Filters->OpenFilter(Pid, Tid, Mask);

	return -1;
}

void cStreamdevDevice::CloseFilter(int Handle) {

	if(m_Filters)
		m_Filters->CloseFilter(Handle);
	else
		esyslog("cStreamdevDevice::CloseFilter called while m_Filters is null");
}

bool cStreamdevDevice::ReInit(bool Disable) {
	LOCK_THREAD;
	m_Disabled = Disable;
	m_Filters->SetConnection(-1);
	m_Pids = 0;
	m_ClientSocket->Quit();
	m_ClientSocket->Reset();
	//DELETENULL(m_TSBuffer);
	return true;
}

void cStreamdevDevice::UpdatePriority(bool SwitchingChannels) const {
	if (!m_Disabled) {
		//LOCK_THREAD;
		const_cast<cStreamdevDevice*>(this)->Lock();
		if (m_ClientSocket->SupportsPrio() && m_ClientSocket->DataSocket(siLive)) {
			int Priority = this->Priority();
			// override TRANSFERPRIORITY (-1) with live TV priority from setup
			if (Priority == TRANSFERPRIORITY && this == cDevice::ActualDevice()) {
				Priority = StreamdevClientSetup.LivePriority;
				// temporarily lower priority
				if (SwitchingChannels)
					Priority--;
				if (Priority < 0 && m_ClientSocket->ServerVersion() < 100)
					Priority = 0;
			}
			m_ClientSocket->SetPriority(Priority);
		}
		const_cast<cStreamdevDevice*>(this)->Unlock();
	}
}

cString cStreamdevDevice::DeviceName(void) const {
	return StreamdevClientSetup.RemoteIp;
}

cString cStreamdevDevice::DeviceType(void) const {
	static int dev = -1;
	static cString devType("STRDev");
	int d = -1;
	if (m_ClientSocket->DataSocket(siLive) != NULL)
		m_ClientSocket->GetSignal(NULL, NULL, &d);
	if (d != dev) {
		dev = d;
		devType = d < 0 ? "STRDev" : *cString::sprintf("STRD%2d", d);
	}
	return devType;
}

int cStreamdevDevice::SignalStrength(void) const {
	int strength = -1;
	if (m_ClientSocket->DataSocket(siLive) != NULL)
		m_ClientSocket->GetSignal(&strength, NULL, NULL);
	return strength;
}

int cStreamdevDevice::SignalQuality(void) const {
	int quality = -1;
	if (m_ClientSocket->DataSocket(siLive) != NULL)
		m_ClientSocket->GetSignal(NULL, &quality, NULL);
	return quality;
}

