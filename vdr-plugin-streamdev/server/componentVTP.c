/*
 *  $Id: componentVTP.c,v 1.2 2005/05/09 20:22:29 lordjaxom Exp $
 */
 
#include "server/componentVTP.h"
#include "server/connectionVTP.h"
#include "server/setup.h"

cComponentVTP::cComponentVTP(void): 
		cServerComponent("VTP", StreamdevServerSetup.VTPBindIP, 
		                 StreamdevServerSetup.VTPServerPort)
{
}

cServerConnection *cComponentVTP::NewClient(void)
{
	return new cConnectionVTP;
}
