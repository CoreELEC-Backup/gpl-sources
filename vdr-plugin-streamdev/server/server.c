/*
 *  $Id: server.c,v 1.10 2009/02/13 10:39:22 schmirl Exp $
 */

#include "server/server.h"
#include "server/componentVTP.h"
#include "server/componentHTTP.h"
#include "server/componentIGMP.h"
#include "server/setup.h"

#include <vdr/tools.h>
#include <tools/select.h>
#include <string.h>
#include <errno.h>

cSVDRPhosts StreamdevHosts;
char *opt_auth = NULL;
char *opt_remux = NULL;

cStreamdevServer         *cStreamdevServer::m_Instance = NULL;
cList<cServerComponent>   cStreamdevServer::m_Servers;
cList<cServerConnection>  cStreamdevServer::m_Clients;

cStreamdevServer::cStreamdevServer(void):
		cThread("streamdev server")
{
	Start();
}

cStreamdevServer::~cStreamdevServer() 
{
	Stop();
}

void cStreamdevServer::Initialize(void) 
{
	if (m_Instance == NULL) {
		if (StreamdevServerSetup.StartVTPServer)  Register(new cComponentVTP);
		if (StreamdevServerSetup.StartHTTPServer) Register(new cComponentHTTP);
		if (StreamdevServerSetup.StartIGMPServer) {
			if (strcmp(StreamdevServerSetup.IGMPBindIP, "0.0.0.0") == 0)
				esyslog("streamdev-server: Not starting IGMP. IGMP must be bound to a local IP");
			else
				Register(new cComponentIGMP);
		}

		m_Instance = new cStreamdevServer;
	}
}

void cStreamdevServer::Destruct(void) 
{
	DELETENULL(m_Instance);
}

void cStreamdevServer::Stop(void) 
{
	if (Running())
		Cancel(3);
}

void cStreamdevServer::Register(cServerComponent *Server) 
{
	m_Servers.Add(Server);
}

void cStreamdevServer::Action(void) 
{
	/* Initialize Server components, deleting those that failed */
	for (cServerComponent *c = m_Servers.First(); c;) {
		cServerComponent *next = m_Servers.Next(c);
		if (!c->Initialize())
			m_Servers.Del(c);
		c = next;
	}
			
	if (m_Servers.Count() == 0) {
		esyslog("ERROR: no streamdev server activated, exiting");
		Cancel(-1);
	}

	cTBSelect select;
	while (Running()) {
		select.Clear();

		/* Ask all Server components to register to the selector */
		for (cServerComponent *c = m_Servers.First(); c; c = m_Servers.Next(c))
			select.Add(c->Socket(), false);
		
		/* Ask all Client connections to register to the selector */
		for (cServerConnection *s = m_Clients.First(); s; s = m_Clients.Next(s))
		{
			select.Add(s->Socket(), false);
			if (s->HasData())
				select.Add(s->Socket(), true);
		}

		int sel;
		do
		{
			sel = select.Select(400);
			if (sel < 0 && errno == ETIMEDOUT) {
				// check for aborted clients
				for (cServerConnection *s = m_Clients.First(); s; s = m_Clients.Next(s)) {
					if (s->Abort())
						sel = 0;
				}
			}
		} while (sel < 0 && errno == ETIMEDOUT && Running());

		if (!Running())
			break;
		if (sel < 0) {
			esyslog("fatal error, server exiting: %m");
			break;
		}
	
		/* Ask all Server components to act on signalled sockets */
		for (cServerComponent *c = m_Servers.First(); c; c = m_Servers.Next(c)){
			if (sel && select.CanRead(c->Socket())) {
				cServerConnection *client = c->Accept();
				if (!client)
					continue;
				Lock();
				m_Clients.Add(client);
				Unlock();

				if (m_Clients.Count() > StreamdevServerSetup.MaxClients) {
					esyslog("streamdev: too many clients, rejecting %s:%d",
					        client->RemoteIp().c_str(), client->RemotePort());
					client->Reject();
				} else if (!client->CanAuthenticate() && !StreamdevHosts.Acceptable(client->RemoteIpAddr())) {
					esyslog("streamdev: client %s:%d not allowed to connect",
					        client->RemoteIp().c_str(), client->RemotePort());
					client->Reject();
				} else 
					client->Welcome();
			}
		}

		/* Ask all Client connections to act on signalled sockets */
		for (cServerConnection *s = m_Clients.First(); s;) {
			bool result = true;

			if (sel && select.CanWrite(s->Socket()))
				result = s->Write();

			if (sel && result && select.CanRead(s->Socket()))
				result = s->Read();

			result &= !s->Abort();
			
			cServerConnection *next = m_Clients.Next(s);
			if (!result) {
				if (s->IsOpen())
					s->Close();
				Lock();
				m_Clients.Del(s);
				Unlock();
			}
			s = next;
		}
	}
	
	Lock();
	while (m_Clients.Count() > 0) {
		cServerConnection *s = m_Clients.First();
		s->Close();
		m_Clients.Del(s);
	}
	Unlock();

	while (m_Servers.Count() > 0) {
		cServerComponent *c = m_Servers.First();
		c->Destruct();
		m_Servers.Del(c);
	}
}

#if APIVERSNUM >= 20300
cList<cServerConnection>& cStreamdevServer::Clients(cThreadLock& Lock)
#else
const cList<cServerConnection>& cStreamdevServer::Clients(cThreadLock& Lock)
#endif
{
	Lock.Lock(m_Instance);
	return m_Clients;
}
