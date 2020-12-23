/*
 *  $Id: componentHTTP.h,v 1.2 2005/05/09 20:22:29 lordjaxom Exp $
 */
 
#ifndef VDR_STREAMDEV_HTTPSERVER_H
#define VDR_STREAMDEV_HTTPSERVER_H

#include "server/component.h"

class cComponentHTTP: public cServerComponent {
protected:
	virtual cServerConnection *NewClient(void);

public:
	cComponentHTTP(void);
};

#endif // VDR_STREAMDEV_HTTPSERVER_H
