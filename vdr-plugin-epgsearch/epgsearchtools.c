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

#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>
#include <ctype.h>
#include <netdb.h>
#include <arpa/inet.h>
#ifdef __FreeBSD__
#include <netinet/in.h>
#endif
#include "uservars.h"
#include "epgsearchtools.h"
#include "epgsearchext.h"
#include "epgsearchcats.h"
#include "epgsearchcfg.h"
#include "svdrpclient.h"
#include "distance.h"
#include "md5.h"
#include "afuzzy.h"
#include "timerstatus.h"
#include <langinfo.h>

#ifdef HAVE_PCREPOSIX
#include <pcreposix.h>
#elif defined(HAVE_LIBTRE)
#include <tre/regex.h>
#else
#include <regex.h>
#endif

const char AllowedChars[] = trNOOP("$ abcdefghijklmnopqrstuvwxyz0123456789-.,#~\\^$[]|()*+?{}/:%@&_");
extern bool isUTF8;

int CompareEventTime(const void *p1, const void *p2)
{
    time_t time1 = (*(cSearchResult **)p1)->event->StartTime();
    time_t time2 = (*(cSearchResult **)p2)->event->StartTime();
    if (time1 == time2)
        return (int)(ChannelNrFromEvent((*(cSearchResult **)p1)->event) -
                     ChannelNrFromEvent((*(cSearchResult **)p2)->event));
    else
        return (int)(time1 - time2);
}

int CompareEventChannel(const void *p1, const void *p2)
{
    int ch1 = ChannelNrFromEvent((*(cSearchResult **)p1)->event);
    int ch2 = ChannelNrFromEvent((*(cSearchResult **)p2)->event);
    if (ch1 == ch2)
        return (int)((*(cSearchResult **)p1)->event->StartTime() -
                     (*(cSearchResult **)p2)->event->StartTime());
    else
        return ch1 - ch2;
}

int CompareSearchExtPrioDescTerm(const void *p1, const void *p2)
{
    int prio1 = (*(cSearchExt **)p1)->Priority;
    int prio2 = (*(cSearchExt **)p2)->Priority;
    if (prio2 != prio1)
        return prio2 - prio1;
    else
        return strcmp((*(cSearchExt **)p1)->search, (*(cSearchExt **)p2)->search);
}

cString IndentMenuItem(const char* szString, int indentions)
{
    char* szIndented = NULL;
    msprintf(&szIndented, "%*s", strlen(szString) + indentions * 2, szString);
    cString szIndentedStr(szIndented, true /*take pointer*/);
    return szIndentedStr;
}

bool MatchesSearchMode(const char* szTest, const char* searchText, int mode, const char* delim, int tolerance)
{
    if (szTest && *szTest) {
        if (mode == 0) // substring
            return (strstr(szTest, searchText) != NULL);
        else if (mode == 1 || mode == 2) { // AND or OR
            bool bTesting = false;
            char *pstrSearchToken, *pptr;
            bool bFirst = true;
            char *pstrSearch = strdup(searchText);
            pstrSearchToken = strtok_r(pstrSearch, delim, &pptr);
            while (pstrSearchToken) {
                if (szTest && strstr(szTest, skipspace(pstrSearchToken))) {
                    if (mode == 1) {
                        // means AND
                        if (bFirst) {
                            bTesting = true;
                            bFirst = false;
                        } else
                            bTesting &= true;
                    } else
                        bTesting |= true;
                } else {
                    // not found!!
                    if (mode == 1) {
                        // means AND
                        bTesting = false;
                        bFirst = false;
                    }
                }
                pstrSearchToken = strtok_r(NULL, delim, &pptr);
            }
            free(pstrSearch);
            return bTesting;
        } else if (mode == 3) { // match exactly
            if (strcmp(szTest, searchText) == 0)
                return true;
            else
                return false;
        } else if (mode == 4) { // regexp
            regex_t re;

            if (0 == regcomp(&re, searchText, REG_EXTENDED | REG_NOSUB)) {
                int status = regexec(&re, szTest, 0, NULL, 0);
                regfree(&re);
                return (status == 0);
            }
            return false;
        } else if (mode == 5) { // fuzzy
            AFUZZY af = { NULL, NULL, NULL, NULL, NULL, NULL, { 0 }, { 0 }, 0, 0, 0, 0, 0, 0 };
            string query = searchText ? searchText : "";
            if (query.size() > 32) query = query.substr(0, 32);
            afuzzy_init(query.c_str(), tolerance, 0, &af);
            /* Checking substring */
            int res = afuzzy_checkSUB(szTest, &af);
            afuzzy_free(&af);
            return (res > 0);
        } else if (mode >= 10 && mode <= 15) {
            int testvalue = atoi(szTest);
            int value = atoi(searchText);
            if (value == 0) return true;

            if (mode == 10) // less
                return testvalue < value;
            else if (mode == 11) // less or equal
                return testvalue <= value;
            else if (mode == 12) // greater
                return testvalue > value;
            else if (mode == 13) // greater or equal
                return testvalue >= value;
            else if (mode == 14) // equal
                return testvalue == value;
            else if (mode == 15) // not equal
                return testvalue != value;
        }
    }
    return false;
}

void ToLower(char* szText)
{
    if (!szText)
        return;

    if (!isUTF8) {
        for (int loop = 0; szText[loop] != 0; loop++)
            szText[loop] = tolower(szText[loop]);
        return;
    } else {
        int length = strlen(szText) + 1;
        uint* valueUtf8 = new uint[length];
        int lengthUtf8 = Utf8ToArray(szText, valueUtf8, length);
        for (int i = 0; i < lengthUtf8; i++)
            valueUtf8[i] = Utf8to(lower, valueUtf8[i]);
        Utf8FromArray(valueUtf8, szText, length);
        delete [] valueUtf8;
    }
}

char* GetExtEPGValue(const cEvent* e, cSearchExtCat* SearchExtCat)
{
    if (!e || !SearchExtCat)
        return NULL;
    return GetExtEPGValue(e->Description(), SearchExtCat->name, SearchExtCat->format);
}

char* GetExtEPGValue(const char* description, const char* catname, const char *format)
{
    if (isempty(description))
        return NULL;
    char* tmp1 = NULL;
    char* tmp2 = NULL;

    // search the category, must be the first line or at the beginning of a line
    if (msprintf(&tmp1, "\n%s: ", catname) == -1 ||
        msprintf(&tmp2, "%s: ", catname) == -1)
        return NULL;
    char* descr = strdup(description);
    char* cat = NULL;
    int valueOffset = 0;
    if ((cat = strstr(descr, tmp1)) != NULL) {
        cat++; // skip linefeed
        valueOffset = strlen(tmp1);
    } else if (strstr(descr, tmp2) == descr) { // in first line
        cat = descr;
        valueOffset = strlen(tmp2) + 1;
    } else {
        free(descr);
        free(tmp1);
        free(tmp2);
        return NULL;
    }

    // search the value to appear before the next line feed or end
    char* end = strchr(cat, '\n');
    int endpos = strlen(cat);
    if (end)
        endpos = end - cat;
    cat[endpos] = 0;

    char* value = NULL;
    msprintf(&value, "%s", cat + valueOffset - 1);
    if (format) {
        int ivalue;
        if (sscanf(value, "%8i", &ivalue) == 1) {
            free(value);
            value = NULL;
            msprintf(&value, format, ivalue);
        }
    }
    free(descr);
    free(tmp1);
    free(tmp2);

    return value;
}

char* GetAuxValue(const char* aux, const char* name)
{
    if (isempty(aux))
        return NULL;

    char* descr = strdup(aux);
    char* beginaux = strstr(descr, "<epgsearch>");
    char* endaux = strstr(descr, "</epgsearch>");
    if (!beginaux || !endaux) {
        free(descr);
        return NULL;
    }

    beginaux +=  11;  // strlen("<epgsearch>");
    endaux[0] = 0;
    memmove(descr, beginaux, endaux - beginaux + 1);

    if (strcmp(name, "epgsearch") == 0)
        return descr; // full aux

    int namelen = strlen(name);
    char catname[100] = "";
    catname[0] = '<';
    memcpy(catname + 1, name, namelen);
    catname[1 + namelen] = '>';
    catname[2 + namelen] = 0;

    char* cat = strcasestr(descr, catname);
    if (!cat) {
        free(descr);
        return NULL;
    }

    cat += namelen + 2;
    char* end = strstr(cat, "</");
    if (!end) {
        free(descr);
        return NULL;
    }
    end[0] = 0;

    int catlen = end - cat + 1;
    char* value = (char *) malloc(catlen);
    memcpy(value, cat, catlen);

    free(descr);
    return value;
}

char* GetAuxValue(const cRecording *recording, const char* name)
{
    if (!recording || !recording->Info()) return NULL;
    return GetAuxValue(recording->Info()->Aux(), name);
}

char* GetAuxValue(const cTimer *timer, const char* name)
{
    if (!timer || !timer->Aux()) return NULL;
    return GetAuxValue(timer->Aux(), name);
}

string UpdateAuxValue(string aux, string section, long num)
{
    return UpdateAuxValue(aux, section, NumToString(num));
}

string UpdateAuxValue(string aux, string section, string value)
{
    string secStart = "<" + section + ">";
    string secEnd = "</" + section + ">";
    int valueStartPos = aux.find(secStart);
    int valueEndPos = aux.find(secEnd);
    if (valueStartPos >= 0 && valueEndPos >= 0)
        aux.replace(valueStartPos + secStart.size(), valueEndPos - valueStartPos - secStart.size(), value);
    else
        aux += secStart + value + secEnd;
    return aux;
}

// replace s1 with s2 in s ignoring the case of s1
char *strreplacei(char *s, const char *s1, const char *s2)
{
    char *p = strcasestr(s, s1);
    if (p) {
        int of = p - s;
        int l = strlen(s);
        int l1 = strlen(s1);
        int l2 = 0;
        if (s2)
            l2 = strlen(s2);
        if (l2 > l1)
            s = (char *)realloc(s, l + l2 - l1 + 1);
        if (l2 != l1)
            memmove(s + of + l2, s + of + l1, l - of - l1 + 1);
        memcpy(s + of, s2, l2);
    }
    return s;
}

std::string strreplace(
    std::string& result,
    const std::string& replaceWhat,
    const std::string& replaceWithWhat)
{
    while (1) {
        const int pos = result.find(replaceWhat);
        if (pos == -1) break;
        result.replace(pos, replaceWhat.size(), replaceWithWhat);
    }
    return result;
}


void sleepMSec(long ms)
{
    cCondWait::SleepMs(ms);
}

void sleepSec(long s)
{
    sleepMSec(s * 1000);
}

bool SendViaSVDRP(cString SVDRPcmd)
{
    bool bSuccess = true;
    cString cmdbuf;
    if (EPGSearchConfig.useExternalSVDRP) {
        cmdbuf = cString::sprintf("%s -p %d \"%s\"",
                                  epgsSVDRP::cSVDRPClient::SVDRPSendCmd,
                                  EPGSearchConfig.SVDRPPort,
                                  *SVDRPcmd);

        FILE *p = popen(cmdbuf, "r");
        if (p)
            pclose(p);
        else {
            LogFile.eSysLog("can't open pipe for command '%s'", *cmdbuf);
            bSuccess = false;
        }
    } else {
        cmdbuf = SVDRPcmd;
        epgsSVDRP::cSVDRPClient client;
        if (!client.SendCmd(*cmdbuf)) {
            LogFile.eSysLog("command '%s' failed", *cmdbuf);
            bSuccess = false;
        }
    }

    return bSuccess;
}

int SendMsg(cString Message, bool confirm, int seconds, eMessageType messageType)
{
    int Keys = Skins.QueueMessage(messageType, Message, seconds, confirm ? seconds + 2 : 0);
    return Keys;
}


bool InEditMode(const char* ItemText, const char* ItemName, const char* ItemValue)
{
    bool bEditMode = true;
    // ugly solution to detect, if in edit mode
    char* value = strdup(ItemText);
    strreplace(value, ItemName, "");
    strreplace(value, ":\t", "");
    // for bigpatch
    strreplace(value, "\t", "");
    if (strlen(value) == strlen(ItemValue))
        bEditMode = false;
    free(value);
    return bEditMode;
}

// checks if the timer was triggered from a search timer and return a pointer to the search
cSearchExt* TriggeredFromSearchTimer(const cTimer* timer)
{
// searches will create local timers only.
// But, an epgsearch running on a remote VDR might create a timer. And this timer has a search ID ...
// Therefore, we must first ensure that the timer is local
    if(timer->Remote())
        return NULL;
    char* searchID = GetAuxValue(timer, "s-id");
    if (!searchID)
        return NULL;

    cSearchExt* search = SearchExts.GetSearchFromID(atoi(searchID));
    free(searchID);
    return search;
}

int TriggeredFromSearchTimerID(const cTimer* timer)
{
    cSearchExt* trigger = TriggeredFromSearchTimer(timer);
    if (trigger)
        return trigger->ID;
    else
        return -1;
}

double FuzzyMatch(const char* s1, const char* s2, int maxLength)
{
    Distance D;
    int dist = D.LD(s1, s2, maxLength);
    double fMaxLength = std::max(strlen(s1), strlen(s2));
    return (fMaxLength - dist) / fMaxLength;
}

bool DescriptionMatches(const char* eDescr, const char* rDescr, int matchLimit)
{
    if (eDescr == NULL && rDescr == NULL) return true;
    if (eDescr == NULL && rDescr != NULL) return false;
    if (eDescr != NULL && rDescr == NULL) return false;
    int l_eDescr = strlen(eDescr);
    int l_rDescr = strlen(rDescr);
    if (l_eDescr == l_rDescr && strcmp(eDescr, rDescr) == 0) return true;

    // partial match:
    // first check the length, should only be different at match limit
    int minLength = std::min(l_eDescr, l_rDescr);
    int maxLength = std::max(l_eDescr, l_rDescr);
    if (100 * double(minLength) / double(maxLength) < matchLimit)
        return false;

    // last try with Levenshtein Distance, only compare the first 1000 chars
    double fMatch = FuzzyMatch(eDescr, rDescr, 1000);
    double tmp_matchlimit = matchLimit / 100.0;
    if (maxLength - minLength < 5 && matchLimit < 95) {
        tmp_matchlimit = 0.95;
        LogFile.Log(2, "difference between both descriptions is < 5 setting matchlimit to: %.2f %%", tmp_matchlimit * 100);
    }
    if (fMatch > tmp_matchlimit) {
        LogFile.Log(2, "match is: %.2f %%", fMatch * 100);
        return true;
    }
    return false;
}

const cEvent* GetEvent(const cTimer* timer)
{
    const cEvent* event = NULL;
    const cChannel *channel = timer->Channel();
    time_t Time = timer->StartTime() + (timer->StopTime() - timer->StartTime()) / 2;
    for (int seconds = 0; seconds <= 3; seconds++) {
        {
            LOCK_SCHEDULES_READ;
            if (Schedules) {
                const cSchedule *Schedule = Schedules->GetSchedule(channel->GetChannelID());
                if (Schedule) {
                    event = Schedule->GetEventAround(Time);
                    if (event) return event;
                }
            }
        }
        if (seconds == 0)
            LogFile.Log(2, "waiting for EPG info...");
        sleepSec(1);
    }
    LogFile.Log(1, "no EPG info available");
    return NULL;
}

// this extracts the real description from a given epg entry cutting all that looks like a category line
// we assume that a category has a name not longer than MAXCATNAMELENGTH and a value not longer than
// MAXCATVALUELENGTH (so in most cases e.g. the 'cast' category will stay part of the description). name and
// value are separated with ': '
#define MAXCATNAMELENGTH 40
#define MAXCATVALUELENGTH 60

char* GetRawDescription(const char* descr)
{
    if (!descr || !*descr) return NULL;

    char* rawDescr = (char*) calloc(strlen(descr) + 1, sizeof(char));

    const char* tmp = descr;
    while (tmp) {
        // extract a single line
        const char* lf = strchr(tmp, '\n');
        char* line = NULL;
        if (lf)
            line = strndup(tmp, lf - tmp);
        else
            line = strdup(tmp);
        //      if (lf) *lf = 0;
        // check for category
        char* delim = strstr(line, ": ");
        if (!delim ||
            (delim && (delim - line > MAXCATNAMELENGTH || strlen(line) - (delim - line) + 2 > MAXCATVALUELENGTH)))
            if (*line)
                strcat(rawDescr, line);

        if (lf) tmp += strlen(line) + 1;
        else tmp = NULL;
        free(line);
    }
    return rawDescr;
}

void PrepareTimerFile(const cEvent* event, cTimer* timer)
{
    if (!event) return;
    if (EPGSearchConfig.addSubtitleToTimer == addSubtitleNever && strlen(EPGSearchConfig.defrecdir) == 0) // nothing to do
        return;
    if (!isempty(event->ShortText())) { // add subtitle if present
        bool addSubtitle = (EPGSearchConfig.addSubtitleToTimer != addSubtitleNever);
        if (EPGSearchConfig.addSubtitleToTimer == addSubtitleSmart)
            if (event->Duration() > 80 * 60)
                addSubtitle = false;

        if (addSubtitle) {
            char tmp[MaxFileName] = "";
            snprintf(tmp, MaxFileName, "%s~%s", event->Title(), event->ShortText());
            timer->SetFile(tmp);
        }
    }
    if (strlen(EPGSearchConfig.defrecdir) > 0) {
        char directory[MaxFileName] = "";
        strn0cpy(directory, EPGSearchConfig.defrecdir, sizeof(directory));
        cVarExpr varExprDir(directory);
        if (!varExprDir.DependsOnVar("%title%", event)) {
            strcat(directory, "~");
            strcat(directory, timer->File());
        }
        // parse the epxression and evaluate it
        cVarExpr varExpr(directory);
        strcpy(directory, varExpr.Evaluate(event).c_str());
        if (strchr(directory, '%') == NULL) // only set directory to new value if all categories could have been replaced
            timer->SetFile(directory);
    }
}

bool EventsMatch(const cEvent* event1, const cEvent* event2, bool compareTitle, int compareSubtitle, bool compareSummary, int compareDate, unsigned long catvaluesAvoidRepeat, int matchLimit)
{
    if (!event1 || !event2) return false;
    if (event1 == event2) return true;

    // only compare the alphanumeric portions
    string Title1 = "";
    string Title2 = "";
    if (compareTitle) {
        string s1 = event1->Title() ? event1->Title() : "";
        string s2 = event2->Title() ? event2->Title() : "";
        Title1 = GetAlNum(s1);
        Title2 = GetAlNum(s2);
        std::transform(Title1.begin(), Title1.end(), Title1.begin(), tolower);
        std::transform(Title2.begin(), Title2.end(), Title2.begin(), tolower);
    }
    string Subtitle1 = "";
    string Subtitle2 = "";
    if (compareSubtitle) {
        string s1 = event1->ShortText() ? event1->ShortText() : "";
        string s2 = event2->ShortText() ? event2->ShortText() : "";
        Subtitle1 = GetAlNum(s1);
        Subtitle2 = GetAlNum(s2);
        std::transform(Subtitle1.begin(), Subtitle1.end(), Subtitle1.begin(), tolower);
        std::transform(Subtitle2.begin(), Subtitle2.end(), Subtitle2.begin(), tolower);
    }
    string compareExpression = "";
    if (compareDate == 1) compareExpression = "%date%";
    if (compareDate == 2) compareExpression = "%year%-%week%";
    if (compareDate == 3) compareExpression = "%year%-%month%";

    bool match = false;
    if ((!compareTitle || Title1 == Title2) &&
        (!compareSubtitle || (Subtitle1 == Subtitle2 && Subtitle1 != ""))) {
        const char* Descr1    = event1->Description();
        const char* Descr2    = event2->Description();
        if (compareSummary) {
            char* rawDescr1 = GetRawDescription(Descr1);
            char* rawDescr2 = GetRawDescription(Descr2);
            match = DescriptionMatches(rawDescr1, rawDescr2, matchLimit);
            free(rawDescr1);
            free(rawDescr2);
            if (!match) return false;
        }
        if (compareExpression.size() > 0) {
            cVarExpr varExpr(compareExpression);
            string resEvent1 = varExpr.Evaluate(event1);
            string resEvent2 = varExpr.Evaluate(event2);
            if (resEvent1 != resEvent2)
                return false;
        }
        if (catvaluesAvoidRepeat != 0) { // check categories
            bool bCatMatch = ((Descr1 && Descr2) || (!Descr1 && !Descr2));
            cSearchExtCat *SearchExtCat = SearchExtCats.First();
            int index = 0;
            while (catvaluesAvoidRepeat > 0 && SearchExtCat && bCatMatch) {
                if (catvaluesAvoidRepeat & (1 << index)) {
                    char* CatValue1 = GetExtEPGValue(Descr1, SearchExtCat->name, SearchExtCat->format);
                    char* CatValue2 = GetExtEPGValue(Descr2, SearchExtCat->name, SearchExtCat->format);
                    if ((!CatValue1 && CatValue2) ||
                        (!CatValue2 && CatValue1) ||
                        (CatValue1 && CatValue2 && strcmp(CatValue1, CatValue2) != 0))
                        bCatMatch = false;
                    free(CatValue1);
                    free(CatValue2);
                }
                SearchExtCat = SearchExtCats.Next(SearchExtCat);
                index++;
            }
            if (bCatMatch)
                match = true;
        } else
            match = true;
    }
    return match;
}

int ChannelNrFromEvent(const cEvent* pEvent)
{
    if (!pEvent)
        return -1;
    LOCK_CHANNELS_READ;
    const cChannel* channel = Channels->GetByChannelID(pEvent->ChannelID(), true, true);
    if (!channel)
        return -1;
    else
        return channel->Number();
}

void DelTimer(int index)
{
    cString cmdbuf = cString::sprintf("DELT %d", index);
    LogFile.Log(2, "delete timer %d", index);
    SendViaSVDRP(cmdbuf);
    gl_timerStatusMonitor->SetConflictCheckAdvised();
}

char* FixSeparators(char* buffer, char sep)
{
    int l = strlen(buffer);
    char *dest = buffer;
    for (int i = 0; i < l; i ++) {
        char c = buffer[i];
        int j = i;
        if (c == sep) {
            for (j = i + 1; (j < l) & (buffer[j] == ' '); j++)
                ;

            if ((j <= l) | (i + 1 < j)) {
                switch (buffer[j]) {
                case '\t':
                    i = j;
                    c = '\t';
                    break;
                case 0:
                    i = j;
                    c = 0;
                    break;
                default:
                    break;
                }
            }
        }
        if (c == '\t') {
            for (; (j < l) & (buffer[j] == ' '); j++)
                ;
            if (j < l && buffer[j] == sep) {
                buffer[j] = '\t';
                i = j - 1;
                continue;
            }
        }
        *dest++ = c;
    }
    *dest = 0;
    return buffer;
}

cString DateTime(time_t t)
{
    char buffer[32];
    if (t == 0)
        time(&t);
    struct tm tm_r;
    tm *tm = localtime_r(&t, &tm_r);
    snprintf(buffer, sizeof(buffer), "%02d.%02d. %02d:%02d", tm->tm_mday, tm->tm_mon + 1, tm->tm_hour, tm->tm_min);
    return buffer;
}

string NumToString(long num)
{
    ostringstream os;
    os << num;
    return os.str();
}

int FindIgnoreCase(const string& expr, const string& query)
{
    const char *p = expr.c_str();
    const char *r = strcasestr(p, query.c_str());

    if (!r)
        return -1;
    return r - p;
}

bool EqualsNoCase(const string& a, const string& b)
{
    return strcasecmp(a.c_str(), b.c_str()) == 0;
}

string Strip(const string& input)
{
    string str = input;
    string::size_type pos = str.find_last_not_of(' ');
    if (pos != string::npos) {
        str.erase(pos + 1);
        pos = str.find_first_not_of(' ');
        if (pos != string::npos) str.erase(0, pos);
    } else str.erase(str.begin(), str.end());
    return str;
}

string GetAlNum(const string& s)
{
    string res;
    for (unsigned int i = 0; i < s.size(); i++)
        if (isalnum(s[i]))
            res += s[i];
    return res;
}

string ReplaceAll(const string& input, const string& what, const string& with)
{
    string result = input;
    int pos = 0;

    while ((pos = FindIgnoreCase(result, what)) >= 0)
        result.replace(pos, what.size(), with);
    return result;
}

string EscapeString(const string& S)
{
    string tmp = S;
    int apostrophPos = 0;
    int apostrophTempPos = 0;
    while ((apostrophPos = tmp.find("'", apostrophTempPos)) >= apostrophTempPos) {
        tmp.replace(apostrophPos, 1, "'\"'\"'");
        apostrophTempPos = apostrophPos + 5;
    }
    return tmp;
}

string QuoteApostroph(const string& S)
{
    string tmp = S;
    int apostrophPos = 0;
    int apostrophTempPos = 0;
    while ((apostrophPos = tmp.find("\"", apostrophTempPos)) >= apostrophTempPos) {
        tmp.replace(apostrophPos, 1, "\\\"");
        apostrophTempPos = apostrophPos + 2;
    }
    return tmp;
}

string MD5(const string& input)
{
    char* szInput = strdup(input.c_str());
    if (!szInput) return "";
    char* szRes = MD5String(szInput);
    string res = szRes;
    free(szRes);
    return res;
}

void SetAux(cTimer* timer, string aux)
{
    if (!timer) return;
    string timerText = *timer->ToText();
    string::size_type auxPos = string::npos;
    for (int i = 0; i <= 7; i++) // get aux value
        auxPos = timerText.find(":", auxPos == string::npos ? 0 : auxPos + 1);
    if (auxPos == string::npos) return;
    timerText.replace(auxPos + 1, timerText.size() - auxPos + 1, aux);
    timer->Parse(timerText.c_str());
}

int msprintf(char **strp, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int res = vasprintf(strp, fmt, ap);
    va_end(ap);
    return res;
}

std::string GetCodeset()
{
    // Taken from VDR's vdr.c
    char *CodeSet = NULL;
    if (setlocale(LC_CTYPE, ""))
        CodeSet = nl_langinfo(CODESET);
    else {
        char *LangEnv = getenv("LANG"); // last resort in case locale stuff isn't installed
        if (LangEnv) {
            CodeSet = strchr(LangEnv, '.');
            if (CodeSet)
                CodeSet++; // skip the dot
        }
    }
    if (CodeSet)
        return std::string(CodeSet);
    else
        return "ISO-8859-15";
}

/*  Read a line from a socket  */
ssize_t Readline(int sockd, char *vptr, size_t maxlen)
{
    size_t n, rc;
    char    c, *buffer;

    buffer = vptr;

    for (n = 1; n < maxlen; n++) {

        if ((rc = read(sockd, &c, 1)) == 1) {
            if (c == '\n')
                break;
            *buffer++ = c;
        } else if (rc == 0) {
            if (n == 1)
                return 0;
            else
                break;
        } else {
            if (errno == EINTR)
                continue;
            return -1;
        }
    }

    *buffer = 0;
    return n;
}

/*  Write a line to a socket  */
ssize_t Writeline(int sockd, const char *vptr, ssize_t n)
{
    ssize_t      nleft;
    ssize_t     nwritten;
    const char *buffer;

    buffer = vptr;
    nleft  = n;

    while (nleft > 0) {
        if ((nwritten = write(sockd, buffer, nleft)) <= 0) {
            if (errno == EINTR)
                nwritten = 0;
            else
                return -1;
        }
        nleft  -= nwritten;
        buffer += nwritten;
    }

    return n;
}

long getAddrFromString(const char* hostnameOrIp, struct sockaddr_in* addr)
{
    unsigned long ip;

    struct hostent * he;

    if (hostnameOrIp == NULL || addr == NULL)
        return -1;

    ip = inet_addr(hostnameOrIp);

    if (ip != INADDR_NONE) {
        addr->sin_addr.s_addr = ip;
        return 0;
    } else {
        he = gethostbyname(hostnameOrIp);
        if (he == NULL)
            return -1;
        else
            memcpy(&(addr->sin_addr), he->h_addr_list[0], 4);
        return 0;
    }
}

char *cCommand::result = NULL;

cCommand::cCommand(void)
{
    title = command = NULL;
    confirm = false;
}

cCommand::~cCommand()
{
    free(title);
    free(command);
}

bool cCommand::Parse(const char *s)
{
    const char *p = strchr(s, ':');
    if (p) {
        int l = p - s;
        if (l > 0) {
            title = MALLOC(char, l + 1);
            stripspace(strn0cpy(title, s, l + 1));
            if (!isempty(title)) {
                int l = strlen(title);
                if (l > 1 && title[l - 1] == '?') {
                    confirm = true;
                    title[l - 1] = 0;
                }
                command = stripspace(strdup(skipspace(p + 1)));
                return !isempty(command);
            }
        }
    }
    return false;
}

const char *cCommand::Execute(const char *Parameters)
{
    free(result);
    result = NULL;
    cString cmdbuf;
    if (Parameters)
        cmdbuf = cString::sprintf("%s %s", command, Parameters);
    const char *cmd = *cmdbuf ? *cmdbuf : command;
    dsyslog("executing command '%s'", cmd);
    cPipe p;
    if (p.Open(cmd, "r")) {
        int l = 0;
        int c;
        while ((c = fgetc(p)) != EOF) {
            if (l % 20 == 0)
                result = (char *)realloc(result, l + 21);
            result[l++] = char(c);
        }
        if (result)
            result[l] = 0;
        p.Close();
    } else
        esyslog("ERROR: can't open pipe for command '%s'", cmd);
    return result;
}
