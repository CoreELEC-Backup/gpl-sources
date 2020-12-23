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
#include "menu_favorites.h"
#include "menu_whatson.h"
#include "menu_commands.h"

using std::set;

extern const char* ButtonBlue[3];
extern int exitToMainMenu;

cMenuFavorites::cMenuFavorites()
    : cMenuSearchResults(cTemplFile::GetTemplateByName("MenuFavorites"))
{
    BuildList();
}

#ifdef USE_GRAPHTFT
const char* cMenuFavorites::MenuKind()
{
    return "MenuEpgsFavorites";
}

void cMenuFavorites::Display(void)
{
    cOsdMenu::Display();

    if (Count() > 0) {
        int i = 0;

        for (cOsdItem *item = First(); item; item = Next(item))
            cStatus::MsgOsdEventItem(!item->Selectable() ? 0 :
                                     ((cMenuSearchResultsItem*)item)->event,
                                     item->Text(), i++, Count());
    }
}
#endif /* GRAPHTFT */

bool cMenuFavorites::BuildList()
{
    Clear();
    eventObjects.Clear();

    cSearchResults* pCompleteSearchResults = NULL;
    cMutexLock SearchExtsLock(&SearchExts);
    cSearchExt *SearchExt = SearchExts.First();
    int timespan = EPGSearchConfig.FavoritesMenuTimespan * 60;

    while (SearchExt) {
        if (SearchExt->useInFavorites)
            pCompleteSearchResults = SearchExt->Run(modeBlue == showNoPayTV ? 1 : 0, false, timespan, pCompleteSearchResults, true);
        SearchExt = SearchExts.Next(SearchExt);
    }

    if (pCompleteSearchResults) {
        set<const cEvent*> foundEvents;
        pCompleteSearchResults->SortBy(CompareEventTime);

        for (cSearchResult* pResultObj = pCompleteSearchResults->First();
             pResultObj;
             pResultObj = pCompleteSearchResults->Next(pResultObj)) {
            if (foundEvents.find(pResultObj->event) == foundEvents.end()) {
                foundEvents.insert(pResultObj->event);
                Add(new cMenuSearchResultsItem(pResultObj->event, modeYellow == showEpisode, false, menuTemplate));
                eventObjects.Add(pResultObj->event);
            }
        }
        delete pCompleteSearchResults;
    }
    SetHelpKeys();
    cString szTitle = cString::sprintf("%s: %d %s", tr("Favorites"), Count(), tr("Search results"));
    SetTitle(szTitle);
    Display();

    return true;
}

eOSState cMenuFavorites::OnGreen()
{
    eOSState state = osUnknown;
    if (!HasSubMenu()) {
        toggleKeys = 0;
        cMenuWhatsOnSearch::currentShowMode = cMenuWhatsOnSearch::GetNextMode();
        return osUnknown;
    }
    return state;
}

eOSState cMenuFavorites::OnYellow()
{
    eOSState state = osUnknown;
    if (!HasSubMenu()) {
        cMenuSearchResultsItem *item = (cMenuSearchResultsItem *)Get(Current());
        if (item && item->event) {
            LOCK_CHANNELS_READ;
            const cChannel *channel = Channels->GetByChannelID(item->event->ChannelID(), true, true);
            cMenuWhatsOnSearch::scheduleChannel = channel;
            cMenuWhatsOnSearch::currentShowMode = showNow;
        }
        toggleKeys = 0;
        return osBack;
    }
    return state;
}

eOSState cMenuFavorites::ProcessKey(eKeys Key)
{
    exitToMainMenu = 0;
    if (!HasSubMenu() && Key == kBack) {
        exitToMainMenu = 1;
        return osBack;
    }

    eOSState state = cMenuSearchResults::ProcessKey(Key);
    if (state == osUnknown) {
        switch (Key) {
        case kRecord:
        case kRed:
            state = OnRed();
            break;
        case k0:
            if (!HasSubMenu()) {
                toggleKeys = 1 - toggleKeys;
                SetHelpKeys(true);
            }
            state = osContinue;
            break;
        case k1...k9:
            state = HasSubMenu() ? osContinue : Commands(Key);
            break;
        case kBlue:
            return EPGSearchConfig.useOkForSwitch ? ShowSummary() : Switch();
            break;
        case kOk:
            if (HasSubMenu()) {
                state = cOsdMenu::ProcessKey(Key);
                break;
            }
            if (Count())
                state = EPGSearchConfig.useOkForSwitch ? Switch() : ShowSummary();
            else
                state = osBack;
            break;
        default:
            break;
        }
    }
    return state;
}

void cMenuFavorites::SetHelpKeys(bool Force)
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
        showMode nextShowMode = cMenuWhatsOnSearch::GetNextMode();
        cShowMode* mode = cMenuWhatsOnSearch::GetShowMode(nextShowMode);
        const char* szButtonGreen = NULL;
        if (mode)
            szButtonGreen = mode->GetDescription();

        if (toggleKeys == 0)
            SetHelp((EPGSearchConfig.redkeymode == 0 ? (hasTimer ? trVDR("Button$Timer") : trVDR("Button$Record")) : tr("Button$Commands")), szButtonGreen, trVDR("Button$Schedule"), EPGSearchConfig.useOkForSwitch ? trVDR("Button$Info") : trVDR("Button$Switch"));
        else
            SetHelp((EPGSearchConfig.redkeymode == 1 ? (hasTimer ? trVDR("Button$Timer") : trVDR("Button$Record")) : tr("Button$Commands")), szButtonGreen, trVDR("Button$Schedule"), EPGSearchConfig.useOkForSwitch ? trVDR("Button$Info") : trVDR("Button$Switch"));
        helpKeys = NewHelpKeys;
    }
}

