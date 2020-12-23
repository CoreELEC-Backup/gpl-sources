/*
 * scanmenu.h: wirbelscanosd - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef _W_MENU_H_
#define _W_MENU_H_

#include <vdr/plugin.h>
#include <vdr/osdbase.h>
#include "wirbelscan_services.h"

class cScanOSD : public cOsdMenu {
private:
   cPlugin * wirbelscan;
   WIRBELSCAN_SERVICE::cWirbelscanInfo * info;
   WIRBELSCAN_SERVICE::cWirbelscanStatus status;
   // user scan
   int frequency;
   int modulation;
   int fec_hp;
   int fec_lp;
   int bandwidth;
   int guard;
   int hierarchy;
   int transmission;
   int useNit;
   int symbolrate;
   int satsystem;
   int polarisation;
   int rolloff;
   uint32_t userdata[3];
   // auto scan
   int systems[6];
   int sat;
   int country;
   int source;
   int terrinv;
   int qam;
   int cableinv;
   int atsc;
   int srate;
   bool Tp_unsupported;

   cString transponder;
public:
   cScanOSD(void);
   virtual ~cScanOSD();
   eOSState ProcessKey(eKeys Key);
   void SetBySource(int View, int direction);
   void SetToScanning(void);
   void PutCommand(WIRBELSCAN_SERVICE::s_cmd command);
   void TransferSetup(void);
   void GetStatus(void);
   void Update(void);
};

class cTickTimer : public cThread {
private:
   int stop;
public:
   cTickTimer(void);
   ~cTickTimer();
   void Action();
   void Stop(bool value = true) { stop = value; };
};

#endif
