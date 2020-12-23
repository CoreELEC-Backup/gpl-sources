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

#ifndef VDR_CONFLICTCHECK_THREAD_H
#define VDR_CONFLICTCHECK_THREAD_H

#include <vdr/thread.h>
#include "conflictcheck.h"
#include "epgsearch.h"

class cConflictCheckThread: public cThread
{
private:
    bool m_Active;
    time_t m_lastUpdate;
    cPluginEpgsearch* m_plugin;
    static bool m_runOnce;
    static bool m_forceUpdate;
    cCondWait Wait;
protected:
    virtual void Action(void);
    void Stop(void);
public:
    static cConflictCheckThread *m_Instance;
    static time_t m_cacheNextConflict;
    static int m_cacheRelevantConflicts;
    static int m_cacheTotalConflicts;
    cConflictCheckThread(cPluginEpgsearch* thePlugin);
    virtual ~cConflictCheckThread();
    static void Init(cPluginEpgsearch* thePlugin, bool runOnce = false);
    static void Exit(void);
};

#endif
