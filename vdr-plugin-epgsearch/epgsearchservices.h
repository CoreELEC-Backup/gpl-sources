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

#ifndef _EPGSEARCH_SERVICES_INC_
#define _EPGSEARCH_SERVICES_INC_

#include "services.h"
#include <vdr/tools.h>
#include "epgsearchext.h"

class cEpgsearchServiceHandler: public cServiceHandler_v1_2
{
    virtual std::list<std::string> TranslateResults(cSearchResults* pCompleteSearchResults);
public:
    virtual std::list<std::string> SearchTimerList();
    virtual int AddSearchTimer(const std::string&);
    virtual bool ModSearchTimer(const std::string&);
    virtual bool DelSearchTimer(int);
    virtual std::list<std::string> QuerySearchTimer(int);
    virtual std::list<std::string> QuerySearch(std::string);
    virtual std::list<std::string> ExtEPGInfoList();
    virtual std::list<std::string> ChanGrpList();
    virtual std::list<std::string> BlackList();
    virtual std::set<std::string> DirectoryList();
    virtual std::string ReadSetupValue(const std::string& entry);
    virtual bool WriteSetupValue(const std::string& entry, const std::string& value);
    virtual std::list<std::string> TimerConflictList(bool relOnly = false);
    virtual bool IsConflictCheckAdvised();
    virtual std::set<std::string> ShortDirectoryList();
    virtual std::string Evaluate(const std::string& expr, const cEvent* event);
};

#endif
