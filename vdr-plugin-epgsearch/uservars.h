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

#ifndef __USERVARS_INC__
#define __USERVARS_INC__

#include <algorithm>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <vdr/plugin.h>
#include <vdr/videodir.h>
#include "varparser.h"
#include "epgsearchtools.h"
#include "epgsearchcats.h"

using std::string;
using std::set;
using std::map;
using std::ostringstream;

class cUserVar : public cListObject
{
    const cEvent* oldEvent; // cache
    bool oldescapeStrings;

    string oldResult;
    string EvaluateCondExpr(const cEvent* e, bool escapeStrings = false);
    string EvaluateCompExpr(const cEvent* e, bool escapeStrings = false);
    string EvaluateShellCmd(const cEvent* e);
    string EvaluateConnectCmd(const cEvent* e);
    string EvaluateLengthCmd(const cEvent* e);
public:
    cUserVar();
    cVarParser varparser;
    set<cUserVar*> usedVars;

    virtual string Evaluate(const cEvent* e, bool escapeStrings = false);

    string EvaluateInternalVars(const string& Expr, const cEvent* e, bool escapeStrings = false);
    string EvaluateInternalTimerVars(const string& Expr, const cTimer* t);
    string EvaluateInternalSearchVars(const string& Expr, const cSearchExt* s);
    string EvaluateExtEPGVars(const string& Expr, const cEvent* e, bool escapeStrings = false);
    string EvaluateUserVars(const string& Expr, const cEvent* e, bool escapeStrings = false);
    virtual string Name(bool = false) {
        return varparser.varName;
    }
    virtual bool IsCondExpr() {
        return varparser.IsCondExpr();
    }
    virtual bool IsShellCmd() {
        return varparser.IsShellCmd();
    }
    virtual bool IsConnectCmd() {
        return varparser.IsConnectCmd();
    }
    virtual bool IsLengthCmd() {
        return varparser.IsLengthCmd();
    }
    bool DependsOnVar(const string& varName);
    bool DependsOnVar(cUserVar* var);
    bool AddDepVar(cUserVar* var);
    void ResetCache();
};

class cExtEPGVar : public cUserVar
{
    const string name;
    static string nameSpace;
public:
    cExtEPGVar(const string& Name) : name(Name) {}
    string Name(bool withNamespace = false) {
        return "%" + (withNamespace ? nameSpace + "." : "") + name + "%";
    }
    bool IsCondExpr() {
        return false;
    }
    bool IsShellCmd() {
        return false;
    }
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        if (!e) return "";

        cSearchExtCat* SearchExtCat = SearchExtCats.First();
        while (SearchExtCat) {
            string varName = string("%") + SearchExtCat->name + string("%");
            int varPos = FindIgnoreCase(varName, Name());
            if (varPos == 0) {
                char* value = GetExtEPGValue(e, SearchExtCat);
                string res =  value ? value : "";
                if (escapeStrings) return "'" + EscapeString(res) + "'";
                else return res;
            }
            SearchExtCat = SearchExtCats.Next(SearchExtCat);
        }
        return "";
    }
};

class cInternalVar : public cUserVar
{
    const string name;
public:
    cInternalVar(const string& Name) : name(Name) {}
    string Name(bool = false) {
        return "%" + name + "%";
    }
    bool IsCondExpr() {
        return false;
    }
    bool IsShellCmd() {
        return false;
    }
};

class cTitleVar : public cInternalVar
{
public:
    cTitleVar() : cInternalVar("title") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        string res = (e && !isempty(e->Title())) ? e->Title() : "";
        if (escapeStrings) return "'" + EscapeString(res) + "'";
        else return res;
    }
};

class cSubtitleVar : public cInternalVar
{
public:
    cSubtitleVar() : cInternalVar("subtitle") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        string res = (e && !isempty(e->ShortText())) ? e->ShortText() : "";
        if (escapeStrings) return "'" + EscapeString(res) + "'";
        else return res;
    }
};

class cSummaryVar : public cInternalVar
{
public:
    cSummaryVar() : cInternalVar("summary") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        string res = (e && !isempty(e->Description())) ? e->Description() : "";
        if (escapeStrings) return "'" + EscapeString(res) + "'";
        else return res;
    }
};

class cHTMLSummaryVar : public cInternalVar
{
public:
    cHTMLSummaryVar() : cInternalVar("htmlsummary") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        if (e && !isempty(e->Description())) {
            string res = ReplaceAll(e->Description(), "\n", "<br />");
            if (escapeStrings) return "'" + EscapeString(res) + "'";
            else return res;
        } else
            return "";
    }
};

class cEventIDVar : public cInternalVar
{
public:
    cEventIDVar() : cInternalVar("eventid") {}
    string Evaluate(const cEvent* e,  bool escapeStrings = false) {
        if (e) {
            ostringstream os;
            os << e->EventID();
            return os.str();
        } else return "";
    }
};

class cLiveEventIDVar : public cInternalVar
{
public:
    cLiveEventIDVar() : cInternalVar("liveeventid") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        if (!e) return "";
        LOCK_CHANNELS_READ;
        const cChannel *channel = Channels->GetByChannelID(e->ChannelID(), true);
        if (!channel) return "";

        string res(channel->GetChannelID().ToString());
        res = "event_" + res;
        res = ReplaceAll(res, ".", "p");
        res = ReplaceAll(res, "-", "m");
        res += "_" + NumToString(e->EventID());
        if (escapeStrings) return "'" + EscapeString(res) + "'";
        else return res;
    }
};

class cTimeVar : public cInternalVar
{
public:
    cTimeVar() : cInternalVar("time") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        string res = (e ? * (e->GetTimeString()) : "");
        if (escapeStrings) return "'" + EscapeString(res) + "'";
        else return res;
    }
};

class cTimeEndVar : public cInternalVar
{
public:
    cTimeEndVar() : cInternalVar("timeend") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        string res = (e ? * (e->GetEndTimeString()) : "");
        if (escapeStrings) return "'" + EscapeString(res) + "'";
        else return res;
    }
};

class cTime_wVar : public cInternalVar
{
public:
    cTime_wVar() : cInternalVar("time_w") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        string res = (e ? WEEKDAYNAME(e->StartTime()) : "");
        if (escapeStrings) return "'" + EscapeString(res) + "'";
        else return res;
    }
};

class cTime_dVar : public cInternalVar
{
public:
    cTime_dVar() : cInternalVar("time_d") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        if (!e) return "";
        char day[3] = "";
        struct tm tm_r;
        const time_t t = e->StartTime();
        tm *tm = localtime_r(&t, &tm_r);
        strftime(day, sizeof(day), "%d", tm);
        if (escapeStrings) return "'" + EscapeString(day) + "'";
        else return day;
    }
};

class cTime_lngVar : public cInternalVar
{
public:
    cTime_lngVar() : cInternalVar("time_lng") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        if (!e) return "";
        ostringstream os;
        os << e->StartTime();
        if (escapeStrings) return "'" + EscapeString(os.str()) + "'";
        else return os.str();
    }
};

class cTimeSpanVar : public cInternalVar
{
public:
    cTimeSpanVar() : cInternalVar("timespan") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        if (!e) return "";
        time_t diff = e->StartTime() - time(NULL);
        string res;
        if (labs(diff) >= SECSINDAY) {
            cString buffer;
            if (diff > 0)
                buffer = cString::sprintf(tr("in %02ldd"), long(diff / SECSINDAY));
            else
                buffer = cString::sprintf("%02ldd", long(-diff / SECSINDAY));
            res = buffer;
        } else if (labs(diff) >= (60 * 60)) {
            cString buffer;
            if (diff > 0)
                buffer = cString::sprintf(tr("in %02ldh"), long(diff / (60 * 60)));
            else
                buffer = cString::sprintf("%02ldh", long(-diff / (60 * 60)));
            res = buffer;
        } else {
            cString buffer;
            if (diff > 0)
                buffer = cString::sprintf(tr("in %02ldm"), long(diff / 60));
            else
                buffer = cString::sprintf("%02ldm", long(-diff / 60));
            res = buffer;
        }
        if (escapeStrings) return "'" + EscapeString(res) + "'";
        else return res;
    }
};

class cLength_Var : public cInternalVar
{
public:
    cLength_Var() : cInternalVar("length") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        return (e ? NumToString(e->Duration()) : "");
    }
};

class cDateVar : public cInternalVar
{
public:
    cDateVar() : cInternalVar("date") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        if (!e) return "";
        char date[9] = "";
        struct tm tm_r;
        const time_t t = e->StartTime();
        tm *tm = localtime_r(&t, &tm_r);
        strftime(date, sizeof(date), "%d.%m.%y", tm);
        if (escapeStrings) return "'" + EscapeString(date) + "'";
        else return date;
    }
};

class cDateShortVar : public cInternalVar
{
public:
    cDateShortVar() : cInternalVar("datesh") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        if (!e) return "";
        char dateshort[7] = "";
        struct tm tm_r;
        const time_t t = e->StartTime();
        tm *tm = localtime_r(&t, &tm_r);
        strftime(dateshort, sizeof(dateshort), "%d.%m.", tm);
        if (escapeStrings) return "'" + EscapeString(dateshort) + "'";
        else return dateshort;
    }
};

class cDateISOVar : public cInternalVar
{
public:
    cDateISOVar() : cInternalVar("date_iso") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        if (!e) return "";
        char dateISO[11] = "";
        struct tm tm_r;
        const time_t t = e->StartTime();
        tm *tm = localtime_r(&t, &tm_r);
        strftime(dateISO, sizeof(dateISO), "%Y-%m-%d", tm);
        if (escapeStrings) return "'" + EscapeString(dateISO) + "'";
        else return dateISO;
    }
};

class cYearVar : public cInternalVar
{
public:
    cYearVar() : cInternalVar("year") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        if (!e) return "";
        char year[5] = "";
        struct tm tm_r;
        const time_t t = e->StartTime();
        tm *tm = localtime_r(&t, &tm_r);
        strftime(year, sizeof(year), "%Y", tm);
        if (escapeStrings) return "'" + EscapeString(year) + "'";
        else return year;
    }
};

class cMonthVar : public cInternalVar
{
public:
    cMonthVar() : cInternalVar("month") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        if (!e) return "";
        char month[3] = "";
        struct tm tm_r;
        const time_t t = e->StartTime();
        tm *tm = localtime_r(&t, &tm_r);
        strftime(month, sizeof(month), "%m", tm);
        if (escapeStrings) return "'" + EscapeString(month) + "'";
        else return month;
    }
};

class cDayVar : public cInternalVar
{
public:
    cDayVar() : cInternalVar("day") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        if (!e) return "";
        char day[3] = "";
        struct tm tm_r;
        const time_t t = e->StartTime();
        tm *tm = localtime_r(&t, &tm_r);
        strftime(day, sizeof(day), "%d", tm);
        if (escapeStrings) return "'" + EscapeString(day) + "'";
        else return day;
    }
};

class cWeekVar : public cInternalVar
{
public:
    cWeekVar() : cInternalVar("week") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        if (!e) return "";
        char day[3] = "";
        struct tm tm_r;
        const time_t t = e->StartTime();
        tm *tm = localtime_r(&t, &tm_r);
        strftime(day, sizeof(day), "%V", tm);
        if (escapeStrings) return "'" + EscapeString(day) + "'";
        else return day;
    }
};

class cChannelNrVar : public cInternalVar
{
public:
    cChannelNrVar() : cInternalVar("chnr") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        int chnr = ChannelNrFromEvent(e);
        if (chnr < 0) return "";
        return NumToString(chnr);
    }
};

class cChannelShortVar : public cInternalVar
{
public:
    cChannelShortVar() : cInternalVar("chsh") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        if (!e) return "";
        LOCK_CHANNELS_READ;
        const cChannel *channel = Channels->GetByChannelID(e->ChannelID(), true);
        string res = channel ? channel->ShortName(true) : "";
        if (escapeStrings) return "'" + EscapeString(res) + "'";
        else return res;
    }
};

class cChannelLongVar : public cInternalVar
{
public:
    cChannelLongVar() : cInternalVar("chlng") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        if (!e) return "";
        LOCK_CHANNELS_READ;
        const cChannel *channel = Channels->GetByChannelID(e->ChannelID(), true);
        string res = channel ? channel->Name() : "";
        if (escapeStrings) return "'" + EscapeString(res) + "'";
        else return res;
    }
};

class cChannelDataVar : public cInternalVar
{
public:
    cChannelDataVar() : cInternalVar("chdata") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        if (!e) return "";
        LOCK_CHANNELS_READ;
        const cChannel *channel = Channels->GetByChannelID(e->ChannelID(), true);
        return channel ? CHANNELSTRING(channel) : "";
    }
};

class cChannelGroupVar : public cInternalVar
{
public:
    cChannelGroupVar() : cInternalVar("chgrp") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        if (!e) return "";
        ostringstream os;
        LOCK_CHANNELS_READ;
        const cChannel *channel = Channels->GetByChannelID(e->ChannelID(), true);
        while (channel && !channel->GroupSep())
            channel = Channels->Prev(channel);
        if (!channel || !channel->Name()) return "";
        string grpName = channel->Name();
        if (escapeStrings) return "'" + EscapeString(grpName) + "'";
        else return grpName;
    }
};

class cNEWTCmdVar : public cInternalVar
{
public:
    cNEWTCmdVar() : cInternalVar("newtcmd") {}
    string Evaluate(const cEvent* e, bool escapeStrings = false) {
        if (!e) return "";
        cTimer* timer = new cTimer(e);
        string newtCmd =  *(timer->ToText());
        if (escapeStrings) return "'" + EscapeString(newtCmd) + "'";
        else return newtCmd;
    }
};

// independet variables
class cColonVar : public cInternalVar
{
public:
    cColonVar() : cInternalVar("colon") {}
    string Evaluate(const cEvent*, bool escapeStrings = false) {
        return ":";
    }
};

class cDateNowVar : public cInternalVar
{
public:
    cDateNowVar() : cInternalVar("datenow") {}
    string Evaluate(const cEvent*, bool escapeStrings = false) {
        char date[9] = "";
        struct tm tm_r;
        const time_t t = time(NULL);
        tm *tm = localtime_r(&t, &tm_r);
        strftime(date, sizeof(date), "%d.%m.%y", tm);
        if (escapeStrings) return "'" + EscapeString(date) + "'";
        else return date;
    }
};

class cDateShortNowVar : public cInternalVar
{
public:
    cDateShortNowVar() : cInternalVar("dateshnow") {}
    string Evaluate(const cEvent*, bool escapeStrings = false) {
        char dateshort[7] = "";
        struct tm tm_r;
        const time_t t = time(NULL);
        tm *tm = localtime_r(&t, &tm_r);
        strftime(dateshort, sizeof(dateshort), "%d.%m.", tm);
        if (escapeStrings) return "'" + EscapeString(dateshort) + "'";
        else return dateshort;
    }
};

class cDateISONowVar : public cInternalVar
{
public:
    cDateISONowVar() : cInternalVar("date_iso_now") {}
    string Evaluate(const cEvent*, bool escapeStrings = false) {
        char dateISO[11] = "";
        struct tm tm_r;
        const time_t t = time(NULL);
        tm *tm = localtime_r(&t, &tm_r);
        strftime(dateISO, sizeof(dateISO), "%Y-%m-%d", tm);
        if (escapeStrings) return "'" + EscapeString(dateISO) + "'";
        else return dateISO;
    }
};

class cTimeNowVar : public cInternalVar
{
public:
    cTimeNowVar() : cInternalVar("timenow") {}
    string Evaluate(const cEvent*, bool escapeStrings = false) {
        return TIMESTRING(time(NULL));
    }
};

class cVideodirVar : public cInternalVar
{
public:
    cVideodirVar() : cInternalVar("videodir") {}
    string Evaluate(const cEvent*, bool escapeStrings = false) {
        return cVideoDirectory::Name();
    }
};

class cPlugconfdirVar : public cInternalVar
{
public:
    static string dir;
    cPlugconfdirVar() : cInternalVar("plugconfdir") {}
    string Evaluate(const cEvent*, bool escapeStrings = false) {
        return dir;
    }
};

class cEpgsearchconfdirVar : public cInternalVar
{
public:
    static string dir;
    cEpgsearchconfdirVar() : cInternalVar("epgsearchdir") {}
    string Evaluate(const cEvent*, bool escapeStrings = false) {
        return CONFIGDIR;
    }
};

// timer variables
class cTimerVar
{
    static string nameSpace;
    const string name;
public:
    cTimerVar(const string& Name) : name(Name) {}
    virtual ~cTimerVar() {}
    string Name() {
        return "%" + nameSpace + "." + name + "%";
    }
    virtual string Evaluate(const cTimer* t) = 0;
};

class cTimerDateVar : public cTimerVar
{
public:
    cTimerDateVar() : cTimerVar("date") {}
    string Evaluate(const cTimer* t) {
        if (!t) return "";
        return DATESTRING(t->StartTime());
    }
};

class cTimerStartVar : public cTimerVar
{
public:
    cTimerStartVar() : cTimerVar("start") {}
    string Evaluate(const cTimer* t) {
        if (!t) return "";
        return TIMESTRING(t->StartTime());
    }
};

class cTimerStopVar : public cTimerVar
{
public:
    cTimerStopVar() : cTimerVar("stop") {}
    string Evaluate(const cTimer* t) {
        if (!t) return "";
        return TIMESTRING(t->StopTime());
    }
};

class cTimerFileVar : public cTimerVar
{
public:
    cTimerFileVar() : cTimerVar("file") {}
    string Evaluate(const cTimer* t) {
        if (!t) return "";
        return t->File();
    }
};

class cTimerChnrVar : public cTimerVar
{
public:
    cTimerChnrVar() : cTimerVar("chnr") {}
    string Evaluate(const cTimer* t) {
        if (!t || !t->Channel()) return "";
        return NumToString(t->Channel()->Number());
    }
};

class cTimerChannelShortVar : public cTimerVar
{
public:
    cTimerChannelShortVar() : cTimerVar("chsh") {}
    string Evaluate(const cTimer* t) {
        if (!t || !t->Channel()) return "";
        return t->Channel()->ShortName(true);
    }
};

class cTimerChannelLongVar : public cTimerVar
{
public:
    cTimerChannelLongVar() : cTimerVar("chlng") {}
    string Evaluate(const cTimer* t) {
        if (!t || !t->Channel()) return "";
        return t->Channel()->Name();
    }
};

class cTimerSearchVar : public cTimerVar
{
public:
    cTimerSearchVar() : cTimerVar("search") {}
    string Evaluate(const cTimer* t) {
        if (!t) return "";
        cSearchExt* s = TriggeredFromSearchTimer(t);
        if (!s) return "";
        return s->search;
    }
};

class cTimerSearchIDVar : public cTimerVar
{
public:
    cTimerSearchIDVar() : cTimerVar("searchid") {}
    string Evaluate(const cTimer* t) {
        if (!t) return "";
        int ID = TriggeredFromSearchTimerID(t);
        if (ID < 0) return "";
        return NumToString(ID);
    }
};

class cTimerLiveIDVar : public cTimerVar
{
public:
    cTimerLiveIDVar() : cTimerVar("liveid") {}
    string Evaluate(const cTimer* t) {
        if (!t || !t->Channel()) return "";
        ostringstream builder;
        builder << *(t->Channel()->GetChannelID().ToString()) << ":" << t->WeekDays() << ":"
                << t->Day() << ":" << t->Start() << ":" << t->Stop();
        string res = builder.str();
        res = "timer_" + res;
        res = ReplaceAll(res, ".", "p");
        res = ReplaceAll(res, "-", "m");
        res = ReplaceAll(res, ":", "c");
        return res;
    }
};

// search variables
class cSearchVar
{
    const string name;
    static string nameSpace;
public:
    cSearchVar(const string& Name) : name(Name) {}
    virtual ~cSearchVar() {}
    string Name() {
        return "%" + nameSpace + "." + name + "%";
    }
    virtual string Evaluate(const cSearchExt* s) = 0;
};

class cSearchQueryVar : public cSearchVar
{
public:
    cSearchQueryVar() : cSearchVar("query") {}
    string Evaluate(const cSearchExt* s) {
        if (!s) return "";
        return s->search;
    }
};

class cSearchSeriesVar : public cSearchVar
{
public:
    cSearchSeriesVar() : cSearchVar("series") {}
    string Evaluate(const cSearchExt* s) {
        if (!s) return "";
        return NumToString(s->useEpisode);
    }
};

class cUserVars : public cList<cUserVar>
{
public:
    cTitleVar titleVar;
    cSubtitleVar subtitleVar;
    cSummaryVar summaryVar;
    cHTMLSummaryVar htmlsummaryVar;
    cEventIDVar eventIDVar;
    cLiveEventIDVar liveeventIDVar;
    cTimeVar timeVar;
    cTimeEndVar timeEndVar;
    cTime_wVar time_wVar;
    cTime_dVar time_dVar;
    cTime_lngVar time_lngVar;
    cTimeSpanVar time_spanVar;
    cLength_Var length_Var;
    cDateVar dateVar;
    cDateShortVar dateShortVar;
    cDateISOVar dateISOVar;
    cYearVar yearVar;
    cMonthVar monthVar;
    cDayVar dayVar;
    cWeekVar weekVar;
    cChannelNrVar chnrVar;
    cChannelShortVar chShortVar;
    cChannelLongVar chLongVar;
    cChannelDataVar chDataVar;
    cChannelGroupVar chGroupVar;
    cNEWTCmdVar newtCmdVar;
    cSearchQueryVar searchQueryVar;
    cSearchSeriesVar searchSeriesVar;

    cColonVar colonVar;
    cDateNowVar dateNowVar;
    cDateShortNowVar dateShortNowVar;
    cDateISONowVar dateISONowVar;
    cTimeNowVar timeNowVar;
    cVideodirVar videodirVar;
    cPlugconfdirVar plugconfdirVar;
    cEpgsearchconfdirVar epgsearchconfdirVar;

    cTimerDateVar timerDateVar;
    cTimerStartVar timerStartVar;
    cTimerStopVar timerStopVar;
    cTimerFileVar timerFileVar;
    cTimerChnrVar timerChnrVar;
    cTimerChannelShortVar timerChShortVar;
    cTimerChannelLongVar timerChLongVar;
    cTimerSearchVar timerSearchVar;
    cTimerSearchIDVar timerSearchIDVar;
    cTimerLiveIDVar timerLiveIDVar;

    map<string, cExtEPGVar*> extEPGVars;
    set<cUserVar*> userVars;
    map<string, cInternalVar*> internalVars;
    map<string, cTimerVar*> internalTimerVars;
    map<string, cSearchVar*> internalSearchVars;

    void InitInternalVars() {
        internalVars[titleVar.Name()] = &titleVar;
        internalVars[subtitleVar.Name()] = &subtitleVar;
        internalVars[summaryVar.Name()] = &summaryVar;
        internalVars[htmlsummaryVar.Name()] = &htmlsummaryVar;
        internalVars[eventIDVar.Name()] = &eventIDVar;
        internalVars[liveeventIDVar.Name()] = &liveeventIDVar;
        internalVars[timeVar.Name()] = &timeVar;
        internalVars[timeEndVar.Name()] = &timeEndVar;
        internalVars[time_wVar.Name()] = &time_wVar;
        internalVars[time_dVar.Name()] =  &time_dVar;
        internalVars[time_lngVar.Name()] = &time_lngVar;
        internalVars[time_spanVar.Name()] = &time_spanVar;
        internalVars[length_Var.Name()] = &length_Var;
        internalVars[dateVar.Name()] = &dateVar;
        internalVars[dateShortVar.Name()] = &dateShortVar;
        internalVars[dateISOVar.Name()] = &dateISOVar;
        internalVars[yearVar.Name()] = &yearVar;
        internalVars[monthVar.Name()] = &monthVar;
        internalVars[dayVar.Name()] = &dayVar;
        internalVars[weekVar.Name()] = &weekVar;
        internalVars[chnrVar.Name()] = &chnrVar;
        internalVars[chShortVar.Name()] = &chShortVar;
        internalVars[chLongVar.Name()] = &chLongVar;
        internalVars[chDataVar.Name()] = &chDataVar;
        internalVars[chGroupVar.Name()] = &chGroupVar;
        internalVars[newtCmdVar.Name()] = &newtCmdVar;

        internalVars[colonVar.Name()] = &colonVar;
        internalVars[dateNowVar.Name()] = &dateNowVar;
        internalVars[dateShortNowVar.Name()] = &dateShortNowVar;
        internalVars[dateISONowVar.Name()] = &dateISONowVar;
        internalVars[timeNowVar.Name()] = &timeNowVar;
        internalVars[videodirVar.Name()] = &videodirVar;
        internalVars[plugconfdirVar.Name()] = &plugconfdirVar;
        internalVars[epgsearchconfdirVar.Name()] = &epgsearchconfdirVar;

        internalTimerVars[timerDateVar.Name()] = &timerDateVar;
        internalTimerVars[timerStartVar.Name()] = &timerStartVar;
        internalTimerVars[timerStopVar.Name()] = &timerStopVar;
        internalTimerVars[timerFileVar.Name()] = &timerFileVar;
        internalTimerVars[timerChnrVar.Name()] = &timerChnrVar;
        internalTimerVars[timerChShortVar.Name()] = &timerChShortVar;
        internalTimerVars[timerChLongVar.Name()] = &timerChLongVar;
        internalTimerVars[timerSearchVar.Name()] = &timerSearchVar;
        internalTimerVars[timerSearchIDVar.Name()] = &timerSearchIDVar;
        internalTimerVars[timerLiveIDVar.Name()] = &timerLiveIDVar;

        internalSearchVars[searchQueryVar.Name()] = &searchQueryVar;
        internalSearchVars[searchSeriesVar.Name()] = &searchSeriesVar;
    }

    void InitExtEPGVars() {
        cSearchExtCat* SearchExtCat = SearchExtCats.First();
        while (SearchExtCat) {
            string varName = SearchExtCat->name;
            std::transform(varName.begin(), varName.end(), varName.begin(), tolower);
            cExtEPGVar* extEPGVar = new cExtEPGVar(varName);
            extEPGVars[extEPGVar->Name()] =  extEPGVar;
            SearchExtCat = SearchExtCats.Next(SearchExtCat);
        }
    }
    void ResetCache() {
        cUserVar* var = First();
        while (var) {
            var->ResetCache();
            var = Next(var);
        }
    }
    ~cUserVars() {
        std::map<string, cExtEPGVar*>::iterator evar;
        for (evar = extEPGVars.begin(); evar != extEPGVars.end(); ++evar)
            delete evar->second;
        extEPGVars.clear();

        std::set<cUserVar*>::iterator uvar;
        for (uvar = userVars.begin(); uvar != userVars.end(); ++uvar)
            delete(*uvar);
        userVars.clear();
    }
    cUserVar* GetFromName(const string& varName, bool log = true);
};

extern cUserVars UserVars;

class cUserVarLine : public cListObject
{
public:
    static bool Parse(char *s);
};


class cUserVarFile : public cConfig<cUserVarLine>
{
public:
    cUserVarFile() {
        UserVars.Clear();
    };
};

class cVarExpr
{
    string expr;
public:
    set<cUserVar*> usedVars;
    cVarExpr(const string& Expr) : expr(Expr) {}
    string Evaluate(const cEvent* e = NULL);
    string Evaluate(const cTimer* t);
    string Evaluate(const cSearchExt* s);
    bool DependsOnVar(const string& varName, const cEvent* e);
};

#endif
