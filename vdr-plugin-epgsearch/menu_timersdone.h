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

#ifndef __MENU_TIMERSDONE_H
#define __MENU_TIMERSDONE_H

#include "epgsearchext.h"
#include "timerdone.h"
#include "menu_event.h"
#include <vdr/menu.h>
#include <vdr/menuitems.h>


// --- cMenuTimerDoneItem ----------------------------------------------------------
class cMenuTimerDoneItem : public cOsdItem
{
public:
    cTimerDone* timerDone;
    cMenuTimerDoneItem(cTimerDone* TimerDone);
    void Set(void);
    int Compare(const cListObject &ListObject) const;
};


class cMenuTimersDone : public cOsdMenu
{
private:
    cSearchExt* search;
    eOSState Delete(void);
    eOSState DeleteAll(void);
    bool showAll;
    cEventObjects eventObjects;
protected:
    void Set();
    virtual eOSState ProcessKey(eKeys Key);
    void UpdateTitle();
    eOSState Summary(void);
    cTimerDone* CurrentTimerDone(void);
    void UpdateCurrent();
public:
    cMenuTimersDone(cSearchExt* search = NULL);
};

#endif
