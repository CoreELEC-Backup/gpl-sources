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

#ifndef __TIMERDONE_H
#define __TIMERDONE_H

#include <string>
#include <vdr/plugin.h>

using std::string;

// --- cTimerDone --------------------------------------------------------
class cTimerDone : public cListObject
{
public:
    time_t start;        // Start time of the timer
    time_t stop;         // Stop time of the timer
    tChannelID channelID;
    int searchID;
    string title;
    string shorttext;

    cTimerDone();
    cTimerDone(const time_t Start, const time_t Stop, const cEvent* pEvent, const int SearchID);
    bool operator== (const cTimerDone &arg) const;

    static bool Read(FILE *f);
    bool Parse(const char *s);
    cString ToText(void) const;
    bool Save(FILE *f);
    const cEvent* GetEvent() const;
};

class cTimersDone : public cConfig<cTimerDone>, public cMutex
{
public:
    cTimerDone* InList(const time_t Start, const time_t Stop, const cEvent* pEvent, const int SearchID);
    void Update(const time_t Start, const time_t Stop, const cEvent* pEvent, const int SearchID, cTimerDone* timerdone);
    void ClearOutdated(void);
    void RemoveEntriesOfSearch(const int SearchID);
};

extern cTimersDone TimersDone;

#endif
