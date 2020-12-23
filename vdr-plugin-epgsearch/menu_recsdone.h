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

#ifndef __MENU_RECSDONE_H
#define __MENU_RECSDONE_H

#include "epgsearchext.h"
#include "recdone.h"
#include <vdr/menu.h>
#include <vdr/menuitems.h>

// --- cMenuRecDoneItem ----------------------------------------------------------
class cMenuRecDoneItem : public cOsdItem
{
public:
    cRecDone* recDone;
    bool showEpisodeOnly;
    cMenuRecDoneItem(cRecDone* RecDone, bool ShowEpisodeOnly = false);
    void Set();
    int Compare(const cListObject &ListObject) const;
};

// --- cMenuRecDone ----------------------------------------------------------
class cMenuRecsDone : public cOsdMenu
{
private:
    cSearchExt* search;
    eOSState Delete(void);
    eOSState DeleteAll(void);
    const char* ButtonBlue(cSearchExt* Search);
    int showMode;
    bool showEpisodeOnly;
protected:
    void Set();
    virtual eOSState ProcessKey(eKeys Key);
    void UpdateTitle();
    eOSState Summary(void);
    cRecDone* CurrentRecDone(void);
public:
    cMenuRecsDone(cSearchExt* search = NULL);
};

// --- cMenuTextDone ----------------------------------------------------------
class cMenuTextDone : public cMenuText
{
    cRecDone* recDone;
public:
    cMenuTextDone(const char *Title, cRecDone* RecDone, eDvbFont Font = fontOsd);
    virtual eOSState ProcessKey(eKeys Key);
};

#endif
