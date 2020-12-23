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

#ifndef __EPGSEARCHTEMPLFILE_H
#define __EPGSEARCHTEMPLFILE_H

#include <vdr/config.h>
#include <vdr/skins.h>
#include <set>

using std::set;

#define MAXTEMPLLEN 2000

class cMenuTemplate
{
private:
    char name[MAXTEMPLLEN];
    char* menuTemplate;
    int  menuTabs[cSkinDisplayMenu::MaxTabs];
public:
    cMenuTemplate(const char* Name) {
        strcpy(name, Name);
        menuTemplate = 0;
        for (int i = 0; i < cSkinDisplayMenu::MaxTabs; i++) menuTabs[i] = 0;
    }
    ~cMenuTemplate() {
        if (menuTemplate) free(menuTemplate);
    }
    const char* Name(void) {
        return name;
    }
    const char* MenuTemplate(void) {
        return menuTemplate;
    }
    int Tab(int i) {
        return menuTabs[i];
    }
    bool PrepareTemplate(const char* templateLine);
};

class cTemplLine : public cListObject
{
private:
    char *name;
    char *value;
public:
    cTemplLine(void);
    cTemplLine(const char *Name, const char *Value);
    virtual ~cTemplLine();
    const char *Name(void) {
        return name;
    }
    const char *Value(void) {
        return value;
    }
    bool Parse(char *s);
};


class cTemplFile : public cConfig<cTemplLine>
{
public:
    static set<cMenuTemplate*> menuTemplates; // the set of all templates
    static char** SearchTemplates; // an array thats stores the name of all search results templates
    cTemplFile();
    static bool Parse(const char *Name, const char *Value);
    bool Load(const char *FileName);
    static void PrepareDefaultTemplates();
    static void Reset();
    static cMenuTemplate* GetTemplateByName(const char* Name);
    static int CountSearchResultsTemplates();
    static cMenuTemplate* GetSearchTemplateByPos(int iPos);
};


#endif
