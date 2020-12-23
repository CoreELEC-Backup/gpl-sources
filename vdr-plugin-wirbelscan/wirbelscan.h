#ifndef __WIRBELSCAN_H__
#define __WIRBELSCAN_H__
/*
 * wirbelscan.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 */

#include <vdr/plugin.h>

class cPluginWirbelscan : public cPlugin {
private:
  // Add any member variables or functions you may need here.
  int servicetype(const char* id, bool init = false);
public:
  cPluginWirbelscan(void);
  virtual ~cPluginWirbelscan();
  virtual const char *Version(void);
  virtual const char *Description(void);
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual void MainThreadHook(void);
  virtual cString Active(void);
  virtual const char *MainMenuEntry(void);
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual void StoreSetup();
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };
extern cPluginWirbelscan* thisPlugin;

#endif
