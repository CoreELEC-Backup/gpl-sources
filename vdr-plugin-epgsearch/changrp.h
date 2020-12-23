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

#ifndef __EPGSEARCHCHANGRP_H
#define __EPGSEARCHCHANGRP_H

#include <vdr/status.h>
#include <vdr/plugin.h>
#include <vdr/tools.h>

class cSearchExt;

// --- cChannelGroupItem --------------------------------------------------------
class cChannelGroupItem : public cListObject
{
public:
    const cChannel* channel;
public:
    cChannelGroupItem(const cChannel* ch) : channel(ch) {}
};

// --- cChannelGroup --------------------------------------------------------
class cChannelGroup : public cListObject
{
public:
    char name[MaxFileName];
    cList<cChannelGroupItem> channels;
public:
    cChannelGroup(void);
    virtual ~cChannelGroup(void);

    bool Parse(const char *s);
    const char *ToText(void);
    bool Save(FILE *f);
    int* CreateChannelSel();
    void CreateChannelList(int*);
    bool ChannelInGroup(const cChannel*);
};

// --- cChannelGroups --------------------------------------------------------
class cChannelGroups : public cConfig<cChannelGroup>
{
private:
public:
    cChannelGroups(void) {}
    ~cChannelGroups(void) {}
    int GetIndex(char* channelGroup);
    cChannelGroup* GetGroupByName(const char* channelGroup);
    cSearchExt* Used(cChannelGroup*);
    char** CreateMenuitemsList();
};

extern cChannelGroups ChannelGroups;

// --- cMenuChannelGroupItem ----------------------------------------------------------
class cMenuChannelGroupItem : public cOsdItem
{
private:
public:
    cChannelGroup* group;
    cMenuChannelGroupItem(cChannelGroup*);
    void Set(void);
};

// --- cMenuChannelGroups --------------------------------------------------------
class cMenuChannelGroups : public cOsdMenu
{
private:
    cChannelGroup *CurrentGroup(void);
    eOSState New(void);
    eOSState Delete(void);
    int groupSel;
    char** groupName;

protected:
    virtual eOSState ProcessKey(eKeys Key);
public:
    cMenuChannelGroups(char** groupName = NULL);
};

// --- cMenuEditChannelGroup --------------------------------------------------------
class cMenuEditChannelGroup : public cOsdMenu
{
private:
    cChannelGroup *group;
    bool addIfConfirmed;
    char name[MaxFileName];
    int* channelSel;
public:
    cMenuEditChannelGroup(cChannelGroup *group, bool New = false);
    ~cMenuEditChannelGroup();
    void Set();
    virtual eOSState ProcessKey(eKeys Key);
};

#endif
