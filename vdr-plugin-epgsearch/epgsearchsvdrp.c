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
#include <set>
#include <iomanip>
#include <sstream>
#include <vdr/plugin.h>
#include <vdr/epg.h>
#include "epgsearchtools.h"
#include "recdone.h"
#include "epgsearch.h"
#include "changrp.h"
#include "epgsearchcfg.h"
#include "searchtimer_thread.h"
#include "blacklist.h"
#include "epgsearchcats.h"
#include "menu_dirselect.h"
#include "menu_deftimercheckmethod.h"
#include "conflictcheck.h"
#include "menu_main.h"

using std::string;
using std::set;

template< class Iter > Iter advance_copy(Iter it, std::size_t count = 1)
{
    using std::advance;
    advance(it, count);
    return it;
}

extern int updateForced;
extern int exitToMainMenu;

const char **cPluginEpgsearch::SVDRPHelpPages(void)
{
    static const char *HelpPages[] = {
        "LSTS [ ID ]\n"
        "    List searches.\n"
        "    If the optional numeric argument ID is passed,\n"
        "    only the search with the according ID is listed",
        "NEWS <settings>\n"
        "    Add a new search\n",
        "DELS <ID>\n"
        "    Delete search with passed ID\n",
        "EDIS <settings>\n"
        "    Edit an existing search\n",
        "MODS ID ON|OFF\n"
        "    Turn on/off 'Use as search timer'\n",
        "UPDS [ OSD ] [ SCAN ]\n"
        "    Update search timers.\n"
        "    If the optional keyword 'OSD' is passed, an OSD message\n"
        "    will inform about update completion. With 'SCAN' you can\n"
        "    trigger an EPG scan before the searchtimer udpate.",
        "UPDD\n"
        "    Reload epgsearchdone.data",
        "SETS <ON|OFF>\n"
        "    Temporarily activate or cancel the search timer background\n"
        "    thread.",
        "FIND <settings>\n"
        "    Search the EPG for events and receive a result list.",
        "QRYS < ID[|ID] >|<settings> \n"
        "    Search the EPG for events and receive a result list\n"
        "    for the given ID or any search settings. Separate multiple\n"
        "    searches with '|'",
        "QRYF [hours]\n"
        "    Search the EPG for favorite events with the next 24h or\n"
        "    the given number of hours",
        "LSRD\n"
        "    List of all recording directories used in recordings, timers,\n"
        "    search timers or in epgsearchdirs.conf",
        "LSTC [ channel group name ]\n"
        "    List all channel groups or if given the one with name\n"
        "    group name.",
        "NEWC <channel group settings>\n"
        "    Create a new channel group, format as in\n"
        "    epgsearchchangrps.conf.",
        "EDIC <channel group settings>\n"
        "    Modify an existing channel group, format as in\n"
        "    epgsearchchangrps.conf.",
        "DELC <channel group name>\n"
        "    Delete an existing channel group.",
        "RENC <old channelgroup name|new channel group name>\n"
        "    Rename an existing channel group.",
        "LSTB [ ID ]\n"
        "    List blacklists.\n"
        "    If the optional numeric argument ID is passed,\n"
        "    only the blacklist with the according ID is listed",
        "NEWB <settings>\n"
        "    Add a new blacklist",
        "DELB <ID>\n"
        "    Delete blacklist with passed ID",
        "EDIB <settings>\n"
        "    Edit an existing blacklist",
        "LSTE [ ID ]\n"
        "    List the extended EPG categories defined in\n"
        "    epgsearchcats.conf or only the one with the given ID",
        "SETP [ option ]\n"
        "    Get the current setup option value",
        "LSTT [ ID ]\n"
        "    List search templates.\n"
        "    If the optional numeric argument ID is passed,\n"
        "    only the search template with the according ID\n"
        "    is listed",
        "NEWT <settings>\n"
        "    Add a new search template\n",
        "DELT <ID>\n"
        "    Delete search template with passed ID\n",
        "EDIT <settings>\n"
        "    Edit an existing search template\n",
        "DEFT [ ID ]\n"
        "    Returns the ID of the default search template\n"
        "    or activates a search template with ID as default",
        "LSCC [ REL ]\n"
        "    Returns the current (local) timer conflicts. With the option\n"
        "    'REL' only relevant conflicts are listed",
        "MENU [ NOW|PRG|SUM ]\n"
        "    Calls one of the main menus of epgsearch or the summary\n"
        "    of the current event\n",
        "UPDT\n"
        "    Reload search timers from epgsearch.conf",
        NULL
    };
    return HelpPages;
}

cString cPluginEpgsearch::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
    if (strcasecmp(Command, "UPDS") == 0) {
        if (!EPGSearchConfig.useSearchTimers) // enable search timer thread if necessary
            cSearchTimerThread::Init((cPluginEpgsearch*) cPluginManager::GetPlugin("epgsearch"), true);
        updateForced = 1;
        if (Option) {
            char *pstrOptionToken, *pptr;
            char *pstrOptions = strdup(Option);
            pstrOptionToken = strtok_r(pstrOptions, " ", &pptr);
            while (pstrOptionToken) {
                if (strcasecmp(Option, "OSD") == 0)
                    updateForced |= UPDS_WITH_OSD;
                if (strcasecmp(Option, "SCAN") == 0)
                    updateForced |= UPDS_WITH_EPGSCAN;
                pstrOptionToken = strtok_r(NULL, "|", &pptr);
            }
            free(pstrOptions);
        }
        return cString("update triggered");
    } else if (strcasecmp(Command, "UPDD") == 0) {
        if (RecsDone.Load(AddDirectory(CONFIGDIR, "epgsearchdone.data")))
            return cString("reload successful");
        else
            return cString("reload failed");
    }
    // -----------------------
    // search timer management
    else if (strcasecmp(Command, "LSTS") == 0) {
        if (*Option) {
            if (isnumber(Option)) {
                cSearchExt *search = SearchExts.GetSearchFromID(atol(Option));
                if (search)
                    return cString(search->ToText());
                else {
                    ReplyCode = 901;
                    return cString::sprintf("search id %s not defined", Option);
                }
            } else {
                ReplyCode = 901;
                return cString::sprintf("Error in search ID \"%s\"", Option);
            }
        } else if (SearchExts.Count() > 0) {
            string sBuffer;
            cMutexLock SearchExtsLock(&SearchExts);
            for (int i = 0; i < SearchExts.Count(); i++) {
                cSearchExt* search = SearchExts.Get(i);
                if (search)
                    sBuffer += string(search->ToText()) + string((i < SearchExts.Count() - 1) ? "\n" : "");
            }
            return sBuffer.c_str();
        } else {
            ReplyCode = 901;
            return cString("no searches defined");
        }
    } else if (strcasecmp(Command, "DELS") == 0) {
        if (*Option) {
            string sOption = Option;
            bool delTimers = false;
            if (strcasestr(Option, "DELT")) {
                delTimers = true;
                sOption = ReplaceAll(sOption, "DELT", "");
                sOption = Strip(sOption);
            }
            if (isnumber(sOption.c_str())) {
                long SID = atol(sOption.c_str());
                cSearchExt *search = SearchExts.GetSearchFromID(SID);
                if (search) {
                    LogFile.Log(1, "search '%s' deleted via SVDRP", search->search);
                    cMutexLock SearchExtsLock(&SearchExts);
                    if (delTimers)
                        search->DeleteAllTimers();
                    SearchExts.Del(search);
                    SearchExts.Save();
                    RecsDone.RemoveSearchID(SID);
                    return cString::sprintf("search id %s deleted%s", sOption.c_str(), delTimers ? " with timers" : "");
                } else {
                    ReplyCode = 901;
                    return cString::sprintf("search id %s not defined", sOption.c_str());
                }
            } else {
                ReplyCode = 901;
                return cString::sprintf("Error in search ID \"%s\"", Option);
            }
        } else {
            ReplyCode = 901;
            return cString("missing search ID");
        }
    } else if (strcasecmp(Command, "NEWS") == 0) {
        if (*Option) {
            cSearchExt* search = new cSearchExt;
            if (search->Parse(Option)) {
                search->ID = SearchExts.GetNewID();
                LogFile.Log(1, "added search '%s' (%d) via SVDRP", search->search, search->ID);
                cMutexLock SearchExtsLock(&SearchExts);
                SearchExts.Add(search);
                SearchExts.Save();
                if (search->useAsSearchTimer && !EPGSearchConfig.useSearchTimers) // enable search timer thread if necessary
                    cSearchTimerThread::Init(this, true);
                return cString::sprintf("search '%s' (with new ID %d) added", search->search, search->ID);
            } else {
                ReplyCode = 901;
                delete search;
                return cString("Error in search settings");
            }
        } else {
            ReplyCode = 901;
            return cString("missing search settings");
        }
    } else if (strcasecmp(Command, "EDIS") == 0) {
        if (*Option) {
            cSearchExt* search = new cSearchExt;
            if (search->Parse(Option)) {
                cSearchExt *searchTemp = SearchExts.GetSearchFromID(search->ID);
                if (searchTemp) {
                    searchTemp->Parse(Option);
                    LogFile.Log(1, "modified search '%s' (%d) via SVDRP", searchTemp->search, searchTemp->ID);
                    SearchExts.Save();
                    if (searchTemp->useAsSearchTimer && !EPGSearchConfig.useSearchTimers) // enable search timer thread if necessary
                        cSearchTimerThread::Init((cPluginEpgsearch*) cPluginManager::GetPlugin("epgsearch"), true);

                    return cString::sprintf("search '%s' with %d modified", searchTemp->search, searchTemp->ID);
                } else {
                    ReplyCode = 901;
                    int ID = search->ID;
                    delete search;
                    return cString::sprintf("search id %d does not exists", ID);
                }
            } else {
                ReplyCode = 901;
                delete search;
                return cString("Error in search settings");
            }
        } else {
            ReplyCode = 901;
            return cString("missing search settings");
        }
    } else if (strcasecmp(Command, "MODS") == 0) {
        if (*Option) {
            char *tail;
            int ID = strtol(Option, &tail, 10);
            tail = skipspace(tail);
            if (!*tail) {
                ReplyCode = 901;
                return cString::sprintf("missing parameter ON|OFF");
            }
            cSearchExt *searchTemp = SearchExts.GetSearchFromID(ID);
            if (searchTemp) {
                searchTemp->useAsSearchTimer = (strcasecmp(tail, "ON") == 0) ? 1 : 0;
                LogFile.Log(1, "modified search '%s' (%d) via SVDRP", searchTemp->search, searchTemp->ID);
                SearchExts.Save();

                if (searchTemp->useAsSearchTimer && !EPGSearchConfig.useSearchTimers) // enable search timer thread if necessary
                    cSearchTimerThread::Init((cPluginEpgsearch*) cPluginManager::GetPlugin("epgsearch"), true);

                return cString::sprintf("search '%s' with ID %d modified", searchTemp->search, searchTemp->ID);
            } else {
                ReplyCode = 901;
                return cString::sprintf("search id %d does not exists", ID);
            }
        } else {
            ReplyCode = 901;
            return cString("missing search ID");
        }
    } else if (strcasecmp(Command, "SETS") == 0) {
        if (*Option) {
            if (strcasecmp(Option, "ON") == 0) {
                if (cSearchTimerThread::m_Instance) {
                    ReplyCode = 901;
                    return cString("search timer thread already active!");
                } else {
                    LogFile.Log(1, "search timer thread started via SVDRP");
                    cSearchTimerThread::Init(this);
                    return cString("search timer activated.");
                }
            } else if (strcasecmp(Option, "OFF") == 0) {
                if (!cSearchTimerThread::m_Instance) {
                    ReplyCode = 901;
                    return cString("search timer thread already inactive!");
                } else {
                    LogFile.Log(1, "search timer thread canceled via SVDRP");
                    cSearchTimerThread::Exit();
                    return cString("search timer thread canceled.");
                }
            } else {
                ReplyCode = 901;
                return cString::sprintf("unknown option '%s'", Option);
            }
        } else {
            ReplyCode = 901;
            return cString("missing option <on|off>");
        }
    } else if (strcasecmp(Command, "FIND") == 0) {
        if (*Option) {
            cSearchExt* search = new cSearchExt;
            if (search->Parse(Option)) {
                cSearchResults* results = search->Run();
                // transfer to result list
                string sBuffer;
                if (results) {
                    results->SortBy(CompareEventTime);
                    cSearchResult *result = results->First();
                    while (result) {
                        const cEvent* pEvent = result->event;
                        cTimer* Timer = new cTimer(pEvent);

                        static char bufStart[25];
                        static char bufEnd[25];

                        struct tm tm_r;
                        time_t eStart = pEvent->StartTime();
                        time_t eStop = pEvent->EndTime();
                        time_t start = eStart - (search->MarginStart * 60);
                        time_t stop  = eStop + (search->MarginStop * 60);
                        int Flags = Timer->Flags();
                        if (search->useVPS && pEvent->Vps() && Setup.UseVps) {
                            start = pEvent->Vps();
                            stop = start + pEvent->Duration();
                        } else
                            Flags = 1; // don't use VPS, if not set in this search

                        strftime(bufStart, sizeof(bufStart), "%H%M", localtime_r(&start, &tm_r));
                        strftime(bufEnd, sizeof(bufEnd), "%H%M", localtime_r(&stop, &tm_r));

                        cString cmdbuf = cString::sprintf("NEWT %d:%d:%s:%s:%s:%d:%d:%s:%s",
                                                          Flags,
                                                          Timer->Channel()->Number(),
                                                          *Timer->PrintDay(start, Timer->WeekDays(), true),
                                                          bufStart,
                                                          bufEnd,
                                                          search->Priority,
                                                          search->Lifetime,
                                                          Timer->File(),
                                                          "");


                        sBuffer += string(cmdbuf) + string(results->Next(result) ? "\n" : "");
                        delete(Timer);
                        result = results->Next(result);
                    }
                    return sBuffer.c_str();
                } else {
                    ReplyCode = 901;
                    delete search;
                    return cString("no results");
                }
            } else {
                ReplyCode = 901;
                delete search;
                return cString("Error in search settings");
            }
        } else {
            ReplyCode = 901;
            return cString("missing search settings");
        }
    } else if (strcasecmp(Command, "QRYS") == 0 || strcasecmp(Command, "QRYF") == 0) {
        cSearchExt *temp_SearchExt = NULL;
        cSearchResults* pCompleteSearchResults = NULL;
        if (strcasecmp(Command, "QRYS") == 0) { // query one or more searches
            if (*Option) {
                if (strchr(Option, ':')) {
                    cSearchExt* temp_SearchExt = new cSearchExt;
                    if (temp_SearchExt->Parse(Option))
                        pCompleteSearchResults = temp_SearchExt->Run();
                } else {
                    char *pstrSearchToken, *pptr;
                    char *pstrSearch = strdup(Option);
                    pstrSearchToken = strtok_r(pstrSearch, "|", &pptr);

                    while (pstrSearchToken) {
                        cSearchExt* search = SearchExts.GetSearchFromID(atoi(pstrSearchToken));
                        if (search)
                            pCompleteSearchResults = search->Run(-1, false, 0, pCompleteSearchResults);
                        pstrSearchToken = strtok_r(NULL, "|", &pptr);
                    }
                    free(pstrSearch);
                }
            } else {
                ReplyCode = 901;
                return cString("missing search IDs");
            }
        } else { // query the favorites
            int hours = EPGSearchConfig.FavoritesMenuTimespan;
            if (*Option)
                hours = atoi(Option);

            cMutexLock SearchExtsLock(&SearchExts);
            cSearchExt *SearchExt = SearchExts.First();
            while (SearchExt) {
                if (SearchExt->useInFavorites)
                    pCompleteSearchResults = SearchExt->Run(-1, false, 60 * hours, pCompleteSearchResults);
                SearchExt = SearchExts.Next(SearchExt);
            }
        }

        if (pCompleteSearchResults) {
            // transfer to result list
            string sBuffer;
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

                string title = pEvent->Title() ? ReplaceAll(pEvent->Title(), "|", "!^pipe!^") : "";
                title = ReplaceAll(title, ":", "|");
                string shorttext = pEvent->ShortText() ? ReplaceAll(pEvent->ShortText(), "|", "!^pipe!^") : "";
                shorttext = ReplaceAll(shorttext, ":", "|");

                cString cmdbuf = cString::sprintf("%d:%u:%s:%s:%ld:%ld:%s:%ld:%ld:%s:%d",
                                                  result->search->ID,
                                                  pEvent->EventID(),
                                                  title.c_str(),
                                                  shorttext.c_str(),
                                                  pEvent->StartTime(),
                                                  pEvent->EndTime(),
                                                  CHANNELSTRING(channel),
                                                  timerMode > 0 ? start : -1,
                                                  timerMode > 0 ? stop : -1,
                                                  timerMode > 0 ? result->search->BuildFile(pEvent) : "",
                                                  timerMode);

                sBuffer += string(cmdbuf) + string(pCompleteSearchResults->Next(result) ? "\n" : "");
                delete(Timer);
                result = pCompleteSearchResults->Next(result);
            }
            if (temp_SearchExt) delete temp_SearchExt;
            if (pCompleteSearchResults) delete pCompleteSearchResults;
            return sBuffer.c_str();
        } else {
            ReplyCode = 901;
            if (temp_SearchExt) delete temp_SearchExt;
            return cString("no results");
        }
    } else if (strcasecmp(Command, "UPDT") == 0) {
        if (SearchExts.Load(AddDirectory(CONFIGDIR, "epgsearch.conf")))
            return cString("reload epgsearch.conf successful");
        else
            return cString("reload epgsearch.conf failed");
    }
    // ---------------------
    // recording directories
    else if (strcasecmp(Command, "LSRD") == 0) {
        cMenuDirSelect::CreateDirSet();

        if (cMenuDirSelect::directorySet.size() > 0) {
            cString sBuffer("");
            std::set<string>::iterator it;
            for (it = cMenuDirSelect::directorySet.begin(); it != cMenuDirSelect::directorySet.end(); ++it) {
                cString sOldBuffer = sBuffer;
                sBuffer = cString::sprintf("%s%s\n", *sOldBuffer, (*it).c_str());
            }
            char* buffer = strdup(*sBuffer);
            cString sResBuffer = cString(buffer);
            free(buffer);
            return sResBuffer;
        } else {
            ReplyCode = 901;
            return cString("no recording directories found");
        }
    }
    // -------------------------
    // channel groups management
    else if (strcasecmp(Command, "LSTC") == 0) {
        if (*Option) {
            cChannelGroup *changrp = ChannelGroups.GetGroupByName(Option);
            if (changrp)
                return cString(changrp->ToText());
            else {
                ReplyCode = 901;
                return cString::sprintf("channel group '%s' not defined", Option);
            }
        } else if (ChannelGroups.Count() > 0) {
            cString sBuffer("");
            for (int i = 0; i < ChannelGroups.Count(); i++) {
                cChannelGroup *changrp = ChannelGroups.Get(i);
                if (changrp) {
                    cString sOldBuffer = sBuffer;
                    sBuffer = cString::sprintf("%s%s\n", *sOldBuffer, changrp->ToText());
                }
            }
            char* buffer = strdup(*sBuffer);
            cString sResBuffer = cString(buffer);
            free(buffer);
            return sResBuffer;
        } else {
            ReplyCode = 901;
            return cString("no channel groups defined");
        }
    } else if (strcasecmp(Command, "EDIC") == 0) {
        if (*Option) {
            cChannelGroup *changrp = new cChannelGroup;
            if (changrp->Parse(Option)) {
                cChannelGroup *changrpTemp = ChannelGroups.GetGroupByName(changrp->name);
                if (changrpTemp) {
                    changrpTemp->channels.Clear();
                    changrpTemp->Parse(Option);
                    LogFile.Log(1, "modified channel group '%s' via SVDRP", changrpTemp->name);
                    ChannelGroups.Save();
                    return cString::sprintf("channel group '%s' modified", changrpTemp->name);
                } else {
                    ReplyCode = 901;
                    delete changrp;
                    return cString::sprintf("channel group '%s' does not exists", Option);
                }
            } else {
                ReplyCode = 901;
                delete changrp;
                return cString("Error in channel group settings");
            }
        } else {
            ReplyCode = 901;
            return cString("missing channel group settings");
        }
    } else if (strcasecmp(Command, "DELC") == 0) {
        if (*Option) {
            cChannelGroup *changrp = ChannelGroups.GetGroupByName(Option);
            if (changrp) {
                cSearchExt* search = ChannelGroups.Used(changrp);
                if (search) {
                    ReplyCode = 901;
                    return cString::sprintf("channel group '%s' used by: %s", changrp->name, search->search);
                }
                LogFile.Log(1, "channel group '%s' deleted via SVDRP", changrp->name);
                ChannelGroups.Del(changrp);
                ChannelGroups.Save();
                return cString::sprintf("channel group '%s' deleted", Option);
            } else {
                ReplyCode = 901;
                return cString::sprintf("channel group '%s' not defined", Option);
            }
        } else {
            ReplyCode = 901;
            return cString("missing channel group");
        }
    } else if (strcasecmp(Command, "NEWC") == 0) {
        if (*Option) {
            cChannelGroup *changrp = new cChannelGroup;
            if (changrp->Parse(Option)) {
                LogFile.Log(1, "added channel group '%s' via SVDRP", changrp->name);
                ChannelGroups.Add(changrp);
                ChannelGroups.Save();
                return cString::sprintf("channel group '%s' added", changrp->name);
            } else {
                ReplyCode = 901;
                delete changrp;
                return cString("Error in channel group settings");
            }
        } else {
            ReplyCode = 901;
            return cString("missing channel group settings");
        }
    } else if (strcasecmp(Command, "RENC") == 0) {
        if (*Option) {
            const char* pipePos = strchr(Option, '|');
            if (pipePos) {
                int index = pipePos - Option;
                char* oldName = strdup(Option);
                *(oldName + index) = 0;
                const char* newName = oldName + index + 1;
                if (strlen(oldName) > 0 && strlen(newName) > 0) {
                    cChannelGroup *changrp = ChannelGroups.GetGroupByName(oldName);
                    if (changrp) {
                        strcpy(changrp->name, newName);
                        cMutexLock SearchExtsLock(&SearchExts);
                        cSearchExt *SearchExt = SearchExts.First();
                        while (SearchExt) {
                            if (SearchExt->useChannel == 2 &&
                                SearchExt->channelGroup &&
                                strcmp(SearchExt->channelGroup, oldName) == 0) {
                                free(SearchExt->channelGroup);
                                SearchExt->channelGroup = strdup(newName);
                            }
                            SearchExt = SearchExts.Next(SearchExt);
                        }
                        ChannelGroups.Save();
                        SearchExts.Save();
                        cString strReturn = cString::sprintf("renamed channel group '%s' to '%s'", oldName, newName);
                        free(oldName);
                        return strReturn;

                    } else {
                        free(oldName);
                        ReplyCode = 901;
                        return cString::sprintf("channel group '%s' not defined", Option);
                    }
                }
                free(oldName);
            }
            ReplyCode = 901;
            return cString("Error in channel group parameters");
        } else {
            ReplyCode = 901;
            return cString("missing channel group parameters");
        }
    }
    // --------------------
    // blacklist management
    else if (strcasecmp(Command, "LSTB") == 0) {
        if (*Option) {
            if (isnumber(Option)) {
                cBlacklist *blacklist = Blacklists.GetBlacklistFromID(atol(Option));
                if (blacklist)
                    return cString(blacklist->ToText());
                else {
                    ReplyCode = 901;
                    return cString::sprintf("blacklist id %s not defined", Option);
                }
            } else {
                ReplyCode = 901;
                return cString::sprintf("Error in blacklist ID \"%s\"", Option);
            }
        } else if (Blacklists.Count() > 0) {
            cMutexLock BlacklistLock(&Blacklists);
            string sBuffer;
            for (int i = 0; i < Blacklists.Count(); i++) {
                cBlacklist *blacklist = Blacklists.Get(i);
                if (blacklist)
                    sBuffer += string(blacklist->ToText()) + string((i < Blacklists.Count() - 1) ? "\n" : "");
            }
            return sBuffer.c_str();
        } else {
            ReplyCode = 901;
            return cString("no blacklists defined");
        }
    } else if (strcasecmp(Command, "DELB") == 0) {
        if (*Option) {
            if (isnumber(Option)) {
                cBlacklist *blacklist = Blacklists.GetBlacklistFromID(atol(Option));
                if (blacklist) {
                    LogFile.Log(1, "blacklist '%s' deleted via SVDRP", blacklist->search);
                    SearchExts.RemoveBlacklistID(blacklist->ID);
                    cMutexLock BlacklistLock(&Blacklists);
                    Blacklists.Del(blacklist);
                    Blacklists.Save();
                    RecsDone.RemoveSearchID(atol(Option));
                    return cString::sprintf("blacklist id %s deleted", Option);
                } else {
                    ReplyCode = 901;
                    return cString::sprintf("blacklist id %s not defined", Option);
                }
            } else {
                ReplyCode = 901;
                return cString::sprintf("Error in blacklist ID \"%s\"", Option);
            }
        } else {
            ReplyCode = 901;
            return cString("missing blacklist ID");
        }
    } else if (strcasecmp(Command, "NEWB") == 0) {
        if (*Option) {
            cBlacklist *blacklist = new cBlacklist;
            if (blacklist->Parse(Option)) {
                blacklist->ID = Blacklists.GetNewID();
                LogFile.Log(1, "added blacklist '%s' (%d) via SVDRP", blacklist->search, blacklist->ID);
                cMutexLock BlacklistLock(&Blacklists);
                Blacklists.Add(blacklist);
                Blacklists.Save();
                return cString::sprintf("blacklist '%s' (with new ID %d) added", blacklist->search, blacklist->ID);
            } else {
                ReplyCode = 901;
                delete blacklist;
                return cString("Error in blacklist settings");
            }
        } else {
            ReplyCode = 901;
            return cString("missing blacklist settings");
        }
    } else if (strcasecmp(Command, "EDIB") == 0) {
        if (*Option) {
            cBlacklist *blacklist = new cBlacklist;
            if (blacklist->Parse(Option)) {
                cBlacklist *blacklistTemp = Blacklists.GetBlacklistFromID(blacklist->ID);
                if (blacklistTemp) {
                    blacklistTemp->Parse(Option);
                    LogFile.Log(1, "modified blacklist '%s' (%d) via SVDRP", blacklistTemp->search, blacklistTemp->ID);
                    Blacklists.Save();
                    return cString::sprintf("blacklist '%s' with ID %d modified", blacklistTemp->search, blacklistTemp->ID);
                } else {
                    ReplyCode = 901;
                    int ID = blacklist->ID;
                    delete blacklist;
                    return cString::sprintf("blacklist id %d does not exists", ID);
                }
            } else {
                ReplyCode = 901;
                delete blacklist;
                return cString("Error in blacklist settings");
            }
        } else {
            ReplyCode = 901;
            return cString("missing blacklist settings");
        }
    }
    // ----------------------------------
    // extended EPG categories management
    else if (strcasecmp(Command, "LSTE") == 0) {
        if (*Option) {
            if (isnumber(Option)) {
                cSearchExtCat *SearchExtCat = NULL;
                int index = SearchExtCats.GetIndexFromID(atoi(Option));
                if (index >= 0) SearchExtCat = SearchExtCats.Get(index);
                if (SearchExtCat)
                    return cString(SearchExtCat->ToText());
                else {
                    ReplyCode = 901;
                    return cString::sprintf("category id %s not defined", Option);
                }
            } else {
                ReplyCode = 901;
                return cString::sprintf("Error in category ID \"%s\"", Option);
            }
        } else if (SearchExtCats.Count() > 0) {
            string sBuffer;
            for (int i = 0; i < SearchExtCats.Count(); i++) {
                cSearchExtCat *SearchExtCat = SearchExtCats.Get(i);
                if (SearchExtCat)
                    sBuffer += string(SearchExtCat->ToText()) + string((i < SearchExtCats.Count() - 1) ? "\n" : "");
            }
            return sBuffer.c_str();
        } else {
            ReplyCode = 901;
            return cString("no EPG categories defined");
        }
    }
    // ------------
    // setup values
    else if (strcasecmp(Command, "SETP") == 0) {
        if (*Option) {
            if (strcasecmp(Option, "ShowFavoritesMenu") == 0)
                return cString::sprintf("%d", EPGSearchConfig.showFavoritesMenu);
            else if (strcasecmp(Option, "UseSearchTimers") == 0)
                return cString::sprintf("%d", EPGSearchConfig.useSearchTimers);
            else if (strcasecmp(Option, "DefRecordingDir") == 0) {
                if (strlen(EPGSearchConfig.defrecdir) > 0)
                    return cString::sprintf("%s", EPGSearchConfig.defrecdir);
                else {
                    ReplyCode = 901;
                    return cString::sprintf("empty");
                }
            } else if (strcasecmp(Option, "AddSubtitleToTimerMode") == 0)
                return cString::sprintf("%d", EPGSearchConfig.addSubtitleToTimer);
            else if (strcasecmp(Option, "DefPriority") == 0)
                return cString::sprintf("%d", EPGSearchConfig.DefPriority);
            else if (strcasecmp(Option, "DefLifetime") == 0)
                return cString::sprintf("%d", EPGSearchConfig.DefLifetime);
            else if (strcasecmp(Option, "DefMarginStart") == 0)
                return cString::sprintf("%d", EPGSearchConfig.DefMarginStart);
            else if (strcasecmp(Option, "DefMarginStop") == 0)
                return cString::sprintf("%d", EPGSearchConfig.DefMarginStop);
            else if (strcasestr(Option, "DefTimerCheckMethod") == Option) {
                tChannelID chID;
                if (strlen(Option) > strlen("DefTimerCheckMethod") + 1) {
                    chID = tChannelID::FromString(Option + strlen("DefTimerCheckMethod") + 1);
                    if (!chID.Valid()) {
                        ReplyCode = 901;
                        return cString::sprintf("invalid channel id");
                    }
                    LOCK_CHANNELS_READ;
                    const cChannel *ch = Channels->GetByChannelID(chID, true, true);
                    if (!ch) {
                        ReplyCode = 901;
                        return cString::sprintf("unknown channel");
                    }
                    return cString::sprintf("%s: %d", *ch->GetChannelID().ToString(), DefTimerCheckModes.GetMode(ch));
                } else {
                    LOCK_CHANNELS_READ;
                    string sBuffer;
                    for (int i = 0; i < Channels->Count(); i++) {
                        const cChannel* ch = Channels->Get(i);
                        if (ch && !ch->GroupSep())
                            sBuffer += string(*ch->GetChannelID().ToString()) + string(": ") + NumToString(DefTimerCheckModes.GetMode(ch)) + string((i < Channels->Count() - 1) ? "\n" : "");
                    }
                    return sBuffer.c_str();
                }
            } else {
                ReplyCode = 901;
                return cString::sprintf("setup option not supported");
            }
        } else {
            string sBuffer;
            sBuffer += "ShowFavoritesMenu: " + NumToString(EPGSearchConfig.showFavoritesMenu) + "\n";
            sBuffer += "UseSearchTimers: " + NumToString(EPGSearchConfig.useSearchTimers) + "\n";
            sBuffer += "DefRecordingDir: " + string(EPGSearchConfig.defrecdir) + "\n";
            sBuffer += "AddSubtitleToTimerMode: " + NumToString(EPGSearchConfig.addSubtitleToTimer) + "\n";
            sBuffer += "DefPriority: " + NumToString(EPGSearchConfig.DefPriority) + "\n";
            sBuffer += "DefLifetime: " + NumToString(EPGSearchConfig.DefLifetime) + "\n";
            sBuffer += "DefMarginStart: " + NumToString(EPGSearchConfig.DefMarginStart) + "\n";
            sBuffer += "DefMarginStop: " + NumToString(EPGSearchConfig.DefMarginStop);

            return sBuffer.c_str();
        }
    }
    // ---------------------------------
    // search timer templates management
    else if (strcasecmp(Command, "LSTT") == 0) {
        if (*Option) {
            if (isnumber(Option)) {
                cSearchExt *search = SearchTemplates.GetSearchFromID(atol(Option));
                if (search)
                    return cString(search->ToText());
                else {
                    ReplyCode = 901;
                    return cString::sprintf("search template id %s not defined", Option);
                }
            } else {
                ReplyCode = 901;
                return cString::sprintf("Error in search template ID \"%s\"", Option);
            }
        } else if (SearchTemplates.Count() > 0) {
            string sBuffer;
            cMutexLock SearchExtsLock(&SearchTemplates);
            for (int i = 0; i < SearchTemplates.Count(); i++) {
                cSearchExt* search = SearchTemplates.Get(i);
                if (search)
                    sBuffer += string(search->ToText()) + string((i < SearchTemplates.Count() - 1) ? "\n" : "");
            }
            return sBuffer.c_str();
        } else {
            ReplyCode = 901;
            return cString("no search templates defined");
        }
    } else if (strcasecmp(Command, "DELT") == 0) {
        if (*Option) {
            if (isnumber(Option)) {
                cSearchExt *search = SearchTemplates.GetSearchFromID(atoi(Option));
                if (search) {
                    LogFile.Log(1, "search template '%s' deleted via SVDRP", search->search);
                    cMutexLock SearchExtsLock(&SearchTemplates);
                    SearchTemplates.Del(search);
                    SearchTemplates.Save();
                    return cString::sprintf("search template id %s deleted", Option);
                } else {
                    ReplyCode = 901;
                    return cString::sprintf("search template id %s not defined", Option);
                }
            } else {
                ReplyCode = 901;
                return cString::sprintf("Error in search template ID \"%s\"", Option);
            }
        } else {
            ReplyCode = 901;
            return cString("missing search template ID");
        }
    } else if (strcasecmp(Command, "NEWT") == 0) {
        if (*Option) {
            cSearchExt* search = new cSearchExt;
            if (search->Parse(Option)) {
                search->ID = SearchTemplates.GetNewID();
                LogFile.Log(1, "added search template '%s' (%d) via SVDRP", search->search, search->ID);
                cMutexLock SearchExtsLock(&SearchTemplates);
                SearchTemplates.Add(search);
                SearchTemplates.Save();
                return cString::sprintf("search template '%s' (with new ID %d) added", search->search, search->ID);
            } else {
                ReplyCode = 901;
                delete search;
                return cString("Error in search template settings");
            }
        } else {
            ReplyCode = 901;
            return cString("missing search template settings");
        }
    } else if (strcasecmp(Command, "EDIT") == 0) {
        if (*Option) {
            cSearchExt* search = new cSearchExt;
            if (search->Parse(Option)) {
                cSearchExt *searchTemp = SearchTemplates.GetSearchFromID(search->ID);
                if (searchTemp) {
                    searchTemp->Parse(Option);
                    LogFile.Log(1, "modified search template '%s' (%d) via SVDRP", searchTemp->search, searchTemp->ID);
                    SearchTemplates.Save();
                    return cString::sprintf("search template '%s' with ID %d modified", searchTemp->search, searchTemp->ID);
                } else {
                    ReplyCode = 901;
                    int ID = search->ID;
                    delete search;
                    return cString::sprintf("search template id %d does not exists", ID);
                }
            } else {
                ReplyCode = 901;
                delete search;
                return cString("Error in search template settings");
            }
        } else {
            ReplyCode = 901;
            return cString("missing search template settings");
        }
    } else if (strcasecmp(Command, "DEFT") == 0) {
        if (*Option) {
            cSearchExt *searchTemp = SearchTemplates.GetSearchFromID(atoi(Option));
            if (searchTemp) {
                LogFile.Log(1, "set search template '%s' (%d) as default via SVDRP", searchTemp->search, searchTemp->ID);
                EPGSearchConfig.DefSearchTemplateID = searchTemp->ID;
                cPluginManager::GetPlugin("epgsearch")->SetupStore("DefSearchTemplateID",  EPGSearchConfig.DefSearchTemplateID);
                return cString::sprintf("search template '%s' with ID %d set as default", searchTemp->search, searchTemp->ID);
            } else {
                ReplyCode = 901;
                return cString::sprintf("search template id %s does not exists", Option);
            }
        } else
            return cString::sprintf("%d", EPGSearchConfig.DefSearchTemplateID);
    }

    // ---------------------------------
    // timer conflicts
    else if (strcasecmp(Command, "LSCC") == 0) {
        bool relOnly = false;
        if (*Option && strcasecmp(Option, "REL") == 0)
            relOnly = true;

        LogFile.Log(3, "svdrp LSCC");
        cConflictCheck conflictCheck;
        conflictCheck.SetLocal(); // including remote timers results in an infinite loop
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
                sBuffer += conflline.str() + "\n";
            }
            return sBuffer.c_str();
        } else {
            ReplyCode = 901;
            return cString("no conflicts found");
        }
    } else if (strcasecmp(Command, "MENU") == 0) {
        if (*Option) {
            if (cMenuSearchMain::forceMenu == 0) {
                if (strcasecmp(Option, "PRG") == 0)
                    cMenuSearchMain::forceMenu = 2;
                else if (strcasecmp(Option, "NOW") == 0)
                    cMenuSearchMain::forceMenu = 1;
                else if (strcasecmp(Option, "SUM") == 0)
                    cMenuSearchMain::forceMenu = 3;
                else {
                    ReplyCode = 901;
                    return cString::sprintf("unknown option '%s'", Option);
                }
                cRemote::CallPlugin("epgsearch");
                return "menu called";
            } else {
                cRemote::Put(kBack);
                exitToMainMenu = 1;
                return "menu closed";
            }
        }
    }

    return NULL;
}
