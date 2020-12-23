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
#include "epgsearchext.h"
#include "epgsearchcfg.h"
#include "epgsearchcats.h"
#include "epgsearchtools.h"
#include <vdr/tools.h>
#include "menu_searchresults.h"
#include "menu_dirselect.h"
#include "changrp.h"
#include "menu_search.h"
#include "menu_searchedit.h"
#include "menu_recsdone.h"
#include "searchtimer_thread.h"
#include "timer_thread.h"
#include "uservars.h"
#include "blacklist.h"
#include <math.h>

cSearchExts SearchExts;
cSearchExts SearchTemplates;

#ifndef MAX_SUBTITLE_LENGTH
#define MAX_SUBTITLE_LENGTH 40
#endif

// -- cSearchExt -----------------------------------------------------------------
char *cSearchExt::buffer = NULL;

cSearchExt::cSearchExt(void)
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
        channelMax = Channels->GetByNumber(cDevice::CurrentChannel());
    }
    channelGroup = NULL;
    useCase = false;
    mode = 0;
    useTitle = true;
    useSubtitle = true;
    useDescription = true;
    useDuration = false;
    minDuration = 0;
    maxDuration = 2359;
    useAsSearchTimer = false;
    useDayOfWeek = false;
    DayOfWeek = 0;
    buffer = NULL;
    *directory = 0;
    useEpisode = 0;
    Priority = EPGSearchConfig.DefPriority;
    Lifetime = EPGSearchConfig.DefLifetime;
    MarginStart = EPGSearchConfig.DefMarginStart;
    MarginStop = EPGSearchConfig.DefMarginStop;
    useVPS = false;
    action = searchTimerActionRecord;
    useExtEPGInfo = false;
    contentsFilter = "";
    catvalues = (char**) malloc(SearchExtCats.Count() * sizeof(char*));
    cSearchExtCat *SearchExtCat = SearchExtCats.First();
    int index = 0;
    while (SearchExtCat) {
        catvalues[index] = (char*)malloc(MaxFileName);
        *catvalues[index] = 0;
        SearchExtCat = SearchExtCats.Next(SearchExtCat);
        index++;
    }
    avoidRepeats = 0;
    compareTitle = 1;
    compareSubtitle = 1;
    compareSummary = 1;
    compareSummaryMatchInPercent = 90;
    compareDate = 0;
    allowedRepeats = 0;
    catvaluesAvoidRepeat = 0;
    repeatsWithinDays = 0;
    delAfterDays = 0;
    recordingsKeep = 0;
    switchMinsBefore = 1;
    pauseOnNrRecordings = 0;
    blacklistMode = blacklistsOnlyGlobal; // no blacklists
    blacklists.Clear();
    fuzzyTolerance = 1;
    useInFavorites = 0;
    menuTemplate = 0;
    delMode = 0;
    delAfterCountRecs = 0;
    delAfterDaysOfFirstRec = 0;
    useAsSearchTimerFrom = 0;
    useAsSearchTimerTil = 0;
    ignoreMissingEPGCats = 0;
    unmuteSoundOnSwitch = 0;
    skipRunningEvents = false;
}

cSearchExt::~cSearchExt(void)
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

cSearchExt& cSearchExt::operator= (const cSearchExt &SearchExt)
{
    CopyFromTemplate(&SearchExt);
    ID = SearchExt.ID;
    strcpy(search, SearchExt.search);

    cSearchExtCat *SearchExtCat = SearchExtCats.First();
    int index = 0;
    while (SearchExtCat) {
        *catvalues[index] = 0;
        strcpy(catvalues[index], SearchExt.catvalues[index]);
        SearchExtCat = SearchExtCats.Next(SearchExtCat);
        index++;
    }

    return *this;
}

void cSearchExt::CopyFromTemplate(const cSearchExt* templ, bool ignoreChannelSettings)
{
    options = templ->options;
    useTime = templ->useTime;
    startTime = templ->startTime;
    stopTime = templ->stopTime;
    if (!ignoreChannelSettings)
        useChannel = templ->useChannel;
    useCase = templ->useCase;
    mode = templ->mode;
    useTitle = templ->useTitle;
    useSubtitle = templ->useSubtitle;
    useDescription = templ->useDescription;
    useDuration = templ->useDuration;
    minDuration = templ->minDuration;
    maxDuration = templ->maxDuration;
    useAsSearchTimer = templ->useAsSearchTimer;
    useDayOfWeek = templ->useDayOfWeek;
    DayOfWeek = templ->DayOfWeek;
    useEpisode = templ->useEpisode;
    strcpy(directory, templ->directory);
    Priority = templ->Priority;
    Lifetime = templ->Lifetime;
    MarginStart = templ->MarginStart;
    MarginStop = templ->MarginStop;
    useVPS = templ->useVPS;
    action = templ->action;
    useExtEPGInfo = templ->useExtEPGInfo;
    contentsFilter = templ->contentsFilter;
    switchMinsBefore = templ->switchMinsBefore;
    pauseOnNrRecordings = templ->pauseOnNrRecordings;

    cSearchExtCat *SearchExtCat = SearchExtCats.First();
    int index = 0;
    while (SearchExtCat) {
        strcpy(catvalues[index], templ->catvalues[index]);
        SearchExtCat = SearchExtCats.Next(SearchExtCat);
        index++;
    }

    if (!ignoreChannelSettings) {
        channelMin = templ->channelMin;
        channelMax = templ->channelMax;
        if (channelGroup) {
            free(channelGroup);
            channelGroup = NULL;
        }
        if (templ->channelGroup)
            channelGroup = strdup(templ->channelGroup);
    }
    avoidRepeats = templ->avoidRepeats;
    compareTitle = templ->compareTitle;
    compareSubtitle = templ->compareSubtitle;
    compareSummary = templ->compareSummary;
    compareSummaryMatchInPercent = templ->compareSummaryMatchInPercent;
    compareDate = templ->compareDate;
    allowedRepeats = templ->allowedRepeats;
    catvaluesAvoidRepeat = templ->catvaluesAvoidRepeat;
    repeatsWithinDays = templ->repeatsWithinDays;
    delAfterDays = templ->delAfterDays;
    recordingsKeep = templ->recordingsKeep;
    blacklistMode = templ->blacklistMode;
    blacklists.Clear();
    const cBlacklistObject* blacklistObj = templ->blacklists.First();
    while (blacklistObj) {
        blacklists.Add(new cBlacklistObject(blacklistObj->blacklist));
        blacklistObj = templ->blacklists.Next(blacklistObj);
    }
    fuzzyTolerance = templ->fuzzyTolerance;
    useInFavorites = templ->useInFavorites;
    menuTemplate = templ->menuTemplate;
    delMode = templ->delMode;
    delAfterCountRecs = templ->delAfterCountRecs;
    delAfterDaysOfFirstRec = templ->delAfterDaysOfFirstRec;
    useAsSearchTimerFrom = templ->useAsSearchTimerFrom;
    useAsSearchTimerTil = templ->useAsSearchTimerTil;
    ignoreMissingEPGCats = templ->ignoreMissingEPGCats;
    unmuteSoundOnSwitch = templ->unmuteSoundOnSwitch;
}

bool cSearchExt::operator< (const cListObject &ListObject)
{
    cSearchExt *SE = (cSearchExt *)&ListObject;
    return strcasecmp(search, SE->search) < 0;
}

char* replaceSpecialChars(const char* in)
{
    char* tmp_in = strdup(in);
    while (strstr(tmp_in, "|"))
        tmp_in = strreplace(tmp_in, "|", "!^pipe^!"); // ugly: replace a pipe with something,
    strreplace(tmp_in, ':', '|');
    return tmp_in;
}

const char *cSearchExt::ToText()
{
    char tmp_Start[5] = "";
    char tmp_Stop[5] = "";
    char tmp_minDuration[5] = "";
    char tmp_maxDuration[5] = "";
    cString tmp_chanSel;
    char* tmp_catvalues = NULL;
    char* tmp_blacklists = NULL;

    free(buffer);
    char* tmp_search = replaceSpecialChars(search);
    char* tmp_directory = replaceSpecialChars(directory);
    char* tmp_contentsFilter = replaceSpecialChars(contentsFilter.c_str());

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
            tmp_chanSel = cString::sprintf("%s|%s", CHANNELSTRING(channelMin), CHANNELSTRING(channelMax));
        else
            tmp_chanSel = cString(CHANNELSTRING(channelMin));
    }
    if (useChannel == 2) {
        int channelGroupNr = ChannelGroups.GetIndex(channelGroup);
        if (channelGroupNr == -1) {
            LogFile.eSysLog("channel group '%s' does not exist!", channelGroup);
            useChannel = 0;
        } else
            tmp_chanSel = cString(channelGroup);
    }

    if (useExtEPGInfo) {
        cSearchExtCat *SearchExtCat = SearchExtCats.First();
        int index = 0;
        while (SearchExtCat) {
            char* catvalue = NULL;
            if (msprintf(&catvalue, "%s", catvalues[index]) == -1) break;
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
            SearchExtCat = SearchExtCats.Next(SearchExtCat);
            index++;
            free(catvalue);
        }
    }

    if (blacklistMode == blacklistsSelection && blacklists.Count() > 0) {
        cBlacklistObject *blacklistObj = blacklists.First();
        int index = 0;
        while (blacklistObj) {
            if (index == 0)
                msprintf(&tmp_blacklists, "%d", blacklistObj->blacklist->ID);
            else {
                char* temp = tmp_blacklists;
                msprintf(&tmp_blacklists, "%s|%d", tmp_blacklists, blacklistObj->blacklist->ID);
                free(temp);
            }
            blacklistObj = blacklists.Next(blacklistObj);
            index++;
        }
    }

    msprintf(&buffer, "%d:%s:%d:%s:%s:%d:%s:%d:%d:%d:%d:%d:%d:%s:%s:%d:%d:%d:%d:%s:%d:%d:%d:%d:%d:%d:%d:%s:%d:%d:%d:%d:%d:%ld:%d:%d:%d:%d:%d:%d:%s:%d:%d:%d:%d:%d:%d:%ld:%ld:%d:%d:%d:%s:%d",
             ID,
             tmp_search,
             useTime,
             tmp_Start,
             tmp_Stop,
             useChannel,
             (useChannel > 0 && useChannel < 3) ? *tmp_chanSel : "0",
             useCase,
             mode,
             useTitle,
             useSubtitle,
             useDescription,
             useDuration,
             tmp_minDuration,
             tmp_maxDuration,
             useAsSearchTimer,
             useDayOfWeek,
             DayOfWeek,
             useEpisode,
             tmp_directory,
             Priority,
             Lifetime,
             MarginStart,
             MarginStop,
             useVPS,
             action,
             useExtEPGInfo,
             useExtEPGInfo ? tmp_catvalues : "",
             avoidRepeats,
             allowedRepeats,
             compareTitle,
             compareSubtitle,
             compareSummary,
             catvaluesAvoidRepeat,
             repeatsWithinDays,
             delAfterDays,
             recordingsKeep,
             switchMinsBefore,
             pauseOnNrRecordings,
             blacklistMode,
             blacklists.Count() > 0 ? tmp_blacklists : "",
             fuzzyTolerance,
             useInFavorites,
             menuTemplate,
             delMode,
             delAfterCountRecs,
             delAfterDaysOfFirstRec,
             useAsSearchTimerFrom,
             useAsSearchTimerTil,
             ignoreMissingEPGCats,
             unmuteSoundOnSwitch,
             compareSummaryMatchInPercent,
             contentsFilter.c_str(),
             compareDate);

    if (tmp_search) free(tmp_search);
    if (tmp_directory) free(tmp_directory);
    if (tmp_catvalues) free(tmp_catvalues);
    if (tmp_blacklists) free(tmp_blacklists);
    if (tmp_contentsFilter) free(tmp_contentsFilter);

    return buffer;
}

bool cSearchExt::Parse(const char *s)
{
    char *line;
    char *pos;
    char *pos_next;
    int parameter = 1;
    int valuelen;
    char value[MaxFileName];
    bool disableSearchtimer = false;

    *directory = 0;
    *search = 0;

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
                    if (!isnumber(value)) return false;
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
                                LogFile.eSysLog("ERROR: channel '%s' not defined", channelMinbuffer);
                                channelMin = channelMax = NULL;
                                disableSearchtimer = true;
                                useChannel = 0;
                            }
                            if (channels == 1)
                                channelMax = channelMin;
                            else {
                                channelMax = Channels->GetByChannelID(tChannelID::FromString(channelMaxbuffer), true, true);
                                if (!channelMax) {
                                    LogFile.eSysLog("ERROR: channel '%s' not defined", channelMaxbuffer);
                                    channelMin = channelMax = NULL;
                                    disableSearchtimer = true;
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
                    useAsSearchTimer = atoi(value);
                    break;
                case 17:
                    useDayOfWeek = atoi(value);
                    break;
                case 18:
                    DayOfWeek = atoi(value);
                    break;
                case 19:
                    useEpisode = atoi(value);
                    break;
                case 20:
                    strcpy(directory, value);
                    break;
                case 21:
                    Priority = atoi(value);
                    break;
                case 22:
                    Lifetime = atoi(value);
                    break;
                case 23:
                    MarginStart = atoi(value);
                    break;
                case 24:
                    MarginStop = atoi(value);
                    break;
                case 25:
                    useVPS = atoi(value);
                    break;
                case 26:
                    action = atoi(value);
                    break;
                case 27:
                    useExtEPGInfo = atoi(value);
                    break;
                case 28:
                    if (!ParseExtEPGValues(value)) {
                        LogFile.eSysLog("ERROR reading ext. EPG values - 1");
                        free(line);
                        return false;
                    }
                    break;
                case 29:
                    avoidRepeats = atoi(value);
                    break;
                case 30:
                    allowedRepeats = atoi(value);
                    break;
                case 31:
                    compareTitle = atoi(value);
                    break;
                case 32:
                    compareSubtitle = atoi(value) > 0 ? 1 : 0;
                    break;
                case 33:
                    compareSummary = atoi(value);
                    break;
                case 34:
                    catvaluesAvoidRepeat = atol(value);
                    break;
                case 35:
                    repeatsWithinDays = atoi(value);
                    break;
                case 36:
                    delAfterDays = atoi(value);
                    break;
                case 37:
                    recordingsKeep = atoi(value);
                    break;
                case 38:
                    switchMinsBefore = atoi(value);
                    break;
                case 39:
                    pauseOnNrRecordings = atoi(value);
                    break;
                case 40:
                    blacklistMode = atoi(value);
                    break;
                case 41:
                    if (blacklistMode == blacklistsSelection && !ParseBlacklistIDs(value)) {
                        LogFile.eSysLog("ERROR parsing blacklist IDs");
                        free(line);
                        return false;
                    }
                    break;
                case 42:
                    fuzzyTolerance = atoi(value);
                    break;
                case 43:
                    useInFavorites = atoi(value);
                    break;
                case 44:
                    menuTemplate = atoi(value);
                    break;
                case 45:
                    delMode = atoi(value);
                    break;
                case 46:
                    delAfterCountRecs = atoi(value);
                    break;
                case 47:
                    delAfterDaysOfFirstRec = atoi(value);
                    break;
                case 48:
                    useAsSearchTimerFrom = atol(value);
                    break;
                case 49:
                    useAsSearchTimerTil = atol(value);
                    break;
                case 50:
                    ignoreMissingEPGCats = atoi(value);
                    break;
                case 51:
                    unmuteSoundOnSwitch = atoi(value);
                    break;
                case 52:
                    compareSummaryMatchInPercent = atoi(value);
                    break;
                case 53:
                    contentsFilter = value;
                    break;
                case 54:
                    compareDate = atoi(value);
                    break;
                default:
                    break;
                } //switch
            }
            parameter++;
        }
        if (*pos) pos++;
    } //while

    strreplace(directory, '|', ':');
    strreplace(search, '|', ':');
    strreplace(contentsFilter, "|", ":");

    while (strstr(search, "!^pipe^!"))
        strreplace(search, "!^pipe^!", "|");
    while (strstr(directory, "!^pipe^!"))
        strreplace(directory, "!^pipe^!", "|");
    strreplace(contentsFilter, "!^pipe^!", "|");

    if (disableSearchtimer && useAsSearchTimer) {
        useAsSearchTimer = false;
        LogFile.Log(1, "search timer '%s' disabled", search);
    }

    free(line);
    return (parameter >= 11) ? true : false;
}

char* cSearchExt::BuildFile(const cEvent* pEvent) const
{
    char* file = NULL;

    if (!pEvent)
        return file;

    const char *Subtitle = pEvent ? pEvent->ShortText() : NULL;
    char SubtitleBuffer[Utf8BufSize(MAX_SUBTITLE_LENGTH)];
    if (isempty(Subtitle)) {
        time_t Start = pEvent->StartTime();
        struct tm tm_r;
        strftime(SubtitleBuffer, sizeof(SubtitleBuffer), "%Y.%m.%d-%R-%a", localtime_r(&Start, &tm_r));
        Subtitle = SubtitleBuffer;
    } else if (Utf8StrLen(Subtitle) > MAX_SUBTITLE_LENGTH) {
        Utf8Strn0Cpy(SubtitleBuffer, Subtitle, sizeof(SubtitleBuffer));
        SubtitleBuffer[Utf8SymChars(SubtitleBuffer, MAX_SUBTITLE_LENGTH)] = 0;
        Subtitle = SubtitleBuffer;
    }

    if (useEpisode) {
        cString pFile = cString::sprintf("%s~%s", pEvent->Title(), Subtitle);
        if (file) free(file);
        file = strdup(pFile);
    } else if (pEvent->Title())
        file = strdup(pEvent->Title());

    if (!isempty(directory)) {
        char* pFile = NULL;

        cVarExpr varExprDir(directory);
        if (!varExprDir.DependsOnVar("%title%", pEvent) && !varExprDir.DependsOnVar("%subtitle%", pEvent))
            msprintf(&pFile, "%s~%s", directory, file ? file : "");
        else
            // ignore existing title and subtitle in file if already used as variables in directory
            msprintf(&pFile, "%s", directory);

        // parse the epxression and evaluate it
        cVarExpr varExprFile(pFile);
        if (pFile) free(pFile);
        pFile = strdup(varExprFile.Evaluate(pEvent).c_str());

        cVarExpr varExprSearchFile(pFile);
        if (pFile) free(pFile);
        pFile = strdup(varExprSearchFile.Evaluate(this).c_str());

        if (file) free(file);
        file = strdup(pFile);
        free(pFile);
    }
// replace some special chars
    if (file) {
        while (strstr(file, "|")) file = strreplace(file, "|", "!^pipe^!");
        while (strstr(file, ":")) file = strreplace(file, ':', '|');
        while (strstr(file, " ~")) file = strreplace(file, " ~", "~");
        while (strstr(file, "~ ")) file = strreplace(file, "~ ", "~");
    }
    return file;
}

bool cSearchExt::ParseBlacklistIDs(const char *s)
{
    char *line;
    char *pos;
    char *pos_next;
    int valuelen;
    char value[MaxFileName];

    cMutexLock BlacklistLock(&Blacklists);
    blacklists.Clear();

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
                cBlacklist* blacklist = Blacklists.GetBlacklistFromID(atoi(value));
                if (!blacklist)
                    LogFile.eSysLog("blacklist ID %s missing, will be skipped", value);
                else
                    blacklists.Add(new cBlacklistObject(blacklist));
            }
        }
        if (*pos) pos++;
    } //while

    free(line);
    return true;
}

bool cSearchExt::ParseExtEPGValues(const char *s)
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

bool cSearchExt::ParseExtEPGEntry(const char *s)
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
                    if (index > -1 && index < SearchExtCats.Count())
                        strcpy(catvalues[index], "");
                }
                break;
                case 2:
                    if (currentid > -1) {
                        int index = SearchExtCats.GetIndexFromID(currentid);
                        if (index > -1 && index < SearchExtCats.Count()) {
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

bool cSearchExt::Save(FILE *f)
{
    return fprintf(f, "%s\n", ToText()) > 0;
}

const cEvent * cSearchExt::GetEventBySearchExt(const cSchedule *schedules, const cEvent *Start, bool inspectTimerMargin)
{
    if (!schedules) return NULL;

    const cEvent *pe = NULL;
    const cEvent *p1 = NULL;

    const cList<cEvent>* Events = schedules->Events();
    if (Start)
        p1 = Events->Next(Start);
    else
        p1 = Events->First();

    time_t tNow = time(NULL);
    char* searchText = strdup(search);

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

    if (!useCase)
        ToLower(searchText);

    for (const cEvent *p = p1; p; p = Events->Next(p)) {
        if (!p) {
            break;
        }

        if (skipRunningEvents && tNow > p->StartTime())
            continue;

        // ignore events without title
        if (!p->Title() || !*p->Title())
            continue;

        if (tNow < p->EndTime() + (inspectTimerMargin ? (MarginStop * 60) : 0)) {
            if (useTime) {
                time_t tEvent = p->StartTime();
                struct tm tmEvent;
                localtime_r(&tEvent, &tmEvent);

                int eventStart = tmEvent.tm_hour * 100 + tmEvent.tm_min;
                int eventStart2 = eventStart + 2400;
                if ((eventStart < searchStart || eventStart > searchStop) &&
                    (eventStart2 < searchStart || eventStart2 > searchStop))
                    continue;

                if (useDayOfWeek) {
                    if (DayOfWeek >= 0) {
                        if ((DayOfWeek != tmEvent.tm_wday || (DayOfWeek == tmEvent.tm_wday && eventStart < searchStart)) &&
                            (!((DayOfWeek + 1) % 7 == tmEvent.tm_wday && eventStart2 < searchStop)))
                            continue;
                    } else {
                        int iFound = 0;
                        for (int i = 0; i < 7; i++) {
                            if ((abs(DayOfWeek) & (int)pow(2, i)) && ((i == tmEvent.tm_wday && eventStart >= searchStart) ||
                                                                      ((i + 1) % 7 == tmEvent.tm_wday && eventStart2 < searchStop))) {
                                iFound = 1;
                                break;
                            }
                        }
                        if (!iFound)
                            continue;
                    }
                }
            }
            if (useDuration) {
                int duration = p->Duration() / 60;
                if (minSearchDuration > duration || maxSearchDuration < duration)
                    continue;
            }

            if (!useTime && useDayOfWeek) {
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

            char* szTest = NULL;
            msprintf(&szTest, "%s%s%s%s%s", (useTitle ? (p->Title() ? p->Title() : "") : ""), (useSubtitle || useDescription) ? "~" : "",
                         (useSubtitle ? (p->ShortText() ? p->ShortText() : "") : ""), useDescription ? "~" : "",
                         (useDescription ? (p->Description() ? p->Description() : "") : ""));

            if (!useCase)
                ToLower(szTest);

            if (szTest && *szTest) {
                if (!MatchesSearchMode(szTest, searchText, mode, " ,;|~", fuzzyTolerance)) {
                    free(szTest);
                    continue;
                }
            }
            if (szTest)
                free(szTest);

            if (contentsFilter.size() > 0 && !MatchesContentsFilter(p))
                continue;

            if (useExtEPGInfo && !MatchesExtEPGInfo(p))
                continue;
            pe = p;
            break;
        }
    }
    free(searchText);
    return pe;
}

// returns a pointer array to the matching search results
cSearchResults* cSearchExt::Run(int PayTVMode, bool inspectTimerMargin, int evalLimitMins, cSearchResults* pPrevResults, bool suppressRepeatCheck)
{
    LogFile.Log(3, "start search for search timer '%s'", search);

    bool noPayTV = false;
    if (PayTVMode == -1) // use search's setting
        noPayTV = (useChannel == 3);
    else
        noPayTV = (PayTVMode == 1);

    time_t tNow = time(NULL);
    cSearchResults* pSearchResults = pPrevResults;
    cSearchResults* pBlacklistResults = GetBlacklistEvents(inspectTimerMargin ? MarginStop : 0);

    {
        LOCK_CHANNELS_READ;
        LOCK_SCHEDULES_READ;

        int counter = 0;
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

            if (useChannel == 3 && noPayTV) {
                if (channel->Ca() >= CA_ENCRYPTED_MIN) {
                    Schedule = Schedules->Next(Schedule);
                    continue;
                }
            }

            if (noPayTV) { // no paytv
                if (channel->Ca() >= CA_ENCRYPTED_MIN) {
                    Schedule = Schedules->Next(Schedule);
                    continue;
                }
            }

            const cEvent *pPrevEvent = NULL;
            do {
                const cEvent* event = GetEventBySearchExt(Schedule, pPrevEvent, inspectTimerMargin);
                pPrevEvent = event;
                if (evalLimitMins && event) { // limit evaluation to now + limit
                    if (tNow + evalLimitMins * 60 <= event->EndTime())
                        break;
                }
                if (event && Channels->GetByChannelID(event->ChannelID(), true, true)) {
                    if (pBlacklistResults && pBlacklistResults->Lookup(event)) {
                        LogFile.Log(3, "skip '%s~%s' (%s - %s, channel %d): matches blacklist", event->Title() ? event->Title() : "no title", event->ShortText() ? event->ShortText() : "no subtitle", GETDATESTRING(event), GETTIMESTRING(event), ChannelNrFromEvent(event));
                        continue;
                    }
                    if (!pSearchResults) pSearchResults = new cSearchResults;
                    pSearchResults->Add(new cSearchResult(event, this));
                    counter++;
                }
            } while (pPrevEvent);
            Schedule = Schedules->Next(Schedule);
        }
        LogFile.Log(3, "found %d event(s) for search timer '%s'", counter, search);
    } // Give up locks

    if (pBlacklistResults) delete pBlacklistResults;

    if (useAsSearchTimer && avoidRepeats && pSearchResults && !suppressRepeatCheck) {
        pSearchResults->SortBy(CompareEventTime); // sort before checking repeats to make sure the first event is selected
        CheckRepeatTimers(pSearchResults);
    }

    skipRunningEvents = false;
    return pSearchResults;
}

cSearchResults* cSearchExt::GetBlacklistEvents(int MarginStop)
{
    if (blacklistMode == blacklistsNone) return NULL;

    cMutexLock BlacklistLock(&Blacklists);
    cSearchResults* blacklistEvents = NULL;
    if (blacklistMode == blacklistsOnlyGlobal) {
        cBlacklist* tmpblacklist = Blacklists.First();
        while (tmpblacklist) {
            if (tmpblacklist->isGlobal)
                blacklistEvents = tmpblacklist->Run(blacklistEvents, MarginStop);
            tmpblacklist = Blacklists.Next(tmpblacklist);
        }
    }
    if (blacklistMode == blacklistsAll) {
        cBlacklist* tmpblacklist = Blacklists.First();
        while (tmpblacklist) {
            blacklistEvents = tmpblacklist->Run(blacklistEvents, MarginStop);
            tmpblacklist = Blacklists.Next(tmpblacklist);
        }
    }
    if (blacklistMode == blacklistsSelection) {
        cBlacklistObject* tmpblacklistObj = blacklists.First();
        while (tmpblacklistObj) {
            blacklistEvents = tmpblacklistObj->blacklist->Run(blacklistEvents, MarginStop);
            tmpblacklistObj = blacklists.Next(tmpblacklistObj);
        }
    }
    return blacklistEvents;

}

void cSearchExt::CheckRepeatTimers(cSearchResults* pResults)
{
    if (!pResults)
        return;
    if (avoidRepeats == 0)
        return;

    LogFile.Log(2, "analysing repeats for search timer '%s'...", search);
    if ((action != searchTimerActionRecord) && (action != searchTimerActionInactiveRecord)) {
        LogFile.Log(3, "search timer not set to 'record', so skip all");
        return;
    }

    cSearchResult* pResultObj = NULL;
    for (pResultObj = pResults->First(); pResultObj; pResultObj = pResults->Next(pResultObj)) {
        if ((action != searchTimerActionRecord) && (action != searchTimerActionInactiveRecord)) { // only announce if there is no timer for the event
            pResultObj->needsTimer = false;
            continue;
        }

        const cEvent* pEvent = pResultObj->event;
        // check if this event was already recorded
        int records = 0;
        cRecDone* firstRec = NULL;
        LogFile.Log(3, "get count recordings with %d%% match", compareSummaryMatchInPercent);
        records = RecsDone.GetCountRecordings(pEvent, this, &firstRec, compareSummaryMatchInPercent);
        LogFile.Log(3, "recordings: %d", records);

        if (records > allowedRepeats) { // already recorded
            LogFile.Log(3, "skip '%s~%s' (%s - %s, channel %d): already recorded %d equal event(s)", pEvent->Title() ? pEvent->Title() : "no title", pEvent->ShortText() ? pEvent->ShortText() : "no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent), records);
            pResultObj->needsTimer = false; // first assume we need no timer
            continue;
        }

        int plannedTimers = 0;
        LogFile.Log(3, "get planned recordings");
        cSearchResult* pFirstResultMatching = NULL;
        // check other results, if they are already planned for equal events
        for (cSearchResult* pResultObjP = pResults->First(); pResultObjP; pResultObjP = pResults->Next(pResultObjP)) {
            if (pResultObj == pResultObjP) break;

            const cEvent* pEventP = pResultObjP->event;
            if (!pEventP) continue;

            if (!pResultObjP->needsTimer) continue;

            if (EventsMatch(pEvent, pEventP, compareTitle, compareSubtitle, compareSummary, compareDate, catvaluesAvoidRepeat, compareSummaryMatchInPercent)) {
                if (!pFirstResultMatching) pFirstResultMatching = pResultObjP;
                plannedTimers++;
            }
        }
        LogFile.Log(3, "planned: %d", plannedTimers);

        if (plannedTimers + records > allowedRepeats) {
            LogFile.Log(3, "skip '%s~%s' (%s - %s, channel %d): events planned(%d), recorded(%d), allowed(%d)", pEvent->Title() ? pEvent->Title() : "no title", pEvent->ShortText() ? pEvent->ShortText() : "no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent), plannedTimers, records, allowedRepeats);
            pResultObj->needsTimer = false;
            continue;
        } else if (allowedRepeats > 0 && repeatsWithinDays > 0) { // if we only allow repeats with in a given range
            if (firstRec) { // already recorded, check for allowed repeat within days
                if (firstRec->startTime > pEvent->StartTime() - pEvent->Duration()) { // no repeat
                    LogFile.Log(3, "skip '%s~%s' (%s - %s, channel %d); no repeat for event already recorded at %s, channel %d", pEvent->Title() ? pEvent->Title() : "no title", pEvent->ShortText() ? pEvent->ShortText() : "no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent), DAYDATETIME(firstRec->startTime), firstRec->ChannelNr());
                    pResultObj->needsTimer = false;
                    continue;
                }
                int daysFromFirstRec = int(double((pEvent->StartTime() - firstRec->startTime)) / (60 * 60 * 24) + 0.5);
                if (daysFromFirstRec  > repeatsWithinDays) {
                    LogFile.Log(3, "skip '%s~%s' (%s - %s, channel %d); first recording at %s is %d days before, limit is %d days", pEvent->Title() ? pEvent->Title() : "no title", pEvent->ShortText() ? pEvent->ShortText() : "no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent), DAYDATETIME(firstRec->startTime), daysFromFirstRec, repeatsWithinDays);
                    pResultObj->needsTimer = false;
                    continue;
                }
            }
            if (plannedTimers > 0 && pFirstResultMatching) {
                const cEvent* pFirst = pFirstResultMatching->event;
                if (pFirst->StartTime() > pEvent->StartTime() - pEvent->Duration()) { // no repeat
                    LogFile.Log(3, "skip '%s~%s' (%s - %s, channel %d); no repeat for event already recorded at %s - %s, channel %d", pEvent->Title() ? pEvent->Title() : "no title", pEvent->ShortText() ? pEvent->ShortText() : "no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent), GETDATESTRING(pFirst), GETTIMESTRING(pFirst), ChannelNrFromEvent(pFirst));
                    pResultObj->needsTimer = false;
                    continue;
                }

                int daysBetween = int(double((pEvent->StartTime() - pFirst->StartTime())) / (60 * 60 * 24) + 0.5);
                if (daysBetween  > repeatsWithinDays) {
                    LogFile.Log(3, "skip '%s~%s' (%s - %s, channel %d); first event '%s~%s' (%s - %s) is %d days before, limit is %d days", pEvent->Title() ? pEvent->Title() : "no title", pEvent->ShortText() ? pEvent->ShortText() : "no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent), GETDATESTRING(pFirst), GETTIMESTRING(pFirst), daysBetween, repeatsWithinDays);
                    pResultObj->needsTimer = false;
                    continue;
                }
            }
        }
        bool dummy;
        const cTimer* timer = cSearchTimerThread::GetTimer(this, pEvent, dummy);
        if (timer && !timer->HasFlags(tfActive)) {
            LogFile.Log(3, "skip '%s~%s' (%s - %s, channel %d), existing timer disabled", pEvent->Title() ? pEvent->Title() : "no title", pEvent->ShortText() ? pEvent->ShortText() : "no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent));
            pResultObj->needsTimer = false;
            continue;
        } else
            LogFile.Log(3, "*** planning event '%s~%s' (%s - %s, channel %d) for recording", pEvent->Title() ? pEvent->Title() : "no title", pEvent->ShortText() ? pEvent->ShortText() : "no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent));
    }
    int needsTimer = 0;
    for (pResultObj = pResults->First(); pResultObj; pResultObj = pResults->Next(pResultObj))
        if (pResultObj->needsTimer) needsTimer++;

    LogFile.Log(2, "%d/%d events need a timer for search timer '%s'", needsTimer, pResults->Count(), search);
}

void cSearchExt::CheckExistingRecordings(cSearchResults* pResults)
{
    if (!pResults)
        return;

    LogFile.Log(3, "analysing existing recordings for search timer '%s'...", search);

    // how many recordings do we already have?
    int num = GetCountRecordings();

    cSearchResult* pResultObj = NULL;
    int remain = pauseOnNrRecordings - num;
    for (pResultObj = pResults->First(); pResultObj; pResultObj = pResults->Next(pResultObj), remain--) {
        if (!pResultObj->needsTimer) {
            remain++;
            continue; // maybe already disabled because of done feature
        }
        pResultObj->needsTimer = (remain > 0);
        if (remain <= 0) {
            const cEvent* pEvent = pResultObj->event;
            LogFile.Log(3, "skip '%s~%s' (%s - %s, channel %d): only %d recordings are allowed", pEvent->Title() ? pEvent->Title() : "no title", pEvent->ShortText() ? pEvent->ShortText() : "no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent), pauseOnNrRecordings);
        }
    }
}

bool cSearchExt::MatchesExtEPGInfo(const cEvent* e)
{
    if (!e || !e->Description())
        return false;
    cSearchExtCat* SearchExtCat = SearchExtCats.First();
    while (SearchExtCat) {
        char* value = NULL;
        int index = SearchExtCats.GetIndexFromID(SearchExtCat->id);
        if (index > -1)
            value = catvalues[index];
        if (value && SearchExtCat->searchmode >= 10 && atol(value) == 0) // numerical value != 0 ?
            value = NULL;
        if (value && *value) {
            char* testvalue = GetExtEPGValue(e, SearchExtCat);
            if (!testvalue)
                return (ignoreMissingEPGCats ? true : false);

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

void cSearchExt::OnOffTimers(bool bOn)
{
    LOCK_TIMERS_WRITE;
    for (cTimer *ti = Timers->First(); ti; ti = Timers->Next(ti)) {
        if (((!bOn && ti->HasFlags(tfActive)) || (bOn && !ti->HasFlags(tfActive))) && TriggeredFromSearchTimerID(ti) == ID)
            ti->OnOff();
    }
}

void cSearchExt::DeleteAllTimers()
{
    cList<cTimerObj> DelTimers;
    LOCK_TIMERS_WRITE;
    Timers->SetExplicitModify();
    cTimer *ti = Timers->First();
    while (ti) {
        if (!ti->Recording() && TriggeredFromSearchTimerID(ti) == ID) {
            cTimer* tiNext = Timers->Next(ti);
            LogFile.iSysLog("deleting timer %s", *ti->ToDescr());
            Timers->Del(ti);
            Timers->SetModified();
            ti = tiNext;
        } else
            ti = Timers->Next(ti);
    };
}

cTimerObjList* cSearchExt::GetTimerList(cTimerObjList* timerList)
{
    if (!timerList)
        timerList = new cTimerObjList;

    LOCK_TIMERS_READ;
    for (const cTimer *ti = Timers->First(); ti; ti = Timers->Next(ti)) {
        if (TriggeredFromSearchTimerID(ti) == ID) {
            // check if already in list
            bool found = false;
            for (cTimerObj *tObj = timerList->First(); tObj; tObj = timerList->Next(tObj)) {
                if (tObj->timer == ti) {
                    found = true;
                    break;
                }
            }
            if (!found)
                timerList->Add(new cTimerObj(ti));
        }
    }
    return timerList;
}

// counts the currently existent recordings triggered by this search timer
int cSearchExt::GetCountRecordings()
{
    int countRecs = 0;

    LOCK_RECORDINGS_READ;
    for (const cRecording *recording = Recordings->First(); recording; recording = Recordings->Next(recording)) {
        if (recording->IsEdited()) continue; // ignore recordings edited
        if (!recording->Info()) continue;
        char* searchID = GetAuxValue(recording, "s-id");

        if (!searchID) continue;
        if (ID == atoi(searchID))
            countRecs++;
        free(searchID);
    }
    LogFile.Log(3, "found %d recordings for search '%s'", countRecs, search);
    return countRecs;
}

bool cSearchExt::IsActiveAt(time_t t)
{
    if (useAsSearchTimer == 0) return false;
    if (useAsSearchTimer == 2) {
        if (useAsSearchTimerFrom > 0 && t < useAsSearchTimerFrom) return false;
        if (useAsSearchTimerTil > 0 && t > useAsSearchTimerTil) return false;
    }
    return true;
}

bool cSearchExt::HasContent(int contentID)
{
    for (unsigned int i = 0; i < contentsFilter.size(); i += 2) {
        std::string hexContentID = contentsFilter.substr(i, 2);
        if (hexContentID.size() != 2) return false;
        std::istringstream iss(hexContentID);
        int tmpContentID = 0;
        if (!(iss >> std::noshowbase >> std::hex >> tmpContentID)) return false;
        if (contentID == tmpContentID) return true;
    }
    return false;
}

void cSearchExt::SetContentFilter(int* contentStringsFlags)
{
    // create the hex array of content descriptor IDs
    string tmp;
    contentsFilter = "";
    for (unsigned int i = 0; contentStringsFlags && i <= CONTENT_DESCRIPTOR_MAX; i++) {
        if (contentStringsFlags[i]) {
            std::ostringstream oss;
            oss << std::hex << std::noshowbase << i;
            contentsFilter += oss.str();
        }
    }
}

bool cSearchExt::MatchesContentsFilter(const cEvent* e)
{
    if (!e) return false;
    // check if each content filter ID is contained in the events descriptors
    for (unsigned int i = 0; i < contentsFilter.size(); i += 2) {
        std::string hexContentID = contentsFilter.substr(i, 2);
        if (hexContentID.size() != 2) return false;
        std::istringstream iss(hexContentID);
        int searchContentID = 0;
        if (!(iss >> std::hex >> searchContentID)) return false;
        int c = 0, eventContentID = 0;
        bool found = false;
        while ((eventContentID = e->Contents(c++)) > 0)
            if (eventContentID == searchContentID) {
                found = true;
                break;
            }
        if (!found) return false;
    }
    return true;
}

// -- cSearchExts ----------------------------------------------------------------
bool cSearchExts::Load(const char *FileName)
{
    cMutexLock SearchExtsLock(this);
    Clear();
    if (FileName) {
        free(fileName);
        fileName = strdup(FileName);
    }

    bool result = true;
    if (fileName && access(fileName, F_OK) == 0) {
        LogFile.iSysLog("loading %s", fileName);
        FILE *f = fopen(fileName, "r");
        if (f) {
            int line = 0;
            char buffer[MAXPARSEBUFFER];
            result = true;
            while (fgets(buffer, sizeof(buffer), f) > 0) {
                line++;
                char *p = strchr(buffer, '#');
                if (p == buffer) *p = 0;

                stripspace(buffer);
                if (!isempty(buffer)) {
                    cSearchExt* search = new cSearchExt;
                    if (search->Parse(buffer))
                        Add(search);
                    else {
                        LogFile.eSysLog("error in '%s', line %d\n", fileName, line);
                        delete search;
                        result = false;
                        break;
                    }
                }
            }
            fclose(f);
        } else {
            LOG_ERROR_STR(fileName);
            result = false;
        }
    }

    if (!result)
        fprintf(stderr, "vdr: error while reading '%s'\n", fileName);
    LogFile.Log(2, "loaded searches from %s (count: %d)", fileName, Count());
    return result;
}

int cSearchExts::GetNewID()
{
    cMutexLock SearchExtsLock(this);
    int newID = -1;
    cSearchExt *l = (cSearchExt *)First();
    while (l) {
        newID = std::max(newID, l->ID);
        l = (cSearchExt *)l->Next();
    }
    return newID + 1;
}

void cSearchExts::Update(void)
{
    cMutexLock SearchExtsLock(this);
    cSearchExt *l = (cSearchExt *)First();
    while (l) {
        // check if ID is set
        if (l->ID == -1)
            l->ID = GetNewID();
        l = (cSearchExt *)l->Next();
    }
}

bool cSearchExts::Save(void)
{
    cMutexLock SearchExtsLock(this);
    bool result = true;
    cSearchExt *l = (cSearchExt *)this->First();
    cSafeFile f(fileName);
    if (f.Open()) {
        while (l) {
            if (!l->Save(f)) {
                result = false;
                break;
            }
            l = (cSearchExt *)l->Next();
        }
        if (!f.Close())
            result = false;
    } else
        result = false;
    return result;
}

cSearchExt* cSearchExts::GetSearchFromID(int ID)
{
    if (ID == -1)
        return NULL;
    cMutexLock SearchExtsLock(this);
    cSearchExt *l = (cSearchExt *)First();
    while (l) {
        if (l->ID == ID)
            return l;
        l = (cSearchExt *)l->Next();
    }
    return NULL;
}

void cSearchExts::RemoveBlacklistID(int ID)
{
    bool changed = false;
    cMutexLock SearchExtsLock(this);
    cSearchExt *l = (cSearchExt *)First();
    while (l) {
        cBlacklistObject* blacklistObj = l->blacklists.First();
        while (blacklistObj) {
            cBlacklistObject* blacklistObjNext = l->blacklists.Next(blacklistObj);
            if (blacklistObj->blacklist->ID == ID) {
                l->blacklists.Del(blacklistObj);
                changed = true;
            }
            blacklistObj = blacklistObjNext;
        }
        l = (cSearchExt *)l->Next();
    }
    if (changed)
        Save();
}

bool cSearchExts::Exists(const cSearchExt* SearchExt)
{
    cMutexLock SearchExtsLock(this);
    cSearchExt *l = (cSearchExt *)First();
    while (l) {
        if (l == SearchExt)
            return true;
        l = (cSearchExt *)l->Next();
    }
    return false;
}

cSearchExts* cSearchExts::Clone()
{
    cSearchExts* clonedList = new cSearchExts();

    cMutexLock SearchExtsLock(this);
    cSearchExt *l = (cSearchExt *)First();
    while (l) {
        cSearchExt* clone = new cSearchExt();
        *clone = *l;
        clonedList->Add(clone);
        l = (cSearchExt *)l->Next();
    }
    return clonedList;
}

bool cSearchExts::CheckForAutoDelete(cSearchExt* SearchExt)
{
    if (!SearchExt || SearchExt->delMode == 0) return false;

    cRecDone* firstRec = NULL;
    bool delSearch = false;
    int recs = RecsDone.GetTotalCountRecordings(SearchExt, &firstRec);
    if (SearchExt->delMode == 1 && SearchExt->delAfterCountRecs > 0)
        delSearch = recs >= SearchExt->delAfterCountRecs;
    if (SearchExt->delMode == 2 && SearchExt->delAfterDaysOfFirstRec && firstRec)
        delSearch = (time(NULL) - firstRec->startTime) > SearchExt->delAfterDaysOfFirstRec * 24 * 60 * 60;
    if (delSearch) {
        int DelID = SearchExt->ID;
        LogFile.Log(1, "auto deleting search '%s' (ID: %d)", SearchExt->search, DelID);
        cMutexLock SearchExtsLock(&SearchExts);
        SearchExts.Del(SearchExt);
        SearchExts.Save();
        RecsDone.RemoveSearchID(DelID);
    }
    return delSearch;
}

void cSearchExts::SortBy(int(*compar)(const void *, const void *))
{
    int n = Count();
    cListObject *a[n];
    cListObject *object = objects;
    int i = 0;
    while (object && i < n) {
        a[i++] = object;
        object = object->Next();
    }
    qsort(a, n, sizeof(cListObject *), compar);
    objects = lastObject = NULL;
    for (i = 0; i < n; i++) {
        a[i]->Unlink();
        count--;
        Add(a[i]);
    }
}

cSearchResult::cSearchResult(const cEvent* Event, int searchID) : event(Event), blacklist(NULL), needsTimer(true)
{
    search = SearchExts.GetSearchFromID(searchID);
}
