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

#ifndef __MENU_BLACKLISTEDIT_H
#define __MENU_BLACKLISTEDIT_H

#include "blacklist.h"

// --- cMenuBlacklistEdit --------------------------------------------------------
class cMenuBlacklistEdit: public cOsdMenu
{
protected:
    cBlacklist *blacklist;
    cBlacklist data;
    int channelMin;
    int channelMax;
    bool addIfConfirmed;
    int UserDefDayOfWeek;
    int channelGroupNr;
    char* channelGroupName;
    char** menuitemsChGr;
    int* catvaluesNumeric;

    char *SearchModes[6];
    char *DaysOfWeek[8];
    char *UseChannelSel[4];

public:
    cMenuBlacklistEdit(cBlacklist *Blacklist, bool New = false);
    virtual eOSState ProcessKey(eKeys Key);
    virtual ~cMenuBlacklistEdit();
    virtual void Set();
    void CreateMenuitemsChannelGroups();
};

#endif
