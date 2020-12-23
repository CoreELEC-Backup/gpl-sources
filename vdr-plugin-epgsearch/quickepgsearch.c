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
#include <vdr/plugin.h>
#include "services.h"
#include "mainmenushortcut.h"

static const char VERSION[]        = "0.0.1";
static const char DESCRIPTION[]    = trNOOP("Quick search for broadcasts");
static const char MAINMENUENTRY[]  = trNOOP("Quick search");
static const char SETUPTEXT[]      = trNOOP("Show in main menu");

class cPluginQuicksearch : public cMainMenuShortcut
{
public:
    virtual const char* Version() {
        return VERSION;
    }
    virtual const char* Description() {
        return I18nTranslate(DESCRIPTION, I18nEpgsearch);
    }
    virtual bool Initialize();
    virtual cOsdObject* MainMenuAction() {
        return GetEpgSearchMenu("Epgsearch-quicksearch-v1.0");
    };

protected:
    virtual const char* SetupText() {
        return I18nTranslate(SETUPTEXT, I18nEpgsearch);
    }
    virtual const char* MainMenuText() {
        return I18nTranslate(MAINMENUENTRY, I18nEpgsearch);
    }
};

bool cPluginQuicksearch::Initialize()
{
    return cMainMenuShortcut::Initialize();
}

VDRPLUGINCREATOR(cPluginQuicksearch); // Don't touch this!
