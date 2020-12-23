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
#include <list>
#include <sstream>
#include <vdr/plugin.h>
#include "epgsearchservices.h"
#include "epgsearchext.h"
#include "epgsearchcfg.h"
#include "searchtimer_thread.h"
#include "epgsearchcats.h"
#include "changrp.h"
#include "blacklist.h"
#include "menu_dirselect.h"
#include "epgsearchtools.h"
#include "conflictcheck.h"
#include "timerstatus.h"
#include "uservars.h"

std::list<std::string> cEpgsearchServiceHandler::SearchTimerList()
{
    std::list<std::string> list;
    cMutexLock SearchExtsLock(&SearchExts);
    for (int i = 0; i < SearchExts.Count(); i++) {
        cSearchExt* search = SearchExts.Get(i);
        if (search)
            list.push_back(search->ToText());
    }
    return list;
}

int cEpgsearchServiceHandler::AddSearchTimer(const std::string& settings)
{
    cSearchExt* search = new cSearchExt;
    if (search->Parse(settings.c_str())) {
        search->ID = SearchExts.GetNewID();
        LogFile.Log(1, "added search '%s' (%d) via service interface", search->search, search->ID);
        cMutexLock SearchExtsLock(&SearchExts);
        SearchExts.Add(search);
        SearchExts.Save();
        if (search->useAsSearchTimer && !EPGSearchConfig.useSearchTimers) // enable search timer thread if necessary
            cSearchTimerThread::Init((cPluginEpgsearch*) cPluginManager::GetPlugin("epgsearch"), true);
        return search->ID;
    } else {
        LogFile.Log(1, "error adding '%s' via service interface", search->search);
        delete search;
        return -1;
    }
}

bool cEpgsearchServiceHandler::ModSearchTimer(const std::string& settings)
{
    cSearchExt Search;
    if (Search.Parse(settings.c_str())) {
        cSearchExt *searchTemp = SearchExts.GetSearchFromID(Search.ID);
        if (searchTemp) {
            searchTemp->Parse(settings.c_str());
            LogFile.Log(1, "modified search '%s' (%d) via service interface", searchTemp->search, searchTemp->ID);
            SearchExts.Save();
            if (searchTemp->useAsSearchTimer && !EPGSearchConfig.useSearchTimers) // enable search timer thread if necessary
                cSearchTimerThread::Init((cPluginEpgsearch*) cPluginManager::GetPlugin("epgsearch"), true);
            return true;
        }
    } else {
        LogFile.Log(1, "error modifying '%s' via service interface", settings.c_str());
    }
    return false;
}

bool cEpgsearchServiceHandler::DelSearchTimer(int ID)
{
    cSearchExt *search = SearchExts.GetSearchFromID(ID);
    if (search) {
        LogFile.Log(1, "search '%s' deleted via service interface", search->search);
        cMutexLock SearchExtsLock(&SearchExts);
        SearchExts.Del(search);
        SearchExts.Save();
        RecsDone.RemoveSearchID(ID);
        return true;
    } else
        LogFile.Log(1, "error deleting search with id '%d' via service interface", ID);
    return false;
}

std::list<std::string> cEpgsearchServiceHandler::TranslateResults(cSearchResults* pCompleteSearchResults)
{
    std::list<std::string> list;

    if (pCompleteSearchResults) {
        // transfer to result list
        pCompleteSearchResults->SortBy(CompareEventTime);
        cSearchResult *result = pCompleteSearchResults->First();
        while (result && result->search) {
            const cEvent* pEvent = result->event;
            cTimer* Timer = new cTimer(pEvent);

            static char bufStart[25];
            static char bufEnd[25];

            struct tm tm_r;
            time_t eStart = pEvent->StartTime();
            time_t eStop = pEvent->EndTime();
            time_t start = eStart - (result->search->MarginStart * 60);
            time_t stop  = eStop + (result->search->MarginStop * 60);
            if (result->search->useVPS && pEvent->Vps() && Setup.UseVps) {
                start = pEvent->Vps();
                stop = start + pEvent->Duration();
            }

            strftime(bufStart, sizeof(bufStart), "%H%M", localtime_r(&start, &tm_r));
            strftime(bufEnd, sizeof(bufEnd), "%H%M", localtime_r(&stop, &tm_r));

            eTimerMatch timerMatch;
            bool hasTimer = false;
            LOCK_TIMERS_READ;
            if (Timers->GetMatch(pEvent, &timerMatch))
                hasTimer = (timerMatch == tmFull);

            if (!result->search->useAsSearchTimer)
                result->needsTimer = false;

            LOCK_CHANNELS_READ;
            const cChannel *channel = Channels->GetByChannelID(pEvent->ChannelID(), true, true);
            int timerMode = hasTimer ? 1 : (result->needsTimer ? 2 : 0);

            std::string title = pEvent->Title() ? ReplaceAll(pEvent->Title(), "|", "!^pipe!^") : "";
            title = ReplaceAll(title, ":", "|");
            std::string shorttext = pEvent->ShortText() ? ReplaceAll(pEvent->ShortText(), "|", "!^pipe!^") : "";
            shorttext = ReplaceAll(shorttext, ":", "|");
            std::string description = pEvent->Description() ? ReplaceAll(pEvent->Description(), "|", "!^pipe!^") : "";
            description = ReplaceAll(description, ":", "|");

            cString cmdbuf = cString::sprintf("%d:%u:%s:%s:%s:%ld:%ld:%s:%ld:%ld:%s:%d",
                                              result->search->ID,
                                              pEvent->EventID(),
                                              title.c_str(),
                                              shorttext.c_str(),
                                              description.c_str(),
                                              pEvent->StartTime(),
                                              pEvent->EndTime(),
                                              CHANNELSTRING(channel),
                                              timerMode > 0 ? start : -1,
                                              timerMode > 0 ? stop : -1,
                                              timerMode > 0 ? result->search->BuildFile(pEvent) : "",
                                              timerMode);

            list.push_back(*cmdbuf);

            delete(Timer);
            result = pCompleteSearchResults->Next(result);
        }
    }
    return list;
}

std::list<std::string> cEpgsearchServiceHandler::QuerySearchTimer(int ID)
{
    std::list<std::string> list;

    cSearchResults* pCompleteSearchResults = NULL;
    cSearchExt* search = SearchExts.GetSearchFromID(ID);
    if (!search) return list;

    pCompleteSearchResults = search->Run();
    list = TranslateResults(pCompleteSearchResults);
    if (pCompleteSearchResults) delete pCompleteSearchResults;
    return list;
}

std::list<std::string> cEpgsearchServiceHandler::QuerySearch(std::string query)
{
    std::list<std::string> list;

    cSearchExt* temp_SearchExt = new cSearchExt;
    if (temp_SearchExt->Parse(query.c_str())) {
        cSearchResults* pCompleteSearchResults = temp_SearchExt->Run();
        list = TranslateResults(pCompleteSearchResults);
        if (pCompleteSearchResults) delete pCompleteSearchResults;
    }
    delete temp_SearchExt;
    return list;
}

std::list<std::string> cEpgsearchServiceHandler::ExtEPGInfoList()
{
    std::list<std::string> list;
    for (int i = 0; i < SearchExtCats.Count(); i++) {
        cSearchExtCat *SearchExtCat = SearchExtCats.Get(i);
        if (SearchExtCat)
            list.push_back(SearchExtCat->ToText());
    }
    return list;
}

std::list<std::string> cEpgsearchServiceHandler::ChanGrpList()
{
    std::list<std::string> list;
    for (int i = 0; i < ChannelGroups.Count(); i++) {
        cChannelGroup *changrp = ChannelGroups.Get(i);
        if (changrp)
            list.push_back(changrp->ToText());
    }
    return list;
}

std::list<std::string> cEpgsearchServiceHandler::BlackList()
{
    std::list<std::string> list;
    cMutexLock BlacklistLock(&Blacklists);

    for (int i = 0; i < Blacklists.Count(); i++) {
        cBlacklist* blacklist = Blacklists.Get(i);
        if (blacklist)
            list.push_back(blacklist->ToText());
    }
    return list;
}

std::set<std::string> cEpgsearchServiceHandler::DirectoryList()
{
    cMenuDirSelect::CreateDirSet();
    return cMenuDirSelect::directorySet;
}


std::string cEpgsearchServiceHandler::ReadSetupValue(const std::string& entry)
{
    if (entry == "DefPriority") return NumToString(EPGSearchConfig.DefPriority);
    if (entry == "DefLifetime") return NumToString(EPGSearchConfig.DefLifetime);
    if (entry == "DefMarginStart") return NumToString(EPGSearchConfig.DefMarginStart);
    if (entry == "DefMarginStop") return NumToString(EPGSearchConfig.DefMarginStop);
    return "";
}

bool cEpgsearchServiceHandler::WriteSetupValue(const std::string& entry, const std::string& value)
{
    return true;
}

std::list<std::string> cEpgsearchServiceHandler::TimerConflictList(bool relOnly)
{
    std::list<std::string> list;
    cConflictCheck conflictCheck;
    conflictCheck.SetLocal(); // remote Timers would give a wrong Id
    conflictCheck.Check();

    if ((relOnly && conflictCheck.numConflicts > 0) ||
        conflictCheck.relevantConflicts > 0) {
        string sBuffer;
        cList<cConflictCheckTime>* failedList = conflictCheck.GetFailed();
        for (cConflictCheckTime* ct = failedList->First(); ct; ct = failedList->Next(ct)) {
            if (relOnly && ct->ignore) continue;

            std::ostringstream conflline;
            conflline << ct->evaltime << ":";
            std::set<cConflictCheckTimerObj*, TimerObjSort>::iterator it;

            std::ostringstream timerparts;
            for (it = ct->failedTimers.begin(); it != ct->failedTimers.end(); ++it) {
                if (relOnly && (*it)->ignore) continue;
                std::ostringstream timerpart;
                int recPart = (*it)->recDuration * 100 / ((*it)->stop - (*it)->start);
                timerpart << (*it)->timer->Id() << "|" << recPart << "|";
                std::set<cConflictCheckTimerObj*, TimerObjSort>::iterator itcc;
                if ((*it)->concurrentTimers) {
                    std::ostringstream cctimers;
                    for (itcc = (*it)->concurrentTimers->begin(); itcc != (*it)->concurrentTimers->end(); ++itcc)
                        cctimers << (cctimers.str().empty() ? "" : "#") << (*itcc)->timer->Id();
                    timerpart << cctimers.str();
                }
                timerparts << (timerparts.str().empty() ? "" : ":") << timerpart.str();
            }
            conflline << timerparts.str();
            list.push_back(conflline.str());
        }
    }

    // set advised to false after an external conflict check
    if (gl_timerStatusMonitor) gl_timerStatusMonitor->SetConflictCheckAdvised(false);

    return list;
}

bool cEpgsearchServiceHandler::IsConflictCheckAdvised()
{
    return gl_timerStatusMonitor ? gl_timerStatusMonitor->ConflictCheckAdvised() : false;
}

std::set<std::string> cEpgsearchServiceHandler::ShortDirectoryList()
{
    cMenuDirSelect::CreateDirSet(false);
    return cMenuDirSelect::directorySet;
}

std::string cEpgsearchServiceHandler::Evaluate(const std::string& expr, const cEvent* event)
{
    if (!event) return expr;
    cVarExpr varExpr(expr);
    return varExpr.Evaluate(event);
}
