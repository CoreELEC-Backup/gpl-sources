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
#include <string>
#include "epgsearchsetup.h"
#include "epgsearchcfg.h"
#include "searchtimer_thread.h"
#include "epgsearchtools.h"
#include "changrp.h"
#include "menu_dirselect.h"
#include "menu_searchtemplate.h"
#include "menu_blacklists.h"
#include "templatefile.h"
#include "conflictcheck_thread.h"
#include "mail.h"
#include "menu_deftimercheckmethod.h"

const char *OkKeyMode[2];
const char *RedKeyMode[2];
const char *BlueKeyMode[2];
const char *StartMenuMode[2];
const char *AddSubtitleMode[3];
const char *FavoritesMenuMode[4];
const char *MailMethod[2];

const char *cMenuSetupMailNotification::HostNameChars    = " abcdefghijklmnopqrstuvwxyz0123456789-._~:";
const char *cMenuSetupMailNotification::UserNameChars    = " abcdefghijklmnopqrstuvwxyz0123456789-+.,:;?!$&#@~{}[]()_/";
const char *cMenuSetupMailNotification::PasswordChars    = " abcdefghijklmnopqrstuvwxyz0123456789-+.,:;?!$&#@~{}[]()_/";
const char *cMenuSetupMailNotification::MailBoxChars     = " abcdefghijklmnopqrstuvwxyz0123456789-.#@~{}[]_/";

// ------------------
// cMenuSetupSubMenu
cMenuSetupSubMenu::cMenuSetupSubMenu(const char* Title, cEPGSearchConfig* Data)
    : cOsdMenu(Title, 33)
{
    SetMenuCategory(mcSetupPlugins);
    data = Data;
}

eOSState cMenuSetupSubMenu::ProcessKey(eKeys Key)
{
    eOSState state = cOsdMenu::ProcessKey(Key);

    if (state == osUnknown) {
        switch (Key) {
        case kInfo:
        case kYellow:
            state = Help();
            break;
        default:
            break;
        }
    }

    return state;
}

void cMenuSetupSubMenu::AddHelp(const char* helpText)
{
    helpTexts.push_back(helpText);
}

eOSState cMenuSetupSubMenu::Help()
{
    const char* ItemText = Get(Current())->Text();
    eOSState state = osContinue;
    if (Current() < (int) helpTexts.size()) {
        char* title = NULL;
        if (msprintf(&title, "%s - %s", tr("Button$Help"), ItemText) != -1) {
            if (strchr(title, ':'))
                *strchr(title, ':') = 0;
            state = AddSubMenu(new cMenuText(title, helpTexts[Current()]));
            free(title);
        }
    }
    return state;
}

// ------------------
// cMenuEPGSearchSetup
cMenuEPGSearchSetup::cMenuEPGSearchSetup(void)
{
    OkKeyMode[0] = trVDR("Button$Info");
    OkKeyMode[1] = trVDR("Button$Switch");

    RedKeyMode[0] = tr("Standard");
    RedKeyMode[1] = tr("Button$Commands");

    BlueKeyMode[0] = tr("Standard");
    BlueKeyMode[1] = tr("Button$Search");

    StartMenuMode[0] = trVDR("Button$Schedule");
    StartMenuMode[1] = trVDR("Button$Now");

    AddSubtitleMode[0] = tr("never");
    AddSubtitleMode[1] = tr("always");
    AddSubtitleMode[2] = tr("smart");

    FavoritesMenuMode[0] = trVDR("no");
    FavoritesMenuMode[1] = tr("before user-def. times");
    FavoritesMenuMode[2] = tr("after user-def. times");
    FavoritesMenuMode[3] = tr("before 'next'");

    MailMethod[0] = "sendmail";
    MailMethod[1] = "sendEmail.pl";

    Setup();
}

void cMenuEPGSearchSetup::Setup(void)
{
    data = EPGSearchConfig;
    if (isempty(EPGSearchConfig.mainmenuentry))
        strcpy(data.mainmenuentry, tr("Program guide"));

    Set();
    SetHelp(NULL, NULL, NULL, trVDR("Button$Open"));
}

void cMenuEPGSearchSetup::Set()
{
    int current = Current();
    Clear();

    Add(new cOsdItem(tr("General")));
    Add(new cOsdItem(tr("EPG menus")));
    Add(new cOsdItem(tr("User-defined EPG times")));
    Add(new cOsdItem(tr("Timer programming")));
    Add(new cOsdItem(tr("Search and search timers")));
    Add(new cOsdItem(tr("Timer conflict checking")));
    Add(new cOsdItem(tr("Email notification")));

    SetCurrent(Get(current));
    Display();
}

void cMenuEPGSearchSetup::Store(void)
{
    bool RestartSearchTimerThread = false;
    if (EPGSearchConfig.useSearchTimers != data.useSearchTimers) {
        RestartSearchTimerThread = true;
        cSearchTimerThread::Exit();
    }

    bool RestartConflictCheckThread = false;
    if (EPGSearchConfig.checkTimerConflictsAfterUpdate != data.checkTimerConflictsAfterUpdate ||
        EPGSearchConfig.conflictCheckIntervall != data.conflictCheckIntervall) {
        RestartConflictCheckThread = true;
        cConflictCheckThread::Exit();
    }

    EPGSearchConfig = data;
    if (strcmp(EPGSearchConfig.mainmenuentry, tr("Program guide")) == 0)
        strcpy(EPGSearchConfig.mainmenuentry, "");

    if (isempty(EPGSearchConfig.MailAddressTo))
        strcpy(EPGSearchConfig.MailAddressTo, EPGSearchConfig.MailAddress);

    SetupStore("HideMenu",  EPGSearchConfig.hidemenu);
    SetupStore("MainMenuEntry",  EPGSearchConfig.mainmenuentry);
    SetupStore("ReplaceOrgSchedule",  EPGSearchConfig.ReplaceOrgSchedule);
    SetupStore("StartMenu",  EPGSearchConfig.StartMenu);
    SetupStore("RedKeyMode",  EPGSearchConfig.redkeymode);
    SetupStore("BlueKeyMode",  EPGSearchConfig.bluekeymode);
    SetupStore("ShowProgress",  EPGSearchConfig.showProgress);
    SetupStore("ShowChannelNr",  EPGSearchConfig.showChannelNr);
    SetupStore("OnePressTimerCreation",  EPGSearchConfig.onePressTimerCreation);
    SetupStore("ShowFavoritesMenu",  EPGSearchConfig.showFavoritesMenu);
    SetupStore("FavoritesMenuTimespan",  EPGSearchConfig.FavoritesMenuTimespan);
    SetupStore("UseOkForSwitch",  EPGSearchConfig.useOkForSwitch);
    SetupStore("MaxChannelMenuNow",  EPGSearchConfig.maxChannelMenuNow);

    SetupStore("UserMode1UseIt",  EPGSearchConfig.ShowModes[showUserMode1].GetUsage());
    SetupStore("UserMode1Description",  EPGSearchConfig.ShowModes[showUserMode1].GetDescription());
    SetupStore("UserMode1Time",  EPGSearchConfig.ShowModes[showUserMode1].GetTime());

    SetupStore("UserMode2UseIt",  EPGSearchConfig.ShowModes[showUserMode2].GetUsage());
    SetupStore("UserMode2Description",  EPGSearchConfig.ShowModes[showUserMode2].GetDescription());
    SetupStore("UserMode2Time",  EPGSearchConfig.ShowModes[showUserMode2].GetTime());

    SetupStore("UserMode3UseIt",  EPGSearchConfig.ShowModes[showUserMode3].GetUsage());
    SetupStore("UserMode3Description",  EPGSearchConfig.ShowModes[showUserMode3].GetDescription());
    SetupStore("UserMode3Time",  EPGSearchConfig.ShowModes[showUserMode3].GetTime());

    SetupStore("UserMode4UseIt",  EPGSearchConfig.ShowModes[showUserMode4].GetUsage());
    SetupStore("UserMode4Description",  EPGSearchConfig.ShowModes[showUserMode4].GetDescription());
    SetupStore("UserMode4Time",  EPGSearchConfig.ShowModes[showUserMode4].GetTime());

    SetupStore("UseSearchTimers",  EPGSearchConfig.useSearchTimers);
    SetupStore("UpdateIntervall",  EPGSearchConfig.UpdateIntervall);
    SetupStore("SVDRPPort",  EPGSearchConfig.SVDRPPort);
    SetupStore("CheckTimerConflicts",  EPGSearchConfig.checkTimerConflictsAfterUpdate);
    SetupStore("CheckTimerConflictsPriority",  EPGSearchConfig.checkMinPriority);
    SetupStore("CheckTimerConflictsDays",  EPGSearchConfig.checkMaxDays);
    SetupStore("CheckConflictsIntervall",  EPGSearchConfig.conflictCheckIntervall);
    SetupStore("CheckConflictsWithinLimit",  EPGSearchConfig.conflictCheckWithinLimit);
    SetupStore("CheckConflictsIntervall2",  EPGSearchConfig.conflictCheckIntervall2);
    SetupStore("CheckConflictsMinDuration",  EPGSearchConfig.checkMinDuration);
    SetupStore("CheckConflictsAfterTimerProg",  EPGSearchConfig.checkTimerConflAfterTimerProg);
    SetupStore("CheckConflictsOnRecording",  EPGSearchConfig.checkTimerConflOnRecording);
    SetupStore("NoConflMsgWhileReplay",  EPGSearchConfig.noConflMsgWhileReplay);
    SetupStore("RemoteConflictCheck",  EPGSearchConfig.RemoteConflictCheck);
    SetupStore("CheckEPGHours",  EPGSearchConfig.checkEPGHours);
    SetupStore("CheckEPGWarnByOSD",  EPGSearchConfig.checkEPGWarnByOSD);
    SetupStore("CheckEPGWarnByMail",  EPGSearchConfig.checkEPGWarnByMail);
    SetupStore("CheckEPGChannelgroup",  EPGSearchConfig.checkEPGchannelGroupNr);

    SetupStore("NoAnnounceWhileReplay",  EPGSearchConfig.noAnnounceWhileReplay);
    SetupStore("TimerProgRepeat",  EPGSearchConfig.TimerProgRepeat);

    SetupStore("TimeIntervallFRFF",  EPGSearchConfig.timeShiftValue);
    SetupStore("ToggleGreenYellow",  EPGSearchConfig.toggleGreenYellow);

    SetupStore("ShowRadioChannels", EPGSearchConfig.showRadioChannels);

    SetupStore("DefPriority",  EPGSearchConfig.DefPriority);
    SetupStore("DefLifetime",  EPGSearchConfig.DefLifetime);
    SetupStore("DefMarginStart",  EPGSearchConfig.DefMarginStart);
    SetupStore("DefMarginStop",  EPGSearchConfig.DefMarginStop);

    SetupStore("IgnorePayTV",  EPGSearchConfig.ignorePayTV);
    SetupStore("DefRecordingDir",  EPGSearchConfig.defrecdir);
    SetupStore("UseVDRTimerEditMenu",  EPGSearchConfig.useVDRTimerEditMenu);
    SetupStore("ShowChannelGroups",  EPGSearchConfig.showChannelGroups);
    SetupStore("ShowDaySeparators",  EPGSearchConfig.showDaySeparators);
    SetupStore("ShowEmptyChannels",  EPGSearchConfig.showEmptyChannels);

    SetupStore("DefSearchTemplateID",  EPGSearchConfig.DefSearchTemplateID);

    SetupStore("AddSubtitleToTimerMode",  EPGSearchConfig.addSubtitleToTimer);

    SetupStore("MailNotificationSearchtimers",  EPGSearchConfig.sendMailOnSearchtimers);
    SetupStore("MailNotificationSearchtimersHours",  EPGSearchConfig.sendMailOnSearchtimerHours);
    SetupStore("MailNotificationConflicts",  EPGSearchConfig.sendMailOnConflicts);
    SetupStore("MailAddress",  EPGSearchConfig.MailAddress);
    SetupStore("MailAddressTo",  EPGSearchConfig.MailAddressTo);
    SetupStore("MailServer",  EPGSearchConfig.MailServer);
    SetupStore("MailUseAuth",  EPGSearchConfig.MailUseAuth);
    SetupStore("MailAuthUser",  EPGSearchConfig.MailAuthUser);
    SetupStore("MailAuthPass",  EPGSearchConfig.MailAuthPass);
    SetupStore("MailViaScript",  EPGSearchConfig.mailViaScript);

    cTemplFile::Reset();
    char* templateFilename = strdup(AddDirectory(CONFIGDIR, "epgsearchmenu.conf"));
    if (access(templateFilename, F_OK) == 0) {
        cTemplFile templFile;
        if (!templFile.Load(templateFilename))
            LogFile.eSysLog("could not load '%s'", templateFilename);
    }
    cTemplFile::PrepareDefaultTemplates();
    free(templateFilename);

    cPluginEpgsearch *p = (cPluginEpgsearch*) cPluginManager::GetPlugin("epgsearch");
    if (RestartSearchTimerThread)
        cSearchTimerThread::Init(p);
    if (RestartConflictCheckThread)
        cConflictCheckThread::Init(p);
}

eOSState cMenuEPGSearchSetup::ProcessKey(eKeys Key)
{
    bool hadSubMenu = HasSubMenu();
    eOSState state = cMenuSetupPage::ProcessKey(Key);

    const char* ItemText = Get(Current())->Text();
    int iOnGeneral = 0;
    int iOnEPGMenus = 0;
    int iOnUserdefTimes = 0;
    int iOnTimers = 0;
    int iOnSearchtimers = 0;
    int iOnTimerConflicts = 0;
    int iOnEmailNotification = 0;

    if (!HasSubMenu()) {
        if (strstr(ItemText, tr("General")) == ItemText)
            iOnGeneral = 1;
        else if (strstr(ItemText, tr("EPG menus")) == ItemText)
            iOnEPGMenus = 1;
        else if (strstr(ItemText, tr("User-defined EPG times")) == ItemText)
            iOnUserdefTimes = 1;
        else if (strstr(ItemText, tr("Timer programming")) == ItemText)
            iOnTimers = 1;
        else if (strstr(ItemText, tr("Search and search timers")) == ItemText)
            iOnSearchtimers = 1;
        else if (strstr(ItemText, tr("Timer conflict checking")) == ItemText)
            iOnTimerConflicts = 1;
        else if (strstr(ItemText, tr("Email notification")) == ItemText)
            iOnEmailNotification = 1;
    }

    if (!HasSubMenu() && (state == osUnknown || Key == kOk)) {
        if ((Key == kOk && !hadSubMenu) || Key == kBlue) {
            if (iOnGeneral == 1)
                state = AddSubMenu(new cMenuSetupGeneral(&data));
            else if (iOnEPGMenus == 1)
                state = AddSubMenu(new cMenuSetupEPGMenus(&data));
            else if (iOnUserdefTimes == 1)
                state = AddSubMenu(new cMenuSetupUserdefTimes(&data));
            else if (iOnTimers == 1)
                state = AddSubMenu(new cMenuSetupTimers(&data));
            else if (iOnSearchtimers == 1)
                state = AddSubMenu(new cMenuSetupSearchtimers(&data));
            else if (iOnTimerConflicts == 1)
                state = AddSubMenu(new cMenuSetupTimerConflicts(&data));
            else if (iOnEmailNotification == 1)
                state = AddSubMenu(new cMenuSetupMailNotification(&data));
        }
    }
    if (!HasSubMenu() && hadSubMenu)
        Store();

    return state;
}

// ------------------
// cMenuSetupGeneral
cMenuSetupGeneral::cMenuSetupGeneral(cEPGSearchConfig* Data)
    : cMenuSetupSubMenu(tr("General"), Data)
{
    Set();
}

void cMenuSetupGeneral::Set()
{
    int current = Current();
    Clear();
    helpTexts.clear();

    Add(new cMenuEditBoolItem(tr("Hide main menu entry"),         &data->hidemenu,       trVDR("no"),      trVDR("yes")));
    AddHelp(tr("Help$Hides the main menu entry and may be useful if this plugin is used to replace the original 'Schedule' entry."));
    if (!data->hidemenu) {
        Add(new cMenuEditStrItem(IndentMenuItem(tr("Main menu entry")), data->mainmenuentry, sizeof(data->mainmenuentry), tr(AllowedChars)));
        AddHelp(tr("Help$The name of the main menu entry which defaults to 'Programm guide'."));
    }
    Add(new cMenuEditBoolItem(tr("Replace original schedule"),    &data->ReplaceOrgSchedule, trVDR("no"),      trVDR("yes")));
    AddHelp(tr("Help$When VDR is patched to allow this plugin to replace the original 'Schedule' entry, you can de/activate this replacement here."));
    Add(new cMenuEditStraItem(tr("Start menu"), &data->StartMenu, 2, StartMenuMode));
    AddHelp(tr("Help$Choose between 'Overview - Now' and 'Schedule' as start menu when this plugin is called."));

    SetCurrent(Get(current));
    Display();
    SetHelp(NULL, NULL, tr("Button$Help"), NULL);
}

eOSState cMenuSetupGeneral::ProcessKey(eKeys Key)
{
    int iTemp_hidemenu = data->hidemenu;

    eOSState state = cMenuSetupSubMenu::ProcessKey(Key);

    if (iTemp_hidemenu != data->hidemenu) {
        Set();
        Display();
    }

    if (state == osUnknown) {
        switch (Key) {
        case kOk:
            return osBack;
        default:
            break;
        }
    }

    return state;
}



// ------------------
// cMenuSetupEPGMenus
cMenuSetupEPGMenus::cMenuSetupEPGMenus(cEPGSearchConfig* Data)
    : cMenuSetupSubMenu(tr("EPG menus"), Data)
{
    Set();
}

void cMenuSetupEPGMenus::Set()
{
    int current = Current();
    Clear();
    helpTexts.clear();

    Add(new cMenuEditStraItem(tr("Ok key"), &data->useOkForSwitch, 2, OkKeyMode));
    AddHelp(tr("Help$Choose here the behaviour of key 'Ok'. You can use it to display the summary or to switch to the corresponding channel.\nNote: the functionality of key 'blue' (Switch/Info/Search) depends on this setting."));

    Add(new cMenuEditStraItem(tr("Red key"), &data->redkeymode, 2, RedKeyMode));
    AddHelp(tr("Help$Choose which standard function ('Record' or 'Commands') you like to have on the red key.\n(Can be toggled with key '0')"));
    Add(new cMenuEditStraItem(tr("Blue key"), &data->bluekeymode, 2, BlueKeyMode));
    AddHelp(tr("Help$Choose which standard function ('Switch'/'Info' or 'Search') you like to have on the blue key.\n(Can be toggled with key '0')"));

    Add(new cMenuEditBoolItem(tr("Show progress in 'Now'"), &data->showProgress, trVDR("no"), trVDR("yes")));
    AddHelp(tr("Help$Shows a progressbar in 'Overview - Now' that informs about the remaining time of the current event."));
    Add(new cMenuEditBoolItem(tr("Show channel numbers"), &data->showChannelNr,       trVDR("no"),      trVDR("yes")));
    AddHelp(tr("Help$Display channel numbers in 'Overview - Now'.\n\n(To completely define your own menu look please inspect the MANUAL)"));
    Add(new cMenuEditBoolItem(tr("Show channel separators"), &data->showChannelGroups,       trVDR("no"),      trVDR("yes")));
    AddHelp(tr("Help$Display VDR channel groups as separators between your channels in 'Overview - Now'."));
    Add(new cMenuEditBoolItem(tr("Show day separators"), &data->showDaySeparators,       trVDR("no"),      trVDR("yes")));
    AddHelp(tr("Help$Display a separator line at day break in 'Schedule'."));
    Add(new cMenuEditBoolItem(tr("Show radio channels"), &data->showRadioChannels,       trVDR("no"),      trVDR("yes")));
    AddHelp(tr("Help$Show also radio channels."));
    Add(new cMenuEditIntItem(tr("Limit channels from 1 to"), &data->maxChannelMenuNow, 0, 9999));
    AddHelp(tr("Help$If you have a large channel set you can speed up things when you limit the displayed channels with this setting. Use '0' to disable the limit."));
    Add(new cMenuEditBoolItem(tr("'One press' timer creation"), &data->onePressTimerCreation,       trVDR("no"),      trVDR("yes")));
    AddHelp(tr("Help$When a timer is created with 'Record' you can choose between an immediate creation of the timer or the display of the timer edit menu."));
    Add(new cMenuEditBoolItem(tr("Show channels without EPG"), &data->showEmptyChannels,       trVDR("no"),      trVDR("yes")));
    AddHelp(tr("Help$Choose 'yes' here if you want to display channels without EPG in 'Overview - Now'. 'Ok' on these entries switches the channel."));
    Add(new cMenuEditIntItem(tr("Time interval for FRew/FFwd [min]"), &data->timeShiftValue, 1, 9999));
    AddHelp(tr("Help$Choose here the time interval which should be used for jumping through the EPG by pressing FRew/FFwd.\n\n(If you don't have those keys, you can toggle to this functionality pressing '0' and get '<<' and '>>' on the keys green and yellow)"));
    Add(new cMenuEditBoolItem(tr("Toggle Green/Yellow"), &data->toggleGreenYellow,       trVDR("no"),      trVDR("yes")));
    AddHelp(tr("Help$Specify if green and yellow shall also be switched when pressing '0'."));

    Add(new cMenuEditStraItem(tr("Show favorites menu"), &data->showFavoritesMenu, 4, FavoritesMenuMode));
    AddHelp(tr("Help$A favorites menu can display a list of your favorite broadcasts. Enable this if you want an additional menu besides 'Now' and 'Next'\nAny search can be used as a favorite. You only have to set the option 'Use in favorites menu' when editing a search."));
    if (data->showFavoritesMenu) {
        Add(new cMenuEditIntItem(IndentMenuItem(tr("for the next ... hours")), &data->FavoritesMenuTimespan, 1, 9999));
        AddHelp(tr("Help$This value controls the timespan used to display your favorites."));
    }

    SetCurrent(Get(current));
    Display();
    SetHelpKeys();
}

void cMenuSetupEPGMenus::SetHelpKeys()
{
    SetHelp(NULL, NULL, tr("Button$Help"), NULL);
}

// ------------------
// cMenuSetupUsedefTimes
cMenuSetupUserdefTimes::cMenuSetupUserdefTimes(cEPGSearchConfig* Data)
    : cMenuSetupSubMenu(tr("User-defined EPG times"), Data)
{
    Set();
}

void cMenuSetupUserdefTimes::Set()
{
    int current = Current();
    Clear();
    helpTexts.clear();

    cString szUseUserTime = cString::sprintf("%s %d", tr("Use user-defined time"), 1);
    Add(new cMenuEditBoolItem(szUseUserTime, &data->ShowModes[showUserMode1].useIt,       trVDR("no"),      trVDR("yes")));
    AddHelp(tr("Help$Besides 'Now' and 'Next' you can specify up to 4 other times in the EPG which can be used by repeatedly pressing the green key, e.g. 'prime time', 'late night',..."));
    if (data->ShowModes[showUserMode1].GetUsage()) {
        Add(new cMenuEditStrItem(IndentMenuItem(tr("Description")), data->ShowModes[showUserMode1].description, sizeof(data->ShowModes[showUserMode1].description), trVDR(FileNameChars)));
        AddHelp(tr("Help$This is the description for your user-defined time as it will appear as label on the green button."));
        Add(new cMenuEditTimeItem(IndentMenuItem(tr("Time")), &data->ShowModes[showUserMode1].itime));
        AddHelp(tr("Help$Specify the user-defined time here in 'HH:MM'."));
    }

    szUseUserTime = cString::sprintf("%s %d", tr("Use user-defined time"), 2);
    Add(new cMenuEditBoolItem(szUseUserTime, &data->ShowModes[showUserMode2].useIt,       trVDR("no"),      trVDR("yes")));
    AddHelp(tr("Help$Besides 'Now' and 'Next' you can specify up to 4 other times in the EPG which can be used by repeatedly pressing the green key, e.g. 'prime time', 'late night',..."));
    if (data->ShowModes[showUserMode2].GetUsage()) {
        Add(new cMenuEditStrItem(IndentMenuItem(tr("Description")), data->ShowModes[showUserMode2].description, sizeof(data->ShowModes[showUserMode2].description), trVDR(FileNameChars)));
        AddHelp(tr("Help$This is the description for your user-defined time as it will appear as label on the green button."));
        Add(new cMenuEditTimeItem(IndentMenuItem(tr("Time")), &data->ShowModes[showUserMode2].itime));
        AddHelp(tr("Help$Specify the user-defined time here in 'HH:MM'."));
    }

    szUseUserTime = cString::sprintf("%s %d", tr("Use user-defined time"), 3);
    Add(new cMenuEditBoolItem(szUseUserTime, &data->ShowModes[showUserMode3].useIt,       trVDR("no"),      trVDR("yes")));
    AddHelp(tr("Help$Besides 'Now' and 'Next' you can specify up to 4 other times in the EPG which can be used by repeatedly pressing the green key, e.g. 'prime time', 'late night',..."));
    if (data->ShowModes[showUserMode3].GetUsage()) {
        Add(new cMenuEditStrItem(IndentMenuItem(tr("Description")), data->ShowModes[showUserMode3].description, sizeof(data->ShowModes[showUserMode3].description), trVDR(FileNameChars)));
        AddHelp(tr("Help$This is the description for your user-defined time as it will appear as label on the green button."));
        Add(new cMenuEditTimeItem(IndentMenuItem(tr("Time")), &data->ShowModes[showUserMode3].itime));
        AddHelp(tr("Help$Specify the user-defined time here in 'HH:MM'."));
    }

    szUseUserTime = cString::sprintf("%s %d", tr("Use user-defined time"), 4);
    Add(new cMenuEditBoolItem(szUseUserTime, &data->ShowModes[showUserMode4].useIt,       trVDR("no"),      trVDR("yes")));
    AddHelp(tr("Help$Besides 'Now' and 'Next' you can specify up to 4 other times in the EPG which can be used by repeatedly pressing the green key, e.g. 'prime time', 'late night',..."));
    if (data->ShowModes[showUserMode4].GetUsage()) {
        Add(new cMenuEditStrItem(IndentMenuItem(tr("Description")), data->ShowModes[showUserMode4].description, sizeof(data->ShowModes[showUserMode4].description), trVDR(FileNameChars)));
        AddHelp(tr("Help$This is the description for your user-defined time as it will appear as label on the green button."));
        Add(new cMenuEditTimeItem(IndentMenuItem(tr("Time")), &data->ShowModes[showUserMode4].itime));
        AddHelp(tr("Help$Specify the user-defined time here in 'HH:MM'."));
    }

    SetCurrent(Get(current));
    Display();
    SetHelp(NULL, NULL, tr("Button$Help"), NULL);
}

eOSState cMenuSetupUserdefTimes::ProcessKey(eKeys Key)
{
    int iTemp_useUserTime1 = data->ShowModes[showUserMode1].GetUsage();
    int iTemp_useUserTime2 = data->ShowModes[showUserMode2].GetUsage();
    int iTemp_useUserTime3 = data->ShowModes[showUserMode3].GetUsage();
    int iTemp_useUserTime4 = data->ShowModes[showUserMode4].GetUsage();

    eOSState state = cMenuSetupSubMenu::ProcessKey(Key);

    if (iTemp_useUserTime1 != data->ShowModes[showUserMode1].GetUsage() ||
        iTemp_useUserTime2 != data->ShowModes[showUserMode2].GetUsage() ||
        iTemp_useUserTime3 != data->ShowModes[showUserMode3].GetUsage() ||
        iTemp_useUserTime4 != data->ShowModes[showUserMode4].GetUsage()) {
        Set();
        Display();
    }

    if (state == osUnknown) {
        switch (Key) {
        case kOk:
            return osBack;
        default:
            break;
        }
    }

    return state;
}


// ------------------
// cMenuSetupTimers
cMenuSetupTimers::cMenuSetupTimers(cEPGSearchConfig* Data)
    : cMenuSetupSubMenu(tr("Timer programming"), Data)
{
    Set();
}

void cMenuSetupTimers::Set()
{
    int current = Current();
    Clear();
    helpTexts.clear();

    Add(new cMenuEditBoolItem(tr("Use VDR's timer edit menu"),         &data->useVDRTimerEditMenu,       trVDR("no"),      trVDR("yes")));
    AddHelp(tr("Help$This plugin has its own timer edit menu extending the original one with some extra functionality like\n- an additional directory entry\n- user-defined days of week for repeating timers\n- adding an episode name\n- support for EPG variables (see MANUAL)"));
    Add(new cMenuEditStrItem(tr("Default recording dir"), data->defrecdir, sizeof(data->defrecdir), tr(AllowedChars)));
    AddHelp(tr("Help$When creating a timer you can specify here a default recording directory."));
    Add(new cMenuEditStraItem(tr("Add episode to manual timers"), &data->addSubtitleToTimer, 3, AddSubtitleMode));
    AddHelp(tr("Help$If you create a timer for a series, you can automatically add the episode name.\n\n- never: no addition\n- always: always add episode name if present\n- smart: add only if event lasts less than 80 mins."));
    Add(new cOsdItem(tr("Default timer check method")));
    AddHelp(tr("Help$Manual timers can be checked for EPG changes. Here you can setup the default check method for each channel. Choose between\n\n- no checking\n- by event ID: checks by an event ID supplied by the channel provider.\n- by channel and time: check by the duration match."));

    SetCurrent(Get(current));
    Display();
    SetHelpKeys();
}

void cMenuSetupTimers::SetHelpKeys()
{
    const char* ItemText = Get(Current())->Text();
    if (!HasSubMenu()) {
        if (strstr(ItemText, tr("Default recording dir")) == ItemText) {
            if (!InEditMode(ItemText, tr("Default recording dir"), data->defrecdir))
                SetHelp(NULL, NULL, tr("Button$Help"), tr("Button$Select"));
        } else if (strstr(ItemText, tr("Default timer check method")) == ItemText)
            SetHelp(NULL, NULL, tr("Button$Help"), tr("Button$Setup"));
        else
            SetHelp(NULL, NULL, tr("Button$Help"), NULL);
    }
}

eOSState cMenuSetupTimers::ProcessKey(eKeys Key)
{
    eOSState state = cMenuSetupSubMenu::ProcessKey(Key);

    const char* ItemText = Get(Current())->Text();
    int iOnDefRecDir = 0;
    int iOnDefTimerCheck = 0;
    if (!HasSubMenu()) {
        if (strstr(ItemText, tr("Default recording dir")) == ItemText)
            iOnDefRecDir = 1;
        if (strstr(ItemText, tr("Default timer check method")) == ItemText)
            iOnDefTimerCheck = 1;
    }
    SetHelpKeys();

    if (state == osUnknown) {
        switch (Key) {
        case kBlue:
            if (!HasSubMenu()) {
                if (iOnDefRecDir == 1)
                    state = AddSubMenu(new cMenuDirSelect(data->defrecdir));
                if (iOnDefTimerCheck == 1)
                    state = AddSubMenu(new cMenuDefTimerCheckMethod());
            }
            break;
        case kOk:
            return osBack;
        default:
            break;
        }
    }

    return state;
}

// ------------------
// cMenuSetupSearchtimers
cMenuSetupSearchtimers::cMenuSetupSearchtimers(cEPGSearchConfig* Data)
    : cMenuSetupSubMenu(tr("Search and search timers"), Data)
{
    menuitemsChGr = NULL;
    Set();
}

cMenuSetupSearchtimers::~cMenuSetupSearchtimers()
{
    if (menuitemsChGr)
        free(menuitemsChGr);
}

void cMenuSetupSearchtimers::Set()
{
    int current = Current();
    Clear();
    helpTexts.clear();

    Add(new cMenuEditBoolItem(tr("Use search timers"), &data->useSearchTimers, trVDR("no"), trVDR("yes")));
    AddHelp(tr("Help$'Search timers' can be used to automatically create timers for events that match your search criterions."));
    if (data->useSearchTimers) {
        Add(new cMenuEditIntItem(tr("  Update interval [min]"), &data->UpdateIntervall, 1, 9999));
        AddHelp(tr("Help$Specify here the time intervall to be used when searching for events in the background."));
        Add(new cMenuEditIntItem(tr("  SVDRP port"), &data->SVDRPPort, 1, 99999));
        AddHelp(tr("Help$Programming of new timers or timer changes is done with SVDRP. The default value should be correct, so change it only if you know what you are doing."));
        Add(new cMenuEditIntItem(IndentMenuItem(trVDR("Setup.Recording$Default priority")), &data->DefPriority, 0, MAXPRIORITY));
        AddHelp(tr("Help$Specify here the default priority of timers created with this plugin. This value can also be adjusted for each search itself."));
        Add(new cMenuEditIntItem(IndentMenuItem(trVDR("Setup.Recording$Default lifetime (d)")), &data->DefLifetime, 0, MAXLIFETIME));
        AddHelp(tr("Help$Specify here the default lifetime of timers/recordings created with this plugin. This value can also be adjusted for each search itself."));
        Add(new cMenuEditIntItem(IndentMenuItem(trVDR("Setup.Recording$Margin at start (min)")), &data->DefMarginStart));
        AddHelp(tr("Help$Specify here the default start recording margin of timers/recordings created with this plugin. This value can also be adjusted for each search itself."));
        Add(new cMenuEditIntItem(IndentMenuItem(trVDR("Setup.Recording$Margin at stop (min)")), &data->DefMarginStop));
        AddHelp(tr("Help$Specify here the default stop recording margin of timers/recordings created with this plugin. This value can also be adjusted for each search itself."));
        Add(new cMenuEditBoolItem(IndentMenuItem(tr("No announcements when replaying")), &data->noAnnounceWhileReplay, trVDR("no"), trVDR("yes")));
        AddHelp(tr("Help$Set this to 'yes' if you don't like to get any announcements of broadcasts if you currently replay anything."));
        Add(new cMenuEditBoolItem(IndentMenuItem(tr("Recreate timers after deletion")), &data->TimerProgRepeat, trVDR("no"), trVDR("yes")));
        AddHelp(tr("Help$Set this to 'yes' if you want timers to be recreated with the next search timer update after deleting them."));
        Add(new cMenuEditIntItem(IndentMenuItem(tr("Check if EPG exists for ... [h]")), &data->checkEPGHours, 0, 999));
        AddHelp(tr("Help$Specify how many hours of future EPG there should be and get warned else after a search timer update."));
        if (data->checkEPGHours > 0) {
            Add(new cMenuEditBoolItem(IndentMenuItem(tr("Warn by OSD"), 2), &data->checkEPGWarnByOSD, trVDR("no"), trVDR("yes")));
            AddHelp(tr("Help$Set this to 'yes' if you want get warnings from the EPG check via OSD."));
            Add(new cMenuEditBoolItem(IndentMenuItem(tr("Warn by mail"), 2), &data->checkEPGWarnByMail, trVDR("no"), trVDR("yes")));
            AddHelp(tr("Help$Set this to 'yes' if you want get warnings from the EPG check by mail."));

            // create the char array for the menu display
            if (menuitemsChGr) delete [] menuitemsChGr;
            menuitemsChGr = ChannelGroups.CreateMenuitemsList();
            Add(new cMenuEditStraItem(IndentMenuItem(tr("Channel group to check"), 2), &data->checkEPGchannelGroupNr, ChannelGroups.Count() + 1, menuitemsChGr));
            AddHelp(tr("Help$Specify the channel group to check."));
        }
    }

    Add(new cMenuEditBoolItem(tr("Ignore PayTV channels"), &data->ignorePayTV, trVDR("no"), trVDR("yes")));
    AddHelp(tr("Help$Set this to 'yes' if don't want to see events on PayTV channels when searching for repeats."));
    Add(new cOsdItem(tr("Search templates")));
    AddHelp(tr("Help$Here you can setup templates for your searches."));
    Add(new cOsdItem(tr("Blacklists")));
    AddHelp(tr("Help$Here you can setup blacklists which can be used within a search to exclude events you don't like."));
    Add(new cOsdItem(tr("Channel groups")));
    AddHelp(tr("Help$Here you can setup channel groups which can be used within a search. These are different to VDR channel groups and represent a set of arbitrary channels, e.g. 'FreeTV'."));

    SetCurrent(Get(current));
    Display();
    SetHelpKeys();
}

void cMenuSetupSearchtimers::SetHelpKeys()
{
    const char* ItemText = Get(Current())->Text();
    if (!HasSubMenu()) {
        if (strstr(ItemText, tr("Channel groups")) == ItemText)
            SetHelp(NULL, NULL, tr("Button$Help"), tr("Button$Setup"));
        else if (strstr(ItemText, tr("Search templates")) == ItemText)
            SetHelp(NULL, NULL, tr("Button$Help"), tr("Button$Setup"));
        else if (strstr(ItemText, tr("Blacklists")) == ItemText)
            SetHelp(NULL, NULL, tr("Button$Help"), tr("Button$Setup"));
        else
            SetHelp(NULL, NULL, tr("Button$Help"), NULL);
    }
}

eOSState cMenuSetupSearchtimers::ProcessKey(eKeys Key)
{
    int iTemp_useSearchTimers = data->useSearchTimers;
    int iTemp_checkEPGHours = data->checkEPGHours;

    int iOnSearchTemplates = 0;
    int iOnBlacklists = 0;
    int iOnChannelGroups = 0;

    eOSState state = cMenuSetupSubMenu::ProcessKey(Key);

    if (iTemp_useSearchTimers != data->useSearchTimers ||
        iTemp_checkEPGHours != data->checkEPGHours) {
        Set();
        Display();
    }

    const char* ItemText = Get(Current())->Text();
    if (!HasSubMenu()) {
        if (strstr(ItemText, tr("Search templates")) == ItemText)
            iOnSearchTemplates = 1;
        else if (strstr(ItemText, tr("Blacklists")) == ItemText)
            iOnBlacklists = 1;
        if (strstr(ItemText, tr("Channel groups")) == ItemText)
            iOnChannelGroups = 1;
    }
    SetHelpKeys();

    if (state == osUnknown) {
        switch (Key) {
        case kBlue:
            if (!HasSubMenu()) {
                if (iOnSearchTemplates == 1)
                    state = AddSubMenu(new cMenuEPGSearchTemplate(NULL, NULL, false));
                else if (iOnBlacklists == 1)
                    state = AddSubMenu(new cMenuBlacklists);
                else if (iOnChannelGroups == 1)
                    state = AddSubMenu(new cMenuChannelGroups);
            }
            break;
        case kOk:
            return osBack;
        default:
            break;
        }
    }
    return state;
}

// ------------------------
// cMenuSetupTimerConflicts
cMenuSetupTimerConflicts::cMenuSetupTimerConflicts(cEPGSearchConfig* Data)
    : cMenuSetupSubMenu(tr("Timer conflict checking"), Data)
{
    Set();
}

void cMenuSetupTimerConflicts::Set()
{
    int current = Current();
    Clear();
    helpTexts.clear();

    Add(new cMenuEditIntItem(tr("Ignore below priority"), &data->checkMinPriority, 0, MAXPRIORITY));
    AddHelp(tr("Help$If a timer with priority below the given value will fail it will not be classified as important. Only important conflicts will produce an OSD message about the conflict after an automatic conflict check."));
    Add(new cMenuEditIntItem(tr("Ignore conflict duration less ... min."), &data->checkMinDuration, 0, 999));
    AddHelp(tr("Help$If a conflicts duration is less then the given number of minutes it will not be classified as important. Only important conflicts will produce an OSD message about the conflict after an automatic conflict check."));
    Add(new cMenuEditIntItem(tr("Only check within next ... days"), &data->checkMaxDays, 1, 14));
    AddHelp(tr("Help$This value reduces the conflict check to the given range of days. All other conflicts are classified as 'not yet important'."));

    Add(new cMenuEditBoolItem(tr("Check also remote conflicts"), &data->RemoteConflictCheck, trVDR("no"), trVDR("yes")));
    AddHelp(tr("Help$Set this to 'yes' if the conflict check should be done for timers set on remote hosts. This needs SVDRPPeering to be set in vdr and Plugin epgsearch running on the remote host."));

    cOsdItem* sep = new cOsdItem(tr("--- Automatic checking ---"));
    sep->SetSelectable(false);
    Add(sep);
    AddHelp("dummy");

    Add(new cMenuEditBoolItem(tr("After each timer programming"), &data->checkTimerConflAfterTimerProg, trVDR("no"), trVDR("yes")));
    AddHelp(tr("Help$Set this to 'yes' if the conflict check should be performed after each manual timer programming. In the case of a conflict you get immediately a message that informs you about it. The message is only displayed if this timer is involved in any conflict."));
    Add(new cMenuEditBoolItem(tr("When a recording starts"), &data->checkTimerConflOnRecording, trVDR("no"), trVDR("yes")));
    AddHelp(tr("Help$Set this to 'yes' if the conflict check should be performed when a recording starts. In the case of a conflict you get immediately a message that informs you about it. The message is only displayed if the conflict is within the next 2 hours."));

    Add(new cMenuEditBoolItem(tr("After each search timer update"), &data->checkTimerConflictsAfterUpdate, trVDR("no"), trVDR("yes")));
    AddHelp(tr("Help$Set this to 'yes' if the conflict check should be performed after each search timer update."));
    if (!data->checkTimerConflictsAfterUpdate) {
        Add(new cMenuEditIntItem(IndentMenuItem(tr("every ... minutes")), &data->conflictCheckIntervall, 0, 999));
        AddHelp(tr("Help$Specify here the time intervall to be used for an automatic conflict check in the background.\n('0' disables an automatic check)"));
        Add(new cMenuEditIntItem(IndentMenuItem(tr("if conflicts within next ... minutes")), &data->conflictCheckWithinLimit, 0, 9999));
        AddHelp(tr("Help$If the next conflict will appear in the given number of minutes you can specify here a shorter check intervall to get more OSD notifications about it."));
        if (data->conflictCheckWithinLimit) {
            Add(new cMenuEditIntItem(IndentMenuItem(IndentMenuItem(tr("every ... minutes"))), &data->conflictCheckIntervall2, 1, 999));
            AddHelp(tr("Help$If the next conflict will appear in the given number of minutes you can specify here a shorter check intervall to get more OSD notifications about it."));
        }
    }
    Add(new cMenuEditBoolItem(tr("Avoid notification when replaying"), &data->noConflMsgWhileReplay, trVDR("no"), trVDR("yes")));
    AddHelp(tr("Help$Set this to 'yes' if the don't want to get OSD messages about conflicts if you currently replay something. Nevertheless messages will be displayed if the first upcoming conflict is within the next 2 hours."));

    SetCurrent(Get(current));
    Display();
    SetHelp(NULL, NULL, tr("Button$Help"), NULL);
}

eOSState cMenuSetupTimerConflicts::ProcessKey(eKeys Key)
{
    int iTemp_checkTimerConflictsAfterUpdate = data->checkTimerConflictsAfterUpdate;
    int iTemp_conflictCheckWithinLimit = data->conflictCheckWithinLimit;

    eOSState state = cMenuSetupSubMenu::ProcessKey(Key);

    if (iTemp_checkTimerConflictsAfterUpdate != data->checkTimerConflictsAfterUpdate ||
        (iTemp_conflictCheckWithinLimit != data->conflictCheckWithinLimit && (iTemp_conflictCheckWithinLimit == 0 || data->conflictCheckWithinLimit == 0))) {
        Set();
        Display();
    }


    if (state == osUnknown) {
        switch (Key) {
        case kOk:
            return osBack;
        default:
            break;
        }
    }

    return state;
}

// --------------------------
// cMenuSetupMailNotification
cMenuSetupMailNotification::cMenuSetupMailNotification(cEPGSearchConfig* Data)
    : cMenuSetupSubMenu(tr("Email notification"), Data)
{
    string strHidden(strlen(data->MailAuthPass), '*');
    strcpy(tmpMailAuthPass, strHidden.c_str());
    Set();
}

void cMenuSetupMailNotification::Set()
{
    int current = Current();
    Clear();
    helpTexts.clear();

    Add(new cMenuEditBoolItem(tr("Search timer notification"), &data->sendMailOnSearchtimers, trVDR("no"), trVDR("yes")));
    AddHelp(tr("Help$Set this to 'yes' if you want to get an email notification about the search timers that where programmed automatically in the background."));

    if (data->sendMailOnSearchtimers) {
        Add(new cMenuEditIntItem(IndentMenuItem(tr("Time between mails [h]")), &data->sendMailOnSearchtimerHours,  0, 999999, ""));
        AddHelp(tr("Help$Specifiy how much time in [h] you would\nlike to have atleast between two mails.\nWith '0' you get a new mail after each\nsearch timer update with new results."));
    }
    Add(new cMenuEditBoolItem(tr("Timer conflict notification"), &data->sendMailOnConflicts, trVDR("no"), trVDR("yes")));
    AddHelp(tr("Help$Set this to 'yes' if you want to get an email notification about the timer conflicts."));

    Add(new cMenuEditStrItem(tr("Send to"), data->MailAddressTo, sizeof(data->MailAddressTo), MailBoxChars));
    AddHelp(tr("Help$Specify the email address where notifications should be sent to."));

    Add(new cMenuEditStraItem(tr("Mail method"), &data->mailViaScript, 2, MailMethod));
    AddHelp(tr("Help$Specify here the method to use when sending mails.\nYou can choose between\n - 'sendmail': requires a properly configured email system\n - 'SendEmail.pl': simple script for mail delivery"));

    if (data->mailViaScript) {
        cOsdItem* sep = new cOsdItem(tr("--- Email account ---"));
        sep->SetSelectable(false);
        Add(sep);
        AddHelp(" dummy");

        Add(new cMenuEditStrItem(tr("Email address"), data->MailAddress, sizeof(data->MailAddress), MailBoxChars));
        AddHelp(tr("Help$Specify the email address where notifications should be sent from."));

        Add(new cMenuEditStrItem(tr("SMTP server"), data->MailServer, sizeof(data->MailServer), HostNameChars));
        AddHelp(tr("Help$Specify the SMTP server that should deliver the notifications. If it's using a port different from the default(25) append the port with \":port\"."));
        Add(new cMenuEditBoolItem(tr("Use SMTP authentication"), &data->MailUseAuth, trVDR("no"), trVDR("yes")));
        AddHelp(tr("Help$Set this to 'yes' if your account needs authentication to send mails."));

        if (data->MailUseAuth) {
            Add(new cMenuEditStrItem(IndentMenuItem(tr("Auth user")), data->MailAuthUser, sizeof(data->MailAuthUser), UserNameChars));
            AddHelp(tr("Help$Specify the auth user, if this account needs authentication for SMTP."));
            Add(new cMenuEditStrItem(IndentMenuItem(tr("Auth password")), tmpMailAuthPass, sizeof(tmpMailAuthPass), PasswordChars));
            AddHelp(tr("Help$Specify the auth password, if this account needs authentication for SMTP."));
        }
    }

    SetCurrent(Get(current));
    Display();

    SetHelpKeys();
}

eOSState cMenuSetupMailNotification::TestMailAccount()
{
    if (strlen(data->MailAddress) == 0 || strlen(data->MailServer) == 0)
        return osContinue;
    cMailNotifier M;
    if (M.TestMailAccount(data->MailAddressTo, data->MailAddress, data->MailServer, data->MailAuthUser, data->MailAuthPass))
        return AddSubMenu(new cMenuText("", M.scriptReply.c_str(), fontSml));
    else
        ERROR(tr("Mail account check failed!"));
    return osContinue;
}

void cMenuSetupMailNotification::SetHelpKeys()
{
    bool showTestButton = strlen(data->MailAddress) > 0 && strlen(data->MailServer) > 0 && data->mailViaScript;

    const char* ItemText = Get(Current())->Text();
    if (!HasSubMenu()) {
        if (strstr(ItemText, tr("Email address")) == ItemText) {
            if (!InEditMode(ItemText, tr("Email address"), data->MailAddress))
                SetHelp(NULL, NULL, tr("Button$Help"), showTestButton ? tr("Button$Test") : NULL);
        } else if (strstr(ItemText, tr("SMTP server")) == ItemText) {
            if (!InEditMode(ItemText, tr("SMTP server"), data->MailServer))
                SetHelp(NULL, NULL, tr("Button$Help"), showTestButton ? tr("Button$Test") : NULL);
        } else if (strstr(ItemText, tr("Use SMTP authentication")) == ItemText)
            SetHelp(NULL, NULL, tr("Button$Help"), showTestButton ? tr("Button$Test") : NULL);
        else if (strstr(ItemText, IndentMenuItem(tr("Auth user"))) == ItemText) {
            if (!InEditMode(ItemText, IndentMenuItem(tr("Auth user")), data->MailAuthUser))
                SetHelp(NULL, NULL, tr("Button$Help"), showTestButton ? tr("Button$Test") : NULL);
        } else if (strstr(ItemText, IndentMenuItem(tr("Auth password"))) == ItemText) {
            if (!InEditMode(ItemText, IndentMenuItem(tr("Auth password")), tmpMailAuthPass))
                SetHelp(NULL, NULL, tr("Button$Help"), showTestButton ? tr("Button$Test") : NULL);
        } else
            SetHelp(NULL, NULL, tr("Button$Help"), NULL);
    }
}

eOSState cMenuSetupMailNotification::ProcessKey(eKeys Key)
{
    int iTemp_MailUseAuth = data->MailUseAuth;
    int iTemp_sendMailOnSearchtimers = data->sendMailOnSearchtimers;
    int iTemp_mailViaScript = data->mailViaScript;

    const char* ItemText = Get(Current())->Text();
    bool bAuthPassWasInEditMode = false;
    if (ItemText && strlen(ItemText) > 0 && strstr(ItemText, IndentMenuItem(tr("Auth password"))) == ItemText)
        bAuthPassWasInEditMode = InEditMode(ItemText, IndentMenuItem(tr("Auth password")), tmpMailAuthPass);

    eOSState state = cMenuSetupSubMenu::ProcessKey(Key);

    ItemText = Get(Current())->Text();
    bool bAuthPassIsInEditMode = false;
    if (ItemText && strlen(ItemText) > 0 && strstr(ItemText, IndentMenuItem(tr("Auth password"))) == ItemText)
        bAuthPassIsInEditMode = InEditMode(ItemText, IndentMenuItem(tr("Auth password")), tmpMailAuthPass);

    if (bAuthPassWasInEditMode && !bAuthPassIsInEditMode) {
        strcpy(data->MailAuthPass, tmpMailAuthPass);
        string strHidden(strlen(data->MailAuthPass), '*');
        strcpy(tmpMailAuthPass, strHidden.c_str());
        Set();
        Display();
    }
    if (!bAuthPassWasInEditMode && bAuthPassIsInEditMode) {
        strcpy(tmpMailAuthPass, "");
        Set();
        Display();
        state = cMenuSetupSubMenu::ProcessKey(Key);
    }

    if (iTemp_MailUseAuth != data->MailUseAuth ||
        iTemp_mailViaScript != data->mailViaScript ||
        iTemp_sendMailOnSearchtimers != data->sendMailOnSearchtimers) {
        Set();
        Display();
    }

    SetHelpKeys();

    if (state == osUnknown) {
        switch (Key) {
        case kOk:
            return osBack;
        case kBlue:
            if (data->mailViaScript)
                return TestMailAccount();
            else
                return osContinue;
        default:
            break;
        }
    }

    return state;
}

