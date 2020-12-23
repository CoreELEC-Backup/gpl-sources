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

#include <ctype.h>
#include <algorithm>
#include "epgsearchtools.h"
#include "recdone.h"
#include "epgsearchcfg.h"
#include "epgsearchcats.h"
#include "searchtimer_thread.h"
#include "uservars.h"

#include <vdr/tools.h>

cRecsDone RecsDone;

char *cRecDone::buffer = NULL;

// -- cRecDone -----------------------------------------------------------------

cRecDone::cRecDone()
{
    title = shortText = description = aux = NULL;
    startTime = 0;
    duration = 0;
    if (buffer)
        free(buffer);
    buffer = NULL;
    searchID = -1;
    rawdescription = NULL;
}

cRecDone::cRecDone(const cTimer* Timer, const cEvent* Event, cSearchExt* Search)
{
    title = shortText = description = aux = rawdescription = NULL;
    startTime = 0;
    duration = 0;
    searchID = Search ? Search->ID : -1;
    buffer = NULL;

    if (Event) {
        if (Event->Title())
            title = strdup(Event->Title());
        if (Event->ShortText())
            shortText = strdup(Event->ShortText());
        if (Event->Description())
            description = strdup(Event->Description());
        startTime = Event->StartTime();
        duration = Event->Duration();
        if (Timer) {
            channelID = Timer->Channel()->GetChannelID();
            if (!isempty(Timer->Aux()))
                aux = strdup(Timer->Aux());
        } else {
            channelID = tChannelID::InvalidID;
            aux = NULL;
        }
    }
}

cRecDone::~cRecDone()
{
    if (buffer) {
        free(buffer);
        buffer = NULL;
    }
    if (title) {
        free(title);
        title = NULL;
    }
    if (shortText) {
        free(shortText);
        shortText = NULL;
    }
    if (description) {
        free(description);
        description = NULL;
    }
    if (rawdescription) {
        free(rawdescription);
        rawdescription = NULL;
    }
    if (aux) {
        free(aux);
        aux = NULL;
    }

}

bool cRecDone::Parse(char *s)
{
    char *t = skipspace(s + 1);
    switch (*s) {
    case 'T':
        title = strdup(t);
        break;
    case 'S':
        shortText = strdup(t);
        break;
    case 'D':
        strreplace(t, '|', '\n');
        description = strdup(t);
        break;
    case '@':
        strreplace(t, '|', '\n');
        aux = strdup(t);
        break;
    default:
        LogFile.eSysLog("ERROR: unexpected tag while reading epgsearch done data: %s", s);
        return false;
    }
    return true;
}

bool cRecDone::Read(FILE *f)
{
    cRecDone *recDone = NULL;
    char *s;
    cReadLine ReadLine;
    while ((s = ReadLine.Read(f)) != NULL) {
        char *t = skipspace(s + 1);
        switch (*s) {
        case 'R':
            if (!recDone) {
                time_t StartTime;
                int Duration, SearchID;
                int n = sscanf(t, "%ld %d %d", &StartTime, &Duration, &SearchID);
                if (n == 3) {
                    recDone = new cRecDone;
                    if (recDone) {
                        recDone->searchID = SearchID;
                        recDone->startTime = StartTime;
                        recDone->duration = Duration;
                        RecsDone.Add(recDone);
                    }
                }
            }
            break;
        case 'C': {
            s = skipspace(s + 1);
            char *p = strchr(s, ' ');
            if (p)
                *p = 0; // strips optional channel name
            if (*s) {
                tChannelID channelID = tChannelID::FromString(s);
                if (channelID.Valid()) {
                    if (recDone)
                        recDone->channelID = channelID;
                } else {
                    LogFile.Log(3, "ERROR: illegal channel ID: %s", s);
                    return false;
                }
            }
        }
        break;
        case 'r':
            recDone = NULL;
            break;
        default:
            if (recDone && !recDone->Parse(s)) {
                LogFile.Log(1, "ERROR: parsing %s", s);
                return false;
            }
        }
    }
    return true;
}


const char *cRecDone::ToText(void)
{
    char* tmpDescr = description ? strdup(description) : NULL;
    if (tmpDescr)
        strreplace(tmpDescr, '\n', '|');

    char* tmpInfo = aux ? strdup(aux) : NULL;
    if (tmpInfo)
        strreplace(tmpInfo, '\n', '|');

    if (buffer)
        free(buffer);
    buffer = NULL;

    LOCK_CHANNELS_READ;
    const cChannel *channel = Channels->GetByChannelID(channelID, true, true);
    if (!channel)
        LogFile.Log(3, "invalid channel in recs done!");

    msprintf(&buffer, "R %ld %d %d\nC %s\n%s%s%s%s%s%s%s%s%s%s%s%sr",
             startTime, duration, searchID,
             channel ? CHANNELSTRING(channel) : "",
             title ? "T " : "", title ? title : "", title ? "\n" : "",
             shortText ? "S " : "", shortText ? shortText : "", shortText ? "\n" : "",
             tmpDescr ? "D " : "", tmpDescr ? tmpDescr : "", tmpDescr ? "\n" : "",
             tmpInfo ? "@ " : "", tmpInfo ? tmpInfo : "", tmpInfo ? "\n" : "");

    if (tmpDescr)
        free(tmpDescr);
    if (tmpInfo)
        free(tmpInfo);
    return buffer;
}

bool cRecDone::Save(FILE *f)
{
    return fprintf(f, "%s\n", ToText()) > 0;
}

int cRecDone::ChannelNr()
{
    LOCK_CHANNELS_READ;
    const cChannel* channel = Channels->GetByChannelID(channelID, true, true);
    if (!channel)
        return -1;
    else
        return channel->Number();
}

// -- cRecsDone -----------------------------------------------------------------
int cRecsDone::GetCountRecordings(const cEvent* event, cSearchExt* search, cRecDone** first, int matchLimit)
{
    return GetCountRecordings(event, search->compareTitle, search->compareSubtitle, search->compareSummary, search->compareDate, search->catvaluesAvoidRepeat, first, matchLimit);
}

bool CatValuesMatch(unsigned long catvaluesAvoidRepeat, const string& rDescr, const string& eDescr)
{
    bool bCatMatch = ((rDescr != "" && eDescr != "") || (rDescr == "" && eDescr == ""));
    cSearchExtCat *SearchExtCat = SearchExtCats.First();
    int index = 0;
    while (catvaluesAvoidRepeat > 0 && SearchExtCat && bCatMatch) {
        if (catvaluesAvoidRepeat & (1 << index)) {
            char* eCatValue = GetExtEPGValue(eDescr.c_str(), SearchExtCat->name, SearchExtCat->format);
            char* rCatValue = GetExtEPGValue(rDescr.c_str(), SearchExtCat->name, SearchExtCat->format);
            if ((!eCatValue && rCatValue) ||
                (!rCatValue && eCatValue) ||
                (eCatValue && rCatValue && strcmp(eCatValue, rCatValue) != 0))
                bCatMatch = false;
            free(eCatValue);
            free(rCatValue);
        }
        SearchExtCat = SearchExtCats.Next(SearchExtCat);
        index++;
    }
    return bCatMatch;
}

bool MatchesInExpression(const string& expression, const cRecDone* recDone, const cEvent* event)
{
    cVarExpr varExpr(expression);

    cEvent recDoneEvent(0);
    recDoneEvent.SetTitle(recDone->title);
    recDoneEvent.SetShortText(recDone->shortText);
    recDoneEvent.SetDescription(recDone->description);
    recDoneEvent.SetStartTime(recDone->startTime);
    recDoneEvent.SetDuration(recDone->duration);

    string resRecDone = varExpr.Evaluate(&recDoneEvent);
    string resEvent = varExpr.Evaluate(event);
    return resRecDone == resEvent;
}

int cRecsDone::GetCountRecordings(const cEvent* event, bool compareTitle, int compareSubtitle, bool compareSummary, int compareDate, unsigned long catvaluesAvoidRepeat, cRecDone** first, int matchLimit)
{
    if (first)
        *first = NULL;
    if (!event)
        return 0;

    cMutexLock RecsDoneLock(this);
    int count = 0;

    string eTitle = "";
    if (compareTitle) {
        string s = event->Title() ? event->Title() : "";
        eTitle = GetAlNum(s);
        std::transform(eTitle.begin(), eTitle.end(), eTitle.begin(), tolower);
    }
    string eSubtitle = "";
    if (compareSubtitle) {
        string s = event->ShortText() ? event->ShortText() : "";
        eSubtitle = GetAlNum(s);
        std::transform(eSubtitle.begin(), eSubtitle.end(), eSubtitle.begin(), tolower);
    }
    string eDescr = "";
    string eRawDescr = "";
    if ((compareSummary || catvaluesAvoidRepeat != 0) && event->Description()) {
        eDescr = event->Description();
        char* rawDescr = GetRawDescription(event->Description());
        eRawDescr = rawDescr ? rawDescr : "";
        if (rawDescr) free(rawDescr);
    }

    string compareExpression = "";
    if (compareDate == 1) compareExpression = "%date%";
    if (compareDate == 2) compareExpression = "%year%-%week%";
    if (compareDate == 3) compareExpression = "%year%-%month%";

    cRecDone* firstrecDone = NULL;
    cRecDone* recDone = First();
    while (recDone) {
        string rTitle = "";
        if (compareTitle) {
            string s = recDone->title ? recDone->title : "";
            rTitle = GetAlNum(s);
            std::transform(rTitle.begin(), rTitle.end(), rTitle.begin(), tolower);
        }
        string rSubtitle = "";
        if (compareSubtitle) {
            string s = recDone->shortText ? recDone->shortText : "";
            rSubtitle = GetAlNum(s);
            std::transform(rSubtitle.begin(), rSubtitle.end(), rSubtitle.begin(), tolower);
        }
        string rDescr = "";
        string rRawDescr = "";
        if ((compareSummary || catvaluesAvoidRepeat != 0) && recDone->description) {
            rDescr = recDone->description;
            char* rawDescr = recDone->rawdescription ? recDone->rawdescription : GetRawDescription(recDone->description);
            recDone->rawdescription = rawDescr;
            rRawDescr = rawDescr ? rawDescr : "";
        }

        if ((!compareTitle || rTitle == eTitle) &&
            (!compareSubtitle || (rSubtitle == eSubtitle && rSubtitle != "")) &&
            (!compareSummary || DescriptionMatches(eRawDescr.c_str(), rRawDescr.c_str(), matchLimit)) &&
            (catvaluesAvoidRepeat == 0 || CatValuesMatch(catvaluesAvoidRepeat, rDescr, eDescr)) &&
            (compareExpression.size() == 0 || MatchesInExpression(compareExpression, recDone, event))) {
            if (!firstrecDone) firstrecDone = recDone;
            else if (firstrecDone->startTime > recDone->startTime)
                firstrecDone = recDone;
            LogFile.Log(3, "same event already recorded at %s", DAYDATETIME(recDone->startTime));
            count++;
        }

        recDone = Next(recDone);
    }


    if (first)
        *first = firstrecDone;

    return count;
}

void cRecsDone::RemoveSearchID(int ID)
{
    if (ID == -1)
        return;
    cMutexLock RecsDoneLock(this);
    if (Count() == 0)
        Load(AddDirectory(CONFIGDIR, "epgsearchdone.data"));
    cRecDone* recDone = First();
    while (recDone) {
        if (recDone->searchID == ID) {
            recDone->searchID = -1;
            LogFile.Log(2, "search timer %d removed in recording %s~%s", ID, recDone->title ? recDone->title : "unknown title", recDone->shortText ? recDone->shortText : "unknown subtitle");
        }
        recDone = Next(recDone);
    }
    Save();
}

bool cRecsDone::Load(const char *FileName)
{
    cMutexLock RecsDoneLock(this);
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
            result = cRecDone::Read(f);
            fclose(f);
        }
        if (result)
            LogFile.Log(2, "loaded recordings done from %s (count: %d)", fileName, Count());
        else
            LogFile.Log(1, "error loading recordings done from %s (count: %d)", fileName, Count());
        return result;
    }
    return false;
}

bool cRecsDone::Save(void)
{
    cMutexLock RecsDoneLock(this);
    bool result = true;
    cRecDone* l = (cRecDone*)this->First();
    cSafeFile f(fileName);
    if (f.Open()) {
        while (l) {
            if (!l->title) { // atleast a title should exist
                l = (cRecDone*)l->Next();
                continue;
            }
            if (!l->Save(f)) {
                result = false;
                break;
            }
            l = (cRecDone*)l->Next();
        }
        if (!f.Close())
            result = false;
    } else
        result = false;
    LogFile.Log(2, "saved recordings done (count: %d)", Count());
    return result;
}

int cRecsDone::GetTotalCountRecordings(cSearchExt* search, cRecDone** first)
{
    if (!search) return 0;
    if (first)
        *first = NULL;

    cMutexLock RecsDoneLock(this);
    int count = 0;

    cRecDone* firstrecDone = NULL;
    cRecDone* recDone = First();
    while (recDone) {
        if (recDone->searchID == search->ID) {
            count++;
            if (!firstrecDone) firstrecDone = recDone;
            else if (firstrecDone->startTime > recDone->startTime)
                firstrecDone = recDone;
        }
        recDone = Next(recDone);
    }

    if (first)
        *first = firstrecDone;

    return count;
}
