/*
 * menusetup.h: wirbelscan - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 */

#ifndef __WIRBELSCAN_MENUSETUP_H_
#define __WIRBELSCAN_MENUSETUP_H_

#include <vdr/menuitems.h>
#include <vdr/osdbase.h>
#include <vdr/thread.h>
#include "scanner.h"
#include "common.h"
 
extern const char* extVer;
extern const char* extDesc;
extern cScanner* Scanner;
extern std::string deviceName;
extern std::string lTransponder;
extern int lProgress;
extern int lStrength;
extern int channelcount;

void stopScanners(void);
bool DoScan (int DVB_Type);
void DoStop (void);

class cMenuScanning : public cMenuSetupPage {
private:
  bool needs_update;
  bool log_busy;
  int transponder;
  int transponders;

protected:
  virtual bool StartScan(void);
  virtual bool StopScan(void);
  virtual void AddCategory(const char * category);

public:
  cMenuScanning(void);
  ~cMenuScanning(void);
  virtual void Store(void);
  virtual eOSState ProcessKey(eKeys Key);
  void SetStatus(int status);
  void SetProgress(const int progress);
  void SetCounters(int curr_tp, int all_tp);
  void SetTransponder(const TChannel* transponder); 
  void SetStr(uint strength, bool locked);
  void SetChan(int count);
  void SetDeviceInfo(std::string Info, bool update = true);
  void SetChanAdd(uint32_t flags);
  void AddLogMsg(const char* Msg);
};
extern cMenuScanning* MenuScanning;

#endif
