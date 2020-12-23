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

#include <string>
#include "pending_notifications.h"
#include "epgsearchtools.h"


cPendingNotifications PendingNotifications;
char *cPendingNotification::buffer = NULL;
// -- cPendingNotifications -----------------------------------------------------------------

cPendingNotification::~cPendingNotification(void)
{
    if (buffer) {
        free(buffer);
        buffer = NULL;
    }
}
bool cPendingNotification::Parse(const char *s)
{
    char *t = skipspace(s + 1);
    switch (*s) {
    case 'F':
        strreplace(t, '|', '\n');
        formatted = strdup(t);
        break;
    default:
        LogFile.eSysLog("ERROR: unexpected tag while reading epgsearch pending notifications data: %s", s);
        return false;
    }
    return true;
}

bool cPendingNotification::Read(FILE *f)
{
    cPendingNotification *p = NULL;
    char *s;
    cReadLine ReadLine;
    while ((s = ReadLine.Read(f)) != NULL) {
        char *t = skipspace(s + 1);
        switch (*s) {
        case 'N':
            if (!p) {
                tEventID EventID;
                int Type, TimerMod, SearchID;
                time_t Start;
                int n = sscanf(t, "%d %u %d %d %ld", &Type, &EventID, &TimerMod, &SearchID, &Start);
                if (n == 5) {
                    p = new cPendingNotification;
                    if (p) {
                        p->type = Type;
                        p->eventID = EventID;
                        p->timerMod = TimerMod;
                        p->searchID = SearchID;
                        p->start = Start;

                        PendingNotifications.Add(p);
                    }
                }
            }
            break;
        case 'C': {
            s = skipspace(s + 1);
            char *pC = strchr(s, ' ');
            if (pC)
                *pC = 0; // strips optional channel name
            if (*s) {
                tChannelID channelID = tChannelID::FromString(s);
                if (channelID.Valid()) {
                    if (p)
                        p->channelID = channelID;
                } else {
                    LogFile.Log(3, "ERROR: illegal channel ID: %s", s);
                    return false;
                }
            }
        }
        break;
        case 'n':
            p = NULL;
            break;
        default:
            if (p && !p->Parse(s)) {
                LogFile.Log(1, "ERROR: parsing %s", s);
                return false;
            }
        }
    }
    return true;
}


const char *cPendingNotification::ToText(void) const
{
    char* tmpFormatted = formatted != "" ? strdup(formatted.c_str()) : NULL;
    if (tmpFormatted)
        strreplace(tmpFormatted, '\n', '|');

    if (buffer)
        free(buffer);
    buffer = NULL;

    LOCK_CHANNELS_READ;
    const cChannel *channel = Channels->GetByChannelID(channelID, true, true);
    if (!channel)
        LogFile.Log(3, "invalid channel in pending notifications!");

    msprintf(&buffer, "N %d %u %d %d %ld\nC %s\n%s%s%sn",
             type, eventID, timerMod, searchID, start,
             channel ? CHANNELSTRING(channel) : "",
             tmpFormatted ? "F " : "", tmpFormatted ? tmpFormatted : "", tmpFormatted ? "\n" : "");

    if (tmpFormatted)
        free(tmpFormatted);

    return buffer;
}

bool cPendingNotification::Save(FILE *f)
{
    return fprintf(f, "%s\n", ToText()) > 0;
}

bool cPendingNotifications::Load(const char *FileName)
{
    Clear();
    if (FileName) {
        free(fileName);
        fileName = strdup(FileName);
    }

    if (fileName && access(fileName, F_OK) == 0) {
        LogFile.iSysLog("loading %s", fileName);
        FILE *f = fopen(fileName, "r");
        bool result = false;
        if (f) {
            result = cPendingNotification::Read(f);
            fclose(f);
        }
        if (result)
            LogFile.Log(2, "loaded pending notifications from %s (count: %d)", fileName, Count());
        else
            LogFile.Log(1, "error loading pending notifications from %s (count: %d)", fileName, Count());
        return result;
    }
    return false;
}

bool cPendingNotifications::Save(void)
{
    bool result = true;
    cPendingNotification* l = (cPendingNotification*)this->First();
    cSafeFile f(fileName);
    if (f.Open()) {
        while (l) {
            if (!l->Save(f)) {
                result = false;
                break;
            }
            l = (cPendingNotification*)l->Next();
        }
        if (!f.Close())
            result = false;
    } else
        result = false;
    LogFile.Log(2, "saved pending notifications (count: %d)", Count());
    return result;
}


