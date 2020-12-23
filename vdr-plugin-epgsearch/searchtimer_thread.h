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

#ifndef VDR_SEARCHTIMER_THREAD_H
#define VDR_SEARCHTIMER_THREAD_H

#include <vdr/thread.h>
#include "epgsearchext.h"
#include "epgsearchtools.h"
#include "mail.h"

class cPluginEpgsearch;

// --- cRecordingObj --------------------------------------------------------
class cRecordingObj : public cListObject
{
public:
    cRecording* recording;
    cSearchExt* search;
public:
    cRecordingObj(cRecording* r, cSearchExt* s) : recording(r), search(s) {}
    ~cRecordingObj() {
        recording = NULL;   // do not delete anything!
    }
};

// --- cSearchTimerThread----------------------------------------------------
class cSearchTimerThread: public cThread
{
private:
    bool m_Active;
    time_t m_lastUpdate;
    cPluginEpgsearch* m_plugin;
    cMailUpdateNotifier mailNotifier;
    cCondWait Wait;

protected:
    virtual void Action(void);
    bool AddModTimer(cTimer* Timer, int, cSearchExt*, const cEvent*, int Prio, int Lifetime, char* Summary = NULL, uint timerMod = tmNoChange);
    void RemoveTimer(const cTimer* Timer, const cEvent* Event = NULL);
    void Stop(void);
    bool NeedUpdate();
    bool TimerWasModified(const cTimer* t);
public:
    static cSearchResults announceList;
    static char* SummaryExtended(cSearchExt* searchExt, const cTimer* Timer, const cEvent* pEvent);
    static cSearchTimerThread *m_Instance;
    static const cTimer* GetTimer(cSearchExt *searchExt, const cEvent *pEvent, bool& bTimesMatchExactly);
    static bool justRunning;

    cSearchTimerThread(cPluginEpgsearch* thePlugin);
    virtual ~cSearchTimerThread();
    static void Init(cPluginEpgsearch* thePlugin, bool activatePermanently = false);
    static void Exit(void);
    void CheckExpiredRecs();
    void DelRecording(int index);
    void CheckManualTimers(void);
    void ModifyManualTimer(const cEvent* event, const cTimer* timer, int bstart, int bstop);
    void CheckEPGHours();
};

#endif
