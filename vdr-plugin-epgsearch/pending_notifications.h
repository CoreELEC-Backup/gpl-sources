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

#ifndef __PENDING_NOTIFICATIONS_H
#define __PENDING_NOTIFICATIONS_H

#include <vdr/plugin.h>

using std::string;

// --- cPendingNotification --------------------------------------------------------
class cPendingNotification : public cListObject
{
public:
    int type;
    tEventID eventID;
    tChannelID channelID;
    time_t start;
    uint timerMod;
    int searchID;
    string formatted;


    static char *buffer;

    cPendingNotification()
        : type(-1), start(-1), timerMod(-1), searchID(-1) {}
    cPendingNotification(int Type, tEventID EventID, tChannelID ChannelID,  time_t Start, uint TimerMod = -1,
                         int SearchID = -1, string Formatted = "")
        : type(Type), eventID(EventID), channelID(ChannelID), start(Start), timerMod(TimerMod),
          searchID(SearchID), formatted(Formatted) {}
    ~cPendingNotification();

    static bool Read(FILE *f);
    bool Parse(const char *s);
    const char *ToText(void) const;
    bool Save(FILE *f);
};

class cPendingNotifications : public cList<cPendingNotification>
{
public:
private:
    char *fileName;
public:
    cPendingNotifications() {
        fileName = NULL;
    }
    void Clear(void) {
        free(fileName);
        fileName = NULL;
        cList<cPendingNotification>::Clear();
    }
    bool Load(const char *FileName = NULL);
    bool Save(void);
};

extern cPendingNotifications PendingNotifications;

#endif
