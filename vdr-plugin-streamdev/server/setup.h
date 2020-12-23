/*
 *  $Id: setup.h,v 1.4 2010/07/19 13:49:31 schmirl Exp $
 */
 
#ifndef VDR_STREAMDEV_SETUPSERVER_H
#define VDR_STREAMDEV_SETUPSERVER_H

#include "common.h"

enum eStartSuspended {
	ssNo,
	ssYes,
	ssAuto,
	ss_Count
};

struct cStreamdevServerSetup {
	cStreamdevServerSetup(void);

	bool SetupParse(const char *Name, const char *Value);

	int HideMenuEntry;
	int MaxClients;
	int StartSuspended;
	int LiveBufferMs;
	int StartVTPServer;
	int VTPServerPort;
	char VTPBindIP[20];
	int VTPPriority;
	int AllowSuspend;
	int LoopPrevention;
	int StartHTTPServer;
	int HTTPServerPort;
	int HTTPPriority;
	int HTTPStreamType;
	char HTTPBindIP[20];
	int StartIGMPServer;
	int IGMPClientPort;
	int IGMPPriority;
	int IGMPStreamType;
	char IGMPBindIP[20];
};

extern cStreamdevServerSetup StreamdevServerSetup;

class cStreamdevServerMenuSetupPage: public cMenuSetupPage {
private:
	static const char* StreamTypes[];
	cStreamdevServerSetup m_NewSetup;

	void AddCategory(const char *Title);
	void Set();
protected:
	virtual void Store(void);

public:
	cStreamdevServerMenuSetupPage(void);
	virtual ~cStreamdevServerMenuSetupPage();
};

#endif // VDR_STREAMDEV_SETUPSERVER_H
