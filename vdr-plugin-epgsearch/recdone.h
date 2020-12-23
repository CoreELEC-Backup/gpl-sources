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

#ifndef __RECDONE_H
#define __RECDONE_H

#include <vdr/status.h>
#include <vdr/plugin.h>
#include <vdr/recording.h>
#include "epgsearchext.h"

class cSearchExt;

// --- cRecDone --------------------------------------------------------
class cRecDone : public cListObject
{
public:
    char *title;             // Title of this event
    char *shortText;         // Short description of this event
    char *description;       // Description of this event
    char *aux;               // Aux info
    time_t startTime;        // Start time of the timer
    int duration;
    int searchID;          // ID of the search, that triggered this recording
    tChannelID channelID;

    char* rawdescription;
    static char *buffer;

    cRecDone();
    cRecDone(const cTimer*, const cEvent* event, cSearchExt* search);
    ~cRecDone();

    static bool Read(FILE *f);
    bool Parse(char *s);
    const char *ToText(void);
    bool Save(FILE *f);
    int ChannelNr();
};

class cRecsDone : public cList<cRecDone>, public cMutex
{
private:
    char *fileName;
public:
    void Clear(void) {
        free(fileName);
        fileName = NULL;
        cList<cRecDone>::Clear();
    }
    cRecsDone(void) {
        fileName = NULL;
    }
    int GetCountRecordings(const cEvent* event, cSearchExt* search, cRecDone** first = NULL, int matchLimit = 90);
    int GetCountRecordings(const cEvent*, bool compareTitle, int compareSubtitle, bool compareSummary, int compareDate, unsigned long, cRecDone** first = NULL, int matchLimit = 90);
    int GetTotalCountRecordings(cSearchExt* search, cRecDone** first);
    void RemoveSearchID(int ID);
    bool Load(const char *FileName = NULL);
    bool Save(void);
};

extern cRecsDone RecsDone;

#endif
