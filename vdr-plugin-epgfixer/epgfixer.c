/*
 * epgfixer.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <getopt.h>
#include <stdlib.h>
#include <vdr/plugin.h>
#include <vdr/i18n.h>
#include "blacklist.h"
#include "charset.h"
#include "epgclone.h"
#include "epghandler.h"
#include "regexp.h"
#include "setup_menu.h"

#if defined(APIVERSNUM) && APIVERSNUM < 10726
#error "VDR-1.7.26 API version or greater is required!"
#endif

#ifndef GITVERSION
#define GITVERSION ""
#endif

static const char VERSION[]        = "0.3.1" GITVERSION;
static const char DESCRIPTION[]    = trNOOP("Fix bugs in EPG");

class cPluginEpgfixer : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginEpgfixer(void);
  virtual ~cPluginEpgfixer();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual void MainThreadHook(void);
  virtual cString Active(void);
  virtual time_t WakeupTime(void);
  virtual const char *MainMenuEntry(void);
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginEpgfixer::cPluginEpgfixer(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginEpgfixer::~cPluginEpgfixer()
{
  // Clean up after yourself!
}

const char *cPluginEpgfixer::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginEpgfixer::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginEpgfixer::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  EpgfixerRegexps.SetConfigFile(AddDirectory(cPlugin::ConfigDirectory(PLUGIN_NAME_I18N), "regexp.conf")); // allowed only via main thread!);
  EpgfixerRegexps.ReloadConfigFile();
  EpgfixerCharSets.SetConfigFile(AddDirectory(cPlugin::ConfigDirectory(PLUGIN_NAME_I18N), "charset.conf")); // allowed only via main thread!);
  EpgfixerCharSets.ReloadConfigFile();
  EpgfixerBlacklists.SetConfigFile(AddDirectory(cPlugin::ConfigDirectory(PLUGIN_NAME_I18N), "blacklist.conf")); // allowed only via main thread!);
  EpgfixerBlacklists.ReloadConfigFile();
  EpgfixerEpgClones.SetConfigFile(AddDirectory(cPlugin::ConfigDirectory(PLUGIN_NAME_I18N), "epgclone.conf")); // allowed only via main thread!);
  EpgfixerEpgClones.ReloadConfigFile();
  return new cEpgfixerEpgHandler();
}

bool cPluginEpgfixer::Start(void)
{
  // Start any background activities the plugin shall perform.
  return true;
}

void cPluginEpgfixer::Stop(void)
{
  // Stop any background activities the plugin is performing.
}

void cPluginEpgfixer::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

const char *cPluginEpgfixer::MainMenuEntry(void)
{
  return NULL;
}

void cPluginEpgfixer::MainThreadHook(void)
{
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!
}

cString cPluginEpgfixer::Active(void)
{
  // Return a message string if shutdown should be postponed
  return NULL;
}

time_t cPluginEpgfixer::WakeupTime(void)
{
  // Return custom wakeup time for shutdown script
  return 0;
}

cOsdObject *cPluginEpgfixer::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return NULL;
}

cMenuSetupPage *cPluginEpgfixer::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cMenuSetupEpgfixer();
}

bool cPluginEpgfixer::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  cString m_ProcessedArgs;
  const char *pt;
  if (*m_ProcessedArgs && NULL != (pt = strstr(m_ProcessedArgs + 1, Name)) &&
      *(pt - 1) == ' ' && *(pt + strlen(Name)) == ' ') {
     dsyslog("Skipping configuration entry %s=%s (overridden in command line)", Name, Value);
     return true;
     }

  if (!strcasecmp(Name, "RemoveQuotesFromShortText"))                EpgfixerSetup.quotedshorttext = atoi(Value);
  else if (!strcasecmp(Name, "MoveDescriptionFromShortText"))        EpgfixerSetup.blankbeforedescription = atoi(Value);
  else if (!strcasecmp(Name, "RemoveRepeatedTitleFromShortText"))    EpgfixerSetup.repeatedtitle = atoi(Value);
  else if (!strcasecmp(Name, "RemoveDoubleQuotesFromShortText"))     EpgfixerSetup.doublequotedshorttext = atoi(Value);
  else if (!strcasecmp(Name, "RemoveUselessFormatting"))             EpgfixerSetup.removeformatting = atoi(Value);
  else if (!strcasecmp(Name, "MoveLongShortTextToDescription"))      EpgfixerSetup.longshorttext = atoi(Value);
  else if (!strcasecmp(Name, "PreventEqualShortTextAndDescription")) EpgfixerSetup.equalshorttextanddescription = atoi(Value);
  else if (!strcasecmp(Name, "ReplaceBackticksWithSingleQuotes"))    EpgfixerSetup.nobackticks = atoi(Value);
  else if (!strcasecmp(Name, "FixStreamComponentDescriptions"))      EpgfixerSetup.components = atoi(Value);
  else if (!strcasecmp(Name, "StripHTMLEntities"))                   EpgfixerSetup.striphtml = atoi(Value);
  else
     return false;

  return true;
}

bool cPluginEpgfixer::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginEpgfixer::SVDRPHelpPages(void)
{
  static const char *HelpPages[] = {
    "REL\n"
    "    Reload all configs.",
    "RLRE\n"
    "    Reload regexp.conf.",
    "RLCH\n"
    "    Reload charset.conf.",
    "RLBL\n"
    "    Reload blacklist.conf.",
    "RLEP\n"
    "    Reload epgclone.conf.",
    NULL
    };
  return HelpPages;
}

cString cPluginEpgfixer::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  if (strcasecmp(Command, "RLRE") == 0) {
     if (EpgfixerRegexps.ReloadConfigFile()) {
        return cString("Reloaded regexp.conf");
     } else {
        ReplyCode = 554; // Requested action failed
        return cString("Reloading regexp.conf failed");
     }
  }
  else if (strcasecmp(Command, "RLCH") == 0) {
     if (EpgfixerCharSets.ReloadConfigFile()) {
        return cString("Reloaded charset.conf");
     } else {
        ReplyCode = 554; // Requested action failed
        return cString("Reloading charset.conf failed");
     }
  }
  else if (strcasecmp(Command, "RLBL") == 0) {
     if (EpgfixerBlacklists.ReloadConfigFile()) {
        return cString("Reloaded blacklist.conf");
     } else {
        ReplyCode = 554; // Requested action failed
        return cString("Reloading blacklist.conf failed");
     }
  }
  else if (strcasecmp(Command, "RLEP") == 0) {
     if (EpgfixerEpgClones.ReloadConfigFile()) {
        return cString("Reloaded epgclone.conf");
     } else {
        ReplyCode = 554; // Requested action failed
        return cString("Reloading epgclone.conf failed");
     }
  }
  else if (strcasecmp(Command, "REL") == 0) {
     if (EpgfixerRegexps.ReloadConfigFile() &&
         EpgfixerCharSets.ReloadConfigFile() &&
         EpgfixerBlacklists.ReloadConfigFile() &&
         EpgfixerEpgClones.ReloadConfigFile()) {
        return cString("Reloaded all configs");
     } else {
        ReplyCode = 554; // Requested action failed
        return cString("Reloading all or some configs failed");
     }
  }
  return NULL;
}

VDRPLUGINCREATOR(cPluginEpgfixer); // Don't touch this!
