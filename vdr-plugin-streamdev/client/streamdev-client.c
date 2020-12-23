/*
 * streamdev.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: streamdev-client.c,v 1.3 2010/08/18 10:26:56 schmirl Exp $
 */

#include "streamdev-client.h"
#include "client/device.h"
#include "client/setup.h"

#if !defined(APIVERSNUM) || APIVERSNUM < 10516
#error "VDR-1.5.16 API version or greater is required!"
#endif

const char *cPluginStreamdevClient::DESCRIPTION = trNOOP("VTP Streaming Client");

cPluginStreamdevClient::cPluginStreamdevClient(void): m_Devices() {
}

cPluginStreamdevClient::~cPluginStreamdevClient() {
}

const char *cPluginStreamdevClient::Description(void) {
	return tr(DESCRIPTION);
}

bool cPluginStreamdevClient::Initialize(void) {
	for (int i = 0; i < STREAMDEV_MAXDEVICES; i++) {
		if (m_Devices[i])
			m_Devices[i]->ReInit(i >= StreamdevClientSetup.StartClient);
		else if (i < StreamdevClientSetup.StartClient)
			m_Devices[i] = new cStreamdevDevice();
	}
	return true;
}

const char *cPluginStreamdevClient::MainMenuEntry(void) {
	return StreamdevClientSetup.StartClient && !StreamdevClientSetup.HideMenuEntry ? tr("Suspend Server") : NULL;
}

cOsdObject *cPluginStreamdevClient::MainMenuAction(void) {
	if (StreamdevClientSetup.StartClient && m_Devices[0]->SuspendServer())
		Skins.Message(mtInfo, tr("Server is suspended"));
	else
		Skins.Message(mtError, tr("Couldn't suspend Server!"));
	return NULL;
}

cMenuSetupPage *cPluginStreamdevClient::SetupMenu(void) {
  return new cStreamdevClientMenuSetupPage(this);
}

bool cPluginStreamdevClient::SetupParse(const char *Name, const char *Value) {
  return StreamdevClientSetup.SetupParse(Name, Value);
}

bool cPluginStreamdevClient::Service(const char *Id, void *Data) {
	if (!strcmp(Id, LOOP_PREVENTION_SERVICE)) {
		cStreamdevDevice::DenyChannel((const cChannel*) Data);
  		return true;
	}
	return false;
}

void cPluginStreamdevClient::MainThreadHook(void) {
	for (int i = 0; i < StreamdevClientSetup.StartClient; i++)
		m_Devices[i]->UpdatePriority();
}

VDRPLUGINCREATOR(cPluginStreamdevClient); // Don't touch this!
