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

#ifndef __EPGSEARCH_H
#define __EPGSEARCH_H

#include <vdr/plugin.h>
#include "epgsearchcfg.h"
#include <vector>

class cPluginEpgsearch;

class cMenuSetupSubMenu : public cOsdMenu
{
protected:
    cEPGSearchConfig* data;
    std::vector<const char*> helpTexts;
    eOSState Help();
    void AddHelp(const char* helpText);
    virtual eOSState ProcessKey(eKeys Key);
    virtual void Set(void) = 0;
public:
    cMenuSetupSubMenu(const char* Title, cEPGSearchConfig* Data);
};


class cMenuSetupGeneral : public cMenuSetupSubMenu
{
protected:
    virtual eOSState ProcessKey(eKeys Key);
    void Set(void);
public:
    cMenuSetupGeneral(cEPGSearchConfig* Data);
};

class cMenuSetupEPGMenus : public cMenuSetupSubMenu
{
protected:
    void Set(void);
    void SetHelpKeys();
public:
    cMenuSetupEPGMenus(cEPGSearchConfig* Data);
};

class cMenuSetupUserdefTimes : public cMenuSetupSubMenu
{
protected:
    virtual eOSState ProcessKey(eKeys Key);
    void Set(void);
public:
    cMenuSetupUserdefTimes(cEPGSearchConfig* Data);
};

class cMenuSetupTimers : public cMenuSetupSubMenu
{
protected:
    virtual eOSState ProcessKey(eKeys Key);
    void Set(void);
    void SetHelpKeys();
public:
    cMenuSetupTimers(cEPGSearchConfig* Data);
};

class cMenuSetupSearchtimers : public cMenuSetupSubMenu
{
    char** menuitemsChGr;
protected:
    virtual eOSState ProcessKey(eKeys Key);
    void Set(void);
    void SetHelpKeys();
public:
    cMenuSetupSearchtimers(cEPGSearchConfig* Data);
    ~cMenuSetupSearchtimers();
};

class cMenuSetupTimerConflicts : public cMenuSetupSubMenu
{
protected:
    virtual eOSState ProcessKey(eKeys Key);
    void Set(void);
public:
    cMenuSetupTimerConflicts(cEPGSearchConfig* Data);
};

class cMenuSetupMailNotification : public cMenuSetupSubMenu
{
    char tmpMailAuthPass[MaxFileName];
    void SetHelpKeys();
protected:
    virtual eOSState ProcessKey(eKeys Key);
    void Set(void);
public:
    cMenuSetupMailNotification(cEPGSearchConfig* Data);
    eOSState TestMailAccount();

    static const char *HostNameChars;
    static const char *UserNameChars;
    static const char *PasswordChars;
    static const char *MailBoxChars;
};

class cMenuEPGSearchSetup : public cMenuSetupPage
{
private:
    virtual void Setup(void);
    cEPGSearchConfig data;
    int delaySearchTimerThreadMode;
protected:
    virtual eOSState ProcessKey(eKeys Key);
    virtual void Store(void);
    void Set(void);
public:
    cMenuEPGSearchSetup(void);
};

#endif
