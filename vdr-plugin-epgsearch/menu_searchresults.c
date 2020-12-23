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
#include "menu_searchresults.h"
#include "blacklist.h"
#include "epgsearchext.h"
#include "menu_myedittimer.h"
#include "menu_event.h"
#include "menu_commands.h"
#include "epgsearchcfg.h"
#include "epgsearchtools.h"
#include "changrp.h"
#include "recdone.h"
#include "epgsearchcats.h"
#include <vdr/menu.h>
#include "menu_conflictcheck.h"
#include "uservars.h"
#include "menu_deftimercheckmethod.h"
#include "afuzzy.h"
#include "timerstatus.h"
#include "switchtimer.h"

const char* ButtonBlue[3] = {NULL, NULL, NULL};
extern int gl_InfoConflict;
extern bool isUTF8;

static int CompareRecording(const void *p1, const void *p2)
{
    return (int)((*(cRecording **)p1)->Start() - (*(cRecording **)p2)->Start());
}

// --- cMenuSearchResultsItem -------------------------------------------------------
cMenuSearchResultsItem::cMenuSearchResultsItem(const cEvent *EventInfo, bool EpisodeOnly,
                                               bool PreviewTimer, cMenuTemplate* MenuTemplate,
                                               const cSearchExt* Search)
{
    fileName = NULL;
    event = EventInfo;
    timerMatch = tmNone;
    episodeOnly = EpisodeOnly;
    previewTimer = PreviewTimer;
    menuTemplate = MenuTemplate ? MenuTemplate : cTemplFile::GetTemplateByName("MenuSearchResults");
    search = Search;
    inSwitchList = false;
    timerActive = false;
    Update(true);
}

bool cMenuSearchResultsItem::Update(bool Force)
{
    if (!menuTemplate)
        return false;

    bool result = false;

    eTimerMatch OldTimerMatch = timerMatch;
    bool OldInSwitchList = inSwitchList;
    bool OldtimerActive = timerActive;
    bool hasMatch = false;
    const cTimer* timer = NULL;
    LOCK_TIMERS_READ;
    if (event) timer = Timers->GetMatch(event, &timerMatch);
    if (event) inSwitchList = (SwitchTimers.InSwitchList(event) != NULL);
    if (timer) hasMatch = true;
    if (timer) timerActive = timer->HasFlags(tfActive);

    if (Force || timerMatch != OldTimerMatch || inSwitchList != OldInSwitchList || timerActive != OldtimerActive) {
        char t[Utf8BufSize(2)] = "", v[Utf8BufSize(2)] = "", r[Utf8BufSize(2)] = "";
        char szStatus[Utf8BufSize(4)] = "";
        if (EPGSearchConfig.WarEagle) {
            if (!isUTF8) {
                t[0] = event && hasMatch ? (timerMatch == tmFull) ? ((timer && timer->Recording()) ? ICON_REC : (timerActive ? ICON_CLOCK : ICON_TIMER_INACT)) : (timerActive ? ICON_CLOCK_HALF : ' ') : ' ';
                t[1] = '\0';
                v[0] = event && event->Vps() && (event->Vps() - event->StartTime()) ? ICON_VPS : ' ';
                v[1] = '\0';
                r[0] = event && event->IsRunning() ? ICON_RUNNING : ' ';
                r[1] = '\0';

            } else {
#if defined(__GNUC__) && __GNUC__ < 3 && __GNUC_MINOR__ < 96
#else
                sprintf(t, "%s", (event && hasMatch ? (timerMatch == tmFull) ? ((timer && timer->Recording()) ? ICON_REC_UTF8 : (timerActive ? ICON_CLOCK_UTF8 : ICON_TIMER_INACT_UTF8)) : (timerActive ? ICON_CLOCK_HALF_UTF8 : " ") : " "));
                sprintf(v, "%s", event && event->Vps() && (event->Vps() - event->StartTime()) ? ICON_VPS_UTF8 : " ");
                sprintf(r, "%s", (event && event->IsRunning() ? ICON_RUNNING_UTF8 : " "));
#endif
            }
        } else {
            t[0] = event && hasMatch ? (timerMatch == tmFull) ? ((timer && timer->Recording()) ? 'R' : (timerActive ? 'T' : 'i')) : (timerActive ? 't' : ' ') : ' ';
            t[1] = '\0';
            v[0] = event && event->Vps() && (event->Vps() - event->StartTime()) ? 'V' : ' ';
            v[1] = '\0';
            r[0] = event && event->IsRunning() ? '*' : ' ';
            r[1] = '\0';
        }
        if (event && inSwitchList) {
            cSwitchTimer* s = SwitchTimers.InSwitchList(event);
            t[0] = (s && s->mode == 1) ? 's' : 'S';
        }
        if (t[0] != 'T' && previewTimer)
            t[0] = 'P';

        strcpy(szStatus, t);
        strcat(szStatus, v);
        strcat(szStatus, r);

        char* buffer = strdup(menuTemplate->MenuTemplate());
        strreplace(buffer, '|', '\t');

        if (!strcasestr(buffer, "%subtitle%") && cTemplFile::GetTemplateByName("MenuFavorites") != menuTemplate)
            // make sure, there is a subtitle
            buffer = strreplacei(buffer, "%title%", "%title% ~ %subtitle%");
        if (episodeOnly)
            buffer = strreplacei(buffer, "%title%", "");

        // parse the epxression and evaluate it
        cVarExpr varExpr(buffer);
        char* tmp = strdup(varExpr.Evaluate(event).c_str());
        free(buffer);
        buffer = tmp;

        buffer = strreplacei(buffer, "$status$", szStatus);
        buffer = strreplacei(buffer, "$t_status$", t);
        buffer = strreplacei(buffer, "$v_status$", v);
        buffer = strreplacei(buffer, "$r_status$", r);

        buffer = FixSeparators(buffer, '~');
        buffer = FixSeparators(buffer, ':');
        buffer = FixSeparators(buffer, '-');

        SetText(buffer, false);

        if (EPGSearchConfig.checkTimerConflAfterTimerProg && !Force && timer && timerMatch && timerMatch != OldTimerMatch) {
            cConflictCheck C;
            C.Check();
            if (C.TimerInConflict(timer))
                gl_InfoConflict = 1;
        }

        return true;
    }
    return result;
}

cMenuSearchResultsItem::cMenuSearchResultsItem(const cRecording *Recording)
{
    previewTimer = false;
    episodeOnly = false;
    menuTemplate = NULL;
    timerMatch = tmNone;
    inSwitchList = false;
    event = NULL;
    search = NULL;
    fileName = strdup(Recording->FileName());
    SetText(Recording->Title('\t'));
}

void cMenuSearchResultsItem::SetMenuItem(cSkinDisplayMenu *DisplayMenu, int Index, bool Current, bool Selectable)
{
    LOCK_CHANNELS_READ;
    const cChannel *channel = event ? Channels->GetByChannelID(event->ChannelID(), true, true) : NULL;
    if (!event)
        DisplayMenu->SetItem(Text(), Index, Current, Selectable);
    else if (!DisplayMenu->SetItemEvent(event, Index, Current, Selectable, channel, true, timerMatch))
        DisplayMenu->SetItem(Text(), Index, Current, Selectable);
}

// --- cMenuSearchResults -------------------------------------------------------

const cEvent *cMenuSearchResults::scheduleEventInfo = NULL;

cMenuSearchResults::cMenuSearchResults(cMenuTemplate* MenuTemplate)
    : cOsdMenu("", MenuTemplate->Tab(0), MenuTemplate->Tab(1), MenuTemplate->Tab(2), MenuTemplate->Tab(3), MenuTemplate->Tab(4))
{
    SetMenuCategory(mcSchedule);

    helpKeys = -1;
    menuTemplate = MenuTemplate;
    modeYellow = showTitleEpisode;
    modeBlue = showAll;
    m_bSort = false;
    ignoreRunning = false;
}

int cMenuSearchResults::GetTab(int Tab)
{
    if (!menuTemplate)
        menuTemplate = cTemplFile::GetTemplateByName("MenuSearchResults");

    return menuTemplate->Tab(Tab - 1);
}

bool cMenuSearchResults::Update(void)
{
    bool result = false;
    LOCK_TIMERS_READ;
    for (cOsdItem *item = First(); item; item = Next(item)) {
        if (((cMenuSearchResultsItem *)item)->Update(Timers))
            result = true;
    }
    return result;
}

eOSState cMenuSearchResults::Record(void)
{
    UpdateCurrent();
    cMenuSearchResultsItem *item = (cMenuSearchResultsItem *)Get(Current());
    if (item) {
        LOCK_TIMERS_WRITE;
        Timers->SetExplicitModify();
        if (item->timerMatch == tmFull) {
            eTimerMatch tm = tmNone;
            cTimer *timer = Timers->GetMatch(item->event, &tm);
            if (timer) {
                if (EPGSearchConfig.useVDRTimerEditMenu)
                    return AddSubMenu(new cMenuEditTimer(timer));
                else
                    return AddSubMenu(new cMenuMyEditTimer(timer, false, item->event));
            }
        }

        cTimer *timer = new cTimer(item->event);
        PrepareTimerFile(item->event, timer);
        cTimer *t = Timers->GetTimer(timer);
        if (EPGSearchConfig.onePressTimerCreation == 0 || t || !item->event || (!t && item->event && item->event->StartTime() - (Setup.MarginStart + 2) * 60 < time(NULL))) {
            if (t) {
                delete timer;
                timer = t;
            }
            if (EPGSearchConfig.useVDRTimerEditMenu)
                return AddSubMenu(new cMenuEditTimer(timer, !t));
            else
                return AddSubMenu(new cMenuMyEditTimer(timer, !t, item->event));
        } else {
            string fullaux = "";
            string aux = "";
            if (item->event) {
                const cEvent* event = item->event;
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
            gl_timerStatusMonitor->SetConflictCheckAdvised();
            timer->Matches();
            Timers->SetModified();
            if (!HandleRemoteTimerModifications(timer)) {
                ERROR(tr("Epgsearch: RemoteTimerModifications failed"));
                Timers->Del(timer);
            }

            if (HasSubMenu())
                CloseSubMenu();
            if (Update())
                Display();
            SetHelpKeys();
        }
    }
    return osContinue;
}


eOSState cMenuSearchResults::Switch(void)
{
    UpdateCurrent();
    cMenuSearchResultsItem *item = (cMenuSearchResultsItem *)Get(Current());
    if (item) {
        LOCK_CHANNELS_READ;
        const cChannel *channel = Channels->GetByChannelID(item->event->ChannelID(), true, true);
        if (channel && cDevice::PrimaryDevice()->SwitchChannel(channel, true))
            return osEnd;
    }
    INFO(trVDR("Can't switch channel!"));
    return osContinue;
}

eOSState cMenuSearchResults::Commands(eKeys Key, cSearchExt* SearchExt)
{
    if (HasSubMenu() || Count() == 0)
        return osContinue;
    cMenuSearchResultsItem *mi = (cMenuSearchResultsItem *)Get(Current());
    if (mi && mi->event) {
        cMenuSearchCommands *menu;
        eOSState state = AddSubMenu(menu = new cMenuSearchCommands(tr("EPG Commands"), mi->event, true, SearchExt));
        if (Key != kNone)
            state = menu->ProcessKey(Key);
        return state;
    }
    return osContinue;
}

eOSState cMenuSearchResults::ShowSummary()
{
    if (Count()) {
        const cEvent *ei = ((cMenuSearchResultsItem *)Get(Current()))->event;
        if (ei) {
            const cChannel *channel;
            {
                LOCK_CHANNELS_READ;
                channel = Channels->GetByChannelID(ei->ChannelID(), true, true);
            }
            if (channel)
                return AddSubMenu(new cMenuEventSearch(ei, eventObjects));
        }
    }
    return osContinue;
}

eOSState cMenuSearchResults::OnRed(cSearchExt* searchExt)
{
    eOSState state = osUnknown;

    if (HasSubMenu())
        return Record();

    if (Count()) {
        if (EPGSearchConfig.redkeymode == toggleKeys)
            state = Record();
        else {
            cMenuSearchResultsItem* mi = (cMenuSearchResultsItem*)Get(Current());
            if (mi) {
                if (mi->event) {
                    state = AddSubMenu(new cMenuSearchCommands(tr("EPG Commands"), mi->event, false, searchExt));
                }
            }
        }
    }
    return state;
}

eOSState cMenuSearchResults::OnGreen()
{
    eOSState state = osUnknown;
    if (!HasSubMenu()) {
        m_bSort = !m_bSort;
        BuildList();
        state = osContinue;
    }
    return state;
}

eOSState cMenuSearchResults::OnYellow()
{
    eOSState state = osUnknown;
    if (!HasSubMenu()) {
        modeYellow = (modeYellow == showTitleEpisode ? showEpisode : showTitleEpisode);
        BuildList();
        state = osContinue;
    }
    return state;
}

void cMenuSearchResults::UpdateCurrent()
{
    cEventObj* cureventObj = eventObjects.GetCurrent();
    if (cureventObj && cureventObj->Event())
        for (cMenuSearchResultsItem* item = (cMenuSearchResultsItem*)First(); item; item = (cMenuSearchResultsItem*)Next(item))
            if (item->Selectable() && item->event == cureventObj->Event()) {
                cureventObj->Select(false);
                SetCurrent(item);
                Display();
                break;
            }
}

eOSState cMenuSearchResults::ProcessKey(eKeys Key)
{
    bool HadSubMenu = HasSubMenu();
    eOSState state = cOsdMenu::ProcessKey(Key);

    if (!HasSubMenu() && HadSubMenu) // navigation in summary could have changed current item, so update it
        UpdateCurrent();

    if (state == osUnknown) {
        switch (Key) {
        case k0:
            if (!HasSubMenu()) {
                toggleKeys = 1 - toggleKeys;
                SetHelpKeys(true);
            }
            state = osContinue;
            break;
        case kGreen:
            state = OnGreen();
            break;
        case kYellow:
            state = OnYellow();
            break;
        case kOk:
        case kInfo:
            if (HasSubMenu()) {
                state = cOsdMenu::ProcessKey(Key);
                break;
            }
            if (Count())
                state = ShowSummary();
            else
                state = osBack;
            break;
        default:
            break;
        }
    }
    if (!HasSubMenu()) {
        if ((HadSubMenu || gl_TimerProgged) && Update()) {
            if (gl_TimerProgged) { // when using epgsearch's timer edit menu, update is delayed because of SVDRP
                gl_TimerProgged = 0;
                SetHelpKeys();
            }
            Display();
        }
        if (Key != kNone)
            SetHelpKeys();
        if (gl_InfoConflict) {
            gl_InfoConflict = 0;
            if (Interface->Confirm(tr("Timer conflict! Show?")))
                state = AddSubMenu(new cMenuConflictCheck());
        }
    }
    return state;
}

// --- cMenuSearchResultsForSearch -------------------------------------------------------
cMenuSearchResultsForSearch::cMenuSearchResultsForSearch(cSearchExt* SearchExt, cMenuTemplate* MenuTemplate)
    : cMenuSearchResults(MenuTemplate)
{
    ButtonBlue[0] = tr("Button$all channels");
    ButtonBlue[1] = tr("Button$only FTA");
    ButtonBlue[2] = tr("Button$Timer preview");

    searchExt = SearchExt;
    m_bSort = true;
    if (searchExt) {
        modeBlue = searchExt->useChannel == 3 ? showNoPayTV : (EPGSearchConfig.ignorePayTV ? showNoPayTV : showAll);
        BuildList();
    }
}

bool cMenuSearchResultsForSearch::BuildList()
{
    bool hasResults = false;
    int current = Current();
    Clear();
    time_t now = time(NULL);
    cSearchResults* pSearchResults = searchExt->Run(modeBlue == showNoPayTV ? 1 : 0, false, 0, NULL, modeBlue != showTimerPreview);
    Clear();
    eventObjects.Clear();

    if (pSearchResults) {
        pSearchResults->SortBy(m_bSort ? CompareEventTime : CompareEventChannel);

        for (cSearchResult* pResultObj = pSearchResults->First();
             pResultObj;
             pResultObj = pSearchResults->Next(pResultObj)) {
            if (ignoreRunning && now > pResultObj->event->StartTime())
                continue;
            if (!(searchExt->useAsSearchTimer && searchExt->avoidRepeats && modeBlue == showTimerPreview))
                pResultObj->needsTimer = false;
            hasResults = true;
            Add(new cMenuSearchResultsItem(pResultObj->event, modeYellow == showEpisode, pResultObj->needsTimer, menuTemplate));
            eventObjects.Add(pResultObj->event);
        }

        delete pSearchResults;
    }
    if (Count())
        SetCurrent(Get(0));
    SetHelpKeys(true);

    cString szTitle = cString::sprintf("%d %s - %s", Count(), tr("Search results"), searchExt->search);
    SetTitle(szTitle);

    SetCurrent(Get(current));
    Display();

    return hasResults;
}

void cMenuSearchResultsForSearch::SetHelpKeys(bool Force)
{
    cMenuSearchResultsItem *item = (cMenuSearchResultsItem *)Get(Current());
    int NewHelpKeys = 0;
    if (item) {
        if (item->Selectable() && item->timerMatch == tmFull)
            NewHelpKeys = 2;
        else
            NewHelpKeys = 1;
    }

    bool hasTimer = (NewHelpKeys == 2);
    if (NewHelpKeys != helpKeys || Force) {
        ModeBlueSR nextModeBlue = (ModeBlueSR)(((int)modeBlue + 1) % 3);
        if (nextModeBlue == showTimerPreview &&
            (searchExt->useAsSearchTimer == 0 || searchExt->avoidRepeats == 0))
            nextModeBlue = (ModeBlueSR)(((int)nextModeBlue + 1) % 3);

        if (toggleKeys == 0)
            SetHelp((EPGSearchConfig.redkeymode == 0 ? (hasTimer ? trVDR("Button$Timer") : trVDR("Button$Record")) : tr("Button$Commands")), m_bSort ? tr("Button$by channel") : tr("Button$by time"), modeYellow == showTitleEpisode ? tr("Button$Episode") : tr("Button$Title"), ButtonBlue[(int)nextModeBlue]);
        else
            SetHelp((EPGSearchConfig.redkeymode == 1 ? (hasTimer ? trVDR("Button$Timer") : trVDR("Button$Record")) : tr("Button$Commands")), m_bSort ? tr("Button$by channel") : tr("Button$by time"), modeYellow == showTitleEpisode ? tr("Button$Episode") : tr("Button$Title"), ButtonBlue[(int)nextModeBlue]);
        helpKeys = NewHelpKeys;
    }
}

eOSState cMenuSearchResultsForSearch::ProcessKey(eKeys Key)
{
    eOSState state = cMenuSearchResults::ProcessKey(Key);

    if (state == osUnknown) {
        switch (Key) {
        case kRecord:
        case kRed:
            state = OnRed(searchExt);
            break;
        case k1...k9:
            state = HasSubMenu() ? osContinue : Commands(Key, searchExt);
            break;
        case kBlue:
            if (HasSubMenu())
                state = Switch();
            else {
                modeBlue = (ModeBlueSR)(((int)modeBlue + 1) % 3);
                if (modeBlue == showTimerPreview &&
                    (!searchExt || (searchExt && (searchExt->useAsSearchTimer == 0 || searchExt->avoidRepeats == 0))))
                    modeBlue = (ModeBlueSR)(((int)modeBlue + 1) % 3);

                if (modeBlue == showTimerPreview)
                    m_bSort = true; // show always sorted by channel
                BuildList();

                state = osContinue;
            }
            break;
        default:
            break;
        }
    }
    return state;
}

// --- cMenuSearchResultsForBlacklist -------------------------------------------------------
cMenuSearchResultsForBlacklist::cMenuSearchResultsForBlacklist(cBlacklist* Blacklist)
    : cMenuSearchResults(cTemplFile::GetTemplateByName("MenuSearchResults"))
{
    ButtonBlue[0] = tr("Button$all channels");
    ButtonBlue[1] = tr("Button$only FTA");
    ButtonBlue[2] = tr("Button$Timer preview");

    blacklist = Blacklist;
    m_bSort = true;
    modeBlue = blacklist->useChannel == 3 ? showNoPayTV : (EPGSearchConfig.ignorePayTV ? showNoPayTV : showAll);

    BuildList();
}

bool cMenuSearchResultsForBlacklist::BuildList()
{
    int current = Current();
    time_t now = time(NULL);
    cSearchResults* pSearchResults = blacklist->Run();
    Clear();
    eventObjects.Clear();
    if (pSearchResults) {
        pSearchResults->SortBy(m_bSort ? CompareEventTime : CompareEventChannel);

        for (cSearchResult* pResultObj = pSearchResults->First();
             pResultObj;
             pResultObj = pSearchResults->Next(pResultObj)) {
            if (ignoreRunning && now > pResultObj->event->StartTime())
                continue;
            Add(new cMenuSearchResultsItem(pResultObj->event, modeYellow == showEpisode, false));
            eventObjects.Add(pResultObj->event);
        }

        delete pSearchResults;
    }
    if (Count())
        SetCurrent(Get(0));
    SetHelpKeys();
    cString szTitle = cString::sprintf("%d %s - %s", Count(), tr("Blacklist results"), blacklist->search);
    SetTitle(szTitle);

    SetCurrent(Get(current));
    Display();

    return true;
}

eOSState cMenuSearchResultsForBlacklist::ProcessKey(eKeys Key)
{
    eOSState state = cMenuSearchResults::ProcessKey(Key);

    if (state == osUnknown) {
        switch (Key) {
        case k1...k9:
            state = HasSubMenu() ? osContinue : Commands(Key);
            break;
        case kRecord:
        case kRed:
            state = OnRed();
            break;
        case kBlue:
            if (HasSubMenu())
                state = Switch();
            else
                state = osContinue;
            break;
        default:
            break;
        }
    }
    return state;
}

void cMenuSearchResultsForBlacklist::SetHelpKeys(bool Force)
{
    cMenuSearchResultsItem *item = (cMenuSearchResultsItem *)Get(Current());
    int NewHelpKeys = 0;
    if (item) {
        if (item->Selectable() && item->timerMatch == tmFull)
            NewHelpKeys = 2;
        else
            NewHelpKeys = 1;
    }

    bool hasTimer = (NewHelpKeys == 2);
    if (NewHelpKeys != helpKeys || Force) {

        ModeBlueSR nextModeBlue = (ModeBlueSR)(((int)modeBlue + 1) % 3);
        if (nextModeBlue == showTimerPreview)
            nextModeBlue = (ModeBlueSR)(((int)nextModeBlue + 1) % 3);

        if (toggleKeys == 0)
            SetHelp((EPGSearchConfig.redkeymode == 0 ? (hasTimer ? trVDR("Button$Timer") : trVDR("Button$Record")) : tr("Button$Commands")), m_bSort ? tr("Button$by channel") : tr("Button$by time"), modeYellow == showTitleEpisode ? tr("Button$Episode") : tr("Button$Title"), NULL);
        else
            SetHelp((EPGSearchConfig.redkeymode == 1 ? (hasTimer ? trVDR("Button$Timer") : trVDR("Button$Record")) : tr("Button$Commands")), m_bSort ? tr("Button$by channel") : tr("Button$by time"), modeYellow == showTitleEpisode ? tr("Button$Episode") : tr("Button$Title"), NULL);
        helpKeys = NewHelpKeys;
    }
}

// --- cMenuSearchResultsForQuery -------------------------------------------------------
cMenuSearchResultsForQuery::cMenuSearchResultsForQuery(const char *query, bool IgnoreRunning)
    : cMenuSearchResultsForSearch(NULL, cTemplFile::GetTemplateByName("MenuSearchResults"))
{
    modeBlue = EPGSearchConfig.ignorePayTV ? showNoPayTV : showAll;
    ignoreRunning = IgnoreRunning;
    // create a dummy search
    if (query) {
        searchExt = new cSearchExt;
        strcpy(searchExt->search, query);
        searchExt->mode = 0; // substring
        searchExt->useTitle = 1;
        searchExt->useSubtitle = 0;
        searchExt->useDescription = 0;
        searchExt->blacklistMode = blacklistsNone;
        BuildList();
    }
}

cMenuSearchResultsForQuery::~cMenuSearchResultsForQuery()
{
    delete searchExt;
}

bool cMenuSearchResultsForQuery::BuildList()
{
    bool bRes = cMenuSearchResultsForSearch::BuildList();
    /*   if (!bRes)
       {
          char* szMessage = NULL;
          asprintf(&szMessage, tr("No results! Try again with tolerance %d?"), searchExt->mode == 5?searchExt->fuzzyTolerance+1:1);
          string sMessage = szMessage;
          free(szMessage);
          if (Interface->Confirm(sMessage.c_str()))
          {
             if (searchExt->mode == 5) // fuzzy
                searchExt->fuzzyTolerance++;
             searchExt->mode = 5;
             return BuildList();
          }
       }
    */   return bRes;
}

// --- cMenuSearchResultsForRecs -------------------------------------------------------
cMenuSearchResultsForRecs::cMenuSearchResultsForRecs(const char *query)
    : cMenuSearchResultsForQuery(NULL)
{
    SetMenuCategory(mcCommand);
    SetTitle(tr("found recordings"));
    if (query) {
        searchExt = new cSearchExt;
        strcpy(searchExt->search, query);
        searchExt->mode = 0; // substring
        searchExt->useTitle = 1;
        searchExt->useSubtitle = 0;
        searchExt->useDescription = 0;
        BuildList();
    }
}

bool cMenuSearchResultsForRecs::BuildList()
{
    const cRecording **pArray = NULL;
    int num = 0;

    int current = Current();
    Clear();
    {
        LOCK_RECORDINGS_READ;
        for (const cRecording *recording = Recordings->First(); recording; recording = Recordings->Next(recording)) {
            const cRecordingInfo *recInfo = recording->Info();
            if (!recInfo) continue;
            string s1 = (recInfo && recInfo->Title()) ? recInfo->Title() : "";
            string s2 = searchExt->search;
            if (s1.empty() || s2.empty()) continue;

            // tolerance for fuzzy searching: 90% of the shorter text length, but at least 1
            int tolerance = std::max(1, (int)std::min(s1.size(), s2.size()) / 10);

            bool match = FindIgnoreCase(s1, s2) >= 0 ||
                         FindIgnoreCase(s2, s1) >= 0;

            if (!match) {
                AFUZZY af = { NULL, NULL, NULL, NULL, NULL, NULL, { 0 }, { 0 }, 0, 0, 0, 0, 0, 0 };
                if (s1.size() > 32) s1 = s1.substr(0, 32);
                afuzzy_init(s1.c_str(), tolerance, 0, &af);
                /* Checking substring */
                int res = afuzzy_checkSUB(s2.c_str(), &af);
                afuzzy_free(&af);
                match = (res > 0);
            }
            if (!match) {
                AFUZZY af = { NULL, NULL, NULL, NULL, NULL, NULL, { 0 }, { 0 }, 0, 0, 0, 0, 0, 0 };
                if (s2.size() > 32) s2 = s2.substr(0, 32);
                afuzzy_init(s2.c_str(), tolerance, 0, &af);
                /* Checking substring */
                int res = afuzzy_checkSUB(s1.c_str(), &af);
                afuzzy_free(&af);
                match = (res > 0);
            }

            if (match) {
                const cRecording **tmp = (const cRecording **)realloc(pArray, (num + 1) * sizeof(cRecording *));
                if (tmp) {
                    pArray = tmp;
                    pArray[num++] = recording;
                }
            }
        }

        qsort(pArray, num, sizeof(cRecording *), CompareRecording);
        for (int a = 0; a < num; a++)
            Add(new cMenuSearchResultsItem(pArray[a]));
        delete pArray;
    } // Recordinglock must be released before Display() is called

    SetHelp(NULL);

    SetCurrent(Get(current));
    Display();
    return true;
}

const cRecording *cMenuSearchResultsForRecs::GetRecording(cMenuSearchResultsItem *Item)
{
    LOCK_RECORDINGS_READ;
    const cRecording *recording = Recordings->GetByName(Item->FileName());
    if (!recording)
        ERROR(tr("Error while accessing recording!"));
    return recording;
}

eOSState cMenuSearchResultsForRecs::Play(void)
{
    cMenuSearchResultsItem *ri = (cMenuSearchResultsItem*)Get(Current());
    if (ri) {
        const cRecording *recording = GetRecording(ri);
        if (recording) {
            cReplayControl::SetRecording(recording->FileName());
            return osReplay;
        }
    }
    return osContinue;
}

eOSState cMenuSearchResultsForRecs::ProcessKey(eKeys Key)
{
    eOSState state = cOsdMenu::ProcessKey(Key);

    if (state == osUnknown) {
        if (Key == kOk) {
            if (Count() > 0)
                state = Play();
            else
                state = osBack;
        } else
            state = osContinue;
    }
    return state;
}

// --- cMenuSearchResultsForList -------------------------------------------------------
cMenuSearchResultsForList::cMenuSearchResultsForList(cSearchResults& SearchResults, const char* Title, bool IgnoreRunning)
    : cMenuSearchResults(cTemplFile::GetTemplateByName("MenuSearchResults"))
{
    ButtonBlue[0] = tr("Button$Setup");
    searchResults = &SearchResults;
    m_bSort = true;
    ignoreRunning = IgnoreRunning;

    BuildList();

    cString szTitle = cString::sprintf(Title, Count());
    SetTitle(szTitle);
}

void cMenuSearchResultsForList::SetHelpKeys(bool Force)
{
    cMenuSearchResultsItem *item = (cMenuSearchResultsItem *)Get(Current());
    int NewHelpKeys = 0;
    if (item) {
        if (item->Selectable() && item->timerMatch == tmFull)
            NewHelpKeys = 2;
        else
            NewHelpKeys = 1;
    }

    bool hasTimer = (NewHelpKeys == 2);
    if (NewHelpKeys != helpKeys || Force) {
        if (toggleKeys == 0)
            SetHelp((EPGSearchConfig.redkeymode == 0 ? (hasTimer ? trVDR("Button$Timer") : trVDR("Button$Record")) : tr("Button$Commands")), m_bSort ? tr("Button$by channel") : tr("Button$by time"), modeYellow == showTitleEpisode ? tr("Button$Episode") : tr("Button$Title"), ButtonBlue[0]);
        else
            SetHelp((EPGSearchConfig.redkeymode == 1 ? (hasTimer ? trVDR("Button$Timer") : trVDR("Button$Record")) : tr("Button$Commands")), m_bSort ? tr("Button$by channel") : tr("Button$by time"), modeYellow == showTitleEpisode ? tr("Button$Episode") : tr("Button$Title"), ButtonBlue[0]);
        helpKeys = NewHelpKeys;
    }
}

bool cMenuSearchResultsForList::BuildList()
{
    time_t now = time(NULL);
    int current = Current();

    Clear();
    eventObjects.Clear();
    searchResults->SortBy(m_bSort ? CompareEventTime : CompareEventChannel);

    for (cSearchResult* pResultObj = searchResults->First();
         pResultObj;
         pResultObj = searchResults->Next(pResultObj)) {
        if (ignoreRunning && now > pResultObj->event->StartTime())
            continue;
        Add(new cMenuSearchResultsItem(pResultObj->event, modeYellow == showEpisode, false, menuTemplate, pResultObj->search));
        eventObjects.Add(pResultObj->event);
    }
    if (Count())
        SetCurrent(Get(0));

    SetHelpKeys();

    SetCurrent(Get(current));
    Display();

    return true;
}

eOSState cMenuSearchResultsForList::ProcessKey(eKeys Key)
{
    eOSState state = cMenuSearchResults::ProcessKey(Key);

    if (state == osUnknown) {
        switch (Key) {
        case k1...k9:
            state = HasSubMenu() ? osContinue : Commands(Key);
            break;
        case kRecord:
        case kRed:
            state = OnRed();
            break;
        default:
            break;
        }
    }
    return state;
}
