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

#include "switchtimer.h"
#include "epgsearchtools.h"

cSwitchTimers SwitchTimers;

// -- cSwitchTimer -----------------------------------------------------------------
cSwitchTimer::cSwitchTimer(void)
{
    switchMinsBefore = 1;
    mode = 0;
    unmute = 0;
}

cSwitchTimer::cSwitchTimer(const cEvent* Event, int SwitchMinsBefore, int Mode, int Unmute)
{
    eventID = 0;
    startTime = 0;
    if (Event) {
        eventID = Event->EventID();
        channelID = Event->ChannelID();
        startTime = Event->StartTime();
    }
    switchMinsBefore = SwitchMinsBefore;
    mode = Mode;
    unmute = Unmute;
}

cSwitchTimer& cSwitchTimer::operator= (const cSwitchTimer &SwitchTimer)
{
    this->eventID = SwitchTimer.eventID;
    this->startTime = SwitchTimer.startTime;
    this->channelID = SwitchTimer.channelID;
    this->switchMinsBefore = SwitchTimer.switchMinsBefore;
    this->mode = SwitchTimer.mode;
    this->unmute = SwitchTimer.unmute;
    return *this;
}

bool cSwitchTimer::Parse(const char *s)
{
    char *line;
    char *pos;
    char *pos_next;
    int parameter = 1;
    int valuelen;
#define MAXVALUELEN (10 * MaxFileName)

    char value[MAXVALUELEN];
    startTime = 0;

    pos = line = strdup(s);
    pos_next = pos + strlen(pos);
    if (*pos_next == '\n') *pos_next = 0;
    while (*pos) {
        while (*pos == ' ') pos++;
        if (*pos) {
            if (*pos != ':') {
                pos_next = strchr(pos, ':');
                if (!pos_next)
                    pos_next = pos + strlen(pos);
                valuelen = pos_next - pos + 1;
                if (valuelen > MAXVALUELEN)
                    valuelen = MAXVALUELEN;
                strn0cpy(value, pos, valuelen);
                pos = pos_next;
                switch (parameter) {
                case 1:
                    channelID = tChannelID::FromString(value);
                    break;
                case 2:
                    eventID = atoi(value);
                    break;
                case 3:
                    startTime = atol(value);
                    break;
                case 4:
                    switchMinsBefore = atoi(value);
                    break;
                case 5:
                    mode = atoi(value);
                    break;
                case 6:
                    unmute = atoi(value);
                    break;
                default:
                    break;
                } //switch
            }
            parameter++;
        }
        if (*pos) pos++;
    } //while

    free(line);
    return (parameter >= 3) ? true : false;
}

const cEvent* cSwitchTimer::Event()
{
    time_t now = time(NULL);
    const cEvent* event = NULL;
    if (startTime > now) {
        LOCK_SCHEDULES_READ;
        if (!Schedules) return NULL;
        const cSchedule *Schedule = Schedules->GetSchedule(channelID);
        if (Schedule) {
            event = Schedule->GetEvent(eventID, startTime);
            if (!event)
                event = Schedule->GetEventAround(startTime);
        }
    }
    return event;
}

cString cSwitchTimer::ToText(bool& ignore)
{
    ignore = false;
    if (!Event()) {
        ignore = true;
        return NULL;
    }
    LOCK_CHANNELS_READ;
    const cChannel *channel = Channels->GetByChannelID(channelID, true, true);
    if (!channel) return NULL;
    cString buffer = cString::sprintf("%s:%u:%ld:%d:%d:%d",
                                      CHANNELSTRING(channel), eventID,
                                      startTime, switchMinsBefore,
                                      mode, unmute ? 1 : 0);
    return buffer;
}

bool cSwitchTimer::Save(FILE *f)
{
    bool ignore = false;
    cString buffer = ToText(ignore);
    if (!ignore)
        return fprintf(f, "%s\n", *buffer) > 0;
    return true;
}


cSwitchTimer* cSwitchTimers::InSwitchList(const cEvent* event)
{
    if (!event) return NULL;
    cMutexLock SwitchTimersLock(this);
    cSwitchTimer* switchTimer = SwitchTimers.First();
    while (switchTimer) {
        if (switchTimer->eventID == event->EventID() &&
            switchTimer->channelID == event->ChannelID() &&
            switchTimer->startTime == event->StartTime())
            return switchTimer;
        switchTimer = SwitchTimers.Next(switchTimer);
    }
    return NULL;
}

bool cSwitchTimers::Exists(const cSwitchTimer* SwitchTimer)
{
    cMutexLock SwitchTimersLock(this);
    cSwitchTimer* switchTimer = SwitchTimers.First();
    while (switchTimer) {
        if (switchTimer == SwitchTimer)
            return true;
        switchTimer = SwitchTimers.Next(switchTimer);
    }
    return false;
}
