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

#include "menu_quicksearch.h"
#include "templatefile.h"
#include "menu_searchresults.h"
#include "epgsearchcats.h"
#include "changrp.h"
#include "epgsearchcfg.h"
#include "blacklist.h"

#define QUICKSEARCHSIMPLE 0
#define QUICKSEARCHEXT 1
#define GREENLABEL (editmode==QUICKSEARCHSIMPLE?tr("Button$Extended"):tr("Button$Simple"))

// --- cMenuQuickSearch --------------------------------------------------------
cMenuQuickSearch::cMenuQuickSearch(cSearchExt* Quicksearch)
    : cMenuEditSearchExt(Quicksearch, true, true)
{
    editmode = QUICKSEARCHSIMPLE;
    Set();
    SetHelp(NULL, GREENLABEL, NULL, NULL);
}

void cMenuQuickSearch::Set()
{
    int current = Current();
    Clear();

    Add(new cMenuEditStrItem(tr("Search term"), data.search, sizeof(data.search), tr(AllowedChars)));
    if (editmode == QUICKSEARCHEXT) {
        Add(new cMenuEditStraItem(tr("Search mode"),     &data.mode, 6, SearchModes));
        if (data.mode == 5) // fuzzy
            Add(new cMenuEditIntItem(IndentMenuItem(tr("Tolerance")), &data.fuzzyTolerance, 1, 9));

        Add(new cMenuEditBoolItem(tr("Match case"), &data.useCase, trVDR("no"), trVDR("yes")));
        Add(new cMenuEditBoolItem(tr("Use title"), &data.useTitle, trVDR("no"), trVDR("yes")));
        Add(new cMenuEditBoolItem(tr("Use subtitle"), &data.useSubtitle, trVDR("no"), trVDR("yes")));
        Add(new cMenuEditBoolItem(tr("Use description"), &data.useDescription, trVDR("no"), trVDR("yes")));


        // show Categories only if we have them
        if (SearchExtCats.Count() > 0) {
            Add(new cMenuEditBoolItem(tr("Use extended EPG info"), &data.useExtEPGInfo, trVDR("no"), trVDR("yes")));
            if (data.useExtEPGInfo) {
                cSearchExtCat *SearchExtCat = SearchExtCats.First();
                int index = 0;
                while (SearchExtCat) {
                    Add(new cMenuEditStrItem(IndentMenuItem(SearchExtCat->menuname), data.catvalues[index], MaxFileName, tr(AllowedChars)));
                    SearchExtCat = SearchExtCats.Next(SearchExtCat);
                    index++;
                }
            }
        }

        Add(new cMenuEditStraItem(tr("Use channel"), &data.useChannel, 4, UseChannelSel));
        if (data.useChannel == 1) {
            Add(new cMenuEditChanItem(tr("  from channel"),      &channelMin));
            Add(new cMenuEditChanItem(tr("  to channel"),      &channelMax));
        }
        if (data.useChannel == 2) {
            // create the char array for the menu display
            if (menuitemsChGr) delete [] menuitemsChGr;
            menuitemsChGr = ChannelGroups.CreateMenuitemsList();
            int oldchannelGroupNr = channelGroupNr;
            channelGroupNr = ChannelGroups.GetIndex(channelGroupName);
            if (channelGroupNr == -1) {
                if (oldchannelGroupNr > 0 && oldchannelGroupNr <= ChannelGroups.Count()) // perhaps its name was changed
                    channelGroupNr = oldchannelGroupNr;
                else
                    channelGroupNr = 0; // no selection
            } else
                channelGroupNr++;
            Add(new cMenuEditStraItem(IndentMenuItem(tr("Channel group")), &channelGroupNr, ChannelGroups.Count() + 1, menuitemsChGr));
        }

        Add(new cMenuEditBoolItem(tr("Use time"), &data.useTime, trVDR("no"), trVDR("yes")));
        if (data.useTime == true) {
            Add(new cMenuEditTimeItem(tr("  Start after"),        &data.startTime));
            Add(new cMenuEditTimeItem(tr("  Start before"),         &data.stopTime));
        }
        Add(new cMenuEditBoolItem(tr("Use duration"), &data.useDuration, trVDR("no"), trVDR("yes")));
        if (data.useDuration == true) {
            Add(new cMenuEditTimeItem(tr("  Min. duration"), &data.minDuration));
            Add(new cMenuEditTimeItem(tr("  Max. duration"), &data.maxDuration));
        }
        Add(new cMenuEditBoolItem(tr("Use day of week"), &data.useDayOfWeek, trVDR("no"), trVDR("yes")));
        if (data.useDayOfWeek) {
            if (data.DayOfWeek < 0) {
                UserDefDayOfWeek = data.DayOfWeek;
                data.DayOfWeek = 7;
            }
            Add(new cMenuEditStraItem(IndentMenuItem(tr("Day of week")),     &data.DayOfWeek, 8, DaysOfWeek));
        }
        Add(new cMenuEditStraItem(tr("Use blacklists"), &data.blacklistMode, 3, BlacklistModes));
    }
    SetCurrent(Get(current));
}

eOSState cMenuQuickSearch::ProcessKey(eKeys Key)
{
    bool bHadSubMenu = HasSubMenu();

    int iTemp_mode = data.mode;
    int iTemp_useTime = data.useTime;
    int iTemp_useChannel = data.useChannel;
    int iTemp_useDuration = data.useDuration;
    int iTemp_useDayOfWeek = data.useDayOfWeek;
    int iTemp_useExtEPGInfo = data.useExtEPGInfo;
    int iTemp_avoidRepeats = data.avoidRepeats;
    int iTemp_allowedRepeats = data.allowedRepeats;
    int iTemp_delAfterDays = data.delAfterDays;
    int iTemp_action = data.action;

    eOSState state = cOsdMenu::ProcessKey(Key);

    if (iTemp_mode != data.mode ||
        iTemp_useTime != data.useTime ||
        iTemp_useChannel != data.useChannel ||
        iTemp_useDuration != data.useDuration ||
        iTemp_useDayOfWeek != data.useDayOfWeek ||
        iTemp_useExtEPGInfo != data.useExtEPGInfo ||
        iTemp_avoidRepeats != data.avoidRepeats ||
        iTemp_allowedRepeats != data.allowedRepeats ||
        iTemp_delAfterDays != data.delAfterDays ||
        iTemp_action != data.action) {
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
    int iOnUseChannelGroups = 0;
    int iOnChannelGroup = 0;
    int iOnCompareCats = 0;
    int iOnUseBlacklistsSelection = 0;
    int iOnExtCatItemBrowsable = 0;
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
        if (strstr(ItemText, tr("Search term")) == ItemText) {
            if (!InEditMode(ItemText, tr("Search term"), data.search)) { // show template for a new search
                SetHelp(NULL, GREENLABEL, NULL, NULL);
            }
        }
        if (strstr(ItemText, IndentMenuItem(tr("Day of week"))) == ItemText) {
            if (data.DayOfWeek == 7) {
                SetHelp(trVDR("Button$Edit"), GREENLABEL);
                iOnUserDefDayItem = 1;
            } else
                SetHelp(NULL);
        } else if (strstr(ItemText, tr("Use channel")) == ItemText && data.useChannel == 2) {
            SetHelp(NULL, GREENLABEL, NULL, tr("Button$Setup"));
            iOnUseChannelGroups = 1;
        } else if (strstr(ItemText, IndentMenuItem(tr("Channel group"))) == ItemText) {
            SetHelp(NULL, GREENLABEL, NULL, tr("Button$Setup"));
            iOnChannelGroup = 1;
        } else if (strstr(ItemText, tr("Use blacklists")) == ItemText && data.blacklistMode == blacklistsSelection) {
            SetHelp(NULL, GREENLABEL, NULL, tr("Button$Setup"));
            iOnUseBlacklistsSelection = 1;
        } else if (iOnExtCatItem) {
            if (!InEditMode(ItemText, IndentMenuItem(catname), data.catvalues[iCatIndex]))
                SetHelp(NULL, GREENLABEL, NULL, iOnExtCatItemBrowsable ? tr("Button$Select") : NULL);
        } else if (strstr(ItemText, tr("Search term")) != ItemText)
            SetHelp(NULL, GREENLABEL, NULL, NULL);
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

            if ((data.useTitle || data.useSubtitle || data.useDescription) &&
                (int(strlen(data.search)) < 3) &&
                !Interface->Confirm(tr("Edit$Search text too short - use anyway?")))
                break;

            if (searchExt) {
                *searchExt = data;
                if (data.DayOfWeek == 7)
                    searchExt->DayOfWeek = UserDefDayOfWeek;

                if (data.blacklistMode == blacklistsSelection) {
                    searchExt->blacklists.Clear();
                    cBlacklistObject* blacklistObj = blacklists.First();
                    while (blacklistObj) {
                        searchExt->blacklists.Add(new cBlacklistObject(blacklistObj->blacklist));
                        blacklistObj = blacklists.Next(blacklistObj);
                    }
                } else
                    searchExt->blacklists.Clear();

                state = AddSubMenu(new cMenuSearchResultsForSearch(searchExt, cTemplFile::GetTemplateByName("MenuSearchResults")));
            }
            break;
        case kRed:
            if (iOnUserDefDayItem)
                state = AddSubMenu(new cMenuEditDaysOfWeek(&UserDefDayOfWeek));
            break;

        case kBlue:
            if (iOnUseChannelGroups || iOnChannelGroup) {
                if (channelGroupName)
                    free(channelGroupName);
                channelGroupName = strdup(menuitemsChGr[channelGroupNr]);
                state = AddSubMenu(new cMenuChannelGroups(&channelGroupName));
            }
            if (iOnUseBlacklistsSelection)
                state = AddSubMenu(new cMenuBlacklistsSelection(&blacklists));
            if (iOnExtCatItemBrowsable)
                state = AddSubMenu(new cMenuCatValuesSelect(data.catvalues[iCatIndex], iCatIndex, SearchExtCats.Get(iCatIndex)->searchmode));
            break;
        case kGreen:
            editmode = (editmode == QUICKSEARCHSIMPLE ? QUICKSEARCHEXT : QUICKSEARCHSIMPLE);
            SetHelp(NULL, GREENLABEL, NULL, NULL);
            Set();
            Display();
            break;
        case kYellow:
            state = osContinue;
        default:
            break;
        }
    }
    if ((iOnUseChannelGroups || iOnChannelGroup || iOnCompareCats) && bHadSubMenu && !HasSubMenu()) { // return form submenu
        Set();
        Display();
    }
    return state;
}



