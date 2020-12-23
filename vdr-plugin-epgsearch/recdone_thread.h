/*                                                                  -*- c++ -*-
Copyright (C) 2004-2013 Christian Wieninger 2017 Johann Friedrichs

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

*/

#ifndef VDR_RECDONE_THREAD_H
#define VDR_RECDONE_THREAD_H

#include <vector>
#include <string>
#include <vdr/thread.h>
#include "recstatus.h"

using std::vector;
using std::string;

class cRecdoneThread: public cThread
{
private:
    vector<string>m_fnames;
public:
    virtual void Action(void);
    cRecdoneThread(void);
    virtual ~cRecdoneThread();
    void SetFilename(const char *FileName) {
        m_fnames.push_back(FileName);
    }
    int RecLengthInSecs(const cRecording *pRecording);
    bool IsPesRecording(const cRecording *pRecording);
};

#endif
