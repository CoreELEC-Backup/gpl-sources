/*                                                                  -*- c++ -*-
Copyright (C) 2004-2013 Christian Wieninger

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html

The author can be reached at cwieninger@gmx.de

The project's page is at http://winni.vdr-developer.org/epgsearch
*/

#include <vector>
#include <vdr/status.h>
#include "menu_event.h"
#include "menu_commands.h"
#include "epgsearchcfg.h"
#include "epgsearchtools.h"

// --- cMenuEventSearch ------------------------------------------------------------
cMenuEventSearch::cMenuEventSearch(const cEvent* Event, cEventObjects& EventObjects, MenuEventSurfMode SurfMode)
    : cOsdMenu(tr("Event")),
      eventObjects(EventObjects),
      surfMode(SurfMode)
{
    SetMenuCategory(mcEvent);

    eventObjects.SetCurrent(Event);
    szGreen = szYellow = NULL;
    Set();
}

cEventObj* cMenuEventSearch::GetPrev(const cEvent* Event)
{
    cEventObj* prevEventObj = NULL;
    cEventObjects::iterator i;
    for (i = eventObjects.begin(); i != eventObjects.end(); ++i) {
        if (*i && (*i)->Event() == Event)
            return prevEventObj;
        else
            prevEventObj = *i;
    }
    return NULL;
}

cEventObj* cMenuEventSearch::GetNext(const cEvent* Event)
{
    cEventObjects::iterator i;
    for (i = eventObjects.begin(); i != eventObjects.end(); ++i)
        if (*i && (*i)->Event() == Event) {
            cEventObjects::iterator nexti = i;
            ++nexti;
            return nexti != eventObjects.end() ? *nexti : NULL;
        }

    return NULL;
}

void cMenuEventSearch::Set()
{
    cEventObj* eventObj = eventObjects.GetCurrent();
    if (!eventObj) return;
    event = eventObj->Event();
    if (!event) return;

    if (szGreen) free(szGreen);
    if (szYellow) free(szYellow);
    szGreen = szYellow = NULL;

    if (event) {
        LOCK_TIMERS_READ;
        LOCK_CHANNELS_READ;
        const cChannel *channel = Channels->GetByChannelID(event->ChannelID(), true, true);
        bool canSwitch = false;
        if (channel) {
            SetTitle(channel->Name());
            canSwitch = channel->Number() != cDevice::CurrentChannel();
        }

        cEventObj* eventObjPrev = GetPrev(event);
        cEventObj* eventObjNext = GetNext(event);

        eTimerMatch timerMatch = tmNone;
        Timers->GetMatch(event, &timerMatch);
        const char* szRed = trVDR("Button$Record");
        if (timerMatch == tmFull)
            szRed = trVDR("Button$Timer");

        if (surfMode == SurfModeUnknown)
            SetHelp(szRed, eventObjPrev ? "<<" : NULL, eventObjNext ? ">>" : NULL, canSwitch ? trVDR("Button$Switch") : NULL);
        else if (surfMode == SurfModeTime) {
            if (eventObjPrev && eventObjPrev->Event()) szGreen = strdup(GETTIMESTRING(eventObjPrev->Event()));
            if (eventObjNext && eventObjNext->Event()) szYellow = strdup(GETTIMESTRING(eventObjNext->Event()));
            SetHelp(szRed, szGreen, szYellow, canSwitch ? trVDR("Button$Switch") : NULL);
        } else if (surfMode == SurfModeChannel) {
            if (eventObjPrev && eventObjPrev->Event())
                szGreen = strdup(CHANNELNAME(Channels->GetByChannelID(eventObjPrev->Event()->ChannelID(), true, true)));
            if (eventObjNext && eventObjNext->Event())
                szYellow = strdup(CHANNELNAME(Channels->GetByChannelID(eventObjNext->Event()->ChannelID(), true, true)));
            SetHelp(szRed, szGreen, szYellow, canSwitch ? trVDR("Button$Switch") : NULL);
        }
    }
}

cMenuEventSearch::~cMenuEventSearch()
{
    if (szGreen) free(szGreen);
    if (szYellow) free(szYellow);
}

void cMenuEventSearch::Display(void)
{
    cOsdMenu::Display();
#ifdef USE_GRAPHTFT
    cStatus::MsgOsdSetEvent(event);
#endif
    if (event) {
        DisplayMenu()->SetEvent(event);
        cStatus::MsgOsdTextItem(event->Description());
    }
}

eOSState cMenuEventSearch::Commands(eKeys Key)
{
    if (HasSubMenu())
        return osContinue;
    if (event) {
        cMenuSearchCommands *menu;
        eOSState state = AddSubMenu(menu = new cMenuSearchCommands(tr("EPG Commands"), event, true));
        if (Key != kNone)
            state = menu->ProcessKey(Key);
        return state;
    }
    return osContinue;
}

eOSState cMenuEventSearch::ProcessKey(eKeys Key)
{
    if (!HasSubMenu()) {
        switch ((int)Key) {
        case kUp|k_Repeat:
        case kUp:
        case kDown|k_Repeat:
        case kDown:
        case kLeft|k_Repeat:
        case kLeft:
        case kRight|k_Repeat:
        case kRight:
            DisplayMenu()->Scroll(NORMALKEY(Key) == kUp || NORMALKEY(Key) == kLeft, NORMALKEY(Key) == kLeft || NORMALKEY(Key) == kRight);
            cStatus::MsgOsdTextItem(NULL, NORMALKEY(Key) == kUp);
            return osContinue;
        case k1...k9:
            if (!HasSubMenu())
                return Commands(Key);
            else
                return osContinue;
            break;
        case kInfo:
            return osBack;
        default:
            break;
        }
    }

    eOSState state = cOsdMenu::ProcessKey(Key);

    if (state == osUnknown) {
        switch (Key) {
        case kOk:
            return osBack;
        case kGreen:
        case kFastRew: {
            cEventObj* eventObjPrev = GetPrev(event);
            if (eventObjPrev && eventObjPrev->Event()) {
                eventObjects.SetCurrent(eventObjPrev->Event());
                Set();
                Display();
            }
            state = osContinue;
        }
        break;
        case kFastFwd:
        case kYellow: {
            cEventObj* eventObjNext = GetNext(event);
            if (eventObjNext && eventObjNext->Event()) {
                eventObjects.SetCurrent(eventObjNext->Event());
                Set();
                Display();
            }
            state = osContinue;
        }
        break;
        default:
            break;
        }
    }
    return state;
}

cMenuEventSearchSimple::cMenuEventSearchSimple(const cEvent* Event, cEventObjects& EventObjects)
    : cMenuEventSearch(Event, EventObjects)
{
    Set();
}

void cMenuEventSearchSimple::Set()
{
    cEventObj* eventObj = eventObjects.GetCurrent();
    if (!eventObj) return;
    event = eventObj->Event();
    if (!event) return;

    if (szGreen) free(szGreen);
    if (szYellow) free(szYellow);
    szGreen = szYellow = NULL;

    if (event) {
        LOCK_CHANNELS_READ;
        const cChannel *channel = Channels->GetByChannelID(event->ChannelID(), true, true);
        if (channel) {
            SetTitle(channel->Name());
        }

        cEventObj* eventObjPrev = GetPrev(event);
        cEventObj* eventObjNext = GetNext(event);
        SetHelp(NULL, eventObjPrev ? "<<" : NULL, eventObjNext ? ">>" : NULL, NULL);
    }
    Display();
}
