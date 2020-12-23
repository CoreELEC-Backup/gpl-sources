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
#include <string>
#include "menu_conflictcheck.h"
#include "menu_commands.h"
#include "menu_event.h"
#include "conflictcheck.h"
#include "timer_thread.h"
#include <vdr/menu.h>

using std::string;

// --- cMenuConflictCheckItem -------------------------------------------------------
cMenuConflictCheckItem::cMenuConflictCheckItem(cConflictCheckTime* Ct, cConflictCheckTimerObj* TimerObj)
{
    checktime = Ct;
    timerObj = TimerObj;
    cString buffer;
    if (!TimerObj) { // print header
        struct tm tm_r;
        const time_t t = checktime->evaltime;
        tm *tm = localtime_r(&t, &tm_r);
        char dateshort[7] = "";
        strftime(dateshort, sizeof(dateshort), "%d.%m.", tm);
        buffer = cString::sprintf("%s\t%s  %s", WEEKDAYNAME(t), dateshort, TIMESTRING(t));
        SetSelectable(false);
    } else {
        const cTimer* t = timerObj->timer;
        int recPart = timerObj->recDuration * 100 / (timerObj->stop - timerObj->start);
        buffer = cString::sprintf("%d\t%s\t%d\t%2d%%\t%s", t->Channel()->Number(), t->Channel()->ShortName(true), t->Priority(), recPart, t->File());
    }
    SetText(buffer);
}

// --- cMenuConflictCheck -------------------------------------------------------

cMenuConflictCheck::cMenuConflictCheck()
    : cOsdMenu("", 4, 12, 4, 5, 30)
{
    SetMenuCategory(mcTimerEdit);
    showAll = false;
    lastSel = -1;
    BuildList();
    Update();
}

void cMenuConflictCheck::Update()
{
    if (conflictCheck.numConflicts > 0) {
        if (conflictCheck.numConflicts == conflictCheck.relevantConflicts)
            SetHelp(conflictCheck.relevantConflicts ? tr("Button$Details") : NULL, NULL, NULL, NULL);
        else if (showAll)
            SetHelp(tr("Button$Details"), NULL, NULL, tr("Button$Filter"));
        else
            SetHelp(conflictCheck.relevantConflicts ? tr("Button$Details") : NULL, NULL, NULL, tr("Button$Show all"));
    } else
        SetHelp(NULL, NULL, NULL, NULL);

    cString buffer = cString::sprintf("%s - %d/%d %s", tr("Timer conflicts"),
                                      showAll ? conflictCheck.numConflicts : conflictCheck.relevantConflicts,
                                      conflictCheck.numConflicts,
                                      tr("conflicts"));
    SetTitle(buffer);
    Display();
}

bool cMenuConflictCheck::BuildList()
{
    Clear();
    conflictCheck.Check();

    if ((showAll && conflictCheck.numConflicts > 0) ||
        conflictCheck.relevantConflicts > 0) {
        cList<cConflictCheckTime>* failedList = conflictCheck.GetFailed();
        for (cConflictCheckTime* ct = failedList->First(); ct; ct = failedList->Next(ct)) {
            if (!showAll && ct->ignore) continue;
            Add(new cMenuConflictCheckItem(ct));
            std::set<cConflictCheckTimerObj*, TimerObjSort>::iterator it;
            for (it = ct->failedTimers.begin(); it != ct->failedTimers.end(); ++it)
                if (!(*it)->ignore || showAll)
                    Add(new cMenuConflictCheckItem(ct, *it));
        }
    } else {
        cOsdItem* pInfoItem = new cOsdItem(conflictCheck.numConflicts == 0 ? tr("no conflicts!") : tr("no important conflicts!"));
        pInfoItem->SetSelectable(false);
        Add(pInfoItem);
    }
    SetCurrent(Get(lastSel));
    return false;
}

cConflictCheckTimerObj* cMenuConflictCheck::CurrentTimerObj(void)
{
    cMenuConflictCheckItem *item = (cMenuConflictCheckItem *)Get(Current());
    return (item && item->Selectable()) ? item->timerObj : NULL;
}

eOSState cMenuConflictCheck::ProcessKey(eKeys Key)
{
    bool HadSubMenu = HasSubMenu();
    eOSState state = cOsdMenu::ProcessKey(Key);
    if (state == osUnknown) {
        switch (Key) {
        case kOk:
            if (Count() == 1)
                return osBack;
        case kRed:
            if (CurrentTimerObj())
                state = AddSubMenu(new cMenuConflictCheckDetails(CurrentTimerObj(), &conflictCheck));
            break;
        case kBlue:
            showAll = !showAll;
            BuildList();
            Update();
            state = osContinue;
            break;
        default:
            break;
        }
    }
    if (HadSubMenu && !HasSubMenu()) { // Update conflict list
        lastSel = Current();
        BuildList();
        Update();
    }
    return state;
}

// --- cMenuConflictCheckDetailsItem -------------------------------------------------------
cMenuConflictCheckDetailsItem::cMenuConflictCheckDetailsItem(cConflictCheckTimerObj* TimerObj)
{
    timerObj = TimerObj;
    LOCK_TIMERS_READ;
    hasTimer = timerObj->OrigTimer(Timers) ? timerObj->OrigTimer(Timers)->HasFlags(tfActive) : false;
    Update(true);
}

bool cMenuConflictCheckDetailsItem::Update(bool Force)
{
    bool oldhasTimer = hasTimer;
    LOCK_TIMERS_READ;
    hasTimer = timerObj->OrigTimer(Timers) ? timerObj->OrigTimer(Timers)->HasFlags(tfActive) : false;
    if (Force || hasTimer != oldhasTimer) {
        const cTimer* timer = timerObj->timer;
        char device[3] = ""; // compiler warns if MAXDEVICES too big 
        if (hasTimer) {
            if (!timerObj->conflCheckTime && timerObj->device > -1)
                sprintf(device, "%d", (timerObj->device + 1)&MAXDEVICES);
            else {
                if (timer->Local())
                    strcpy(device, tr("C"));
                else
                    strcpy(device, "R"); //Remote Timer
            }
        }

        cString buffer = cString::sprintf("%s\t%d\t%s - %s\t%d\t%s\t%s", hasTimer ? ">" : "", timer->Channel()->Number(), TIMESTRING(timerObj->start), TIMESTRING(timerObj->stop), timer->Priority(), device, timer->File());

        SetText(buffer);
    }
    return true;
}

// --- cMenuConflictCheckDetails -------------------------------------------------------

cMenuConflictCheckDetails::cMenuConflictCheckDetails(cConflictCheckTimerObj* TimerObj, cConflictCheck* ConflictCheck)
    : cOsdMenu(tr("Timer conflicts"), 2, 4, 13, 3, 2)
{
    SetMenuCategory(mcTimerEdit);
    timerObj = TimerObj;
    checktime = timerObj->conflCheckTime;
    conflictCheck = ConflictCheck;
    BuildList();
    SetHelpKeys();

    int recPart = timerObj->recDuration * 100 / (timerObj->stop - timerObj->start);
    cString buffer = cString::sprintf("%s - %s - %d%%", DATESTRING(checktime->evaltime), TIMESTRING(checktime->evaltime), recPart);
    SetTitle(buffer);
}

bool cMenuConflictCheckDetails::BuildList()
{
    Clear();
    int sel = -1;

    std::set<cConflictCheckTimerObj*, TimerObjSort>::iterator it;
    if (timerObj->concurrentTimers)
        for (it = timerObj->concurrentTimers->begin(); it != timerObj->concurrentTimers->end(); ++it) {
            Add(new cMenuConflictCheckDetailsItem(*it));
            if ((*it)->Event())
                eventObjects.Add((*it)->Event());
            if ((*it) == timerObj) sel = Count() - 1;
        }
    SetCurrent(Get(sel));
    return true;
}

void cMenuConflictCheckDetails::SetHelpKeys()
{
    cConflictCheckTimerObj* curTimerObj = CurrentTimerObj();
    bool hasTimer = true;
    bool hasEvent = false;
    if (curTimerObj) {
        hasTimer = curTimerObj->timer->HasFlags(tfActive);
        hasEvent = curTimerObj->Event();
    }
    SetHelp(hasEvent ? tr("Button$Repeats") : NULL, trVDR("Button$On/Off"), hasTimer ? trVDR("Button$Delete") : NULL, tr("Button$Commands"));
}

cConflictCheckTimerObj* cMenuConflictCheckDetails::CurrentTimerObj(void)
{
    cMenuConflictCheckDetailsItem *item = (cMenuConflictCheckDetailsItem *)Get(Current());
    return item ? item->timerObj : NULL;
}

eOSState cMenuConflictCheckDetails::Commands(eKeys Key)
{
    if (HasSubMenu() || Count() == 0)
        return osContinue;
    cConflictCheckTimerObj* curTimerObj = CurrentTimerObj();
    if (curTimerObj && curTimerObj->Event()) {
        cMenuSearchCommands *menu;
        eOSState state = AddSubMenu(menu = new cMenuSearchCommands(tr("EPG Commands"), curTimerObj->Event(), true));
        if (Key != kNone)
            state = menu->ProcessKey(Key);
        return state;
    }
    return osContinue;
}

eOSState cMenuConflictCheckDetails::ToggleTimer(cConflictCheckTimerObj* TimerObj)
{
    LOCK_TIMERS_WRITE;
    Timers->SetExplicitModify();
    if (!TimerObj || !TimerObj->OrigTimer(Timers)) return osContinue;
    TimerObj->OrigTimer(Timers)->OnOff();  // Toggles Timer Flag
    Timers->SetModified();
    Update();
    Display();
    return osContinue;
}

bool cMenuConflictCheckDetails::Update(bool Force)
{
    bool result = false;
    for (cOsdItem *item = First(); item; item = Next(item)) {
        if (item->Selectable() && ((cMenuConflictCheckDetailsItem *)item)->Update(Force))
            result = true;
    }
    return result;
}

eOSState cMenuConflictCheckDetails::DeleteTimer(cConflictCheckTimerObj* TimerObj)
{
    LOCK_TIMERS_WRITE;
    Timers->SetExplicitModify();
    cTimer* timer = TimerObj->OrigTimer(Timers);
    // Check if this timer is active:
    if (timer) {
        if (Interface->Confirm(trVDR("Delete timer?"))) {
            if (timer->Recording()) {
                if (Interface->Confirm(trVDR("Timer still recording - really delete?"))) {
                    timer->Skip();
                    cRecordControls::Process(Timers, time(NULL));
                } else
                    return osContinue;
            }
            if (!HandleRemoteTimerModifications(NULL, timer)) {
                LogFile.Log(2, "HandleRemoteTimerModifications failed");
                return osContinue;
            }
            LogFile.iSysLog("deleting timer %s", *timer->ToDescr());
            Timers->Del(timer);
            cOsdMenu::Del(Current());
            Timers->SetModified();
            Display();
            return osBack;
        }
    }
    return osContinue;
}


eOSState cMenuConflictCheckDetails::ShowSummary()
{
    cConflictCheckTimerObj* curTimerObj = CurrentTimerObj();
    if (!curTimerObj)
        return osContinue;

    const cEvent *ei = curTimerObj->Event();
    if (ei) {
        const cChannel *channel;
        {
            LOCK_CHANNELS_READ;
            channel = Channels->GetByChannelID(ei->ChannelID(), true, true);
        }
        if (channel)
            return AddSubMenu(new cMenuEventSearchSimple(ei, eventObjects));
    }
    return osContinue;
}

void cMenuConflictCheckDetails::UpdateCurrent()
{
    cEventObj* cureventObj = eventObjects.GetCurrent();
    if (cureventObj && cureventObj->Event())
        for (cMenuConflictCheckDetailsItem *item = (cMenuConflictCheckDetailsItem *)First(); item; item = (cMenuConflictCheckDetailsItem *)Next(item))
            if (item->Selectable() && item->timerObj && item->timerObj->Event() == cureventObj->Event()) {
                cureventObj->Select(false);
                SetCurrent(item);
                Display();
                break;
            }
}

eOSState cMenuConflictCheckDetails::ProcessKey(eKeys Key)
{
    bool HadSubMenu = HasSubMenu();
    eOSState state = cOsdMenu::ProcessKey(Key);

    if (!HasSubMenu() && HadSubMenu) // navigation in summary could have changed current item, so update it
        UpdateCurrent();

    if (state == osUnknown) {
        switch (Key) {
        case k1...k9:
            if (!HasSubMenu())
                return Commands(Key);
            else
                state = osContinue;
            break;
        case kOk:
            if (!HasSubMenu())
                return ShowSummary();
            else
                state = osContinue;
            break;
        case kRed:
            if (!HasSubMenu()) {
                if (CurrentTimerObj() && CurrentTimerObj()->Event())
                    return Commands(k1);
                else
                    state = osContinue;
            } else
                state = osContinue;
            break;
        case kGreen:
            if (!HasSubMenu())
                state = ToggleTimer(CurrentTimerObj());
            break;
        case kYellow:
            if (!HasSubMenu())
                state = DeleteTimer(CurrentTimerObj());
            break;
        case kBlue:
            if (!HasSubMenu()) {
                if (CurrentTimerObj() && CurrentTimerObj()->Event())
                    return AddSubMenu(new cMenuSearchCommands(tr("EPG Commands"), CurrentTimerObj()->Event()));
                else
                    state = osContinue;
            } else
                state = osContinue;
            break;
        default:
            break;
        }
    }
    if (!HasSubMenu()) {
        // perhaps a timer was deleted, so first check this
        if (conflictCheck) {
            std::set<cConflictCheckTimerObj*, TimerObjSort>::iterator it;
            if (timerObj->concurrentTimers) {
                for (it = timerObj->concurrentTimers->begin(); it != timerObj->concurrentTimers->end(); ++it) {
                    bool found = false;
                    LOCK_TIMERS_READ;
                    for (const cTimer* checkT = Timers->First(); checkT; checkT = Timers->Next(checkT)) {
                        checkT->Matches();
                        if (checkT == (*it)->OrigTimer(Timers)) { // ok -> found, check for changes
                            if (checkT->IsSingleEvent()) {
                                if (checkT->StartTime() == (*it)->start && checkT->StopTime() == (*it)->stop) {
                                    found = true;
                                    break;
                                }
                            } else { // repeating timer
                                if (checkT->DayMatches((*it)->start)) { // same day?
                                    int begin  = cTimer::TimeToInt(checkT->Start()); // seconds from midnight
                                    int length = cTimer::TimeToInt(checkT->Stop()) - begin;
                                    if (length < 0)
                                        length += SECSINDAY;

                                    time_t Start = cTimer::SetTime((*it)->start, cTimer::TimeToInt(checkT->Start()));
                                    if (Start == (*it)->start && (*it)->stop == (*it)->start + length)
                                        found = true;
                                }
                            }
                        }
                    }
                    if (!found) return osBack;
                }
            }
        }

        if (Key != kNone)
            SetHelpKeys();
        if ((HadSubMenu || gl_TimerProgged) && Update(true)) {
            if (gl_TimerProgged) { // when using epgsearch's timer edit menu, update is delayed because of SVDRP
                gl_TimerProgged = 0;
                SetHelpKeys();
            }
            Display();
        }
    }
    return state;
}
