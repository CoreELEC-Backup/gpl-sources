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

#include "epgsearchtools.h"
#include "recstatus.h"
#include "recdone_thread.h"
#include "conflictcheck_thread.h"
#include "epgsearchcfg.h"
#include <math.h>
#define ALLOWED_BREAK_INSECS 2

extern int gl_InfoConflict;
cTimersRecording TimersRecording;
cRecdoneThread RecdoneThread;

cRecStatusMonitor* gl_recStatusMonitor = NULL;

cRecStatusMonitor::cRecStatusMonitor()
{
}

void cRecStatusMonitor::Recording(const cDevice *Device, const char *Name, const char* Filename, bool On)
{
    time_t now = time(NULL);
    // insert new timers currently recording in TimersRecording
    if (On && Name) {
        if (EPGSearchConfig.checkTimerConflOnRecording)
            cConflictCheckThread::Init((cPluginEpgsearch*)cPluginManager::GetPlugin("epgsearch"), true);

        LOCK_TIMERS_READ;
        for (const cTimer *ti = Timers->First(); ti; ti = Timers->Next(ti))
            if (ti->Recording()) {
                // check if this is a new entry
                cRecDoneTimerObj *tiRFound = NULL;
                cMutexLock TimersRecordingLock(&TimersRecording);
                for (cRecDoneTimerObj *tiR = TimersRecording.First(); tiR; tiR = TimersRecording.Next(tiR))
                    if (tiR->timer == ti) {
                        tiRFound = tiR;
                        break;
                    }

                if (tiRFound) { // already handled, perhaps a resume
                    if (tiRFound->lastBreak > 0 && now - tiRFound->lastBreak <= ALLOWED_BREAK_INSECS) {
                        LogFile.Log(1, "accepting resume of '%s' on device %d", Name, Device->CardIndex());
                        tiRFound->lastBreak = 0;
                    }
                    continue;
                }

                cRecDoneTimerObj* timerObj = new cRecDoneTimerObj(ti, Device->DeviceNumber());
                TimersRecording.Add(timerObj);

                cSearchExt* search = TriggeredFromSearchTimer(ti);
                if (!search || (search->avoidRepeats == 0 && search->delMode == 0)) // ignore if not avoid repeats and no auto-delete
                    continue;

                bool vpsUsed = ti->HasFlags(tfVps) && ti->Event() && ti->Event()->Vps();
                LogFile.Log(1, "recording started '%s' on device %d (search timer '%s'); VPS used: %s", Name, Device->CardIndex(), search->search, vpsUsed ? "Yes" : "No");
                const cEvent* event = ti->Event();
                if (!event) {
                    event = GetEvent(ti);
                    if (event)
                        LogFile.Log(3, "timer had no event: assigning '%s'", event->Title());
                }
                if (!event) {
                    LogFile.Log(1, "no event for timer found! will be ignored in done list");
                    continue;
                }
                time_t now = time(NULL);
                if (vpsUsed || now < ti->StartTime() + 60) { // allow a delay of one minute
                    timerObj->recDone = new cRecDone(ti, event, search);
                    return;
                } else
                    LogFile.Log(1, "recording started too late! will be ignored");
            }
    }

    if (!On) {
        // must be done in a different thread because we hold timer and scheduling lock here
        RecdoneThread.SetFilename(Filename);  // push_back Filename for processing
        RecdoneThread.Start();
    }
}

int cRecStatusMonitor::TimerRecDevice(const cTimer* timer)
{
    if (!timer) return 0;
    cMutexLock TimersRecordingLock(&TimersRecording);
    for (cRecDoneTimerObj *tiR = TimersRecording.First(); tiR; tiR = TimersRecording.Next(tiR))
        if (tiR->timer == timer && timer->Recording()) return tiR->deviceNr + 1;
    return 0;
}
