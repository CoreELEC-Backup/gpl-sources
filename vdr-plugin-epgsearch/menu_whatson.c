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
#include <iostream>
#include <sstream>
#include <string>
#include "uservars.h"
#include "menu_whatson.h"
#include "epgsearchext.h"
#include "menu_event.h"
#include "menu_myedittimer.h"
#include "menu_searchresults.h"
#include "menu_search.h"
#include "menu_commands.h"
#include "epgsearchcfg.h"
#include "switchtimer.h"
#include "epgsearchcats.h"
#include "conflictcheck.h"
#include <math.h>
#include <vdr/menu.h>
#include "menu_conflictcheck.h"
#include "templatefile.h"
#include "menu_deftimercheckmethod.h"
#include "timerstatus.h"

#define HOURS(x) ((x)/100)
#define MINUTES(x) ((x)%100)

#define SKIPHOURS 20
#define HOURS2SECS(x) (x*60*60)

extern int exitToMainMenu;
extern bool isUTF8;
int gl_InfoConflict = 0;

// --- cMenuMyScheduleItem ------------------------------------------------------
cMenuMyScheduleItem::cMenuMyScheduleItem(const cTimers *Timers, const cEvent *Event, const cChannel *Channel, showMode Mode, cMenuTemplate* MenuTemplate)
{
    event = Event;
    channel = Channel;
    mode = Mode;
    timerMatch = tmNone;
    isRemote = false;
    inSwitchList = false;
    timerActive = false;
    menuTemplate = MenuTemplate;
    Update(Timers, true);
}

bool cMenuMyScheduleItem::Update(const cTimers* Timers, bool Force)
{
    if (!menuTemplate)
        return false;

    const char* menutemplate = menuTemplate->MenuTemplate();
    if (!menutemplate || strlen(menutemplate) == 0)
        return false;

    bool result = false;

    eTimerMatch OldTimerMatch = timerMatch;
    bool OldIsRemote = isRemote;
    bool OldInSwitchList = inSwitchList;
    bool OldtimerActive = timerActive;
    bool hasMatch = false;
    const cTimer* timer = NULL;
    if (event) timer = Timers->GetMatch(event, &timerMatch);
    if (event) inSwitchList = (SwitchTimers.InSwitchList(event) != NULL);
    if (timer) hasMatch = true;
    if (timer) timerActive = timer->HasFlags(tfActive);
    if (timer) isRemote = timer->Remote();

    if (Force || timerMatch != OldTimerMatch || inSwitchList != OldInSwitchList
        || isRemote != OldIsRemote || timerActive != OldtimerActive) {
        char szProgressPart[Utf8BufSize(12)] = "";
        char szProgressPartT2S[12] = "";
        time_t now = time(NULL);
        if (channel) {
            if (event) {
                time_t startTime = event->StartTime();
                if ((now - event->StartTime()) >= 0 || strstr(menutemplate, "%time%") != NULL) {
                    int frac = 0;
                    if (mode == showNow) {
                        int dur = event->Duration();
                        if (dur != 0)
                            frac = ((now - startTime) * 8 + (dur >> 1)) / dur;
                    }
                    if (mode == showNext)
                        frac = ((30 * 60 - std::min((time_t)30 * 60, startTime - now)) * 8 + 15 * 60) / (30 * 60);

                    frac = std::min(8, std::max(0, frac));

                    szProgressPartT2S[0] = '[';
                    memset(szProgressPartT2S + 1, '|', frac);
                    memset(szProgressPartT2S + 1 + frac , ' ', 8 - frac);
                    szProgressPartT2S[9] = ']';
                    szProgressPartT2S[10] = 0;

                    if (!isUTF8) {
                        szProgressPart[0] = ICON_BAR_OPEN;
                        memset(szProgressPart + 1, ICON_BAR_EMPTY, 6);
                        szProgressPart[7] = ICON_BAR_CLOSE;
                        szProgressPart[8] = 0;
                        memset(szProgressPart, ICON_BAR_FULL, frac ? frac : sizeof(szProgressPart));
                    } else {
#if defined(__GNUC__) && __GNUC__ < 3 && __GNUC_MINOR__ < 96
#else
                        std::stringstream buffer;
                        buffer << ICON_BAR_OPEN_UTF8;
                        for (int i = 0; i < 8; i++) buffer << (i < frac ? ICON_BAR_FULL_UTF8 : ICON_BAR_EMPTY_UTF8);
                        buffer << ICON_BAR_CLOSE_UTF8;
                        char* temp = strdup(buffer.str().c_str());
                        sprintf(szProgressPart, "%s", temp);
                        free(temp);
#endif
                    }
                } else {
                    strncpy(szProgressPart, *event->GetTimeString(), 12);
                    szProgressPart[11] = 0;
                    memcpy(szProgressPartT2S, szProgressPart, 12);
                }
            }
        }

        char t[Utf8BufSize(2)], v[Utf8BufSize(2)], r[Utf8BufSize(2)];
        char szStatus[Utf8BufSize(4)];
        szStatus[3] = 0;
        t[1] = v[1] = r[1] = 0;

        if (EPGSearchConfig.WarEagle) {
            if (!isUTF8) {
                t[0] = event && hasMatch ? (timerMatch == tmFull) ? ((timer && timer->Recording()) ? ICON_REC : (timerActive ? ICON_CLOCK : ICON_TIMER_INACT)) : (timerActive ? ICON_CLOCK_HALF : ' ') : ' ';
                v[0] = event && event->Vps() && (event->Vps() - event->StartTime()) ? ICON_VPS : ' ';
                r[0] = event && event->IsRunning() ? ICON_RUNNING : ' ';
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
            v[0] = event && event->Vps() && (event->Vps() - event->StartTime()) ? 'V' : ' ';
            r[0] = event && event->IsRunning() ? '*' : ' ';
        }

        if (event && inSwitchList) {
            cSwitchTimer* s = SwitchTimers.InSwitchList(event);
            t[0] = (s && s->mode == 1) ? 's' : 'S';
        }
        if (EPGSearchConfig.WarEagle && isUTF8) {
            std::stringstream buffer;
            buffer << t << v << r;
            char* temp = strdup(buffer.str().c_str());
            sprintf(szStatus, "%s", temp);
            free(temp);
        } else {
            szStatus[0] = t[0];
            szStatus[1] = v[0];
            szStatus[2] = r[0];
        }

        char* buffer = strdup(menutemplate);
        strreplace(buffer, '|', '\t');

        char* title = NULL;
        title = strdup(event ? event->Title() : tr(">>> no info! <<<"));

        title = strreplacei(title, ":", "%colon%"); // assume a title has the form "a?b:c",
        // we need to replace the colon to avoid misinterpretation of the expression as a condition
        buffer = strreplacei(buffer, "%title%", title);
        free(title);

        if (channel) {
            char szChannelNr[6] = "";
            snprintf(szChannelNr, 6, "%d", channel->Number());
            buffer = strreplacei(buffer, "%chnr%", szChannelNr);
            buffer = strreplacei(buffer, "%chsh%", channel->ShortName(true));
            buffer = strreplacei(buffer, "%chlng%", channel->Name());
            buffer = strreplacei(buffer, "%progr%", szProgressPart);
            buffer = strreplacei(buffer, "%progrT2S%", szProgressPartT2S);
        }

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

        if (gl_InfoConflict == 0 && EPGSearchConfig.checkTimerConflAfterTimerProg && !Force && timer && ((timerMatch && timerMatch != OldTimerMatch) || (isRemote != OldIsRemote))) {
            cConflictCheck C;
            C.Check();
            if (C.TimerInConflict(timer))
                gl_InfoConflict = 1;
        }
        return true;
    }
    return result;
}

void cMenuMyScheduleItem::SetMenuItem(cSkinDisplayMenu *DisplayMenu, int Index, bool Current, bool Selectable)
{
    bool withDate = (channel == NULL); // search for a better way to determine this
    if (!DisplayMenu->SetItemEvent(event, Index, Current, Selectable, channel, withDate, timerMatch))
        DisplayMenu->SetItem(Text(), Index, Current, Selectable);
}

// --- cMenuMyScheduleSepItem ------------------------------------------------------
cMenuMyScheduleSepItem::cMenuMyScheduleSepItem(const cTimers *Timers, const cEvent *Event, const cChannel *Channel)
    : cMenuMyScheduleItem(Timers, Event, Channel, showNow, NULL)
{
    event = Event;
    channel = Channel;
    dummyEvent = NULL;
    SetSelectable(false);
    Update(Timers, true);
}

cMenuMyScheduleSepItem::~cMenuMyScheduleSepItem()
{
    if (dummyEvent)
        delete dummyEvent;
}

bool cMenuMyScheduleSepItem::Update(const cTimers *Timers, bool Force)
{
    if (channel)
        SetText(cString::sprintf("%s\t %s %s", MENU_SEPARATOR_ITEMS, channel->Name(), MENU_SEPARATOR_ITEMS));
    else if (event) {
        dummyEvent = new cEvent(0);
        dummyEvent->SetTitle(cString::sprintf("%s\t %s %s", MENU_SEPARATOR_ITEMS, GETDATESTRING(event), MENU_SEPARATOR_ITEMS));
        SetText(dummyEvent->Title());
    }
    return true;
}

void cMenuMyScheduleSepItem::SetMenuItem(cSkinDisplayMenu *DisplayMenu, int Index, bool Current, bool Selectable)
{
    bool withDate = (channel == NULL); // search for a better way to determine this
    if (!DisplayMenu->SetItemEvent(dummyEvent, Index, Current, Selectable, channel, withDate, timerMatch))
        DisplayMenu->SetItem(Text(), Index, Current, Selectable);
}


// --- cMenuWhatsOnSearch ----------------------------------------------------------

int cMenuWhatsOnSearch::currentChannel = 0;
showMode cMenuWhatsOnSearch::currentShowMode = showNow;
const cChannel *cMenuWhatsOnSearch::scheduleChannel = NULL;
extern const char *ShowModes[];
cList<cShowMode> cMenuWhatsOnSearch::showModes;
time_t cMenuWhatsOnSearch::seekTime = 0;
int cMenuWhatsOnSearch::shiftTime = 0;

cMenuWhatsOnSearch::cMenuWhatsOnSearch(int CurrentChannelNr)
    : cOsdMenu("", GetTab(1), GetTab(2), GetTab(3), GetTab(4), GetTab(5))
{
    if (currentShowMode == showNow)
        SetMenuCategory(mcScheduleNow);
    else if (currentShowMode == showNext)
        SetMenuCategory(mcScheduleNext);
    else
        SetMenuCategory(mcSchedule);

    helpKeys = -1;
    shiftTime = 0;

    CreateShowModes();

    LoadSchedules();

    currentChannel = CurrentChannelNr;

    SetHelpKeys();
}

cMenuWhatsOnSearch::~cMenuWhatsOnSearch()
{
}

#ifdef USE_GRAPHTFT
const char* cMenuWhatsOnSearch::MenuKind()
{
    if (currentShowMode == showNow)  return "MenuEpgsWhatsOnNow";
    if (currentShowMode == showNext) return "MenuEpgsWhatsOnNext";
    if (currentShowMode > showNext)  return "MenuEpgsWhatsOnElse";
    else return "MenuWhatsOnElse";
}

void cMenuWhatsOnSearch::Display(void)
{
    cOsdMenu::Display();

    if (Count() > 0) {
        int i = 0;

        for (cOsdItem *item = First(); item; item = Next(item))
            cStatus::MsgOsdEventItem(!item->Selectable() ? 0 :
                                     ((cMenuMyScheduleItem*)item)->event,
                                     item->Text(), i++, Count());
    }
}
#endif /* GRAPHTFT */

int cMenuWhatsOnSearch::GetTab(int Tab)
{
    if (currentShowMode == showNow)
        return cTemplFile::GetTemplateByName("MenuWhatsOnNow")->Tab(Tab - 1);
    if (currentShowMode == showNext)
        return cTemplFile::GetTemplateByName("MenuWhatsOnNext")->Tab(Tab - 1);
    if (currentShowMode > showNext)
        return cTemplFile::GetTemplateByName("MenuWhatsOnElse")->Tab(Tab - 1);
    else
        return 0;
}

void cMenuWhatsOnSearch::LoadSchedules()
{
    Clear();
    eventObjects.Clear();

    //   time_t SeekTime;
    cString szTitle;
    cShowMode* mode = GetShowMode(currentShowMode);
    if (!mode) return;

    if (shiftTime != 0) {
        if (currentShowMode == showNow || currentShowMode == showNext)
            seekTime = time(NULL);
        else {
            if (mode)
                seekTime = GetTimeT(mode->GetTime());
            if (seekTime < time(NULL)) seekTime += HOURS2SECS(24);
        }
        seekTime += shiftTime * 60;

        struct tm tm_r;
        time_t now = time(NULL);

        tm tm_seek = *localtime_r(&seekTime, &tm_r);
        tm tm_now = *localtime_r(&now, &tm_r);
        if (tm_seek.tm_mday != tm_now.tm_mday)
            szTitle = cString::sprintf("%s - %s", tr("Overview"), DAYDATETIME(seekTime));
        else
            szTitle = cString::sprintf("%s - %02d:%02d", tr("Overview"), tm_seek.tm_hour, tm_seek.tm_min);
    } else {
        seekTime = GetTimeT(mode->GetTime());
        if (seekTime < time(NULL) && currentShowMode != showNow && currentShowMode != showNext) {
            seekTime += HOURS2SECS(24);
            szTitle = cString::sprintf("%s - %s (%s)", tr("Overview"), mode->GetDescription(), *WeekDayName(seekTime));
        } else
            szTitle = cString::sprintf("%s - %s", tr("Overview"), mode->GetDescription());
    }
    SetTitle(szTitle);

    cMenuTemplate* currentTemplate = NULL;
    if (currentShowMode == showNow)
        currentTemplate = cTemplFile::GetTemplateByName("MenuWhatsOnNow");
    if (currentShowMode == showNext)
        currentTemplate = cTemplFile::GetTemplateByName("MenuWhatsOnNext");
    if (currentShowMode > showNext)
        currentTemplate = cTemplFile::GetTemplateByName("MenuWhatsOnElse");

    int maxChannel = EPGSearchConfig.maxChannelMenuNow;
    if (currentChannel > maxChannel)
        maxChannel = 0;

    LOCK_TIMERS_READ; // needed in MyScheduleItem
    LOCK_CHANNELS_READ;
    for (const cChannel *Channel = Channels->First(); Channel; Channel = Channels->Next(Channel)) {
        if (!Channel->GroupSep()) {
            if (maxChannel && Channel->Number() > maxChannel) break;
            if (EPGSearchConfig.showRadioChannels == 0 && ISRADIO(Channel))
                continue;

            const cSchedule *Schedule;
            {
                LOCK_SCHEDULES_READ;
                Schedule = Schedules->GetSchedule(Channel);
            }
            const cEvent *Event = NULL;
            if (Schedule) {
                if (shiftTime != 0)
                    Event = Schedule->GetEventAround(seekTime);
                else {
                    switch (currentShowMode) {
                    default:
                    case showNow:
                        Event = Schedule->GetPresentEvent();
                        break;
                    case showNext:
                        Event = Schedule->GetFollowingEvent();
                        break;
                    case showUserMode1:
                    case showUserMode2:
                    case showUserMode3:
                    case showUserMode4:
                        Event = Schedule->GetEventAround(seekTime);
                        break;
                    }
                }
            }
            if (!EPGSearchConfig.showEmptyChannels && !Event)
                continue;

            Add(new cMenuMyScheduleItem(Timers, Event, Channel, currentShowMode, currentTemplate), Channel->Number() == currentChannel);
            if (Event) eventObjects.Add(Event);
        } else {
            if (EPGSearchConfig.showChannelGroups && strlen(Channel->Name()))
                Add(new cMenuMyScheduleSepItem(NULL, NULL, Channel));
        }
    }
}

time_t cMenuWhatsOnSearch::GetTimeT(int iTime)
{
    struct tm tm_r;
    time_t t = time(NULL);
    tm* tmnow = localtime_r(&t, &tm_r);
    tmnow->tm_hour = HOURS(iTime);
    tmnow->tm_min = MINUTES(iTime);
    return mktime(tmnow);
}

showMode cMenuWhatsOnSearch::GetNextMode()
{
    showMode nextShowMode = currentShowMode;
    cShowMode* Mode = GetShowMode(currentShowMode);
    if (Mode) {
        cShowMode* ModeNext = showModes.Next(Mode);
        if (ModeNext == NULL)
            nextShowMode = showNow;
        else
            nextShowMode = ModeNext->GetMode();
    } else // no mode found? fall back to 'now'
        nextShowMode = showNow;
    return nextShowMode;
}

void cMenuWhatsOnSearch::CreateShowModes()
{
    showModes.Clear();

    cShowMode* ModeNow = new cShowMode(showNow, trVDR("Button$Now"));
    showModes.Add(ModeNow);
    cShowMode* ModeNext = new cShowMode(showNext, trVDR("Button$Next"));
    showModes.Add(ModeNext);

    time_t now = time(NULL);
    for (int i = showUserMode1; i < showModeMax; i++) {
        if (!EPGSearchConfig.ShowModes[i].GetUsage())
            continue;

        time_t SeekTime = GetTimeT(EPGSearchConfig.ShowModes[i].GetTime());
        if (SeekTime < now)
            SeekTime += HOURS2SECS(24);
        if (SeekTime - now > HOURS2SECS(SKIPHOURS))
            continue;
        cShowMode* Mode = new cShowMode((showMode)i, EPGSearchConfig.ShowModes[i].GetDescription(),
                                        1, EPGSearchConfig.ShowModes[i].GetTime(), SeekTime);
        showModes.Add(Mode);
    }
    if (EPGSearchConfig.showFavoritesMenu) {
        cShowMode* ModeFav = new cShowMode(showFavorites, tr("Button$Favorites"));
        showModes.Add(ModeFav);
    }
    showModes.Sort();
}

cShowMode* cMenuWhatsOnSearch::GetShowMode(showMode mode)
{
    for (cShowMode *showMode = showModes.First(); showMode; showMode = showModes.Next(showMode))
        if (mode == showMode->GetMode())
            return showMode;
    return NULL;
}

const cChannel *cMenuWhatsOnSearch::ScheduleChannel(const cChannel *force_channel)
{
    const cChannel *ch = force_channel ? force_channel : scheduleChannel;
    scheduleChannel = NULL;
    return ch;
}


eOSState cMenuWhatsOnSearch::Switch(void)
{
    cMenuMyScheduleItem *item = (cMenuMyScheduleItem *)Get(Current());
    if (item && item->channel) {
        if (cDevice::PrimaryDevice()->SwitchChannel(item->channel, true))
            return osEnd;
    }
    INFO(trVDR("Can't switch channel!"));
    return osContinue;
}

eOSState cMenuWhatsOnSearch::Record(void)
{
    cMenuMyScheduleItem *item = (cMenuMyScheduleItem *)Get(Current());
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
                    return AddSubMenu(new cMenuMyEditTimer(timer, false, item->event, item->channel));
            }
        }

        cTimer *timer = NULL;
        if (item->event) {
            timer = new cTimer(item->event);
            PrepareTimerFile(item->event, timer);
        } else
            timer = new cTimer(false, false, item->channel);

        cTimer *t = Timers->GetTimer(timer);
        if (EPGSearchConfig.onePressTimerCreation == 0 || t || !item->event || (!t && item->event && item->event->StartTime() - (Setup.MarginStart + 2) * 60 < time(NULL))) {
            if (t) {
                delete timer;
                timer = t;
            }
            timer->SetFlags(tfActive);
            if (EPGSearchConfig.useVDRTimerEditMenu)
                return AddSubMenu(new cMenuEditTimer(timer, !t));
            else
                return AddSubMenu(new cMenuMyEditTimer(timer, !t, item->event, item->channel));
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
            if (!HandleRemoteTimerModifications(timer)) {
                Timers->Del(timer);
                ERROR(tr("Epgsearch: RemoteTimerModifications failed"));
            } else {
                gl_timerStatusMonitor->SetConflictCheckAdvised();
                timer->Matches();
                Timers->SetModified();
                LogFile.iSysLog("timer %s added (active)", *timer->ToDescr());
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

bool cMenuWhatsOnSearch::Update(void)
{
    bool result = false;
    LOCK_TIMERS_READ;
    for (cOsdItem *item = First(); item; item = Next(item)) {
        if (item->Selectable() && ((cMenuMyScheduleItem *)item)->Update(Timers))
            result = true;
    }
    return result;
}

eOSState cMenuWhatsOnSearch::Commands(eKeys Key)
{
    if (HasSubMenu() || Count() == 0)
        return osContinue;

    cMenuMyScheduleItem *mi = (cMenuMyScheduleItem *)Get(Current());
    if (mi) {
        if (!mi->event) {
            if (Key == k3)
                return Switch();
            else if (Key == k2)
                return Record();
            else
                return osContinue;
        }
        cMenuSearchCommands *menu;
        eOSState state = AddSubMenu(menu = new cMenuSearchCommands(tr("EPG Commands"), mi->event, true));
        if (Key != kNone)
            state = menu->ProcessKey(Key);
        return state;
    }
    return osContinue;
}

eOSState cMenuWhatsOnSearch::ExtendedSearch(void)
{
    return AddSubMenu(new cMenuEPGSearchExt());
}

void cMenuWhatsOnSearch::SetHelpKeys(bool Force)
{
    cMenuMyScheduleItem *item = (cMenuMyScheduleItem *)Get(Current());
    int NewHelpKeys = 0;
    if (item) {
        if (item->Selectable() && item->timerMatch == tmFull)
            NewHelpKeys = 2;
        else
            NewHelpKeys = 1;
    }

    bool hasTimer = (NewHelpKeys == 2);
    if (NewHelpKeys != helpKeys || Force) {
        showMode nextShowMode = GetNextMode();
        cShowMode* mode = GetShowMode(nextShowMode);
        const char* szButtonGreen = NULL;
        if (mode)
            szButtonGreen = mode->GetDescription();
        if (toggleKeys == 0)
            SetHelp((EPGSearchConfig.redkeymode == 0 ? (hasTimer ? trVDR("Button$Timer") : trVDR("Button$Record")) : tr("Button$Commands")),
                        szButtonGreen,
                        trVDR("Button$Schedule"),
                        EPGSearchConfig.bluekeymode == 0 ? (EPGSearchConfig.useOkForSwitch ? trVDR("Button$Info") : trVDR("Button$Switch")) : tr("Button$Search"));
        else
            SetHelp((EPGSearchConfig.redkeymode == 1 ? (hasTimer ? trVDR("Button$Timer") : trVDR("Button$Record")) : tr("Button$Commands")),
                        (EPGSearchConfig.toggleGreenYellow == 0 ? szButtonGreen : "<<"),
                        (EPGSearchConfig.toggleGreenYellow == 0 ? trVDR("Button$Schedule") : ">>"),
                        EPGSearchConfig.bluekeymode == 1 ? (EPGSearchConfig.useOkForSwitch ? trVDR("Button$Info") : trVDR("Button$Switch")) : tr("Button$Search"));
        helpKeys = NewHelpKeys;
    }
}

eOSState cMenuWhatsOnSearch::Shift(int iMinutes)
{
    shiftTime += iMinutes;
    cMenuMyScheduleItem *mi = (cMenuMyScheduleItem *)Get(Current());
    int TempChannel = currentChannel;
    if (mi) {
        currentChannel = mi->channel->Number();
        LOCK_CHANNELS_READ;
        scheduleChannel = Channels->GetByNumber(currentChannel);
    }
    LoadSchedules();
    Display();
    currentChannel = TempChannel;
    SetHelpKeys();
    return osContinue;
}

eOSState cMenuWhatsOnSearch::ShowSummary()
{
    if (Count()) {
        const cEvent *ei = ((cMenuMyScheduleItem *)Get(Current()))->event;
        if (ei) {
            const cChannel *channel;
            {
                LOCK_CHANNELS_READ;
                channel = Channels->GetByChannelID(ei->ChannelID(), true, true);
            }
            if (channel)
                return AddSubMenu(new cMenuEventSearch(ei, eventObjects, SurfModeChannel));
        }
    }
    return osContinue;
}

void cMenuWhatsOnSearch::UpdateCurrent()
{
    // navigation in summary could have changed current item, so update it
    cEventObj* cureventObj = eventObjects.GetCurrent();
    if (cureventObj && cureventObj->Event())
        for (cMenuMyScheduleItem *item = (cMenuMyScheduleItem *)First(); item; item = (cMenuMyScheduleItem *)Next(item))
            if (item->Selectable() && item->event == cureventObj->Event()) {
                cureventObj->Select(false);
                SetCurrent(item);
                Display();
                break;
            }
}

eOSState cMenuWhatsOnSearch::ProcessKey(eKeys Key)
{
    exitToMainMenu = 0;
    if (!HasSubMenu() && Key == kBack) {
        exitToMainMenu = 1;
        return osBack;
    }

    bool HadSubMenu = HasSubMenu();
    eOSState state = cOsdMenu::ProcessKey(Key);

    if (!HasSubMenu() && HadSubMenu) // navigation in summary could have changed current item, so update it
        UpdateCurrent();

    if (state == osUnknown) {
        switch (Key) {
        case kFastRew:
            if (!HasSubMenu())
                return Shift(-EPGSearchConfig.timeShiftValue);
            break;
        case kFastFwd:
            if (!HasSubMenu())
                return Shift(EPGSearchConfig.timeShiftValue);
            break;
        case kRecord:
        case kRed:
            if (HasSubMenu()) {
                UpdateCurrent();
                state = Record();
                break;
            }
            if (Count()) {
                if (EPGSearchConfig.redkeymode == toggleKeys)
                    state = Record();
                else {
                    cMenuMyScheduleItem *mi = (cMenuMyScheduleItem *)Get(Current());
                    if (mi && mi->Selectable()) {
                        if (mi->event)
                            return AddSubMenu(new cMenuSearchCommands(tr("EPG Commands"), mi->event));
                        else
                            return osContinue;
                    }
                }
            }
            break;

        case k0:
            if (!HasSubMenu()) {
                toggleKeys = 1 - toggleKeys;
                SetHelpKeys(true);
            }
            state = osContinue;
            break;
        case k1...k9:
            return Commands(Key);
        case kYellow:
            if (!HasSubMenu()) {
                if (toggleKeys == 0 || (toggleKeys == 1 && EPGSearchConfig.toggleGreenYellow == 0)) {
                    cMenuMyScheduleItem *mi = (cMenuMyScheduleItem *)Get(Current());
                    if (mi && mi->Selectable() && mi->channel) {
                        LOCK_SCHEDULES_READ;
                        const cSchedule *Schedule = Schedules->GetSchedule(mi->channel);
                        if (Schedule) {
                            time_t now = time(NULL);
                            bool hasFutureEvents = false;
                            for (const cEvent *e = Schedule->Events()->First(); e; e = Schedule->Events()->Next(e))
                                if (e->StartTime() > now) {
                                    hasFutureEvents = true;
                                    break;
                                }
                            if (!hasFutureEvents)
                                return osContinue;
                        } else
                            return osContinue;
                    }
                    state = osBack;
                    // continue with kGreen
                } else
                    return Shift(EPGSearchConfig.timeShiftValue);
            }
        case kGreen:
            if (!HasSubMenu()) {
                if (toggleKeys == 0 || (toggleKeys == 1 && EPGSearchConfig.toggleGreenYellow == 0)) {
                    if (Key == kYellow)
                        currentShowMode = showNow;
                    else
                        currentShowMode = GetNextMode();
                    cMenuMyScheduleItem *mi = (cMenuMyScheduleItem *)Get(Current());
                    if (mi && mi->Selectable()) {
                        currentChannel = mi->channel->Number();
                        LOCK_CHANNELS_READ;
                        scheduleChannel = Channels->GetByNumber(currentChannel);
                    }
                } else
                    return Shift(-EPGSearchConfig.timeShiftValue);
            }
            break;
        case kBlue:
            if (HasSubMenu()) {
                UpdateCurrent();
                return Switch();
            }
            if (EPGSearchConfig.bluekeymode == toggleKeys)
                return EPGSearchConfig.useOkForSwitch ? ShowSummary() : Switch();
            else
                return ExtendedSearch();
            break;
        case kOk: {
            cMenuMyScheduleItem *mi = (cMenuMyScheduleItem *)Get(Current());
            if (mi && mi->Selectable()) {
                if (!mi->event) // no EPG, so simply switch to channel
                    return Switch();
                else
                    return EPGSearchConfig.useOkForSwitch ? Switch() : ShowSummary();
            }
        }
        break;
        case kInfo:
            return ShowSummary();
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
                return AddSubMenu(new cMenuConflictCheck());
        }
    }

    return state;
}


