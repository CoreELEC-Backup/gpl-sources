/*
 *  $Id: componentVTP.h,v 1.2 2005/05/09 20:22:29 lordjaxom Exp $
 */
 
#ifndef VDR_STREAMDEV_SERVERS_SERVERVTP_H
#define VDR_STREAMDEV_SERVERS_SERVERVTP_H

#include "server/component.h"

class cComponentVTP: public cServerComponent {
protected:
	virtual cServerConnection *NewClient(void);

public:
	cComponentVTP(void);
};

#endif // VDR_STREAMDEV_SERVERS_SERVERVTP_H
