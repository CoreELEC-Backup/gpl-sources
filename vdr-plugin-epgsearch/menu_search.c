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
#include <string>
#include <iomanip>
#include <sstream>

#include "menu_search.h"
#include "epgsearchtools.h"
#include "epgsearchcfg.h"
#include "recdone.h"
#include "menu_searchedit.h"
#include "menu_searchactions.h"
#include "timerdone.h"

using namespace std;

// --- cMenuSearchExtItem ----------------------------------------------------------
class cMenuSearchExtItem : public cOsdItem
{
private:
public:
    cSearchExt* searchExt;
    cMenuSearchExtItem(cSearchExt* SearchExt);
    int Compare(const cListObject &ListObject) const;
    void Set(void);
};

cMenuSearchExtItem::cMenuSearchExtItem(cSearchExt* SearchExt)
{
    searchExt = SearchExt;
    Set();
}

void cMenuSearchExtItem::Set(void)
{
    ostringstream line;

    if (searchExt->useAsSearchTimer) {
        if (searchExt->IsActiveAt(time(NULL)))
            line << ">";
        else
            line << "!";
    }

    line << "\t";
    if (searchExt->search && strlen(searchExt->search) > 0)
        line << setiosflags(ios::left) << string(searchExt->search);
    else
        line << setiosflags(ios::left) << "*";

    line << "\t";
    if (searchExt->useChannel == 1) {
        if (searchExt->channelMin != searchExt->channelMax)
            line << setiosflags(ios::left) << searchExt->channelMin->Number() << " - " << searchExt->channelMax->Number();
        else
            line << setiosflags(ios::left) << setw(11) << (searchExt->useChannel ? CHANNELNAME(searchExt->channelMin) : "");
    } else if (searchExt->useChannel == 2)
        line << setiosflags(ios::left) << setw(11) << searchExt->channelGroup;
    else
        line << " ";

    line << "\t";
    if (searchExt->useTime) {
        ostringstream timeline;
        timeline << setfill('0') << setw(2) << searchExt->startTime / 100 << ":" << setw(2) << searchExt->startTime % 100;
        timeline << "\t";
        timeline << setfill('0') << setw(2) << searchExt->stopTime / 100 << ":" << setw(2) << searchExt->stopTime % 100;
        line << timeline.str();
    } else
        line << "--:--\t--:--";

    SetText(strdup(line.str().c_str()), false);
}

int cMenuSearchExtItem::Compare(const cListObject &ListObject) const
{
    cMenuSearchExtItem *p = (cMenuSearchExtItem *)&ListObject;
    return strcasecmp(searchExt->search, p->searchExt->search);
}

// --- cMenuEPGSearchExt ----------------------------------------------------------
cMenuEPGSearchExt::cMenuEPGSearchExt()
    : cOsdMenu("", 2, 20, 11, 6, 5)
{
    SetMenuCategory(mcPlugin);
    cMutexLock SearchExtsLock(&SearchExts);
    cSearchExt *SearchExt = SearchExts.First();
    while (SearchExt) {
        Add(new cMenuSearchExtItem(SearchExt));
        SearchExt = SearchExts.Next(SearchExt);
    }

    UpdateTitle();
    SetHelp(trVDR("Button$Edit"), trVDR("Button$New"), trVDR("Button$Delete"), tr("Button$Actions"));
    Sort();
}

void cMenuEPGSearchExt::UpdateTitle()
{
    int total = 0, active = 0;
    cMutexLock SearchExtsLock(&SearchExts);
    cSearchExt *SearchExt = SearchExts.First();
    while (SearchExt) {
        if (SearchExt->useAsSearchTimer) active++;
        SearchExt = SearchExts.Next(SearchExt);
        total++;
    }

    cString buffer = cString::sprintf("%s (%d/%d %s)", tr("Search entries"), active, total, tr("active"));
    SetTitle(buffer);
    Display();
}

cSearchExt *cMenuEPGSearchExt::CurrentSearchExt(void)
{
    cMenuSearchExtItem *item = (cMenuSearchExtItem *)Get(Current());
    if (item && SearchExts.Exists(item->searchExt))
        return item->searchExt;
    return NULL;
}


eOSState cMenuEPGSearchExt::New(void)
{
    if (HasSubMenu())
        return osContinue;
    return AddSubMenu(new cMenuEditSearchExt(new cSearchExt, true));
}

eOSState cMenuEPGSearchExt::Delete(void)
{
    cSearchExt *curSearchExt = CurrentSearchExt();
    if (curSearchExt) {
        if (Interface->Confirm(tr("Edit$Delete search?"))) {
            int DelID = curSearchExt->ID;
            if (Interface->Confirm(tr("Delete all timers created from this search?")))
                curSearchExt->DeleteAllTimers();
            LogFile.Log(1, "search timer %s (%d) deleted", curSearchExt->search, curSearchExt->ID);
            cMutexLock SearchExtsLock(&SearchExts);
            SearchExts.Del(curSearchExt);
            SearchExts.Save();
            RecsDone.RemoveSearchID(DelID);
            TimersDone.RemoveEntriesOfSearch(DelID);
            cOsdMenu::Del(Current());
            Display();
            UpdateTitle();
        }
    }
    return osContinue;
}

eOSState cMenuEPGSearchExt::Actions(eKeys Key)
{
    if (HasSubMenu() || Count() == 0)
        return osContinue;
    cSearchExt* search = CurrentSearchExt();

    cMenuSearchActions *menu;
    eOSState state = AddSubMenu(menu = new cMenuSearchActions(search, true));
    if (Key != kNone)
        state = menu->ProcessKey(Key);
    return state;
}


eOSState cMenuEPGSearchExt::ProcessKey(eKeys Key)
{
    int SearchNumber = HasSubMenu() ? Count() : -1;
    eOSState state = cOsdMenu::ProcessKey(Key);
    if (state == osUnknown) {
        switch (Key) {
        case k0:
            if (HasSubMenu())
                return osContinue;
            if (CurrentSearchExt())
                state = AddSubMenu(new cMenuSearchActions(CurrentSearchExt()));
            else
                state = osContinue;
            break;
        case k1...k9:
            return Actions(Key);
        case kOk:
            state = Actions(k1);
        case kBlue:
            if (HasSubMenu())
                return osContinue;
            state = AddSubMenu(new cMenuSearchActions(CurrentSearchExt()));
            break;
        case kRed:
            if (HasSubMenu())
                return osContinue;
            if (CurrentSearchExt())
                state = AddSubMenu(new cMenuEditSearchExt(CurrentSearchExt()));
            else
                state = osContinue;
            break;
        case kGreen:
            state = New();
            break;
        case kYellow:
            state = Delete();
            break;
        default:
            break;
        }
    }
    if (SearchNumber >= 0 && !HasSubMenu()) {
        cMutexLock SearchExtsLock(&SearchExts);
        cSearchExt* search = SearchExts.Get(SearchNumber);
        if (search)       // a newly created search was confirmed with Ok
            Add(new cMenuSearchExtItem(search));
        else
            search = CurrentSearchExt();
        // always update all entries, since channel group names may have changed and affect other searches
        Sort();
        for (int i = 0; i < Count(); i++) {
            cMenuSearchExtItem *item = (cMenuSearchExtItem *)Get(i);
            if (item) {
                item->Set();
                if (item->searchExt == search)
                    SetCurrent(item);
            }
        }
        Display();
        UpdateTitle();
    }

    return state;
}
