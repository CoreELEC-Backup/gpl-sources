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

#include <set>
#include "menu_announcelist.h"
#include "epgsearchcfg.h"
#include "noannounce.h"

bool cMenuAnnounceList::showsDetails;

// --- cMenuAnnounceList -------------------------------------------------------
cMenuAnnounceList::cMenuAnnounceList(cSearchResults& SearchResults)
    : cMenuSearchResultsForList(SearchResults,  tr("%d new broadcast"), false)
{
    showsDetails = false;
}

void cMenuAnnounceList::SetHelpKeys(bool Force)
{
    cMenuSearchResultsItem *item = (cMenuSearchResultsItem *)Get(Current());
    int NewHelpKeys = 0;
    if (item) {
        if (item->Selectable() && item->timerMatch == tmFull)
            NewHelpKeys = 2;
        else
            NewHelpKeys = 1;
    }

    bool hasTimer = (NewHelpKeys == 2);
    if (NewHelpKeys != helpKeys || Force) {
        if (toggleKeys == 0)
            SetHelp((EPGSearchConfig.redkeymode == 0 ? (hasTimer ? trVDR("Button$Timer") : trVDR("Button$Record")) : tr("Button$Commands")), m_bSort ? tr("Button$by channel") : tr("Button$by time"), modeYellow == showTitleEpisode ? tr("Button$Episode") : tr("Button$Title"), trVDR("Button$Edit"));
        else
            SetHelp((EPGSearchConfig.redkeymode == 1 ? (hasTimer ? trVDR("Button$Timer") : trVDR("Button$Record")) : tr("Button$Commands")), m_bSort ? tr("Button$by channel") : tr("Button$by time"), modeYellow == showTitleEpisode ? tr("Button$Episode") : tr("Button$Title"), trVDR("Button$Edit"));
        helpKeys = NewHelpKeys;
    }
}

eOSState cMenuAnnounceList::ProcessKey(eKeys Key)
{
    eOSState state = cMenuSearchResultsForList::ProcessKey(Key);
    if (state == osUnknown) {
        switch (Key) {
        case kBlue: {
            cMenuSearchResultsItem *item = (cMenuSearchResultsItem *)Get(Current());
            if (item) {
                if (!HasSubMenu())
                    return AddSubMenu(new cMenuAnnounceDetails(item->event, item->search));
                else if (!showsDetails)
                    return Switch();
                else
                    return osContinue;
            }
        }
        break;
        default:
            break;
        }
    }
    return state;
}

// --- cMenuAnnounceDetails -------------------------------------------------------
cMenuAnnounceDetails::cMenuAnnounceDetails(const cEvent* Event, const cSearchExt* Search)
    : cOsdMenu("", 25), event(Event)
{
    SetMenuCategory(mcEvent);
    cMenuAnnounceList::showsDetails = true;
    if (event && !isempty(event->Title())) {
        cString szTitle = cString::sprintf("%s: %s", tr("announce details"), event->Title());
        SetTitle(szTitle);
    }
    search = Search;

    announceAgain = 1;
    announceWithNextUpdate = 1;
    announceAgainDay = 0;

    cNoAnnounce* noAnnounce = NoAnnounces.InList(event);
    if (noAnnounce) {
        if (noAnnounce->nextAnnounce > 0) {
            announceAgainDay = noAnnounce->nextAnnounce;
            announceWithNextUpdate = 0;
        } else
            announceAgain = 0;
    }
    Set();
}

cMenuAnnounceDetails::~cMenuAnnounceDetails()
{
    cMenuAnnounceList::showsDetails = false;
}

void cMenuAnnounceDetails::Set()
{
    int current = Current();
    Clear();

    Add(new cMenuEditBoolItem(tr("announce again"), &announceAgain, trVDR("no"), trVDR("yes")));
    if (announceAgain) {
        Add(new cMenuEditBoolItem(IndentMenuItem(tr("with next update")), &announceWithNextUpdate, trVDR("no"), trVDR("yes")));
        if (!announceWithNextUpdate)
            Add(new cMenuEditDateItem(IndentMenuItem(IndentMenuItem(tr("again from"))), &announceAgainDay, NULL));
    } else
        announceAgainDay = 0;

    if (search) {
        cOsdItem* pInfoItem = new cOsdItem("");
        pInfoItem->SetSelectable(false);
        Add(pInfoItem);

        cString info = cString::sprintf("%s: %s", tr("Search timer"), search->search);
        pInfoItem = new cOsdItem(info);
        pInfoItem->SetSelectable(false);
        Add(pInfoItem);
    }

    SetCurrent(Get(current));
    Display();
}

eOSState cMenuAnnounceDetails::ProcessKey(eKeys Key)
{
    int iTemp_announceAgain = announceAgain;
    int iTemp_announceWithNextUpdate = announceWithNextUpdate;

    eOSState state = cOsdMenu::ProcessKey(Key);

    if (iTemp_announceAgain != announceAgain ||
        iTemp_announceWithNextUpdate != announceWithNextUpdate) {
        Set();
        Display();
    }

    if (state == osUnknown) {
        switch (Key) {
        case kRed:
        case kGreen:
        case kBlue:
        case kYellow:
        case kFastRew:
        case kFastFwd:
        case kRecord:
        case k0...k9:
            return osContinue;
        case kOk: {
            cNoAnnounce* noAnnounce = NoAnnounces.InList(event);
            if (!announceAgain || !announceWithNextUpdate) {
                if (!noAnnounce) {
                    noAnnounce = new cNoAnnounce(event, announceAgainDay);
                    NoAnnounces.Add(noAnnounce);
                } else
                    NoAnnounces.UpdateNextAnnounce(event, announceAgainDay);
                NoAnnounces.ClearOutdated();
            }
            if (announceAgain && announceWithNextUpdate && noAnnounce)
                NoAnnounces.Del(noAnnounce);
            NoAnnounces.Save();
        }
        return osBack;
        default:
            break;
        }
    }

    return state;
}
