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

#include "menu_switchtimers.h"
#include "epgsearchtools.h"

// --- cMenuEditSwitchTimer --------------------------------------------------------
class cMenuEditSwitchTimer : public cOsdMenu
{
private:
    char *SwitchModes[3];
    cSwitchTimer *switchTimer;
    cSwitchTimer data;
    bool addIfConfirmed;
public:
    cMenuEditSwitchTimer(cSwitchTimer *SwitchTimer, bool New = false);
    void Set();
    virtual eOSState ProcessKey(eKeys Key);
};

cMenuEditSwitchTimer::cMenuEditSwitchTimer(cSwitchTimer* SwitchTimer, bool New)
    : cOsdMenu(tr("Edit entry"), 30)
{
    SetMenuCategory(mcPlugin);

    SwitchModes[0] = strdup(tr("Switch"));
    SwitchModes[1] = strdup(tr("Announce only"));
    SwitchModes[2] = strdup(tr("Announce and switch"));

    switchTimer = SwitchTimer;
    addIfConfirmed = New;
    if (switchTimer) {
        data = *switchTimer;
        Set();
    }
}

void cMenuEditSwitchTimer::Set()
{
    int current = Current();
    Clear();

    Add(new cMenuEditStraItem(tr("Action"), &data.mode, 3, SwitchModes));
    if (data.mode == 0) // always switch
        Add(new cMenuEditIntItem(tr("Switch ... minutes before start"), &data.switchMinsBefore, 0, 99));
    if (data.mode == 1) // only announce
        Add(new cMenuEditIntItem(tr("Announce ... minutes before start"), &data.switchMinsBefore, 0, 99));
    if (data.mode == 2) // ask for switching
        Add(new cMenuEditIntItem(tr("Ask ... minutes before start"), &data.switchMinsBefore, 0, 99));

    cString info = cString::sprintf("%s:\t%s", tr("action at"),
                                    TIMESTRING(data.startTime - 60 * data.switchMinsBefore));
    cOsdItem* pInfoItem = new cOsdItem(info);
    pInfoItem->SetSelectable(false);
    Add(pInfoItem);
    Add(new cMenuEditBoolItem(tr("Unmute sound"), &data.unmute, trVDR("no"), trVDR("yes")));
    SetCurrent(Get(current));
}

eOSState cMenuEditSwitchTimer::ProcessKey(eKeys Key)
{
    int iOldMinsBefore = data.switchMinsBefore;
    int iOldMode = data.mode;
    eOSState state = cOsdMenu::ProcessKey(Key);

    if (iOldMinsBefore != data.switchMinsBefore ||
        iOldMode != data.mode) {
        time_t now = time(NULL);
        if (data.startTime - 60 * data.switchMinsBefore < now)
            data.switchMinsBefore = iOldMinsBefore;
        Set();
        Display();
    }

    if (state == osUnknown) {
        switch (Key) {
        case kOk: {
            if (switchTimer) {
                *switchTimer = data;
                cMutexLock SwitchTimersLock(&SwitchTimers);
                if (addIfConfirmed)
                    SwitchTimers.Add(switchTimer);
                SwitchTimers.Save();
            }
            addIfConfirmed = false;
            return osBack;
        }
        default:
            break;
        }
    }
    return state;
}

cMenuSwitchTimerItem::cMenuSwitchTimerItem(cSwitchTimer* SwitchTimer, const cEvent* Event)
{
    switchTimer = SwitchTimer;
    event = Event;
    Set();
}

void cMenuSwitchTimerItem::Set()
{
    if (!SwitchTimers.Exists(switchTimer) || !switchTimer || !event)
        return;

    time_t startTime = switchTimer->startTime;
    char *buffer = NULL;

    char datebuf[32];
    struct tm tm_r;
    tm *tm = localtime_r(&startTime, &tm_r);
    strftime(datebuf, sizeof(datebuf), "%d.%m", tm);

    LOCK_CHANNELS_READ;
    const cChannel* channel = Channels->GetByChannelID(switchTimer->channelID, true, true);

    msprintf(&buffer, "%s\t%d\t%s\t%s\t%d\'\t%s~%s", switchTimer->mode == 1 ? "" : ">", channel ? channel->Number() : -1, datebuf, TIMESTRING(startTime), switchTimer->switchMinsBefore, event->Title() ? event->Title() : "", event->ShortText() ? event->ShortText() : "");
    SetText(buffer, false);
}

int cMenuSwitchTimerItem::Compare(const cListObject &ListObject) const
{
    cMenuSwitchTimerItem *p = (cMenuSwitchTimerItem *)&ListObject;
    if (switchTimer->startTime > p->switchTimer->startTime)
        return 1;
    else
        return -1;
}

// --- cMenuSwitchTimers ----------------------------------------------------------
cMenuSwitchTimers::cMenuSwitchTimers()
    : cOsdMenu(tr("Switch list"), 2, 4, 6, 6, 4)
{
    SetMenuCategory(mcPlugin);

    Set();
    Display();
}

void cMenuSwitchTimers::Set()
{
    Clear();
    cMutexLock SwitchTimersLock(&SwitchTimers);
    cSwitchTimer* switchTimer = SwitchTimers.First();
    while (switchTimer) {
        const cEvent* event = switchTimer->Event();
        if (event)
            Add(new cMenuSwitchTimerItem(switchTimer, event));
        switchTimer = SwitchTimers.Next(switchTimer);
    }
    Display();
    SetHelp(trVDR("Button$Edit"), tr("Button$Delete all"), trVDR("Button$Delete"), NULL);
    Sort();
}

cSwitchTimer* cMenuSwitchTimers::CurrentSwitchTimer(void)
{
    cMenuSwitchTimerItem *item = (cMenuSwitchTimerItem *)Get(Current());
    if (item && SwitchTimers.Exists(item->switchTimer))
        return item->switchTimer;
    return NULL;
}

eOSState cMenuSwitchTimers::Delete(void)
{
    cSwitchTimer *curSwitchTimer = CurrentSwitchTimer();
    if (curSwitchTimer) {
        if (Interface->Confirm(tr("Edit$Delete entry?"))) {
            cMutexLock SwitchTimersLock(&SwitchTimers);
            SwitchTimers.Del(curSwitchTimer);
            SwitchTimers.Save();
            cOsdMenu::Del(Current());
            Display();
        }
    }
    return osContinue;
}

eOSState cMenuSwitchTimers::DeleteAll(void)
{
    if (Interface->Confirm(tr("Edit$Delete all entries?"))) {
        cMutexLock SwitchTimersLock(&SwitchTimers);
        while (SwitchTimers.First())
            SwitchTimers.Del(SwitchTimers.First());
        SwitchTimers.Save();
        Set();
    }

    return osContinue;
}

eOSState cMenuSwitchTimers::Summary(void)
{
    if (HasSubMenu() || Count() == 0)
        return osContinue;
    cSwitchTimer *curSwitchTimer = CurrentSwitchTimer();

    if (curSwitchTimer) {
        const cEvent* event = curSwitchTimer->Event();
        if (event && !isempty(event->Description()))
            return AddSubMenu(new cMenuText(tr("Summary"), event->Description()));
    }
    return osContinue;
}

eOSState cMenuSwitchTimers::ProcessKey(eKeys Key)
{
    eOSState state = cOsdMenu::ProcessKey(Key);
    if (state == osUnknown) {
        switch (Key) {
        case kOk:
            state = Summary();
            break;
        case kGreen:
            state = DeleteAll();
            break;
        case kYellow:
            state = Delete();
            break;
        case kRed:
            if (HasSubMenu())
                return osContinue;
            if (CurrentSwitchTimer())
                state = AddSubMenu(new cMenuEditSwitchTimer(CurrentSwitchTimer()));
            else
                state = osContinue;
            break;
        case k0:
            if (CurrentSwitchTimer()) {
                cSwitchTimer* switchTimer = CurrentSwitchTimer();
                switchTimer->mode = switchTimer->mode == 1 ? 2 : 1;
                cMutexLock SwitchTimersLock(&SwitchTimers);
                SwitchTimers.Save();
                RefreshCurrent();
                Display();
            }
            break;
        default:
            break;
        }
    }

    return state;
}
