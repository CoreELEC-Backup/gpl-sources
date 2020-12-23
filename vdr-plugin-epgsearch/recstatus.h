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

#ifndef __RECSTATUS_H
#define __RECSTATUS_H

#include <vdr/status.h>
#include "recdone.h"
#include "epgsearchtools.h"

// --- cRecDoneTimerObj --------------------------------------------------------
class cRecDoneTimerObj : public cTimerObj
{
public:
    int deviceNr;
    cRecDone* recDone;
    time_t lastBreak;
public:
    cRecDoneTimerObj(const cTimer* Timer, int DeviceNr) : cTimerObj(Timer), deviceNr(DeviceNr), recDone(NULL), lastBreak(0) {}
    ~cRecDoneTimerObj() {
        timer = NULL;    // do not delete anything!
        recDone = NULL;
    }
};

class cTimersRecording: public cList<cRecDoneTimerObj>, public cMutex
{
};

extern cTimersRecording TimersRecording;

class cRecStatusMonitor : public cStatus
{
protected:
    virtual void Recording(const cDevice *Device, const char *Name, const char*, bool On);
public:
    cRecStatusMonitor();
    int TimerRecDevice(const cTimer*);
};

extern cRecStatusMonitor* gl_recStatusMonitor;

#endif
