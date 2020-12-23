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

#ifndef __MAINMENUSHORTCUT_H
#define __MAINMENUSHORTCUT_H

#include <vdr/plugin.h>

static const char I18nEpgsearch[] = "vdr-epgsearch";

class cMainMenuShortcutSetupPage : public cMenuSetupPage
{
private:
    const char* _setupEntry;
    int* const _setupValue;
    int dummy; // don't know why, but this is necessary to avoid a crash with ext-patch and active USE_LIEMIKUUTIO

public:
    cMainMenuShortcutSetupPage(const char* setupText, const char* setupEntry, int* const setupValue);

protected:
    virtual void Store(void);
};

class cMainMenuShortcut : public cPlugin
{
private:
    int _mainMenuEntryEnabled;

public:
    cMainMenuShortcut();
    virtual ~cMainMenuShortcut();
    virtual bool Initialize();
    virtual bool SetupParse(const char* Name, const char* Value);
    virtual cMenuSetupPage* SetupMenu();
    virtual const char* MainMenuEntry();

protected:
    cOsdMenu* GetEpgSearchMenu(const char* serviceName);
    virtual const char* SetupText() = 0;
    virtual const char* MainMenuText() = 0;
};

#endif
