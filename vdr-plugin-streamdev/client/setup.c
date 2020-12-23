/*
 *  $Id: setup.c,v 1.10 2010/06/08 05:55:17 schmirl Exp $
 */
 
#include <vdr/menuitems.h>

#include "client/setup.h"
#include "client/streamdev-client.h"

#ifndef MINPRIORITY
#define MINPRIORITY -MAXPRIORITY
#endif

cStreamdevClientSetup StreamdevClientSetup;

cStreamdevClientSetup::cStreamdevClientSetup(void) {
	StartClient   = false;
	RemotePort    = 2004;
	Timeout       = 2;
	StreamFilters = false;
	HideMenuEntry = false;
	LivePriority  = 0;
	MinPriority   = MINPRIORITY;
	MaxPriority   = MAXPRIORITY;
#if APIVERSNUM >= 10700
	NumProvidedSystems = 1;
#endif
	strcpy(RemoteIp, "");
	FilterSockBufSize = 0;
}

bool cStreamdevClientSetup::SetupParse(const char *Name, const char *Value) {
	if      (strcmp(Name, "StartClient") == 0)   StartClient = atoi(Value);
	else if (strcmp(Name, "RemoteIp") == 0) {
		if (strcmp(Value, "-none-") == 0)
			strcpy(RemoteIp, "");
		else
			strcpy(RemoteIp, Value);
	}
	else if (strcmp(Name, "RemotePort") == 0)    RemotePort = atoi(Value);
	else if (strcmp(Name, "Timeout") == 0)       Timeout = atoi(Value);
	else if (strcmp(Name, "StreamFilters") == 0) StreamFilters = atoi(Value);
	else if (strcmp(Name, "HideMenuEntry") == 0) HideMenuEntry = atoi(Value);
	else if (strcmp(Name, "LivePriority") == 0)  LivePriority = atoi(Value);
	else if (strcmp(Name, "MinPriority") == 0)   MinPriority = atoi(Value);
	else if (strcmp(Name, "MaxPriority") == 0)   MaxPriority = atoi(Value);
	else if (strcmp(Name, "FilterSockBufSize") == 0) FilterSockBufSize = atoi(Value);
#if APIVERSNUM >= 10700
	else if (strcmp(Name, "NumProvidedSystems") == 0) NumProvidedSystems = atoi(Value);
#endif
	else return false;
	return true;
}

cStreamdevClientMenuSetupPage::cStreamdevClientMenuSetupPage(cPluginStreamdevClient *Plugin) {
	m_Plugin = Plugin;
	m_NewSetup = StreamdevClientSetup;

	Add(new cMenuEditBoolItem(tr("Hide Mainmenu Entry"), &m_NewSetup.HideMenuEntry));
	Add(new cMenuEditIntItem (tr("Simultaneously used Devices"), &m_NewSetup.StartClient, 0, STREAMDEV_MAXDEVICES));
	Add(new cMenuEditIpItem  (tr("Remote IP"),            m_NewSetup.RemoteIp));
	Add(new cMenuEditIntItem (tr("Remote Port"),         &m_NewSetup.RemotePort, 0, 65535));
	Add(new cMenuEditIntItem (tr("Timeout (s)"),         &m_NewSetup.Timeout, 1, 15));
	Add(new cMenuEditBoolItem(tr("Filter Streaming"),    &m_NewSetup.StreamFilters));
	if(m_NewSetup.StreamFilters)
		Add(new cMenuEditIntItem (tr("Filter SockBufSize"), &m_NewSetup.FilterSockBufSize, 0, 8192000));
	Add(new cMenuEditIntItem (tr("Live TV Priority"),    &m_NewSetup.LivePriority, MINPRIORITY, MAXPRIORITY));
	Add(new cMenuEditIntItem (tr("Minimum Priority"),    &m_NewSetup.MinPriority, MINPRIORITY, MAXPRIORITY));
	Add(new cMenuEditIntItem (tr("Maximum Priority"),    &m_NewSetup.MaxPriority, MINPRIORITY, MAXPRIORITY));
#if APIVERSNUM >= 10715
	Add(new cMenuEditIntItem (tr("Broadcast Systems / Cost"),  &m_NewSetup.NumProvidedSystems, 1, 15));
#elif APIVERSNUM >= 10700
	Add(new cMenuEditIntItem (tr("Broadcast Systems / Cost"),  &m_NewSetup.NumProvidedSystems, 1, 4));
#endif
	SetCurrent(Get(0));
}

cStreamdevClientMenuSetupPage::~cStreamdevClientMenuSetupPage() {
}

void cStreamdevClientMenuSetupPage::Store(void) {
	SetupStore("StartClient", m_NewSetup.StartClient);
	if (strcmp(m_NewSetup.RemoteIp, "") == 0)
		SetupStore("RemoteIp", "-none-");
	else
		SetupStore("RemoteIp",    m_NewSetup.RemoteIp);
	SetupStore("RemotePort",    m_NewSetup.RemotePort);
	SetupStore("Timeout",       m_NewSetup.Timeout);
	SetupStore("StreamFilters", m_NewSetup.StreamFilters);
	SetupStore("HideMenuEntry", m_NewSetup.HideMenuEntry);
	SetupStore("LivePriority",  m_NewSetup.LivePriority);
	SetupStore("MinPriority",   m_NewSetup.MinPriority);
	SetupStore("MaxPriority",   m_NewSetup.MaxPriority);
#if APIVERSNUM >= 10700
	SetupStore("NumProvidedSystems", m_NewSetup.NumProvidedSystems);
#endif
	SetupStore("FilterSockBufSize", m_NewSetup.FilterSockBufSize);

	StreamdevClientSetup = m_NewSetup;

	m_Plugin->Initialize();
}

