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

#include <string>
#include <list>
#ifdef __FreeBSD__
#include <netinet/in.h>
#endif
#include "timer_thread.h"
#include "epgsearchcfg.h"
#include "epgsearchtools.h"
#include "services.h"
#include "svdrpclient.h"
#include "timerstatus.h"

#include <vdr/tools.h>
#include <vdr/plugin.h>

cTimerThread *cTimerThread::m_Instance = NULL;
TimerThreadStatus cTimerThread::m_Status = TimerThreadReady;
int gl_TimerProgged = 0; // Flag that indicates, when programming is finished

cTimerThread::cTimerThread()
    : cThread("EPGSearch: timer")
{
    m_Active = false;
}

cTimerThread::~cTimerThread()
{
    if (m_Active)
        Stop();
    cTimerThread::m_Instance = NULL;
}

void cTimerThread::Init(cString cmd)
{
    if (m_Instance == NULL) {
        m_Instance = new cTimerThread;
        m_Instance->m_cmd = cmd;
        m_Instance->Start();
    }
}

void cTimerThread::Exit(void)
{
    if (m_Instance != NULL) {
        m_Instance->Stop();
        DELETENULL(m_Instance);
    }

}

void cTimerThread::Stop(void)
{
    m_Active = false;
    Cancel(3);
}

void cTimerThread::Action(void)
{
    m_Active = true;
    if (EPGSearchConfig.useExternalSVDRP && !epgsSVDRP::cSVDRPClient::SVDRPSendCmd) {
        LogFile.eSysLog("ERROR - SVDRPSend script not specified or does not exist (use -f option)");
        m_Active = false;
        return;
    }
    while (m_Active) {
        if (!Running()) {
            m_Active = false;
            break;
        }
        {
            LOCK_TIMERS_WRITE;
            Timers->SetExplicitModify();
        }
        bool bSuccess = SendViaSVDRP(m_cmd);
        if (!bSuccess) {
            Epgsearch_osdmessage_v1_0* service_data = new Epgsearch_osdmessage_v1_0;
            service_data->message = strdup(tr("Programming timer failed!"));
            service_data->type = mtError;
            cPluginManager::CallFirstService("Epgsearch-osdmessage-v1.0", service_data);
            delete service_data;
        } else {
            gl_TimerProgged = 1;
            if (gl_timerStatusMonitor) gl_timerStatusMonitor->SetConflictCheckAdvised();
        }
        m_Active = false;
    };
}


