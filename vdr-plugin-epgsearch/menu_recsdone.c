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

#include "menu_recsdone.h"
#include "epgsearchtools.h"

int sortModeRecDone = 0;

cMenuRecDoneItem::cMenuRecDoneItem(cRecDone* RecDone, bool ShowEpisodeOnly)
{
    recDone = RecDone;
    showEpisodeOnly = ShowEpisodeOnly;
    Set();
}

void cMenuRecDoneItem::Set()
{
    if (!recDone)
        return;
    char *buffer = NULL;

    char buf[32];
    struct tm tm_r;
    tm *tm = localtime_r(&recDone->startTime, &tm_r);
    strftime(buf, sizeof(buf), "%d.%m.%y %H:%M", tm);

    msprintf(&buffer, "%s\t%s~%s", buf, recDone->title && !showEpisodeOnly ? recDone->title : "",
             recDone->shortText ? recDone->shortText : "");
    SetText(buffer, false);
}

int cMenuRecDoneItem::Compare(const cListObject &ListObject) const
{
    cMenuRecDoneItem *p = (cMenuRecDoneItem *)&ListObject;
    if (sortModeRecDone == 0) // sort by Date
        if (recDone->startTime < p->recDone->startTime) return 1;
        else return -1;
    else {
        cString s1 = cString::sprintf("%s~%s", recDone->title ? recDone->title : "", recDone->shortText ? recDone->shortText : "");
        cString s2 = cString::sprintf("%s~%s", p->recDone->title ? p->recDone->title : "", p->recDone->shortText ? p->recDone->shortText : "");
        int res = strcasecmp(s1, s2);
        return res;
    }
}

// --- cMenuRecsDone ----------------------------------------------------------
#define SHOW_RECDONE_SEARCH 0
#define SHOW_RECDONE_ALL 1
#define SHOW_RECDONE_ORPHANED 2

cMenuRecsDone::cMenuRecsDone(cSearchExt* Search)
    : cOsdMenu("", 16)
{
    SetMenuCategory(mcUnknown);
    search = Search;
    showMode = SHOW_RECDONE_ALL;
    showEpisodeOnly = false;
    sortModeRecDone = 0;
    if (search) showMode = SHOW_RECDONE_SEARCH;
    Set();
    Display();
}

const char* cMenuRecsDone::ButtonBlue(cSearchExt* Search)
{
    if (showMode == SHOW_RECDONE_SEARCH && Search)
        return tr("Button$Show all");
    else if (showMode == SHOW_RECDONE_ALL)
        return tr("Button$Orphaned");
    else
        return Search->search;
}

void cMenuRecsDone::Set()
{
    Clear();
    cMutexLock RecsDoneLock(&RecsDone);
    cRecDone* recDone = RecsDone.First();
    while (recDone) {
        if (showMode == SHOW_RECDONE_ALL ||
            (showMode == SHOW_RECDONE_SEARCH && search && recDone->searchID == search->ID) ||
            (showMode == SHOW_RECDONE_ORPHANED && recDone->searchID == -1))
            Add(new cMenuRecDoneItem(recDone, showEpisodeOnly));
        recDone = RecsDone.Next(recDone);
    }
    UpdateTitle();
    SetHelp(sortModeRecDone == 0 ? tr("Button$by name") : tr("Button$by date"), tr("Button$Delete all"), trVDR("Button$Delete"), ButtonBlue(search));
    Sort();
}

cRecDone *cMenuRecsDone::CurrentRecDone(void)
{
    cMenuRecDoneItem *item = (cMenuRecDoneItem *)Get(Current());
    return item ? item->recDone : NULL;
}

void cMenuRecsDone::UpdateTitle()
{
    cString buffer = cString::sprintf("%d %s%s%s", Count(), tr("Recordings"), showMode == SHOW_RECDONE_ALL ? "" : " ", showMode != SHOW_RECDONE_SEARCH ? "" : search->search);
    SetTitle(buffer);
    Display();
}

eOSState cMenuRecsDone::Delete(void)
{
    cRecDone *curRecDone = CurrentRecDone();
    if (curRecDone) {
        if (Interface->Confirm(tr("Edit$Delete entry?"))) {
            LogFile.Log(1, "deleted recording done: '%s~%s'", curRecDone->title ? curRecDone->title : "unknown title", curRecDone->shortText ? curRecDone->shortText : "unknown subtitle");
            cMutexLock RecsDoneLock(&RecsDone);
            RecsDone.Del(curRecDone);
            RecsDone.Save();
            cOsdMenu::Del(Current());
            Display();
            UpdateTitle();
        }
    }
    return osContinue;
}

eOSState cMenuRecsDone::DeleteAll(void)
{
    if (Interface->Confirm(tr("Edit$Delete all entries?"))) {
        cMutexLock RecsDoneLock(&RecsDone);
        while (Count() > 0) {
            cMenuRecDoneItem *item = (cMenuRecDoneItem *)Get(0);
            if (!item) break;
            cRecDone *curRecDone = item->recDone;
            RecsDone.Del(curRecDone);
            cOsdMenu::Del(0);
        }
        RecsDone.Save();
        Display();
        UpdateTitle();
    }

    return osContinue;
}

eOSState cMenuRecsDone::Summary(void)
{
    if (HasSubMenu() || Count() == 0)
        return osContinue;
    cRecDone* recDone = CurrentRecDone();
    if (recDone && !isempty(recDone->description))
        return AddSubMenu(new cMenuTextDone(tr("Summary"), recDone));
    return osContinue;
}

eOSState cMenuRecsDone::ProcessKey(eKeys Key)
{
    eOSState state = cOsdMenu::ProcessKey(Key);
    if (state == osUnknown) {
        switch (Key) {
        case kOk:
            state = Summary();
            break;
        case kGreen:
            state = DeleteAll();
            break;
        case kYellow:
            state = Delete();
            break;
        case kBlue:
            showMode = (showMode + 1) % 3;
            Set();
            Display();
            break;
        case k0:
            showEpisodeOnly = !showEpisodeOnly;
            Set();
            Display();
            break;
        case kRed:
            sortModeRecDone = 1 - sortModeRecDone;
            Set();
            Display();
            break;
        case k8:
            return osContinue;
        default:
            break;
        }
    }

    return state;
}

eOSState cMenuTextDone::ProcessKey(eKeys Key)
{
    eOSState state = cMenuText::ProcessKey(Key);
    if (state == osContinue) {
        switch (Key) {
        case kBlue:
            if (recDone->aux) return AddSubMenu(new cMenuText(tr("Auxiliary info"), recDone->aux));
            break;
        case kOk:
            return osBack;
        default:
            state = osContinue;
        }
    }
    return state;
}

// --- cMenuTextDone ----------------------------------------------------------
cMenuTextDone::cMenuTextDone(const char *Title, cRecDone* RecDone, eDvbFont Font)
    : cMenuText(Title, RecDone->description, Font), recDone(RecDone)
{
    if (recDone->aux) SetHelp(NULL, NULL, NULL, tr("Button$Aux info"));
}

