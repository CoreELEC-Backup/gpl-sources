/*                                                                  -*- c++ -*-
Copyright (C) 2004-2013 Christian Wieninger 2017 Johann Friedrichs

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
*/

#include <string>
#include <list>
#ifdef __FreeBSD__
#include <netinet/in.h>
#endif
#include "recdone_thread.h"
#include "recdone.h"

#include <vdr/tools.h>
#include <vdr/plugin.h>
#define ALLOWED_BREAK_INSECS 2

extern int updateForced;

cRecdoneThread::cRecdoneThread()
    : cThread("EPGSearch: recdone")
{
}

cRecdoneThread::~cRecdoneThread()
{
}

bool cRecdoneThread::IsPesRecording(const cRecording *pRecording)
{
    return pRecording && pRecording->IsPesRecording();
}

#define LOC_INDEXFILESUFFIX     "/index"

struct tIndexTs {
    uint64_t offset: 40; // up to 1TB per file (not using off_t here - must definitely be exactly 64 bit!)
    int reserved: 7;    // reserved for future use
    int independent: 1; // marks frames that can be displayed by themselves (for trick modes)
    uint16_t number: 16; // up to 64K files per recording
    tIndexTs(off_t Offset, bool Independent, uint16_t Number) {
        offset = Offset;
        reserved = 0;
        independent = Independent;
        number = Number;
    }
};

int cRecdoneThread::RecLengthInSecs(const cRecording *pRecording)
{
    struct stat buf;
    cString fullname = cString::sprintf("%s%s", pRecording->FileName(), IsPesRecording(pRecording) ? LOC_INDEXFILESUFFIX ".vdr" : LOC_INDEXFILESUFFIX);
    if (pRecording->FileName() && *fullname && access(fullname, R_OK) == 0 && stat(fullname, &buf) == 0) {
        double frames = buf.st_size ? (buf.st_size - 1) / sizeof(tIndexTs) + 1 : 0;
        double Seconds = 0;
        modf((frames + 0.5) / pRecording->FramesPerSecond(), &Seconds);
        return Seconds;
    }
    return -1;
}

void cRecdoneThread::Action(void)
{
    LogFile.Log(1, "started recdone_thread");
    cMutexLock RecsDoneLock(&RecsDone);
    time_t now = time(NULL);
    // remove timers that finished recording from TimersRecording
    // incomplete recordings are kept for a while, perhaps they will be resumed
    LOCK_TIMERS_READ;
    while (m_fnames.size()) {
        vector<string>::iterator it = m_fnames.begin();
        const char *m_filename = (*it).c_str();
        LogFile.Log(1, "recdone_thread processing %s", m_filename);
        cMutexLock TimersRecordingLock(&TimersRecording);
        cRecDoneTimerObj *tiR = TimersRecording.First();
        while (tiR) {
            // check if timer still exists
            bool found = false;

            for (const cTimer *ti = Timers->First(); ti; ti = Timers->Next(ti))
                if (ti == tiR->timer) {
                    found = true;
                    break;
                }

            if (found && !tiR->timer->Recording()) {
                if (tiR->recDone) {
                    cSearchExt* search = SearchExts.GetSearchFromID(tiR->recDone->searchID);
                    if (!search) return;
                    // check if recording has ended before timer end

                    bool complete = true;
                    const cRecording *pRecording;
                    {
                        LOCK_RECORDINGS_READ;
                        pRecording = Recordings->GetByName(m_filename);
                    }
                    long timerLengthSecs = tiR->timer->StopTime() - tiR->timer->StartTime();
                    int recFraction = 100;
                    if (pRecording && timerLengthSecs) {
                        int recLen = RecLengthInSecs(pRecording);
                        recFraction = double(recLen) * 100 / timerLengthSecs;
                    }
                    bool vpsUsed = tiR->timer->HasFlags(tfVps) && tiR->timer->Event() && tiR->timer->Event()->Vps();
                    if ((!vpsUsed && now < tiR->timer->StopTime()) || recFraction < (vpsUsed ? 90 : 98)) { // assure timer has reached its end or at least 98% were recorded
                        complete = false;
                        LogFile.Log(1, "finished: '%s' (not complete! - recorded only %d%%); search timer: '%s'; VPS used: %s", tiR->timer->File(), recFraction, search->search, vpsUsed ? "Yes" : "No");
                    } else {
                        LogFile.Log(1, "finished: '%s'; search timer: '%s'; VPS used: %s", tiR->timer->File(), search->search, vpsUsed ? "Yes" : "No");
                        if (recFraction < 100)
                            LogFile.Log(2, "recorded %d%%'", recFraction);
                    }
                    if (complete) {
                        RecsDone.Add(tiR->recDone);
                        LogFile.Log(1, "added rec done for '%s~%s';%s", tiR->recDone->title ? tiR->recDone->title : "unknown title",
                                    tiR->recDone->shortText ? tiR->recDone->shortText : "unknown subtitle",
                                    search->search);
                        RecsDone.Save();
                        tiR->recDone = NULL; // prevent deletion
                        tiR->lastBreak = 0;

                        // check for search timers to delete automatically
                        SearchExts.CheckForAutoDelete(search);

                        // trigger a search timer update (skip running events)
                        search->skipRunningEvents = true;
                        updateForced = 1;
                    } else if (tiR->lastBreak == 0) // store first break
                        tiR->lastBreak = now;
                }
                if (tiR->lastBreak == 0 || (now - tiR->lastBreak) > ALLOWED_BREAK_INSECS) {
                    // remove finished recordings or those with an unallowed break
                    if (tiR->recDone) delete tiR->recDone; // clean up
                    cRecDoneTimerObj *tiRNext = TimersRecording.Next(tiR);
                    TimersRecording.Del(tiR);
                    tiR = tiRNext;
                    continue;
                }
                break;
            }
            if (!found) {
                if (tiR->recDone) delete tiR->recDone; // clean up
                cRecDoneTimerObj *tiRNext = TimersRecording.Next(tiR);
                TimersRecording.Del(tiR);
                tiR = tiRNext;
                continue;
            }
            tiR = TimersRecording.Next(tiR);
        }
        m_fnames.erase(it);
    } // while fnames
    LogFile.Log(1, "recdone_thread ended");
}
