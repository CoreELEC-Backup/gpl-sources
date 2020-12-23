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

#ifndef __EPGSEARCHBL_H
#define __EPGSEARCHBL_H

#include <vdr/plugin.h>

class cSearchExt;
class cSearchResults;

class cBlacklist : public cListObject
{
    friend class cMenuEditSearchExt;
public:
    int      ID;
    char     search[MaxFileName];
    int      options;
    int      useTime;
    int      startTime;
    int      stopTime;
    int      useChannel;
    const cChannel *channelMin;
    const cChannel *channelMax;
    char*    channelGroup;
    int      useCase;
    int      mode;
    int      useTitle;
    int      useSubtitle;
    int      useDescription;
    int      useDuration;
    int      minDuration;
    int      maxDuration;
    int      useDayOfWeek;
    int      DayOfWeek;
    int      useExtEPGInfo;
    int      ignoreMissingEPGCats;
    char**   catvalues;
    int      fuzzyTolerance;
    int      isGlobal;
    static char *buffer;
public:
    cBlacklist(void);
    virtual ~cBlacklist(void);
    cBlacklist& operator= (const cBlacklist&);
    virtual bool operator< (const cListObject &ListObject);

    const char *Search(void) {
        return search;
    }
    int Options(void) {
        return options;
    }
    int StartTime(void) {
        return startTime;
    }
    int StopTime(void) {
        return stopTime;
    }
    int UseChannel(void) {
        return useChannel;
    }
    const cChannel *ChannelMin(void) {
        return channelMin;
    }
    const cChannel *ChannelMax(void) {
        return channelMax;
    }
    const cEvent * GetEventByBlacklist(const cSchedule *schedules, const cEvent *Start, int MarginStop = 0);
    bool MatchesExtEPGInfo(const cEvent* e);
    const char *ToText(void);
    bool Parse(const char *s);
    bool ParseExtEPGValues(const char *s);
    bool ParseExtEPGEntry(const char *s);
    bool Save(FILE *f);
    cSearchResults* Run(cSearchResults* pSearchResults = NULL, int MarginStop = 0);
    void CopyFromTemplate(const cSearchExt* templ);
};

class cBlacklists : public cConfig<cBlacklist>, public cMutex
{
public:
    int GetNewID(void);
    cBlacklist* GetBlacklistFromID(int ID);
    bool Exists(const cBlacklist* Blacklist);
};

class cBlacklistObject: public cListObject
{
public:
    cBlacklist* blacklist;
    cBlacklistObject(cBlacklist* Blacklist) : blacklist(Blacklist) {}
};

extern cBlacklists Blacklists;

#endif
