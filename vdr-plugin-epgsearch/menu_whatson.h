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

#ifndef __EPGSEARCH_MENUWHATSON_H
#define __EPGSEARCH_MENUWHATSON_H

#include <vdr/menuitems.h>
#include "epgsearchcfg.h"
#include "templatefile.h"
#include "menu_event.h"

// --- cMenuMyScheduleItem ------------------------------------------------------
class cMenuMyScheduleItem : public cOsdItem
{
public:
    const cEvent *event;
    const cChannel *channel;
    showMode mode;
    eTimerMatch timerMatch;
    bool isRemote;
    bool inSwitchList;
    bool timerActive;
    cMenuTemplate* menuTemplate;

    cMenuMyScheduleItem(const cTimers *Timers, const cEvent *Event, const cChannel *Channel = NULL, showMode ShowMode = showNow, cMenuTemplate* menuTemplate = NULL);
    virtual bool Update(const cTimers *Timers, bool Force = false);
    virtual void SetMenuItem(cSkinDisplayMenu *DisplayMenu, int Index, bool Current, bool Selectable);

};

// --- cMenuMyScheduleSepItem ------------------------------------------------------
class cMenuMyScheduleSepItem : public cMenuMyScheduleItem
{
    cEvent *dummyEvent; // this event is used to store the text of the separator in its title
    // to pass it in SetMenuItem via the event argument. Would be nice
    // if VDR had a SetItemSeparator function for this
public:

    cMenuMyScheduleSepItem(const cTimers *Timers = NULL, const cEvent *Event = NULL, const cChannel *Channel = NULL);
    ~cMenuMyScheduleSepItem();
    virtual bool Update(const cTimers *Timers = NULL, bool Force = false);
    virtual void SetMenuItem(cSkinDisplayMenu *DisplayMenu, int Index, bool Current, bool Selectable);
};

// --- cMenuWhatsOnSearch ----------------------------------------------------------
class cMenuWhatsOnSearch : public cOsdMenu
{
private:
    int helpKeys;
    eOSState Record(void);
    eOSState ExtendedSearch(void);
    static int currentChannel;
    cEventObjects eventObjects;
public:
    static const cChannel *scheduleChannel;
    static cList<cShowMode> showModes;
    static showMode currentShowMode;
    static int shiftTime;
    static time_t seekTime;
    time_t GetTimeT(int iTime);
    static showMode GetNextMode();
    cMenuWhatsOnSearch(int CurrentChannelNr);
    ~cMenuWhatsOnSearch();
    void LoadSchedules();
    static int CurrentChannel(void) {
        return currentChannel;
    }
    static void SetCurrentChannel(int ChannelNr) {
        currentChannel = ChannelNr;
    }
    static const cChannel *ScheduleChannel(const cChannel* forceChannel = NULL);
    virtual eOSState ProcessKey(eKeys Key);
    virtual eOSState Switch(void);
    virtual eOSState Shift(int);
    virtual eOSState Commands(eKeys Key);
    virtual eOSState ShowSummary();
    void SetHelpKeys(bool Force = false);
    int GetTab(int Tab);
    bool Update(void);
    void CreateShowModes();
    static cShowMode* GetShowMode(showMode mode);
    void UpdateCurrent();
#ifdef USE_GRAPHTFT
    virtual const char* MenuKind();
    virtual void Display(void);
#endif
};

#endif
