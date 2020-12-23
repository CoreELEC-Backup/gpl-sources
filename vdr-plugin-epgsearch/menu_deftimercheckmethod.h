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

#ifndef __EPGSEARCHDEFTIMERCHECKMETHOD_H
#define __EPGSEARCHDEFTIMERCHECKMETHOD_H

#include <vdr/menuitems.h>
#include <vdr/plugin.h>

#define UPD_CHDUR 1
#define UPD_EVENTID 2

class cDefTimerCheckMode : public cListObject
{
public:
    tChannelID channelID;
    int mode;

    cDefTimerCheckMode() : mode(0) {}
    cDefTimerCheckMode(tChannelID ChannelID, int Mode) : channelID(ChannelID), mode(Mode) {}
    bool Parse(const char *s);
    cString ToText(void) const;
    bool Save(FILE *f);
};

class cDefTimerCheckModes : public cConfig<cDefTimerCheckMode>
{
public:
    int GetMode(const cChannel* channel);
    void SetMode(const cChannel* channel, int mode);
};

extern cDefTimerCheckModes DefTimerCheckModes;

// --- cMenuDefTimerCheckMethod ---------------------------------------------------------

class cMenuDefTimerCheckMethod : public cOsdMenu
{
    int* modes;
public:
    cMenuDefTimerCheckMethod();
    ~cMenuDefTimerCheckMethod();
    void Set();
    eOSState ProcessKey(eKeys Key);

    static const char* CheckModes[3];
};

#endif
