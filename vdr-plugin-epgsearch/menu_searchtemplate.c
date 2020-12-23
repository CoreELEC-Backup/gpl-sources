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

#include "menu_searchtemplate.h"
#include "epgsearchtools.h"
#include "epgsearchcfg.h"
#include "recdone.h"
#include "menu_templateedit.h"
#include "menu_searchactions.h"

using namespace std;

// --- cMenuSearchTemplateItem ----------------------------------------------------------
class cMenuSearchTemplateItem : public cOsdItem
{
private:
public:
    cSearchExt* searchExt;
    cMenuSearchTemplateItem(cSearchExt* SearchExt);
    int Compare(const cListObject &ListObject) const;
    void Set(void);
};

cMenuSearchTemplateItem::cMenuSearchTemplateItem(cSearchExt* SearchExt)
{
    searchExt = SearchExt;
    Set();
}

void cMenuSearchTemplateItem::Set(void)
{
    ostringstream line;

    if (searchExt->ID == EPGSearchConfig.DefSearchTemplateID)
        line << "*";
    else if (searchExt->useAsSearchTimer) {
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

int cMenuSearchTemplateItem::Compare(const cListObject &ListObject) const
{
    cMenuSearchTemplateItem *p = (cMenuSearchTemplateItem *)&ListObject;
    return strcasecmp(searchExt->search, p->searchExt->search);
}

// --- cMenuEPGSearchTemplate ----------------------------------------------------------
cMenuEPGSearchTemplate::cMenuEPGSearchTemplate(cSearchExt* Search, cBlacklist* Blacklist, bool New)
    : cOsdMenu(tr("Search templates"), 2, 20, 11, 6, 5)
{
    SetMenuCategory(mcPlugin);
    search = Search;
    blacklist = Blacklist;
    newSearch = New;
    cMutexLock SearchTemplatesLock(&SearchTemplates);
    cSearchExt *SearchExt = SearchTemplates.First();
    while (SearchExt) {
        Add(new cMenuSearchTemplateItem(SearchExt));
        SearchExt = SearchTemplates.Next(SearchExt);
    }
    SetHelp(trVDR("Button$Edit"), trVDR("Button$New"), trVDR("Button$Delete"), tr("Button$Default"));
    Sort();
    Display();

}

cSearchExt *cMenuEPGSearchTemplate::CurrentSearchTemplate(void)
{
    cMenuSearchTemplateItem *item = (cMenuSearchTemplateItem *)Get(Current());
    return item ? item->searchExt : NULL;
}

eOSState cMenuEPGSearchTemplate::New(void)
{
    if (HasSubMenu())
        return osContinue;
    return AddSubMenu(new cMenuEditTemplate(new cSearchExt, true));
}

eOSState cMenuEPGSearchTemplate::Delete(void)
{
    cSearchExt *curSearchExt = CurrentSearchTemplate();
    if (curSearchExt) {
        if (Interface->Confirm(tr("Edit$Delete template?"))) {
            cMutexLock SearchTemplatesLock(&SearchTemplates);
            SearchTemplates.Del(curSearchExt);
            SearchTemplates.Save();
            cOsdMenu::Del(Current());
            Display();
        }
    }
    return osContinue;
}

eOSState cMenuEPGSearchTemplate::DefaultTemplate(void)
{
    if (HasSubMenu())
        return osContinue;
    cSearchExt *curSearchExt = CurrentSearchTemplate();

    int current = Current();
    cMenuSearchTemplateItem *oldDefaultTemplateItem = NULL;
    for (int i = 0; i < Count(); i++) {
        cMenuSearchTemplateItem *item = (cMenuSearchTemplateItem *)Get(i);
        if (item && item->searchExt && item->searchExt->ID == EPGSearchConfig.DefSearchTemplateID)
            oldDefaultTemplateItem = item;
    }

    if (curSearchExt) {
        if (EPGSearchConfig.DefSearchTemplateID == curSearchExt->ID)
            EPGSearchConfig.DefSearchTemplateID = -1;
        else
            EPGSearchConfig.DefSearchTemplateID = curSearchExt->ID;
        cPluginManager::GetPlugin("epgsearch")->SetupStore("DefSearchTemplateID",  EPGSearchConfig.DefSearchTemplateID);

        SetCurrent(oldDefaultTemplateItem);
        RefreshCurrent();
        DisplayCurrent(true);

        SetCurrent(Get(current));
        RefreshCurrent();
        DisplayCurrent(true);

        Display();
    }
    return osContinue;
}

eOSState cMenuEPGSearchTemplate::ProcessKey(eKeys Key)
{
    int SearchNumber = HasSubMenu() ? Count() : -1;
    eOSState state = cOsdMenu::ProcessKey(Key);
    if (state == osUnknown) {
        if (HasSubMenu())
            return osContinue;
        switch (Key) {
        case kOk: {
            if (search) {
                if (!newSearch && !Interface->Confirm(tr("Overwrite existing entries?")))
                    return osBack;
                // duplicate template
                cSearchExt* t = CurrentSearchTemplate();
                if (!t) return osContinue;
                // copy all except the name and id
                search->CopyFromTemplate(t);
            }
            if (blacklist) {
                if (!newSearch && !Interface->Confirm(tr("Overwrite existing entries?")))
                    return osBack;
                // duplicate template
                cSearchExt* t = CurrentSearchTemplate();
                if (!t) return osContinue;
                // copy all except the name and id
                blacklist->CopyFromTemplate(t);
            }
            return osBack;
        }
        case kRed:
            if (CurrentSearchTemplate())
                state = AddSubMenu(new cMenuEditTemplate(CurrentSearchTemplate()));
            else
                state = osContinue;
            break;
        case kGreen:
            state = New();
            break;
        case kYellow:
            state = Delete();
            break;
        case kBlue:
            state = DefaultTemplate();
            break;
        default:
            break;
        }
    }
    if (SearchNumber >= 0 && !HasSubMenu()) {
        cSearchExt* search = SearchTemplates.Get(SearchNumber);
        if (search)       // a newly created template was confirmed with Ok
            Add(new cMenuSearchTemplateItem(search));
        else
            search = CurrentSearchTemplate();
        // always update all entries, since channel group names may have changed and affect other searches
        Sort();
        for (int i = 0; i < Count(); i++) {
            cMenuSearchTemplateItem *item = (cMenuSearchTemplateItem *)Get(i);
            if (item) {
                item->Set();
                if (item->searchExt == search)
                    SetCurrent(item);
            }
        }
        Display();
    }

    return state;
}
