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

#include <list>
#include "menu_timersdone.h"
#include "epgsearchtools.h"

int sortModeTimerDone = 0;

cMenuTimerDoneItem::cMenuTimerDoneItem(cTimerDone* TimerDone)
{
    timerDone = TimerDone;
    Set();
}

void cMenuTimerDoneItem::Set(void)
{
    if (!timerDone)
        return;
    char *buffer = NULL;

    char buf[32];
    struct tm tm_r;
    tm *tm = localtime_r(&timerDone->start, &tm_r);
    strftime(buf, sizeof(buf), "%d.%m.%y %H:%M", tm);

    LOCK_CHANNELS_READ;
    const cChannel* ch = Channels->GetByChannelID(timerDone->channelID, true, true);
    msprintf(&buffer, "%d\t%s\t%s~%s", ch ? ch->Number() : 0, buf, timerDone->title.c_str(), timerDone->shorttext.c_str());
    SetText(buffer, false);
}

int cMenuTimerDoneItem::Compare(const cListObject &ListObject) const
{
    cMenuTimerDoneItem *p = (cMenuTimerDoneItem *)&ListObject;
    if (sortModeTimerDone == 0) // sort by Date
        if (timerDone->start > p->timerDone->start) return 1;
        else return -1;
    else {
        cString s1 = cString::sprintf("%s~%s", timerDone->title.c_str(), timerDone->shorttext.c_str());
        cString s2 = cString::sprintf("%s~%s", p->timerDone->title.c_str(), p->timerDone->shorttext.c_str());
        int res = strcasecmp(s1, s2);
        return res;
    }
}

// --- cMenuTimersDone ----------------------------------------------------------
cMenuTimersDone::cMenuTimersDone(cSearchExt* Search)
    : cOsdMenu("", 4, 15)
{
    SetMenuCategory(mcTimer);

    search = Search;
    showAll = true;
    sortModeTimerDone = 0;
    if (search) showAll = false;
    Set();
    Display();
}

void cMenuTimersDone::Set()
{
    Clear();
    eventObjects.Clear();
    cMutexLock TimersDoneLock(&TimersDone);
    cTimerDone* timerDone = TimersDone.First();
    while (timerDone) {
        if (showAll || (!showAll && search && timerDone->searchID == search->ID))
            Add(new cMenuTimerDoneItem(timerDone));
        timerDone = TimersDone.Next(timerDone);
    }
    UpdateTitle();
    SetHelp(sortModeTimerDone == 0 ? tr("Button$by name") : tr("Button$by date"), tr("Button$Delete all"), trVDR("Button$Delete"), showAll ? search->search : tr("Button$Show all"));
    Sort();

    cMenuTimerDoneItem* item = (cMenuTimerDoneItem*)First();
    while (item) {
        if (item->timerDone) {
            const cEvent* Event = item->timerDone->GetEvent();
            if (Event) eventObjects.Add(Event);
        }
        item = (cMenuTimerDoneItem*)Next(item);
    }
}

void cMenuTimersDone::UpdateCurrent()
{
    // navigation in summary could have changed current item, so update it
    cEventObj* cureventObj = eventObjects.GetCurrent();
    if (cureventObj && cureventObj->Event())
        for (cMenuTimerDoneItem *item = (cMenuTimerDoneItem *)First(); item; item = (cMenuTimerDoneItem *)Next(item))
            if (item->Selectable() && item->timerDone->GetEvent() == cureventObj->Event()) {
                cureventObj->Select(false);
                SetCurrent(item);
                Display();
                break;
            }
}

cTimerDone *cMenuTimersDone::CurrentTimerDone(void)
{
    cMenuTimerDoneItem *item = (cMenuTimerDoneItem *)Get(Current());
    return item ? item->timerDone : NULL;
}

void cMenuTimersDone::UpdateTitle()
{
    cString buffer = cString::sprintf("%d %s%s%s", Count(), tr("Timers"), showAll ? "" : " ", showAll ? "" : search->search);
    SetTitle(buffer);
    Display();
}

eOSState cMenuTimersDone::Delete(void)
{
    cTimerDone *curTimerDone = CurrentTimerDone();
    if (curTimerDone) {
        if (Interface->Confirm(tr("Edit$Delete entry?"))) {
            LogFile.Log(1, "deleted timer done: '%s~%s'", curTimerDone->title != "" ? curTimerDone->title.c_str() : "unknown title", curTimerDone->shorttext != "" ? curTimerDone->shorttext.c_str() : "unknown subtitle");
            cMutexLock TimersDoneLock(&TimersDone);
            TimersDone.Del(curTimerDone);
            TimersDone.Save();
            cOsdMenu::Del(Current());
            Display();
            UpdateTitle();
        }
    }
    return osContinue;
}

eOSState cMenuTimersDone::DeleteAll(void)
{
    if (Interface->Confirm(tr("Edit$Delete all entries?"))) {
        cMutexLock TimersDoneLock(&TimersDone);
        while (Count() > 0) {
            cMenuTimerDoneItem *item = (cMenuTimerDoneItem *)Get(0);
            if (!item) break;
            cTimerDone *curTimerDone = item->timerDone;
            TimersDone.Del(curTimerDone);
            cOsdMenu::Del(0);
        }
        TimersDone.Save();
        Display();
        UpdateTitle();
    }

    return osContinue;
}

eOSState cMenuTimersDone::ProcessKey(eKeys Key)
{
    bool HadSubMenu = HasSubMenu();
    eOSState state = cOsdMenu::ProcessKey(Key);
    if (!HasSubMenu() && HadSubMenu)
        UpdateCurrent();

    if (state == osUnknown) {
        switch (Key) {
        case kGreen:
            state = DeleteAll();
            break;
        case kYellow:
            state = Delete();
            break;
        case kBlue:
            if (!HasSubMenu()) {
                showAll = !showAll;
                Set();
                Display();
            }
            break;
        case k0:
        case kRed:
            if (!HasSubMenu()) {
                sortModeTimerDone = 1 - sortModeTimerDone;
                Set();
                Display();
            }
            break;
        case k8:
            return osContinue;
        case kOk: {
            cTimerDone *TimerDone = CurrentTimerDone();
            if (TimerDone) {
                const cEvent* Event = TimerDone->GetEvent();
                if (!Event) break;
                return AddSubMenu(new cMenuEventSearchSimple(Event, eventObjects));
            }
        }
        default:
            break;
        }
    }

    return state;
}


