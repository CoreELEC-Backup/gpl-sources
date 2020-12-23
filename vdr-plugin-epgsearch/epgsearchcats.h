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

#ifndef __EPGSEARCHCATS_H
#define __EPGSEARCHCATS_H

#include <vdr/status.h>
#include <vdr/plugin.h>
#include <vdr/tools.h>

// --- cSearchExtCat --------------------------------------------------------
class cSearchExtCat : public cListObject
{
public:
    int      id;
    char*    name;
    char*    format;
    char*    menuname;
    int      searchmode; // text comarison:
    // 0 - substring,
    // 1 - substring-and,
    // 2 - substring or,
    // 3 - equal,
    // 4 - regular expression,
    // 5 - fuzzy (not available for categories)
    // numerical comparison:
    // 10 - less
    // 11 - less or equal
    // 12 - greater
    // 13 - greater or equal
    // 14 - equal
    // 15 - not equal
    char**   values;
    int      nvalues;
public:
    cSearchExtCat(void);
    virtual ~cSearchExtCat(void);

    bool Parse(const char *s);
    const char* ToText(void);
};

class cSearchExtCats : public cConfig<cSearchExtCat>
{
private:
public:
    cSearchExtCats(void) {}
    int GetIndexFromID(int id);
};

extern cSearchExtCats SearchExtCats;

#endif
