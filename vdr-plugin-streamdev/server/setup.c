/*
 *  $Id: setup.c,v 1.10 2010/07/19 13:49:31 schmirl Exp $
 */
 
#include <vdr/menuitems.h>

#include "server/setup.h"
#include "server/server.h"

cStreamdevServerSetup StreamdevServerSetup;

cStreamdevServerSetup::cStreamdevServerSetup(void) {
	HideMenuEntry   = false;
	MaxClients      = 5;
	StartSuspended  = ssAuto;
	LiveBufferMs    = 0;
	StartVTPServer  = true;
	VTPServerPort   = 2004;
	VTPPriority     = 0;
	LoopPrevention  = false;
	StartHTTPServer = true;
	HTTPServerPort  = 3000;
	HTTPPriority    = 0;
	HTTPStreamType  = stTS;
	StartIGMPServer = false;
	IGMPClientPort  = 1234;
	IGMPPriority    = 0;
	IGMPStreamType  = stTS;
	AllowSuspend    = false;
	strcpy(VTPBindIP, "0.0.0.0");
	strcpy(HTTPBindIP, "0.0.0.0");
	strcpy(IGMPBindIP, "0.0.0.0");
}

bool cStreamdevServerSetup::SetupParse(const char *Name, const char *Value) {
	if      (strcmp(Name, "HideMenuEntry") == 0)   HideMenuEntry   = atoi(Value);
	else if (strcmp(Name, "MaxClients") == 0)      MaxClients      = atoi(Value);
	else if (strcmp(Name, "StartSuspended") == 0)  StartSuspended  = atoi(Value);
	else if (strcmp(Name, "LiveBufferMs") == 0)    LiveBufferMs    = atoi(Value);
	else if (strcmp(Name, "StartServer") == 0)     StartVTPServer  = atoi(Value);
	else if (strcmp(Name, "ServerPort") == 0)      VTPServerPort   = atoi(Value);
	else if (strcmp(Name, "VTPPriority") == 0)     VTPPriority     = atoi(Value);
	else if (strcmp(Name, "VTPBindIP") == 0)       strcpy(VTPBindIP, Value);
	else if (strcmp(Name, "LoopPrevention") == 0)  LoopPrevention  = atoi(Value);
	else if (strcmp(Name, "StartHTTPServer") == 0) StartHTTPServer = atoi(Value);
	else if (strcmp(Name, "HTTPServerPort") == 0)  HTTPServerPort  = atoi(Value);
	else if (strcmp(Name, "HTTPPriority") == 0)    HTTPPriority    = atoi(Value);
	else if (strcmp(Name, "HTTPStreamType") == 0)  HTTPStreamType  = atoi(Value);
	else if (strcmp(Name, "HTTPBindIP") == 0)      strcpy(HTTPBindIP, Value);
	else if (strcmp(Name, "StartIGMPServer") == 0) StartIGMPServer = atoi(Value);
	else if (strcmp(Name, "IGMPClientPort") == 0)  IGMPClientPort  = atoi(Value);
	else if (strcmp(Name, "IGMPPriority") == 0)    IGMPPriority    = atoi(Value);
	else if (strcmp(Name, "IGMPStreamType") == 0)  IGMPStreamType  = atoi(Value);
	else if (strcmp(Name, "IGMPBindIP") == 0)      strcpy(IGMPBindIP, Value);
	else if (strcmp(Name, "AllowSuspend") == 0)    AllowSuspend    = atoi(Value);
	else return false;
	return true;
}

const char* cStreamdevServerMenuSetupPage::StreamTypes[st_Count - 1] = {
	"TS",
	"PES",
	"PS",
	"ES",
	"EXT"
};

cStreamdevServerMenuSetupPage::cStreamdevServerMenuSetupPage(void) {
	m_NewSetup = StreamdevServerSetup;

	Set();
}

cStreamdevServerMenuSetupPage::~cStreamdevServerMenuSetupPage() {
}

void cStreamdevServerMenuSetupPage::Set(void) {
	static const char *StartSuspendedItems[ss_Count] =
	{
		trVDR("no"),
		trVDR("yes"),
		trVDR("auto")
	};

	int current = Current();
	Clear();
	AddCategory (tr("Common Settings"));
	Add(new cMenuEditBoolItem(tr("Hide Mainmenu Entry"),       &m_NewSetup.HideMenuEntry));
	Add(new cMenuEditStraItem(tr("Start with Live TV suspended"),   &m_NewSetup.StartSuspended, ss_Count, StartSuspendedItems));
	Add(new cMenuEditIntItem (tr("Maximum Number of Clients"), &m_NewSetup.MaxClients, 0, 100));
	Add(new cMenuEditIntItem (tr("Live TV buffer delay (ms)"), &m_NewSetup.LiveBufferMs, 0, 1500));

	
	AddCategory (tr("VDR-to-VDR Server"));
	Add(new cMenuEditBoolItem(tr("Start VDR-to-VDR Server"),   &m_NewSetup.StartVTPServer));
	Add(new cMenuEditIntItem (tr("VDR-to-VDR Server Port"),    &m_NewSetup.VTPServerPort, 0, 65535));
	Add(new cMenuEditIpItem  (tr("Bind to IP"),                 m_NewSetup.VTPBindIP));
	Add(new cMenuEditIntItem (tr("Legacy Client Priority"),    &m_NewSetup.VTPPriority, MINPRIORITY, MAXPRIORITY));
	Add(new cMenuEditBoolItem(tr("Client may suspend"),        &m_NewSetup.AllowSuspend));
	if (cPluginManager::CallFirstService(LOOP_PREVENTION_SERVICE))
		Add(new cMenuEditBoolItem(tr("Loop Prevention"),           &m_NewSetup.LoopPrevention));

	AddCategory (tr("HTTP Server"));
	Add(new cMenuEditBoolItem(tr("Start HTTP Server"),         &m_NewSetup.StartHTTPServer));
	Add(new cMenuEditIntItem (tr("HTTP Server Port"),          &m_NewSetup.HTTPServerPort, 0, 65535));
	Add(new cMenuEditIpItem  (tr("Bind to IP"),                 m_NewSetup.HTTPBindIP));
	Add(new cMenuEditIntItem (tr("Priority"),                  &m_NewSetup.HTTPPriority, MINPRIORITY, MAXPRIORITY));
	Add(new cMenuEditStraItem(tr("HTTP Streamtype"),           &m_NewSetup.HTTPStreamType, st_Count - 1, StreamTypes));

	AddCategory (tr("Multicast Streaming Server"));
	Add(new cMenuEditBoolItem(tr("Start IGMP Server"),         &m_NewSetup.StartIGMPServer));
	Add(new cMenuEditIntItem (tr("Multicast Client Port"),     &m_NewSetup.IGMPClientPort, 0, 65535));
	Add(new cMenuEditIpItem  (tr("Bind to IP"),                 m_NewSetup.IGMPBindIP));
	Add(new cMenuEditIntItem (tr("Priority"),                  &m_NewSetup.IGMPPriority, MINPRIORITY, MAXPRIORITY));
	Add(new cMenuEditStraItem(tr("Multicast Streamtype"),      &m_NewSetup.IGMPStreamType, st_Count - 1, StreamTypes));
	SetCurrent(Get(current));
	Display();
}

void cStreamdevServerMenuSetupPage::AddCategory(const char *Title) {

  cString str = cString::sprintf("--- %s -------------------------------------------------"
   		"---------------", Title );

  cOsdItem *item = new cOsdItem(*str);
  item->SetSelectable(false);
  Add(item);
}
	
void cStreamdevServerMenuSetupPage::Store(void) {
	bool restart = false;
	if (m_NewSetup.StartVTPServer != StreamdevServerSetup.StartVTPServer
			|| m_NewSetup.VTPServerPort != StreamdevServerSetup.VTPServerPort
			|| strcmp(m_NewSetup.VTPBindIP, StreamdevServerSetup.VTPBindIP) != 0
			|| m_NewSetup.StartHTTPServer != StreamdevServerSetup.StartHTTPServer
			|| m_NewSetup.HTTPServerPort != StreamdevServerSetup.HTTPServerPort
			|| strcmp(m_NewSetup.HTTPBindIP, StreamdevServerSetup.HTTPBindIP) != 0
			|| m_NewSetup.StartIGMPServer != StreamdevServerSetup.StartIGMPServer
			|| m_NewSetup.IGMPClientPort != StreamdevServerSetup.IGMPClientPort
			|| strcmp(m_NewSetup.IGMPBindIP, StreamdevServerSetup.IGMPBindIP) != 0) {
		restart = true;
		cStreamdevServer::Destruct();
	}
	
	SetupStore("HideMenuEntry",   m_NewSetup.HideMenuEntry);
	SetupStore("MaxClients",      m_NewSetup.MaxClients);
	SetupStore("StartSuspended",  m_NewSetup.StartSuspended);
	SetupStore("LiveBufferMs",    m_NewSetup.LiveBufferMs);
	SetupStore("StartServer",     m_NewSetup.StartVTPServer);
	SetupStore("ServerPort",      m_NewSetup.VTPServerPort);
	SetupStore("VTPBindIP",       m_NewSetup.VTPBindIP);
	SetupStore("VTPPriority",     m_NewSetup.VTPPriority);
	SetupStore("LoopPrevention",  m_NewSetup.LoopPrevention);
	SetupStore("StartHTTPServer", m_NewSetup.StartHTTPServer);
	SetupStore("HTTPServerPort",  m_NewSetup.HTTPServerPort);
	SetupStore("HTTPBindIP",      m_NewSetup.HTTPBindIP);
	SetupStore("HTTPPriority",    m_NewSetup.HTTPPriority);
	SetupStore("HTTPStreamType",  m_NewSetup.HTTPStreamType);
	SetupStore("StartIGMPServer", m_NewSetup.StartIGMPServer);
	SetupStore("IGMPClientPort",  m_NewSetup.IGMPClientPort);
	SetupStore("IGMPBindIP",      m_NewSetup.IGMPBindIP);
	SetupStore("IGMPPriority",    m_NewSetup.IGMPPriority);
	SetupStore("IGMPStreamType",  m_NewSetup.IGMPStreamType);
	SetupStore("AllowSuspend",    m_NewSetup.AllowSuspend);

	StreamdevServerSetup = m_NewSetup;

	if (restart) 
		cStreamdevServer::Initialize();
}
