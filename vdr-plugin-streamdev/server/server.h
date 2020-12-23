/*
 *  $Id: server.h,v 1.6 2008/10/22 11:59:32 schmirl Exp $
 */
 
#ifndef VDR_STREAMDEV_SERVER_H
#define VDR_STREAMDEV_SERVER_H

#include <vdr/thread.h>

#include "server/component.h"
#include "server/connection.h"

#define DEFAULT_EXTERNREMUX (*AddDirectory(cPlugin::ConfigDirectory(PLUGIN_NAME_I18N), "externremux.sh"))
#define STREAMDEVHOSTSPATH (*AddDirectory(cPlugin::ConfigDirectory(PLUGIN_NAME_I18N), "streamdevhosts.conf"))

extern char *opt_auth;
extern char *opt_remux;

class cStreamdevServer: public cThread {
private:
	static cStreamdevServer         *m_Instance;
	static cList<cServerComponent>   m_Servers;
	static cList<cServerConnection>  m_Clients;

protected:
	void Stop(void);

	virtual void Action(void);

	static void Register(cServerComponent *Server);

public:
	cStreamdevServer(void);
	virtual ~cStreamdevServer();

	static void Initialize(void);
	static void Destruct(void);
	static bool Active(void);

#if APIVERSNUM >= 20300
	static cList<cServerConnection>& Clients(cThreadLock& Lock);
#else
	static const cList<cServerConnection>& Clients(cThreadLock& Lock);
#endif
};

inline bool cStreamdevServer::Active(void) 
{
	return m_Instance != NULL 
	    && m_Instance->m_Clients.Count() > 0;
}

extern cSVDRPhosts StreamdevHosts;

#endif // VDR_STREAMDEV_SERVER_H
