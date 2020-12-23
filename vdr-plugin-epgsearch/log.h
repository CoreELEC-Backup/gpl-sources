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

#ifndef _LOG_INC_
#define _LOG_INC_

#include <stdarg.h>
#include <stdio.h>
#include <vdr/tools.h>

class cLogFile: public cFile
{
public:
    static int loglevellimit;
    void Open(const char* filename, const char* version) {
        if (loglevellimit == 0) return;
        if (!cFile::Open(filename, O_CREAT | O_APPEND | O_WRONLY))
            esyslog("EPGSEARCH: could not open log file: %s", filename);
        Log(1, "---------------------------------------", loglevellimit);
        Log(1, "EPGSearch log started (verbose level %d, version %s)", loglevellimit, version);
    }
    void Log(int LogLevel, const char *text, ...) {
        if (LogLevel > loglevellimit) return;
        if (IsOpen()) {
            char* buffer = NULL;
            va_list Arg;
            va_start(Arg, text);
            if (vasprintf(&buffer, text, Arg) < 0)
                esyslog("EPGSearch: vasprintf error");
            va_end(Arg);
            time_t now = time(NULL);

            char datebuf[32];
            struct tm tm_r;
            tm *tm = localtime_r(&now, &tm_r);

            char *p = stpcpy(datebuf, WeekDayName(tm->tm_wday));
            *p++ = ' ';
            strftime(p, sizeof(datebuf) - (p - datebuf), "%d.%m.%Y", tm);

            char timebuf[25];
            strftime(timebuf, sizeof(timebuf), "%T", localtime_r(&now, &tm_r));

            cString log = cString::sprintf("%s %s: %s\n", datebuf, timebuf, buffer);
            free(buffer);
            safe_write(*this, log, strlen(log));
        }
    }
    void eSysLog(const char *text, ...) {
        char* buffer = NULL;
        va_list Arg;
        va_start(Arg, text);
        if (vasprintf(&buffer, text, Arg) < 0)
            esyslog("EPGSearch: vasprintf error");
        va_end(Arg);
        esyslog("EPGSearch: %s", buffer);
        Log(1, "%s", buffer);
        free(buffer);
    }
    void iSysLog(const char *text, ...) {
        char* buffer = NULL;
        va_list Arg;
        va_start(Arg, text);
        if (vasprintf(&buffer, text, Arg) < 0)
            esyslog("EPGSearch: vasprintf error");
        va_end(Arg);
        isyslog("EPGSearch: %s", buffer);
        Log(1, "%s", buffer);
        free(buffer);
    }
    int Level() {
        return loglevellimit;
    }

    static char *LogFileName;
};

extern cLogFile LogFile;

#endif


