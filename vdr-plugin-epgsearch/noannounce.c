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
#include "noannounce.h"
#include "epgsearchtools.h"

using std::string;

cNoAnnounces NoAnnounces;
char *cNoAnnounce::buffer = NULL;
// -- cNoAnnounce -----------------------------------------------------------------
cNoAnnounce::cNoAnnounce(void)
{
    title = shortText = "";
    startTime = nextAnnounce = 0;
    buffer = NULL;
}

cNoAnnounce::cNoAnnounce(const cEvent* e, time_t NextAnnounce)
{
    title = shortText = "";
    startTime = 0;
    buffer = NULL;
    if (e) {
        if (e->Title()) title = e->Title();
        if (e->ShortText()) shortText = e->ShortText();
        channelID = e->ChannelID();
        startTime = e->StartTime();
        nextAnnounce = NextAnnounce;
    }
}

cNoAnnounce::~cNoAnnounce(void)
{
    if (buffer) {
        free(buffer);
        buffer = NULL;
    }
}

bool cNoAnnounce::operator== (const cNoAnnounce &arg) const
{
    return (startTime == arg.startTime && channelID == arg.channelID);
}

bool cNoAnnounce::Parse(const char *s)
{
    char *line;
    char *pos;
    char *pos_next;
    int parameter = 1;
    int valuelen;

#define MAXVALUELEN (10 * MaxFileName)

    char value[MAXVALUELEN];

    pos = line = strdup(s);
    pos_next = pos + strlen(pos);
    if (*pos_next == '\n') *pos_next = 0;
    while (*pos) {
        while (*pos == ' ') pos++;
        if (*pos) {
            if (*pos != ':') {
                pos_next = strchr(pos, ':');
                if (!pos_next)
                    pos_next = pos + strlen(pos);
                valuelen = pos_next - pos + 1;
                if (valuelen > MAXVALUELEN) {
                    LogFile.eSysLog("entry '%s' is too long. Will be truncated!", pos);
                    valuelen = MAXVALUELEN;
                }
                strn0cpy(value, pos, valuelen);
                pos = pos_next;
                switch (parameter) {
                case 1:
                    title = value;
                    break;
                case 2:
                    shortText = value;
                    break;
                case 3:
                    channelID = tChannelID::FromString(value);
                    break;
                case 4:
                    startTime = atol(value);
                    break;
                case 5:
                    nextAnnounce = atol(value);
                    break;
                default:
                    break;
                } //switch
            }
            parameter++;
        }
        if (*pos) pos++;
    } //while

    title = ReplaceAll(title, "|", ":");
    shortText = ReplaceAll(shortText, "|", ":");

    free(line);
    return (parameter >= 5) ? true : false;
}

const char *cNoAnnounce::ToText(void) const
{
    free(buffer);
    msprintf(&buffer, "%s:%s:%s:%ld:%ld",
             ReplaceAll(title, ":", "|").c_str(),
             ReplaceAll(shortText, ":", "|").c_str(),
             *channelID.ToString(),
             startTime,
             nextAnnounce);
    return buffer;
}

bool cNoAnnounce::Save(FILE *f)
{
    return fprintf(f, "%s\n", ToText()) > 0;
}

// -- cNoAnnounces -----------------------------------------------------------------
cNoAnnounce* cNoAnnounces::InList(const cEvent* e)
{
    cNoAnnounce noAnnounceTemp(e);
    cNoAnnounce* noAnnounce = First();
    while (noAnnounce) {
        if (*noAnnounce == noAnnounceTemp) {
            if (noAnnounce->nextAnnounce > 0 && noAnnounce->nextAnnounce < time(NULL)) {
                Del(noAnnounce); // active again
                return NULL;
            } else
                return noAnnounce;
        }
        noAnnounce = Next(noAnnounce);
    }
    return NULL;
}

void cNoAnnounces::ClearOutdated(void)
{
    // remove outdated items
    cNoAnnounce* noAnnounce = First();
    while (noAnnounce) {
        cNoAnnounce* noAnnounceNext = Next(noAnnounce);
        if (noAnnounce->startTime < time(NULL))
            Del(noAnnounce);
        noAnnounce = noAnnounceNext;
    }
}

void cNoAnnounces::UpdateNextAnnounce(const cEvent* e, time_t NextAnnounce)
{
    cNoAnnounce* noAnnounce = InList(e);
    if (noAnnounce)
        noAnnounce->nextAnnounce = NextAnnounce;
}
