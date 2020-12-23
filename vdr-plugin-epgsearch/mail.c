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

#include <sstream>
#include <iomanip>
#include <fstream>

#include "mail.h"
#include "epgsearchcfg.h"
#include "log.h"
#include "epgsearchtools.h"
#include "uservars.h"
#include "noannounce.h"
#include "pending_notifications.h"

#ifndef SENDMAIL
#define SENDMAIL "/usr/sbin/sendmail"
#endif

extern bool isUTF8;
using namespace std;

string cMailNotifier::MailCmd = "sendEmail.pl";

// ----------------------
// cMailTimerNotification
string cMailTimerNotification::Format(const string& templ) const
{
    const cEvent* pEvent = GetEvent();
    if (!pEvent) return "";

    eTimerMatch TimerMatch = tmNone;
    LOCK_TIMERS_READ;
    const cTimer* pTimer = Timers->GetMatch(pEvent, &TimerMatch);
    if (!pTimer) return "";

    string result = templ;
    cVarExpr varExprEvent(result);
    result =  varExprEvent.Evaluate(pEvent);
    cVarExpr varExprTimer(result);
    result =  varExprTimer.Evaluate(pTimer);

    if (timerMod == tmStartStop)
        result = ReplaceAll(result, "%timer.modreason%", tr("Start/Stop time has changed"));
    if (timerMod == tmFile)
        result = ReplaceAll(result, "%timer.modreason%", tr("Title/episode has changed"));
    else
        result = ReplaceAll(result, "%timer.modreason%", "");
    return result;
}

const cEvent* cMailTimerNotification::GetEvent() const
{
    LOCK_SCHEDULES_READ;
    if (!Schedules) return NULL;
    const cSchedule *schedule = Schedules->GetSchedule(channelID);
    if (!schedule) return NULL;
    return schedule->GetEvent(eventID);
}

bool cMailTimerNotification::operator< (const cMailTimerNotification &N) const
{
    LOCK_CHANNELS_READ;
    const cChannel* channel = Channels->GetByChannelID(channelID, true, true);
    const cChannel* channelOther = Channels->GetByChannelID(N.channelID, true, true);
    if (!channel || !channelOther)
        return false;
    const cEvent* event = GetEvent();
    const cEvent* eventOther = N.GetEvent();
    if (event && eventOther) { // sort event by start time and channel
        if (event->StartTime() == eventOther->StartTime())
            return channel->Number() < channelOther->Number();
        else
            return event->StartTime() < eventOther->StartTime();
    }
    return false;
}

// -------------------------
// cMailDelTimerNotification
cMailDelTimerNotification::cMailDelTimerNotification(const cTimer* pTimer, const cEvent* pEvent, const string& templ)
{
    if (!pTimer || !pTimer->Channel()) return;

    channelID = pTimer->Channel()->GetChannelID();
    start = pTimer->StartTime();

    string result = templ;
    cVarExpr varExprEvent(result);
    result =  varExprEvent.Evaluate(pEvent);
    cVarExpr varExprTimer(result);
    formatted =  varExprTimer.Evaluate(pTimer);
}

cMailDelTimerNotification::cMailDelTimerNotification(const string& Formatted, tChannelID ChannelID, time_t Start)
{
    formatted = Formatted;
    channelID = ChannelID;
    start = Start;
}

bool cMailDelTimerNotification::operator< (const cMailDelTimerNotification &N) const
{
    LOCK_CHANNELS_READ;
    const cChannel* channel = Channels->GetByChannelID(channelID, true, true);
    const cChannel* channelOther = Channels->GetByChannelID(N.channelID, true, true);
    if (!channel || !channelOther)
        return false;
    if (channel != channelOther)
        return channel->Number() < channelOther->Number();
    else
        return (start < N.start);
}

// ----------------------
// cMailAnnounceEventNotification
string cMailAnnounceEventNotification::Format(const string& templ) const
{
    const cEvent* pEvent = GetEvent();
    if (!pEvent) return "";

    string result = templ;
    cVarExpr varExprEvent(result);
    result =  varExprEvent.Evaluate(pEvent);

    result = ReplaceAll(result, "%searchid%", NumToString(searchextID));
    cSearchExt* search = SearchExts.GetSearchFromID(searchextID);
    if (search)
        result = ReplaceAll(result, "%search%", search->search);
    return result;
}

// -------------
// cMailNotifier
cMailNotifier::cMailNotifier(string Subject, string Body)
    : subject(Subject), body(Body)
{
    if (subject.size() > 0)
        SendMail(true);
}

bool cMailNotifier::SendMailViaSendmail()
{
    char mailcmd[256];
    const char* mailargs = "%s -i -FVDR -oem  %s";
    const char* mailproc = SENDMAIL;
    FILE* mail;

    string to = EPGSearchConfig.MailAddressTo;
    snprintf(mailcmd, sizeof(mailcmd), mailargs, mailproc, to.c_str());

    if (!(mail = popen(mailcmd, "w"))) {
        return false;
    }

    fprintf(mail, "From: VDR\n");
    fprintf(mail, "To: %s\n", to.c_str());
    fprintf(mail, "Subject: %s\n", subject.c_str());
    if (FindIgnoreCase(body, "<html>") >= 0)
        fprintf(mail, "Content-Type: text/html; charset=%s\n", GetCodeset().c_str());
    else
        fprintf(mail, "Content-Type: text/plain; charset=%s\n", GetCodeset().c_str());

    fprintf(mail, "\n");

    fprintf(mail, "%s", body.c_str());

    pclose(mail);

    return true;
}

bool cMailNotifier::SendMailViaScript()
{
    // create a temporary file for the message body
    string filename = *AddDirectory(CONFIGDIR, "epgsearchmail.temp");
    std::ofstream bodyfile(filename.c_str());
    if (!bodyfile) {
        LogFile.eSysLog("error opening file %s", filename.c_str());
        return false;
    }
    bodyfile << body;
    bodyfile.close();

    string AuthUser = EPGSearchConfig.MailAuthUser;
    string AuthPass = EPGSearchConfig.MailAuthPass;
    string cmdArgs =
        string(" -f \"VDR <") + EPGSearchConfig.MailAddress + ">\"" +
        " -t " + EPGSearchConfig.MailAddressTo +
        " -s " + EPGSearchConfig.MailServer +
        " -u \"" + subject + "\"" +
        (EPGSearchConfig.MailUseAuth ?
         (AuthUser != "" ? (" -xu " + AuthUser) : "") +
         (AuthPass != "" ? (" -xp " + AuthPass) : "")
             : "") +
            " -o message-charset=" + GetCodeset() +
            " -o message-file=" + filename;

    bool success = ExecuteMailScript(cmdArgs);

    if (remove(filename.c_str()) != 0)
        LogFile.eSysLog("error deleting file %s", filename.c_str());

    return success;
}

bool cMailNotifier::SendMail(bool force)
{
    time_t nextMailDelivery = EPGSearchConfig.lastMailOnSearchtimerAt + EPGSearchConfig.sendMailOnSearchtimerHours * 60 * 60;
    if (time(NULL) > nextMailDelivery || force) {
        if (!EPGSearchConfig.mailViaScript)
            return SendMailViaSendmail();
        else
            return SendMailViaScript();
    } else {

        LogFile.Log(2, "mail delivery delayed until %s", DAYDATETIME(nextMailDelivery));
        return false;
    }
}

bool cMailNotifier::ExecuteMailScript(string ScriptArgs)
{
    string mailCmd = MailCmd;
    LogFile.Log(3, "starting mail script: %s with parameters: %s", mailCmd.c_str(), ScriptArgs.c_str());
    if (mailCmd == "sendEmail.pl") // beautify output for standard script
        ScriptArgs += " | cut -d\" \" -f 6-";

    cCommand cmd;
    string fullcmd = "mailcmd: " + mailCmd;
    if (!cmd.Parse(fullcmd.c_str())) {
        LogFile.eSysLog("error parsing cmd: %s", MailCmd.c_str());
        return false;
    }
    const char* res = cmd.Execute(ScriptArgs.c_str());
    scriptReply = res ? res : "";
    if (scriptReply != "")
        LogFile.iSysLog("mail cmd result: %s", scriptReply.c_str());

    return true;
}

bool cMailNotifier::TestMailAccount(string MailAddressTo, string MailAddress, string MailServer, string AuthUser, string AuthPass)
{
    string cmdArgs =
        string("-v -f \"VDR <") + MailAddress + ">\"" +
        " -t " + MailAddressTo +
        " -s " + MailServer +
        " -u \"VDR-Testmail\"" +
        (AuthUser != "" ? (" -xu " + AuthUser) : "") +
        (AuthPass != "" ? (" -xp " + AuthPass) : "") +
        " -m \"Success! ;-)\"";

    return ExecuteMailScript(cmdArgs);
}

string cMailNotifier::LoadTemplate(const string& templtype)
{
    string filename = *AddDirectory(CONFIGDIR, templtype.c_str());
    string templ = "";
    if (filename != "" && access(filename.c_str(), F_OK) == 0) {
        LogFile.iSysLog("loading %s", filename.c_str());
        FILE *f = fopen(filename.c_str(), "r");
        if (f) {
            templ = "";
            char *s;
            cReadLine ReadLine;
            while ((s = ReadLine.Read(f)) != NULL) {
                if (strlen(s) > 0 && s[0] == '#')
                    continue;
                templ += string(s) + "\n";
            }
            fclose(f);
        }
    }
    return templ;
}

string cMailNotifier::GetTemplValue(const string& templ, const string& entry)
{
    if (templ == "" || entry == "") return "";

    string start = "<" + entry + ">";
    string end = "</" + entry + ">";

    int eBegin = FindIgnoreCase(templ, start);
    int eEnd = FindIgnoreCase(templ, end);
    if (eBegin < 0 || eEnd < 0) return "";
    string value(templ.begin() + eBegin + start.length(), templ.begin() + eEnd);
    return value;
}

// -------------------
// cMailUpdateNotifier
cMailUpdateNotifier::cMailUpdateNotifier()
{
    mailTemplate = LoadTemplate("epgsearchupdmail.templ");
}

void cMailUpdateNotifier::AddNewTimerNotification(tEventID EventID, tChannelID ChannelID)
{
    cMailTimerNotification N(EventID, ChannelID);
    newTimers.insert(N);
}

void cMailUpdateNotifier::AddModTimerNotification(tEventID EventID, tChannelID ChannelID, uint timerMod)
{
    if (timerMod == tmNoChange || timerMod == tmAuxEventID) // if anything but the eventID has changed
        return;
    cMailTimerNotification N(EventID, ChannelID, timerMod);
    modTimers.insert(N);
}

void cMailUpdateNotifier::AddRemoveTimerNotification(const cTimer* t, const cEvent* e)
{
    string templTimer = GetTemplValue(mailTemplate, "timer");
    cMailDelTimerNotification N(t, e, templTimer);
    delTimers.insert(N);
}

void cMailUpdateNotifier::AddRemoveTimerNotification(const string& Formatted, tChannelID ChannelID, time_t Start)
{
    cMailDelTimerNotification N(Formatted, ChannelID, Start);
    delTimers.insert(N);
}

void cMailUpdateNotifier::AddAnnounceEventNotification(tEventID EventID, tChannelID ChannelID, int SearchExtID)
{
    cMailAnnounceEventNotification N(EventID, ChannelID, SearchExtID);
    announceEvents.insert(N);
}

void cMailUpdateNotifier::SendUpdateNotifications()
{
    // insert pending notifications
    cPendingNotification* p = PendingNotifications.First();
    while (p) {
        if (p->type == 0)
            AddNewTimerNotification(p->eventID, p->channelID);
        else if (p->type == 1)
            AddModTimerNotification(p->eventID, p->channelID, p->timerMod);
        else if (p->type == 2)
            AddRemoveTimerNotification(p->formatted, p->channelID, p->start);
        else if (p->type == 3)
            AddAnnounceEventNotification(p->eventID, p->channelID, p->searchID);
        p = PendingNotifications.Next(p);
    }

    if (newTimers.empty() &&
        modTimers.empty() &&
        delTimers.empty() &&
        announceEvents.empty())
        return;

    // extract single templates
    if (mailTemplate == "") {
        LogFile.eSysLog("error loading %s", *AddDirectory(CONFIGDIR, "epgsearchupdmail.templ"));
        return;
    }
    string templSubject = GetTemplValue(mailTemplate, "subject");
    string templBody = GetTemplValue(mailTemplate, "mailbody");
    string templTimer = GetTemplValue(mailTemplate, "timer");
    string templEvent = GetTemplValue(mailTemplate, "event");

    // create the timer list for new timers
    string newtimers;
    if (newTimers.empty())
        newtimers = tr("No new timers were added.");
    std::set<cMailTimerNotification>::iterator itnt;
    for (itnt = newTimers.begin(); itnt != newTimers.end(); ++itnt) {
        string message = (*itnt).Format(templTimer);
        if (message != "") newtimers += message;
    }

    // create the timer list for modified timers
    string modtimers;
    if (modTimers.empty())
        modtimers = tr("No timers were modified.");
    std::set<cMailTimerNotification>::iterator itmt;
    for (itmt = modTimers.begin(); itmt != modTimers.end(); ++itmt) {
        string message = (*itmt).Format(templTimer);
        if (message != "") modtimers += message;
    }

    // create the timer list for removed timers
    string deltimers;
    if (delTimers.empty())
        deltimers = tr("No timers were deleted.");
    std::set<cMailDelTimerNotification>::iterator itdt;
    for (itdt = delTimers.begin(); itdt != delTimers.end(); ++itdt) {
        string message = (*itdt).Format("");
        if (message != "") deltimers += message;
    }

    // create the list of events to announce
    string announceevents;
    if (announceEvents.empty())
        announceevents = tr("No new events to announce.");
    std::set<cMailAnnounceEventNotification>::iterator itae;
    for (itae = announceEvents.begin(); itae != announceEvents.end(); ++itae) {
        string message = (*itae).Format(templEvent);
        if (message != "") announceevents += message;
    }

    // evaluate variables
    cVarExpr varExprSubj(templSubject);
    subject =  varExprSubj.Evaluate();
    subject = ReplaceAll(subject, "%update.countnewtimers%", NumToString((long)newTimers.size()));
    subject = ReplaceAll(subject, "%update.countmodtimers%", NumToString((long)modTimers.size()));
    subject = ReplaceAll(subject, "%update.countdeltimers%", NumToString((long)delTimers.size()));
    subject = ReplaceAll(subject, "%update.countnewevents%", NumToString((long)announceEvents.size()));

    newtimers = ReplaceAll(newtimers, "%update.countnewtimers%", NumToString((long)newTimers.size()));
    modtimers = ReplaceAll(modtimers, "%update.countmodtimers%", NumToString((long)modTimers.size()));
    deltimers = ReplaceAll(deltimers, "%update.countdeltimers%", NumToString((long)delTimers.size()));
    announceevents = ReplaceAll(announceevents, "%update.countnewevents%", NumToString((long)announceEvents.size()));

    cVarExpr varExprBody(templBody);
    body =  varExprBody.Evaluate();
    body = ReplaceAll(body, "%update.countnewtimers%", NumToString((long)newTimers.size()));
    body = ReplaceAll(body, "%update.countmodtimers%", NumToString((long)modTimers.size()));
    body = ReplaceAll(body, "%update.countdeltimers%", NumToString((long)delTimers.size()));
    body = ReplaceAll(body, "%update.countnewevents%", NumToString((long)announceEvents.size()));
    body = ReplaceAll(body, "%update.newtimers%", newtimers);
    body = ReplaceAll(body, "%update.modtimers%", modtimers);
    body = ReplaceAll(body, "%update.deltimers%", deltimers);
    body = ReplaceAll(body, "%update.newevents%", announceevents);

    if (SendMail()) {
        EPGSearchConfig.lastMailOnSearchtimerAt = time(NULL);
        cPluginManager::GetPlugin("epgsearch")->SetupStore("MailNotificationSearchtimersLastAt",
                                                           EPGSearchConfig.lastMailOnSearchtimerAt);
        // remove pending notifications
        while ((p = PendingNotifications.First()) != NULL)
            PendingNotifications.Del(p);
    } else {
        // add current notifications to pending ones
        for (itnt = newTimers.begin(); itnt != newTimers.end(); ++itnt)
            PendingNotifications.Add(new cPendingNotification(0, itnt->eventID, itnt->channelID, -1));
        for (itmt = modTimers.begin(); itmt != modTimers.end(); ++itmt)
            PendingNotifications.Add(new cPendingNotification(1, itmt->eventID, itmt->channelID, -1, itmt->timerMod));
        for (itdt = delTimers.begin(); itdt != delTimers.end(); ++itdt)
            PendingNotifications.Add(new cPendingNotification(2, -1, itdt->channelID, itdt->start, -1, -1, itdt->formatted));
        for (itae = announceEvents.begin(); itae != announceEvents.end(); ++itae)
            PendingNotifications.Add(new cPendingNotification(3, itae->eventID, itae->channelID, -1, -1, itae->searchextID));
    }
    PendingNotifications.Save();

    newTimers.clear();
    modTimers.clear();
    delTimers.clear();
    // Add announced events to the "no announce" list
    for (itae = announceEvents.begin(); itae != announceEvents.end(); ++itae) {
        cNoAnnounce* noAnnounce = new cNoAnnounce(itae->GetEvent());
        if (noAnnounce && noAnnounce->Valid())
            NoAnnounces.Add(noAnnounce);
    }
    if (!announceEvents.empty()) {
        NoAnnounces.ClearOutdated();
        NoAnnounces.Save();
    }

    announceEvents.clear();
}

// ---------------------
// cMailConflictNotifier
void cMailConflictNotifier::SendConflictNotifications(cConflictCheck& conflictCheck)
{
    if (conflictCheck.relevantConflicts == 0)
        return;

    // check if anything has changed since last mail
    ostringstream newMailConflicts;
    cList<cConflictCheckTime>* failedList = conflictCheck.GetFailed();
    if (!failedList) return;

    for (cConflictCheckTime* ct = failedList->First(); ct; ct = failedList->Next(ct)) {
        if (!ct || ct->ignore) continue;
        std::set<cConflictCheckTimerObj*, TimerObjSort>::iterator it;
        for (it = ct->failedTimers.begin(); it != ct->failedTimers.end(); ++it)
            if ((*it) && !(*it)->ignore && (*it)->Event()) {
                std::string channelID = *(*it)->Event()->ChannelID().ToString();
                newMailConflicts << (*it)->Event()->EventID()
                                 << "|"
                                 << channelID;
            }
    }
    string newMailConflictsMD5 = MD5(newMailConflicts.str());
    if (newMailConflictsMD5 == EPGSearchConfig.LastMailConflicts) {
        LogFile.Log(3, "conflicts unchanged - no new notification needed.");
        return;
    }


    // open the template
    string templ = LoadTemplate("epgsearchconflmail.templ");
    if (templ == "") {
        LogFile.eSysLog("error loading %s", *AddDirectory(CONFIGDIR, "epgsearchconflmail.templ"));
        return;
    }

    // extract single templates
    LogFile.Log(3, "extracting templates");
    string templSubject = GetTemplValue(templ, "subject");
    string templBody = GetTemplValue(templ, "mailbody");
    string templConflictsAt = GetTemplValue(templ, "conflictsat");
    string templConflictTimer = GetTemplValue(templ, "conflicttimer");
    LogFile.Log(3, "extracting templates - done");

    // create the conflict list
    string conflicts;
    for (cConflictCheckTime* ct = failedList->First(); ct; ct = failedList->Next(ct)) {
        if (ct->ignore) continue;
        // format conflict time
        string conflictsAt = templConflictsAt;
        conflictsAt = ReplaceAll(conflictsAt, "%conflict.time%", TIMESTRING(ct->evaltime));
        conflictsAt = ReplaceAll(conflictsAt, "%conflict.date%", DATESTRING(ct->evaltime));

        string conflicttimers;
        std::set<cConflictCheckTimerObj*, TimerObjSort>::iterator it;
        for (it = ct->failedTimers.begin(); it != ct->failedTimers.end(); ++it)
            if (!(*it)->ignore && (*it)->Event()) {
                cMailTimerNotification M((*it)->Event()->EventID(), (*it)->Event()->ChannelID());
                string message = M.Format(templConflictTimer);
                if (message != "") {
                    message = ReplaceAll(message, "%device%", NumToString((*it)->device));
                    conflicttimers += message;
                }
            }
        conflictsAt = ReplaceAll(conflictsAt, "%conflict.confltimers%", conflicttimers);

        conflicts += conflictsAt;
    }

    // evaluate variables
    cVarExpr varExprSubj(templSubject);
    subject =  varExprSubj.Evaluate();
    subject = ReplaceAll(subject, "%conflict.count%", NumToString(conflictCheck.relevantConflicts));

    conflicts = ReplaceAll(conflicts, "%conflict.count%", NumToString(conflictCheck.relevantConflicts));

    cVarExpr varExprBody(templBody);
    body =  varExprBody.Evaluate();
    body = ReplaceAll(body, "%conflict.count%", NumToString(conflictCheck.relevantConflicts));
    body = ReplaceAll(body, "%conflict.conflicts%", conflicts);

    SendMail();

    // store conflicts
    strcpy(EPGSearchConfig.LastMailConflicts, newMailConflictsMD5.c_str());
    cPluginManager::GetPlugin("epgsearch")->SetupStore("LastMailConflicts",  EPGSearchConfig.LastMailConflicts);
}
