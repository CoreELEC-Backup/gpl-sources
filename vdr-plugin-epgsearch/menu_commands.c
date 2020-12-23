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
#include "menu_commands.h"
#include <vdr/interface.h>
#include <vdr/menu.h>
#include <vdr/plugin.h>
#include "menu_searchresults.h"
#include "menu_myedittimer.h"
#include "menu_search.h"
#include "menu_searchedit.h"
#include "epgsearchcfg.h"
#include "recdone.h"
#include "epgsearchtools.h"
#include "switchtimer.h"
#include "switchtimer_thread.h"
#include "blacklist.h"
#include "menu_blacklistedit.h"
#include <vdr/menu.h>
#include "menu_deftimercheckmethod.h"

// --- cMenuSearchCommands ---------------------------------------------------------

cMenuSearchCommands::cMenuSearchCommands(const char *Title, const cEvent* Event, bool DirectCall, cSearchExt* Search)
    : cOsdMenu(Title)
{
    SetMenuCategory(mcCommand);

    directCall = DirectCall;
    SetHasHotkeys();
    LoadCommands();

    search = Search;
    event = Event;
    Add(new cOsdItem(hk(tr("Repeats"))));
    Add(new cOsdItem(hk(trVDR("Button$Record"))));
    Add(new cOsdItem(hk(trVDR("Button$Switch"))));
    Add(new cOsdItem(hk(tr("Create search"))));
    Add(new cOsdItem(hk(tr("Search in recordings"))));
    Add(new cOsdItem(hk(tr("Mark as 'already recorded'?"))));
    Add(new cOsdItem(hk(tr("Add/Remove to/from switch list?"))));
    Add(new cOsdItem(hk(tr("Create blacklist"))));
    for (cCommand *command = commands.First(); command; command = commands.Next(command))
        Add(new cOsdItem(hk(command->Title())));
    if (event) {
        cString szTitle = cString::sprintf("%s: %s", tr("EPG Commands"), event->Title());
        SetTitle(szTitle);
    }
}

cMenuSearchCommands::~cMenuSearchCommands()
{
}

void cMenuSearchCommands::LoadCommands()
{
    const char* szLanguageCode = I18nLanguageCode(I18nCurrentLanguage());

    char *pstrSearchToken, *pptr;
    char *pstrSearch = strdup(szLanguageCode);
    pstrSearchToken = strtok_r(pstrSearch, ",", &pptr);
    bool bLoaded = false;
    while (pstrSearchToken) { // may contain multiple code, e.g. 'ger,deu'
        cString cmdFile = cString::sprintf("%s-%s.conf", ADDDIR(CONFIGDIR, "epgsearchcmds"), pstrSearchToken);
        if (access(cmdFile, F_OK) == 0) {
            commands.Load(cmdFile, true);
            bLoaded = true;
            break;
        }
        pstrSearchToken = strtok_r(NULL, ",", &pptr);
    }
    if (!bLoaded)
        commands.Load(AddDirectory(CONFIGDIR, "epgsearchcmds.conf"), true);
    free(pstrSearch);
}

eOSState cMenuSearchCommands::Switch(void)
{
    LOCK_CHANNELS_READ;
    const cChannel *channel = Channels->GetByChannelID(event->ChannelID(), true, true);
    if (channel && cDevice::PrimaryDevice()->SwitchChannel(channel, true))
        return osEnd;
    else {
        INFO(trVDR("Can't switch channel!"));
        return osContinue;
    }
}

eOSState cMenuSearchCommands::ExtendedSearch(void)
{
    return AddSubMenu(new cMenuEPGSearchExt());
}

eOSState cMenuSearchCommands::Record(void)
{
    if (!event) return osContinue;

    eTimerMatch timerMatch = tmNone;
    LOCK_TIMERS_WRITE;
    Timers->SetExplicitModify();
    cTimer* timer = Timers->GetMatch(event, &timerMatch);
    if (timerMatch == tmFull) {
        if (EPGSearchConfig.useVDRTimerEditMenu)
            return AddSubMenu(new cMenuEditTimer(timer));
        else
            return AddSubMenu(new cMenuMyEditTimer(timer, false, event, timer->Channel()));
    }

    timer = new cTimer(event);
    PrepareTimerFile(event, timer);
    cTimer *t = Timers->GetTimer(timer);

    if (EPGSearchConfig.onePressTimerCreation == 0 || t || (!t && event->StartTime() - (Setup.MarginStart + 2) * 60 < time(NULL))) {
        if (t) {
            delete timer;
            timer = t;
        }
        if (EPGSearchConfig.useVDRTimerEditMenu)
            return AddSubMenu(new cMenuEditTimer(timer, !t));
        else
            return AddSubMenu(new cMenuMyEditTimer(timer, !t, event));
    } else {
        string fullaux = "";
        string aux = "";
        if (event) {
            int bstart = event->StartTime() - timer->StartTime();
            int bstop = timer->StopTime() - event->EndTime();
            int checkmode = DefTimerCheckModes.GetMode(timer->Channel());
            aux = UpdateAuxValue(aux, "channel", NumToString(timer->Channel()->Number()) + " - " + CHANNELNAME(timer->Channel()));
            aux = UpdateAuxValue(aux, "update", checkmode);
            aux = UpdateAuxValue(aux, "eventid", event->EventID());
            aux = UpdateAuxValue(aux, "bstart", bstart);
            aux = UpdateAuxValue(aux, "bstop", bstop);
            fullaux = UpdateAuxValue(fullaux, "epgsearch", aux);
        }
#ifdef USE_PINPLUGIN
        aux = "";
        aux = UpdateAuxValue(aux, "protected", timer->FskProtection() ? "yes" : "no");
        fullaux = UpdateAuxValue(fullaux, "pin-plugin", aux);
#endif

        SetAux(timer, fullaux);
        if (Setup.SVDRPPeering && *Setup.SVDRPDefaultHost)
            timer->SetRemote(Setup.SVDRPDefaultHost);
        Timers->Add(timer);
        timer->Matches();
        Timers->SetModified();
        if (!HandleRemoteTimerModifications(timer)) {
            ERROR(tr("Epgsearch: RemoteTimerModifications failed"));
            Timers->Del(timer);
        } else
            return osBack;
    }
    return osContinue;
}

eOSState cMenuSearchCommands::MarkAsRecorded(void)
{
    if (!event) return osContinue;
    if (!Interface->Confirm(tr("Mark as 'already recorded'?")))
        return osContinue;
    cTimer* dummyTimer = new cTimer(event);
    cMutexLock RecsDoneLock(&RecsDone);
    RecsDone.Add(new cRecDone(dummyTimer, event, search));
    RecsDone.Save();
    delete dummyTimer;
    return osBack;
}

eOSState cMenuSearchCommands::AddToSwitchList(void)
{
    if (!event) return osContinue;

    time_t now = time(NULL);
    if (now >= event->StartTime()) {
        ERROR(tr("Already running!"));
        return osBack;
    }
    cSwitchTimer* switchTimer = SwitchTimers.InSwitchList(event);
    if (!switchTimer) {
        if (!Interface->Confirm(tr("Add to switch list?")))
            return osContinue;
        cMutexLock SwitchTimersLock(&SwitchTimers);
        SwitchTimers.Add(new cSwitchTimer(event));
        SwitchTimers.Save();
        cSwitchTimerThread::Init(); // asure the thread is running
    } else {
        if (!Interface->Confirm(tr("Delete from switch list?")))
            return osContinue;
        cMutexLock SwitchTimersLock(&SwitchTimers);
        SwitchTimers.Del(switchTimer);
        SwitchTimers.Save();
        if (SwitchTimers.Count() == 0)
            cSwitchTimerThread::Exit();
    }
    return osBack;
}

eOSState cMenuSearchCommands::CreateSearchTimer(void)
{
    if (!event) return osContinue;

    cSearchExt* pNew = new cSearchExt;
    strcpy(pNew->search, event->Title());
    LOCK_CHANNELS_READ;
    pNew->channelMin = pNew->channelMax = Channels->GetByChannelID(event->ChannelID());
    return AddSubMenu(new cMenuEditSearchExt(pNew, true, false, true));
}

eOSState cMenuSearchCommands::CreateBlacklist(void)
{
    if (!event) return osContinue;

    cBlacklist* pNew = new cBlacklist;
    strcpy(pNew->search, event->Title());
    return AddSubMenu(new cMenuBlacklistEdit(pNew, true));
}

eOSState cMenuSearchCommands::Execute(void)
{
    int current = Current();
    if (current <= 7) {
        if (current == 0)
            return AddSubMenu(new cMenuSearchResultsForQuery(event->Title(), true));
        if (current == 1)
            return Record();
        if (current == 2)
            return Switch();
        if (current == 3)
            return CreateSearchTimer();
        if (current == 4)
            return AddSubMenu(new cMenuSearchResultsForRecs(event->Title()));
        if (current == 5)
            return MarkAsRecorded();
        if (current == 6)
            return AddToSwitchList();
        if (current == 7)
            return CreateBlacklist();
    }

    cCommand *command = commands.Get(current - 8);
    if (command) {
        cString buffer;
        bool confirmed = true;
        if (command->Confirm()) {
            buffer = cString::sprintf("%s?", command->Title());
            confirmed = Interface->Confirm(buffer);
        }
        if (confirmed) {
            buffer = cString::sprintf("%s...", command->Title());
            Skins.Message(mtStatus, buffer);

            {
                LOCK_CHANNELS_READ;
                buffer = cString::sprintf("'%s' %ld %ld %d '%s' '%s'",
                                          EscapeString(event->Title()).c_str(),
                                          event->StartTime(),
                                          event->EndTime(),
                                          ChannelNrFromEvent(event),
                                          EscapeString(Channels->GetByChannelID(event->ChannelID(), true, true)->Name()).c_str(),
                                          EscapeString(event->ShortText() ? event->ShortText() : "").c_str());
            }
            const char *Result = command->Execute(buffer);
            Skins.Message(mtStatus, NULL);
            if (Result)
                return AddSubMenu(new cMenuText(command->Title(), Result, fontFix));
            return osBack;
        }
    }
    return osContinue;
}

eOSState cMenuSearchCommands::ProcessKey(eKeys Key)
{
    bool hadSubmenu = HasSubMenu();
    eOSState state = cOsdMenu::ProcessKey(Key);

    // jump back to calling menu, if a command was called directly with key '1' .. '9'
    if (directCall && hadSubmenu && !HasSubMenu())
        return osBack;

    if (state == osUnknown) {
        switch (Key) {
        case kGreen:
        case kYellow:
        case kBlue:
            return osContinue;
        case k1...k9:
        case kOk:
            return Execute();
        default:
            break;
        }
    }
    return state;
}
