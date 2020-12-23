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

#include "menu_blacklists.h"
#include "epgsearchtools.h"
#include "menu_blacklistedit.h"
#include "epgsearchcfg.h"
#include "menu_searchresults.h"

using namespace std;

// --- cMenuBlacklistsItem ----------------------------------------------------------
class cMenuBlacklistsItem : public cOsdItem
{
private:
public:
    cBlacklist* blacklist;
    cMenuBlacklistsItem(cBlacklist* Blacklist);
    int Compare(const cListObject &ListObject) const;
    void Set(void);
};

cMenuBlacklistsItem::cMenuBlacklistsItem(cBlacklist* Blacklist)
{
    blacklist = Blacklist;
    Set();
}

void cMenuBlacklistsItem::Set(void)
{
    ostringstream line;

    if (blacklist->isGlobal != 0)
        line << setiosflags(ios::left) << "G";
    line << "\t";

    if (blacklist->search && strlen(blacklist->search) > 0)
        line << setiosflags(ios::left) << string(blacklist->search);
    else
        line << setiosflags(ios::left) << "*";

    line << "\t";
    if (blacklist->useChannel == 1) {
        if (blacklist->channelMin != blacklist->channelMax)
            line << setiosflags(ios::left) << blacklist->channelMin->Number() << " - " << blacklist->channelMax->Number();
        else
            line << setiosflags(ios::left) << setw(11) << (blacklist->useChannel ? CHANNELNAME(blacklist->channelMin) : "");
    } else if (blacklist->useChannel == 2)
        line << setiosflags(ios::left) << setw(11) << blacklist->channelGroup;

    line << "\t";
    if (blacklist->useTime) {
        ostringstream timeline;
        timeline << setfill('0') << setw(2) << blacklist->startTime / 100 << ":" << setw(2) << blacklist->startTime % 100;
        timeline << "\t";
        timeline << setfill('0') << setw(2) << blacklist->stopTime / 100 << ":" << setw(2) << blacklist->stopTime % 100;
        line << timeline.str();
    } else
        line << "--:--\t--:--";

    SetText(strdup(line.str().c_str()), false);
}

int cMenuBlacklistsItem::Compare(const cListObject &ListObject) const
{
    cMenuBlacklistsItem *p = (cMenuBlacklistsItem *)&ListObject;
    return strcasecmp(blacklist->search, p->blacklist->search);
}

// --- cMenuBlacklists ----------------------------------------------------------
cMenuBlacklists::cMenuBlacklists()
    : cOsdMenu(tr("Blacklists"), 3, 20, 11, 6, 5)
{
    SetMenuCategory(mcSetupPlugins);
    cMutexLock BlacklistLock(&Blacklists);
    cBlacklist *Blacklist = Blacklists.First();
    while (Blacklist) {
        Add(new cMenuBlacklistsItem(Blacklist));
        Blacklist = Blacklists.Next(Blacklist);
    }
    SetHelp(trVDR("Button$Edit"), trVDR("Button$New"), trVDR("Button$Delete"), NULL);
    Sort();
    Display();

}

cBlacklist *cMenuBlacklists::CurrentBlacklist(void)
{
    cMenuBlacklistsItem *item = (cMenuBlacklistsItem *)Get(Current());
    if (item && Blacklists.Exists(item->blacklist))
        return item->blacklist;
    return NULL;
}

eOSState cMenuBlacklists::New(void)
{
    if (HasSubMenu())
        return osContinue;
    return AddSubMenu(new cMenuBlacklistEdit(new cBlacklist, true));
}

eOSState cMenuBlacklists::Delete(void)
{
    cBlacklist *curBlacklist = CurrentBlacklist();
    if (curBlacklist) {
        if (Interface->Confirm(tr("Edit$Delete blacklist?"))) {
            LogFile.Log(1, "blacklist %s (%d) deleted", curBlacklist->search, curBlacklist->ID);
            SearchExts.RemoveBlacklistID(curBlacklist->ID);
            cMutexLock BlacklistLock(&Blacklists);
            Blacklists.Del(curBlacklist);
            Blacklists.Save();
            cOsdMenu::Del(Current());
            Display();
        }
    }
    return osContinue;
}

eOSState cMenuBlacklists::ProcessKey(eKeys Key)
{
    int BlacklistNumber = HasSubMenu() ? Count() : -1;
    eOSState state = cOsdMenu::ProcessKey(Key);
    if (state == osUnknown) {
        if (HasSubMenu())
            return osContinue;
        switch (Key) {
        case kOk:
            return AddSubMenu(new cMenuSearchResultsForBlacklist(CurrentBlacklist()));
            break;
        case kRed:
            if (CurrentBlacklist())
                state = AddSubMenu(new cMenuBlacklistEdit(CurrentBlacklist()));
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
    if (BlacklistNumber >= 0 && !HasSubMenu()) {
        cMutexLock BlacklistLock(&Blacklists);
        cBlacklist* Blacklist = Blacklists.Get(BlacklistNumber);
        if (Blacklist)       // a newly created search was confirmed with Ok
            Add(new cMenuBlacklistsItem(Blacklist));
        // always update all entries, since channel group names may have changed and affect other searches
        Sort();
        for (int i = 0; i < Count(); i++) {
            cMenuBlacklistsItem *item = (cMenuBlacklistsItem *)Get(i);
            if (item) {
                item->Set();
                if (item->blacklist == Blacklist)
                    SetCurrent(item);
            }
        }
        Display();
    }

    return state;
}
