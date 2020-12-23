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

#ifndef __EPGSEARCH_MAIL_H
#define __EPGSEARCH_MAIL_H

#include <string>
#include <set>

#include <vdr/thread.h>
#include <vdr/plugin.h>
#include "conflictcheck.h"

using std::string;
using std::set;

// --- cMailNotifier --------------------------------------------------------
class cMailNotifier
{
protected:
    string subject;
    string body;

    bool SendMailViaSendmail();
    bool SendMailViaScript();
    bool SendMail(bool force = false);
    bool ExecuteMailScript(string ScriptArgs);
public:
    string scriptReply;

    cMailNotifier() {}
    cMailNotifier(string Subject, string Body);
    bool TestMailAccount(string MailAddressTo, string MailAddress, string MailServer, string AuthUser, string AuthPass);
    static string LoadTemplate(const string& templtype);
    static string GetTemplValue(const string& templ, const string& entry);

    static string MailCmd;
};

class cMailTimerNotification
{
    friend class cMailUpdateNotifier;
    tEventID eventID;
    tChannelID channelID;
    uint timerMod;

protected:
    virtual const cEvent* GetEvent() const;

public:
    cMailTimerNotification(tEventID EventID, tChannelID ChannelID, uint TimerMod = tmNoChange)
        : eventID(EventID), channelID(ChannelID), timerMod(TimerMod) {}
    virtual bool operator< (const cMailTimerNotification &N) const;
    virtual string Format(const string& templ) const;
};

class cMailDelTimerNotification
{
    friend class cMailUpdateNotifier;
    time_t start;
    tChannelID channelID;
public:
    string formatted;

    cMailDelTimerNotification(const cTimer* t, const cEvent* pEvent, const string& templ);
    cMailDelTimerNotification(const string& Formatted, tChannelID ChannelID, time_t Start);
    bool operator< (const cMailDelTimerNotification &N) const;
    string Format(const string& templ) const {
        return formatted;
    }
};

class cMailAnnounceEventNotification : public cMailTimerNotification
{
    friend class cMailUpdateNotifier;
    int searchextID;
public:
    cMailAnnounceEventNotification(tEventID EventID, tChannelID ChannelID, int SearchExtID)
        : cMailTimerNotification(EventID, ChannelID), searchextID(SearchExtID) {}
    string Format(const string& templ) const;
};

class cMailUpdateNotifier : public cMailNotifier
{
    set<cMailTimerNotification> newTimers;
    set<cMailTimerNotification> modTimers;
    set<cMailDelTimerNotification> delTimers;
    set<cMailAnnounceEventNotification> announceEvents;

    string mailTemplate;
public:
    cMailUpdateNotifier();
    void AddNewTimerNotification(tEventID EventID, tChannelID ChannelID);
    void AddModTimerNotification(tEventID EventID, tChannelID ChannelID, uint timerMod = tmNoChange);
    void AddRemoveTimerNotification(const cTimer* t, const cEvent* e = NULL);
    void AddRemoveTimerNotification(const string& Formatted, tChannelID ChannelID, time_t Start);
    void AddAnnounceEventNotification(tEventID EventID, tChannelID ChannelID, int SearchExtID);
    void SendUpdateNotifications();
};

class cMailConflictNotifier : public cMailNotifier
{
public:
    void SendConflictNotifications(cConflictCheck& conflictcheck);
};

#endif
