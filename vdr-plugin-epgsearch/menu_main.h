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

#ifndef __EPGSEARCH_MENUMAIN_H
#define __EPGSEARCH_MENUMAIN_H

#include <vdr/menuitems.h>
#include "menu_event.h"

// --- cMenuSearchMain ---------------------------------------------------------

class cMenuSearchMain : public cOsdMenu
{
private:
    int helpKeys;
    int otherChannel;
    int currentChannel;
    eOSState Record(void);
    eOSState ExtendedSearch(void);
    void PrepareSchedule(const cChannel *Channel);
    eOSState Commands(eKeys Key);
    void SetHelpKeys(bool Force = false);
    int GetTab(int Tab);
    int shiftTime;
    bool InWhatsOnMenu;
    bool InFavoritesMenu;
    cEventObjects eventObjects;
public:
    cMenuSearchMain(void);
    virtual ~cMenuSearchMain();
    virtual eOSState ProcessKey(eKeys Key);
    eOSState Switch(void);
    eOSState Shift(int iMinutes);
    eOSState ShowSummary();
    bool Update(void);
    void UpdateCurrent();
#ifdef USE_GRAPHTFT
    virtual const char* MenuKind() {
        return "MenuEpgsSchedule";
    }
    virtual void Display(void);
#endif

    static int forceMenu;
};

#endif
