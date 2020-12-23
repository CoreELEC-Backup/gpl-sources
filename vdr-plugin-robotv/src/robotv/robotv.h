/*
 *      vdr-plugin-robotv - roboTV server plugin for VDR
 *
 *      Copyright (C) 2016 Alexander Pipelka
 *
 *      https://github.com/pipelka/vdr-plugin-robotv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include <getopt.h>
#include <vdr/plugin.h>
#include "svdrp/channelcmds.h"

#include "robotvserver.h"

static const char* VERSION = ROBOTV_VERSION;
static const char* DESCRIPTION = "roboTV Server";

class PluginRoboTVServer : public cPlugin {
private:

    RoboTVServer* m_server;

    ChannelCmds m_channels;

public:

    PluginRoboTVServer(void);

    virtual ~PluginRoboTVServer();

    virtual const char* Version(void) {
        return VERSION;
    }

    virtual const char* Description(void) {
        return DESCRIPTION;
    }

    virtual const char* CommandLineHelp(void);

    virtual bool ProcessArgs(int argc, char* argv[]);

    virtual bool Initialize(void);

    virtual bool Start(void);

    virtual void Stop(void);

    virtual void Housekeeping(void);

    virtual void MainThreadHook(void);

    virtual cString Active(void);

    virtual time_t WakeupTime(void);

    virtual const char* MainMenuEntry(void) {
        return NULL;
    }
    virtual cOsdObject* MainMenuAction(void) {
        return NULL;
    }
    virtual cMenuSetupPage* SetupMenu(void);

    virtual bool SetupParse(const char* Name, const char* Value);

    virtual bool Service(const char* Id, void* Data = NULL);

    virtual const char** SVDRPHelpPages(void);

    virtual cString SVDRPCommand(const char* Command, const char* Option, int& ReplyCode);

};

