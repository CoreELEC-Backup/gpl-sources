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

#include <vector>
#include "menu_templateedit.h"
#include "changrp.h"
#include "epgsearchcats.h"
#include "epgsearchtools.h"
#include "menu_dirselect.h"
#include "menu_recsdone.h"
#include "menu_searchtemplate.h"
#include "blacklist.h"

#include <math.h>

extern cChannelGroups ChannelGroups;
extern cSearchExtCats SearchExtCats;

cMenuEditTemplate::cMenuEditTemplate(cSearchExt *SearchExt, bool New)
    : cMenuEditSearchExt(SearchExt, New, true)
{
    SetTitle(tr("Edit template"));
}

eOSState cMenuEditTemplate::ProcessKey(eKeys Key)
{
    bool bHadSubMenu = HasSubMenu();

    int iTemp_mode = data.mode;
    int iTemp_useTime = data.useTime;
    int iTemp_useChannel = data.useChannel;
    int iTemp_useDuration = data.useDuration;
    int iTemp_useDayOfWeek = data.useDayOfWeek;
    int iTemp_useAsSearchTimer = data.useAsSearchTimer;
    int iTemp_useExtEPGInfo = data.useExtEPGInfo;
    int iTemp_avoidRepeats = data.avoidRepeats;
    int iTemp_allowedRepeats = data.allowedRepeats;
    int iTemp_delAfterDays = data.delAfterDays;
    int iTemp_delMode = data.delMode;

    eOSState state = cOsdMenu::ProcessKey(Key);

    if (iTemp_mode != data.mode ||
        iTemp_useTime != data.useTime ||
        iTemp_useChannel != data.useChannel ||
        iTemp_useDuration != data.useDuration ||
        iTemp_useDayOfWeek != data.useDayOfWeek ||
        iTemp_useAsSearchTimer != data.useAsSearchTimer ||
        iTemp_useExtEPGInfo != data.useExtEPGInfo ||
        iTemp_avoidRepeats != data.avoidRepeats ||
        iTemp_allowedRepeats != data.allowedRepeats ||
        iTemp_delAfterDays != data.delAfterDays ||
        iTemp_delMode != data.delMode) {
        Set();
        Display();
    }
    const char* ItemText = Get(Current())->Text();

    if (!HasSubMenu()) {
        if (strlen(ItemText) > 0 && strstr(ItemText, tr("  from channel")) == ItemText && ((Key >= k0 &&  Key <= k9) || Key == kLeft || Key == kRight)) {
            channelMax = channelMin;
            Set();
            Display();
        }
    }

    int iOnUserDefDayItem = 0;
    int iOnDirectoryItem = 0;
    int iOnUseChannelGroups = 0;
    int iOnChannelGroup = 0;
    int iOnAvoidRepeats = 0;
    int iOnCompareCats = 0;
    int iOnUseBlacklistsSelection = 0;
    int iOnExtCatItemBrowsable = 0;
    int iOnUseAsSearchTimer = 0;
    int iCatIndex = -1;
    char* catname = NULL;

    if (!HasSubMenu() && strlen(ItemText) > 0) {
        // check, if on an item of ext. EPG info
        int iOnExtCatItem = 0;
        cSearchExtCat *SearchExtCat = SearchExtCats.First();
        int index = 0;
        while (SearchExtCat) {
            if (strstr(ItemText, IndentMenuItem(SearchExtCat->menuname)) == ItemText) {
                iOnExtCatItem = 1;
                if (SearchExtCat->nvalues > 0)
                    iOnExtCatItemBrowsable = 1;
                iCatIndex = index;
                catname = SearchExtCat->menuname;
                break;
            }
            index++;
            SearchExtCat = SearchExtCats.Next(SearchExtCat);
        }

        if (strstr(ItemText, IndentMenuItem(tr("Day of week"))) == ItemText) {
            if (data.DayOfWeek == 7) {
                SetHelp(trVDR("Button$Edit"));
                iOnUserDefDayItem = 1;
            } else
                SetHelp(NULL);
        }
        if (strstr(ItemText, tr("Use as search timer")) == ItemText) {
            if (data.useAsSearchTimer == 2) {
                SetHelp(NULL, NULL, NULL, tr("Button$Setup"));
                iOnUseAsSearchTimer = 1;
            } else
                SetHelp(NULL);
        } else if (strstr(ItemText, IndentMenuItem(tr("Directory"))) == ItemText) {
            if (!InEditMode(ItemText, IndentMenuItem(tr("Directory")), data.directory))
                SetHelp(NULL, NULL, NULL, tr("Button$Select"));
            iOnDirectoryItem = 1;
        } else if (strstr(ItemText, tr("Use channel")) == ItemText && data.useChannel == 2) {
            SetHelp(NULL, NULL, NULL, tr("Button$Setup"));
            iOnUseChannelGroups = 1;
        } else if (strstr(ItemText, IndentMenuItem(tr("Channel group"))) == ItemText) {
            SetHelp(NULL, NULL, NULL, tr("Button$Setup"));
            iOnChannelGroup = 1;
        } else if (strstr(ItemText, tr("Use blacklists")) == ItemText && data.blacklistMode == blacklistsSelection) {
            SetHelp(NULL, NULL, NULL, tr("Button$Setup"));
            iOnUseBlacklistsSelection = 1;
        } else if (strstr(ItemText, IndentMenuItem(tr("Avoid repeats"))) == ItemText) {
            SetHelp(NULL, NULL, NULL, tr("Button$Setup"));
            iOnAvoidRepeats = 1;
        } else if (strstr(ItemText, IndentMenuItem(IndentMenuItem(tr("Compare categories")))) == ItemText) {
            SetHelp(NULL, NULL, NULL, tr("Button$Setup"));
            iOnCompareCats = 1;
        } else if (iOnExtCatItem) {
            if (!InEditMode(ItemText, IndentMenuItem(catname), data.catvalues[iCatIndex]) ||
                SearchExtCats.Get(iCatIndex)->searchmode >= 10)
                SetHelp(NULL, NULL, NULL, iOnExtCatItemBrowsable ? tr("Button$Select") : NULL);
        } else if (strstr(ItemText, tr("Template name")) != ItemText)
            SetHelp(NULL, NULL, NULL, NULL);
    }
    if (state == osUnknown) {
        if (HasSubMenu())
            return osContinue;
        switch (Key) {
        case kOk:
            if (data.useChannel == 1) {
                LOCK_CHANNELS_READ;
                const cChannel *ch = Channels->GetByNumber(channelMin);
                if (ch)
                    data.channelMin = ch;
                else {
                    ERROR(tr("*** Invalid Channel ***"));
                    break;
                }
                ch = Channels->GetByNumber(channelMax);
                if (ch)
                    data.channelMax = ch;
                else {
                    ERROR(tr("*** Invalid Channel ***"));
                    break;
                }
                if (channelMin > channelMax) {
                    ERROR(tr("Please check channel criteria!"));
                    return osContinue;
                }
            }
            if (data.useChannel == 2)
                data.channelGroup = strdup(menuitemsChGr[channelGroupNr]);

            if (searchExt) {
                *searchExt = data;
                if (data.DayOfWeek == 7)
                    searchExt->DayOfWeek = UserDefDayOfWeek;

                // transfer cat selection for 'avoid repeats' back to search
                cSearchExtCat *SearchExtCat = SearchExtCats.First();
                int index = 0;
                searchExt->catvaluesAvoidRepeat = 0;
                while (SearchExtCat) {
                    if (catarrayAvoidRepeats[index])
                        searchExt->catvaluesAvoidRepeat += (1 << index);
                    SearchExtCat = SearchExtCats.Next(SearchExtCat);
                    index++;
                }

                // transfer numeric cat values back to search
                SearchExtCat = SearchExtCats.First();
                index = 0;
                while (SearchExtCat) {
                    if (SearchExtCat->searchmode >= 10) {
                        if (searchExt->catvalues[index]) free(searchExt->catvalues[index]);
                        msprintf(&searchExt->catvalues[index], "%d", catvaluesNumeric[index]);
                    }
                    SearchExtCat = SearchExtCats.Next(SearchExtCat);
                    index++;
                }

                if (data.blacklistMode == blacklistsSelection) {
                    searchExt->blacklists.Clear();
                    cBlacklistObject* blacklistObj = blacklists.First();
                    while (blacklistObj) {
                        searchExt->blacklists.Add(new cBlacklistObject(blacklistObj->blacklist));
                        blacklistObj = blacklists.Next(blacklistObj);
                    }
                } else
                    searchExt->blacklists.Clear();

                if (addIfConfirmed) {
                    cMutexLock SearchTemplatesLock(&SearchTemplates);
                    searchExt->ID = SearchTemplates.GetNewID();
                    SearchTemplates.Add(searchExt);
                }
                SearchTemplates.Save();
                addIfConfirmed = false;
            }
            return osBack;
        case kRed:
            if (iOnUserDefDayItem)
                state = AddSubMenu(new cMenuEditDaysOfWeek(&UserDefDayOfWeek));
            break;

        case kBlue:
            if (iOnDirectoryItem && !InEditMode(ItemText, IndentMenuItem(tr("Directory")), data.directory))
                state = AddSubMenu(new cMenuDirSelect(data.directory));
            if (iOnUseChannelGroups || iOnChannelGroup) {
                if (channelGroupName)
                    free(channelGroupName);
                channelGroupName = strdup(menuitemsChGr[channelGroupNr]);
                state = AddSubMenu(new cMenuChannelGroups(&channelGroupName));
            }
            if (iOnAvoidRepeats)
                state = AddSubMenu(new cMenuRecsDone(searchExt));
            if (iOnCompareCats)
                state = AddSubMenu(new cMenuSearchEditCompCats(catarrayAvoidRepeats));
            if (iOnUseBlacklistsSelection)
                state = AddSubMenu(new cMenuBlacklistsSelection(&blacklists));
            if (iOnExtCatItemBrowsable)
                state = AddSubMenu(new cMenuCatValuesSelect(data.catvalues[iCatIndex], iCatIndex, SearchExtCats.Get(iCatIndex)->searchmode));
            if (iOnUseAsSearchTimer)
                state = AddSubMenu(new cMenuSearchActivSettings(&data));
            break;
        case kGreen:
        case kYellow:
            state = osContinue;
        default:
            break;
        }
    }
    if ((iOnUseChannelGroups || iOnChannelGroup || iOnCompareCats || iOnExtCatItemBrowsable) && bHadSubMenu && !HasSubMenu()) { // return form submenu
        if (iOnExtCatItemBrowsable && SearchExtCats.Count() > 0) {
            cSearchExtCat *SearchExtCat = SearchExtCats.First();
            int index = 0;
            while (SearchExtCat) {
                if (SearchExtCat->searchmode >= 10)
                    catvaluesNumeric[index] = atoi(data.catvalues[index]);
                SearchExtCat = SearchExtCats.Next(SearchExtCat);
                index++;
            }
        }
        Set();
        Display();
    }
    return state;
}

