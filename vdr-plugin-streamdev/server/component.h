/*
 *  $Id: component.h,v 1.3 2009/02/13 10:39:22 schmirl Exp $
 */
 
#ifndef VDR_STREAMDEV_SERVERS_COMPONENT_H
#define VDR_STREAMDEV_SERVERS_COMPONENT_H

#include "tools/socket.h"
#include "tools/select.h"

#include <vdr/tools.h>

class cServerConnection;

/* Basic TCP listen server, all functions virtual if a derivation wants to do 
   things different */

class cServerComponent: public cListObject {
private:
	const char *m_Protocol;
	cTBSocket m_Listen;
	const char *m_ListenIp;
	uint m_ListenPort;

protected:
	/* Returns a new connection object for Accept() */
	virtual cServerConnection *NewClient(void) = 0;

public:
	cServerComponent(const char *Protocol, const char *ListenIp, uint ListenPort, int Type = SOCK_STREAM, int IpProto = 0);
	virtual ~cServerComponent();

	/* Starts listening on the specified Port, override if you want to do things
	   different */
	virtual bool Initialize(void);

	/* Stops listening, override if you want to do things different */
	virtual void Destruct(void);

	/* Get the listening socket's file number */
	virtual int Socket(void) const { return (int)m_Listen; }

	/* Adds the listening socket to the Select object */
	virtual void Add(cTBSelect &Select) const { Select.Add(m_Listen); }

	/* Accepts the connection on a NewClient() object and calls the 
	   Welcome() on it, override if you want to do things different */
	virtual cServerConnection *Accept(void);
};

#endif // VDR_STREAMDEV_SERVERS_COMPONENT_H
