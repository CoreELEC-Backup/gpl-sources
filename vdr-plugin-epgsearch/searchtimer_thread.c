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
#ifdef __FreeBSD__
#include <netinet/in.h>
#endif
#include "searchtimer_thread.h"
#include "epgsearchcfg.h"
#include "epgsearchcats.h"
#include "epgsearchtools.h"
#include "changrp.h"
#include "switchtimer.h"
#include <vdr/tools.h>
#include <vdr/plugin.h>
#include "switchtimer_thread.h"
#include "services.h"
#include "conflictcheck.h"
#include "uservars.h"
#include "svdrpclient.h"
#include "noannounce.h"
#include "timer_thread.h"
#include "timerdone.h"
#include "menu_deftimercheckmethod.h"
#include "timerstatus.h"
#include "epgsearch.h"
#include <vdr/eitscan.h>

// priority for background thread
#define SEARCHTIMER_NICE 19

#define DAYBUFFERSIZE 32

extern int updateForced;

cSearchTimerThread *cSearchTimerThread::m_Instance = NULL;
cSearchResults cSearchTimerThread::announceList;
bool cSearchTimerThread::justRunning = false;

cSearchTimerThread::cSearchTimerThread(cPluginEpgsearch* thePlugin)
    : cThread("EPGSearch: searchtimer")
{
    m_plugin = thePlugin;
    m_Active = false;
    m_lastUpdate = time(NULL);
}

cSearchTimerThread::~cSearchTimerThread()
{
    if (m_Active)
        Stop();
}

void cSearchTimerThread::Init(cPluginEpgsearch* thePlugin, bool activatePermanently)
{
    if (activatePermanently) {
        EPGSearchConfig.useSearchTimers = 1;
        thePlugin->SetupStore("UseSearchTimers",  EPGSearchConfig.useSearchTimers);
    }
    if (!EPGSearchConfig.useSearchTimers)
        return;
    if (m_Instance == NULL) {
        m_Instance = new cSearchTimerThread(thePlugin);
        m_Instance->Start();
    }
}

void cSearchTimerThread::Exit(void)
{
    if (m_Instance != NULL) {
        m_Instance->Stop();
        DELETENULL(m_Instance);
    }
}

void cSearchTimerThread::Stop(void)
{
    m_Active = false;
    Wait.Signal();
    Cancel(6);
}


const cTimer *cSearchTimerThread::GetTimer(cSearchExt *searchExt, const cEvent *pEvent, bool& bTimesMatchExactly)
{
    LOCK_TIMERS_READ;
    LOCK_CHANNELS_READ;
    const cChannel *channel = Channels->GetByChannelID(pEvent->ChannelID(), true, true);
    if (!channel)
        return NULL;

    struct tm tm_r;

    bool UseVPS = searchExt->useVPS && pEvent->Vps() && Setup.UseVps;
    time_t eStart = pEvent->StartTime();
    time_t eStop = pEvent->EndTime();
    int eDuration = pEvent->Duration();

    int AllowedDiff = (eDuration < 10 * 60) ? (eDuration / 60) : 10; // allowed start/stop difference

    int testVpsStart = 0;
    int testVpsStop = 0;
    time_t testVpsDay = 0;
    if (UseVPS) {
        eStart = pEvent->Vps();
        eStop = eStart + pEvent->Duration();
        cTimer VpsTestTimer(pEvent);
        testVpsStart = cTimer::TimeToInt(VpsTestTimer.Start());
        testVpsStop = cTimer::TimeToInt(VpsTestTimer.Stop());
        testVpsDay = VpsTestTimer.Day();
    }

    tm *tmStartEv = localtime_r(&eStart, &tm_r);

    for (const cTimer *ti = Timers->First(); ti; ti = Timers->Next(ti)) {
        if (ti->Channel() != channel)
            continue;

        if (ti->WeekDays()) // ignore serial timers
            continue;

        if (ti->Remote()) // ignore remote timers
            continue;
        // ignore manual timers if this search could modify them
        if ((searchExt->action == searchTimerActionRecord || searchExt->action == searchTimerActionInactiveRecord) && TriggeredFromSearchTimerID(ti) == -1) // manual timer
            continue;

        if (UseVPS && ti->HasFlags(tfVps)) {
            if (testVpsDay != ti->Day()) continue;
            int timerVpsStart = cTimer::TimeToInt(ti->Start());
            int timerVpsStop = cTimer::TimeToInt(ti->Stop());

            if (abs(testVpsStart - timerVpsStart) > AllowedDiff * 60) continue;
            if (abs(testVpsStop - timerVpsStop) > AllowedDiff * 60) continue;

            bTimesMatchExactly = (testVpsStart == timerVpsStart && testVpsStop == timerVpsStop);
            return ti;
        } else {
            time_t tStart = ti->StartTime() + searchExt->MarginStart * 60;
            time_t tStop = ti->StopTime() - searchExt->MarginStop * 60;
            tm *tmStartTi = localtime_r(&tStart, &tm_r);
            if (tmStartEv->tm_mday != tmStartTi->tm_mday)
                continue;

            // some providers change EPG times only for a few seconds
            // ignore this to avoid search timer mails because of such changes
            bTimesMatchExactly = (abs(tStart - eStart) < 60 && abs(tStop - eStop) < 60);

            if (abs(tStart - eStart) < AllowedDiff * 60 && abs(tStop - eStop) < AllowedDiff * 60) // accept a difference of max 10 min., but only if the event duration is more than 10 minutes
                return ti;
        }
    }
    return NULL;
}

bool cSearchTimerThread::TimerWasModified(const cTimer* t)
{
    if (!t) return false;
    if (t->HasFlags(tfVps)) return false; // if timer uses VPS we ignore user changes

    char* start = GetAuxValue(t, "start");
    char* stop = GetAuxValue(t, "stop");
    bool bMod = false;
    if (start || stop) {
        time_t StartTime = time_t(atol(start));
        time_t StopTime = time_t(atol(stop));
        if (abs(t->StartTime() - StartTime) >= 60 || abs(t->StopTime() - StopTime) >= 60)
            bMod = true;
    }
    if (start) free(start);
    if (stop) free(stop);
    return bMod;
}

void cSearchTimerThread::Action(void)
{
    if (EPGSearchConfig.useExternalSVDRP && !epgsSVDRP::cSVDRPClient::SVDRPSendCmd) {
        LogFile.eSysLog("ERROR - SVDRPSend script not specified or does not exist (use -f option)");
        return;
    }
    SetPriority(SEARCHTIMER_NICE);

    m_Active = true;
    // let VDR do its startup
    if (!cPluginEpgsearch::VDR_readyafterStartup)
        LogFile.Log(2, "SearchTimerThread: waiting for VDR to become ready...");
    while (Running() && m_Active && !cPluginEpgsearch::VDR_readyafterStartup)
        Wait.Wait(1000);

    time_t nextUpdate = time(NULL);
    while (m_Active && Running()) {
        time_t now = time(NULL);
        bool needUpdate = NeedUpdate();
        if (now >= nextUpdate || needUpdate) {
            justRunning = true;

            if (updateForced & UPDS_WITH_EPGSCAN) {
                LogFile.Log(1, "starting EPG scan before search timer update");
                EITScanner.ForceScan();
                do {
                    Wait.Wait(1000);
                } while (EITScanner.Active() && m_Active && Running());
                LogFile.Log(1, "EPG scan finished");
            }
            // wait if TimersWriteLock is set or waited for
            {
                LOCK_TIMERS_WRITE;
                Timers->SetExplicitModify();
            }
            LogFile.iSysLog("search timer update started");

            UserVars.ResetCache(); // reset internal cache of user vars
            cTimerObjList* pOutdatedTimers = NULL;

            // for thread safeness we work with a copy of the current searches,
            // because SVDRP would not work if the main thread would be locked
            cSearchExts* localSearchExts = SearchExts.Clone();
            localSearchExts->SortBy(CompareSearchExtPrioDescTerm);
            cSearchExt *searchExt = localSearchExts->First();
            // reset announcelist
            announceList.Clear();
            while (searchExt && m_Active && Running()) {
                if (!searchExt->IsActiveAt(now)) {
                    searchExt = localSearchExts->Next(searchExt);
                    continue;
                }
                pOutdatedTimers = searchExt->GetTimerList(pOutdatedTimers);

                cSearchResults* pSearchResults = searchExt->Run(-1, true);
                if (!pSearchResults) {
                    searchExt = localSearchExts->Next(searchExt);
                    continue;
                }
                pSearchResults->SortBy(CompareEventTime);

                if (searchExt->pauseOnNrRecordings > 0)
                    searchExt->CheckExistingRecordings(pSearchResults);

                for (cSearchResult* pResultObj = pSearchResults->First();
                     pResultObj;
                     pResultObj = pSearchResults->Next(pResultObj)) {
                    if (!Running()) break;
                    const cEvent* pEvent = pResultObj->event;
                    if (!pEvent)
                        continue;

                    {
                        LOCK_CHANNELS_READ;
                        const cChannel *channel = Channels->GetByChannelID(pEvent->ChannelID(), true, true);
                        if (!channel)
                            continue;
                    }

                    int index = 0;
                    cTimer *timer = new cTimer(pEvent);

                    // create the file
                    char* file = NULL;
                    if ((file = searchExt->BuildFile(pEvent)) != NULL) {
                        while (strstr(file, "!^pipe^!")) file = strreplace(file, "!^pipe^!", "|"); // revert the translation of '|' in BuildFile
                        if (strstr(file, "!^invalid^!") || strlen(file) == 0) {
                            LogFile.eSysLog("Skipping timer due to invalid or empty filename");
                            if (time(NULL) <= timer->StopTime())
                                pOutdatedTimers->DelTimer(timer);
                            delete timer;
                            free(file);
                            continue;
                        }
                        timer->SetFile(file);
                        free(file);
                    }
                    int Priority = searchExt->Priority;
                    int Lifetime = searchExt->Lifetime;

                    // search for an already existing timer
                    bool bTimesMatchExactly = false;
                    const cTimer *t = GetTimer(searchExt, pEvent, bTimesMatchExactly);

                    char* Summary = NULL;
                    uint timerMod = tmNoChange;

                    if (t) {
                        // already exists
                        pOutdatedTimers->DelTimer(t);

                        if (!t->HasFlags(tfActive)) {
                            // do not update inactive timers
                            LogFile.Log(2, "timer for '%s~%s' (%s - %s, channel %d) not active - won't be touched", pEvent->Title() ? pEvent->Title() : "no title", pEvent->ShortText() ? pEvent->ShortText() : "no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent));
                            delete timer;
                            continue;
                        }

                        int triggerID = TriggeredFromSearchTimerID(t);
                        if (!pResultObj->needsTimer && !t->Recording()) { // not needed
                            if (triggerID == searchExt->ID) {
                                LogFile.Log(1, "delete timer for '%s~%s' (%s - %s, channel %d)", pEvent->Title() ? pEvent->Title() : "no title", pEvent->ShortText() ? pEvent->ShortText() : "no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent));
                                RemoveTimer(t, pEvent);
                            } else if (triggerID == -1) { //manual timer
                                LogFile.Log(2, "keep obsolete timer for '%s~%s' (%s - %s, channel %d) - was manually created", pEvent->Title() ? pEvent->Title() : "no title", pEvent->ShortText() ? pEvent->ShortText() : "no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent));
                            }
                            delete timer;
                            continue;
                        }
                        if (TimerWasModified(t)) { // don't touch timer modified by user
                            LogFile.Log(2, "timer for '%s~%s' (%s - %s, channel %d) modified by user - won't be touched", pEvent->Title() ? pEvent->Title() : "no title", pEvent->ShortText() ? pEvent->ShortText() : "no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent));
                            delete timer;
                            continue;
                        }
                        if (triggerID > -1 && triggerID != searchExt->ID) {
                            LogFile.Log(2, "timer for '%s~%s' (%s - %s, channel %d) already created by search id %d - won't be touched", pEvent->Title() ? pEvent->Title() : "no title", pEvent->ShortText() ? pEvent->ShortText() : "no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent), triggerID);
                            delete timer;
                            continue;
                        }

                        char* pFile = NULL; // File is prepared for svdrp, so prepare t->File for comparison too
                        msprintf(&pFile, "%s", t->File());
                        pFile = strreplace(pFile, ':', '|');
                        pFile = strreplace(pFile, " ~", "~");
                        pFile = strreplace(pFile, "~ ", "~");

                        Summary =  SummaryExtended(searchExt, t, pEvent);

                        if (bTimesMatchExactly && strcmp(pFile, timer->File()) == 0
                            && (t->Aux() != NULL && strcmp(t->Aux(), Summary) == 0)
                           ) {
                            // dir, title, episode name and summary have not changed
                            if (Summary) free(Summary);
                            delete timer;
                            free(pFile);
                            continue;
                        } else {
                            if (!bTimesMatchExactly) timerMod = (uint)timerMod | tmStartStop;
                            if (strcmp(pFile, timer->File()) != 0) timerMod |= tmFile;
                            if (t->Aux() != NULL && strcmp(t->Aux(), Summary) != 0) {
                                char* oldEventID = GetAuxValue(t, "eventid");
                                char* newEventID = GetAuxValue(Summary, "eventid");
                                if (oldEventID && newEventID && strcmp(oldEventID, newEventID) != 0)
                                    timerMod |= tmAuxEventID;
                                free(oldEventID);
                                free(newEventID);
                            }

                            if (LogFile.Level() >= 3) { // output reasons for a timer modification
                                if (timerMod & tmStartStop)
                                    LogFile.Log(3, "timer for '%s~%s' (%s - %s, channel %d) : start/stop has changed", pEvent->Title() ? pEvent->Title() : "no title", pEvent->ShortText() ? pEvent->ShortText() : "no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent));
                                if (timerMod & tmFile)
                                    LogFile.Log(3, "timer for '%s~%s' (%s - %s, channel %d) : title and/or episdode has changed (old: %s, new: %s", pEvent->Title() ? pEvent->Title() : "no title", pEvent->ShortText() ? pEvent->ShortText() : "no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent), timer ? timer->File() : "", pFile);
                                if (timerMod & tmAuxEventID)
                                    LogFile.Log(3, "timer for '%s~%s' (%s - %s, channel %d) : aux info for event id has changed", pEvent->Title() ? pEvent->Title() : "no title", pEvent->ShortText() ? pEvent->ShortText() : "no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent));
                            }
                            index = t->Id();
                            Priority = t->Priority();
                            Lifetime = t->Lifetime();
                        }
                        free(pFile);

                        if (t->Recording() && t->StopTime() == timer->StopTime()) {
                            // only update recording timers if stop time has changed, since all other settings can't be modified
                            LogFile.Log(2, "timer for '%s~%s' (%s - %s, channel %d) already recording - no changes possible", pEvent->Title() ? pEvent->Title() : "no title", pEvent->ShortText() ? pEvent->ShortText() : "no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent));
                            delete timer;
                            continue;
                        }
                    } else {
                        if (!pResultObj->needsTimer) {
                            delete timer;
                            continue;
                        }
                    }

                    if (searchExt->action == searchTimerActionAnnounceViaOSD) {
                        if (t || // timer already exists or
                            NoAnnounces.InList(pEvent) || // announcement not wanted anymore or
                            (EPGSearchConfig.noAnnounceWhileReplay &&
                             cDevice::PrimaryDevice()->Replaying() &&
                             !(updateForced & UPDS_WITH_OSD))  // no announce while replay within automatic updates
                           ) {
                            if (Summary) free(Summary);
                            delete timer;
                            continue;
                        }
                        if (!announceList.Lookup(pEvent))
                            announceList.Add(new cSearchResult(pEvent, searchExt->ID));

                        if (Summary) free(Summary);
                        delete timer;
                        continue;
                    }

                    if (searchExt->action == searchTimerActionAnnounceViaMail) {
                        if (t || // timer already exists or
                            NoAnnounces.InList(pEvent) ||
                            pEvent->StartTime() < time(NULL)) { // already started?
                            if (Summary) free(Summary);
                            delete timer;
                            continue;
                        }
                        mailNotifier.AddAnnounceEventNotification(pEvent->EventID(), pEvent->ChannelID(), searchExt->ID);

                        if (Summary) free(Summary);
                        delete timer;
                        continue;
                    }
                    if (searchExt->action == searchTimerActionSwitchOnly ||
                        searchExt->action == searchTimerActionAnnounceAndSwitch) { // add to switch list
                        time_t now = time(NULL);
                        if (now < pEvent->StartTime()) {
                            if (!SwitchTimers.InSwitchList(pEvent)) {
                                cMutexLock SwitchTimersLock(&SwitchTimers);
                                int mode = 0;
                                if (searchExt->action == searchTimerActionAnnounceAndSwitch)
                                    mode = 2;
                                LogFile.Log(3, "adding switch timer event for '%s~%s' (%s - %s); search timer: '%s'", pEvent->Title(), pEvent->ShortText() ? pEvent->ShortText() : "", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), searchExt->search);
                                SwitchTimers.Add(new cSwitchTimer(pEvent, searchExt->switchMinsBefore, mode,
                                                                  searchExt->unmuteSoundOnSwitch));
                                SwitchTimers.Save();
                                cSwitchTimerThread::Init();
                            }
                        }
                        if (Summary) free(Summary);
                        delete timer;
                        continue;
                    }

                    if (AddModTimer(timer, index, searchExt, pEvent, Priority, Lifetime, Summary, timerMod)) {
                        if (index == 0)
                            LogFile.Log(1, "added timer for '%s~%s' (%s - %s); search timer: '%s'", pEvent->Title(), pEvent->ShortText() ? pEvent->ShortText() : "", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), searchExt->search);
                        else
                            LogFile.Log(1, "modified timer %d for '%s~%s' (%s - %s); search timer: '%s'", index, pEvent->Title(), pEvent->ShortText() ? pEvent->ShortText() : "", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), searchExt->search);
                    }
                    if (Summary) free(Summary);
                    delete timer;
                }
                delete pSearchResults;
                searchExt = localSearchExts->Next(searchExt);
            }

            if (localSearchExts) delete localSearchExts;

            if (pOutdatedTimers) {
                if (pOutdatedTimers->Count() > 0) {
                    LogFile.Log(1, "removing outdated timers");
                    for (cTimerObj *tObj = pOutdatedTimers->First(); tObj; tObj = pOutdatedTimers->Next(tObj)) {
                        const cTimer* t = tObj->timer;
                        // timer could have been deleted meanwhile, so check if its still there
                        {
                            LOCK_TIMERS_READ;
                            if (!Timers->Contains(t))
                                continue;
                        }

                        if (TimerWasModified(t)) continue;
                        if (!t->Event()) continue; // if there is no event, we keep the timer, since EPG could have been cleared
                        if (time(NULL) > t->StopTime()) continue; // if this timer has (just) finished, let VDR do the cleanup
                        if (t->Recording()) continue; // do not remove recording timers
                        LogFile.Log(1, "delete timer for '%s' (%s, channel %s)", t->File(), DAYDATETIME(t->StartTime()), CHANNELNAME(t->Channel()));
                        RemoveTimer(t, t->Event());
                    }
                    LogFile.Log(1, "removing outdated timers - done");
                }
                delete pOutdatedTimers;
            }

            TimersDone.ClearOutdated();
            TimersDone.Save();

            if (announceList.Count() > 0) {
                cString msgfmt = cString::sprintf(tr("%d new broadcast(s) found! Show them?"), announceList.Count());
                if (SendMsg(msgfmt, true, 7) == kOk) {
                    m_plugin->showAnnounces = true;
                    cRemote::CallPlugin("epgsearch");
                }
            }

            CheckEPGHours();

            LogFile.iSysLog("search timer update finished");

            // check for conflicts
            if (EPGSearchConfig.checkTimerConflictsAfterUpdate && m_Active && Running()) {
                LogFile.iSysLog("check for timer conflicts");
                cConflictCheck conflictCheck;
                conflictCheck.Check();

                if (conflictCheck.relevantConflicts > 0) {
                    if (EPGSearchConfig.sendMailOnConflicts) {
                        cMailConflictNotifier mailNotifier;
                        mailNotifier.SendConflictNotifications(conflictCheck);
                    }

                    conflictCheck.EvaluateConflCheckCmd();

                    cString msgfmt = "";
                    if (conflictCheck.relevantConflicts == 1)
                        msgfmt = cString::sprintf(tr("timer conflict at %s! Show it?"),
                                                  *DateTime(conflictCheck.nextRelevantConflictDate));
                    else
                        msgfmt = cString::sprintf(tr("%d timer conflicts! First at %s. Show them?"),
                                                  conflictCheck.relevantConflicts,
                                                  *DateTime(conflictCheck.nextRelevantConflictDate));

                    bool doMessage = EPGSearchConfig.noConflMsgWhileReplay == 0 ||
                                     !cDevice::PrimaryDevice()->Replaying() ||
                                     conflictCheck.nextRelevantConflictDate - now < 2 * 60 * 60 ||
                                     (updateForced & UPDS_WITH_OSD);
                    if (doMessage && SendMsg(msgfmt, true, 7, mtWarning) == kOk) {
                        m_plugin->showConflicts = true;
                        cRemote::CallPlugin("epgsearch");
                    }
                }

                LogFile.iSysLog("check for timer conflicts - done");
            }

            // delete expired recordings
            CheckExpiredRecs();

            // check for updates for manual timers
            CheckManualTimers();

            if (m_Active)
                mailNotifier.SendUpdateNotifications();

            if ((updateForced & UPDS_WITH_OSD) && m_Active)
                SendMsg(tr("Search timer update done!"));

            // reset service call flag
            updateForced = 0;

            m_lastUpdate = time(NULL);
            nextUpdate = long(m_lastUpdate / 60) * 60 + (EPGSearchConfig.UpdateIntervall * 60);
            justRunning = false;
        }
        if (m_Active && Running())
            Wait.Wait(2000); // to avoid high system load if time%30==0
        while (Running() && m_Active && !NeedUpdate() && time(NULL) % 30 != 0) // sync heart beat to a multiple of 5secs
            Wait.Wait(1000);
    };
    LogFile.iSysLog("Leaving search timer thread");
}

bool cSearchTimerThread::NeedUpdate()
{
    return (m_lastUpdate <= LastModifiedTime(AddDirectory(CONFIGDIR, ".epgsearchupdate")) || updateForced > 0);
}

char* cSearchTimerThread::SummaryExtended(cSearchExt* searchExt, const cTimer* Timer, const cEvent* pEvent)
{
    bool UseVPS = searchExt->useVPS && pEvent->Vps() && Setup.UseVps;
    time_t eStart;
    if (!UseVPS)
        eStart = pEvent->StartTime();
    else
        eStart = pEvent->Vps();
    time_t eStop;
    if (!UseVPS)
        eStop = pEvent->EndTime();
    else
        eStop = pEvent->Vps() + pEvent->Duration();
    // make sure that eStart and eStop represent a full minute
    eStart = (eStart / 60) * 60;
    eStop = (eStop / 60) * 60;

    time_t start = eStart - (UseVPS ? 0 : (searchExt->MarginStart * 60));
    time_t stop  = eStop + (UseVPS ? 0 : (searchExt->MarginStop * 60));

    char* addSummaryFooter = NULL;
    msprintf(&addSummaryFooter, "<channel>%d - %s</channel><searchtimer>%s</searchtimer><start>%ld</start><stop>%ld</stop><s-id>%d</s-id><eventid>%ld</eventid>",
             Timer->Channel()->Number(), CHANNELNAME(Timer->Channel()),
             searchExt->search,
             start,
             stop,
             searchExt->ID,
             (long) pEvent->EventID());

    const char* aux = Timer->Aux();
    // remove epgsearch entries
    char* tmpaux = NULL;
    if (!isempty(aux)) {
        tmpaux = strdup(aux);
        const char* begin = strstr(aux, "<epgsearch>");
        const char* end = strstr(aux, "</epgsearch>");
        if (begin && end) {
            if (begin == aux) strcpy(tmpaux, "");
            else strn0cpy(tmpaux, aux, begin - aux + 1);
            strcat(tmpaux, end + strlen("</epgsearch>"));
        }
    }

    char* tmpSummary = NULL;
    msprintf(&tmpSummary, "<epgsearch>%s</epgsearch>%s", addSummaryFooter, tmpaux ? tmpaux : "");
    free(addSummaryFooter);
    if (tmpaux) free(tmpaux);
    return tmpSummary;
}

bool cSearchTimerThread::AddModTimer(cTimer* Timer, int index, cSearchExt* searchExt, const cEvent* pEvent, int Prio, int Lifetime, char* Summary, uint timerMod)
{
    char *cmdbuf = NULL;

    static char bufStart[25];
    static char bufEnd[25];

    struct tm tm_r;
    time_t eStart = pEvent->StartTime();
    time_t eStop = pEvent->EndTime();
    time_t start = eStart - (searchExt->MarginStart * 60);
    time_t stop  = eStop + (searchExt->MarginStop * 60);
    int Flags = Timer->Flags();
    if (searchExt->useVPS && pEvent->Vps() && Setup.UseVps) {
        start = pEvent->Vps();
        stop = start + pEvent->Duration();
    } else
        Flags &= ~tfVps; // don't use VPS, if not set in this search

    if (searchExt->action == searchTimerActionInactiveRecord)
        Flags &= ~tfActive;

    // already done the same timer?
    if (!EPGSearchConfig.TimerProgRepeat && index == 0 && TimersDone.InList(start, stop, pEvent, -1)) {
        LogFile.Log(2, "skip timer for '%s~%s' (%s - %s); search timer: '%s' - already done", pEvent->Title(), pEvent->ShortText() ? pEvent->ShortText() : "", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), searchExt->search);
        return false;
    }

    strftime(bufStart, sizeof(bufStart), "%H%M", localtime_r(&start, &tm_r));
    strftime(bufEnd, sizeof(bufEnd), "%H%M", localtime_r(&stop, &tm_r));

    // add additional info
    char* tmpSummary = NULL;
    if (Summary) {
        tmpSummary = strdup(Summary);
        strreplace(tmpSummary, '\n', '|');
    } else
        tmpSummary = SummaryExtended(searchExt, Timer, pEvent);
    if (index == 0)
        msprintf(&cmdbuf, "NEWT %d:%d:%s:%s:%s:%d:%d:%s:%s",
                 Flags,
                 Timer->Channel()->Number(),
                 *Timer->PrintDay(start, Timer->WeekDays(), true),
                 bufStart,
                 bufEnd,
                 Prio,
                 Lifetime,
                 Timer->File(),
                 tmpSummary ? tmpSummary : "");
    else
        msprintf(&cmdbuf, "MODT %d %d:%d:%s:%s:%s:%d:%d:%s:%s",
                 index,
                 Flags,
                 Timer->Channel()->Number(),
                 *Timer->PrintDay(start, Timer->WeekDays(), true),
                 bufStart,
                 bufEnd,
                 Prio,
                 Lifetime,
                 Timer->File(),
                 tmpSummary ? tmpSummary : "");

    if (!SendViaSVDRP(cmdbuf))
        return false;

    if (gl_timerStatusMonitor) gl_timerStatusMonitor->SetConflictCheckAdvised();

    cTimerDone* timerdone = new cTimerDone(start, stop, pEvent, searchExt->ID);
    if (index == 0)
        TimersDone.Add(timerdone);
    else
        TimersDone.Update(start, stop, pEvent, searchExt->ID, timerdone);

    if (EPGSearchConfig.sendMailOnSearchtimers) {
        if (index == 0) // new
            mailNotifier.AddNewTimerNotification(pEvent->EventID(), pEvent->ChannelID());
        else
            mailNotifier.AddModTimerNotification(pEvent->EventID(), pEvent->ChannelID(), timerMod);
    }
    free(cmdbuf);
    if (tmpSummary) free(tmpSummary);

    return true;
}

void cSearchTimerThread::RemoveTimer(const cTimer* t, const cEvent* e)
{
    if (!t) return;
    if (EPGSearchConfig.sendMailOnSearchtimers)
        mailNotifier.AddRemoveTimerNotification(t, e);
    if (!EPGSearchConfig.TimerProgRepeat) {
        cTimerDone * TimerDone = TimersDone.InList(t->StartTime(), t->StopTime(), e, -1);
        if (TimerDone) {
            cMutexLock TimersDoneLock(&TimersDone);
            TimersDone.Del(TimerDone);
            TimersDone.Save();
        }
    }
    DelTimer(t->Id());
}

void cSearchTimerThread::DelRecording(int index)
{
    cString cmdbuf = cString::sprintf("DELR %d", index);
    LogFile.Log(2, "delete recording %d", index);
    SendViaSVDRP(cmdbuf);
}

void cSearchTimerThread::CheckExpiredRecs()
{
    LogFile.Log(1, "check for expired recordings started");
    LOCK_RECORDINGS_WRITE;
    Recordings->SetExplicitModify();
    cList<cRecordingObj> DelRecordings;
    for (cRecording *recording = Recordings->First(); recording && m_Active; recording = Recordings->Next(recording)) {
        LogFile.Log(3, "check recording %s from %s for expiration", recording->Name(), DAYDATETIME(recording->Start()));
        if (recording->Start() == 0) {
            LogFile.Log(2, "oops, recording %s has no start time, skipped", recording->Name());
            continue;
        }
        if (recording->IsEdited()) continue;

        if (!recording->Info()) continue;
        char* searchID = GetAuxValue(recording, "s-id");
        char* searchName = GetAuxValue(recording, "searchtimer");
        if (!searchName)
            searchName = GetAuxValue(recording, "search timer");

        if (!searchID || !searchName) {
            if (searchID) free(searchID);
            if (searchName) free(searchName);
            continue;
        }
        cSearchExt* search = SearchExts.GetSearchFromID(atoi(searchID));
        if (!search || strcmp(search->search, searchName) != 0) {
            if (searchID) free(searchID);
            if (searchName) free(searchName);
            continue;
        }
        free(searchID);
        free(searchName);
        LogFile.Log(3, "recording triggered from search timer %s", search->search);
        if (search->delAfterDays == 0) continue;
        time_t now = time(NULL);

        int daysBetween = int(double((now - recording->Start())) / (60 * 60 * 24));
        if (daysBetween  >= search->delAfterDays)
            DelRecordings.Add(new cRecordingObj(recording, search));
        else
            LogFile.Log(3, "recording will expire in %d days, search timer %s", search->delAfterDays - daysBetween, search->search);
    }
    for (cRecordingObj *recordingObj = DelRecordings.First(); recordingObj && m_Active; recordingObj = DelRecordings.Next(recordingObj)) {
        cRecording* recording = recordingObj->recording;
        cSearchExt* search = recordingObj->search;
        if (search->recordingsKeep > 0 && search->recordingsKeep >= search->GetCountRecordings()) {
            LogFile.Log(1, "recording '%s' from %s expired, but will be kept, search timer %s", recording->Name(), DAYDATETIME(recording->Start()), recordingObj->search->search);
            continue;
        }
        LogFile.Log(1, "delete expired recording '%s' from %s, search timer %s", recording->Name(), DAYDATETIME(recording->Start()), recordingObj->search->search);
        cRecordControl *rc = cRecordControls::GetRecordControl(recording->FileName());
        if (!rc) {
            if (!recording->Delete())
                LogFile.Log(1, "error deleting recording!");
            else {
                Recordings->DelByName(recording->FileName());
                Recordings->SetModified();
            }
        } else
            LogFile.Log(1, "recording already in use by a timer!");
    }
    LogFile.Log(1, "check for expired recordings finished");
}

void cSearchTimerThread::ModifyManualTimer(const cEvent* event, const cTimer* timer, int bstart, int bstop)
{
    LogFile.Log(1, "modified manual timer %d for '%s~%s' (%s - %s)", timer->Id(), event->Title(), event->ShortText() ? event->ShortText() : "", GETDATESTRING(event), GETTIMESTRING(event));

    time_t start = event->StartTime() - bstart;
    time_t stop = event->EndTime() + bstop;
    struct tm tm_r_start;
    struct tm tm_r_stop;
    localtime_r(&start, &tm_r_start);
    localtime_r(&stop, &tm_r_stop);
    char daybuffer[DAYBUFFERSIZE];
    char startbuffer[DAYBUFFERSIZE];
    char stopbuffer[DAYBUFFERSIZE];
    strftime(daybuffer, DAYBUFFERSIZE, "%Y-%m-%d", &tm_r_start);
    strftime(startbuffer, DAYBUFFERSIZE, "%H%M", &tm_r_start);
    strftime(stopbuffer, DAYBUFFERSIZE, "%H%M", &tm_r_stop);

    char* cmdbuf = NULL;
    msprintf(&cmdbuf, "MODT %d %d:%d:%s:%s:%s:%d:%d:%s:%s",
             timer->Id(),
             timer->Flags(),
             timer->Channel()->Number(),
             daybuffer,
             startbuffer,
             stopbuffer,
             timer->Priority(),
             timer->Lifetime(),
             timer->File(),
             timer->Aux());

    if (EPGSearchConfig.sendMailOnSearchtimers)
        mailNotifier.AddModTimerNotification(event->EventID(), event->ChannelID());

    cTimerThread timerThread;
    timerThread.Init(cmdbuf);
    free(cmdbuf);
}

void cSearchTimerThread::CheckManualTimers(void)
{
    LogFile.Log(1, "manual timer check started");

    LOCK_TIMERS_READ;
    LOCK_SCHEDULES_READ;

    for (const cTimer *ti = Timers->First(); ti && m_Active; ti = Timers->Next(ti)) {
        if (TriggeredFromSearchTimerID(ti) != -1) continue; // manual timer?

        if (TimerWasModified(ti)) {
            LogFile.Log(2, "timer for '%s' (%s, channel %s) modified by user - won't be touched", ti->File(), DAYDATETIME(ti->StartTime()), CHANNELNAME(ti->Channel()));
            continue; // don't update timers modified by user
        }

        char* szbstart = GetAuxValue(ti, "bstart");
        int bstart = szbstart ? atoi(szbstart) : 0;
        free(szbstart);
        char* szbstop = GetAuxValue(ti, "bstop");
        int bstop = szbstop ? atoi(szbstop) : 0;
        free(szbstop);

        // how to check?
        char* updateMethod = GetAuxValue(ti, "update");
        if (updateMethod && atoi(updateMethod) == UPD_EVENTID) { // by event ID?
            // get the channels schedule
            const cSchedule* schedule = Schedules->GetSchedule(ti->Channel());
            if (schedule) {
                tEventID eventID = 0;
                char* szEventID = GetAuxValue(ti, "eventid");
                if (szEventID)
                    eventID = atol(szEventID);
                LogFile.Log(3, "checking manual timer %d by event ID %u", ti->Id(), eventID);
                const cEvent* event = schedule->GetEvent(eventID);
                if (event) {
                    if (event->StartTime() - bstart != ti->StartTime() || event->EndTime() + bstop != ti->StopTime())
                        ModifyManualTimer(event, ti, bstart, bstop);
                } else
                    LogFile.Log(1, "ooops - no event found with id %u for manual timer %d", eventID, ti->Id());

                if (szEventID) free(szEventID);
            }
        }
        if (updateMethod && atoi(updateMethod) == UPD_CHDUR) { // by channel and time?
            // get the channels schedule
            const cSchedule* schedule = Schedules->GetSchedule(ti->Channel());
            if (schedule) {
                // collect all events touching the old timer margins
                cSearchResults eventlist;
                for (const cEvent *testevent = schedule->Events()->First(); testevent; testevent = schedule->Events()->Next(testevent)) {
                    if (testevent->StartTime() < ti->StopTime() && testevent->EndTime() > ti->StartTime())
                        eventlist.Add(new cSearchResult(testevent, (const cSearchExt*)NULL));
                }
                LogFile.Log(3, "checking manual timer %d by channel and time, found %d candidates", ti->Id(), eventlist.Count());
                if (eventlist.Count() > 0) {
                    // choose the event with the best match by duration
                    long origlen = (ti->StopTime() - bstop) - (ti->StartTime() + bstart);
                    double maxweight = 0;
                    const cEvent* event = eventlist.First()->event;
                    for (cSearchResult* pResultObj = eventlist.First();  pResultObj; pResultObj = eventlist.Next(pResultObj)) {
                        const cEvent* testevent = pResultObj->event;
                        time_t start = (testevent->StartTime() < ti->StartTime()) ? ti->StartTime() : testevent->StartTime();
                        time_t stop = (testevent->EndTime() > ti->StopTime()) ? ti->StopTime() : testevent->EndTime();
                        double weight = double(stop - start) / double(testevent->EndTime() - testevent->StartTime());
                        LogFile.Log(3, "candidate '%s~%s' (%s - %s) timer match: %f, duration match: %f", testevent->Title(), testevent->ShortText() ? testevent->ShortText() : "", GETDATESTRING(testevent), GETTIMESTRING(testevent), weight, (double(testevent->EndTime() - testevent->StartTime()) / origlen));
                        if (weight > maxweight && (double(testevent->EndTime() - testevent->StartTime()) / origlen) >= 0.9) {
                            maxweight = weight;
                            event = testevent;
                        }
                    }
                    LogFile.Log(3, "selected candidate is '%s~%s' (%s - %s)", event->Title(), event->ShortText() ? event->ShortText() : "", GETDATESTRING(event), GETTIMESTRING(event));
                    if ((maxweight > 0 && event->StartTime() - bstart != ti->StartTime()) || (event->EndTime() + bstop != ti->StopTime()))
                        ModifyManualTimer(event, ti, bstart, bstop);
                    else if (maxweight <= 0)
                        LogFile.Log(3, "selected candidate is too bad");
                } else
                    LogFile.Log(1, "ooops - no events found touching manual timer %d", ti->Id());
            }
            if (updateMethod) free(updateMethod);
        }
    }
    LogFile.Log(1, "manual timer check finished");
}

// check if EPG is present for the configured hours
void cSearchTimerThread::CheckEPGHours()
{
    if (EPGSearchConfig.checkEPGHours <= 0 ||
        (EPGSearchConfig.checkEPGWarnByOSD == 0 && EPGSearchConfig.checkEPGWarnByMail == 0) ||
        EPGSearchConfig.checkEPGchannelGroupNr <= 0)
        return;

    LogFile.Log(2, "check if relevant EPG exists for the next %d hours", EPGSearchConfig.checkEPGHours);
    cChannelGroup* channelGroup = ChannelGroups.Get(EPGSearchConfig.checkEPGchannelGroupNr - 1); // not zero-based!
    if (!channelGroup) {
        LogFile.Log(1, "channel group with index %d does not exist!", EPGSearchConfig.checkEPGchannelGroupNr);
        return;
    }

    LogFile.Log(2, "checking channel group '%s'", channelGroup->name);

    time_t checkTime = time(NULL) + EPGSearchConfig.checkEPGHours * 60 * 60;

    LOCK_SCHEDULES_READ;

    cChannelGroup channelsWithSmallEPG;
    cChannelGroupItem* channelInGroup = channelGroup->channels.First();
    while (channelInGroup) {
        const cChannel* channel = channelInGroup->channel;
        // get the channels schedule
        const cSchedule* schedule = Schedules->GetSchedule(channel);
        if (!schedule || !schedule->GetEventAround(checkTime)) {
            LogFile.Log(3, "less than %d hours of EPG for channel %s!", EPGSearchConfig.checkEPGHours, channel->Name());
            cChannelGroupItem* channelitem = new cChannelGroupItem(channel);
            channelsWithSmallEPG.channels.Add(channelitem);
        }
        channelInGroup = channelGroup->channels.Next(channelInGroup);
    }

    // create a string list of the channels found
    if (channelsWithSmallEPG.channels.Count() > 0) {
        string sBuffer;
        channelInGroup = channelsWithSmallEPG.channels.First();
        while (channelInGroup) {
            const cChannel* channel = channelInGroup->channel;
            if (channel)
                sBuffer += " " + string(channel->ShortName(true));
            channelInGroup = channelsWithSmallEPG.channels.Next(channelInGroup);
        }


        if (EPGSearchConfig.checkEPGWarnByOSD) {
            cString msgfmt = cString::sprintf(tr("small EPG content on:%s"), sBuffer.c_str());
            SendMsg(msgfmt, false, 0, mtWarning);
        }
        if (EPGSearchConfig.checkEPGWarnByMail) {
            cString msgfmt = cString::sprintf(tr("small EPG content on:%s"), sBuffer.c_str());
            cMailNotifier M(string(tr("VDR EPG check warning")), string(msgfmt));
        }
    }

    LogFile.Log(2, "check for relevant EPG finished - %d channels with small EPG content", channelsWithSmallEPG.channels.Count());
}
