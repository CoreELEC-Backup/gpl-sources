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

#ifndef __EPGSEARCHMENUCONFL_H
#define __EPGSEARCHMENUCONFL_H

#include <vdr/menuitems.h>
#include "conflictcheck.h"
#include "menu_event.h"

// --- cMenuConflictCheckItem ------------------------------------------------------
class cMenuConflictCheckItem : public cOsdItem
{
public:
    cConflictCheckTime* checktime;
    cConflictCheckTimerObj* timerObj;
    cMenuConflictCheckItem(cConflictCheckTime* Ct, cConflictCheckTimerObj* TimerObj = NULL);
};

// --- cMenuConflictCheck ------------------------------------------------------
class cMenuConflictCheck : public cOsdMenu
{
private:
    cConflictCheck conflictCheck;
    bool showAll;
    int lastSel;
    virtual eOSState ProcessKey(eKeys Key);
    cConflictCheckTimerObj* CurrentTimerObj(void);
    void Update();
    bool BuildList();
public:
    cMenuConflictCheck();
};

// --- cMenuConflictCheckDetailsItem ------------------------------------------------------
class cMenuConflictCheckDetailsItem : public cOsdItem
{
    bool hasTimer;
public:
    cConflictCheckTimerObj* timerObj;
    cMenuConflictCheckDetailsItem(cConflictCheckTimerObj* TimerObj = NULL);
    bool Update(bool Force = false);
};

// --- cMenuConflictCheckDetails ------------------------------------------------------
class cMenuConflictCheckDetails : public cOsdMenu
{
private:
    cConflictCheck* conflictCheck;
    cConflictCheckTimerObj* timerObj;
    cConflictCheckTime* checktime;
    cEventObjects eventObjects;

    virtual eOSState ProcessKey(eKeys Key);
    cConflictCheckTimerObj* CurrentTimerObj(void);
    eOSState Commands(eKeys Key);
    void SetHelpKeys();
    eOSState ToggleTimer(cConflictCheckTimerObj* TimerObj);
    eOSState DeleteTimer(cConflictCheckTimerObj* TimerObj);
    bool Update(bool Force = false);
    bool BuildList();
    eOSState ShowSummary();
    void UpdateCurrent();
public:
    cMenuConflictCheckDetails(cConflictCheckTimerObj* TimerObj = NULL, cConflictCheck* ConflictCheck = NULL);
};

#endif
