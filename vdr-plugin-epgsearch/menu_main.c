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
#include "menu_main.h"
#include "menu_whatson.h"
#include "menu_myedittimer.h"
#include "epgsearchext.h"
#include "menu_event.h"
#include "menu_searchresults.h"
#include "menu_search.h"
#include "menu_commands.h"
#include "epgsearchcfg.h"
#include "epgsearchtools.h"
#include <vdr/menu.h>
#include "menu_conflictcheck.h"
#include "menu_favorites.h"
#include "menu_deftimercheckmethod.h"
#include "timerstatus.h"

int toggleKeys = 0;
int exitToMainMenu = 0;
extern int gl_InfoConflict;

int cMenuSearchMain::forceMenu = 0; // 1 = now, 2 = schedule, 3 = summary

// --- cMenuSearchMain ---------------------------------------------------------
cMenuSearchMain::cMenuSearchMain(void)
    : cOsdMenu("", GetTab(1), GetTab(2), GetTab(3), GetTab(4), GetTab(5))
{
    SetMenuCategory(mcSchedule);
    helpKeys = -1;
    otherChannel = 0;
    toggleKeys = 0;
    shiftTime = 0;
    InWhatsOnMenu = false;
    InFavoritesMenu = false;
    const cChannel *channel;
    {
        LOCK_CHANNELS_READ;
        channel = Channels->GetByNumber(cDevice::CurrentChannel());
    }
    if (channel) {
        cMenuWhatsOnSearch::SetCurrentChannel(channel->Number());
        if (EPGSearchConfig.StartMenu == 0 || forceMenu != 0)
            PrepareSchedule(channel);
        SetHelpKeys();
        cMenuWhatsOnSearch::currentShowMode = showNow;
        //    timeb tnow;
        //ftime(&tnow);
        //isyslog("duration epgs sched:  %d", tnow.millitm - gl_time.millitm + ((tnow.millitm - gl_time.millitm<0)?1000:0));

    }
    if ((EPGSearchConfig.StartMenu == 1 || forceMenu == 1) && forceMenu != 2) {
        InWhatsOnMenu = true;
        AddSubMenu(new cMenuWhatsOnSearch(cDevice::CurrentChannel()));
    }
    if (forceMenu == 3)
        ShowSummary();
}

#ifdef USE_GRAPHTFT
void cMenuSearchMain::Display(void)
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

cMenuSearchMain::~cMenuSearchMain()
{
    cMenuWhatsOnSearch::ScheduleChannel(); // makes sure any posted data is cleared
    forceMenu = 0;
}

int cMenuSearchMain::GetTab(int Tab)
{
    return cTemplFile::GetTemplateByName("MenuSchedule")->Tab(Tab - 1);
}

void cMenuSearchMain::PrepareSchedule(const cChannel *Channel)
{
    Clear();
    cString buffer = cString::sprintf("%s - %s", trVDR("Schedule"), Channel->Name());
    SetTitle(buffer);

    cMenuTemplate* ScheduleTemplate = cTemplFile::GetTemplateByName("MenuSchedule");
    eventObjects.Clear();

    LOCK_TIMERS_READ;
    const cSchedule *Schedule;
    {
        LOCK_SCHEDULES_READ;
        Schedule = Schedules->GetSchedule(Channel);
    }
    currentChannel = Channel->Number();
    if (Schedule && Schedule->Events()->First()) {
        const cEvent *PresentEvent = Schedule->GetPresentEvent();
        time_t now = time(NULL);
        now -= Setup.EPGLinger * 60;
        if (shiftTime != 0)
            PresentEvent = Schedule->GetEventAround(time(NULL) + shiftTime * 60);

        time_t lastEventDate = Schedule->Events()->First()->StartTime();

        //          timeb tstart;
        // ftime(&tstart);
        for (const cEvent *Event = Schedule->Events()->First(); Event; Event = Schedule->Events()->Next(Event)) {
            if (Event->EndTime() > now || (shiftTime == 0 && Event == PresentEvent)) {
                if (EPGSearchConfig.showDaySeparators) {
                    struct tm tm_rEvent;
                    struct tm tm_rLastEvent;
                    time_t EventDate = Event->StartTime();
                    struct tm *t_event = localtime_r(&EventDate, &tm_rEvent);
                    struct tm *t_lastevent = localtime_r(&lastEventDate, &tm_rLastEvent);
                    if (t_event->tm_mday != t_lastevent->tm_mday)
                        Add(new cMenuMyScheduleSepItem(NULL, Event));
                    lastEventDate = EventDate;
                }
                Add(new cMenuMyScheduleItem(Timers, Event, NULL, showNow, ScheduleTemplate), Event == PresentEvent);
                eventObjects.Add(Event);
            }
        }
        //              timeb tnow;
        //      ftime(&tnow);
        //      isyslog("duration epgs prepsched:  %d (%d)", tnow.millitm - tstart.millitm + ((tnow.millitm - tstart.millitm<0)?1000:0), Count());
    }
    if (shiftTime) {
        cString buffer = cString::sprintf("%s (%s%dh %dm)", Channel->Name(), shiftTime > 0 ? "+" : "",
                                          shiftTime / 60, abs(shiftTime) % 60);
        SetTitle(buffer);
    }
}

bool cMenuSearchMain::Update(void)
{
    bool result = false;
    LOCK_TIMERS_READ;
    for (cOsdItem *item = First(); item; item = Next(item)) {
        if (item->Selectable() && ((cMenuMyScheduleItem *)item)->Update(Timers))
            result = true;
    }
    return result;
}

eOSState cMenuSearchMain::Record(void)
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
            Timers->SetModified();
            gl_timerStatusMonitor->SetConflictCheckAdvised();
            timer->Matches();
            if (!HandleRemoteTimerModifications(timer)) {
                ERROR(tr("Epgsearch: RemoteTimerModifications failed"));
                Timers->Del(timer);
            } else
                LogFile.iSysLog("timer %s added (active)", *timer->ToDescr());

            if (HasSubMenu())
                CloseSubMenu();
            if (Update())
                Display();
            SetHelpKeys();
        }
    }
    return osContinue;
}

eOSState cMenuSearchMain::Switch(void)
{
    cMenuMyScheduleItem *item = (cMenuMyScheduleItem *)Get(Current());
    if (item) {
        LOCK_CHANNELS_READ;
        const cChannel *channel = Channels->GetByChannelID(item->event->ChannelID(), true, true);
        if (channel && cDevice::PrimaryDevice()->SwitchChannel(channel, true))
            return osEnd;
    }
    INFO(trVDR("Can't switch channel!"));
    return osContinue;
}

eOSState cMenuSearchMain::ExtendedSearch(void)
{
    return AddSubMenu(new cMenuEPGSearchExt());
}

eOSState cMenuSearchMain::Commands(eKeys Key)
{
    if (HasSubMenu() || Count() == 0)
        return osContinue;
    cMenuMyScheduleItem *mi = (cMenuMyScheduleItem *)Get(Current());
    if (mi && mi->event) {
        cMenuSearchCommands *menu;
        eOSState state = AddSubMenu(menu = new cMenuSearchCommands(tr("EPG Commands"), mi->event, true));
        if (Key != kNone)
            state = menu->ProcessKey(Key);
        return state;
    }
    return osContinue;
}

eOSState cMenuSearchMain::ShowSummary()
{
    if (Count()) {
        cMenuMyScheduleItem *mi = (cMenuMyScheduleItem *)Get(Current());
        if (mi && mi->event)
            return AddSubMenu(new cMenuEventSearch(mi->event, eventObjects, SurfModeTime));
    }
    return osContinue;
}

void cMenuSearchMain::SetHelpKeys(bool Force)
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
        if (toggleKeys == 0)
            SetHelp((EPGSearchConfig.redkeymode == 0 ? (hasTimer ? trVDR("Button$Timer") : trVDR("Button$Record")) : tr("Button$Commands")), trVDR("Button$Now"), trVDR("Button$Next"), EPGSearchConfig.bluekeymode == 0 ? trVDR("Button$Switch") : tr("Button$Search"));
        else {
            LOCK_CHANNELS_READ;
            const char* szGreenToggled = CHANNELNAME(Channels->GetByNumber(currentChannel - 1, -1));
            const char* szYellowToggled = CHANNELNAME(Channels->GetByNumber(currentChannel + 1, 1));

            SetHelp((EPGSearchConfig.redkeymode == 1 ? (hasTimer ? trVDR("Button$Timer") : trVDR("Button$Record")) : tr("Button$Commands")), (EPGSearchConfig.toggleGreenYellow == 0 ? trVDR("Button$Now") : szGreenToggled), (EPGSearchConfig.toggleGreenYellow == 0 ? trVDR("Button$Next") : szYellowToggled), EPGSearchConfig.bluekeymode == 1 ? trVDR("Button$Switch") : tr("Button$Search"));

        }
        helpKeys = NewHelpKeys;
    }
}

eOSState cMenuSearchMain::Shift(int iMinutes)
{
    shiftTime += iMinutes;
    LOCK_CHANNELS_READ;
    const cChannel *channel = Channels->GetByNumber(currentChannel);
    PrepareSchedule(channel);
    Display();
    SetHelpKeys();
    return osContinue;
}

void cMenuSearchMain::UpdateCurrent()
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

eOSState cMenuSearchMain::ProcessKey(eKeys Key)
{
    bool HadSubMenu = HasSubMenu();
    eOSState state = cOsdMenu::ProcessKey(Key);

    if (exitToMainMenu == 1) {
        exitToMainMenu = 0;
        return osBack;
    }

    if (!HasSubMenu() && HadSubMenu)
        UpdateCurrent();

    if (state == osUnknown) {
        switch (Key) {
        case kFastRew:
            if (HasSubMenu() && !InWhatsOnMenu && !InFavoritesMenu) {
                /*            if (Count())
                            {
                               CursorUp();
                               return ShowSummary();
                            }
                */
            } else
                return Shift(-EPGSearchConfig.timeShiftValue);
        case kFastFwd:
            if (HasSubMenu() && !InWhatsOnMenu && !InFavoritesMenu) {
                /*       if (Count())
                         {
                             CursorDown();
                             return ShowSummary();
                         }
                */
            } else
                return Shift(EPGSearchConfig.timeShiftValue);
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
                    if (mi) {
                        if (mi->event) {
                            return AddSubMenu(new cMenuSearchCommands(tr("EPG Commands"), mi->event));
                        }
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
            if (!HasSubMenu())
                return Commands(Key);
            else
                state = osContinue;
            break;
        case kGreen: {
            if (HasSubMenu() && !InWhatsOnMenu && !InFavoritesMenu) {
                if (Count()) {
//           CursorUp();
//           return ShowSummary();
                }
            } else if (toggleKeys == 0 || (toggleKeys == 1 && EPGSearchConfig.toggleGreenYellow == 0)) {
                int ChannelNr = cDevice::CurrentChannel();
                if (Count()) {
                    cMenuMyScheduleItem* Item = (cMenuMyScheduleItem *)Get(Current());
                    if (Item && Item->event) {
                        LOCK_CHANNELS_READ;
                        const cChannel *channel = Channels->GetByChannelID(Item->event->ChannelID(), true, true);
                        if (channel)
                            ChannelNr = channel->Number();
                    }
                }
                if (cMenuWhatsOnSearch::currentShowMode == showFavorites) {
                    InFavoritesMenu = true;
                    return AddSubMenu(new cMenuFavorites());
                } else {
                    InWhatsOnMenu = true;
                    return AddSubMenu(new cMenuWhatsOnSearch(ChannelNr));
                }
            } else {
                const cChannel *channel;
                {
                    LOCK_CHANNELS_READ;
                    channel = Channels->GetByNumber(currentChannel - 1, -1);
                }

                if (channel) {
                    PrepareSchedule(channel);
                    if (channel->Number() != cDevice::CurrentChannel()) {
                        otherChannel = channel->Number();
                    }
                    Display();
                }
                SetHelpKeys(true);
                return osContinue;
            }
        }
        case kYellow: {
            if (HasSubMenu()) {
                if (Count()) {
//           CursorDown();
//           return ShowSummary();
                }
            } else if (toggleKeys == 0 || (toggleKeys == 1 && EPGSearchConfig.toggleGreenYellow == 0)) {
                cMenuWhatsOnSearch::currentShowMode = showNext;
                InWhatsOnMenu = true;
                return AddSubMenu(new cMenuWhatsOnSearch(cMenuWhatsOnSearch::CurrentChannel()));
            } else {
                const cChannel *channel;
                {
                    LOCK_CHANNELS_READ;
                    channel = Channels->GetByNumber(currentChannel + 1, 1);
                }
                if (channel) {
                    PrepareSchedule(channel);
                    if (channel->Number() != cDevice::CurrentChannel()) {
                        otherChannel = channel->Number();
                    }
                    Display();
                }
                SetHelpKeys(true);
                return osContinue;
            }
            break;
        }
        case kBlue:
            if (HasSubMenu()) {
                UpdateCurrent();
                return Switch();
            }
            if (EPGSearchConfig.bluekeymode == toggleKeys)
                return Switch();
            else
                return ExtendedSearch();
            break;
        case kInfo:
        case kOk:
            if (Count())
                return ShowSummary();
            break;
        default:
            break;
        }
    }
    if (!HasSubMenu()) {
        const cChannel *ch = cMenuWhatsOnSearch::ScheduleChannel();
        InWhatsOnMenu = false;
        InFavoritesMenu = false;
        if (ch) {
            // when switch from the other menus to the schedule, try to keep the same time
            if (cMenuWhatsOnSearch::shiftTime) {
                time_t diff = cMenuWhatsOnSearch::seekTime - time(NULL);
                shiftTime = (diff + (diff > 0 ? 30 : -30)) / 60 ;
            } else
                shiftTime = 0;

            PrepareSchedule(ch);
            if (ch->Number() != cDevice::CurrentChannel()) {
                otherChannel = ch->Number();
            }
            Display();
        } else if ((HadSubMenu || gl_TimerProgged) && Update()) {
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

