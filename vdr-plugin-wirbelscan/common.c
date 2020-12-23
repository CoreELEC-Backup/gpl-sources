/*
 * common.c: wirbelscan - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 */

/*
 *  Generic functions which will be used in the whole plugin.
 */

#include <stdarg.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <vdr/plugin.h>
#include <vdr/device.h>
#include <vdr/config.h>
#include "common.h"
#include "menusetup.h"

// plugin setup data
cMySetup::cMySetup() {
  verbosity       = 3;              /* default to messages           */
  DVB_Type        = SCAN_TERRESTRIAL;
  DVBT_Inversion  = 0;              /* auto/off                      */
  DVBC_Inversion  = 0;              /* auto/off                      */
  DVBC_Symbolrate = 0;              /* default to AUTO               */
  DVBC_QAM        = 0;              /* default to AUTO               */
  DVBC_Network_PID= 0x10;           /* as 300486                     */
  CountryIndex    = 82;             /* default to Germany            */
  SatIndex        = 67;             /* default to Astra 19.2         */
  enable_s2       = 1;
  ATSC_type       = 0;              /* VSB                           */
  logFile         = STDOUT;         /* log errors/messages to stdout */
  scanflags       = SCAN_TV | SCAN_RADIO | SCAN_FTA | SCAN_SCRAMBLED;
  update          = false;
  initsystems     = false;
  scan_remove_invalid  = false;
  scan_update_existing = false;
  scan_append_new      = true;
}

void cMySetup::InitSystems(void) {
  memset(&systems[0], 0, sizeof(systems));
  while(! cDevice::WaitForAllDevicesReady(20)) sleep(1);

  for(int i = 0; i < cDevice::NumDevices(); i++) {
     cDevice* device = cDevice::GetDevice(i);
     if (device == NULL) continue;
     if (device->ProvidesSource(cSource::stSat))   systems[SCAN_SATELLITE] = 1;
     if (device->ProvidesSource(cSource::stTerr))  systems[SCAN_TERRESTRIAL] = 1;
     if (device->ProvidesSource(cSource::stCable)) systems[SCAN_CABLE] = 1;
     if (device->ProvidesSource(cSource::stAtsc))  systems[SCAN_TERRCABLE_ATSC] = 1;
     }

  if (DVB_Type >= SCAN_NO_DEVICE || ! systems[DVB_Type]) {
     for(DVB_Type = SCAN_TERRESTRIAL; DVB_Type < SCAN_NO_DEVICE; DVB_Type++) {
        if (systems[DVB_Type])
           break;
        }
     }
  initsystems = true;
}

cMySetup wSetup;            

void _log(const char* function, int line, const int level, bool newline, const char* fmt, ...) {
  if (level > wSetup.verbosity)
     return;

  char msg[256];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(msg, sizeof(msg), fmt, ap);
  va_end(ap);

  switch (wSetup.logFile) {
     default:
        printf("WARNING: setting logFile to STDOUT\n");
        wSetup.logFile = STDOUT;
        /* Falls through. */
     case STDOUT: {
        char tmbuf[9];
        time_t now = time(NULL);
        strftime(tmbuf, sizeof(tmbuf), "%H:%M:%S", localtime(&now));
        if (wSetup.verbosity < 5)
           printf("%s %s%s", tmbuf, msg, newline?"\n":"");
        else
           printf("%s %-40s:%d %s%s", tmbuf, function, line, msg, newline?"\n":"");
        fflush(stdout);
        }
        break;
     case SYSLOG:
        syslog(LOG_DEBUG, "%s", msg);
        break;
     }

  if (MenuScanning)
     MenuScanning->AddLogMsg(msg);
}


void hexdump(const char* intro, const unsigned char* buf, int len) {
  int i,j;
  char sbuf[17] = { 0 };


  if (wSetup.verbosity < 3)
    return;

  if (! buf) {
     dlog(0, "BUG: %s was called with buf = NULL", __FUNCTION__);
     return;
     }

  printf("\t===================== %s ", intro);
  for(i = strlen(intro) + 1; i < 50; i++)
     printf("=");
  printf("\n");
  printf("\tlen = %d\n", len);
  for(i = 0; i < len; i++) {
     if ((i % 16) == 0)
        printf("%s0x%.2X: ", i ? "\n\t" : "\t", (i / 16) * 16);
     printf("%.2X ", (uint8_t) *(buf + i));
     sbuf[i % 16] = *(buf + i);
     if (((i + 1) % 16) == 0) {
        // remove non-printable chars
        for(j = 0; j < 16; j++)
           if (!((sbuf[j] > 31) && (sbuf[j] < 127))) {
           sbuf[j] = ' ';
           }
        printf(": %s", sbuf);
        memset(&sbuf, 0, 17);
        }
     }
  if (len % 16) {
     for(i = 0; i < (len % 16); i++)
        if (!((sbuf[i] > 31) && (sbuf[i] < 127)))
        sbuf[i] = ' ';
     for (i = (len % 16); i < 16; i++)
        printf("   ");
     printf(": %s", sbuf);
     }
  printf("\n");
  printf("\t========================================================================\n");
}


int IOCTL(int fd, int cmd, void* data) {
  int retry;
    
  for(retry=10; retry>=0;) {
     if (ioctl(fd, cmd, data) != 0) {
        /* :-( */
        if (retry) {
           usleep(10000); /* 10msec */
           retry--;
           continue;
           }
        return -1;       /* :'-((  */
        }
     else
        return 0;        /* :-)    */
     }
  return 0;
}

#include <sys/stat.h> 
bool FileExists(const char* aFile) {
  struct stat Stat; 

  if (! stat(aFile,&Stat))
     return true; 
  return false; 
}


int dvbc_modulation(int index) {
  switch(index) {
     case 0:   return 64;
     case 1:   return 256;
     case 2:   return 128;
     default:  return 999;
     }
}

int dvbc_symbolrate(int index) {
  switch(index) {
     case 0:   return 6900000;
     case 1:   return 6875000;
     case 2:   return 6111000;
     case 3:   return 6250000;
     case 4:   return 6790000;
     case 5:   return 6811000;
     case 6:   return 5900000;
     case 7:   return 5000000;
     case 8:   return 3450000;
     case 9:   return 4000000;
     case 10:  return 6950000;
     case 11:  return 7000000;
     case 12:  return 6952000;
     case 13:  return 5156000;
     case 14:  return 5483000;
     default:  return 0;
     }
}

/*******************************************************************************
 * TParams
 * read VDR param string and divide to separate items or vice versa.
 ******************************************************************************/

TParams::TParams() :
  Bandwidth(8), FEC(999), FEC_low(999), Guard(999), Polarization(0),
  Inversion(999), Modulation(2), Pilot(999), Rolloff(999),
  StreamId(0), SystemId(0), DelSys(0), Transmission(999),
  MISO(0), Hierarchy(999)
{}

TParams::TParams(std::string& s) :
  Bandwidth(8), FEC(999), FEC_low(999), Guard(999), Polarization(0),
  Inversion(999), Modulation(2), Pilot(999), Rolloff(999),
  StreamId(0), SystemId(0), DelSys(0), Transmission(999),
  MISO(0), Hierarchy(999)
{
  Parse(s);
}

void TParams::Parse(std::string& s) {
  std::transform(s.begin(), s.end() ,s.begin(), ::toupper);
  const char* c = s.c_str();
  while(*c) {
     switch(*c) {
        case 'H':
        case 'V':
        case 'L':
        case 'R':
           Polarization = *c++;
           break;
        case 'B':
           Bandwidth = Value(c);
           break;
        case 'C':
           FEC = Value(c);
           break;
        case 'D':
           FEC_low = Value(c);
           break;
        case 'G':
           Guard = Value(c);
           break;
        case 'I':
           Inversion = Value(c);
           break;
        case 'M':
           Modulation = Value(c);
           break;
        case 'N':
           Pilot= Value(c);
           break;
        case 'O':
           Rolloff= Value(c);
           break;
        case 'P':
           StreamId= Value(c);
           break;
        case 'Q':
           SystemId= Value(c);
           break;
        case 'S':
           DelSys= Value(c);
           break;
        case 'T':
           Transmission= Value(c);
           break;
        case 'X':
           MISO= Value(c);
           break;
        case 'Y':
           Hierarchy= Value(c);
           break;
        default:
           dlog(0, "%s %d: error in '%s': invalid char '%c'",
               __PRETTY_FUNCTION__,__LINE__,s.c_str(),*c);
           return;
        }
     }
}

int TParams::Value(const char*& s) {
  int v = 0;

  ++s;

  while(*s) {
     switch(*s) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
           v = 10 * v + (*s++ - '0');
           break;
        default:
           return v;
        }
     }
  return v;
}

void TParams::Print(std::string& dest, char Source) {
  char tmp[8];
  dest = "";

  switch(Source) {
     case 'A':
        if (Inversion != 999) {
           snprintf(tmp, 8, "I%d", Inversion);
           dest += tmp;
           }
        if (Modulation != 999) {
           snprintf(tmp, 8, "M%d", Modulation);
           dest += tmp;
           }
        break;
     case 'C':
        if (FEC != 999) {
           snprintf(tmp, 8, "C%d", FEC);
           dest += tmp;
           }
        if (Inversion != 999) {
           snprintf(tmp, 8, "I%d", Inversion);
           dest += tmp;
           }
        if (Modulation != 999) {
           snprintf(tmp, 8, "M%d", Modulation);
           dest += tmp;
           }
        break;
     case 'S':
        if (Polarization)
           dest += Polarization;
        if (FEC != 999) {
           snprintf(tmp, 8, "C%d", FEC);
           dest += tmp;
           }
        if (Inversion != 999) {
           snprintf(tmp, 8, "I%d", Inversion);
           dest += tmp;
           }
        if (Modulation != 999) {
           snprintf(tmp, 8, "M%d", Modulation);
           dest += tmp;
           }
        if (DelSys) {
           if (Pilot != 999) {
              snprintf(tmp, 8, "N%d", Pilot);
              dest += tmp;
              }
           if (Rolloff != 999) {
              snprintf(tmp, 8, "O%d", Rolloff);
              dest += tmp;
              }
           if (StreamId != 999) {
              snprintf(tmp, 8, "P%d", StreamId);
              dest += tmp;
              }
           }
        if (DelSys != 999) {
           snprintf(tmp, 8, "S%d", DelSys);
           dest += tmp;
           }
        break;
     case 'T':
        if (Bandwidth != 999) {
           snprintf(tmp, 8, "B%d", Bandwidth);
           dest += tmp;
           }
        if (FEC != 999) {
           snprintf(tmp, 8, "C%d", FEC);
           dest += tmp;
           }
        if (FEC_low != 999) {
           snprintf(tmp, 8, "D%d", FEC_low);
           dest += tmp;
           }
        if (Guard != 999) {
           snprintf(tmp, 8, "G%d", Guard);
           dest += tmp;
           }
        if (Inversion != 999) {
           snprintf(tmp, 8, "I%d", Inversion);
           dest += tmp;
           }
        if (Modulation != 999) {
           snprintf(tmp, 8, "M%d", Modulation);
           dest += tmp;
           }
        if (DelSys) {
           if (StreamId != 999) {
              snprintf(tmp, 8, "P%d", StreamId);
              dest += tmp;
              }
           if (SystemId != 999) {
              snprintf(tmp, 8, "Q%d", SystemId);
              dest += tmp;
              }
           }
        if (DelSys != 999) {
           snprintf(tmp, 8, "S%d", DelSys);
           dest += tmp;
           }
        if (Transmission != 999) {
           snprintf(tmp, 8, "T%d", Transmission);
           dest += tmp;
           }
        if (DelSys and MISO != 999) {
           snprintf(tmp, 8, "X%d", MISO);
           dest += tmp;
           }
        if (Hierarchy != 999) {
           snprintf(tmp, 8, "Y%d", Hierarchy);
           dest += tmp;
           }
        break;
     default:;
        dlog(0, "%s %d: unknown Source 0x%.02X",
            __PRETTY_FUNCTION__,__LINE__,(unsigned char) Source);
     }
}


/*******************************************************************************
 * TChannel
 * read VDR channel string and divide to separate items or vice versa.
 ******************************************************************************/

TChannel::TChannel() :
     Name("???"), Shortname(""), Provider(""), Frequency(0),
     Bandwidth(8), FEC(999), FEC_low(999), Guard(999), Polarization(0),
     Inversion(999), Modulation(2), Pilot(999), Rolloff(999),
     StreamId(0), SystemId(0), DelSys(0), Transmission(999),
     MISO(0), Hierarchy(999), Symbolrate(0), PCR(0), TPID(0),
     SID(0), ONID(0), NID(0), TID(0), RID(0), free_CA_mode(0),
     service_type(0xFFFF), OrbitalPos(0),
     reported(false), Tunable(false), Tested(false)
{}


TChannel& TChannel::operator= (const cChannel* rhs) {
  Name       = rhs->Name();
  Shortname  = rhs->ShortName();
  Provider   = rhs->Provider();
  Frequency  = rhs->Frequency();
  Source     = *cSource::ToString(rhs->Source());
  Symbolrate = rhs->Srate();
  VPID.PID   = rhs->Vpid();
  VPID.Type  = rhs->Vtype();
  PCR        = rhs->Ppid();
  TPID       = rhs->Tpid();
  SID        = rhs->Sid();
  NID = ONID = rhs->Nid();
  TID        = rhs->Tid();
  RID        = rhs->Rid();

  APIDs.Clear();
  for(int i = 0; i < MAXAPIDS and rhs->Apid(i); ++i) {
     TPid a;
     a.PID = rhs->Apid(i);
     a.Type = rhs->Atype(i);
     a.Lang = rhs->Alang(i);
     APIDs.Add(a);
     }

  DPIDs.Clear();
  for(int i = 0; i < MAXDPIDS and rhs->Dpid(i); ++i) {
     TPid d;
     d.PID = rhs->Dpid(i);
     d.Type = rhs->Dtype(i);
     d.Lang = rhs->Dlang(i);
     DPIDs.Add(d);
     }

  SPIDs.Clear();
  for(int i = 0; i < MAXSPIDS and rhs->Spid(i); ++i) {
     TPid s;
     s.PID = rhs->Spid(i);
     s.Type = 0;
     s.Lang = rhs->Slang(i);
     SPIDs.Add(s);
     }

  CAIDs.Clear();
  for(int i = 0; i < MAXCAIDS and rhs->Ca(i); ++i) {
     int ca = rhs->Ca(i);
     CAIDs.Add(ca);
     }

  std::string parameters = rhs->Parameters();

  TParams p(parameters);
  Bandwidth    = p.Bandwidth;
  FEC          = p.FEC;
  FEC_low      = p.FEC_low;
  Guard        = p.Guard;
  Polarization = p.Polarization;
  Inversion    = p.Inversion;
  Modulation   = p.Modulation;
  Pilot        = p.Pilot;
  Rolloff      = p.Rolloff;
  StreamId     = p.StreamId;
  SystemId     = p.SystemId;
  DelSys       = p.DelSys;
  Transmission = p.Transmission;
  MISO         = p.MISO;
  Hierarchy    = p.Hierarchy;
  return *this;
}

void TChannel::CopyTransponderData(const TChannel* Channel) {
  if (Channel) {
     Frequency    = Channel->Frequency;
     Source       = Channel->Source;
     Symbolrate   = Channel->Symbolrate;
     Bandwidth    = Channel->Bandwidth;
     FEC          = Channel->FEC;
     FEC_low      = Channel->FEC_low;
     Guard        = Channel->Guard;
     Polarization = Channel->Polarization;
     Inversion    = Channel->Inversion;
     Modulation   = Channel->Modulation;
     Pilot        = Channel->Pilot;
     Rolloff      = Channel->Rolloff;
     StreamId     = Channel->StreamId;
     SystemId     = Channel->SystemId;
     DelSys       = Channel->DelSys;
     Transmission = Channel->Transmission;
     MISO         = Channel->MISO;
     Hierarchy    = Channel->Hierarchy;
     }
}

void TChannel::Params(std::string& s) {
  if (Source.size() == 0) {
     s.clear();
     return;
     }

  char tmp[8];
  s = "";

  switch(Source[0]) {
     case 'A':
        if (Inversion != 999) {
           snprintf(tmp, 8, "I%d", Inversion);
           s += tmp;
           }
        if (Modulation != 999) {
           snprintf(tmp, 8, "M%d", Modulation);
           s += tmp;
           }
        break;
     case 'C':
        if (FEC != 999) {
           snprintf(tmp, 8, "C%d", FEC);
           s += tmp;
           }
        if (Inversion != 999) {
           snprintf(tmp, 8, "I%d", Inversion);
           s += tmp;
           }
        if (Modulation != 999) {
           snprintf(tmp, 8, "M%d", Modulation);
           s += tmp;
           }
        break;
     case 'S':
        if (Polarization)
           s += Polarization;
        if (FEC != 999) {
           snprintf(tmp, 8, "C%d", FEC);
           s += tmp;
           }
        if (Inversion != 999) {
           snprintf(tmp, 8, "I%d", Inversion);
           s += tmp;
           }
        if (Modulation != 999) {
           snprintf(tmp, 8, "M%d", Modulation);
           s += tmp;
           }
        if (DelSys) {
           if (Pilot != 999) {
              snprintf(tmp, 8, "N%d", Pilot);
              s += tmp;
              }
           if (Rolloff != 999) {
              snprintf(tmp, 8, "O%d", Rolloff);
              s += tmp;
              }
           if (StreamId != 999) {
              snprintf(tmp, 8, "P%d", StreamId);
              s += tmp;
              }
           }
        if (DelSys != 999) {
           snprintf(tmp, 8, "S%d", DelSys);
           s += tmp;
           }
        break;
     case 'T':
        if (Bandwidth != 999) {
           snprintf(tmp, 8, "B%d", Bandwidth);
           s += tmp;
           }
        if (FEC != 999) {
           snprintf(tmp, 8, "C%d", FEC);
           s += tmp;
           }
        if (FEC_low != 999) {
           snprintf(tmp, 8, "D%d", FEC_low);
           s += tmp;
           }
        if (Guard != 999) {
           snprintf(tmp, 8, "G%d", Guard);
           s += tmp;
           }
        if (Inversion != 999) {
           snprintf(tmp, 8, "I%d", Inversion);
           s += tmp;
           }
        if (Modulation != 999) {
           snprintf(tmp, 8, "M%d", Modulation);
           s += tmp;
           }
        if (DelSys) {
           if (StreamId != 999) {
              snprintf(tmp, 8, "P%d", StreamId);
              s += tmp;
              }
           if (SystemId != 999) {
              snprintf(tmp, 8, "Q%d", SystemId);
              s += tmp;
              }
           }
        if (DelSys != 999) {
           snprintf(tmp, 8, "S%d", DelSys);
           s += tmp;
           }
        if (Transmission != 999) {
           snprintf(tmp, 8, "T%d", Transmission);
           s += tmp;
           }
        if (DelSys and MISO != 999) {
           snprintf(tmp, 8, "X%d", MISO);
           s += tmp;
           }
        if (Hierarchy != 999) {
           snprintf(tmp, 8, "Y%d", Hierarchy);
           s += tmp;
           }
        break;
     default:;
        dlog(0, "%s %d: unknown Source %s",
            __PRETTY_FUNCTION__,__LINE__, Source.c_str());
     }
}

void TChannel::PrintTransponder(std::string& dest) {
  std::string s;
  int i = Frequency;
  char b[16];
  char source = Source[0];

  switch(source) {
     case 'A': dest = "ATSC" ; break;
     case 'C': dest = "C"; break;
     case 'S': dest = "S"; break;
     case 'T': dest = "T"; break;
     default:;
     }
  if (DelSys == 1)
     dest += "2";
  else
     dest += " ";

  if (i < 1000)    i *= 1000;
  if (i > 999999)  i /= 1000;
  snprintf(b,16," %8.2f MHz ", source == 'S'? i:i/1000.0);
  dest += b;
//dest += std::to_string(i) + " MHz ";

  switch(source) {
     case 'C':
     case 'S':
        i = Symbolrate;
        if (i < 1000)    i *= 1000;
        if (i > 999999)  i /= 1000;
        snprintf(b,16,"SR %d ",i);
        dest += b;
      //dest += "SR " + std::to_string(i);
        break;
     default:;
     }

  Params(s);
  dest += s;
}

void TChannel::Print(std::string& dest) {
  std::string params;
  char buf[512], *p = buf;

  Params(params);

  sprintf(p, "%s%s%s%s%s:%d:%s:%s:%d:%d",
     Name.size()?Name.c_str():"NULL",
     Shortname.size()?",":"", Shortname.c_str(),
     Provider.size() ?";":"", Provider.c_str(),
     Frequency, params.c_str(), Source.c_str(),
     Symbolrate, VPID.PID);
  p += strlen(p);

  if (PCR and PCR != VPID.PID) {
     sprintf(p, "+%d", PCR);
     p += strlen(p);
     }

  if (VPID.Type) {
     sprintf(p, "=%d", VPID.Type);
     p += strlen(p);
     }

  sprintf(p, "%s", ":"); p++;

  if (APIDs.Count()) {
     for(int i = 0; i < APIDs.Count(); ++i) {
        sprintf(p, "%s%d", i?",":"", APIDs[i].PID);
        p += strlen(p);
        if (APIDs[i].Lang.size()) {
           sprintf(p, "=%s", APIDs[i].Lang.c_str());
           p += strlen(p);
           }
        if (APIDs[i].Type) {
           sprintf(p, "@%d", APIDs[i].Type);
           p += strlen(p);
           }
        }
     }
  else {
     sprintf(p, "%d", 0); p++;
     }

  if (DPIDs.Count()) {
     sprintf(p, "%s", ";"); p++;
     for(int i = 0; i < DPIDs.Count(); ++i) {
        sprintf(p, "%s%d", i?",":"", DPIDs[i].PID);
        p += strlen(p);
        if (DPIDs[i].Lang.size()) {
           sprintf(p, "=%s", DPIDs[i].Lang.c_str());
           p += strlen(p);           
           }
        if (DPIDs[i].Type) {
           sprintf(p, "@%d", DPIDs[i].Type);
           p += strlen(p);           
           }
        }
     }
  sprintf(p, ":%d:", TPID);
  p += strlen(p);
  if (CAIDs.Count()) {
     for(int i = 0; i < CAIDs.Count(); ++i) {
        sprintf(p, "%s%x", i?",":"", CAIDs[i]);
        p += strlen(p);
        }
     }
  else {
     sprintf(p, "%d", 0); p++;
     }
  sprintf(p, ":%d:%d:%d:%d", SID, ONID, TID, RID);
  dest = buf;
}

void TChannel::VdrChannel(cChannel& c) {
  std::string s;
  Print(s);
  c.Parse(s.c_str());
}

static bool SourceMatches(int a, int b) {
  static const int SatRotor = cSource::stSat | cSource::st_Any;

  return (a == b or
         (a == SatRotor and (b & cSource::stSat)));
}

bool TChannel::ValidSatIf() {
  int f = Frequency;

  while(f > 999999) f /= 1000;

  if (Setup.DiSEqC) {
     cDiseqc* d;
     for(d = Diseqcs.First(); d; d = Diseqcs.Next(d))
        if (SourceMatches(d->Source(), cSource::FromString(Source.c_str())) and
            d->Slof() > f and d->Polarization() == Polarization) {
           f -= d->Lof();
           break;
           }
     if (!d) {
        dlog(0, "no diseqc settings for (%s, %d, %c)", Source.c_str(), Frequency, Polarization);
        return false;
        }
     }
  else
     f -= f < Setup.LnbSLOF ? Setup.LnbFrequLo : Setup.LnbFrequHi;

  if (f < 950 or f > 2150) {
     dlog(0, "transponder (%s, %d, %c) (freq %d -> out of tuning range)",
         Source.c_str(), Frequency, Polarization, f);
     return false;
     }
  return true;
}

