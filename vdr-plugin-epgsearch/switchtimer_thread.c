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

#include "switchtimer_thread.h"
#include "epgsearchcfg.h"
#include "epgsearchtools.h"
#include "epgsearch.h"
#include <vdr/tools.h>
#include <vdr/plugin.h>

#define MSG_DELAY 7

cSwitchTimerThread *cSwitchTimerThread::m_Instance = NULL;

cSwitchTimerThread::cSwitchTimerThread(void)
    : cThread("EPGSearch: switchtimer")
{
    m_Active = false;
    m_lastUpdate = time(NULL);
}

cSwitchTimerThread::~cSwitchTimerThread()
{
    if (m_Active)
        Stop();
}

void cSwitchTimerThread::Init(void)
{
    if (m_Instance == NULL && SwitchTimers.Count() > 0) {
        m_Instance = new cSwitchTimerThread;
        m_Instance->Start();
    }
}

void cSwitchTimerThread::Exit(void)
{
    if (m_Instance != NULL) {
        m_Instance->Stop();
        DELETENULL(m_Instance);
    }
}

void cSwitchTimerThread::Stop(void)
{
    m_Active = false;
    Wait.Signal();
    Cancel(6);
}

void cSwitchTimerThread::Action(void)
{
    m_Active = true;

    // let VDR do its startup
    if (!cPluginEpgsearch::VDR_readyafterStartup)
        LogFile.Log(2, "SwitchTimerThread: waiting for VDR to become ready...");
    while (m_Active && !cPluginEpgsearch::VDR_readyafterStartup)
        Wait.Wait(1000);

    time_t nextUpdate = time(NULL);
    while (m_Active) {
        time_t now = time(NULL);
        if (now >= nextUpdate) {
            LogFile.Log(3, "locking switch timers");
            SwitchTimers.cMutex::Lock();
            LogFile.Log(3, "switch timer check started");
            cSwitchTimer* switchTimer = SwitchTimers.First();
            while (switchTimer && m_Active && Running()) {
                if (switchTimer->startTime - now < switchTimer->switchMinsBefore * 60 + MSG_DELAY + 1) {
                    LOCK_CHANNELS_READ;
                    const cChannel *channel = Channels->GetByChannelID(switchTimer->channelID, true, true);
                    bool doSwitch = (switchTimer->mode == 0);
                    bool doAsk = (switchTimer->mode == 2);
                    bool doUnmute = switchTimer->unmute;
                    SwitchTimers.Del(switchTimer);

                    const cEvent* event = switchTimer->Event();
                    if (event && channel && (event->EndTime() >= now)) {
                        cString Message = cString::sprintf("%s: %s - %s", event->Title(),
                                                           CHANNELNAME(channel), GETTIMESTRING(event));
                        cString SwitchCmd = cString::sprintf("CHAN %d", channel->Number());
                        // switch
                        if (doSwitch) {
                            LogFile.Log(1, "switching to channel %d", channel->Number());
                            if (cDevice::CurrentChannel() != channel->Number())
                                SendViaSVDRP(SwitchCmd);

                            if (doUnmute && cDevice::PrimaryDevice()->IsMute())
                                cDevice::PrimaryDevice()->ToggleMute();
                        }
                        if (!doAsk)
                            SendMsg(Message);

                        if (doAsk) {
                            cString Message = cString::sprintf(tr("Switch to (%d) '%s'?"), channel->Number(), event->Title());
                            if (SendMsg(Message, true, MSG_DELAY) == kOk) {
                                LogFile.Log(1, "switching to channel %d", channel->Number());
                                if (cDevice::CurrentChannel() != channel->Number())
                                    SendViaSVDRP(SwitchCmd);

                                if (doUnmute && cDevice::PrimaryDevice()->IsMute())
                                    cDevice::PrimaryDevice()->ToggleMute();

                            }
                        }

                        if (m_Active && Running())
                            Wait.Wait(1000 * MSG_DELAY);
                    }
                    SwitchTimers.Save();
                    break;
                }
                switchTimer = SwitchTimers.Next(switchTimer);
            }
            SwitchTimers.Unlock();
            LogFile.Log(3, "switch timer check finished");
            if (m_Active && Running())
                Wait.Wait(1000 * MSG_DELAY);
            m_lastUpdate = time(NULL);
            nextUpdate = long(m_lastUpdate / 60) * 60 + 60 - MSG_DELAY ; // check at each full minute minus 5sec
            if (SwitchTimers.Count() == 0)
                m_Active = false;
            if (!Running())
                m_Active = false;
        }
        while (m_Active && time(NULL) % MSG_DELAY != 0) // sync heart beat to MSG_DELAY
            Wait.Wait(1000);
        Wait.Wait(1000);
    };
    m_Instance = NULL;
}

