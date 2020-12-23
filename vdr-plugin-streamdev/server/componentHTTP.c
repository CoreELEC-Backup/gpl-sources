/*
 *  $Id: componentHTTP.c,v 1.2 2005/05/09 20:22:29 lordjaxom Exp $
 */
 
#include "server/componentHTTP.h"
#include "server/connectionHTTP.h"
#include "server/setup.h"

cComponentHTTP::cComponentHTTP(void):
		cServerComponent("HTTP", StreamdevServerSetup.HTTPBindIP, 
		                 StreamdevServerSetup.HTTPServerPort) 
{
}

cServerConnection *cComponentHTTP::NewClient(void)
{
	return new cConnectionHTTP;
}
