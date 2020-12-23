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

#ifndef __MENU_SEARCHEDIT_H
#define __MENU_SEARCHEDIT_H

#include <vector>
#include <string>
#include "epgsearchext.h"


// --- cMenuEditSearchExt --------------------------------------------------------
class cMenuEditSearchExt : public cOsdMenu
{
protected:
    cSearchExt *searchExt;
    cSearchExt data;
    int channelMin;
    int channelMax;
    bool addIfConfirmed;
    int UserDefDayOfWeek;
    int channelGroupNr;
    char* channelGroupName;
    char** menuitemsChGr;
    int* catarrayAvoidRepeats;
    int* catvaluesNumeric;
    cList<cBlacklistObject> blacklists;

    char *SearchModes[6];
    char *DaysOfWeek[8];
    char *UseChannelSel[4];
    char *SearchTimerModes[6];
    char *BlacklistModes[4];
    char *DelModes[3];
    char *SearchActiveModes[3];
    char *CompareSubtitleModes[2];
    char *CompareDateModes[4];
    std::vector<int> contentStringIDs;
    int useContentDescriptors;
    int *contentStringsFlags;
    bool templateMode;
    std::vector<const char*> helpTexts;
    void AddHelp(const char* helpText);

public:
    cMenuEditSearchExt(cSearchExt *SearchExt, bool New = false, bool Template = false, bool FromEPG = false);
    virtual ~cMenuEditSearchExt();
    virtual void Set();
    virtual eOSState ProcessKey(eKeys Key);
    eOSState Help();
};

// --- cMenuEditDaysOfWeek --------------------------------------------------------
class cMenuEditDaysOfWeek : public cOsdMenu
{
private:
    int* pDaysOfWeek;
    int Days[7];
    int offset;
    bool negate;
public:
    cMenuEditDaysOfWeek(int* DaysOfWeek, int Offset = 0, bool Negate = true);
    virtual eOSState ProcessKey(eKeys Key);
};

// --- cMenuSearchEditCompCats --------------------------------------------------------
class cMenuSearchEditCompCats : public cOsdMenu
{
private:
    int* search_catarrayAvoidRepeats;
    int* edit_catarrayAvoidRepeats;
public:
    cMenuSearchEditCompCats(int* catarrayAvoidRepeats);
    ~cMenuSearchEditCompCats();
    eOSState ProcessKey(eKeys Key);
};

// --- cMenuBlacklistsSelection --------------------------------------------------------
class cMenuBlacklistsSelection : public cOsdMenu
{
private:
    int* blacklistsSel;
    cList<cBlacklistObject>* blacklists;
public:
    cMenuBlacklistsSelection(cList<cBlacklistObject>* pBlacklists);
    ~cMenuBlacklistsSelection();
    void Set();
    eOSState ProcessKey(eKeys Key);
};

// --- cMenuCatValuesSelect --------------------------------------------------------
class cMenuCatValuesSelect : public cOsdMenu
{
private:
    char* catValues;
    int catIndex;
    int searchMode;
    std::vector<bool> sel_cats;
public:
    cMenuCatValuesSelect(char* CatValues, int CatIndex, int SearchMode);
    void Set();
    eOSState ProcessKey(eKeys Key);
};


// --- cMenuSearchActivSettings --------------------------------------------------------
class cMenuSearchActivSettings : public cOsdMenu
{
private:
    cSearchExt *searchExt;
public:
    cMenuSearchActivSettings(cSearchExt *SearchExt);
    eOSState ProcessKey(eKeys Key);
};

#endif
