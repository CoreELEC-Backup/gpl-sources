/*
 *  $Id: menu.c,v 1.10 2010/07/19 13:49:31 schmirl Exp $
 */
 
#include <vdr/menuitems.h>
#include <vdr/thread.h>
#include <vdr/player.h>

#include "server/menu.h"
#include "server/setup.h"
#include "server/server.h"
#include "server/suspend.h"

cStreamdevServerMenu::cStreamdevServerMenu(): cOsdMenu(tr("Streamdev Connections"), 4, 20) {
	cThreadLock lock;
#if APIVERSNUM >= 20300
	cList<cServerConnection>& clients = cStreamdevServer::Clients(lock);
#else
	const cList<cServerConnection>& clients = cStreamdevServer::Clients(lock);
#endif
	for (cServerConnection *s = clients.First(); s; s = clients.Next(s))
		Add(new cOsdItem(s->ToText('\t')));
	SetHelpKeys();
	Display();
}

cStreamdevServerMenu::~cStreamdevServerMenu() {
}

void cStreamdevServerMenu::SetHelpKeys() {
	SetHelp(Count() ? tr("Disconnect") : NULL, NULL, NULL, tr("Suspend"));
}

eOSState cStreamdevServerMenu::Disconnect() {
	cOsdItem *item = Get(Current());
	if (item) {
		cThreadLock lock;
#if APIVERSNUM >= 20300
		cList<cServerConnection>& clients = cStreamdevServer::Clients(lock);
#else
		const cList<cServerConnection>& clients = cStreamdevServer::Clients(lock);
#endif
		const char *text = item->Text();
		for (cServerConnection *s = clients.First(); s; s = clients.Next(s)) {
			if (!strcmp(text, s->ToText('\t'))) {
				s->Close();
				Del(Current());
				SetHelpKeys();
				Display();
				break;
			}
		}
	}
	return osContinue;
}

eOSState cStreamdevServerMenu::Suspend() {
	if (!cSuspendCtl::IsActive()) {
		cControl::Launch(new cSuspendCtl);
		return osBack;
	}
	return osContinue;
}

eOSState cStreamdevServerMenu::ProcessKey(eKeys Key) {
	eOSState state = cOsdMenu::ProcessKey(Key);
	if (state == osUnknown) {
		switch (Key) {
			case kRed:  return Disconnect();
			case kBlue: return Suspend();
			case kOk:   return osBack;
			default:    break;
		}
	}
	return state;
}
