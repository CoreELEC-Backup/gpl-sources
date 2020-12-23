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
#include "conflictcheck_thread.h"
#include "epgsearchcfg.h"
#include "mail.h"
#include "epgsearch.h"

#define CONFLICTCHECK_NICE 19

cConflictCheckThread *cConflictCheckThread::m_Instance = NULL;
time_t cConflictCheckThread::m_cacheNextConflict = 0;
int cConflictCheckThread::m_cacheRelevantConflicts = 0;
int cConflictCheckThread::m_cacheTotalConflicts = 0;
bool cConflictCheckThread::m_runOnce = false;
bool cConflictCheckThread::m_forceUpdate = false;

cConflictCheckThread::cConflictCheckThread(cPluginEpgsearch* thePlugin)
    : cThread("EPGSearch: conflictcheck")
{
    m_plugin = thePlugin;
    m_Active = false;
    m_lastUpdate = time(NULL);
    m_runOnce = false;
    m_forceUpdate = false;
}

cConflictCheckThread::~cConflictCheckThread()
{
    if (m_Active)
        Stop();
}

void cConflictCheckThread::Init(cPluginEpgsearch* thePlugin, bool runOnce)
{
    if (EPGSearchConfig.checkTimerConflictsAfterUpdate || EPGSearchConfig.conflictCheckIntervall == 0) {
        if (!runOnce) return;
        m_runOnce = true;
    }

    if (m_Instance == NULL) {
        m_Instance = new cConflictCheckThread(thePlugin);
        m_Instance->Start();
    } else if (runOnce) m_forceUpdate = true; // force an update, because thread is already running

}

void cConflictCheckThread::Exit(void)
{
    if (m_Instance != NULL) {
        m_Instance->Stop();
        DELETENULL(m_Instance);
    }
}

void cConflictCheckThread::Stop(void)
{
    m_Active = false;
    Wait.Signal();
    Cancel(6);
}

void cConflictCheckThread::Action(void)
{
    SetPriority(CONFLICTCHECK_NICE);

    m_Active = true;
    // let VDR do its startup
    if (!m_runOnce) {
        if (!cPluginEpgsearch::VDR_readyafterStartup)
            LogFile.Log(2, "ConflictCheckThread: waiting for VDR to become ready...");
        while (m_Active && !cPluginEpgsearch::VDR_readyafterStartup)
            Wait.Wait(1000);
    }

    time_t nextUpdate = time(NULL);
    while (m_Active && Running()) {
        time_t now = time(NULL);
        if (now >= nextUpdate || m_forceUpdate) {
            m_forceUpdate = false;
            {
                LOCK_TIMERS_WRITE;
                Timers->SetExplicitModify();
            }
            LogFile.iSysLog("timer conflict check started");

            cConflictCheck conflictCheck;
            conflictCheck.Check();

            time_t nextConflict = 0;
            if (conflictCheck.relevantConflicts > 0) {
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
                                 conflictCheck.nextRelevantConflictDate - now < 2 * 60 * 60;
                if (doMessage && SendMsg(msgfmt, true, 7, mtWarning) == kOk) {
                    m_plugin->showConflicts = true;
                    cRemote::CallPlugin("epgsearch");
                }

                if (EPGSearchConfig.sendMailOnConflicts) {
                    cMailConflictNotifier mailNotifier;
                    mailNotifier.SendConflictNotifications(conflictCheck);
                }
                conflictCheck.EvaluateConflCheckCmd();
            }
            // store for external access
            cConflictCheckThread::m_cacheNextConflict = conflictCheck.nextRelevantConflictDate;
            cConflictCheckThread::m_cacheRelevantConflicts = conflictCheck.relevantConflicts;
            cConflictCheckThread::m_cacheTotalConflicts = conflictCheck.numConflicts;

            LogFile.iSysLog("timer conflict check finished");

            m_lastUpdate = time(NULL);
            int Intervall = EPGSearchConfig.conflictCheckIntervall;
            if (nextConflict > 0 && EPGSearchConfig.conflictCheckWithinLimit > 0 &&
                nextConflict - time(NULL) < EPGSearchConfig.conflictCheckWithinLimit * 60)
                Intervall = EPGSearchConfig.conflictCheckIntervall2;

            nextUpdate = long(m_lastUpdate / 60) * 60 + (Intervall * 60);
        }
        if (m_Active && Running())
            Wait.Wait(2000); // to avoid high system load if time%30==0 ?????????????????????
        // no waiting in the while loop if m_runOnce is true
        while (Running() && m_Active && time(NULL) % 30 != 0 && !m_runOnce) // sync heart beat to a multiple of 5secs
            Wait.Wait(1000);
    };

    m_Active = false;   // always false at this point
    LogFile.iSysLog("Leaving conflict check thread");
}

