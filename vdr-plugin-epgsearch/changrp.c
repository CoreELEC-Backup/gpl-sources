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

#include "changrp.h"
#include "epgsearchtools.h"
#include "epgsearchcfg.h"
#include "epgsearchext.h"
#include <vdr/interface.h>

// -- cChannelGroup -----------------------------------------------------------------
cChannelGroup::cChannelGroup(void)
{
    strcpy(name, "");
    channels.Clear();
}

cChannelGroup::~cChannelGroup(void)
{
    channels.Clear();
}

bool cChannelGroup::Parse(const char *s)
{
    char *line;
    char *pos;
    char *pos_next;
    int parameter = 1;
    int valuelen;
#define MAXVALUELEN (10 * MaxFileName)

    char value[MAXVALUELEN];

    pos = line = strdup(s);
    pos_next = pos + strlen(pos);
    if (*pos_next == '\n') *pos_next = 0;
    while (*pos) {
        while (*pos == ' ') pos++;
        if (*pos) {
            if (*pos != '|') {
                pos_next = strchr(pos, '|');
                if (!pos_next)
                    pos_next = pos + strlen(pos);
                valuelen = pos_next - pos + 1;
                if (valuelen > MAXVALUELEN) {
                    LogFile.eSysLog("entry '%s' is too long. Will be truncated!", pos);
                    valuelen = MAXVALUELEN;
                }
                strn0cpy(value, pos, valuelen);
                pos = pos_next;
                switch (parameter) {
                case 1:
                    strcpy(name, value);
                    break;
                default: {
#ifdef __FreeBSD__
                    char *channelbuffer = MALLOC(char, 32);
                    int numChannels = sscanf(value, "%31[^|]", channelbuffer);
#else
                    char *channelbuffer = NULL;
                    int numChannels = sscanf(value, "%m[^|]", &channelbuffer);
#endif
                    if (numChannels == 1) {
                        LOCK_CHANNELS_READ;
                        const cChannel* channel = Channels->GetByChannelID(tChannelID::FromString(channelbuffer), true, true);
                        if (channel) {
                            cChannelGroupItem* channelitem = new cChannelGroupItem(channel);
                            channels.Add(channelitem);
                        }
                    }
                    free(channelbuffer);
                }
                break;
                } //switch
            }
            parameter++;
        }
        if (*pos) pos++;
    } //while

    free(line);
    return (parameter >= 1) ? true : false;
}

const char *cChannelGroup::ToText(void)
{
    char* channelbuffer = NULL;
    cChannelGroupItem* ChannelGroupItem = channels.First();
    int index = 0;
    while (ChannelGroupItem) {
        const cChannel* channel = ChannelGroupItem->channel;
        if (index++ == 0)
            channelbuffer = strdup(CHANNELSTRING(channel));
        else {
            char* temp = channelbuffer;
            msprintf(&channelbuffer, "%s|%s", channelbuffer, CHANNELSTRING(channel));
            free(temp);
        }
        ChannelGroupItem = channels.Next(ChannelGroupItem);
    }
    char* buffer = NULL;
    msprintf(&buffer, "%s|%s", name, channelbuffer);
    free(channelbuffer);
    return buffer;
}

int* cChannelGroup::CreateChannelSel()
{
    LOCK_CHANNELS_READ;
    int* channelSel = (int*) malloc(Channels->Count() * sizeof(int));
    const cChannel* channel = Channels->First();
    int index = 0;
    while (channel) {
        if (channel->GroupSep()) {
            channel = Channels->Next(channel);
            continue;
        }
        channelSel[index] = 0;
        cChannelGroupItem* channelInGroup = channels.First();
        while (channelInGroup) {
            if (channel == channelInGroup->channel) {
                channelSel[index] = 1;
                break;
            }
            channelInGroup = channels.Next(channelInGroup);
        }
        index++;
        channel = Channels->Next(channel);
    }
    return channelSel;
}

void cChannelGroup::CreateChannelList(int* channelSel)
{
    channels.Clear();
    LOCK_CHANNELS_READ;
    const cChannel* channel = Channels->First();
    int index = 0;
    while (channel) {
        if (!channel->GroupSep()) {
            if (channelSel[index] == 1)
                channels.Add(new cChannelGroupItem(channel));
            index++;
        }
        channel = Channels->Next(channel);
    }
}

bool cChannelGroup::Save(FILE *f)
{
    return fprintf(f, "%s\n", ToText()) > 0;
}

bool cChannelGroup::ChannelInGroup(const cChannel* channel)
{
    cChannelGroupItem* channelInGroup = channels.First();
    while (channelInGroup) {
        if (channel == channelInGroup->channel)
            return true;
        channelInGroup = channels.Next(channelInGroup);
    }
    return false;
}

// -- cChannelGroups -----------------------------------------------------------------
int cChannelGroups::GetIndex(char* channelGroup)
{
    if (!channelGroup)
        return -1;
    cChannelGroup* ChannelGroup = First();
    int index = 0;
    while (ChannelGroup) {
        if (strcmp(channelGroup, ChannelGroup->name) == 0)
            return index;
        index++;
        ChannelGroup = Next(ChannelGroup);
    }
    return -1;
}

cChannelGroup* cChannelGroups::GetGroupByName(const char* channelGroup)
{
    if (!channelGroup)
        return NULL;
    cChannelGroup* ChannelGroup = First();
    while (ChannelGroup) {
        if (strcmp(channelGroup, ChannelGroup->name) == 0)
            return ChannelGroup;
        ChannelGroup = Next(ChannelGroup);
    }
    return NULL;
}

cSearchExt* cChannelGroups::Used(cChannelGroup* group)
{
    if (!group)
        return NULL;

    if (SearchExts.Count() == 0)
        SearchExts.Load(AddDirectory(CONFIGDIR, "epgsearch.conf"));

    cMutexLock SearchExtsLock(&SearchExts);
    cSearchExt *SearchExt = SearchExts.First();
    while (SearchExt) {
        if (SearchExt->useChannel == 2 && strcmp(SearchExt->channelGroup, group->name) == 0)
            return SearchExt;
        SearchExt = SearchExts.Next(SearchExt);
    }
    return NULL;
}

char** cChannelGroups::CreateMenuitemsList()
{
    char** menuitemsChGr = new char*[ChannelGroups.Count() + 1];
    cChannelGroup* ChannelGroup = First();
    menuitemsChGr[0] = strdup("");
    int index = 1;
    while (ChannelGroup) {
        menuitemsChGr[index++] = ChannelGroup->name;
        ChannelGroup = Next(ChannelGroup);
    }
    return menuitemsChGr;
}

// -- cMenuChannelGroupItem -----------------------------------------------------------------
cMenuChannelGroupItem::cMenuChannelGroupItem(cChannelGroup* Group)
{
    group = Group;
    Set();
}

void cMenuChannelGroupItem::Set(void)
{
    cString channelbuffer;

    cChannelGroupItem* channelInGroup = group->channels.First();
    int channelNr, chIntBegin = -1, chIntEnd = -1, chLast = -1;
    while (channelInGroup) {
        channelNr = channelInGroup->channel->Number();
        if (chIntBegin == -1)
            chIntBegin = channelNr;
        if (chIntEnd == -1)
            chIntEnd = channelNr;

        if (chLast == channelNr - 1)
            chIntEnd = channelNr;
        else {
            chIntEnd = chLast;
            if (chIntBegin == chIntEnd)
                channelbuffer = cString::sprintf("%s %d", *channelbuffer ? *channelbuffer : "", chIntBegin);
            else if (chIntEnd != -1)
                channelbuffer = cString::sprintf("%s %d-%d", *channelbuffer ? *channelbuffer : "", chIntBegin, chIntEnd);
            chIntBegin = chIntEnd = channelNr;
        }

        chLast = channelNr;
        channelInGroup = group->channels.Next(channelInGroup);
        if (!channelInGroup) {
            if (chLast == chIntBegin)
                channelbuffer = cString::sprintf("%s %d", *channelbuffer ? *channelbuffer : "", chIntBegin);
            else
                channelbuffer = cString::sprintf("%s %d-%d", *channelbuffer ? *channelbuffer : "", chIntBegin, chLast);
        }
    }

    SetText(cString::sprintf("%s\t%s", group->name, *channelbuffer ? *channelbuffer : ""));
}

// --- cMenuChannelGroups ----------------------------------------------------------
cMenuChannelGroups::cMenuChannelGroups(char** GroupName)
    : cOsdMenu(tr("Channel groups"), 20)
{
    SetMenuCategory(mcSetupPlugins);
    groupSel = -1;
    groupName = GroupName;
    if (groupName && *groupName)
        groupSel = ChannelGroups.GetIndex(*groupName);

    cChannelGroup* ChannelGroup = ChannelGroups.First();
    int index = 0;
    while (ChannelGroup) {
        Add(new cMenuChannelGroupItem(ChannelGroup), (index == groupSel ? true : false));
        ChannelGroup = ChannelGroups.Next(ChannelGroup);
        index++;
    }

    if (groupName && *groupName)
        SetHelp(trVDR("Button$Edit"), trVDR("Button$New"), trVDR("Button$Delete"), tr("Button$Select"));
    else
        SetHelp(trVDR("Button$Edit"), trVDR("Button$New"), trVDR("Button$Delete"), NULL);
    Sort();
    Display();
}

cChannelGroup *cMenuChannelGroups::CurrentGroup(void)
{
    cMenuChannelGroupItem *item = (cMenuChannelGroupItem *)Get(Current());
    return item ? item->group : NULL;
}

eOSState cMenuChannelGroups::New(void)
{
    if (HasSubMenu())
        return osContinue;
    return AddSubMenu(new cMenuEditChannelGroup(new cChannelGroup, true));
}

eOSState cMenuChannelGroups::Delete(void)
{
    cChannelGroup *curGroup = CurrentGroup();
    if (curGroup) {
        cSearchExt* search = ChannelGroups.Used(curGroup);
        if (search) {
            cString Message = cString::sprintf("%s %s", tr("Channel group used by:"), search->search);
            INFO(Message);
            return osContinue;
        }
        if (Interface->Confirm(tr("Edit$Delete group?"))) {
            ChannelGroups.Del(curGroup);
            ChannelGroups.Save();
            cOsdMenu::Del(Current());
            Display();
        }
    }
    return osContinue;
}

eOSState cMenuChannelGroups::ProcessKey(eKeys Key)
{
    int GroupNumber = HasSubMenu() ? Count() : -1;

    eOSState state = cOsdMenu::ProcessKey(Key);
    if (state == osUnknown) {
        if (HasSubMenu())
            return osContinue;
        switch (Key) {
        case kRed:
            if (CurrentGroup())
                state = AddSubMenu(new cMenuEditChannelGroup(CurrentGroup()));
            else
                state = osContinue;
            break;
        case kGreen:
            state = New();
            break;
        case kYellow:
            state = Delete();
            break;

        case kOk:
        case kBlue:
            if (groupName && *groupName) {
                free(*groupName);
                *groupName = strdup(CurrentGroup()->name);
                return osBack;
            }
        default:
            break;
        }
    }
    if (GroupNumber >= 0 && !HasSubMenu() && ChannelGroups.Get(GroupNumber)) {
        // a newly created group was confirmed with Ok
        cChannelGroup* group = ChannelGroups.Get(GroupNumber);
        Add(new cMenuChannelGroupItem(group), true);
        Display();
    }

    return state;
}

// --- cMenuEditChannelGroup --------------------------------------------------------
cMenuEditChannelGroup::cMenuEditChannelGroup(cChannelGroup *Group, bool New)
    : cOsdMenu(tr("Edit channel group"), 30)
{
    SetMenuCategory(mcSetupPlugins);
    group = Group;
    channelSel = group->CreateChannelSel();
    strcpy(name, group->name);
    addIfConfirmed = New;
    if (group)
        Set();
}

cMenuEditChannelGroup::~cMenuEditChannelGroup()
{
    free(channelSel);
}

void cMenuEditChannelGroup::Set()
{
    int current = Current();
    Clear();

    Add(new cMenuEditStrItem(tr("Group name"), name, sizeof(group->name), trVDR(FileNameChars)));
    LOCK_CHANNELS_READ; // TODO THIS MAY LOCK Channels A LONG TIME!
    const cChannel* channel = Channels->First();
    int index = 0;
    while (channel) {
        if (channel->GroupSep()) {
            channel = Channels->Next(channel);
            continue;
        }
        Add(new cMenuEditBoolItem(CHANNELNAME(channel), &channelSel[index++], trVDR("no"), trVDR("yes")));
        channel = Channels->Next(channel);
    }

    SetCurrent(Get(current));

}

eOSState cMenuEditChannelGroup::ProcessKey(eKeys Key)
{
    eOSState state = cOsdMenu::ProcessKey(Key);

    const char* ItemText = Get(Current())->Text();
    if (strlen(ItemText) > 0 && strstr(ItemText, tr("Group name")) != ItemText)
        SetHelp(tr("Button$Invert selection"), tr("Button$All yes"), tr("Button$All no"), NULL);
    else if (!InEditMode(ItemText, tr("Group name"), name))
        SetHelp(NULL, NULL, NULL, NULL);

    if (state == osUnknown) {
        switch (Key) {
        case kOk:
            if (strlen(name) == 0) {
                ERROR(tr("Group name is empty!"));
                return osContinue;
            }
            if (addIfConfirmed && ChannelGroups.GetGroupByName(name)) {
                ERROR(tr("Group name already exists!"));
                return osContinue;
            }

            {
                bool saveSearchExts = false;
                if (strcmp(group->name, name) != 0 && !addIfConfirmed) { // if group name changed, update searches
                    cMutexLock SearchExtsLock(&SearchExts);
                    cSearchExt *SearchExt = SearchExts.First();
                    while (SearchExt) {
                        if (SearchExt->useChannel == 2 &&
                            SearchExt->channelGroup &&
                            strcmp(SearchExt->channelGroup, group->name) == 0) {
                            free(SearchExt->channelGroup);
                            SearchExt->channelGroup = strdup(name);
                        }
                        SearchExt = SearchExts.Next(SearchExt);
                    }
                    saveSearchExts = true; // save them after groups are saved!
                }

                strcpy(group->name, name);
                group->CreateChannelList(channelSel);
                if (addIfConfirmed)
                    ChannelGroups.Add(group);
                ChannelGroups.Save();
                if (saveSearchExts)
                    SearchExts.Save();
            }
            addIfConfirmed = false;
            return osBack;
            break;
        case kRed:
        case kGreen:
        case kYellow: {
            LOCK_CHANNELS_READ;
            const cChannel* channel = Channels->First();
            int index = 0;
            while (channel) {
                if (channel->GroupSep()) {
                    channel = Channels->Next(channel);
                    continue;
                }

                channelSel[index] = (Key == kGreen ? 1 : (Key == kRed ? 1 - channelSel[index] : 0));
                index++;
                channel = Channels->Next(channel);
            }
            Set();
            Display();
            return osContinue;
        }

        default:
            break;
        }
    }
    return state;
}
