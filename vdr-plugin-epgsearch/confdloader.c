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
#include <vdr/plugin.h>
#include "confdloader.h"
#include "log.h"
#include "uservars.h"
#include "menu_dirselect.h"
#include "templatefile.h"
#include "epgsearchcats.h"

using std::string;

// ---------------------------
// Loads all files in the conf.d subdirectory of <plugin-config-directory> and
// applies found settings.
// ---------------------------
bool cConfDLoader::Load()
{
    const string dirPath(AddDirectory(CONFIGDIR, "conf.d"));
    LogFile.Log(2, "loading entries in %s", dirPath.c_str());
    cReadDir d(dirPath.c_str());
    struct dirent* e;
    string parent("..");
    string current(".");
    int count = 0;
    bool success = true;
    while ((e = d.Next())) {
        string direntry = e->d_name;
        if ((current == direntry) || (parent == direntry) || (direntry[direntry.size() - 1] == '~')) {
            continue;
        }
        /* Check if entry is a directory: I do not rely on e->d_type
           here because on some systems it is always DT_UNKNOWN. Also
           testing for DT_DIR does not take into account symbolic
           links to directories.
        */
        struct stat buf;
        if ((stat((dirPath + "/" + e->d_name).c_str(), &buf) != 0) || (S_ISDIR(buf.st_mode))) {
            continue;
        }
        success &= LoadFile((dirPath + "/" + e->d_name).c_str());
        count++;
    }
    LogFile.Log(2, "loaded %d entries in %s", count, dirPath.c_str());
    return success;
}

// ---------------------------
// Each file has the form
//
// [<section>]
// <setting>
// ...
// [<section>]
// ...
//
// where section is one:
//
// epgsearchuservars
// epgsearchdirs
// epgsearchmenu
// epgsearchcats
//
// <setting> corresponds to the entries in the related conf files.
// ---------------------------
bool cConfDLoader::LoadFile(const char *FileName)
{
    if (FileName && access(FileName, F_OK) == 0) {
        LogFile.Log(1, "loading %s", FileName);
        FILE *f = fopen(FileName, "r");
        if (f) {
            char *s;
            int line = 0;
            cReadLine ReadLine;
            std::string section;
            while ((s = ReadLine.Read(f)) != NULL) {
                line++;
                char *p = strchr(s, '#');
                if (p)
                    *p = 0;
                stripspace(s);
                if (!isempty(s)) {
                    if (*s == '[' && *(s + strlen(s) - 1) == ']') // Section?
                        section = s;
                    else {
                        if (EqualsNoCase(section, "[epgsearchuservars]"))
                            cUserVarLine::Parse(s);
                        if (EqualsNoCase(section, "[epgsearchdirs]")) {
                            cDirExt* D = new cDirExt;
                            if (D && D->Parse(s))
                                ConfDDirExts.Add(D);
                            else
                                delete D;
                        }
                        if (EqualsNoCase(section, "[epgsearchmenu]")) {
                            cTemplLine T;
                            if (T.Parse(s))
                                cTemplFile::Parse(T.Name(), T.Value());
                        }
                        if (EqualsNoCase(section, "[epgsearchcats]")) {
                            cSearchExtCat* cat = new cSearchExtCat;
                            if (cat && cat->Parse(s))
                                SearchExtCats.Add(cat);
                            else
                                delete cat;
                        }
                    }
                }
            }
            fclose(f);
        }
        return true;
    } else {
        LOG_ERROR_STR(FileName);
        return false;
    }
}

