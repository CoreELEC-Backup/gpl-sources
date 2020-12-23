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

#include "timerstatus.h"

cTimerStatusMonitor* gl_timerStatusMonitor = NULL;

cTimerStatusMonitor::cTimerStatusMonitor()
{
    conflictCheckAdvised = true;
}

void cTimerStatusMonitor::TimerChange(const cTimer *Timer, eTimerChange Change)
{
    // vdr-1.5.15 and above will inform us, when there are any timer changes.
    // so timer changes (within epgsearch) in previous versions have to be tracked
    // at the correspondig places.
    conflictCheckAdvised = true;
}

void cTimerStatusMonitor::SetConflictCheckAdvised(bool ConflictCheckAdvised)
{
    if (!ConflictCheckAdvised)
        conflictCheckAdvised = false;
}

bool cTimerStatusMonitor::ConflictCheckAdvised()
{
    return conflictCheckAdvised;
}

