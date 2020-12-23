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
#include "epgsearchcats.h"
#include "log.h"
#include "epgsearchtools.h"
#include <vdr/tools.h>

using std::string;

// -- cSearchExtCat -----------------------------------------------------------------
cSearchExtCat::cSearchExtCat(void)
{
    id = 0;
    name = NULL;
    format = NULL;
    menuname = NULL;
    searchmode = 1; // default: all substrings must exist
    values = NULL;
    nvalues = 0;
}

cSearchExtCat::~cSearchExtCat(void)
{
    free(name);
    free(menuname);
    for (int i = 0; i < nvalues; i++)
        free(values[i]);
    free(values);
}

bool cSearchExtCat::Parse(const char *s)
{
    char *line;
    char *pos;
    char *pos_next;
    int parameter = 1;
    int valuelen;

#define MAXVALUELEN (10 * MaxFileName)

    char value[MAXVALUELEN];

    pos = line = strdup(s);
    pos_next = pos + strlen(pos);
    if (*pos_next == '\n') *pos_next = 0;
    while (*pos) {
        while (*pos == ' ') pos++;
        if (*pos) {
            if (*pos != '|') {
                pos_next = strchr(pos, '|');
                if (!pos_next)
                    pos_next = pos + strlen(pos);
                valuelen = pos_next - pos + 1;
                if (valuelen > MAXVALUELEN) {
                    LogFile.eSysLog("entry '%s' is too long. Will be truncated!", pos);
                    valuelen = MAXVALUELEN;
                }
                strn0cpy(value, pos, valuelen);
                pos = pos_next;
                switch (parameter) {
                case 1:
                    id = atoi(value);
                    break;
                case 2: {
                    name = strdup(value);
                    format = strchr(name, ',');
                    if (format) {
                        *format = 0;
                        format++;
                        char cset[] = "%0123456789di";
                        if (strspn(format, cset) != strlen(format)) format = NULL;
                    }
                    break;
                }
                case 3:
                    menuname = strdup(value);
                    break;
                case 4: {
                    char* szBuffer = strdup(value);
                    char* pptr;
                    char* pstrToken = strtok_r(szBuffer, ",", &pptr);
                    while (pstrToken) {
                        nvalues++;
                        char **tmp = (char**) realloc(values, nvalues * sizeof(char*));
                        if (tmp) {
                            values = tmp;
                            values[nvalues - 1] = strdup(pstrToken);
                        }
                        pstrToken = strtok_r(NULL, ",", &pptr);
                    }
                    free(szBuffer);
                    break;
                }
                case 5:
                    searchmode = atoi(value);
                    break;
                default:
                    break;
                } //switch
            }
            parameter++;
        }
        if (*pos) pos++;
    } //while

    free(line);
    return (parameter >= 3) ? true : false;
}

const char *cSearchExtCat::ToText(void)
{
    char* buffer = NULL;
    string sValues = "";
    for (int i = 0; i < nvalues; i++)
        sValues += string(values[i]) + ((i < nvalues - 1) ? ", " : "");

    if (format) {
        msprintf(&buffer, "%d|%s,%s|%s|%s|%d",
                 id, name, format, menuname, sValues.c_str(), searchmode);
    } else {
        msprintf(&buffer, "%d|%s|%s|%s|%d",
                 id, name, menuname, sValues.c_str(), searchmode);
    }
    return buffer;
}

// -- cSearchExtCats ----------------------------------------------------------------
int cSearchExtCats::GetIndexFromID(int id)
{
    cSearchExtCat *SearchExtCat = SearchExtCats.First();
    int index = 0;
    while (SearchExtCat) {
        if (SearchExtCat->id == id)
            break;
        index++;
        SearchExtCat = SearchExtCats.Next(SearchExtCat);
    }
    if (!SearchExtCat && index == 0)
        return -1;
    else
        return index;
}
