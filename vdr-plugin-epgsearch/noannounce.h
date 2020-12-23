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

#ifndef __NOANNOUCE_H
#define __NOANNOUCE_H

#include <string>
#include <vdr/plugin.h>

using std::string;

// --- cNoAnnouce --------------------------------------------------------
// an event that should not be announced again
class cNoAnnounce : public cListObject
{
public:
    string title;             // Title of this event
    string shortText;         // Short description of this event
    time_t startTime;        // Start time of the timer
    time_t nextAnnounce;     // time of the next announce
    tChannelID channelID;

    static char *buffer;

    cNoAnnounce();
    cNoAnnounce(const cEvent* Event, time_t NextAnnounce = 0);
    ~cNoAnnounce();
    bool operator== (const cNoAnnounce &arg) const;

    static bool Read(FILE *f);
    bool Parse(const char *s);
    const char *ToText(void) const;
    bool Save(FILE *f);
    bool Valid() {
        return startTime > 0;
    }
};

class cNoAnnounces : public cConfig<cNoAnnounce>
{
public:
    cNoAnnounce* InList(const cEvent* e);
    void ClearOutdated(void);
    void UpdateNextAnnounce(const cEvent* e, time_t NextAnnounce);
};

extern cNoAnnounces NoAnnounces;

#endif
