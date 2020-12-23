/*
 * scanmenu.c: wirbelscanosd - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */


#include <vdr/menuitems.h>
#include <vdr/device.h>
#include <vdr/config.h>
#include "scanmenu.h"
#include "wirbelscan_services.h"
using namespace WIRBELSCAN_SERVICE;

cWirbelscanScanSetup _setup;
SListItem * cbuf = NULL;
SListItem * sbuf = NULL;
cPreAllocBuffer CountryBuffer;
cPreAllocBuffer SatBuffer;

#define MIN_CMDVERSION     1    // command api 0001
#define MIN_STATUSVERSION  1    // query status
#define MIN_SETUPVERSION   1    // get/put setup, GetSetup#XXXX/SetSetup#XXXX
#define MIN_COUNTRYVERSION 1    // get list of country IDs and Names
#define MIN_SATVERSION     1    // get list of sat IDs and Names
#define MIN_USERVERSION    1    // scan user defined transponder

int smode;
int channelcount0;
int channelcount1;
int view;
int osdstatus;
int singlescan;
int progress;
time_t start;

cScanOSD * menu = NULL;
cOsdItem * SM  = NULL;
cOsdItem * TM  = NULL;
cOsdItem * CH  = NULL;
cOsdItem * TV1 = NULL;
cOsdItem * TV2 = NULL;
cOsdItem * TV3 = NULL;
cOsdItem * TV4 = NULL;
cOsdItem * TV5 = NULL;
cOsdItem * TP = NULL;
cOsdItem * ProgressBar = NULL;
cTickTimer * timer = NULL;

//---- debugging -------------------------------------------------------------
//#define OSD_DBG 1

#ifdef OSD_DBG
int it;
#define _debug(format, args...) printf (format, ## args)
#else
#define _debug(format, args...)
#endif

//---- macro definitions. ----------------------------------------------------
#define SETSCAN  0
#define SCANNING 1
#define SCANDONE 2
#define CHECKVERSION(a,b,c) p=strchr((char *) info->a,'#') + 1; sscanf(p,"%d ",&version); if (version < b) c = true;
#define CHECKLIMITS(a,v,_min,_max,_def) a=v; if ((a<_min) || (a>_max)) a=_def;
#define freeAndNull(p)   if(p) { free(p);   p=NULL; }
#define deleteAndNull(p) if(p) { delete(p); p=NULL; }

//---- cMenuEditSListItem ----------------------------------------------------

class cMenuEditSListItem : public cMenuEditIntItem {
private:
  SListItem * items;
protected:
  virtual void Set(void) { SetValue(items[*value].full_name); };
public:
  cMenuEditSListItem(const char *Name, int *Value, int NumItems, SListItem * Items)
   : cMenuEditIntItem(Name, Value, 0, NumItems-1) { items = Items; Set(); };
};

// --- cMenuEditSubItem ------------------------------------------------------
// like cMenuEditStraItem, but only allowed values in its range.

class cMenuEditSubItem : public cMenuEditIntItem {
private:
  const char * const *strings;
  int maxval, oldval;
  const int * allowed;
protected:
  virtual void Set(void);
public:
  cMenuEditSubItem(const char *Name, int *Value, int NumStrings, const char * const *Strings, const int *Allowed);
  };

cMenuEditSubItem::cMenuEditSubItem(const char *Name, int *Value, int NumStrings, const char * const *Strings, const int *Allowed)
:cMenuEditIntItem(Name, Value, 0, maxval = (NumStrings - 1))
{
  strings = Strings;
  allowed = Allowed;
  oldval  = *Value - 1;
  while (! allowed[*value]) {
      *value = *value + 1;
      if (*value > maxval)
          *value = *value = 0;
      oldval = *value - 1;
      }
  Set();
}

void cMenuEditSubItem::Set(void)
{
  bool upward = ((*value - oldval) == 1 ) || ((oldval == maxval) && ! *value);
  while (! allowed[*value]) {
     if (upward) {
         if (*value < maxval) *value = *value + 1;
         else *value = 0;
         }
     else {
         if (*value > 0) *value = *value - 1;
         else *value = maxval;
         }
     }
  SetValue(strings[(oldval = *value)]);
}


// --- cScanOSD --------------------------------------------------------------

cScanOSD::cScanOSD(void) : cOsdMenu(tr("Channel Scan"), 14, 18)
{
  menu = this;
  int version=0;
  char *p;
  bool unsupported = false;
  int cardnr = 0;
  cDevice *device;

  memset(&systems,0,sizeof(systems));
  osdstatus = -1;
  smode = singlescan = 0;
  Tp_unsupported = false;

  while ((device = cDevice::GetDevice(cardnr++))) {
     if (device->ProvidesSource(cSource::stTerr))  systems[0] = 1;
     if (device->ProvidesSource(cSource::stCable)) systems[1] = 1;
     if (device->ProvidesSource(cSource::stSat))   systems[2] = 1;
     #if VDRVERSNUM > 10713
     if (device->ProvidesSource(cSource::stAtsc))  systems[5] = 1;
     #endif
     }

  if (cPluginManager::GetPlugin("pvrinput")) {
     systems[3] = 1;
     systems[4] = 1;
     }

  #ifdef OSD_DBG
  memset(&systems,1,sizeof(systems));
  #endif

  if (! (wirbelscan = cPluginManager::GetPlugin("wirbelscan"))) {
     Add(new cOsdItem(tr("wirbelscan was not found - pls install.")));
     return;
     }

  info = new cWirbelscanInfo;
  asprintf(&p, "%s%s", SPlugin, SInfo);
  if (! wirbelscan->Service("wirbelscan_GetVersion", info)) {
     free(p);
     Add(new cOsdItem(tr("Your wirbelscan version doesnt"    )));
     Add(new cOsdItem(tr("support services - Please upgrade.")));
     return;
     }
  free(p);

  CHECKVERSION(CommandVersion,MIN_CMDVERSION,     unsupported);
  CHECKVERSION(StatusVersion, MIN_STATUSVERSION,  unsupported);
  CHECKVERSION(SetupVersion,  MIN_SETUPVERSION,   unsupported);
  CHECKVERSION(CountryVersion,MIN_COUNTRYVERSION, unsupported);
  CHECKVERSION(SatVersion,    MIN_SATVERSION,     unsupported);
  CHECKVERSION(UserVersion,   MIN_USERVERSION,    Tp_unsupported);

  if (unsupported) {
     Add(new cOsdItem(tr("Your wirbelscan version is")));
     Add(new cOsdItem(tr("too old - Please upgrade." )));
     return;
     }

  asprintf(&p, "%sGet%s", SPlugin, SSetup);
  wirbelscan->Service(p, &_setup);
  free(p);

  if (!Tp_unsupported) {
     asprintf(&p, "%sGet%s", SPlugin, SUser);
     wirbelscan->Service(p, &userdata);
     free(p);
     }

  CHECKLIMITS(sat,      _setup.SatId,           0 ,0xFFFF,0);
  CHECKLIMITS(country,  _setup.CountryId       ,0 ,0xFFFF,0);
  CHECKLIMITS(source,   _setup.DVB_Type        ,0 ,5     ,0);
  CHECKLIMITS(terrinv,  _setup.DVBT_Inversion  ,0 ,1     ,0);
  CHECKLIMITS(cableinv, _setup.DVBC_Inversion  ,0 ,1     ,0);
  CHECKLIMITS(srate,    _setup.DVBC_Symbolrate ,0 ,16    ,0);
  CHECKLIMITS(qam,      _setup.DVBC_QAM        ,0 ,4     ,0);
  CHECKLIMITS(atsc,     _setup.ATSC_type       ,0 ,1     ,0);
  view = -1;

  CountryBuffer.size = 0;
  CountryBuffer.count = 0;
  CountryBuffer.buffer = NULL;
  asprintf(&p, "%sGet%s", SPlugin, SCountry);      
  wirbelscan->Service(p, &CountryBuffer);                             // query buffer size.
  cbuf = (SListItem*) malloc(CountryBuffer.size * sizeof(SListItem)); // now, allocate memory.
  CountryBuffer.buffer = cbuf;                                        // assign buffer
  wirbelscan->Service(p, &CountryBuffer);                             // fill buffer with values.
  free(p);

  SatBuffer.size = 0;
  SatBuffer.count = 0;
  SatBuffer.buffer = NULL;
  asprintf(&p, "%sGet%s", SPlugin, SSat);     
  wirbelscan->Service(p, &SatBuffer);                                 // query buffer size.
  cbuf = (SListItem*) malloc(SatBuffer.size * sizeof(SListItem));     // now, allocate memory.
  SatBuffer.buffer = cbuf;                                            // assign buffer
  wirbelscan->Service(p, &SatBuffer);                                 // fill buffer with values.
  free(p);

  osdstatus = SETSCAN; // before scan.
  SetHelp(tr("Scan"), NULL, NULL, tr("Store"));

  GetStatus();

  if ((status.status == StatusScanning) || timer) {
     view = source;
     osdstatus = SCANNING;
     SetToScanning();
     }
  else {
     osdstatus = SETSCAN;
     SetBySource((view = source),1);
     }
}

cScanOSD::~cScanOSD()
{
  menu = NULL;
  freeAndNull(cbuf);
  freeAndNull(sbuf);
}

eOSState cScanOSD::ProcessKey(eKeys Key)
{
   eOSState state = cOsdMenu::ProcessKey(Key);
   int direction = 0;
#if APIVERSNUM >= 20301
   LOCK_CHANNELS_READ;
#endif
   switch (Key) {
      case kLeft:    direction = -1;
                     break;

      case kRight:   direction = 1;
                     break;

      case kRed:     switch(osdstatus) {
                        _debug("kRed %d\n",osdstatus);
                        case SCANDONE:
                        case SETSCAN:
                          osdstatus = SCANNING;
                          start = time(NULL);
#if APIVERSNUM < 20301
                          channelcount0 = Channels.Count();
#else
                          channelcount0 = Channels->Count();
#endif
                          SetHelp(tr("Stop"), NULL, NULL, NULL);
                          TransferSetup();
                          PutCommand(CmdStartScan);
                          SetToScanning();
                          break;

                        case SCANNING:
                          osdstatus = SCANDONE;
                          _debug("kRed, stoooop\n");
                          SetHelp(tr("Start"), NULL, NULL, NULL);
                          PutCommand(CmdStopScan);
                          break;
                         
                        default:
                          _debug("WARN: unknown osdstatus %d\n", osdstatus);
                        }
                     break;

      case kBlue:    if (osdstatus > SETSCAN) break;
                     TransferSetup();
                     PutCommand(CmdStore);
                     return osContinue;
                     break;
      default:;
      }
   if (state == osUnknown) {
      switch (Key) {
         case kBack:    return osEnd;
         case kNone:    break;

         default:       break;
      }
      state = osContinue;
   }
   if ((view != source) || (smode != singlescan)) {
      //smode = singlescan;
      SetBySource((view = source),direction);
      }
   return state;
}

static const char * DVB_Types[] = {"DVB-T - Terrestrisch","DVB-C - Cable","DVB-S/S2 - Satellite", "analog TV - pvrinput","analog Radio - pvrinput","ATSC (North America)"};
static const char * INV[]       = {"AUTO (fallback: OFF)", "AUTO (fallback: ON)"};
static const char * SR[]        = {"AUTO", "6900", "6875", "6111", "6250", "6790", "6811", "5900", "5000", "3450", "4000", "6950", "7000", "6952", "5156", "5483", "ALL" };
static const char * QAM[]       = {"AUTO", "QAM-64", "QAM-128", "QAM-256", "ALL"};
static const char * ATSC[]      = {"VSB (aerial DTV)", "QAM (cable DTV)"};
static const char * MODE[]      = {"AUTO", "single transponder"};
static const char * FEC[]       = {"OFF", "1/2", "2/3", "3/4", "4/5", "5/6", "6/7", "7/8", "8/9", "AUTO", "3/5", "9/10"};
static const char * BW[]        = {"8MHz", "7MHz", "6MHz", "5MHz" };
static const char * GUARD[]     = {"1/32", "1/16", "1/8", "1/4" };
static const char * HIERARCHY[] = {"OFF", "alpha = 1", "alpha = 2", "alpha = 4"};
static const char * TRANSM[]    = {"2k", "8k", "4k"};
static const char * SATSYS[]    = {"DVB-S", "DVB-S2"};
static const char * POL[]       = {"H", "V", "L", "R"};
static const char * MOD[]       = {"QPSK", "QAM16", "QAM32","QAM64","QAM128","QAM256","QAM-AUTO","VSB8","VSB16","PSK8"};
static const char * RO[]        = {"0.35", "0.25", "0.20"};

static const int    FEC_S[]     = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
static const int    FEC_T[]     = { 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0 }; 
static const int    BW_T[]      = { 1, 1, 1, 0  }; 
static const int    TRANSM_T[]  = { 1, 1, 0  };
static const int    MOD_A[]     = { 0, 0, 0, 1, 0, 1, 1, 1, 1, 0 };
static const int    MOD_C[]     = { 0, 0, 0, 1, 1, 1, 0, 0, 0, 0 };
static const int    MOD_S[]     = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
static const int    MOD_T[]     = { 1, 1, 0, 1, 0, 0, 0, 0, 0, 0 };


void cScanOSD::SetBySource(int View, int direction)
{ 
  bool supported = systems[View];
  int type = 999;

  //_debug("%d  view %s(%s), %s\n", ++it, DVB_Types[View], direction>0?"UP":"DOWN", supported?"show":"skip over");
  if (! supported) {
     if (direction > 0) {
        if (View < 5)
            view = source = ++ View;
        else
            view = source = 0;
        }
     else {
        if (View > 0)
            view = source = -- View;
        else
            view = source = 5;
        }
     SetBySource(view, direction);
     return;
     }

  Clear();
  
  if (singlescan) {
      cUserTransponder * t = new cUserTransponder(&userdata[0]);
      frequency    = t->Frequency();
      modulation   = t->Modulation();
      fec_hp       = t->FecHP();
      fec_lp       = t->FecLP();
      bandwidth    = t->Bandwidth();
      guard        = t->Guard();
      hierarchy    = t->Hierarchy();
      transmission = t->Transmission();
      useNit       = t->UseNit();
      symbolrate   = t->Symbolrate();
      satsystem    = t->Satsystem();
      polarisation = t->Polarisation();
      rolloff      = t->Rolloff();
      type         = t->Type();
      delete(t);
      }

  switch (View) {
     case 0: //DVB-T = 0
        Add(new cMenuEditStraItem(tr("Source Type"), &source, 6, DVB_Types));
        Add(new cMenuEditSListItem(tr("Country") ,   &country, CountryBuffer.count, CountryBuffer.buffer));
        Add(new cMenuEditStraItem(tr("Inversion"),   &terrinv, 2, INV));
        if (Tp_unsupported) {
            singlescan = 0;
            }
        else {
            Add(SM = new cMenuEditStraItem(tr("Scan mode"),   &singlescan, 2, MODE));
            if (singlescan) {
                if (type != View) {
                    frequency = 658000;
                    modulation = 1;
                    fec_hp = 2;
                    fec_lp = 0;
                    bandwidth = 0;
                    guard = 2;
                    hierarchy = 0;
                    transmission = 1;
                    }
                Add(new cMenuEditIntItem(tr("Frequency"), &frequency, 177500, 858000));
                Add(new cMenuEditSubItem(tr("Modulation"), &modulation, 4, MOD, MOD_T));
                Add(new cMenuEditSubItem(tr("FEC HP"), &fec_hp, 8, FEC, FEC_T));
                Add(new cMenuEditSubItem(tr("FEC LP"), &fec_lp, 8, FEC, FEC_T));
                Add(new cMenuEditSubItem(tr("Bandwidth"), &bandwidth, 3, BW, BW_T));
                Add(new cMenuEditStraItem(tr("Guard Interval"), &guard, 4, GUARD));
                Add(new cMenuEditStraItem(tr("Hierarchy"), &hierarchy, 4, HIERARCHY));
                Add(new cMenuEditSubItem(tr("Trans. Mode"), &transmission, 3, TRANSM, TRANSM_T));
                Add(new cMenuEditBoolItem(tr("use NIT"), &useNit));
                }
            }
        break;
     case 1: //DVB-C = 1
        Add(new cMenuEditStraItem(tr("Source Type"), &source, 6, DVB_Types));
        Add(new cMenuEditSListItem(tr("Country") ,   &country, CountryBuffer.count, CountryBuffer.buffer));
        Add(new cMenuEditStraItem(tr("Inversion"),   &cableinv, 2, INV));       
        if (Tp_unsupported) {
            Add(new cMenuEditStraItem(tr("Modulation"),  &qam, 5, QAM));
            Add(new cMenuEditStraItem(tr("Symbol Rate"), &srate, 16, SR));
            singlescan = 0;
            }
        else {
            Add(SM = new cMenuEditStraItem(tr("Scan mode"),   &singlescan, 2, MODE));
            if (singlescan) {
                if (type != View) {
                    frequency = 346000;
                    modulation = 5;
                    symbolrate = 6900;
                    }
                Add(new cMenuEditIntItem (tr("Frequency"),  &frequency, 73000, 858000));
                Add(new cMenuEditIntItem (tr("Symbol Rate"), &symbolrate, 4000, 7000));
                Add(new cMenuEditSubItem(tr("Modulation"), &modulation, 6, MOD, MOD_C));
                Add(new cMenuEditBoolItem(tr("use NIT"), &useNit));
                }
            else {
               Add(new cMenuEditStraItem(tr("Modulation"),  &qam, 5, QAM));
               Add(new cMenuEditStraItem(tr("Symbol Rate"), &srate, 16, SR));
               }
            }
        break;
     case 2: //DVB-S/S2 = 2
        Add(new cMenuEditStraItem(tr("Source Type"), &source, 6, DVB_Types));
        Add(new cMenuEditSListItem(tr("Satellite") , &sat, SatBuffer.count, SatBuffer.buffer));
        if (Tp_unsupported) {
            singlescan = 0;
            }
        else {
            Add(SM = new cMenuEditStraItem(tr("Scan mode"),   &singlescan, 2, MODE));
            if (singlescan) {
                if (type != View) {
                    satsystem = 0;
                    frequency = 11836;
                    modulation = 0;
                    polarisation = 0;
                    fec_hp = 3;
                    symbolrate = 27500;
                    rolloff = 0;
                    }
                Add(new cMenuEditStraItem(tr("System"), &satsystem, 2, SATSYS));
                Add(new cMenuEditIntItem (tr("Frequency"),  &frequency, 2000, 15000));
                Add(new cMenuEditStraItem(tr("Polarisation"), &polarisation, 4, POL));
                Add(new cMenuEditSubItem(tr("Modulation"), &modulation, 10, MOD, MOD_S));
                Add(new cMenuEditIntItem (tr("Symbol Rate"), &symbolrate, 2000, 65000));
                Add(new cMenuEditSubItem(tr("FEC"), &fec_hp, 12, FEC, FEC_S));
                Add(new cMenuEditStraItem(tr("Rolloff"), &rolloff, 3, RO));
                Add(new cMenuEditBoolItem(tr("use NIT"), &useNit));
                }
            };
        break;
     case 3: //PVRINPUT = 3
        Add(new cMenuEditStraItem(tr("Source Type"), &source, 6, DVB_Types));
        Add(new cMenuEditSListItem(tr("Country") ,   &country, CountryBuffer.count, CountryBuffer.buffer));
        break;
     case 4: //PVRINPUT(FM Radio) = 4
        Add(new cMenuEditStraItem(tr("Source Type"), &source, 6, DVB_Types));
        break;
     case 5: //ATSC = 5
        Add(new cMenuEditStraItem(tr("Source Type"), &source, 6, DVB_Types));
        Add(new cMenuEditStraItem(tr("Type"),        &atsc, 2, ATSC));
        if (Tp_unsupported) {
            singlescan = 0;
            Add(new cMenuEditStraItem(tr("Scan mode"),   &singlescan, 1, MODE));
            }
        else {
            Add(SM = new cMenuEditStraItem(tr("Scan mode"),   &singlescan, 2, MODE));
            if (singlescan) {
                if (type != View) {
                    frequency = 533000;
                    modulation = 5;
                    }
                Add(new cMenuEditIntItem (tr("Frequency"),  &frequency, 73000, 858000));
                Add(new cMenuEditSubItem(tr("Modulation"), &modulation, 10, MOD, MOD_A));
                Add(new cMenuEditBoolItem(tr("use NIT"), &useNit));
                }
            }
        break;
     default:
        Add(new cMenuEditStraItem(tr("Source Type"), &source, 6, DVB_Types));
        Add(new cOsdItem(tr("Unknown Source ???"), osUnknown, false));
     }
  if (smode != singlescan) {
      if (SM) SetCurrent(SM);
      smode = singlescan;
      }
  Display();
}

void cScanOSD::SetToScanning(void)
{
  Clear();

  SetTitle(tr("Scan in progress"));
  Add(TM  = new cOsdItem(tr("Running: 0h 00m 00s"), osUnknown, false));
  Add(CH  = new cOsdItem(tr("New channels: 0"), osUnknown, false));
  Add(TV1 = new cOsdItem(" ", osUnknown, false));
  Add(TV2 = new cOsdItem(" ", osUnknown, false));
  Add(TV3 = new cOsdItem(" ", osUnknown, false));
  Add(TV4 = new cOsdItem(" ", osUnknown, false));
  Add(TV5 = new cOsdItem(" ", osUnknown, false));
  Add(new cOsdItem("  ", osUnknown, false));
  Add(TP = new cOsdItem("  ", osUnknown, false));
  Add(new cOsdItem("  ", osUnknown, false));
  Add(ProgressBar = new cOsdItem("[]", osUnknown, true));
  Display();

  if (! timer)
     timer = new cTickTimer();

  timer->Stop(false);
  timer->Start();// start polling thread

}

void cScanOSD::PutCommand(WIRBELSCAN_SERVICE::s_cmd command)
{
  cWirbelscanCmd cmd;
  char * request;
  asprintf(&request, "%s%s", SPlugin, SCommand);
 
  cmd.cmd = command;
  wirbelscan->Service(request, &cmd);
  free(request);
}

void cScanOSD::TransferSetup (void)
{
  char * s;

  // _setup.verbosity = ;  // 0 (errors only) .. 5 (extended debug); default = 3 (messages)
  // _setup.logFile = ;    // 0 = off, 1 = stdout, 2 = syslog
  _setup.DVB_Type = source;
  _setup.DVBT_Inversion = terrinv;
  _setup.DVBC_Inversion = cableinv;
  _setup.DVBC_Symbolrate = srate;
  _setup.DVBC_QAM = qam;
  _setup.CountryId = country;
  _setup.SatId = sat;
  //_setup.scanflags;      // bitwise flag of wanted channels: TV = (1 << 0), RADIO = (1 << 1), FTA = (1 << 2), SCRAMBLED = (1 << 4), HDTV = (1 << 5)
  _setup.ATSC_type = atsc;

  if (singlescan && !Tp_unsupported) {

     cUserTransponder * Transponder;
     _setup.DVB_Type = 999; // TRANSPONDER
     switch (source) {
         case 0: //DVB-T = 0
            Transponder = new cUserTransponder(country, frequency, modulation, fec_hp, fec_lp, bandwidth,
                                              0, hierarchy, guard, transmission, terrinv, useNit);
            break;
         case 1: //DVB-C = 1
            Transponder = new cUserTransponder(country, frequency, symbolrate, modulation, cableinv, useNit);
            break;
         case 2: //DVB-S/S2 = 2
            Transponder = new cUserTransponder(sat, satsystem, frequency, polarisation, symbolrate,
                                               modulation, fec_hp, 0, 0, rolloff, useNit);
            break;
         case 5: //ATSC = 5
            Transponder = new cUserTransponder(country, frequency, modulation, useNit);
            break;
         default:
            Transponder = NULL;
         }

     if (Transponder) {
         userdata[0] = *(Transponder->Data() + 0);
         userdata[1] = *(Transponder->Data() + 1);
         userdata[2] = *(Transponder->Data() + 2);
         asprintf(&s, "%sSet%s", SPlugin, SUser);
         wirbelscan->Service(s, &userdata);
         free(s);         
         }

     }

  asprintf(&s, "%sSet%s", SPlugin, SSetup);
  wirbelscan->Service(s, &_setup);
  free(s);
}

void cScanOSD::GetStatus(void)
{
  char * s;

  asprintf(&s, "%sGet%s", SPlugin, SStatus);
  wirbelscan->Service(s, &status);
  free(s);

  transponder = cString::sprintf("%s",status.transponder);
  channelcount1 = status.newChannels;
  progress = status.progress;
}

void cScanOSD::Update(void)
{
  #define BARWIDTH 33

  char buf[256];
  cString sbuf;
  int i = 0;
  time_t running = time(NULL) - start;

  int hh = running/3600; if (running >= 3600) running = running - hh * 3600;
  int mm = running/60;   if (running >= 60)   running = running - mm * 60;
  int ss = running;

  GetStatus();

  sbuf = cString::sprintf("%s %.2dh %.2dm %.2ds", tr("Running:"), hh, mm, ss);
  if (TM) TM->SetText(*sbuf);

  if (status.status == StatusStopped) {
     if (timer) timer->Stop();
     osdstatus = SCANDONE;
     SetHelp(tr("Start"), NULL, NULL, NULL);
     SetTitle(tr("Scan finished"));
     Add(new cOsdItem("  "));
     Add(new cOsdItem("scan finished."));
     Display();
     status.progress = 100;
     delete(timer);
     }

  buf[i++] = '[';
  while (i < (status.progress*BARWIDTH)/100)
     buf[i++] = '|';
  while (i < BARWIDTH)
     buf[i++] = ' ';
  buf[i++] = ']';
  buf[i++] = 0;

  if (TP) TP->SetText(*transponder);
  sbuf = cString::sprintf("%s (%d%% transponders from scan list + %u queued)", buf, status.progress, status.nextTransponders);
  ProgressBar->SetText(*sbuf);

#if APIVERSNUM < 20301
  if (TV1 && TV2 && TV3 && TV4 && TV5) {
     switch (Channels.Count() - channelcount0) {
         default:;
         case 5: TV5->SetText(Channels.GetByNumber(Channels.Count()-4)->Name());
         case 4: TV4->SetText(Channels.GetByNumber(Channels.Count()-3)->Name());
         case 3: TV3->SetText(Channels.GetByNumber(Channels.Count()-2)->Name());
         case 2: TV2->SetText(Channels.GetByNumber(Channels.Count()-1)->Name());
         case 1: TV1->SetText(Channels.GetByNumber(Channels.Count()-0)->Name());
         case 0:; 
         }
  }
  sbuf = cString::sprintf("%s%d", tr("New channels: "), Channels.Count() - channelcount0);
#else
  LOCK_CHANNELS_READ;
  if (TV1 && TV2 && TV3 && TV4 && TV5) {
     switch (Channels->Count() - channelcount0) {
         default:;
         case 5: TV5->SetText(Channels->GetByNumber(Channels->Count()-4)->Name());
         case 4: TV4->SetText(Channels->GetByNumber(Channels->Count()-3)->Name());
         case 3: TV3->SetText(Channels->GetByNumber(Channels->Count()-2)->Name());
         case 2: TV2->SetText(Channels->GetByNumber(Channels->Count()-1)->Name());
         case 1: TV1->SetText(Channels->GetByNumber(Channels->Count()-0)->Name());
         case 0:; 
         }
  }
  sbuf = cString::sprintf("%s%d", tr("New channels: "), Channels->Count() - channelcount0);
#endif
  if (CH) CH->SetText(*sbuf);

  Display();
}


cTickTimer::cTickTimer(void)
{
  timer = this;
  stop = false;
}

cTickTimer::~cTickTimer()
{ 
  timer = NULL;
  Stop();
}

void cTickTimer::Action(void)
{
  _debug("timer start.\n");
   while (Running() && ! stop) {

      cCondWait::SleepMs(1000);
      if (!menu) break;
      menu->Update();
      }
  _debug("timer stop.\n");
  Cancel(0);
}


