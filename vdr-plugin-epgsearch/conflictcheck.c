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
#include <algorithm>
#include "conflictcheck.h"
#include "epgsearchcfg.h"
/*#include <libsi/si.h>*/
#include "conflictcheck_thread.h"
#include "recstatus.h"
#include "timerstatus.h"
#include "uservars.h"
#include <vdr/svdrp.h>

#define FULLMATCH 1000
#define EPGLIMITBEFORE   (1 * 3600) // Time in seconds before a timer's start time and
#define EPGLIMITAFTER    (1 * 3600) // after its stop time within which EPG events will be taken into consideration.

// --- cConflictCheckTimerObj --------------------------------------------------------
cConflictCheckTimerObj::cConflictCheckTimerObj(cTimer* Timer, time_t Start, time_t Stop, int Device, int OrigIndex) : cTimerObj(Timer), start(Start), stop(Stop), device(Device), origIndex(OrigIndex), conflCheckTime(NULL), concurrentTimers(NULL), ignore(false)
{
    event = Timer->Event();
    recDuration = 0;
    lastRecStart = 0;
    lastRecStop = 0;
}

cConflictCheckTimerObj::~cConflictCheckTimerObj()
{
    // conflict checks works on a copy of a timer, so delete it again
    DELETENULL(timer);
}

int cConflictCheckTimerObj::Compare(const cListObject &ListObject) const
{
    cConflictCheckTimerObj *p = (cConflictCheckTimerObj *)&ListObject;
    long diff = start - p->start;
    if (diff == 0)
        diff = p->timer->Priority() - timer->Priority();
    if (diff == 0)
        diff = origIndex - p->origIndex;
    return diff;
}

const cEvent* cConflictCheckTimerObj::Event()
{
    if (timer->Event())
        return timer->Event();
    else if (!event)
        event = SetEventFromSchedule();
    return event;
}

const cEvent* cConflictCheckTimerObj::SetEventFromSchedule()
{
    LOCK_SCHEDULES_READ;
    if (!Schedules)
        return NULL;

    const cSchedule *Schedule = Schedules->GetSchedule(timer->Channel());
    if (Schedule && Schedule->Events()->First()) {
        const cEvent *Event = NULL;
        if (timer->HasFlags(tfVps) && Schedule->Events()->First()->Vps()) {
            // VPS timers only match if their start time exactly matches the event's VPS time:
            for (const cEvent *e = Schedule->Events()->First(); e; e = Schedule->Events()->Next(e)) {
                if (e->StartTime() && e->RunningStatus() != SI::RunningStatusNotRunning) {
                    // skip outdated events
                    int overlap = 0;
                    Matches(e, &overlap);
                    if (overlap > FULLMATCH) {
                        Event = e;
                        break; // take the first matching event
                    }
                }
            }
        } else {
            // Normal timers match the event they have the most overlap with:
            int Overlap = 0;
            // Set up the time frame within which to check events:
            timer->Matches(0, true);
            time_t TimeFrameBegin = start - EPGLIMITBEFORE;
            time_t TimeFrameEnd   = stop  + EPGLIMITAFTER;
            for (const cEvent *e = Schedule->Events()->First(); e; e = Schedule->Events()->Next(e)) {
                if (e->EndTime() < TimeFrameBegin)
                    continue; // skip events way before the timer starts
                if (e->StartTime() > TimeFrameEnd)
                    break; // the rest is way after the timer ends
                int overlap = 0;
                Matches(e, &overlap);
                if (overlap && overlap >= Overlap) {
                    if (Event && overlap == Overlap && e->Duration() <= Event->Duration())
                        continue; // if overlap is the same, we take the longer event
                    Overlap = overlap;
                    Event = e;
                }
            }
        }
        return Event;
    }
    return NULL;
}

int cConflictCheckTimerObj::Matches(const cEvent *Event, int *Overlap) const
{
    // Overlap is the percentage of the Event's duration that is covered by
    // this timer (based on FULLMATCH for finer granularity than just 100).
    // To make sure a VPS timer can be distinguished from a plain 100% overlap,
    // it gets an additional 100 added, and a VPS event that is actually running
    // gets 200 added to the FULLMATCH.
    if (timer->Channel()->GetChannelID() == Event->ChannelID()) {
        bool UseVps = timer->HasFlags(tfVps) && Event->Vps();
        timer->Matches(UseVps ? Event->Vps() : Event->StartTime(), true);
        int overlap = 0;
        if (UseVps)
            overlap = (start == Event->Vps()) ? FULLMATCH + (Event->IsRunning() ? 200 : 100) : 0;
        if (!overlap) {
            if (start <= Event->StartTime() && Event->EndTime() <= stop)
                overlap = FULLMATCH;
            else if (stop <= Event->StartTime() || Event->EndTime() <= start)
                overlap = 0;
            else
                overlap = (std::min(stop, Event->EndTime()) - std::max(start, Event->StartTime())) * FULLMATCH / std::max(Event->Duration(), 1);
        }
        if (Overlap)
            *Overlap = overlap;
        if (UseVps)
            return overlap > FULLMATCH ? tmFull : tmNone;
        return overlap >= FULLMATCH ? tmFull : overlap > 0 ? tmPartial : tmNone;
    }
    return tmNone;
}

// --- cConflictCheck -------------------------------------------------------
cConflictCheck::cConflictCheck()
{
    evaltimeList = NULL;
    timerList = NULL;
    failedList = NULL;
    relevantConflicts = 0;
    numConflicts = 0;
    devices = NULL;
    localConflicts = !(EPGSearchConfig.RemoteConflictCheck && Setup.SVDRPPeering);
    InitDevicesInfo();
}

cConflictCheck::~cConflictCheck()
{
    if (failedList && (failedList != evaltimeList)) {
        // if no local active timers but remote failed
        // we have a new list
        failedList->Clear();
        DELETENULL(failedList);
    }
    if (evaltimeList) {
        evaltimeList->Clear();
        DELETENULL(evaltimeList);
    }
    if (timerList) {
        timerList->Clear();
        DELETENULL(timerList);
    }
    if (devices)
        delete [] devices;
}

void cConflictCheck::InitDevicesInfo()
{
    if (devices)
        delete [] devices;

    devices = new cConflictCheckDevice[MAXDEVICES];
#ifdef DEBUG_CONFL
    numDevices = 4;
    for (int i = 0; i < numDevices; i++) {
        devices[i].devicenr = i;
        devices[i].device = NULL;
    }
#else
    numDevices = cDevice::NumDevices();
    for (int i = 0; i < numDevices; i++)
        devices[i].device = cDevice::GetDevice(i);
#endif

    BondDevices(Setup.DeviceBondings);
}

void cConflictCheck::BondDevices(const char *Bondings)
{
    LogFile.Log(3, "Bond Devices");
    if (Bondings) {
        cSatCableNumbers SatCableNumbers(MAXDEVICES, Bondings);
        int* array = SatCableNumbers.Array();
        for (int i = 0; i < SatCableNumbers.Size(); i++) {
            for (int j = 0; j < SatCableNumbers.Size(); j++) {
                if (array[i] > 0 && array[i] == array[j] && i != j) {
                    LogFile.Log(3, "Bond devices %i and %i.", i + 1, j + 1);
                    devices[i].bondedDevices.push_back(&(devices[j]));
                }
            }
        }
    }
    LogFile.Log(3, "Bond Devices done.");
}

void cConflictCheck::Check()
{
    if (failedList && (failedList != evaltimeList))
        DELETENULL(failedList);
    if (evaltimeList)
        DELETENULL(evaltimeList);
    if (timerList)
        DELETENULL(timerList);
    relevantConflicts = 0;
    numConflicts = 0;

    LogFile.Log(3, "check only local conflicts = %s", GetLocal() ? "yes" : "no");
    timerList = CreateCurrentTimerList();
    if (timerList) evaltimeList = CreateEvaluationTimeList(timerList);
    if (evaltimeList) failedList = CreateConflictList(evaltimeList, timerList);
    if ((!localConflicts) && timerList) failedList = CreateRemoteConflictList(timerList, failedList);
    if (failedList)
        for (cConflictCheckTime* checkTime = failedList->First(); checkTime; checkTime = failedList->Next(checkTime)) {
            LogFile.Log(2, "result of conflict check for %s:", DAYDATETIME(checkTime->evaltime));
            std::set<cConflictCheckTimerObj*, TimerObjSort>::iterator it;
            for (it = checkTime->failedTimers.begin(); it != checkTime->failedTimers.end(); ++it) {
                if (!localConflicts)
                    LogFile.Log(2, "timer '%s' (%s, channel %s) %s%s failed", (*it)->timer->File(), DAYDATETIME((*it)->timer->StartTime()), CHANNELNAME((*it)->timer->Channel()), (*it)->timer->Remote() ? "@" : "", (*it)->timer->Remote() ? (*it)->timer->Remote() : "");
                else
                    LogFile.Log(2, "timer '%s' (%s, channel %s) failed", (*it)->timer->File(), DAYDATETIME((*it)->timer->StartTime()), CHANNELNAME((*it)->timer->Channel()));
            }
        }
    if (numConflicts > 0 && gl_timerStatusMonitor)
        gl_timerStatusMonitor->SetConflictCheckAdvised();
}

cList<cConflictCheckTimerObj>* cConflictCheck::CreateCurrentTimerList()
{
    LogFile.Log(3, "current timer list creation started");
    cList<cConflictCheckTimerObj>* CurrentTimerList = NULL;

    // collect single event timers
    time_t tMax = 0;
    LOCK_TIMERS_READ;
    const cTimer* ti = NULL;
    for (ti = Timers->First(); ti; ti = Timers->Next(ti)) {
        tMax = std::max(tMax, ti->StartTime());
        if (localConflicts && ti->Remote()) continue;
        if (!ti->IsSingleEvent()) continue;
        // already recording?
        int deviceNr = -1;
        if (ti->Local()) // we check devices only for local timers
            deviceNr = gl_recStatusMonitor->TimerRecDevice(ti) - 1;

        // create a copy of this timer
        cTimer* clone = new cTimer(*ti);
        clone->SetEvent(ti->Event());

        cConflictCheckTimerObj* timerObj = new cConflictCheckTimerObj(clone, ti->StartTime(), ti->StopTime(), deviceNr, ti->Id());
        if (deviceNr >= 0) {
            devices[deviceNr].recTimers.insert(timerObj);
            timerObj->lastRecStart = ti->StartTime();
        }
        LogFile.Log(3, "add timer '%s' (%s, channel %s) for conflict check", ti->File(), DAYDATETIME(ti->StartTime()), CHANNELNAME(ti->Channel()));
        if (deviceNr >= 0)
            LogFile.Log(3, "timer already recording since %s on device %d", DAYDATETIME(ti->StartTime()), deviceNr + 1);

        if (!CurrentTimerList) CurrentTimerList = new cList<cConflictCheckTimerObj>;
        CurrentTimerList->Add(timerObj);
    }

    // collect repeating timers from now until the date of the timer with tMax
    time_t maxCheck = time(NULL) + std::min(14, EPGSearchConfig.checkMaxDays) * SECSINDAY;
    tMax = std::max(tMax, maxCheck);
    for (ti = Timers->First(); ti; ti = Timers->Next(ti)) {
        if (ti->IsSingleEvent()) continue;
        if (localConflicts && ti->Remote()) continue;  //JF???
        time_t day = time(NULL);
        while (day < tMax) {
            if (ti->DayMatches(day)) {
                time_t Start = cTimer::SetTime(day, cTimer::TimeToInt(ti->Start()));
                int deviceNr = -1;
                if (Start < time(NULL)) {
#ifndef DEBUG_CONFL
                    if (ti->Local() && ti->Recording())
                        deviceNr = gl_recStatusMonitor->TimerRecDevice(ti) - 1;
#else
                    if (Start + ti->StopTime() - ti->StartTime() > time(NULL))
                        deviceNr = 0;
#endif
                    if (deviceNr == -1) { // currently not recording, skip it
                        day += SECSINDAY;
                        continue;
                    }
                } else if (Start < ti->StartTime()) {
                    day += SECSINDAY;
                    continue;
                }

                // create a copy of this timer
                cTimer* clone = new cTimer(*ti);
                clone->SetEvent(ti->Event());

                cConflictCheckTimerObj* timerObj = new cConflictCheckTimerObj(clone, Start, Start + ti->StopTime() - ti->StartTime(), deviceNr, ti->Id());
                LogFile.Log(3, "add timer '%s' (%s, channel %s) for conflict check", ti->File(), DAYDATETIME(Start), CHANNELNAME(ti->Channel()));
                if (deviceNr >= 0) {
                    LogFile.Log(3, "timer already recording since %s on device %d", DAYDATETIME(Start), deviceNr + 1);
                    devices[deviceNr].recTimers.insert(timerObj);
                    timerObj->lastRecStart = Start;
                }
                if (!CurrentTimerList) CurrentTimerList = new cList<cConflictCheckTimerObj>;
                CurrentTimerList->Add(timerObj);
            }
            day += SECSINDAY;
        }
    }

    if (CurrentTimerList) CurrentTimerList->Sort();
    LogFile.Log(3, "current timer list created");
    return CurrentTimerList;
}

// create a list of all times that have to be checked
cList<cConflictCheckTime>* cConflictCheck::CreateEvaluationTimeList(cList<cConflictCheckTimerObj>* TimerList)
{
    LogFile.Log(3, "create check time list");
    cList<cConflictCheckTime>* EvalTimeList = NULL;
    for (cConflictCheckTimerObj* TimerObj = TimerList->First(); TimerObj; TimerObj = TimerList->Next(TimerObj)) {
        if (!TimerObj->timer->HasFlags(tfActive)) continue;

        if (TimerObj->timer->Remote()) continue; // here we check local timers only

        if (!EvalTimeList) EvalTimeList = new cList<cConflictCheckTime>;

        cConflictCheckTime* checkTime = NULL;

        // add all timer start times
        for (cConflictCheckTime* checkTimeTest = EvalTimeList->First(); checkTimeTest; checkTimeTest = EvalTimeList->Next(checkTimeTest)) {
            if (checkTimeTest->evaltime == TimerObj->start) {
                checkTime = checkTimeTest;
                break;
            }
        }
        if (!checkTime) {
            checkTime = new cConflictCheckTime(TimerObj->start);
            EvalTimeList->Add(checkTime);
        }
        checkTime->startingTimers.insert(TimerObj);


        // add all timer stop times
        checkTime = NULL;
        for (cConflictCheckTime* checkTimeTest = EvalTimeList->First(); checkTimeTest; checkTimeTest = EvalTimeList->Next(checkTimeTest)) {
            if (checkTimeTest->evaltime == TimerObj->stop) {
                checkTime = checkTimeTest;
                break;
            }
        }
        if (!checkTime) {
            checkTime = new cConflictCheckTime(TimerObj->stop);
            EvalTimeList->Add(checkTime);
        }
        checkTime->stoppingTimers.insert(TimerObj);
    }
    if (EvalTimeList)
        EvalTimeList->Sort();

    LogFile.Log(3, "create check time list - done");
    return EvalTimeList;
}

// this return a list of all conflicts
cList<cConflictCheckTime>* cConflictCheck::CreateConflictList(cList<cConflictCheckTime>* EvalTimeList, cList<cConflictCheckTimerObj>* TimerList)
{
    LogFile.Log(3, "create conflict list");
    maxCheck = time(NULL) + std::min(14, EPGSearchConfig.checkMaxDays) * SECSINDAY;

    // check each time
    for (cConflictCheckTime* checkTime = EvalTimeList->First(); checkTime; checkTime = EvalTimeList->Next(checkTime)) {
        int Conflicts = ProcessCheckTime(checkTime);
        if (Conflicts > 0) { // if there were conflicts do a retry as VDR would do a few seconds after the conflict
            LogFile.Log(3, "retry check time %s", DAYDATETIME(checkTime->evaltime));
            int OldConflicts = Conflicts;
            while (true) {
                Conflicts = ProcessCheckTime(checkTime);
                if (Conflicts == OldConflicts) break; // no change after retry?
                OldConflicts = Conflicts;
            };
        }
    }

    nextRelevantConflictDate = 0;
    for (cConflictCheckTime* checkTime = EvalTimeList->First(); checkTime;) { // clear the list
        cConflictCheckTime* checkTimeNext = EvalTimeList->Next(checkTime);
        if (checkTime->failedTimers.empty())
            EvalTimeList->Del(checkTime);
        else {
            bool allTimersIgnored = true;
            std::set<cConflictCheckTimerObj*, TimerObjSort>::iterator it;
            for (it = checkTime->failedTimers.begin(); it != checkTime->failedTimers.end(); ++it) {
                numConflicts++;
                if (!(*it)->ignore) {
                    if (!nextRelevantConflictDate)
                        nextRelevantConflictDate = checkTime->evaltime;
                    else
                        nextRelevantConflictDate = std::min(nextRelevantConflictDate, checkTime->evaltime);

                    relevantConflicts++;
                    allTimersIgnored = false;
                    break;
                }
            }
            if (allTimersIgnored)
                checkTime->ignore = true;
        }
        checkTime = checkTimeNext;
    }

    // store for external access
    cConflictCheckThread::m_cacheNextConflict = nextRelevantConflictDate;
    cConflictCheckThread::m_cacheRelevantConflicts = relevantConflicts;
    cConflictCheckThread::m_cacheTotalConflicts = numConflicts;

    LogFile.Log(3, "create conflict list - done");

    return EvalTimeList;
}

cList<cConflictCheckTime>* cConflictCheck::CreateRemoteConflictList(cList<cConflictCheckTimerObj>* TimerList, cList<cConflictCheckTime>* failedList)
{
    LogFile.Log(3, "add remote conflicts to list");
    bool foundRemote = false;
    cStringList RemoteHosts;
    // check  if we have any Remote timers
    RemoteHosts.Clear();
    for (cConflictCheckTimerObj* TimerObj = TimerList->First(); TimerObj; TimerObj = TimerList->Next(TimerObj)) {
        if (!TimerObj->timer->HasFlags(tfActive)) continue;

        if (TimerObj->timer->Remote()) {
            if (RemoteHosts.Find(TimerObj->timer->Remote()) < 0) {
                foundRemote = true;
                RemoteHosts.Append(strdup(TimerObj->timer->Remote()));
            }
        }
    }

    if (!foundRemote) {
        LogFile.Log(3, "no remote timers to add");
        return failedList;
    }

    RemoteHosts.Sort();

    cStringList Response;
    // for all RemoteHosts
    for (int i = 0; i < RemoteHosts.Size(); i++) {
        Response.Clear();
        if (ExecSVDRPCommand(RemoteHosts[i], "PLUG epgsearch LSCC REL", &Response)) {
            for (int j = 0; j < Response.Size(); j++) {
                const char *s = Response[j];
                int Code = SVDRPCode(s);
                if (Code == 901) {
                    LogFile.Log(3, "conflictcheck %s no remote conflicts found", RemoteHosts[i]);
                    continue;
                }  else if (Code != 900) {
                    LogFile.Log(2, "Invalid remote response %d %s", Code,
                                SVDRPValue(s));
                    break;
                }  else if (const char* line = SVDRPValue(s)) {
                    LogFile.Log(2, "remote conflictcheck line %s", line);
                    int Id, recPart;
                    char rest[256];
                    time_t evaltime;
                    sscanf(line, "%ld:%d|%s", &evaltime, &Id, rest);
                    cConflictCheckTime* checkTime = new cConflictCheckTime(evaltime);
                    if (!failedList)
                        failedList = new cList<cConflictCheckTime>;
                    LogFile.Log(2, "added remote checkTime %s to failedList", DAYDATETIME(evaltime));
                    failedList->Add(checkTime);
                    if (!nextRelevantConflictDate)
                        nextRelevantConflictDate = checkTime->evaltime;
                    else
                        nextRelevantConflictDate = std::min(nextRelevantConflictDate, checkTime->evaltime);
                    numConflicts++;
                    // find TimerObj with id Id in timerList
                    cConflictCheckTimerObj* failedTimer = NULL;
                    bool foundfT = false;
                    for (failedTimer = TimerList->First(); failedTimer; failedTimer = TimerList->Next(failedTimer)) {
                        if (failedTimer->timer->Id() == Id) {
                            foundfT = true;
                            break;
                        }
                    }
                    if (!foundfT) {
                        LogFile.Log(2, "remote failed Timer disappeared");
                        continue;
                    }
                    LogFile.Log(2, "create remote failedTimer with Id %d", Id);
                    failedTimer->conflCheckTime = checkTime;
                    failedTimer->origIndex = Id;
                    sscanf(rest, "%d|%s", &recPart, rest);
                    failedTimer->recDuration = ((failedTimer->stop - failedTimer->start) * recPart / 100);
                    cConflictCheckTimerObj* concurrentTimer = NULL;
                    while (strlen(rest) > 0) {
                        int n = sscanf(rest, "%d#%s", &Id, rest);
                        if (n < 2) {
                            if (sscanf(rest, "%d", &Id) <= 0) {
                                LogFile.Log(2, "error scanning rest of line %s", rest);
                                break;
                            }
                            *rest = 0;  // TODO :<more timers> possible ??
                        }
                        // find TimerObj itcc for with Id in timerList
                        bool foundcT = false;
                        for (concurrentTimer = TimerList->First(); concurrentTimer; concurrentTimer = TimerList->Next(concurrentTimer)) {
                            if (concurrentTimer->timer->Id() == Id) {
                                foundcT = true;
                                break;
                            }
                        }
                        if (!foundcT) {
                            LogFile.Log(2, "remote concurrent Timer disappeared");
                            continue;
                        }
                        if (!failedTimer->concurrentTimers)
                            failedTimer->concurrentTimers = new std::set<cConflictCheckTimerObj*, TimerObjSort>;
                        LogFile.Log(2, "insert remote Id %d into concurrentTimers", concurrentTimer->timer->Id());
                        failedTimer->concurrentTimers->insert(concurrentTimer);
                    } // while concurrent Timers
                    LogFile.Log(2, "insert Id %d into checkTime->failedTimers", failedTimer->timer->Id());
                    checkTime->failedTimers.insert(failedTimer);
                    relevantConflicts++;
                } else
                    LogFile.Log(2, "got Code %d, but no Value from %s", Code, RemoteHosts[i]);
            } // received response
        } else {
            LogFile.Log(2, "ExecSVDRPCommand failed for %s", RemoteHosts[i]);
        }
    } // for all RemoteHosts
    cConflictCheckThread::m_cacheNextConflict = nextRelevantConflictDate;
    cConflictCheckThread::m_cacheTotalConflicts = numConflicts;
    cConflictCheckThread::m_cacheRelevantConflicts = relevantConflicts;
    LogFile.Log(3, "add remote conflicts done");
    return failedList;
}

// checks for conflicts at one special time
int cConflictCheck::ProcessCheckTime(cConflictCheckTime* checkTime)
{
    if (!checkTime) return 0;

    LogFile.Log(3, "check time %s", DAYDATETIME(checkTime->evaltime));

    LogFile.Log(3, "detach stopping timers");
    int Conflicts = 0;
    // detach all stopping timers from their devices
    std::set<cConflictCheckTimerObj*, TimerObjSort>::iterator it;
    for (it = checkTime->stoppingTimers.begin(); it != checkTime->stoppingTimers.end(); ++it)
        if ((*it) && (*it)->device >= 0) {
            LogFile.Log(3, "detach device %d from  timer '%s' (%s, channel %s) at %s", ((*it)->device) + 1, (*it)->timer->File(), DAYDATETIME((*it)->start), CHANNELNAME((*it)->timer->Channel()), DAYDATETIME(checkTime->evaltime));
            devices[(*it)->device].recTimers.erase(*it);
            (*it)->lastRecStop = checkTime->evaltime;
            if ((*it)->lastRecStart > 0 && (*it)->lastRecStart < (*it)->lastRecStop) {
                (*it)->recDuration += (*it)->lastRecStop - (*it)->lastRecStart;
                (*it)->lastRecStart = 0;
                if (((*it)->stop - (*it)->start - (*it)->recDuration) < EPGSearchConfig.checkMinDuration * 60)
                    (*it)->ignore = true;
            }
        }

    LogFile.Log(3, "add pending timers");
    // if we have pending timers add them to the current start list
    for (it = pendingTimers.begin(); it != pendingTimers.end(); ++it) {
        if ((*it) && (*it)->stop > checkTime->evaltime)
            checkTime->startingTimers.insert(*it);
    }
    pendingTimers.clear();

    LogFile.Log(3, "attach starting timers");
    // handle starting timers
    for (it = checkTime->startingTimers.begin(); it != checkTime->startingTimers.end(); ++it) {
        bool NeedsDetachReceivers = false;
        if (!(*it) || (*it)->device >= 0) continue; // already has a device
        int device = GetDevice(*it, &NeedsDetachReceivers);
        if (device >= 0) { // device will be attached?
            if (NeedsDetachReceivers) { // but needs to detach all others?
                // disable running timers
                std::set<cConflictCheckTimerObj*, TimerObjSort>::iterator it2 = devices[device].recTimers.begin();
                for (; it2 != devices[device].recTimers.end(); ++it2) {
                    LogFile.Log(3, "stopping timer '%s' (%s, channel %s) at %s on device %d because of higher priority", (*it2)->timer->File(), DAYDATETIME((*it2)->start), CHANNELNAME((*it2)->timer->Channel()), DAYDATETIME(checkTime->evaltime), device + 1);
                    AddConflict((*it2), checkTime, pendingTimers);
                    Conflicts++;
                }
                devices[device].recTimers.clear();
            }
            devices[device].recTimers.insert(*it);
            (*it)->device = device;
            (*it)->lastRecStart = checkTime->evaltime;

            LogFile.Log(3, "recording  timer '%s' (%s, channel %s) at %s on device %d", (*it)->timer->File(), DAYDATETIME((*it)->start), CHANNELNAME((*it)->timer->Channel()), DAYDATETIME(checkTime->evaltime), device + 1);
        } else {
            AddConflict((*it), checkTime, pendingTimers);
            Conflicts++;
        }
    }
    LogFile.Log(3, "check time %s - done", DAYDATETIME(checkTime->evaltime));
    return Conflicts;
}

eModuleStatus cConflictCheck::CamSlotModuleStatus(cCamSlot *CamSlot)
{
    if (!CamSlot) return msNone;
    if ((int)camSlotStatusArray.size() != CamSlots.Count())
        for (cCamSlot *CamSlot = CamSlots.First(); CamSlot; CamSlot = CamSlots.Next(CamSlot))
            camSlotStatusArray.push_back(CamSlot->ModuleStatus());
    if (CamSlot->Index() < (int)camSlotStatusArray.size())
        return camSlotStatusArray[CamSlot->Index()];
    else
        return msNone;
}

int cConflictCheck::GetDevice(cConflictCheckTimerObj* TimerObj, bool* NeedsDetachReceivers)
{
    int Priority = TimerObj->timer->Priority();
    const cChannel* Channel = TimerObj->timer->Channel();

    // Collect the current priorities of all CAM slots that can decrypt the channel:
    int selDevice = -1;
    int NumCamSlots = CamSlots.Count();
    int SlotPriority[NumCamSlots];
    int NumUsableSlots = 0;
    if (Channel->Ca() >= CA_ENCRYPTED_MIN) {
        for (cCamSlot *CamSlot = CamSlots.First(); CamSlot; CamSlot = CamSlots.Next(CamSlot)) {
            SlotPriority[CamSlot->Index()] = MAXPRIORITY + 1; // assumes it can't be used
            if (CamSlotModuleStatus(CamSlot) == msReady) {
                if (CamSlot->ProvidesCa(Channel->Caids())) {
                    if (!ChannelCamRelations.CamChecked(Channel->GetChannelID(), CamSlot->SlotNumber())) {
                        SlotPriority[CamSlot->Index()] = CamSlot->Priority();
                        NumUsableSlots++;
                    }
                }
            }
        }
#ifdef CFLC
        int NumUsableSlots = 1;
#endif
        if (!NumUsableSlots)
            return selDevice; // no CAM is able to decrypt this channel
    }

    if (NeedsDetachReceivers)
        *NeedsDetachReceivers = false;

    uint32_t Impact = 0xFFFFFFFF; // we're looking for a device with the least impact
    for (int j = 0; j < NumCamSlots || !NumUsableSlots; j++) {
        if (NumUsableSlots && SlotPriority[j] > MAXPRIORITY)
            continue; // there is no CAM available in this slot
        for (int i = 0; i < numDevices; i++) {
            if (Channel->Ca() && Channel->Ca() <= CA_DVB_MAX && Channel->Ca() != devices[i].CardIndex() + 1)
                continue; // a specific card was requested, but not this one
            if (NumUsableSlots && !CamSlots.Get(j)->Assign(devices[i].device, true))
                continue; // CAM slot can't be used with this device
            bool ndr;
            if (devices[i].ProvidesChannel(Channel, Priority, &ndr)) { // this device is basically able to do the job
                if (NumUsableSlots && devices[i].CamSlot() && devices[i].CamSlot() != CamSlots.Get(j))
                    ndr = true; // using a different CAM slot requires detaching receivers
                // Put together an integer number that reflects the "impact" using
                // this device would have on the overall system. Each condition is represented
                // by one bit in the number (or several bits, if the condition is actually
                // a numeric value). The sequence in which the conditions are listed corresponds
                // to their individual severity, where the one listed first will make the most
                // difference, because it results in the most significant bit of the result.
                uint32_t imp = 0;
                // prefer the primary device for live viewing if we don't need to detach existing receivers
                imp <<= 1; ;
                // use receiving devices if we don't need to detach existing receivers
                imp <<= 1;
                imp |= !devices[i].Receiving() || ndr;
                // avoid devices that are receiving
                imp <<= 1;
                imp |= devices[i].Receiving();
                // use the device with the lowest priority (+MAXPRIORITY to assure that values -99..99 can be used)
                imp <<= 8;
                imp |= std::min(std::max(devices[i].Priority() + MAXPRIORITY, 0), 0xFF);
                // use the CAM slot with the lowest priority (+MAXPRIORITY to assure that values -99..99 can be used)
                imp <<= 8;
                imp |= std::min(std::max((NumUsableSlots ? SlotPriority[j] : 0) + MAXPRIORITY, 0), 0xFF);
                // avoid devices if we need to detach existing receivers
                imp <<= 1;
                imp |= ndr;
                // avoid the primary device
                imp <<= 1;
                imp |= devices[i].IsPrimaryDevice();
                // avoid cards with Common Interface for FTA channels
                imp <<= 1;
                imp |= NumUsableSlots ? 0 : devices[i].HasCi();
                // avoid full featured cards
                imp <<= 1;
                imp |= devices[i].HasDecoder();
                // prefer CAMs that are known to decrypt this channel
                imp <<= 1;
                imp |= NumUsableSlots ? !ChannelCamRelations.CamDecrypt(Channel->GetChannelID(), j + 1) : 0;
                if (imp < Impact) {
                    // This device has less impact than any previous one, so we take it.
                    Impact = imp;
                    selDevice = i;
                    if (NeedsDetachReceivers)
                        *NeedsDetachReceivers = ndr;
                }
            }
        }
        if (!NumUsableSlots)
            break; // no CAM necessary, so just one loop over the devices
    }
    return selDevice;
}

void cConflictCheck::AddConflict(cConflictCheckTimerObj* TimerObj, cConflictCheckTime* CheckTime, std::set<cConflictCheckTimerObj*>& pendingTimers)
{
    for (cConflictCheckTimerObj* concTimer = timerList->First(); concTimer; concTimer = timerList->Next(concTimer)) {
        if (concTimer->timer->Remote()) continue; // ignore overlapping remote timers
        if (concTimer->start >= TimerObj->stop) continue;
        if (concTimer->stop <= TimerObj->start) continue;
        if (!TimerObj->concurrentTimers) TimerObj->concurrentTimers = new std::set<cConflictCheckTimerObj*, TimerObjSort>;
        TimerObj->concurrentTimers->insert(concTimer);
    }
    TimerObj->ignore = (TimerObj->timer->Priority() < EPGSearchConfig.checkMinPriority) || TimerObj->start > maxCheck;
    CheckTime->concurrentRecs.insert(TimerObj);
    pendingTimers.insert(TimerObj);

    TimerObj->lastRecStop = CheckTime->evaltime;
    if (TimerObj->lastRecStart > 0 && TimerObj->lastRecStart < TimerObj->lastRecStop) {
        TimerObj->recDuration += TimerObj->lastRecStop - TimerObj->lastRecStart;
        TimerObj->lastRecStart = 0;
        if ((TimerObj->stop - TimerObj->start - TimerObj->recDuration) < EPGSearchConfig.checkMinDuration * 60)
            TimerObj->ignore = true;
    }

    TimerObj->device = -1;
    if (!TimerObj->conflCheckTime)
        TimerObj->conflCheckTime = CheckTime;
    else
        return;
    CheckTime->failedTimers.insert(TimerObj);

    LogFile.Log(3, "conflict found for timer '%s' (%s, channel %s)", TimerObj->timer->File(), DAYDATETIME(TimerObj->start), CHANNELNAME(TimerObj->timer->Channel()));
}

bool cConflictCheck::TimerInConflict(const cTimer* timer)
{
    if (!failedList)
        return false;
    for (cConflictCheckTime* checkTime = failedList->First(); checkTime; checkTime = failedList->Next(checkTime)) {
        std::set<cConflictCheckTimerObj*, TimerObjSort>::iterator it;
        for (it = checkTime->failedTimers.begin(); it != checkTime->failedTimers.end(); ++it) {
            if (!(*it)->ignore) {
                std::set<cConflictCheckTimerObj*, TimerObjSort>::iterator it2;
                if ((*it)->concurrentTimers) {
                    LOCK_TIMERS_READ;
                    for (it2 = (*it)->concurrentTimers->begin(); it2 != (*it)->concurrentTimers->end(); ++it2) {
                        if ((*it2)->OrigTimer(Timers) == timer)
                            return true;
                    }
                }
            }
        }
    }
    return false;
}

void cConflictCheck::EvaluateConflCheckCmd()
{
    if (strlen(EPGSearchConfig.conflCheckCmd) > 0) {
        LogFile.Log(2, "evaluating conflict check command '%s'", EPGSearchConfig.conflCheckCmd);
        for (cConflictCheckTime* ct = failedList->First(); ct; ct = failedList->Next(ct)) {
            if (ct->ignore) continue;
            std::set<cConflictCheckTimerObj*, TimerObjSort>::iterator it;
            for (it = ct->failedTimers.begin(); it != ct->failedTimers.end(); ++it)
                if ((*it) && !(*it)->ignore) {
                    string result = EPGSearchConfig.conflCheckCmd;
                    LOCK_TIMERS_READ;
                    if (!(*it)->OrigTimer(Timers)) {
                        LogFile.Log(3, "timer has disappeared meanwhile");
                        continue;
                    } else
                        LogFile.Log(3, "evaluating conflict check command for timer '%s' (%s, channel %s)", (*it)->timer->File(), DAYDATETIME((*it)->start), CHANNELNAME((*it)->timer->Channel()));

                    if ((*it)->Event()) {
                        cVarExpr varExprEvent(result);
                        result = varExprEvent.Evaluate((*it)->Event());
                    }
                    cVarExpr varExprTimer(result);
                    result =  varExprTimer.Evaluate((*it)->timer);
                    cVarExpr varExpr(result);
                    varExpr.Evaluate();
                }
        }
    }
}
