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

#ifndef __EPGSEARCH_MENUMYEDITTIMER_H
#define __EPGSEARCH_MENUMYEDITTIMER_H

#include <vdr/menuitems.h>
#include "timer_thread.h"
#include <vdr/svdrp.h>

// --- cMenuMyEditTimer --------------------------------------------------------
class cMenuMyEditTimer : public cOsdMenu
{
private:
    cTimer *timer;
    cTimer newtimer;
    const cEvent* event;
    int channel;
    bool addIfConfirmed;
    int UserDefDaysOfWeek;
    cMenuEditStrItem* m_DirItem;

    uint flags;
    time_t day;
    int weekdays;
    int start;
    int stop;
    int priority;
    int lifetime;
    char file[MaxFileName];
    char directory[MaxFileName];
#ifdef USE_PINPLUGIN
    int fskProtection;
#endif
    cStringList svdrpServerNames;
    char remote[HOST_NAME_MAX];
    int checkmode;
public:
    cMenuMyEditTimer(cTimer *Timer, bool New, const cEvent* event, const cChannel* forcechannel = NULL);
    virtual ~cMenuMyEditTimer();
    virtual eOSState ProcessKey(eKeys Key);
    void HandleSubtitle();
    void Set();
    void ReplaceDirVars();
    bool IsSingleEvent(void) const;
    void SplitFile();
    eOSState DeleteTimer();

    static const char* CheckModes[3];
};
#endif
