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

static const char VERSION[]       = "0.0.1";
static const char DESCRIPTION[]   = trNOOP("Direct access to epgsearch's conflict check menu");
static const char MAINMENUENTRY[] = trNOOP("Timer conflicts");
static const char SETUPTEXT[]     = trNOOP("Conflict info in main menu");

cString DateTime(time_t t)
{
    char buffer[32];
    if (t == 0) {
        time(&t);
    }
    struct tm tm_r;
    tm *tm = localtime_r(&t, &tm_r);
    snprintf(buffer, sizeof(buffer), "%02d.%02d. %02d:%02d", tm->tm_mday,
             tm->tm_mon + 1, tm->tm_hour, tm->tm_min);
    return buffer;
}

class cPluginConflictcheckonly: public cMainMenuShortcut
{
private:
    char *_menuText;

public:
    cPluginConflictcheckonly();
    ~cPluginConflictcheckonly();
    virtual const char *Version() {
        return VERSION;
    }
    virtual const char *Description() {
        return I18nTranslate(DESCRIPTION, I18nEpgsearch);
    }
    virtual bool Initialize();
    virtual cOsdObject *MainMenuAction() {
        return GetEpgSearchMenu("Epgsearch-conflictmenu-v1.0");
    }

protected:
    virtual const char *SetupText() {
        return I18nTranslate(SETUPTEXT, I18nEpgsearch);
    }
    virtual const char *MainMenuText(void);
};

cPluginConflictcheckonly::cPluginConflictcheckonly(): _menuText(NULL)
{
}

cPluginConflictcheckonly::~cPluginConflictcheckonly()
{
    free(_menuText);
}

const char *cPluginConflictcheckonly::MainMenuText(void)
{
    const char *menuText = I18nTranslate(MAINMENUENTRY, I18nEpgsearch);

    cPlugin *epgSearchPlugin = cPluginManager::GetPlugin("epgsearch");
    if (epgSearchPlugin) {
        Epgsearch_lastconflictinfo_v1_0 *serviceData = new Epgsearch_lastconflictinfo_v1_0;
        if (epgSearchPlugin->Service("Epgsearch-lastconflictinfo-v1.0", serviceData)) {
            if (serviceData->relevantConflicts > 0) {
                free(_menuText);
                if (asprintf(&_menuText, "%s (%d, %s: %s)", menuText, serviceData->relevantConflicts,
                             I18nTranslate(trNOOP("next"), I18nEpgsearch), *DateTime(serviceData->nextConflict)))
                    menuText = _menuText;
            }
        }
        delete serviceData;
    }
    return menuText;
}

bool cPluginConflictcheckonly::Initialize(void)
{
    return cMainMenuShortcut::Initialize();
}

VDRPLUGINCREATOR(cPluginConflictcheckonly); // Don't touch this!
