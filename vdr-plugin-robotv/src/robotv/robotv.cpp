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
#include "robotv.h"

PluginRoboTVServer::PluginRoboTVServer(void) {
    m_server = NULL;
}

PluginRoboTVServer::~PluginRoboTVServer() {
    // Clean up after yourself!
}

const char* PluginRoboTVServer::CommandLineHelp(void) {
    return "  -t n, --timeout=n      stream data timeout in seconds (default: 3)\n";
}

bool PluginRoboTVServer::ProcessArgs(int argc, char* argv[]) {
    // Implement command line argument processing here if applicable.
    return true;
}

bool PluginRoboTVServer::Initialize(void) {
    // Initialize any background activities the plugin shall perform.
    RoboTVServerConfig& config = RoboTVServerConfig::instance();

    const char* directory = ConfigDirectory(PLUGIN_NAME_I18N);
    if(directory == nullptr) {
        return false;
    }

    config.configDirectory = directory;

    directory = CacheDirectory(PLUGIN_NAME_I18N);
    if(directory == nullptr) {
        return false;
    }

    config.cacheDirectory = directory;
    config.Load();

    return true;
}

bool PluginRoboTVServer::Start(void) {
    m_server = new RoboTVServer(RoboTVServerConfig::instance().listenPort);

    return true;
}

void PluginRoboTVServer::Stop(void) {
    delete m_server;
    m_server = NULL;
}

void PluginRoboTVServer::Housekeeping(void) {
    // Perform any cleanup or other regular tasks.
}

void PluginRoboTVServer::MainThreadHook(void) {
    // Perform actions in the context of the main program thread.
    // WARNING: Use with great care - see PLUGINS.html!
}

cString PluginRoboTVServer::Active(void) {
    // Return a message string if shutdown should be postponed
    return NULL;
}

time_t PluginRoboTVServer::WakeupTime(void) {
    // Return custom wakeup time for shutdown script
    return 0;
}

cMenuSetupPage* PluginRoboTVServer::SetupMenu(void) {
    // Return a setup menu in case the plugin supports one.
    return NULL;
}

bool PluginRoboTVServer::SetupParse(const char* Name, const char* Value) {
    // Parse your own setup parameters and store their values.
    return false;
}

bool PluginRoboTVServer::Service(const char* Id, void* Data) {
    // Handle custom service requests from other plugins
    return false;
}

const char** PluginRoboTVServer::SVDRPHelpPages(void) {
    static const char* HelpPages[] = {
        "LSCJ\n"
        "    List all channels activated for roboTV in JSON format.",
        "LSEJ channelUid | channelNumber\n"
        "    List upcoming EPG entries of the channel.",
        NULL
    };

    return HelpPages;
}

cString PluginRoboTVServer::SVDRPCommand(const char* Command, const char* Option, int& ReplyCode) {
    // Process SVDRP commands this plugin implements
    return m_channels.SVDRPCommand(Command, Option, ReplyCode);
}

VDRPLUGINCREATOR(PluginRoboTVServer); // Don't touch this!
