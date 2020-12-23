/*
 *  $Id: setup.h,v 1.7 2010/06/08 05:55:17 schmirl Exp $
 */
 
#ifndef VDR_STREAMDEV_SETUPCLIENT_H
#define VDR_STREAMDEV_SETUPCLIENT_H

#include "common.h"

struct cStreamdevClientSetup {
	cStreamdevClientSetup(void);

	bool SetupParse(const char *Name, const char *Value);

	int  StartClient;
	char RemoteIp[20];
	int  RemotePort;
	int  Timeout;
	int  StreamFilters;
	int  FilterSockBufSize;
	int  HideMenuEntry;
	int  LivePriority;
	int  MinPriority;
	int  MaxPriority;
#if APIVERSNUM >= 10700
	int  NumProvidedSystems;
#endif
};

extern cStreamdevClientSetup StreamdevClientSetup;

class cPluginStreamdevClient;

class cStreamdevClientMenuSetupPage: public cMenuSetupPage {
private:
	cPluginStreamdevClient *m_Plugin;
	cStreamdevClientSetup    m_NewSetup;
	
protected:
	virtual void Store(void);

public:
	cStreamdevClientMenuSetupPage(cPluginStreamdevClient *Plugin);
	virtual ~cStreamdevClientMenuSetupPage();
};

#endif // VDR_STREAMDEV_SETUPCLIENT_H
