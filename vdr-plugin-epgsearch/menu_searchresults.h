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

#ifndef __EPGSEARCHRES_H
#define __EPGSEARCHRES_H

#include <vdr/menuitems.h>
#include <vdr/recording.h>
#include "templatefile.h"
#include "epgsearchext.h"
#include "menu_event.h"

typedef enum {
    showTitleEpisode,
    showEpisode
} ModeYellowSR;

typedef enum {
    showAll,
    showNoPayTV,
    showTimerPreview
} ModeBlueSR;

// --- cMenuSearchResultsItem ------------------------------------------------------
class cMenuSearchResultsItem : public cOsdItem
{
    char *fileName; // for search in recordings
    bool previewTimer;
    bool episodeOnly;
    cMenuTemplate* menuTemplate;
public:
    eTimerMatch timerMatch;
    bool inSwitchList;
    bool timerActive;
    const cEvent *event;
    const cSearchExt* search;
    const char *FileName(void) {
        return fileName;
    }
    cMenuSearchResultsItem(const cEvent *EventInfo, bool EpisodeOnly = false,
                           bool PreviewTimer = false, cMenuTemplate* MenuTemplate = NULL,
                           const cSearchExt* Search = NULL);
    cMenuSearchResultsItem(const cRecording *Recording);
    bool Update(bool Force = false);
    void SetMenuItem(cSkinDisplayMenu *DisplayMenu, int Index, bool Current, bool Selectable);
};

// --- cMenuSearchResults ------------------------------------------------------
class cMenuSearchResults : public cOsdMenu
{
    friend class cMenuFavorites;
protected:
    int helpKeys;
    cMenuTemplate* menuTemplate;
    bool ignoreRunning;
    cEventObjects eventObjects;

    virtual bool BuildList() = 0;
    virtual eOSState ProcessKey(eKeys Key);
    eOSState ShowSummary(void);
    virtual eOSState OnRed(cSearchExt* searchExt = NULL);
    virtual eOSState OnGreen();
    virtual eOSState OnYellow();
    eOSState Record(void);
    eOSState Switch(void);
    eOSState Commands(eKeys Key, cSearchExt* SearchExt = NULL);
    int GetTab(int Tab);
    virtual void SetHelpKeys(bool Force = false) = 0;
    bool Update(void);
    void UpdateCurrent();

    static const cEvent *scheduleEventInfo;
    ModeYellowSR modeYellow;
    ModeBlueSR modeBlue;
public:
    bool m_bSort;

    cMenuSearchResults(cMenuTemplate* MenuTemplate);
};

// --- cMenuSearchResultsForSearch ------------------------------------------------------
class cMenuSearchResultsForSearch : public cMenuSearchResults
{
protected:
    cSearchExt* searchExt;
    virtual bool BuildList();
    virtual void SetHelpKeys(bool Force = false);
    eOSState ProcessKey(eKeys Key);
public:
    cMenuSearchResultsForSearch(cSearchExt*, cMenuTemplate* MenuTemplate);
    virtual ~cMenuSearchResultsForSearch() {}
};

class cBlacklist;
// --- cMenuSearchResultsForBlacklist ------------------------------------------------------
class cMenuSearchResultsForBlacklist : public cMenuSearchResults
{
    cBlacklist* blacklist;
    virtual bool BuildList();
    virtual void SetHelpKeys(bool Force = false);
    eOSState ProcessKey(eKeys Key);
public:
    cMenuSearchResultsForBlacklist(cBlacklist*);
};

// --- cMenuSearchResultsForQuery ------------------------------------------------------
class cMenuSearchResultsForQuery : public cMenuSearchResultsForSearch
{
public:
    cMenuSearchResultsForQuery(const char *query, bool IgnoreRunning = false);
    ~cMenuSearchResultsForQuery();
    virtual bool BuildList();
};

// --- cMenuSearchResultsForRecs ------------------------------------------------------
class cMenuSearchResultsForRecs : public cMenuSearchResultsForQuery
{
    virtual bool BuildList();
    eOSState ProcessKey(eKeys Key);
    eOSState Play(void);
    const cRecording *GetRecording(cMenuSearchResultsItem *Item);
public:
    cMenuSearchResultsForRecs(const char *query);
};

// --- cMenuSearchResultsForList ------------------------------------------------------
class cMenuSearchResultsForList : public cMenuSearchResults
{
protected:
    cSearchResults* searchResults;
    virtual bool BuildList();
    virtual void SetHelpKeys(bool Force = false);
    virtual eOSState ProcessKey(eKeys Key);
public:
    cMenuSearchResultsForList(cSearchResults& SearchResults, const char* Title, bool IgnoreRunning = false);
};

#endif
