/*
 * wirbelscan.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 */

#include <getopt.h>
#include <vdr/plugin.h>
#include <vdr/i18n.h>
#include "wirbelscan.h"
#include "wirbelscan_services.h"
#include "menusetup.h"
#include "countries.h"
#include "satellites.h"
extern TChannels NewChannels;

static const char *VERSION        = "2018.11.04";
static const char *DESCRIPTION    = "DVB channel scan for VDR";
static const char *MAINMENUENTRY  = NULL; /* main menu -> use wirbelscancontrol plugin */
const char* extVer  = VERSION;
const char* extDesc = DESCRIPTION;
cPluginWirbelscan* thisPlugin;

const char* cPluginWirbelscan::Version(void) {
  return VERSION;
}

const char* cPluginWirbelscan::Description(void) {
  return DESCRIPTION; //return I18nTranslate(DESCRIPTION, "vdr-wirbelscan");
}

const char* cPluginWirbelscan::MainMenuEntry(void) {
  return MAINMENUENTRY;
}

// constructor
cPluginWirbelscan::cPluginWirbelscan(void) {
  thisPlugin = this;
  servicetype("", true);
}

// destructor
cPluginWirbelscan::~cPluginWirbelscan() {
}

// Return a string that describes all known command line options.
const char *cPluginWirbelscan::CommandLineHelp(void) {
  return NULL;
}

// Implement command line argument processing here if applicable.
bool cPluginWirbelscan::ProcessArgs(int argc, char* argv[]) {
  return true;
}

// Initialize any background activities the plugin shall perform.
bool cPluginWirbelscan::Initialize(void) {
  return true;
}

// Start any background activities the plugin shall perform.
bool cPluginWirbelscan::Start(void) {
  return true;
}

// Stop any background activities the plugin shall perform.
void cPluginWirbelscan::Stop(void) {
  stopScanners();
}

// Perform any cleanup or other regular tasks.
void cPluginWirbelscan::Housekeeping(void) {
}

// Perform actions in the context of the main program thread.
void cPluginWirbelscan::MainThreadHook(void) {
}

// Return a message string if shutdown should be postponed
cString cPluginWirbelscan::Active(void) {
  return NULL;
}

// Perform the action when selected from the main VDR menu.
cOsdObject *cPluginWirbelscan::MainMenuAction(void) { 
  return NULL;
}

// Return a setup menu in case the plugin supports one.
cMenuSetupPage *cPluginWirbelscan::SetupMenu(void) {
  return new cMenuScanning();
}

// read back plugins settings.
bool cPluginWirbelscan::SetupParse(const char* Name, const char* Value) {
  std::string name = Name;
  if      (name == "verbosity")        wSetup.verbosity=atoi(Value);
  else if (name == "logFile")          wSetup.logFile=atoi(Value);
  else if (name == "DVB_Type")         wSetup.DVB_Type=atoi(Value);
  else if (name == "DVBT_Inversion")   wSetup.DVBT_Inversion=atoi(Value);
  else if (name == "DVBC_Inversion")   wSetup.DVBC_Inversion=atoi(Value);
  else if (name == "DVBC_Symbolrate")  wSetup.DVBC_Symbolrate=atoi(Value);
  else if (name == "DVBC_Network_PID") wSetup.DVBC_Network_PID=atoi(Value);
  else if (name == "DVBC_QAM")         wSetup.DVBC_QAM=atoi(Value);
  else if (name == "CountryIndex")     wSetup.CountryIndex=atoi(Value);
  else if (name == "SatIndex")         wSetup.SatIndex=atoi(Value);
  else if (name == "enable_s2")        wSetup.enable_s2=atoi(Value);
  else if (name == "ATSC_type")        wSetup.ATSC_type=atoi(Value);
  else if (name == "scanflags")        wSetup.scanflags=atoi(Value);
  else if (name == "user0")            wSetup.user[0]=atol(Value);
  else if (name == "user1")            wSetup.user[1]=atol(Value);
  else if (name == "user2")            wSetup.user[2]=atol(Value);
  else if (name == "ri")               wSetup.scan_remove_invalid=atoi(Value);
  else if (name == "ue")               wSetup.scan_update_existing=atoi(Value);
  else if (name == "an")               wSetup.scan_append_new=atoi(Value);
  else return false;                                              
  return true;
}

using namespace WIRBELSCAN_SERVICE;

void cPluginWirbelscan::StoreSetup(void) {
  if (wSetup.DVB_Type > 5)
     wSetup.DVB_Type = (int) G(wSetup.user[1],3,29);

  SetupStore("verbosity",       wSetup.verbosity);
  SetupStore("logFile",         wSetup.logFile);
  SetupStore("DVB_Type",        wSetup.DVB_Type);
  SetupStore("DVBT_Inversion",  wSetup.DVBT_Inversion);
  SetupStore("DVBC_Inversion",  wSetup.DVBC_Inversion);
  SetupStore("DVBC_Symbolrate", wSetup.DVBC_Symbolrate);
  SetupStore("DVBC_QAM",        wSetup.DVBC_QAM);
  SetupStore("DVBC_Network_PID",wSetup.DVBC_Network_PID);
  SetupStore("CountryIndex",    wSetup.CountryIndex);
  SetupStore("SatIndex",        wSetup.SatIndex);
  SetupStore("enable_s2",       wSetup.enable_s2);
  SetupStore("ATSC_type",       wSetup.ATSC_type);
  SetupStore("scanflags",       wSetup.scanflags);
  SetupStore("user0",           wSetup.user[0]);
  SetupStore("user1",           wSetup.user[1]);
  SetupStore("user2",           wSetup.user[2]);
  SetupStore("ri",              wSetup.scan_remove_invalid);
  SetupStore("ue",              wSetup.scan_update_existing);
  SetupStore("an",              wSetup.scan_append_new);
  cCondWait::SleepMs(50);
  Setup.Save();
}

/* convert service strings to zero based int, -1 on error
 */
int cPluginWirbelscan::servicetype(const char* id, bool init) {
  static char strings[9][32];
  int num = sizeof(strings) / sizeof(strings[0]);
  int i = 0;

  if (init) {
     sprintf(strings[i++], "%s%s"   , SPlugin, SInfo);
     sprintf(strings[i++], "%sGet%s", SPlugin, SStatus); 
     sprintf(strings[i++], "%s%s"   , SPlugin, SCommand);
     sprintf(strings[i++], "%sGet%s", SPlugin, SSetup);
     sprintf(strings[i++], "%sSet%s", SPlugin, SSetup);
     sprintf(strings[i++], "%sGet%s", SPlugin, SSat);
     sprintf(strings[i++], "%sGet%s", SPlugin, SCountry);
     sprintf(strings[i++], "%sGet%s", SPlugin, SUser);
     sprintf(strings[i++], "%sSet%s", SPlugin, SUser);
     }

  for(i = 0; i < num; i++) {
     if (!strcmp(id, strings[i]))
        return i;
     }
  return -1;
}

// Handle custom service requests from other plugins
bool cPluginWirbelscan::Service(const char* id, void* Data) {
  switch(servicetype(id)) {
     case 0: { // info
        if (! Data) return true; // check for support.
        cWirbelscanInfo* info = (cWirbelscanInfo*) Data;
        info->PluginVersion  = VERSION;
        info->CommandVersion = SCommand;
        info->StatusVersion  = SStatus;
        info->SetupVersion   = SSetup;
        info->CountryVersion = SCountry;
        info->SatVersion     = SSat;
        info->UserVersion    = SUser;   // 0.0.5-pre12b
        info->reserved2      = VERSION; // may change later.
        info->reserved3      = VERSION; // may change later.
        return true;
        }
     case 1: { // status
        if (! Data) return true; // check for support.
        cWirbelscanStatus* s = (cWirbelscanStatus*) Data;
        if (Scanner)
           s->status = StatusScanning;
        else
           s->status = StatusStopped;
        memset(s->curr_device, 0, 256);
        strcpy(s->curr_device, deviceName.length()? deviceName.c_str():"none");
        memset(s->transponder, 0, 256);
        strcpy(s->transponder, lTransponder.length()? lTransponder.c_str():"none");
        s->progress = s->status == StatusScanning?lProgress:0;
        s->strength = s->status == StatusScanning?lStrength:0;
        s->numChannels = cChannels::MaxNumber();
        s->newChannels = (NewChannels.Count() > cChannels::MaxNumber()) ? NewChannels.Count() - cChannels::MaxNumber():0;
        s->nextTransponders = nextTransponders;
        return true;
        }
     case 2: { // command
        if (! Data) return true; // check for support.
        cWirbelscanCmd* request = (cWirbelscanCmd*) Data;
        switch (request->cmd) {
           case CmdStartScan:
              request->replycode = DoScan(wSetup.DVB_Type);
              break;
           case CmdStopScan:
              DoStop();
              request->replycode = true;
              break;
           case CmdStore:
              StoreSetup();
              request->replycode = true;
              break;
           default:
              request->replycode = false;
              return false;
           }
        return true;
        }
     case 3: { // get setup
        if (! Data) return true; // check for support.
        cWirbelscanScanSetup* d = (cWirbelscanScanSetup*) Data;
        d->verbosity       = wSetup.verbosity;
        d->logFile         = wSetup.logFile;
        d->DVB_Type        = wSetup.DVB_Type;
        d->DVBT_Inversion  = wSetup.DVBT_Inversion;
        d->DVBC_Inversion  = wSetup.DVBC_Inversion;
        d->DVBC_Symbolrate = wSetup.DVBC_Symbolrate;
        d->DVBC_QAM        = wSetup.DVBC_QAM;
        d->CountryId       = wSetup.CountryIndex;
        d->SatId           = wSetup.SatIndex;
        d->scanflags       = wSetup.scanflags;
        d->ATSC_type       = wSetup.ATSC_type;
        return true;
        }
     case 4: { // set setup
        if (! Data) return true; // check for support.
        cWirbelscanScanSetup* d = (cWirbelscanScanSetup*) Data;
        wSetup.verbosity       = d->verbosity;
        wSetup.logFile         = d->logFile;
        wSetup.DVB_Type        = (int) d->DVB_Type;
        wSetup.DVBT_Inversion  = d->DVBT_Inversion;
        wSetup.DVBC_Inversion  = d->DVBC_Inversion;
        wSetup.DVBC_Symbolrate = d->DVBC_Symbolrate;
        wSetup.DVBC_QAM        = d->DVBC_QAM;
        wSetup.CountryIndex    = d->CountryId;
        wSetup.SatIndex        = d->SatId;
        wSetup.scanflags       = d->scanflags;
        wSetup.ATSC_type       = d->ATSC_type;
        return true;
        }
     case 5: { // get sat
        if (! Data) return true; // check for support.
        cPreAllocBuffer* b = (cPreAllocBuffer*) Data;
        SListItem* l = b->buffer;
        b->count = 0;
        if (b->size < (uint) sat_count()) {
           b->size = sat_count();
           return true;
           }
        for (int i = 0; i < sat_count(); i++) {
           memset(&l[i], 0, sizeof(SListItem));
           l[i].id = sat_list[i].id;
           strcpy(l[i].short_name, sat_list[i].short_name);
           strcpy(l[i].full_name, sat_list[i].full_name);
           b->count++;
           }
        return true;
        }
     case 6: { // get country
        if (! Data) return true; // check for support.
        cPreAllocBuffer* b = (cPreAllocBuffer*) Data;
        SListItem* l = b->buffer;
        b->count = 0;
        if (b->size < (uint) COUNTRY::country_count()) {
           b->size = COUNTRY::country_count();
           return true;
           }
        for (int i = 0; i < COUNTRY::country_count(); i++) {
           memset(&l[i], 0, sizeof(SListItem));
           l[i].id = COUNTRY::country_list[i].id;
           strcpy(l[i].short_name, COUNTRY::country_list[i].short_name);
           strcpy(l[i].full_name, COUNTRY::country_list[i].full_name);
           b->count++;
           }
        return true;
        }
     case 7: { // get user
        if (! Data) return true; // check for support
        *((uint32_t*) Data + 0) = wSetup.user[0];
        *((uint32_t*) Data + 1) = wSetup.user[1];
        *((uint32_t*) Data + 2) = wSetup.user[2];
        return true;
        }
     case 8: { // set user
        if (! Data) return true; // check for support
        wSetup.user[0] = *((uint32_t*) Data + 0);
        wSetup.user[1] = *((uint32_t*) Data + 1);
        wSetup.user[2] = *((uint32_t*) Data + 2);
        return true;
        }
     default:
        return false;
     }
}

const char** cPluginWirbelscan::SVDRPHelpPages(void) {
  static const char * SVDRHelp[] = {
    "S_START\n"
    "    Start scan",
    "S_STOP\n"
    "    Stop scan(s) (if any)",
    "S_TERR\n"
    "    Start DVB-T scan",
    "S_CABL\n"
    "    Start DVB-C scan",
    "S_SAT\n"
    "    Start DVB-S/S2 scan",
    "SETUP <verb:log:type:inv_t:inv_c:srate:qam:cidx:sidx:s2:atsc:flags>\n"
    "    verb   verbostity (0..5)\n"
    "    log    logfile (0=OFF, 1=stdout, 2=syslog)\n"
    "    type   scan type\n"
    "           (0=DVB-T, 1=DVB-C, 2=DVB-S/S2, 5=ATSC)\n"
    "    inv_t  DVB-T inversion\n"
    "           (0=AUTO/OFF, 1=AUTO/ON)\n"
    "    inv_c  DVB-C inversion\n"
    "           (0=AUTO/OFF, 1=AUTO/ON)\n"
    "    srate  DVB-C Symbolrate (0..15)\n"
    "    qam    DVB-C modulation\n"
    "           (0=AUTO, 1=QAM64, 2=QAM128, 3=QAM256, 4=ALL)\n"
    "    cidx   country list index\n"
    "    sidx   satellite list index\n"
    "    s2     enable DVB-S2 (0=OFF, 1=ON)\n"
    "    atsc   ATSC scan type\n"
    "           (0=VSB, 1=QAM, 2=VSB+QAM)\n"
    "    flags  bitwise flag of\n"
    "           TV=1, RADIO=2, FTA=4, SCRAMBLED=8, HDTV=16",
    "STORE\n"
    "    Store current setup",
    "LSTC\n"
    "    list countries",
    "LSTS\n"
    "    list satellites",
    "QUERY\n"
    "    return plugin version, current setup and service versions",
    NULL
    };
  return SVDRHelp;
}

#define cmd(x) (strcasecmp(Command, x) == 0)

// process svdrp commands.
cString cPluginWirbelscan::SVDRPCommand(const char* Command, const char* Option, int& ReplyCode) {
  if      cmd("S_TERR"  ) { return DoScan(wSetup.DVB_Type = SCAN_TERRESTRIAL)   ? "DVB-T scan started"     : "Could not start DVB-T scan.";    }
  else if cmd("S_CABL"  ) { return DoScan(wSetup.DVB_Type = SCAN_CABLE)         ? "DVB-C scan started"     : "Could not start DVB-C scan.";    }
  else if cmd("S_SAT"   ) { return DoScan(wSetup.DVB_Type = SCAN_SATELLITE)     ? "DVB-S scan started"     : "Could not start DVB-S scan.";    }
  else if cmd("S_START" ) { return DoScan(wSetup.DVB_Type)              ? "starting scan"          : "Could not start scan.";          }
  else if cmd("S_STOP"  ) { DoStop();       return "stopping scan(s)";  }
  else if cmd("STORE"   ) { StoreSetup();   return "setup stored.";     }

  else if cmd("SETUP") {
     cMySetup d;

     if (12 != sscanf(Option, "%i:%i:%i:%i:%i:%i:%i:%i:%i:%i:%i:%u",
        &d.verbosity, &d.logFile,
        &d.DVB_Type,
        &d.DVBT_Inversion, &d.DVBC_Inversion,
        &d.DVBC_Symbolrate, &d.DVBC_QAM,
        &d.CountryIndex, &d.SatIndex,
        &d.enable_s2, &d.ATSC_type, &d.scanflags)) {
        //error.
        ReplyCode = 501;
        return "couldnt parse setup string.";
        }

     wSetup = d;
     return cString::sprintf("changed setup to %d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%u",
                            wSetup.verbosity,
                            wSetup.logFile,
                            wSetup.DVB_Type,
                            wSetup.DVBT_Inversion,
                            wSetup.DVBC_Inversion,
                            wSetup.DVBC_Symbolrate,
                            wSetup.DVBC_QAM,
                            wSetup.CountryIndex,
                            wSetup.SatIndex,
                            wSetup.enable_s2,
                            wSetup.ATSC_type,
                            wSetup.scanflags);
     }

  else if cmd("QUERY") {
     return cString::sprintf("plugin version: %s\n"
                            "current setup:  %d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d\n"
                            "commands api:   %s\n"
                            "status api:     %s\n"
                            "setup api:      %s\n"
                            "country api:    %s\n"
                            "sat api:        %s\n"
                            "user api:       %s",
                            VERSION,
                            wSetup.verbosity,
                            wSetup.logFile,
                            wSetup.DVB_Type,
                            wSetup.DVBT_Inversion,
                            wSetup.DVBC_Inversion,
                            wSetup.DVBC_Symbolrate,
                            wSetup.DVBC_QAM,
                            wSetup.CountryIndex,
                            wSetup.SatIndex,
                            wSetup.enable_s2,
                            wSetup.ATSC_type,
                            wSetup.scanflags,
                            SCommand,
                            SStatus,
                            SSetup,
                            SCountry,
                            SSat,
                            SUser);
     }

  else if cmd("LSTC") {
     cString s = "";
     for(int i = 0; i < COUNTRY::country_count(); i++)
        s = cString::sprintf("%s%d:%s:%s\n", *s,
                            COUNTRY::country_list[i].id,
                            COUNTRY::country_list[i].short_name,
                            COUNTRY::country_list[i].full_name);
     return s;
     }

  else if cmd("LSTS") {
     cString s = "";
     for(int i = 0; i < sat_count(); i++)
        s = cString::sprintf("%s%d:%s:%s\n", *s,
                            sat_list[i].id,
                            sat_list[i].short_name,
                            sat_list[i].full_name);
     return s;
     }

  return NULL;
}

VDRPLUGINCREATOR(cPluginWirbelscan); // Don't touch this!
