/*
 *  $Id: device.h,v 1.10 2010/08/18 10:26:55 schmirl Exp $
 */
 
#ifndef VDR_STREAMDEV_DEVICE_H
#define VDR_STREAMDEV_DEVICE_H

#include <vdr/device.h>

#include "client/socket.h"
#include "client/filter.h"

class cTBString;

#define CMD_LOCK_OBJ(x) cMutexLock CmdLock((cMutex*)&(x)->m_Mutex)

class cStreamdevDevice: public cDevice {

private:
	bool                 m_Disabled;
	cClientSocket       *m_ClientSocket;
	const cChannel      *m_Channel;
	cTSBuffer           *m_TSBuffer;
	cStreamdevFilters   *m_Filters;
	int                  m_Pids;

	static const cChannel   *m_DenyChannel;

protected:
	virtual bool SetChannelDevice(const cChannel *Channel, bool LiveView);
#if APIVERSNUM >= 10738
	virtual bool HasLock(int TimeoutMs) const
#else
	virtual bool HasLock(int TimeoutMs)
#endif
	{
		//printf("HasLock is %d\n", (ClientSocket.DataSocket(siLive) != NULL));
		//return ClientSocket.DataSocket(siLive) != NULL;
		return true;
	}

	virtual bool SetPid(cPidHandle *Handle, int Type, bool On);
	virtual bool OpenDvr(void);
	virtual void CloseDvr(void);
	virtual bool GetTSPacket(uchar *&Data);

	virtual int OpenFilter(u_short Pid, u_char Tid, u_char Mask);
	virtual void CloseFilter(int Handle);

public:
	cStreamdevDevice(void);
	virtual ~cStreamdevDevice();

	virtual bool HasInternalCam(void) { return true; }
	virtual bool ProvidesSource(int Source) const;
	virtual bool ProvidesTransponder(const cChannel *Channel) const;
	virtual bool ProvidesChannel(const cChannel *Channel, int Priority = -1,
			bool *NeedsDetachReceivers = NULL) const;
#if APIVERSNUM >= 10700
	virtual int NumProvidedSystems(void) const;
#endif
#if APIVERSNUM >= 10719
	virtual bool AvoidRecording(void) const { return true; }
#endif
#if APIVERSNUM >= 10722
	virtual bool IsTunedToTransponder(const cChannel *Channel) const;
#else
	virtual bool IsTunedToTransponder(const cChannel *Channel);
#endif
	virtual const cChannel *GetCurrentlyTunedTransponder(void) const;
	virtual cString DeviceName(void) const;
	virtual cString DeviceType(void) const;
	virtual int SignalStrength(void) const;
	virtual int SignalQuality(void) const;

	bool ReInit(bool Disable);
	void UpdatePriority(bool SwitchingChannels = false) const;
	bool SuspendServer() { return m_ClientSocket->SuspendServer(); }
	static void DenyChannel(const cChannel *Channel) { m_DenyChannel = Channel; }
};

#endif // VDR_STREAMDEV_DEVICE_H
