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

#include <set>
#include "templatefile.h"
#include "epgsearchcfg.h"
#include "epgsearchtools.h"

std::set<cMenuTemplate*> cTemplFile::menuTemplates;
char** cTemplFile::SearchTemplates = NULL;

bool cMenuTemplate::PrepareTemplate(const char* templateLine)
{
    if (!templateLine || strlen(templateLine) == 0) return true;

    char *pstrSearchToken, *pptr;
    char *pstrSearch = strdup(templateLine);
    pstrSearchToken = strtok_r(pstrSearch, "|", &pptr);
    cString stripped;
    int iToken = 0;
    while (pstrSearchToken) {
        char* tmp = strchr(pstrSearchToken, ':');
        if (tmp) {
            int tabWidth = atoi(tmp + 1);
            if (tabWidth == 0) {
                LogFile.eSysLog("warning - tab width in \"%s\" is 0, set to min value 1!", Name());
                tabWidth = 1;
            }
            menuTabs[iToken++] = tabWidth;
        } else {
            free(pstrSearch);
            LogFile.eSysLog("error parsing entry \"%s\", falling back to default entries.", Name());
            return false;
        }
        *tmp = 0;
        if (isempty(stripped))
            stripped = strdup(pstrSearchToken);
        else {
            cString tmp = stripped;
            stripped = cString::sprintf("%s|%s", *tmp, pstrSearchToken);
        }
        pstrSearchToken = strtok_r(NULL, "|", &pptr);
    }
    free(pstrSearch);

    // no limit for the last column
    if (iToken > 0) menuTabs[iToken - 1] = 0;

    if (!isempty(stripped)) {
        menuTemplate = strdup(stripped);
        // the status variables are handled in menu_whatson.c itself
        // to speedup the var-parser we 'hide' them here in renaming them
        menuTemplate = strreplacei(menuTemplate, "%status%", "$status$");
        menuTemplate = strreplacei(menuTemplate, "%t_status%", "$t_status$");
        menuTemplate = strreplacei(menuTemplate, "%v_status%", "$v_status$");
        menuTemplate = strreplacei(menuTemplate, "%r_status%", "$r_status$");
    }
    return true;
}

cTemplLine::cTemplLine(void)
{
    name = value = NULL;
}

cTemplLine::cTemplLine(const char *Name, const char *Value)
{
    name = strdup(Name);
    value = strdup(Value);
}

cTemplLine::~cTemplLine()
{
    free(name);
    free(value);
}

bool cTemplLine::Parse(char *s)
{
    if (!s) return false;
    if (s[0] == '#')
        return true;
    char *p = strchr(s, '=');
    if (p) {
        *p = 0;
        char *Name  = compactspace(s);
        char *Value = compactspace(p + 1);
        if (*Name) {
            name = strdup(Name);
            value = strdup(Value);
            return true;
        }
    }
    return false;
}

cTemplFile::cTemplFile()
{
    Reset();
}

void cTemplFile::Reset()
{
    std::set<cMenuTemplate*>::iterator it;
    for (it = menuTemplates.begin(); it != menuTemplates.end(); ++it)
        delete(*it);
    menuTemplates.clear();
}

cMenuTemplate* cTemplFile::GetTemplateByName(const char* Name)
{
    std::set<cMenuTemplate*>::iterator it;
    for (it = menuTemplates.begin(); it != menuTemplates.end(); ++it)
        if (!strcasecmp(Name, (*it)->Name())) return (*it);
    return NULL;
}

bool cTemplFile::Load(const char *FileName)
{
    // auto-enable WarEagle-Icons if VDRSymbols font is used
    if (strstr(Setup.FontOsd, "VDRSymbols") == Setup.FontOsd)
        EPGSearchConfig.WarEagle = 1;

    if (cConfig<cTemplLine>::Load(FileName, true)) {
        bool result = true;
        for (cTemplLine *l = First(); l; l = Next(l)) {
            bool error = false;
            if (!Parse(l->Name(), l->Value()))
                error = true;
            if (error) {
                result = false;
            }
        }
        return result;
    }
    return false;
}

bool cTemplFile::Parse(const char *Name, const char *Value)
{
    if (Name && Name[0] == '#') return true;
    if (!strcasecmp(Name, "WarEagleIcons")) {
        EPGSearchConfig.WarEagle = atoi(Value);
        return true;
    }

    if (!strcasecmp(Name, "MenuWhatsOnNow") ||
        !strcasecmp(Name, "MenuWhatsOnNext") ||
        !strcasecmp(Name, "MenuWhatsOnElse") ||
        !strcasecmp(Name, "MenuSchedule") ||
        !strncasecmp(Name, "MenuSearchResults", strlen("MenuSearchResults")) ||
        !strcasecmp(Name, "MenuFavorites")) {
        cMenuTemplate* menuTemplate = new cMenuTemplate(Name);
        if (menuTemplate->PrepareTemplate(Value)) {
            LogFile.Log(3, "loaded menu template: %s", Name);
            cMenuTemplate* TemplOld = GetTemplateByName(Name);
            if (TemplOld) {
                LogFile.Log(1, "menu template '%s' gets overwritten", Name);
                menuTemplates.erase(TemplOld);
                delete TemplOld;
            }
            menuTemplates.insert(menuTemplate);
            return true;
        }
    } else {
        LogFile.eSysLog("ERROR: unknown parameter: %s = %s", Name, Value);
        return false;
    }
    return true;
}

void cTemplFile::PrepareDefaultTemplates()
{
    char channelnr[20] = "";
    {
        LOCK_CHANNELS_READ;  // Channels used in CHNUMWIDTH
        sprintf(channelnr, "%%chnr%%:%d|", CHNUMWIDTH);
    }

    bool text2skin = !(strcmp(Setup.OSDSkin, "soppalusikka") == 0 ||
                       strcmp(Setup.OSDSkin, "classic") == 0 ||
                       strcmp(Setup.OSDSkin, "sttng") == 0);

    char menutemplate[MAXTEMPLLEN] = "";
    // What's on now
    cMenuTemplate* WhatsOnNow = GetTemplateByName("MenuWhatsOnNow");
    if (!WhatsOnNow) {
        WhatsOnNow = new cMenuTemplate("MenuWhatsOnNow");
        menuTemplates.insert(WhatsOnNow);
    }
    if (WhatsOnNow && WhatsOnNow->MenuTemplate() == 0) {
        sprintf(menutemplate, "%s%%chsh%%:12|%%time%%:6|%s%s$status$:3|%%title%% ~ %%subtitle%%:30",
                EPGSearchConfig.showChannelNr ? channelnr : "",
                EPGSearchConfig.showProgress == 0 ? "" : (EPGSearchConfig.showProgress == 1 ? "%progrT2S%:4|" : "%progr%:5|"),
                text2skin ? " " : "");
        WhatsOnNow->PrepareTemplate(menutemplate);
    }

    // What's on next and else
    sprintf(menutemplate, "%s%%chsh%%:12|%%time%%:7|$status$:4|%%title%% ~ %%subtitle%%:30",
            EPGSearchConfig.showChannelNr ? channelnr : "");
    cMenuTemplate* WhatsOnNext = GetTemplateByName("MenuWhatsOnNext");
    if (!WhatsOnNext) {
        WhatsOnNext = new cMenuTemplate("MenuWhatsOnNext");
        menuTemplates.insert(WhatsOnNext);
    }
    if (WhatsOnNext && WhatsOnNext->MenuTemplate() == 0)
        WhatsOnNext->PrepareTemplate(menutemplate);
    cMenuTemplate* WhatsOnElse = GetTemplateByName("MenuWhatsOnElse");
    if (!WhatsOnElse) {
        WhatsOnElse = new cMenuTemplate("MenuWhatsOnElse");
        menuTemplates.insert(WhatsOnElse);
    }
    if (WhatsOnElse && WhatsOnElse->MenuTemplate() == 0)
        WhatsOnElse->PrepareTemplate(menutemplate);

    // Schedule
    cMenuTemplate* Schedule = GetTemplateByName("MenuSchedule");
    if (!Schedule) {
        Schedule = new cMenuTemplate("MenuSchedule");
        menuTemplates.insert(Schedule);
    }
    if (Schedule && Schedule->MenuTemplate() == 0) {
        strcpy(menutemplate, "%time_w% %time_d%:7|%time%:6|$status$:4|%title% ~ %subtitle%:30");
        Schedule->PrepareTemplate(menutemplate);
    }

    // Search results
    cMenuTemplate* SearchResults = GetTemplateByName("MenuSearchResults");
    if (!SearchResults) {
        SearchResults = new cMenuTemplate("MenuSearchResults");
        menuTemplates.insert(SearchResults);
    }
    if (SearchResults && SearchResults->MenuTemplate() == 0) {
        sprintf(menutemplate, "%s%%chsh%%:12|%%datesh%%:6|%%time%%:6|$status$:3|%%title%% ~ %%subtitle%%:30",
                EPGSearchConfig.showChannelNr ? channelnr : "");
        SearchResults->PrepareTemplate(menutemplate);
    }

    // Favorites
    cMenuTemplate* Favorites = GetTemplateByName("MenuFavorites");
    if (!Favorites) {
        Favorites = new cMenuTemplate("MenuFavorites");
        menuTemplates.insert(Favorites);
    }
    if (Favorites && Favorites->MenuTemplate() == 0) {
        sprintf(menutemplate, "%s%%chsh%%:12|%%time%%:6|%%timespan%%:7|$status$:3|%%title%% ~ %%subtitle%%:30",
                EPGSearchConfig.showChannelNr ? channelnr : "");
        Favorites->PrepareTemplate(menutemplate);
    }

    // create an array of all search template names
    if (SearchTemplates)
        delete [] SearchTemplates;
    SearchTemplates = new char*[CountSearchResultsTemplates()];
    std::set<cMenuTemplate*>::iterator it;
    int Count = 0;
    for (it = menuTemplates.begin(); it != menuTemplates.end(); ++it)
        if (!strncasecmp("MenuSearchResults", (*it)->Name(), strlen("MenuSearchResults"))) {
            char* templateName = strdup((*it)->Name() + strlen("MenuSearchResults"));
            if (*templateName == 0) templateName = strdup(tr("Standard"));
            SearchTemplates[Count++] = templateName;
        }
}

int cTemplFile::CountSearchResultsTemplates()
{
    int Count = 0;
    std::set<cMenuTemplate*>::iterator it;
    for (it = menuTemplates.begin(); it != menuTemplates.end(); ++it)
        if (!strncasecmp("MenuSearchResults", (*it)->Name(), strlen("MenuSearchResults"))) Count++;
    return Count;
}

cMenuTemplate* cTemplFile::GetSearchTemplateByPos(int iPos)
{
    int Count = 0;
    std::set<cMenuTemplate*>::iterator it;
    for (it = menuTemplates.begin(); it != menuTemplates.end(); ++it)
        if (!strncasecmp("MenuSearchResults", (*it)->Name(), strlen("MenuSearchResults")))
            if (Count++ == iPos)
                return (*it);
    return NULL;
}
