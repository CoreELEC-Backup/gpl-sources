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

#ifndef VDR_TIMER_THREAD_H
#define VDR_TIMER_THREAD_H

#include <vdr/thread.h>
#include "epgsearchext.h"
#include "log.h"

extern int gl_TimerProgged;

typedef enum {
    TimerThreadReady,
    TimerThreadWorking,
    TimerThreadError,
    TimerThreadDone
} TimerThreadStatus;

class cTimerThread: public cThread
{
private:
    static cTimerThread *m_Instance;
    cString m_cmd;
    static TimerThreadStatus m_Status;
protected:
    virtual void Action(void);
    void Stop(void);
public:
    bool m_Active;
    TimerThreadStatus GetStatus() {
        return cTimerThread::m_Status;
    }
    void SetStatus(TimerThreadStatus Status) {
        LogFile.eSysLog("%d", int(Status));
        cTimerThread::m_Status = Status;
    }
    cTimerThread();
    virtual ~cTimerThread();
    void Init(cString);
    void Exit(void);
};

#endif
