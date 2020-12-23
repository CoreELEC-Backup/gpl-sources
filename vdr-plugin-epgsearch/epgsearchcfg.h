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

#ifndef __EPGSEARCHCFG_H
#define __EPGSEARCHCFG_H

#include <stdio.h>
#include <string.h>
#include <vdr/config.h>

extern int toggleKeys;

typedef enum {
    showNow = 0,
    showNext,
    showUserMode1,
    showUserMode2,
    showUserMode3,
    showUserMode4,
    showFavorites,
    showModeMax
} showMode;

class cShowMode: public cListObject
{
    showMode mode;
    time_t seekTime;
public:
    char description[30];
    int useIt;
    int itime;

    cShowMode() : mode(showNow), seekTime(0), useIt(0), itime(0)  {
        description[0] = 0;
    }
    cShowMode(showMode Mode, const char* Description, int UseIt = 1, int iTime = 0, time_t SeekTime = 0)
        : mode(Mode), seekTime(SeekTime), useIt(UseIt), itime(iTime) {
        if (strlen(Description) > 0)
            SetDescription(Description);
        else
            sprintf(description, "%02d:%02d", iTime / 100, iTime % 100);
    }
    cShowMode& operator= (const cShowMode &ShowMode);
    const char* GetDescription() {
        return description;
    }
    int GetTime() const {
        return itime;
    }
    bool GetUsage() const {
        return useIt;
    }

    void SetDescription(const char* szD) {
        if (szD)  strncpy(description, szD, sizeof(description));
    }
    void SetTime(int iT) {
        itime = iT;
    }
    void SetUsage(bool bU) {
        useIt = bU;
    }
    int Compare(const cListObject &ListObject) const;
    showMode GetMode() const {
        return mode;
    }
};


typedef enum {
    addSubtitleNever = 0,
    addSubtitleAlways,
    addSubtitleSmart
} addSubtitleToTimerMode;

struct cEPGSearchConfig {
public:
    cEPGSearchConfig(void);
    int hidemenu;
    int ReplaceOrgSchedule;
    int redkeymode;
    int bluekeymode;
    int showProgress;
    int showChannelNr;
    int useSearchTimers;
    int UpdateIntervall;
    int SVDRPPort;
    int timeShiftValue;
    int toggleGreenYellow;
    int StartMenu;
    int DefPriority;
    int DefLifetime;
    int DefMarginStart;
    int DefMarginStop;
    int checkTimerConflictsAfterUpdate;
    int checkMinPriority;
    int checkMinDuration;
    int checkMaxDays;
    int ignorePayTV;
    int useExternalSVDRP;
    int ignorePrimary;
    char defrecdir[MaxFileName];
    cShowMode ShowModes[showModeMax];
    int useVDRTimerEditMenu;
    int showChannelGroups;
    int showDaySeparators;
    int showEmptyChannels;
    int DefSearchTemplateID;
    int addSubtitleToTimer;
    char mainmenuentry[MaxFileName];
    int WarEagle;
    int showRadioChannels;
    int onePressTimerCreation;
    int conflictCheckIntervall;
    int conflictCheckWithinLimit;
    int conflictCheckIntervall2;
    int checkTimerConflAfterTimerProg;
    int checkTimerConflOnRecording;
    int showFavoritesMenu;
    int FavoritesMenuTimespan;
    int useOkForSwitch;
    int sendMailOnSearchtimers;
    int sendMailOnConflicts;
    int RemoteConflictCheck;
    char MailAddressTo[MaxFileName];
    char MailAddress[MaxFileName];
    char MailServer[MaxFileName];
    int MailUseAuth;
    char MailAuthUser[MaxFileName];
    char MailAuthPass[MaxFileName];
    char LastMailConflicts[MaxFileName];
    int mailViaScript;
    int manualTimerCheckDefault;
    int noAnnounceWhileReplay;
    int TimerProgRepeat;
    int maxChannelMenuNow;
    int noConflMsgWhileReplay;
    int sendMailOnSearchtimerHours;
    int checkEPGHours;
    int checkEPGWarnByOSD;
    int checkEPGWarnByMail;
    int checkEPGchannelGroupNr;
    time_t lastMailOnSearchtimerAt;
    char conflCheckCmd[MaxFileName * 10];
};

extern cEPGSearchConfig EPGSearchConfig;

#endif // __EPGSEARCHCFG_H
