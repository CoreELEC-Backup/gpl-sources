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

#include <vector>
#include "epgsearchtools.h"
#include "blacklist.h"
#include "epgsearchcats.h"
#include <vdr/tools.h>
#include "menu_dirselect.h"
#include "changrp.h"
#include "menu_search.h"
#include "menu_searchedit.h"
#include "menu_searchresults.h"
#include <math.h>

cBlacklists Blacklists;

// -- cBlacklist -----------------------------------------------------------------
char *cBlacklist::buffer = NULL;

cBlacklist::cBlacklist(void)
{
    ID = -1;
    *search = 0;
    options = 1;
    useTime = false;
    startTime = 0000;
    stopTime = 2359;
    useChannel = false;
    {
        LOCK_CHANNELS_READ;
        channelMin = Channels->GetByNumber(cDevice::CurrentChannel());
        channelMax = channelMin;
    }
    channelGroup = NULL;
    useCase = false;
    mode = 0;
    useTitle = true;
    useSubtitle = true;
    useDescription = true;
    useDuration = false;
    minDuration = 0;
    maxDuration = 130;
    useDayOfWeek = false;
    DayOfWeek = 0;
    buffer = NULL;
    isGlobal = 0;

    useExtEPGInfo = false;
    catvalues = (char**) malloc(SearchExtCats.Count() * sizeof(char*));
    cSearchExtCat *SearchExtCat = SearchExtCats.First();
    int index = 0;
    while (SearchExtCat) {
        catvalues[index] = (char*)malloc(MaxFileName);
        *catvalues[index] = 0;
        SearchExtCat = SearchExtCats.Next(SearchExtCat);
        index++;
    }
    ignoreMissingEPGCats = 0;
    fuzzyTolerance = 1;
}

cBlacklist::~cBlacklist(void)
{
    if (buffer) {
        free(buffer);
        buffer = NULL;
    }
    if (catvalues) {
        cSearchExtCat *SearchExtCat = SearchExtCats.First();
        int index = 0;
        while (SearchExtCat) {
            free(catvalues[index]);
            SearchExtCat = SearchExtCats.Next(SearchExtCat);
            index++;
        }
        free(catvalues);
        catvalues = NULL;
    }
}

cBlacklist& cBlacklist::operator= (const cBlacklist &Blacklist)
{
    char**   catvaluesTemp = this->catvalues;
    memcpy(this, &Blacklist, sizeof(*this));
    this->catvalues = catvaluesTemp;

    cSearchExtCat *SearchExtCat = SearchExtCats.First();
    int index = 0;
    while (SearchExtCat) {
        *catvalues[index] = 0;
        strcpy(catvalues[index], Blacklist.catvalues[index]);
        SearchExtCat = SearchExtCats.Next(SearchExtCat);
        index++;
    }
    return *this;
}

void cBlacklist::CopyFromTemplate(const cSearchExt* templ)
{
    options = templ->options;
    useTime = templ->useTime;
    startTime = templ->startTime;
    stopTime = templ->stopTime;
    useChannel = templ->useChannel;
    useCase = templ->useCase;
    mode = templ->mode;
    useTitle = templ->useTitle;
    useSubtitle = templ->useSubtitle;
    useDescription = templ->useDescription;
    useDuration = templ->useDuration;
    minDuration = templ->minDuration;
    maxDuration = templ->maxDuration;
    useDayOfWeek = templ->useDayOfWeek;
    DayOfWeek = templ->DayOfWeek;
    useExtEPGInfo = templ->useExtEPGInfo;
    isGlobal = 0;

    cSearchExtCat *SearchExtCat = SearchExtCats.First();
    int index = 0;
    while (SearchExtCat) {
        strcpy(catvalues[index], templ->catvalues[index]);
        SearchExtCat = SearchExtCats.Next(SearchExtCat);
        index++;
    }

    channelMin = templ->channelMin;
    channelMax = templ->channelMax;
    if (channelGroup)
        free(channelGroup);
    if (templ->channelGroup)
        channelGroup = strdup(templ->channelGroup);
    fuzzyTolerance = templ->fuzzyTolerance;
    ignoreMissingEPGCats = templ->ignoreMissingEPGCats;
}

bool cBlacklist::operator< (const cListObject &ListObject)
{
    cBlacklist *BL = (cBlacklist *)&ListObject;
    return strcasecmp(search, BL->search) < 0;
}

const char *cBlacklist::ToText(void)
{
    char tmp_Start[5] = "";
    char tmp_Stop[5] = "";
    char tmp_minDuration[5] = "";
    char tmp_maxDuration[5] = "";
    char* tmp_chanSel = NULL;
    char* tmp_search = NULL;
    char* tmp_catvalues = NULL;

    free(buffer);
    tmp_search = strdup(search);
    while (strstr(tmp_search, "|"))
        tmp_search = strreplace(tmp_search, "|", "!^pipe^!"); // ugly: replace a pipe with something, that should not happen to be part of a regular expression

    strreplace(tmp_search, ':', '|');

    if (useTime) {
        sprintf(tmp_Start, "%04d", startTime);
        sprintf(tmp_Stop, "%04d", stopTime);
    }
    if (useDuration) {
        sprintf(tmp_minDuration, "%04d", minDuration);
        sprintf(tmp_maxDuration, "%04d", maxDuration);
    }

    if (useChannel == 1) {
        if (channelMin->Number() < channelMax->Number())
            msprintf(&tmp_chanSel, "%s|%s", CHANNELSTRING(channelMin), CHANNELSTRING(channelMax));
        else
            msprintf(&tmp_chanSel, "%s", CHANNELSTRING(channelMin));
    }
    if (useChannel == 2) {
        int channelGroupNr = ChannelGroups.GetIndex(channelGroup);
        if (channelGroupNr == -1) {
            LogFile.eSysLog("channel group %s does not exist!", channelGroup);
            useChannel = 0;
        } else
            tmp_chanSel = strdup(channelGroup);
    }

    if (useExtEPGInfo) {
        cSearchExtCat *SearchExtCat = SearchExtCats.First();
        int index = 0;
        while (SearchExtCat) {
            char* catvalue = NULL;
            if (msprintf(&catvalue, "%s", catvalues[index]) != -1) {
                while (strstr(catvalue, ":"))
                    catvalue = strreplace(catvalue, ":", "!^colon^!"); // ugly: replace with something, that should not happen to be part ofa category value
                while (strstr(catvalue, "|"))
                    catvalue = strreplace(catvalue, "|", "!^pipe^!"); // ugly: replace with something, that should not happen to be part of a regular expression

                if (index == 0)
                    msprintf(&tmp_catvalues, "%d#%s", SearchExtCat->id, catvalue);
                else {
                    char* temp = tmp_catvalues;
                    msprintf(&tmp_catvalues, "%s|%d#%s", tmp_catvalues, SearchExtCat->id, catvalue);
                    free(temp);
                }
            }
            SearchExtCat = SearchExtCats.Next(SearchExtCat);
            index++;
            free(catvalue);
        }
    }

    msprintf(&buffer, "%d:%s:%d:%s:%s:%d:%s:%d:%d:%d:%d:%d:%d:%s:%s:%d:%d:%d:%s:%d:%d:%d",
             ID,
             tmp_search,
             useTime,
             tmp_Start,
             tmp_Stop,
             useChannel,
             (useChannel > 0 && useChannel < 3) ? tmp_chanSel : "0",
             useCase,
             mode,
             useTitle,
             useSubtitle,
             useDescription,
             useDuration,
             tmp_minDuration,
             tmp_maxDuration,
             useDayOfWeek,
             DayOfWeek,
             useExtEPGInfo,
             useExtEPGInfo ? tmp_catvalues : "",
             fuzzyTolerance,
             ignoreMissingEPGCats,
             isGlobal);


    if (tmp_chanSel)
        free(tmp_chanSel);
    if (tmp_search)
        free(tmp_search);
    if (tmp_catvalues)
        free(tmp_catvalues);


    return buffer;
}

bool cBlacklist::Parse(const char *s)
{
    char *line;
    char *pos;
    char *pos_next;
    int parameter = 1;
    int valuelen;
    char value[MaxFileName];

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
                if (valuelen > MaxFileName) valuelen = MaxFileName;
                strn0cpy(value, pos, valuelen);
                pos = pos_next;
                switch (parameter) {
                case 1:
                    ID = atoi(value);
                    break;
                case 2:
                    strcpy(search, value);
                    break;
                case 3:
                    useTime = atoi(value);
                    break;
                case 4:
                    startTime = atoi(value);
                    break;
                case 5:
                    stopTime = atoi(value);
                    break;
                case 6:
                    useChannel = atoi(value);
                    break;
                case 7:
                    if (useChannel == 0) {
                        channelMin = NULL;
                        channelMax = NULL;
                    } else if (useChannel == 1) {
                        int minNum = 0, maxNum = 0;
                        int fields = sscanf(value, "%d-%d", &minNum, &maxNum);
                        if (fields == 0) { // stored with ID
#ifdef __FreeBSD__
                            char *channelMinbuffer = MALLOC(char, 32);
                            char *channelMaxbuffer = MALLOC(char, 32);
                            int channels = sscanf(value, "%31[^|]|%31[^|]", channelMinbuffer, channelMaxbuffer);
#else
                            char *channelMinbuffer = NULL;
                            char *channelMaxbuffer = NULL;
                            int channels = sscanf(value, "%m[^|]|%m[^|]", &channelMinbuffer, &channelMaxbuffer);
#endif
                            LOCK_CHANNELS_READ;
                            channelMin = Channels->GetByChannelID(tChannelID::FromString(channelMinbuffer), true, true);
                            if (!channelMin) {
                                LogFile.eSysLog("ERROR: channel %s not defined", channelMinbuffer);
                                channelMin = channelMax = NULL;
                                useChannel = 0;
                            }
                            if (channels == 1)
                                channelMax = channelMin;
                            else {
                                channelMax = Channels->GetByChannelID(tChannelID::FromString(channelMaxbuffer), true, true);
                                if (!channelMax) {
                                    LogFile.eSysLog("ERROR: channel %s not defined", channelMaxbuffer);
                                    channelMin = channelMax = NULL;
                                    useChannel = 0;
                                }
                            }
                            free(channelMinbuffer);
                            free(channelMaxbuffer);
                        }
                    } else if (useChannel == 2)
                        channelGroup = strdup(value);
                    break;
                case 8:
                    useCase = atoi(value);
                    break;
                case 9:
                    mode = atoi(value);
                    break;
                case 10:
                    useTitle = atoi(value);
                    break;
                case 11:
                    useSubtitle = atoi(value);
                    break;
                case 12:
                    useDescription = atoi(value);
                    break;
                case 13:
                    useDuration = atoi(value);
                    break;
                case 14:
                    minDuration = atoi(value);
                    break;
                case 15:
                    maxDuration = atoi(value);
                    break;
                case 16:
                    useDayOfWeek = atoi(value);
                    break;
                case 17:
                    DayOfWeek = atoi(value);
                    break;
                case 18:
                    useExtEPGInfo = atoi(value);
                    break;
                case 19:
                    if (!ParseExtEPGValues(value)) {
                        LogFile.eSysLog("ERROR reading ext. EPG values - 1");
                        free(line);
                        return false;
                    }
                    break;
                case 20:
                    fuzzyTolerance = atoi(value);
                    break;
                case 21:
                    ignoreMissingEPGCats = atoi(value);
                    break;
                case 22:
                    isGlobal = atoi(value);
                    break;
                default:
                    break;
                } //switch
            }
            parameter++;
        }
        if (*pos) pos++;
    } //while

    strreplace(search, '|', ':');
    while (strstr(search, "!^pipe^!"))
        strreplace(search, "!^pipe^!", "|");

    free(line);
    return (parameter >= 19) ? true : false;
}

bool cBlacklist::ParseExtEPGValues(const char *s)
{
    char *line;
    char *pos;
    char *pos_next;
    int valuelen;
    char value[MaxFileName];

    pos = line = strdup(s);
    pos_next = pos + strlen(pos);
    if (*pos_next == '\n') *pos_next = 0;
    while (*pos) {
        while (*pos == ' ') pos++;
        if (*pos) {
            if (*pos != '|') {
                pos_next = strchr(pos, '|');
                if (!pos_next)
                    pos_next = pos + strlen(pos);
                valuelen = pos_next - pos + 1;
                if (valuelen > MaxFileName) valuelen = MaxFileName;
                strn0cpy(value, pos, valuelen);
                pos = pos_next;
                if (!ParseExtEPGEntry(value)) {
                    LogFile.eSysLog("ERROR reading ext. EPG value: %s", value);
                    free(line);
                    return false;
                }
            }
        }
        if (*pos) pos++;
    } //while

    free(line);
    return true;
}

bool cBlacklist::ParseExtEPGEntry(const char *s)
{
    char *line;
    char *pos;
    char *pos_next;
    int parameter = 1;
    int valuelen;
    char value[MaxFileName];
    int currentid = -1;

    pos = line = strdup(s);
    pos_next = pos + strlen(pos);
    if (*pos_next == '\n') *pos_next = 0;
    while (*pos) {
        while (*pos == ' ') pos++;
        if (*pos) {
            if (*pos != '#') {
                pos_next = strchr(pos, '#');
                if (!pos_next)
                    pos_next = pos + strlen(pos);
                valuelen = pos_next - pos + 1;
                if (valuelen > MaxFileName) valuelen = MaxFileName;
                strn0cpy(value, pos, valuelen);
                pos = pos_next;
                switch (parameter) {
                case 1: {
                    currentid = atoi(value);
                    int index = SearchExtCats.GetIndexFromID(currentid);
                    if (index > -1)
                        strcpy(catvalues[index], "");
                }
                break;
                case 2:
                    if (currentid > -1) {
                        int index = SearchExtCats.GetIndexFromID(currentid);
                        if (index > -1) {
                            while (strstr(value, "!^colon^!"))
                                strreplace(value, "!^colon^!", ":");
                            while (strstr(value, "!^pipe^!"))
                                strreplace(value, "!^pipe^!", "|");
                            strcpy(catvalues[index], value);
                        }
                    }
                    break;
                default:
                    break;
                } //switch
            }
            parameter++;
        }
        if (*pos) pos++;
    } //while

    free(line);
    return (parameter >= 2) ? true : false;
}

bool cBlacklist::Save(FILE *f)
{
    return fprintf(f, "%s\n", ToText()) > 0;
}

const cEvent * cBlacklist::GetEventByBlacklist(const cSchedule *schedule, const cEvent *Start, int MarginStop)
{
    const cEvent *pe = NULL;
    const cEvent *p1 = NULL;

    if (Start)
        p1 = schedule->Events()->Next(Start);
    else
        p1 = schedule->Events()->First();

    time_t tNow = time(NULL);
    char* szTest = NULL;
    char* searchText = strdup(search);

    if (!useCase)
        ToLower(searchText);

    int searchStart = 0, searchStop = 0;
    if (useTime) {
        searchStart = startTime;
        searchStop = stopTime;
        if (searchStop < searchStart)
            searchStop += 2400;
    }
    int minSearchDuration = 0;
    int maxSearchDuration = 0;
    if (useDuration) {
        minSearchDuration = minDuration / 100 * 60 + minDuration % 100;
        maxSearchDuration = maxDuration / 100 * 60 + maxDuration % 100;
    }

    for (const cEvent *p = p1; p; p = schedule->Events()->Next(p)) {
        if (!p) {
            break;
        }

        if (szTest) {
            free(szTest);
            szTest = NULL;
        }

        // ignore events without title
        if (!p->Title() || strlen(p->Title()) == 0)
            continue;

        msprintf(&szTest, "%s%s%s%s%s", (useTitle ? p->Title() : ""), (useSubtitle || useDescription) ? "~" : "",
                 (useSubtitle ? p->ShortText() : ""), useDescription ? "~" : "",
                 (useDescription ? p->Description() : ""));

        if (tNow < p->EndTime() + MarginStop * 60) {
            if (!useCase)
                ToLower(szTest);

            if (useTime) {
                time_t tEvent = p->StartTime();
                struct tm tmEvent;
                localtime_r(&tEvent, &tmEvent);
                int eventStart = tmEvent.tm_hour * 100 + tmEvent.tm_min;
                int eventStart2 = tmEvent.tm_hour * 100 + tmEvent.tm_min + 2400;
                if ((eventStart < searchStart || eventStart > searchStop) &&
                    (eventStart2 < searchStart || eventStart2 > searchStop))
                    continue;
            }
            if (useDuration) {
                int duration = p->Duration() / 60;
                if (minSearchDuration > duration || maxSearchDuration < duration)
                    continue;
            }

            if (useDayOfWeek) {
                time_t tEvent = p->StartTime();
                struct tm tmEvent;
                localtime_r(&tEvent, &tmEvent);
                if (DayOfWeek >= 0 && DayOfWeek != tmEvent.tm_wday)
                    continue;
                if (DayOfWeek < 0) {
                    int iFound = 0;
                    for (int i = 0; i < 7; i++)
                        if (abs(DayOfWeek) & (int)pow(2, i) && i == tmEvent.tm_wday) {
                            iFound = 1;
                            break;
                        }
                    if (!iFound)
                        continue;
                }
            }

            if (strlen(szTest) > 0) {
                if (!MatchesSearchMode(szTest, searchText, mode, " ,;|~", fuzzyTolerance))
                    continue;
            }

            if (useExtEPGInfo && !MatchesExtEPGInfo(p))
                continue;
            pe = p;
            break;
        }
    }
    if (szTest)
        free(szTest);
    free(searchText);
    return pe;
}

// returns a pointer array to the matching search results
cSearchResults* cBlacklist::Run(cSearchResults* pSearchResults, int MarginStop)
{
    LogFile.Log(3, "start search for blacklist '%s'", search);
    LOCK_CHANNELS_READ;
    LOCK_SCHEDULES_READ;
    const cSchedule *Schedule = Schedules->First();

    while (Schedule) {
        const cChannel* channel = Channels->GetByChannelID(Schedule->ChannelID(), true, true);
        if (!channel) {
            Schedule = Schedules->Next(Schedule);
            continue;
        }

        if (useChannel == 1 && channelMin && channelMax) {
            if (channelMin->Number() > channel->Number() || channelMax->Number() < channel->Number()) {
                Schedule = Schedules->Next(Schedule);
                continue;
            }
        }
        if (useChannel == 2 && channelGroup) {
            cChannelGroup* group = ChannelGroups.GetGroupByName(channelGroup);
            if (!group || !group->ChannelInGroup(channel)) {
                Schedule = Schedules->Next(Schedule);
                continue;
            }
        }

        if (useChannel == 3) {
            if (channel->Ca() >= CA_ENCRYPTED_MIN) {
                Schedule = Schedules->Next(Schedule);
                continue;
            }
        }

        const cEvent *pPrevEvent = NULL;
        do {
            const cEvent* event = GetEventByBlacklist(Schedule, pPrevEvent, MarginStop);
            pPrevEvent = event;
            if (event && Channels->GetByChannelID(event->ChannelID(), true, true)) {
                if (!pSearchResults) pSearchResults = new cSearchResults;
                pSearchResults->Add(new cSearchResult(event, this));
            }
        } while (pPrevEvent);
        Schedule = Schedules->Next(Schedule);
    }
    LogFile.Log(3, "found %d event(s) for blacklist '%s'", pSearchResults ? pSearchResults->Count() : 0, search);

    return pSearchResults;
}

bool cBlacklist::MatchesExtEPGInfo(const cEvent* e)
{
    if (!e || !e->Description())
        return false;
    cSearchExtCat* SearchExtCat = SearchExtCats.First();
    while (SearchExtCat) {
        char* value = NULL;
        int index = SearchExtCats.GetIndexFromID(SearchExtCat->id);
        if (index > -1)
            value = catvalues[index];
        if (value && strlen(value) > 0) {
            char* testvalue = GetExtEPGValue(e, SearchExtCat);
            if (!testvalue)
                return false;

            // compare not case sensitive
            char* valueLower = strdup(value);
            ToLower(valueLower);
            ToLower(testvalue);
            if (!MatchesSearchMode(testvalue, valueLower, SearchExtCat->searchmode, ",;|~", fuzzyTolerance)) {
                free(testvalue);
                free(valueLower);
                return false;
            }
            free(testvalue);
            free(valueLower);
        }
        SearchExtCat = SearchExtCats.Next(SearchExtCat);
    }
    return true;
}

// -- cBlacklists ----------------------------------------------------------------
int cBlacklists::GetNewID()
{
    int newID = -1;
    cMutexLock BlacklistLock(this);
    cBlacklist *l = (cBlacklist *)First();
    while (l) {
        newID = std::max(newID, l->ID);
        l = (cBlacklist *)l->Next();
    }
    return newID + 1;
}

cBlacklist* cBlacklists::GetBlacklistFromID(int ID)
{
    if (ID == -1)
        return NULL;
    cMutexLock BlacklistLock(this);
    cBlacklist *l = (cBlacklist *)First();
    while (l) {
        if (l->ID == ID)
            return l;
        l = (cBlacklist *)l->Next();
    }
    return NULL;
}

bool cBlacklists::Exists(const cBlacklist* Blacklist)
{
    cMutexLock BlacklistLock(this);
    cBlacklist *l = (cBlacklist*)First();
    while (l) {
        if (l == Blacklist)
            return true;
        l = (cBlacklist*)l->Next();
    }
    return false;
}
