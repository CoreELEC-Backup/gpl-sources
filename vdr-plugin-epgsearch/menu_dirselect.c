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

#include "menu_dirselect.h"
#include "epgsearchext.h"
#include "epgsearchcats.h"
#include "epgsearchtools.h"

set<string> cMenuDirSelect::directorySet;

cDirExts DirExts;
cConfDDirExts ConfDDirExts;

// --- cMenuDirItem ---------------------------------------------------------
class cMenuDirItem : public cOsdItem
{
private:
    char* directory;
public:
    cMenuDirItem(const char* text) : cOsdItem(text) {
        directory = strdup(text);
    }
    ~cMenuDirItem() {
        if (directory) free(directory);
    }
    virtual int Compare(const cListObject &ListObject) const;
};

int cMenuDirItem::Compare(const cListObject &ListObject) const
{
    const cMenuDirItem *p = (cMenuDirItem *)&ListObject;
    int hasVars1 = (strchr(directory, '%') != NULL ? 1 : 0);
    int hasVars2 = (strchr(p->directory, '%') != NULL ? 1 : 0);
    if (hasVars1 || hasVars2) {
        if (hasVars1 != hasVars2)
            return hasVars2 - hasVars1;
        else
            return strcasecmp(directory, p->directory);
    } else
        return strcasecmp(directory, p->directory);
}


// --- cMenuDirSelect ---------------------------------------------------------

cMenuDirSelect::cMenuDirSelect(char* szDirectory)
    : cOsdMenu(tr("Select directory"))
{
    SetMenuCategory(mcTimerEdit);

    Directory = szDirectory;
    yellow = NULL;
    MaxLevel = 1;
    CurLevel = 1;
    Load();
}

cMenuDirSelect::~cMenuDirSelect()
{
    if (yellow) free(yellow);
}

int cMenuDirSelect::Level(const char* szDir)
{
    int iLevel = 1;
    if (strchr(szDir, '%')) // dirs with vars always have level 1
        return 1;
    do {
        const char* pos = strchr(szDir, '~');
        if (pos) {
            iLevel++;
            szDir = pos + 1;
        } else
            return iLevel;
    } while (true);
    return 1;
}

void cMenuDirSelect::AddDistinct(const char* szText)
{
    int iLevel = Level(szText);
    MaxLevel = std::max(MaxLevel, iLevel);

    if (iLevel > CurLevel) // only show Items of the specified level, except those with vars
        return;

    for (int i = 0; i < Count(); i++) {
        const char* ItemText = Get(i)->Text();
        char* itemtext = strdup(ItemText);
        char* sztext = strdup(szText);
        ToLower(itemtext);
        ToLower(sztext);
        if (itemtext && strlen(itemtext) > 0 && strcmp(sztext, itemtext) == 0) {
            free(itemtext);
            free(sztext);
            return;
        }
        free(itemtext);
        free(sztext);
    }
    Add(new cMenuDirItem(hk(szText)));
}

void cMenuDirSelect::CreateDirSet(bool extraDirs)
{
    directorySet.clear();

    // add distinct directories from current recordings
    {
        LOCK_RECORDINGS_READ;
        for (const cRecording *recording = Recordings->First(); recording; recording = Recordings->Next(recording)) {
            if (recording->HierarchyLevels() > 0) {
                char* dir = strdup(recording->Name());
                // strip the trailing rec dir
                char* pos = strrchr(dir, '~');
                if (pos) {
                    *pos = 0;
                    for (int iLevel = 0; iLevel < recording->HierarchyLevels(); iLevel++) {
                        directorySet.insert(dir);
                        char* pos = strrchr(dir, '~');
                        if (pos)
                            *pos = 0;
                    }
                }
                free(dir);
            }
        }
    } // give up Recordings Lock
    // add distinct directories from current timers
    {
        LOCK_TIMERS_READ;
        for (const cTimer *timer = Timers->First(); timer; timer = Timers->Next(timer)) {
            char* dir = strdup(timer->File());
            // strip the trailing name dir
            char* pos = strrchr(dir, '~');
            if (pos) {
                *pos = 0;
                do {
                    directorySet.insert(dir);
                    char* pos = strrchr(dir, '~');
                    if (pos)
                        *pos = 0;
                    else break;
                } while (true);
            }
            free(dir);
        }
    }

    // add distinct directories from folders.conf
    for (cNestedItem* item = Folders.First(); item; item = Folders.Next(item))
        AddVDRFolders(item);

    if (extraDirs) {
        cMutexLock SearchExtsLock(&SearchExts);
        cSearchExt *searchExt = SearchExts.First();
        // add distinct directories from existing search timers
        while (searchExt) {
            if (strlen(searchExt->directory) > 0)
                directorySet.insert(searchExt->directory);
            searchExt = SearchExts.Next(searchExt);
        }
        // add distinct directories from epgsearchdirs.conf
        DirExts.Load(AddDirectory(CONFIGDIR, "epgsearchdirs.conf"), true);
        cDirExt* DirExt = DirExts.First();
        while (DirExt) {
            directorySet.insert(DirExt->Name());
            DirExt = DirExts.Next(DirExt);
        }
        // add distinct directories from conf.d files
        DirExt = ConfDDirExts.First();
        while (DirExt) {
            directorySet.insert(DirExt->Name());
            DirExt = ConfDDirExts.Next(DirExt);
        }
    }
}

void cMenuDirSelect::AddVDRFolders(cNestedItem* folder, string parentDirectory)
{
    if (folder == NULL) return;
    string folderDirectory = string((parentDirectory.size() == 0) ? "" : parentDirectory + "~") + folder->Text();
    directorySet.insert(folderDirectory);
    if (folder->SubItems() == NULL) return;
    for (cNestedItem* subfolder = folder->SubItems()->First(); subfolder; subfolder = folder->SubItems()->Next(subfolder))
        AddVDRFolders(subfolder, folderDirectory);
}

void cMenuDirSelect::Load()
{
    int current = Current();
    char* oldSelection = NULL; // save old selection for reselection
    if (current > -1)
        oldSelection = strdup(Get(current)->Text());
    Clear();

    CreateDirSet();
    std::set<string>::iterator it;
    for (it = directorySet.begin(); it != directorySet.end(); ++it)
        AddDistinct((*it).c_str());

    Sort();
    for (int i = 0; i < Count(); i++) {
        const char* text = Get(i)->Text();
        if (oldSelection && strchr(text, '%') == NULL && strstr(text, oldSelection) == text) { // skip entries with variables
            SetCurrent(Get(i));
            break;
        }
    }
    if (oldSelection) free(oldSelection);

    if (yellow) {
        free(yellow);
        yellow = NULL;
    }
    msprintf(&yellow, "%s %d", tr("Button$Level"), (CurLevel == MaxLevel ? 1 : CurLevel + 1));
    SetHelp(NULL, NULL, MaxLevel == 1 ? NULL : yellow, tr("Button$Select"));
    Display();
}

eOSState cMenuDirSelect::ProcessKey(eKeys Key)
{
    eOSState state = cOsdMenu::ProcessKey(Key);

    if (state == osUnknown) {
        switch ((int)Key) {
        case kBlue|k_Repeat:
        case kYellow:
            if (++CurLevel > MaxLevel)
                CurLevel = 1;
            Load();
            return osContinue;
        case kGreen:
        case kRed:
            return osContinue;
        case kBlue:
        case kOk:
            if (Count() > 0)
                strn0cpy(Directory, Get(Current())->Text(), MaxFileName);
            return osBack;
        default:
            break;
        }
    }
    return state;
}

