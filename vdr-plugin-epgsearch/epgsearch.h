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

#ifndef VDR_EPGSEARCH_H
#define VDR_EPGSEARCH_H

#include "epgsearchext.h"

class cPluginEpgsearch : public cPlugin
{
public:
    bool showConflicts;
    bool showAnnounces;
    static bool VDR_readyafterStartup;

    cPluginEpgsearch(void);
    virtual ~cPluginEpgsearch();
    virtual const char *Version(void);
    virtual const char *Description(void);
    virtual const char *CommandLineHelp(void);
    virtual bool ProcessArgs(int argc, char *argv[]);
    virtual bool Initialize(void);
    virtual bool Start(void);
    virtual void Stop(void);
    virtual void MainThreadHook(void);
    virtual const char *MainMenuEntry(void);
    virtual cOsdObject *MainMenuAction(void);
    virtual cMenuSetupPage *SetupMenu(void);
    virtual bool SetupParse(const char *Name, const char *Value);
    cOsdObject *DoInitialSearch(char* rcFilename);
    void LoadMenuTemplates();
    bool Service(const char *Id, void *Data);
    virtual const char **SVDRPHelpPages(void);
    virtual cString SVDRPCommand(const char *Cmd, const char *Option, int &ReplyCode);
    void LoadUserVars();
    void LoadConfD();
    void CheckUTF8();
    virtual cString Active(void);
};

#endif
