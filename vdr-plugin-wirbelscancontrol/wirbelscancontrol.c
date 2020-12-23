/*
 * wirbelscancontrol.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include "scanmenu.h"

static const char *VERSION        = "0.0.2";
static const char *DESCRIPTION    = "scan channels using wirbelscan";
static const char *MAINMENUENTRY  = "Channel Scan";

class cPluginWirbelscancontrol : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginWirbelscancontrol(void) { };
  virtual ~cPluginWirbelscancontrol() { };
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void) { return NULL; };
  virtual bool ProcessArgs(int argc, char *argv[]) { return true; };
  virtual bool Initialize(void) { return true; };
  virtual bool Start(void) { return true; };
  virtual void Stop(void) { };
  virtual void Housekeeping(void) { };
  virtual void MainThreadHook(void) { };
  virtual cString Active(void) { return NULL; };
  virtual time_t WakeupTime(void) {  return 0; };
  virtual const char *MainMenuEntry(void) { return tr(MAINMENUENTRY); }
  virtual cOsdObject *MainMenuAction(void) { return new cScanOSD(); };
  virtual cMenuSetupPage *SetupMenu(void) { return NULL; };
  virtual bool SetupParse(const char *Name, const char *Value) { return false; };
  virtual bool Service(const char *Id, void *Data = NULL) { return false; };
  virtual const char **SVDRPHelpPages(void) { return NULL; };
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode) { return NULL; };
  };


VDRPLUGINCREATOR(cPluginWirbelscancontrol); // Don't touch this!
