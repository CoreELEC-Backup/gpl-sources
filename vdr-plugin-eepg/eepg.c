/*
 * Extended Epg plugin to VDR (C++)
 *
 * (C) 2008-2009 Dingo35
 *
 * This code is based on:
 * -Premiere plugin (C) 2005-2007 Stefan Huelswitt <s.huelswitt@gmx.de>
 * -mhwepg program  (C) 2002, 2003 Jean-Claude Repetto <mhwepg@repetto.org>
 * -LoadEpg plugin written by Luca De Pieri <dpluca@libero.it>
 * -Freesat patch written by dom /at/ suborbital.org.uk
 *
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 */

#include <vdr/plugin.h>
#include <vdr/filter.h>
#include <vdr/epg.h>
#include <vdr/channels.h>
#include <vdr/dvbdevice.h>
#include <vdr/i18n.h>
#include <vdr/config.h>
#include <libsi/section.h>
#include <libsi/descriptor.h>
#include <libsi/si.h>
#include "eepg.h"
#include "dish.h"
#if APIVERSNUM > 10725
#include "epghandler.h"
#endif
#include "log.h"
#include "setupeepg.h"
#include "equivhandler.h"
#include "util.h"
#include "eit2.h"

#include <map>
#include <string>
#include <limits>
#include <stdarg.h>
#include <dirent.h>


#if defined(APIVERSNUM) && APIVERSNUM < 10401
#error You need at least VDR API version 1.4.1 for this plugin
#endif
#if APIVERSNUM < 10507
#define trNOOP(s) (s)
#endif

#define PMT_SCAN_TIMEOUT  10 // seconds
#define PMT_SCAN_IDLE     3600 // seconds

static const char *VERSION = "0.0.6pre";
static const char *DESCRIPTION = trNOOP ("Parses Extended EPG data");

using namespace std;
using namespace util;

const char *optPats[] = {
  "%s",
  "%s (Option %d)",
  "%s (O%d)",
  "#%2$d %1$s",
  "[%2$d] %1$s"
};

#define NUM_PATS (sizeof(optPats)/sizeof(char *))

char *cs_hexdump (int m, const uchar * buf, int n)
{
  int i;
  static char dump[1024];

  dump[i = 0] = '\0';
  m = (m) ? 3 : 2;
  if (m * n >= (int) sizeof (dump))
    n = (sizeof (dump) / m) - 1;
  while (i < n)
    sprintf (dump + (m * i++), "%02X%s", *buf++, (m > 2) ? " " : "");
  return (dump);
}

cSetupEEPG* SetupPE = cSetupEEPG::getInstance();

// --- cMenuSetupPremiereEpg ------------------------------------------------------------

class cMenuSetupPremiereEpg:public cMenuSetupPage
{
private:
  cSetupEEPG* data;
  const char *optDisp[NUM_PATS];
  char buff[NUM_PATS][32];
protected:
  virtual void Store (void);
public:
  cMenuSetupPremiereEpg (void);
};

cMenuSetupPremiereEpg::cMenuSetupPremiereEpg (void)
{
  data = cSetupEEPG::getInstance();
  cOsdItem *item = new cOsdItem(tr ("PremiereEPG"));

  if (item) {
    item->SetSelectable(false);
    Add(item);
  }
//  AddCategory (tr ("PremiereEPG"));
  optDisp[0] = tr ("off");
  for (unsigned int i = 1; i < NUM_PATS; i++) {
    snprintf (buff[i], sizeof (buff[i]), optPats[i], "Event", 1);
    optDisp[i] = buff[i];
  }
  Add (new cMenuEditStraItem (tr ("Tag option events"), &data->OptPat, NUM_PATS, optDisp));
  Add (new cMenuEditBoolItem (tr ("Show order information"), &data->OrderInfo));
  Add (new cMenuEditBoolItem (tr ("Show rating information"), &data->RatingInfo));
  Add (new cMenuEditBoolItem (tr ("Fix EPG data"), &data->FixEpg));
  item = new cOsdItem(tr ("General"));
  if (item) {
    item->SetSelectable(false);
    Add(item);
  }
//  AddCategory (tr ("General"));
  Add (new cMenuEditBoolItem (tr ("Display summary message"), &data->DisplayMessage));
  Add (new cMenuEditBoolItem (tr ("Replace empty Short Text with Category - Genre"), &data->ReplaceEmptyShText));
  Add (new cMenuEditBoolItem (tr ("Try to fix CharSet for events"), &data->FixCharset));
#ifdef DEBUG
  Add (new cMenuEditIntItem (tr ("Level of logging verbosity"), &data->LogLevel, 0, 5));
  Add (new cMenuEditBoolItem (tr ("Process EIT info with EEPG"), &data->ProcessEIT));
#endif
}

void cMenuSetupPremiereEpg::Store (void)
{
  //SetupPE = data;
  SetupStore ("OptionPattern", SetupPE->OptPat);
  SetupStore ("OrderInfo", SetupPE->OrderInfo);
  SetupStore ("RatingInfo", SetupPE->RatingInfo);
  SetupStore ("FixEpg", SetupPE->FixEpg);
  SetupStore ("DisplayMessage", SetupPE->DisplayMessage);
  SetupStore ("ReplaceEmptyShText", SetupPE->ReplaceEmptyShText);
  SetupStore ("FixCharset", SetupPE->FixCharset);
#ifdef DEBUG
  SetupStore ("LogLevel", SetupPE->LogLevel);
  SetupStore ("ProcessEIT", SetupPE->ProcessEIT);
#endif
}

//#define Asprintf(a, b, c...) void( asprintf(a, b, c) < 0 ? esyslog("memory allocation error - %s", b) : void() )

// --- CRC16 -------------------------------------------------------------------

#define POLY 0xA001  // CRC16

unsigned int crc16 (unsigned int crc, unsigned char const *p, int len)
{
  while (len--) {
    crc ^= *p++;
    for (int i = 0; i < 8; i++)
      crc = (crc & 1) ? (crc >> 1) ^ POLY : (crc >> 1);
  }
  return crc & 0xFFFF;
}

// --- cFilterEEPG ------------------------------------------------------

#define STARTTIME_BIAS (20*60)

static int LastVersionNagra = -1; //currently only used for Nagra, should be stored per transponder, per system


class cFilterEEPG:public cFilter
{
private:
  int pmtpid, pmtsid, pmtidx, pmtnext;
  int UnprocessedFormat[HIGHEST_FORMAT + 1]; //stores the pid when a format is detected on this transponder, and that are not processed yet
  int nChannels, nThemes, nTitles, nSummaries, NumberOfTables, Version;
  int TitleCounter, SummaryCounter, NoSummaryCounter, RejectTableId;
  bool EndChannels, EndThemes; //only used for ??
  int MHWStartTime; //only used for MHW1
  bool ChannelsOk;
  EFormat Format; //the format that this filter currently is processing
  std::map < int, int >ChannelSeq; // ChannelSeq[ChannelId] returns the recordnumber of the channel

  Summary_t *Summaries[MAX_TITLES];
  Title_t *Titles[MAX_TITLES];
  sChannel sChannels[MAX_CHANNELS];
  unsigned char Themes[MAX_THEMES][64];

  std::map < unsigned short int, unsigned char *>buffer; //buffer[Table_Extension_Id] returns the pointer to the buffer for this TEI
  std::map < unsigned short int, int >bufsize; //bufsize[Table_Extension_Id] returns the buffersize of the buffer for this TEI
  unsigned short int NagraTIE[64]; //at this moment a max of 31 table_ids could be used, so 64 should be enough ....stores the Table_Extension_Id's of summaries received, so they can be processed. Processing while receiving somehow drops sections, the 0x0000 marker will be missed ...
  unsigned short int NagraCounter;

  unsigned char InitialChannel[8];
  unsigned char InitialTitle[64];
  unsigned char InitialSummary[64];

  void NextPmt (void);
  void ProccessContinuous(u_short Pid, u_char Tid, int Length, const u_char *Data);
  bool load_sky_file (const char *filename);
  int sky_huffman_decode (const u_char * Data, int Length, unsigned char *DecodeText);
protected:
  virtual void Process (u_short Pid, u_char Tid, const u_char * Data, int Length);
  virtual void AddFilter (u_short Pid, u_char Tid);
  virtual void AddFilter (u_short Pid, u_char Tid, unsigned char Mask);
  virtual void ProcessNextFormat (bool FirstTime);
  virtual int GetChannelsSKYBOX (const u_char * Data, int Length);
  virtual bool GetThemesSKYBOX (void);
  virtual int GetTitlesSKYBOX (const u_char * Data, int Length);
  virtual int GetSummariesSKYBOX (const u_char * Data, int Length);
  virtual int GetChannelsMHW (const u_char * Data, int Length);
  virtual int GetThemesMHW1 (const u_char * Data, int Length);
  virtual int GetNagra (const u_char * Data, int Length);
  virtual void ProcessNagra (void);
  virtual void GetTitlesNagra (const u_char * Data, int Length, unsigned short TableIdExtension);
  virtual char *GetSummaryTextNagra (const u_char * DataStart, long int Offset, unsigned int EventId);
  virtual int GetChannelsNagra (const u_char * Data, int Length);
  virtual int GetThemesNagra (const u_char * Data, int Length, unsigned short TableIdExtension);
  virtual int GetTitlesMHW1 (const u_char * Data, int Length);
  virtual int GetSummariesMHW1 (const u_char * Data, int Length);
  virtual int GetThemesMHW2 (const u_char * Data, int Length);
  virtual int GetTitlesMHW2 (const u_char * Data, int Length);
  virtual int GetSummariesMHW2 (const u_char * Data, int Length);
  virtual void FreeSummaries (void);
  virtual void FreeTitles (void);
  //virtual void PrepareToWriteToSchedule (sChannel * C, cSchedules * s, cSchedule * ps/*[MAX_EQUIVALENCES]*/); //gets a channel and returns an array of schedules that WriteToSchedule can write to. Call this routine before a batch of titles with the same ChannelId will be WriteToScheduled; batchsize can be 1
  //virtual void FinishWriteToSchedule (sChannel * C, cSchedules * s, cSchedule * ps[MAX_EQUIVALENCES]);
  virtual void WriteToSchedule (tChannelID channelID, cSchedules* s, unsigned int EventId, unsigned int StartTime,
                                unsigned int Duration, char *Text, char *SummText, unsigned short int ThemeId,
                                unsigned short int TableId, unsigned short int Version, char Rating = 0x00, unsigned char ShortTextLenght = 0);
  virtual void LoadIntoSchedule (void);
  //virtual void LoadEquivalentChannels (void);

  void ProcessPremiere(const u_char *& Data);
public:
  cFilterEEPG (void);
  virtual void SetStatus (bool On);
  void Trigger (void);
  bool InitDictionary (void); //Initialize the Huffman tables for SKY and Freesat

  static const int EIT_PID = 0x12;
};

cFilterEEPG::cFilterEEPG (void)
{
  nSummaries = 0;
  nTitles = 0;
  Trigger ();
  //Set (0x00, 0x00);
}

void cFilterEEPG::Trigger (void)
{
  LogI(3, prep("trigger\n"));
  pmtpid = 0;
  pmtidx = 0;
  pmtnext = 0;
}

void cFilterEEPG::SetStatus (bool On)
{
  LogI(0, prep("setstatus %d\n"), On);
  if (!On) {
    FreeSummaries ();
    FreeTitles ();
    Format = MHW1;
    ChannelsOk = false;
    NumberOfTables = 0;
  } else {
    //Set(0x00,0x00);
    for (int i = 0; i <= HIGHEST_FORMAT; i++)
      UnprocessedFormat[i] = 0; //pid 0 is assumed to be nonvalid for EEPG transfers
    AddFilter (0, 0);
  }
  cFilter::SetStatus (On);
  Trigger ();
}

void cFilterEEPG::NextPmt (void)
{
  Del (pmtpid, SI::TableIdPMT);
  pmtpid = 0;
  pmtidx++;
  LogE(3, prep("PMT next\n"));
}



// -------------------  Freesat -------------------

/* FreeSat Huffman decoder for VDR
 *
 * Insert GPL licence
 */

/* The following features can be controlled:
 *
 * FREEVIEW_NO_SYSLOG             - Disable use of isyslog
 */

#ifndef FREEVIEW_NO_SYSLOG
#include <vdr/tools.h>
/* Logging via vdr */
#ifndef isyslog
#define isyslog(a...) void( (SysLogLevel > 1) ? syslog_with_tid(LOG_INFO,  a) : void() )
#endif
void syslog_with_tid (int priority, const char *format, ...) __attribute__ ((format (printf, 2, 3)));
#else
#define isyslog(a...)  fprintf(stderr,a)
#endif

static sNodeH* sky_tables[2];

/** \brief Convert a textual character description into a value
 *
 *  \param str - Encoded (in someway) string
 *
 *  \return Raw character
 */
static unsigned char resolve_char (char *str)
{
  int val;
  if (strcmp (str, "ESCAPE") == 0) {
    return ESCAPE;
  } else if (strcmp (str, "STOP") == 0) {
    return STOP;
  } else if (strcmp (str, "START") == 0) {
    return START;
  } else if (sscanf (str, "0x%02x", &val) == 1) {
    return val;
  }
  return str[0];


}


/** \brief Decode a binary string into a value
 *
 *  \param binary - Binary string to decode
 *
 *  \return Decoded value
 */
static unsigned long decode_binary (char *binary)
{
  unsigned long mask = 0x80000000;
  unsigned long maskval = 0;
  unsigned long val = 0;
  size_t i;

  for (i = 0; i < strlen (binary); i++) {
    if (binary[i] == '1') {
      val |= mask;
    }
    maskval |= mask;
    mask >>= 1;
  }
  return val;
}

/** \brief Load an individual freesat data file
 *
 *  \param tableid   - Table id that should be loaded
 *  \param filename  - Filename to load
 *  \return Success of operation
 */
static bool load_freesat_file (int tableid, const char *filename)
{
  char buf[1024];
  char *from, *to, *binary;
  FILE *fp;

  tableid--;
  if ((fp = fopen (filename, "r")) != NULL) {
    LogI(2, prep("Loading table %d Filename <%s>"), tableid + 1, filename);

    while (fgets (buf, sizeof (buf), fp) != NULL) {
      from = binary = to = NULL;
      int elems = sscanf (buf, "%a[^:]:%a[^:]:%a[^:]:", &from, &binary, &to);
      if (elems == 3) {
        int bin_len = strlen (binary);
        int from_char = resolve_char (from);
        char to_char = resolve_char (to);
        unsigned long bin = decode_binary (binary);
        int i = table_size[tableid][from_char]++;

        tables[tableid][from_char] =
          (struct hufftab *) REALLOC (tables[tableid][from_char], (i + 1) * sizeof (tables[tableid][from_char][0]));
        tables[tableid][from_char][i].value = bin;
        tables[tableid][from_char][i].next = to_char;
        tables[tableid][from_char][i].bits = bin_len;
        /* char from; unsigned int value; short bits; char next; */
        LogI(2, prep("%02x;%08x;%04x;%02x"), from_char, bin, bin_len, to_char);

        free (from);
        free (to);
        free (binary);
      }
    }
    fclose (fp);
  } else {
    LogE(0, prep("Cannot load <%s> for table %d"), filename, tableid + 1);
    return false;
  }
  return true;
}

/** \brief Load an individual sky data file
 *
 *  \param filename  - Filename to load
 *  \return Success of operation
 */
bool cFilterEEPG::load_sky_file (const char *filename)
{
  FILE *FileDict;
  char *Line;
  char Buffer[256];
  sNodeH *nH;
  int tableId;

  FileDict = fopen (filename, "r");
  if (FileDict == NULL) {
    LogE (0, prep("Error opening file '%s'. %s"), filename, strerror (errno));
    return false;
  } else {
    int i;
    int LenPrefix;
    char string1[256];
    char string2[256];

    tableId = Format == SKY_IT ? 0 : 1;
    if (!sky_tables[tableId]) {
      sky_tables[tableId] = (sNodeH*) calloc(1,sizeof(sNodeH));
      if (!sky_tables[tableId]) {
        LogE (0, prep("Not enough memory to load file '%s'."), filename);
        return false;
      }
    }

    while ((Line = fgets (Buffer, sizeof (Buffer), FileDict)) != NULL) {
      if (!isempty (Line)) {
        memset (string1, 0, sizeof (string1));
        memset (string2, 0, sizeof (string2));
        if (sscanf (Line, "%c=%[^\n]\n", string1, string2) == 2
          || (sscanf (Line, "%[^=]=%[^\n]\n", string1, string2) == 2)) {
          nH = sky_tables[tableId];
          LenPrefix = strlen (string2);
          for (i = 0; i < LenPrefix; i++) {
            switch (string2[i]) {
            case '0':
              if (nH->P0 == NULL) {
                nH->P0 = new sNodeH ();
                nH = nH->P0;
                nH->Value = NULL;
                nH->P0 = NULL;
                nH->P1 = NULL;
                if ((LenPrefix - 1) == i) {
                  Asprintf (&nH->Value, "%s", string1);
                }
              } else {
                nH = nH->P0;
                if (nH->Value != NULL || (LenPrefix - 1) == i) {
                  LogE (0 ,prep("Error, huffman prefix code already exists for \"%s\"=%s with '%s'"), string1,
                    string2, nH->Value);
                }
              }
              break;
            case '1':
              if (nH->P1 == NULL) {
                nH->P1 = new sNodeH ();
                nH = nH->P1;
                nH->Value = NULL;
                nH->P0 = NULL;
                nH->P1 = NULL;
                if ((LenPrefix - 1) == i) {
                  Asprintf (&nH->Value, "%s", string1);
                }
              } else {
                nH = nH->P1;
                if (nH->Value != NULL || (LenPrefix - 1) == i) {
                  LogE (0, prep("Error, huffman prefix code already exists for \"%s\"=%s with '%s'"), string1,
                    string2, nH->Value);
                }
              }
              break;
            default:
              break;
            }
          }
        }
      }
    }
    fclose (FileDict);
  }

  // check tree huffman nodes
  FileDict = fopen (filename, "r");
  if (FileDict) {
    int i;
    int LenPrefix;
    char string1[256];
    char string2[256];
    while ((Line = fgets (Buffer, sizeof (Buffer), FileDict)) != NULL) {
      if (!isempty (Line)) {
        memset (string1, 0, sizeof (string1));
        memset (string2, 0, sizeof (string2));
        if (sscanf (Line, "%c=%[^\n]\n", string1, string2) == 2
          || (sscanf (Line, "%[^=]=%[^\n]\n", string1, string2) == 2)) {
          nH = sky_tables[tableId];
          LenPrefix = strlen (string2);
          for (i = 0; i < LenPrefix; i++) {
            switch (string2[i]) {
            case '0':
              if (nH->P0 != NULL) {
                nH = nH->P0;
              }
              break;
            case '1':
              if (nH->P1 != NULL) {
                nH = nH->P1;
              }
              break;
            default:
              break;
            }
          }
          if (nH->Value != NULL) {
            if (memcmp (nH->Value, string1, strlen (nH->Value)) != 0) {
              LogE (0, prep("Error, huffman prefix value '%s' not equal to '%s'"), nH->Value, string1);
            }
          } else {
            LogE (0, prep("Error, huffman prefix value is not exists for \"%s\"=%s"), string1, string2);
          }
        }
      }
    }
    fclose (FileDict);
  }
  return true;
}

#define THEME_TR ".tr"

void load_theme_dictionaries (void)
{
  char Buffer[1024];
  char *Line;
  FILE *File;

  DIR *dp = opendir(cSetupEEPG::getInstance()->getConfDir());
  struct dirent *dirp;
  if(!dp) {
    LogE (0, prep("Can't read configuration folder '%s'"), cSetupEEPG::getInstance()->getConfDir());
    return ;
  }

  if (tableDict.size() > 0)
    tableDict.clear();


  while ((dirp = readdir(dp)) != NULL) {

    string fname = dirp->d_name;      // filename
    if (dirp->d_type == DT_DIR ||  // if entry is a directory
        fname.find(THEME_TR, (fname.length() - strlen(THEME_TR))) == string::npos){
      continue;
    }

    fname = string(cSetupEEPG::getInstance()->getConfDir()) + "/" + fname;

    //Test if file is changed and reload
    struct stat st;
    if (stat(fname.c_str(), &st)) {
        LogE(0, prep("Error obtaining stats for '%s' "), fname.c_str());
        continue;
    }

    File = fopen (fname.c_str(), "r");
    if (!File) continue;

    memset (Buffer, 0, sizeof (Buffer));
    char origThemeName[256];
    char transThemeName[256];
    while ((Line = fgets (Buffer, sizeof (Buffer), File)) != NULL) {
      Line = compactspace (skipspace (stripspace (Line)));
      //Skip empty and commented lines
      if (isempty (Line) || Line[0] == '#' || Line[0] == ';') continue;
      if (sscanf (Line, "%[^=]=%[^\n]\n", origThemeName, transThemeName) == 2) {

        string origTh(compactspace (skipspace (stripspace (origThemeName))));
        string transTh(compactspace (skipspace (stripspace (transThemeName))));
        if (!tableDict.count(origTh) && !transTh.empty()) {
          tableDict.insert(pair<string,string>(string(origThemeName),string(transThemeName)));
          LogD(4, prep("Original '%s' translation to '%s'."), origTh.c_str(), transTh.c_str());
        }
      }     //if scanf
    }      //while
    fclose (File);
    LogD(3, prep("Loaded %i translations from %s."), tableDict.size(), fname.c_str());
  }
  closedir(dp);

  LogD(2, prep("Loaded %i translations."), tableDict.size());
  LogD(2, prep("Original <-> Translation"));
  map<string,string>::iterator it;
  for ( it=tableDict.begin() ; it != tableDict.end(); it++ )
    LogD(2, prep("%s <-> %s"), (*it).first.c_str(), it->second.c_str());

}

/** \brief Decode an EPG string as necessary
 *
 *  \param src - Possibly encoded string
 *  \param size - Size of the buffer
 *
 *  \retval NULL - Can't decode
 *  \return A decoded string
 */
char *freesat_huffman_decode (const unsigned char *src, size_t size)
{
  int tableid;
//  freesat_decode_error = 0;

  if (src[0] == 0x1f && (src[1] == 1 || src[1] == 2)) {
    int uncompressed_len = 30;
    char *uncompressed = (char *) calloc (1, uncompressed_len + 1);
    unsigned value = 0, byte = 2, bit = 0;
    int p = 0;
    unsigned char lastch = START;

    tableid = src[1] - 1;
    while (byte < 6 && byte < size) {
      value |= src[byte] << ((5 - byte) * 8);
      byte++;
    }
    //freesat_table_load ();    /**< Load the tables as necessary */

    do {
      bool found = false;
      unsigned bitShift = 0;
      if (lastch == ESCAPE) {
        char nextCh = (value >> 24) & 0xff;
        found = true;
        // Encoded in the next 8 bits.
        // Terminated by the first ASCII character.
        bitShift = 8;
        if ((nextCh & 0x80) == 0)
          lastch = nextCh;
        if (p >= uncompressed_len) {
          uncompressed_len += 10;
          uncompressed = (char *) REALLOC (uncompressed, uncompressed_len + 1);
        }
        uncompressed[p++] = nextCh;
        uncompressed[p] = 0;
      } else {
        int j;
        for (j = 0; j < table_size[tableid][lastch]; j++) {
          unsigned mask = 0, maskbit = 0x80000000;
          short kk;
          for (kk = 0; kk < tables[tableid][lastch][j].bits; kk++) {
            mask |= maskbit;
            maskbit >>= 1;
          }
          if ((value & mask) == tables[tableid][lastch][j].value) {
            char nextCh = tables[tableid][lastch][j].next;
            bitShift = tables[tableid][lastch][j].bits;
            if (nextCh != STOP && nextCh != ESCAPE) {
              if (p >= uncompressed_len) {
                uncompressed_len += 10;
                uncompressed = (char *) REALLOC (uncompressed, uncompressed_len + 1);
              }
              uncompressed[p++] = nextCh;
              uncompressed[p] = 0;
            }
            found = true;
            lastch = nextCh;
            break;
          }
        }
      }
      if (found) {
        // Shift up by the number of bits.
        unsigned b;
        for (b = 0; b < bitShift; b++) {
          value = (value << 1) & 0xfffffffe;
          if (byte < size)
            value |= (src[byte] >> (7 - bit)) & 1;
          if (bit == 7) {
            bit = 0;
            byte++;
          } else
            bit++;
        }
      } else {
        LogE (0, prep("Missing table %d entry: <%s>"), tableid + 1, uncompressed);
        // Entry missing in table.
        return uncompressed;
      }
    } while (lastch != STOP && value != 0);

    return uncompressed;
  }
  return NULL;
}

int cFilterEEPG::sky_huffman_decode (const u_char * Data, int Length, unsigned char *DecodeText)
{
  sNodeH *nH, H=(Format==SKY_IT)?*sky_tables[0]:*sky_tables[1];
  int i;
  int p;
  int q;
  bool CodeError;
  bool IsFound;
  unsigned char Byte;
  unsigned char lastByte;
  unsigned char Mask;
  unsigned char lastMask;
  nH = &H;
  p = 0;
  q = 0;
  DecodeText[0] = '\0';
  //DecodeErrorText[0] = '\0';
  CodeError = false;
  IsFound = false;
  lastByte = 0;
  lastMask = 0;
  for (i = 0; i < Length; i++) {
    Byte = Data[i];
    Mask = 0x80;
    if (i == 0) {
      Mask = 0x20;
      lastByte = i;
      lastMask = Mask;
    }
loop1:
    if (IsFound) {
      lastByte = i;
      lastMask = Mask;
      IsFound = false;
    }
    if ((Byte & Mask) == 0) {
      if (CodeError) {
        //DecodeErrorText[q] = 0x30;
        q++;
        goto nextloop1;
      }
      if (nH->P0 != NULL) {
        nH = nH->P0;
        if (nH->Value != NULL) {
          memcpy (&DecodeText[p], nH->Value, strlen (nH->Value));
          p += strlen (nH->Value);
          nH = &H;
          IsFound = true;
        }
      } else {
        memcpy (&DecodeText[p], "<...?...>", 9);
        p += 9;
        i = lastByte;
        Byte = Data[lastByte];
        Mask = lastMask;
        CodeError = true;
        goto loop1;
      }
    } else {
      if (CodeError) {
        //DecodeErrorText[q] = 0x31;
        q++;
        goto nextloop1;
      }
      if (nH->P1 != NULL) {
        nH = nH->P1;
        if (nH->Value != NULL) {
          memcpy (&DecodeText[p], nH->Value, strlen (nH->Value));
          p += strlen (nH->Value);
          nH = &H;
          IsFound = true;
        }
      } else {
        memcpy (&DecodeText[p], "<...?...>", 9);
        p += 9;
        i = lastByte;
        Byte = Data[lastByte];
        Mask = lastMask;
        CodeError = true;
        goto loop1;
      }
    }
nextloop1:
    Mask = Mask >> 1;
    if (Mask > 0) {
      goto loop1;
    }
  }
  DecodeText[p] = '\0';
  //DecodeErrorText[q] = '\0';
  return p;
}


bool cFilterEEPG::GetThemesSKYBOX (void) //TODO can't we read this from the DVB stream?
{
  string FileName = cSetupEEPG::getInstance()->getConfDir();
  FILE *FileThemes;
  char *Line;
  char Buffer[256];
  const char **SkyThemes;
  bool updateFile = false;
  if (Format == SKY_IT) {
    FileName += "/sky_it.themes";
    SkyThemes = SkyItThemes;
  }
  else if (Format == SKY_UK) {
    FileName += "/sky_uk.themes";
    SkyThemes = SkyUkThemes;
  }
  else {
    LogE (0, prep("Error, wrong format detected in GetThemesSKYBOX. Format = %i."), Format);
    return false;
  }
  //asprintf( &FileName, "%s/%s", ConfDir, ( lProviders + CurrentProvider )->Parm3 );
  FileThemes = fopen (FileName.c_str(), "r");
  if (FileThemes == NULL) {
    LogE (0, prep("Error opening file '%s'. %s"), FileName.c_str(), strerror (errno));
    return false;
  } else {
    int id = 0;
    nThemes = 0;
    char thId[256];
    char theme[256];

    while ((Line = fgets (Buffer, sizeof (Buffer), FileThemes)) != NULL) {
      memset (thId, 0, sizeof (thId));
      memset (theme, 0, sizeof (theme));
      if (!isempty (Line)) {
        if (sscanf (Line, "%[^=] =%[^\n] ", thId, theme) == 2 && !isempty (theme)) {
          snprintf ((char *) Themes[id], 255, "%s", theme);
          nThemes++;
        } else {
          if (SkyThemes[id]) {
            updateFile = true;
            snprintf ((char *) Themes[id], 255, "%s", SkyThemes[id]);
            LogD (1, prep("Theme '%s' missing in theme file '%s'"), SkyThemes[id], FileName.c_str());
          } else
              Themes[id][0] = '\0';
        }
        id ++;
      }
    }
    fclose (FileThemes);

    if (updateFile) {
      FileThemes = fopen (FileName.c_str(), "w");
      if (FileThemes == NULL) {
        LogE (0, prep("Error re-creating file '%s', %s"), FileName.c_str(), strerror (errno));
      } else {
        for (int i = 0; i < 256; i++) {
          if (Themes[i]) {
            fprintf (FileThemes, "0x%02x=%s\n", i, (char *) Themes[i]);
          }
          else {
            fprintf (FileThemes, "0x%02x=\n", i);
          }
        }

        LogI (0, prep("Success updating file '%s'"), FileName.c_str());
        fclose (FileThemes);
      }
    }
  }
  return true;
}

/**
 * \brief Initialize the Huffman dictionaries if they are not already initialized.
 *
 */
static cMutex InitDictionary_mutex;

bool cFilterEEPG::InitDictionary (void)
{
  // This function must be serialised because it updates sky_tables and tables
  // which are both globals.
  cMutexLock MutexLock(&InitDictionary_mutex);
  string FileName = cSetupEEPG::getInstance()->getConfDir();
  switch (Format) {
  case SKY_IT:
    if (sky_tables[0] == NULL) {
      FileName += "/sky_it.dict";
      LogD (4, prep("EEPGDebug: loading sky_it.dict"));
      return load_sky_file(FileName.c_str());
    } else
        LogD (4, prep("EEPGDebug: sky_it.dict already loaded"));
    break;
  case SKY_UK:
    if (sky_tables[1] == NULL) {
      FileName += "/sky_uk.dict";
      LogD (4, prep("EEPGDebug: loading sky_uk.dict"));
      return load_sky_file(FileName.c_str());
    } else
        LogD (4, prep("EEPGDebug: sky_uk.dict already loaded"));
    break;
  case FREEVIEW:
    if (tables[0][0] == NULL) {
      LogD (4, prep("EEPGDebug: loading freesat.dict"));
      FileName += "/freesat.t1";
      if (!load_freesat_file (1, FileName.c_str()))
        return false;
      FileName = cSetupEEPG::getInstance()->getConfDir();
      FileName += "/freesat.t2";
      return load_freesat_file (2, FileName.c_str());
    } else
        LogD (4, prep("EEPGDebug: freesat.dict already loaded"));
    break;
  default:
    LogE (0 ,prep("Error, wrong format detected in ReadFileDictionary. Format = %i."), Format);
    return false;
  }
  return true;
}

/**
 * \brief Get MHW channels
 *
 * \return 0 = fatal error, code 1 = success, code 2 = last item processed
 */
int cFilterEEPG::GetChannelsMHW (const u_char * Data, int Length)
{
  if (Format != MHW1 && Format != MHW2) return 0;

  if ((Format == MHW1) || (nChannels == 0)) { //prevents MHW2 from reading channels twice while waiting for themes on same filter
    sChannelMHW1 *Channel;
    int Size, Off;
    Size = sizeof (sChannelMHW1);
    Off = 4;
    if (Format == MHW1) {
      //Channel = (sChannelMHW1 *) (Data + 4);
      nChannels = (Length - Off) / sizeof (sChannelMHW1);
    }
    if (Format == MHW2) {
      if (Length > 120)
        nChannels = Data[120];
      else {
        LogE(0, prep("Error, channels packet too short for MHW2."));
        return 0;
      }
      int pName = ((nChannels * 8) + 121);
      if (Length > pName) {
        //Channel = (sChannelMHW1 *) (Data + 120);
        Size -= 14; //MHW2 is 14 bytes shorter
        Off = 121;  //and offset differs
      } else {
        LogE(0, prep("Error, channels length does not match pname."));
        return 0;
      }
    }

    if (nChannels > MAX_CHANNELS) {
      LogE(0, prep("EEPG: Error, %i channels found more than %i"), nChannels, MAX_CHANNELS);
      return 0;
    } else {
      LogI(1, "|  ID  | %-26.26s | %-22.22s | FND | %-8.8s |\n", "Channel ID", "Channel Name", "Sky Num.");
      LogI(1, "|------|-%-26.26s-|-%-22.22s-|-----|-%-8.8s-|\n", "------------------------------",
           "-----------------------------", "--------------------");
      int pName = ((nChannels * 8) + 121); //TODO double ...
      LogD(1, prep("Length:%d pName:%d diff:%d"), Length, pName, Length - pName);
      for (int i = 0; i < nChannels; i++) {
        Channel = (sChannelMHW1 *) (Data + Off);
        sChannel *C = &sChannels[i];
        C->ChannelId = i;
        ChannelSeq[C->ChannelId] = i; //fill lookup table to go from channel-id to sequence nr in table
        C->SkyNumber = 0;
        if (Format == MHW1)
          memcpy (C->Name, &Channel->Name, 16); //MHW1
        else {   //MHW2
          int lenName = Data[pName] & 0x0f;
          //LogD (1, prep("EEPGDebug: MHW2 lenName:%d"), lenName);
          decodeText2(&Data[pName+1],lenName,(char*)C->Name,256);
          //memcpy (C->Name, &Data[pName + 1], lenName);
          //else
          //memcpy (C->Name, &Data[pName + 1], 256);
          pName += (lenName + 1);
        }
        //C->NumberOfEquivalences = 1; //there is always an original channel. every equivalence adds 1
        C->Src = Source (); //assume all EPG channels are on same satellite, if not, manage this via equivalents!!!
        C->Nid = HILO16 (Channel->NetworkId);
        C->Tid = HILO16 (Channel->TransportId);
        C->Sid = HILO16 (Channel->ServiceId);
        tChannelID channelID = tChannelID (C->Src, C->Nid, C->Tid, C->Sid);
        const cChannel *VC = GetChannelByID(channelID, true);
        bool IsFound = (VC);
        if(IsFound) {
          C->Src = VC->Source();
        }
        CleanString (C->Name);

        LogI(1, "|% 5d | %-26.26s | %-22.22s | %-3.3s |  % 6d  |\n", C->ChannelId
             , *tChannelID (C->Src, C->Nid, C->Tid, C->Sid).ToString()
             , C->Name, IsFound ? "YES" : "NO", C->SkyNumber);

        Off += Size;
      } //for loop
    }   //else nChannels > MAX_CHANNELS
    //LoadEquivalentChannels ();
    EquivHandler->loadEquivalentChannelMap();
    GetLocalTimeOffset (); //reread timing variables, only used for MHW
    return 2; //obviously, when you get here, channels are read successfully, but since all channels are sent at once, you can stop now
  } //if nChannels == 0
  LogE (0, prep("Warning: Trying to read Channels more than once!"));
  //you will only get here when GetChannelsMHW is called, and nChannels !=0, e.g. when multiple citpids cause channels to be read multiple times. Second time, do nothing, give error so that the rest of the chain is not restarted also.
  return 0;
}

/**
 * \brief Get MHW1 Themes
 *
 * \return 0 = fatal error, code 1 = success, code 2 = last item processed
 */
int cFilterEEPG::GetThemesMHW1 (const u_char * Data, int Length)
{ //MHW1 Themes
  if (Length > 19) {
    sThemeMHW1 *Theme = (sThemeMHW1 *) (Data + 19);
    nThemes = (Length - 19) / 15;
    if (nThemes > MAX_THEMES) {
      LogE(1, prep("Error, %i themes found more than %i"), nThemes, MAX_THEMES);
      return 0;
    } else {
      LogI(1, "-------------THEMES FOUND--------------");
      int ThemeId = 0;
      int Offset = 0;
      const u_char *ThemesIndex = (Data + 3);
      for (int i = 0; i < nThemes; i++) {
        if (ThemesIndex[ThemeId] == i) {
          //The index of the Themes in MHW1 is not incremented by 1 but with offset calculated like this
          Offset = (Offset + 15) & 0xf0; 
          ThemeId++;
        }
        memcpy (&Themes[Offset][0], &Theme->Name, 15);
        Themes[Offset][15] = '\0'; //trailing null
        CleanString (Themes[Offset]);

        //reformat themes
        if (Offset & 0x0f) {
          char* theme;
          char* cat;
          Asprintf (&theme, "%s", Themes[Offset]);
          Asprintf (&cat, "%s", Themes[Offset& 0xf0]);
          theme = strreplace(theme,"DOC","DOCUMENTAIRE");
          theme = strreplace(theme,"DOCUMENTAIRE.","DOCUMENTAIRE");
          theme = strreplace(theme,"MAG","MAGAZINE");
          theme = strreplace(theme,"INFO-METEO","INFO/METEO");
          //Remove category from genre. TODO Maybe they should be separated in the future
          theme = strreplace(theme,cat,"");
          theme = skipspace(theme);
          if (theme[0] == '-') {
             memmove(theme, theme+1, strlen(theme));
             theme = skipspace(theme);
          }
          char* buf;
          Asprintf(&buf,"%s - %s",cat,theme);
          memcpy(Themes[Offset],buf,strlen(buf)+1);
        }

        LogI(1, prep("%-33.33s|Offset:0x%02x"), Themes[Offset],Offset);

        Offset++;
        Theme++;
      }
      if ((nThemes * 15) + 19 != Length) {
        LogE(0, "Themes error: buffer is smaller or bigger than sum of entries.");
        return 0;
      } else
        return 2;
    }
  }
  return 1;
}

/**
 * \brief Get MHW2 Themes
 *
 * \return 0 = fatal error, code 1 = success, code 2 = last item processed
 */
int cFilterEEPG::GetThemesMHW2 (const u_char * Data, int Length)
{
  if (!EndThemes) { //only process if not processed
    int p1;
    int p2;
    int pThemeName = 0;
    int pSubThemeName = 0;
    int lenThemeName = 0;
    int lenSubThemeName = 0;
    int pThemeId = 0;
    if (Length > 4) {
      LogI(1, "-------------THEMES FOUND--------------");
      for (int i = 0; i < Data[4]; i++) {
        p1 = ((Data[5 + i * 2] << 8) | Data[6 + i * 2]) + 3;
        if (Length > p1) {
          for (int ii = 0; ii <= (Data[p1] & 0x3f); ii++) {
            p2 = ((Data[p1 + 1 + ii * 2] << 8) | Data[p1 + 2 + ii * 2]) + 3;
            if (Length > p2) {
              if (ii == 0) {
                pThemeName = p2 + 1;
                lenThemeName = Data[p2] & 0x1f;
                lenSubThemeName = 0;
              } else {
                pSubThemeName = p2 + 1;
                lenSubThemeName = Data[p2] & 0x1f;
              }
              if (Length >= (pThemeName + lenThemeName)) {
                pThemeId = ((i & 0x3f) << 6) | (ii & 0x3f);
                if (pThemeId > MAX_THEMES) {
                  LogE(1, prep("Error, something wrong with themes id calculation MaxThemes: %i pThemeID:%d"), MAX_THEMES, pThemeId);
                  return 0; //fatal error
                }
                if ((lenThemeName + 2) < 256) {
                  decodeText2(&Data[pThemeName],lenThemeName,(char*)Themes[pThemeId],256);
                  //memcpy (Themes[pThemeId], &Data[pThemeName], lenThemeName);
                  if (Length >= (pSubThemeName + lenSubThemeName))
                    if (lenSubThemeName > 0)
                      if ((lenThemeName + lenSubThemeName + 2) < 256) {
                        Themes[pThemeId][lenThemeName] = '-';
                        decodeText2(&Data[pSubThemeName],lenSubThemeName,(char*)&Themes[pThemeId][lenThemeName + 1],256);
                        //memcpy (&Themes[pThemeId][lenThemeName + 1], &Data[pSubThemeName], lenSubThemeName);
                      }
                  CleanString (Themes[pThemeId]);
                  LogI(1, prep("%.40s|id:%d"), Themes[pThemeId],pThemeId);
                  //isyslog ("%.15s", (lThemes + pThemeId)->Name);
                  nThemes++;
                  if (nThemes > MAX_THEMES) {
                    LogE(1, prep("Error, %i themes found more than %i"), nThemes, MAX_THEMES);
                    return 0; //fatal error
                  }
                }
              } else
                return 1; //I assume non fatal error or success
            } else
              return 1;  //I assume non fatal error or success
          }
        } else
          return 1;  //I assume non fatal error or success
      }    //for loop
      //Del (Pid, Tid);
      EndThemes = true;
      return 2; //all themes read
    } //if length
  } //if !EndThemes
  return 1; //non fatal or success
}

/**
 * \brief Get Nagra Summary text
 *
 * \param TitleEventId EventId passed from title, to check it against the eventId of the summary
 * \return pointer to reserved part of memory with summary text in it, terminated by NULL
 */
char *cFilterEEPG::GetSummaryTextNagra (const u_char * DataStart, long int Offset, unsigned int TitleEventId)
//EventId is passed from title, to check it against the eventid of the summary
{
  u_char *p = (u_char *) DataStart + Offset;
  sSummaryDataNagraGuide *SD = (sSummaryDataNagraGuide *) p;

  if (TitleEventId != HILO32 (SD->EventId)) {
    LogI(0, prep("ERROR, Title has EventId %08x and points to Summary with EventId %08x."), TitleEventId,
      HILO32 (SD->EventId));
    return 0; //return empty string
  }

  if (CheckLevel(3)) {
    if (SD->AlwaysZero1 != 0)
      isyslog ("EEPGDEBUG: SummAlwaysZero1 is NOT ZERO:%x.", SD->AlwaysZero1);
    if (SD->AlwaysZero2 != 0)
      isyslog ("EEPGDEBUG: SummAlwaysZero2 is NOT ZERO:%x.", SD->AlwaysZero2);
    if (SD->AlwaysZero3 != 0)
      isyslog ("EEPGDEBUG: SummAlwaysZero3 is NOT ZERO:%x.", SD->AlwaysZero3);
    if (SD->AlwaysZero4 != 0)
      isyslog ("EEPGDEBUG: SummAlwaysZero4 is NOT ZERO:%x.", SD->AlwaysZero4);
    if (SD->AlwaysZero5 != 0)
      isyslog ("EEPGDEBUG: SummAlwaysZero5 is NOT ZERO:%x.", SD->AlwaysZero5);
    if (SD->AlwaysZero6 != 0)
      isyslog ("EEPGDEBUG: SummAlwaysZero6 is NOT ZERO:%x.", SD->AlwaysZero6);
    if (SD->AlwaysZero7 != 0)
      isyslog ("EEPGDEBUG: SummAlwaysZero7 is NOT ZERO:%x.", SD->AlwaysZero7);
    if (SD->AlwaysZero8 != 0)
      isyslog ("EEPGDEBUG: SummAlwaysZero8 is NOT ZERO:%x.", SD->AlwaysZero8);
    if (SD->AlwaysZero9 != 0)
      isyslog ("EEPGDEBUG: SummAlwaysZero9 is NOT ZERO:%x.", SD->AlwaysZero9);

    if (SD->Always1 != 0x31) //1970
      isyslog ("EEPGDEBUG: SummAlways1:%02x.", SD->Always1);
    if (SD->Always9 != 0x39)
      isyslog ("EEPGDEBUG: SummAlways9:%02x.", SD->Always9);
    if (SD->Always7 != 0x37)
      isyslog ("EEPGDEBUG: SummAlways7:%02x.", SD->Always7);
    if (SD->Always0 != 0x30)
      isyslog ("EEPGDEBUG: SummAlways0:%02x.", SD->Always0);

    if (SD->Always0x01 != 0x01) // 0x01 byte
      isyslog ("EEPGDEBUG: Summ0x01 byte:%02x.", SD->Always0x01);
  }
  u_char *p2 = (u_char *) DataStart + HILO32 (SD->SummTxtOffset);
  /*    esyslog
        ("EEPGDEBUG: EventId %08x NumberOfBlocks %02x BlockId %08x SummTxtOffset %08x *p2: %02x Unkn1:%02x, Unkn2:%02x.",
         HILO32 (SD->EventId), SD->NumberOfBlocks, HILO32 (SD->BlockId), HILO32 (SD->SummTxtOffset), *p2, SD->Unknown1,
         SD->Unknown2);
   */
  unsigned char *Text = NULL; //makes first realloc work like malloc
  int TotLength = 0; //and also makes empty summaries if *p2 != 0x4e
  if (SD->NumberOfBlocks > 1) {
    switch (*p2) {
    case 0x4e: //valid summary text follows
    {
      bool LastTextBlock = false;

      do { //for all text parts
        sSummaryTextNagraGuide *ST = (sSummaryTextNagraGuide *) p2;
        p2 += 8;  //skip fixed block
        if (ST->AlwaysZero1 != 0)
          LogD(3, prep("DEBUG: ST AlwaysZero1 is NOT ZERO:%x."), ST->AlwaysZero1);
        if (ST->Always0x4e != 0x4e) {
          LogI(0, prep("DEBUG: ST Always0x4e is NOT 0x4e:%x."), ST->AlwaysZero1);
          return 0; //fatal error, empty text
        }
        LogI(5, prep("DEBUG: Textnr %i, Lasttxt %i."), ST->TextNr, ST->LastTextNr);
        int SummaryLength = ST->Textlength;

        Text = (unsigned char *) REALLOC (Text, SummaryLength + TotLength);
        if (Text == NULL) {
          LogI(0, prep("Summaries memory allocation error."));
          return 0; //empty text
        }
        memcpy (Text + TotLength, p2, SummaryLength); //append new textpart
        TotLength += SummaryLength;
        p2 += ST->Textlength; //skip text

        LastTextBlock = ((ST->LastTextNr == 0) || (ST->TextNr >= ST->LastTextNr));
      } while (!LastTextBlock);
      Text = (unsigned char *) REALLOC (Text, 1 + TotLength); //allocate 1 extra byte
      Text[TotLength] = '\0'; //terminate string by NULL char
      LogD(5, prep("DEBUG: Full Text:%s."), Text);

      break;
    }
    case 0x8c: //"Geen uitzending" "Geen informatie beschikbaar" e.d.
    {
      sSummaryGBRNagraGuide *GBR = (sSummaryGBRNagraGuide *) p2;

      p2 += 16; //skip fixed part, point to byte after Nextlength
      if (CheckLevel(3)) {
        if (GBR->AlwaysZero1 != 0)
          isyslog ("EEPGDEBUG: GBR AlwaysZero1 is NOT ZERO:%x.", GBR->AlwaysZero1);
        if (GBR->AlwaysZero2 != 0)
          isyslog ("EEPGDEBUG: GBR AlwaysZero2 is NOT ZERO:%x.", GBR->AlwaysZero2);
        if (GBR->AlwaysZero3 != 0)
          isyslog ("EEPGDEBUG: GBR AlwaysZero3 is NOT ZERO:%x.", GBR->AlwaysZero3);
        if (GBR->AlwaysZero4 != 0)
          isyslog ("EEPGDEBUG: GBR AlwaysZero4 is NOT ZERO:%x.", GBR->AlwaysZero4);

        isyslog ("EEPGDEBUG: Blocklength: %02x Data %02x %02x %02x %02x %02x %02x %02x %02x %02x", GBR->Blocklength,
                 GBR->Un1, GBR->Un2, GBR->Un3, GBR->Un4, GBR->Un5, GBR->Un6, GBR->Un7, GBR->Un8, GBR->Un9);
        for (int i = 0; i < GBR->Nextlength; i += 2)
          isyslog ("GBR Extradata %02x %02x.", *(p2 + i), *(p2 + i + 1));
      }
    }
    break;
    default:
      LogE(0, prep("ERROR *p2 has strange value: EventId %08x NumberOfBlocks %02x BlockId %08x SummTxtOffset %08x *p2: %02x Unkn1:%02x, Unkn2:%02x."),
        HILO32 (SD->EventId), SD->NumberOfBlocks, HILO32 (SD->BlockId), HILO32 (SD->SummTxtOffset), *p2,
        SD->Unknown1, SD->Unknown2);
      break;
    } //end of switch
  } //NrOfBlocks > 1

  if (TotLength == 0)
    Text = NULL;

  p += 29; //skip fixed part of block
  if (SD->NumberOfBlocks == 1)
    p -= 4; //in this case there is NO summary text AND no GBR??!!
  for (int i = 1; i < (SD->NumberOfBlocks - 1); i++) {
    LogD(3, prep("DEBUG: Extra Block info: %02x %02x %02x %02x."), *p, *(p + 1), *(p + 2), *(p + 3));
    p += 4; //skip this extra block info
  }
  return (char *) Text;
}


/**
 * \brief write event to schedule
 *
 * \param Duration the Duration of the event in minutes
 * \param ps points to array of schedules ps[eq], where eq is equivalence number of the channel. If channelId is invalid then ps[eq]=NULL
 */
void cFilterEEPG::WriteToSchedule(tChannelID channelID, cSchedules* pSchedules,
    unsigned int EventId, unsigned int StartTime, unsigned int Duration,
    char *Text, char *SummText, unsigned short int ThemeId,
    unsigned short int TableId, unsigned short int Version, char Rating,
    unsigned char ShTxtLen)
{
  bool WrittenTitle = false;
  bool WrittenSummary = false;
//  for (int eq = 0; eq < NumberOfEquivalences; eq++) {
  cSchedule* ps;
  if (channelID.Valid ()) //only add channels that are known to VDR
    ps = pSchedules->AddSchedule (channelID); //open or create new schedule
  else {
    ps = NULL;
  }

  cEvent *Event = NULL;
  if (ps/*[eq]*/) {

      Event = (cEvent *) ps->GetEvent (EventId); //since Nagra uses consistent EventIds, try this first
      bool TableIdMatches = false;
      if (Event)
        TableIdMatches = (Event->TableID() == TableId);
      if (!Event || !TableIdMatches || abs(Event->StartTime() - (time_t) StartTime) > Duration * 60) //if EventId does not match, or it matched with wrong TableId, then try with StartTime
        Event = (cEvent *) ps->GetEvent (EventId, StartTime);
  }
  cEvent *newEvent = NULL;
  if (!Event) {  //event is new
    Event = newEvent = new cEvent (EventId);
    Event->SetSeen ();
  } else if (Event->TableID() < TableId) { //existing table may not be overwritten
    RejectTableId++;
    //esyslog ("EEPGDEBUG: Rejecting Event, existing TableID:%x, new TableID:%x.", Event->TableID (),
    //           TableId);
    Event = NULL;
  }

  if (Event) {
    Event->SetEventID (EventId); //otherwise the summary cannot be added later
    Event->SetTableID (TableId); //TableID 0 is reserved for external epg, will not be overwritten; the lower the TableID, the more actual it is
    Event->SetVersion (Version); //TODO use version and tableID to decide whether to update; TODO add language code
    Event->SetStartTime (StartTime);
    Event->SetDuration (Duration * 60);
    if (Rating) {
      Event->SetParentalRating(Rating);
    }
    if (Text != 0x00) {
      WrittenTitle = true;
      CleanString ((uchar *) Text);
      Event->SetTitle (Text);
    }

    char* tmp = NULL;
    string shText;
    if (SummText && ShTxtLen) {
    
      shText.assign(SummText,ShTxtLen);
      //LogD(0, prep("DEBUG: Title '%s', Subtitle '%s'"), Text, shText.c_str());
      string tmpTitle(Text);
      if (Format == MHW2 && !shText.empty()) {
        size_t found = tmpTitle.find(" (HD)");
        if (found != string::npos)
          tmpTitle.erase(found, 5);
        //found = shText.compare(0, tmpTitle.size() + 2, string(tmpTitle + ": "));
        if (shText.compare(0, tmpTitle.size() + 2, string(tmpTitle + ": "))==0)
          shText.erase(0, tmpTitle.size() + 2);

        //Remove Subtitle from title it is not pretty.
        //int sumLen = strlen(SummText);
        //if (sumLen > ShTxtLen)
        //  memmove(SummText,SummText + ShTxtLen + 1, sumLen - ShTxtLen);
      }

      //Do not use Subtitle if it is substring of Title
      if (tmpTitle.compare(0, shText.size(), shText) == 0)
        shText.clear();

      //The subtitle is wrong if contains '.' after possible initial '...'
      if (Format == SKY_UK && !shText.empty() && shText.find('.',3) != string::npos) {
        shText.clear();
      }

#define MAX_USEFUL_EPISODE_LENGTH 40
      // From VDR FixEPG Bugs
      // Some channels put a whole lot of information in the ShortText and leave
      // the Description totally empty. So if the ShortText length exceeds
      // MAX_USEFUL_EPISODE_LENGTH, let's put this into the Description
      // instead:
      if (Format != SKY_IT && Format != MHW2 && !shText.empty() && shText.size() > MAX_USEFUL_EPISODE_LENGTH)
        shText.clear();

    }
    if (!shText.empty())
      Event->SetShortText (shText.c_str());
    else if (SetupPE->ReplaceEmptyShText) {
      Asprintf (&tmp, "%s - %d\'", Themes[ThemeId], Duration);
      Event->SetShortText (tmp);
      free(tmp);
    }

    if (SummText) {
      WrittenSummary = true;
      CleanString ((uchar *) SummText);

      //Fix MHW1 formating
      if (Format == MHW1) {
        char * pch;
        pch=strchr(SummText,'s');
        while (pch!=NULL) {
          if (*(pch-1) != ' ' && *(pch-1) != '\n')
            *pch = ' ';
          pch=strchr(pch+1,'s');
        }
      }

      //Add themes and categories epgsearch style
      //TODO DPE move this
      char *theme;
      Asprintf (&theme, "%s", Themes[ThemeId]);
      if (theme && 0 != strcmp(theme,"")) {
        char *category, *genre;
        category = NULL;
        genre = NULL;
        char *split = strchr(theme, '-'); // Look for '-' delim to separate category from genre
        if (split){
          *split = 0;
          category = theme;
          genre = (split[1] == 0x20) ? split + 2 : split + 1;
        }else{
          category = theme;
        }

        string desc = SummText;
        if (Format == MHW2 && ShTxtLen) {
          desc.erase(0, ShTxtLen + 1);
        }
        if (stripspace(category)) desc.append("\n").append(CATEGORY).append(findThemeTr(category));
        if (stripspace(genre)) desc += '\n' + string(GENRE) + findThemeTr(genre);
        Event->SetDescription (desc.c_str());

        free(theme);
      }
      else {
#ifdef DEBUG
        string desc = SummText;
        if (ThemeId) {
          char *theme;
          Asprintf (&theme, "0x%x", ThemeId);
          LogD(0, prep("DEBUG: missing theme ID 0x%x title:'%s'"), ThemeId, Event->Title());
          desc += '\n' + string(GENRE) + theme;
          free(theme);
        }
        Event->SetDescription (desc.c_str());
#else
        Event->SetDescription (SummText);
#endif
      }
    }
    if (ps && newEvent)
      ps->AddEvent (newEvent);

    EquivHandler->updateEquivalent(pSchedules, channelID, Event);

    if (!ps) {
    //the event is not send to VDR so it has to be deleted.
      delete Event;
      Event = NULL;
    }

    //newEvent->FixEpgBugs (); causes segfault
  }
      /*      else
            esyslog ("EEPG: ERROR, somehow not able to add/update event.");*///at this moment only reports RejectTableId events
  if (CheckLevel(4)) {
    isyslog ("EEPG: Title:%i, Summary:%i I would put into schedule:", TitleCounter, SummaryCounter);
    //isyslog ("C %s-%i-%i-%i\n", *cSource::ToString (C->Src[eq]), C->Nid[eq], C->Tid[eq], C->Sid[eq]);
    isyslog ("E %u %u %u 01 FF\n", EventId, StartTime, Duration * 60);
    isyslog ("T %s\n", Text);
    isyslog ("S %s - %d\'\n", Themes[ThemeId], Duration);
    isyslog ("D %s\n", SummText);
    isyslog ("e\nc\n.\n");
  }
//    } //if ps[eq]
//  } //for eq
  if (WrittenTitle)
    TitleCounter++;
  if (WrittenSummary)
    SummaryCounter++;
}

/**
 * \brief  Get Nagra titles
 *
 * \param TableIdExtension the TIE from the relevant summary sections!
 */
void cFilterEEPG::GetTitlesNagra (const u_char * Data, int Length, unsigned short TableIdExtension)
{
  u_char *p = (u_char *) Data;
  u_char *DataEnd = (u_char *) Data + Length;
  u_char *next_p;
  unsigned short int MonthdayTitles = ((TableIdExtension & 0x1ff) >> 4); //Day is coded in day of the month
  time_t timeLocal;
  struct tm *tmCurrent;

  timeLocal = time (NULL);
  tmCurrent = gmtime (&timeLocal); //gmtime gives UTC; only used for getting current year and current day of the month...
  unsigned short int CurrentMonthday = tmCurrent->tm_mday;
  unsigned short int CurrentYear = tmCurrent->tm_year;
  unsigned short int CurrentMonth = tmCurrent->tm_mon;
  //esyslog("EEPGDEBUG: CurrentMonthday=%i, TableIdExtension:%04x, MonthdayTitles=%i.",CurrentMonthday,TableIdExtension, MonthdayTitles);
#if APIVERSNUM >= 20300
  LOCK_SCHEDULES_WRITE;
#else
  cSchedulesLock SchedulesLock (true);
  cSchedules *Schedules = (cSchedules *) cSchedules::Schedules (SchedulesLock);
#endif
  do { //process each block of titles
    sTitleBlockNagraGuide *TB = (sTitleBlockNagraGuide *) p;
    int ChannelId = HILO16 (TB->ChannelId);
    int Blocklength = HILO16 (TB->Blocklength);
    long int NumberOfTitles = HILO32 (TB->NumberOfTitles);

    LogD(3, prep("DEBUG: ChannelId %04x, Blocklength %04x, NumberOfTitles %lu."), ChannelId, Blocklength,
         NumberOfTitles);
    p += 4; //skip ChannelId and Block length
    next_p = p + Blocklength;
    if (next_p > DataEnd) { //only process if block is complete
      LogE(0, prep("ERROR, Block exceeds end of Data. p:%p, Blocklength:%x,DataEnd:%p."), p, Blocklength, DataEnd);
      return; //fatal error, this should never happen
    }
    p += 4; //skip Title number

    sChannel *C = &sChannels[ChannelSeq[ChannelId]]; //find channel
    //cSchedule *ps = NULL;//[MAX_EQUIVALENCES];
    //PrepareToWriteToSchedule (C, Schedules, ps);

    for (int i = 0; i < NumberOfTitles; i++) { //process each title within block
      sTitleNagraGuide *Title = (sTitleNagraGuide *) p;
      unsigned int EventId = HILO32 (Title->EventId);

      unsigned int StartTime = Title->StartTimeHigh << 5 | Title->StartTimeLow;
      int Hours = (StartTime / 60);
      int Minutes = StartTime % 60;

      /*StartTime */
      tmCurrent->tm_year = CurrentYear;
      tmCurrent->tm_mon = CurrentMonth;
      tmCurrent->tm_mday = MonthdayTitles;
      tmCurrent->tm_hour = 0;
      tmCurrent->tm_min = StartTime; //if start time is bigger than 1 hour, mktime will correct this!
      tmCurrent->tm_sec = 0;
      tmCurrent->tm_isdst = -1; //now correct with daylight savings
      if (MonthdayTitles < CurrentMonthday - 7) //the titles that are older than one week are not from the past, but from next month!
        //at first this was set at -1 day (= yesterday), but sometimes providers send old data which then
        //end up in next months schedule ...
        tmCurrent->tm_mon++; //if a year border is passed, mktime will take care of this!
      StartTime = UTC2LocalTime (mktime (tmCurrent)); //VDR stores its times in UTC, but wants its input in local time...

      char *Text = NULL;
      u_char *t = (u_char *) Data + HILO32 (Title->OffsetToText);
      //u_char *t2 = (u_char *) Data + HILO32 (Title->OffsetToText2);
      if (t >= DataEnd)
        LogE(0, prep("ERROR, Title Text out of range: t:%p, DataEnd:%p, Data:%p, Length:%i."), t, DataEnd, Data,
          Length);
      else {
        Asprintf (&Text, "%.*s", *t, t + 1);
        //asprintf (&Text, "%.*s %.*s", *t, t + 1, *t2, t2 + 1); //FIXME second text string is not processed right now

        //now get summary texts
        u_char *DataStartSummaries = buffer[TableIdExtension] + 4;
        unsigned int DataLengthSummaries = bufsize[TableIdExtension] - 4;
        char *SummText = NULL;
        if (HILO32 (Title->SumDataOffset) >= DataLengthSummaries)
          LogE(0, prep("ERROR, SumDataOffset out of range: Title->SumDataOffset:%i, DataLengthSummaries:%i."), HILO32 (Title->SumDataOffset),DataLengthSummaries);
        else
          SummText = GetSummaryTextNagra (DataStartSummaries, HILO32 (Title->SumDataOffset), EventId);

        LogD(3, prep("DEBUG: Eventid: %08x ChannelId:%x, Start time %02i:%02i, Duration %i, OffsetToText:%08x, OffsetToText2:%08x, SumDataOffset:%08x ThemeId:%x Title:%s \n SummaryText:%s"),
             EventId, ChannelId, Hours, Minutes, Title->Duration,
             HILO32 (Title->OffsetToText), HILO32 (Title->OffsetToText2),
             HILO32 (Title->SumDataOffset), Title->ThemeId, Text, SummText);

        if (Themes[Title->ThemeId][0] == 0x00) //if detailed themeid is not known, get global themeid
          Title->ThemeId &= 0xf0;

        WriteToSchedule (tChannelID (C->Src, C->Nid, C->Tid, C->Sid), Schedules, EventId, StartTime, Title->Duration, Text, SummText,
                         Title->ThemeId, NAGRA_TABLE_ID, Version);

        if (Text != NULL)
          free (Text);
        Text = NULL;
        if (SummText != NULL)
          free (SummText);
        SummText = NULL;
      }

      if (CheckLevel(3)) {
        if (Title->AlwaysZero16 != 0)
          isyslog ("EEPGDEBUG: TitleAlwaysZero16 (3bits) is NOT ZERO:%x.", Title->AlwaysZero16);
        if (Title->AlwaysZero17 != 0)
          isyslog ("EEPGDEBUG: TitleAlwaysZero17 is NOT ZERO:%x.", Title->AlwaysZero17);
        if (Title->AlwaysZero1 != 0)
          isyslog ("EEPGDEBUG: TitleAlwaysZero1 is NOT ZERO:%x.", Title->AlwaysZero1);
        if (Title->AlwaysZero2 != 0)
          isyslog ("EEPGDEBUG: TitleAlwaysZero2 is NOT ZERO:%x.", Title->AlwaysZero2);
        if (Title->AlwaysZero3 != 0)
          isyslog ("EEPGDEBUG: TitleAlwaysZero3 is NOT ZERO:%x.", Title->AlwaysZero3);
        if (Title->AlwaysZero4 != 0)
          isyslog ("EEPGDEBUG: TitleAlwaysZero4 is NOT ZERO:%x.", Title->AlwaysZero4);
        if (Title->AlwaysZero5 != 0)
          isyslog ("EEPGDEBUG: TitleAlwaysZero5 is NOT ZERO:%x.", Title->AlwaysZero5);
        if (Title->AlwaysZero8 != 0)
          isyslog ("EEPGDEBUG: TitleAlwaysZero8 is NOT ZERO:%x.", Title->AlwaysZero8);
        if (Title->AlwaysZero9 != 0)
          isyslog ("EEPGDEBUG: TitleAlwaysZero9 is NOT ZERO:%x.", Title->AlwaysZero9);
        if (Title->AlwaysZero10 != 0)
          isyslog ("EEPGDEBUG: TitleAlwaysZero10 is NOT ZERO:%x.", Title->AlwaysZero10);
        if (Title->AlwaysZero11 != 0)
          isyslog ("EEPGDEBUG: TitleAlwaysZero11 is NOT ZERO:%x.", Title->AlwaysZero11);
      }
      p += 30; //next title
    } //end for titles

    //FinishWriteToSchedule (C, s, ps);
    sortSchedules(Schedules, tChannelID (C->Src, C->Nid, C->Tid, C->Sid));
    p = next_p;
  } while (p < DataEnd); //end of TitleBlock
}

int cFilterEEPG::GetThemesNagra (const u_char * Data, int Length, unsigned short TableIdExtension) //return code 0 = fatal error, code 1 = success, code 2 = last item processed
{
  u_char *DataStart = (u_char *) Data;
  u_char *p = DataStart; //TODO Language code terminated by 0 is ignored
  u_char *DataEnd = DataStart + Length;
  u_char *DataStartTitles = buffer[TableIdExtension] + 4;
  u_char *DataEndTitles = DataStartTitles + bufsize[TableIdExtension] - 4;
  if (Length == 0) {
    LogI(1, prep("NO THEMES FOUND"));
    return 2;
  }

  int NumberOfThemes = (*p << 24) | *(p + 1) << 16 | *(p + 2) << 8 | *(p + 3);
  p += 4; //skip number of themes block
  //isyslog ("EEPG: found %i themes.", NumberOfThemes);

  if ((nThemes == 0))
    LogI(1, "-------------THEMES FOUND--------------");

  for (int i = 0; i < NumberOfThemes; i++) {
    int Textlength = *p;
    p++; //skip textlength byte
    u_char *Text = p;
    u_char ThemeId = 0;
    p += Textlength; //skip text
    int NrOfBlocks = (*p << 8) | *(p + 1);
    p += 2; //skip nrofblocks
    bool AnyDoubt = false;
    u_char *p2 = p;
    p += NrOfBlocks * 8;
    for (int j = 0; j < NrOfBlocks; j++) {
      sThemesTitlesNagraGuide *TT = (sThemesTitlesNagraGuide *) p2;
      p2 += 8; //skip block
      u_char *NewThemeId = DataStartTitles + HILO32 (TT->TitleOffset) + 28;
      if (NewThemeId >= DataEndTitles)
        LogE(0, prep("ERROR, ThemeId out of range: NewThemeId:%p, DataEndTitles:%p, DataStartTitles:%p."), NewThemeId,
          DataEndTitles, DataStartTitles);
      else {
        //esyslog("EEPGDEBUG: NewThemeId:%02x, Text:%s.",*NewThemeId, Text);
        if (Themes[*NewThemeId][0] != 0x00) { //theme is already filled, break off
          AnyDoubt = true;
          break;
        }
        if (j == 0) //first block
          ThemeId = *NewThemeId;
        else if (ThemeId != *NewThemeId) { //different theme ids in block
          if ((ThemeId & 0xf0) != (*NewThemeId & 0xf0)) { //major nible of themeid does not correspond
            LogE(3, prep("ERROR, Theme has multiple indices which differ in major nibble, old index = %x, new index = %x. Ignoring both indices."),
              ThemeId, *NewThemeId);
            AnyDoubt = true;
            break;
          } else if ((ThemeId & 0x0f) != 0) //ThemeId is like 1a, 2a, not like 10,20. So it is minor in tree-structure, and it should be labeled in major part of tree
            ThemeId = *NewThemeId; //lets hope new themeid is major, if not, it has not worsened....
        }
      }    //else NewThemeId >= DataEndTitles
      if (CheckLevel(3)) {
        if (TT->Always1 != 1)
          isyslog ("EEPGDEBUG: TT Always1 is NOT 1:%x.", TT->Always1);
        if (TT->AlwaysZero1 != 0)
          isyslog ("EEPGDEBUG: TT AlwaysZero1 is NOT ZERO:%x.", TT->AlwaysZero1);
        if (TT->AlwaysZero2 != 0)
          isyslog ("EEPGDEBUG: TT AlwaysZero2 is NOT ZERO:%x.", TT->AlwaysZero2);
      }
    } //for nrofblocks
    // esyslog("EEPGDEBUG: AnyDoubt:%x.",AnyDoubt);
    if (!AnyDoubt) {
      if (Textlength > 63)
        Textlength = 63; //leave room for trailing NULL
      if (Themes[ThemeId][0] != 0) {
        LogE(0, prep("Trying to add new theme, but Id already exists. ThemeId = %x, Old theme with this Id:%s, new theme: %s."),
          ThemeId, Themes[ThemeId], Text);
        continue;
      }
      memcpy (&Themes[ThemeId], Text, Textlength);
      Themes[ThemeId][Textlength] = '\0'; //trailing NULL
      CleanString (Themes[ThemeId]);
      nThemes++;
      LogI(1, prep("%02x %s"), ThemeId, Themes[ThemeId]);
    }
  } //for NumberOfThemes
  if (p != DataEnd) {
    LogE(0, prep("Themes error: buffer is smaller or bigger than sum of entries. p:%p,DataEnd:%p"), p, DataEnd);
    return 0;
  } else
    return 2;
}

/**
 * \brief Get Nagra channels
 *
 * \return 0 = fatal error, code 1 = success, code 2 = last item processed
 */
int cFilterEEPG::GetChannelsNagra (const u_char * Data, int Length)
{
  u_char *DataStart = (u_char *) Data;
  u_char *p = DataStart;
  u_char *DataEnd = DataStart + Length;

  nChannels = (*p << 24) | *(p + 1) << 16 | *(p + 2) << 8 | *(p + 3);
  p += 4; //skip numberofchannels
  if (CheckLevel(1)) {
    isyslog ("|  ID  | %-26.26s | %-22.22s | FND | %-8.8s |\n", "Channel ID", "Channel Name", "Sky Num.");
    isyslog ("|------|-%-26.26s-|-%-22.22s-|-----|-%-8.8s-|\n", "------------------------------",
             "-----------------------------", "--------------------");
  }

  for (int j = 0; j < nChannels; j++) {
    sChannelsNagraGuide *Channel = (sChannelsNagraGuide *) p;
    sChannel *C = &sChannels[j];
    C->ChannelId = j + 1; //Nagra starts numbering at 1
    ChannelSeq[C->ChannelId] = j; //fill lookup table to go from channel-id to sequence nr in table; lookup table starts with 0
    C->SkyNumber = 0;
    //C->NumberOfEquivalences = 1; //there is always an original channel. every equivalence adds 1
    C->Src = Source(); //assume all EPG channels are on same satellite, if not, manage this via equivalents!!!
    C->Nid = HILO16 (Channel->NetworkId);
    C->Tid = HILO16 (Channel->TransportId);
    C->Sid = HILO16 (Channel->ServiceId);
    tChannelID channelID = tChannelID(C->Src, C->Nid, C->Tid, C->Sid);
    const cChannel *VC = GetChannelByID(channelID, true);
    bool IsFound = (VC);
    if(IsFound) {
      strncpy((char*)(C->Name), VC->Name (), 64);
      C->Src = VC->Source();
      CleanString (C->Name);
    }
    else
      C->Name[0] = '\0'; //empty string

    LogI(1, "|% 5d | %-26.26s | %-22.22s | %-3.3s |  % 6d  |\n", C->ChannelId
         , *channelID.ToString(), C->Name, IsFound ? "YES" : "NO", C->SkyNumber);

    LogD(4, prep("DEBUG: start    : %s"), cs_hexdump (0, p, 9));

    p += 8; //skip to first 0x8c if non-FTA, or 0x00 if FTA
    for (int i = 0; i < Channel->Nr8cBlocks; i++) {
      if (*p != 0x8c) {
        LogD(0, prep("DEBUG: ERROR in Channel Table, expected value of 0x8c is %02x"), *p);
        return 0; //fatal error
      }
      p++; //skip 8c byte
      LogD(4, prep("DEBUG: 8c string: %s"), cs_hexdump (0, p + 1, *p));
      p += *p; //skip 8c block
      p++; //forgot to skip length byte
    }
    //start last non 8c block here
    if (*p != 0x00) {
      LogE(0, prep("ERROR in Channel Table, expected value of 0x00 is %02x"), *p);
      return 0; //fatal error
    }
    p++; //skip 0x00 byte
    LogD(4, prep("DEBUG: endstring: %s"), cs_hexdump (0, p + 1, *p * 4));
    p += (*p * 4); //p points to nrofblocks, each block is 4 bytes
    p++; //forgot to skip nrofblocks byte

    /*
        if (Channel->AlwaysZero1 != 0)
          isyslog ("EEPGDEBUG: AlwaysZero1 is NOT ZERO:%x.", Channel->AlwaysZero1);
        if (Channel->AlwaysZero2 != 0)
          isyslog ("EEPGDEBUG: AlwaysZero2 is NOT ZERO:%x.", Channel->AlwaysZero2);
        if (Channel->Always0x8c != 0x8c)
          isyslog ("EEPGDEBUG: Always0x8c is NOT 0x8c:%x.", Channel->Always0x8c);
        if (Channel->Always0x08 != 0x08)
          isyslog ("EEPGDEBUG: Always0x08 is NOT 0x08:%x.", Channel->Always0x08);
        if (Channel->Always0x02 != 0x02)
          isyslog ("EEPGDEBUG: Always0x02 is NOT 0x02:%x.", Channel->Always0x02);
        if (Channel->Always0x01 != 0x01)
          isyslog ("EEPGDEBUG: Always0x01 is NOT 0x01:%x.", Channel->Always0x01);
        if (Channel->Always0x20 != 0x20)
          isyslog ("EEPGDEBUG: Always0x20 is NOT 0x20:%x.", Channel->Always0x20);
        if (Channel->Always0x0a != 0x0a)
          isyslog ("EEPGDEBUG: Always0x0a is NOT 0x0a:%x.", Channel->Always0x0a);
        if (Channel->Always0x81 != 0x81)
          isyslog ("EEPGDEBUG: Always0x81 is NOT 0x81:%x.", Channel->Always0x81);
        if (Channel->Always0x44 != 0x44)
          isyslog ("EEPGDEBUG: Always0x44 is NOT 0x44:%x.", Channel->Always0x44);
     */

  }
  if (p != DataEnd)
    LogE(0, prep("Warning, possible problem at end of channel table; p = %p, DataEnd = %p"), p, DataEnd);
  //LoadEquivalentChannels ();
  EquivHandler->loadEquivalentChannelMap();
  return 2; //obviously, when you get here, channels are read succesfully, but since all channels are sent at once, you can stop now
}

/**
 * \brief Get Nagra extended EPG
 *
 * \return 0 = fatal error, code 1 = success, code 2 = last item processed
 */
int cFilterEEPG::GetNagra (const u_char * Data, int Length)
{
  sTitleBlockHeaderNagraGuide *TBH = (sTitleBlockHeaderNagraGuide *) Data;
  if (InitialTitle[0] == 0x00) { //InitialTitle is empty, so we are waiting for the start marker
    if (TBH->TableIdExtensionHigh == 0x00 && TBH->TableIdExtensionLow == 0x00) { //this is the start of the data
      if (TBH->VersionNumber == LastVersionNagra) {
        LogI(0, prep("Nagra EEPG already up-to-date with version %i"), LastVersionNagra);
        return 2;
      }
      Version = TBH->VersionNumber;
      LogI(0, prep("initialized Nagraguide, version %i"), Version);
      //u_char *p = (u_char *) Data + 11;
      u_char *p = (u_char *) Data + 8;
      if (*p != 0x01) {
        LogE(0, prep("Error, Nagra first byte in table_id_extension 0x00 is not 0x01 but %02x. Format unknown, exiting."),
          *p);
        return 0; //fatal error
      }
      p++; //skip 0x01 byte
      unsigned short int l = ((*p << 8) | *(p + 1));
      u_char *p_end = p + l - 3; //this ensures a full block of 4 bytes is there to process
      p += 2; //skip length bytes
      while (p < p_end) {
        sSection0000BlockNagraGuide *S = (sSection0000BlockNagraGuide *) p;
        int TTT = ((S->TableIdExtensionHigh << 10) | (S->TableIdExtensionLow << 3) | (S->TIE200 << 9));
        LogD(4, prep("DEBUG: TableIdExtension %04x, Unknown1 %02x Version %02x Always 0xd6 %02x DayCounter %02x"),
             TTT, S->Unknown1, S->VersionNumber, S->Always0xd6, S->DayCounter);
        if ((TTT > 0x0400) && (TTT < 0x0600)) //take high byte and compare with summarie tables in 0x0400 and 0x0500 range;
          NagraTIE[NagraCounter++] = TTT; //only store TIEs of titlessummaries, all others can be derived; they are stored in the order of the 0x0000 index table,
        //lets hope first entry corresponds with today, next with tomorrow etc.
        p += 4;
      }
      buffer.clear (); //clear buffer maps
      bufsize.clear (); //clear buffer maps
      InitialTitle[0] = 0xff; //copy data into initial title
    }
    return (1);
  }
  unsigned short SectionLength = ((TBH->SectionLengthHigh & 0x0f) << 8) | TBH->SectionLengthLow;
  unsigned short TableIdExtension = HILO16 (TBH->TableIdExtension);
  if (TableIdExtension == 0x0000) {
    LastVersionNagra = Version;
    return (2); //done
  }
  /*
  Table_id_extensions:
  (0x0000)
  (0x0010) per channel, nr_of_channels entries, every entry is 8 bytes, first 4 bytes gives nr. of titles in corresponding title table
  (0x0020) per channel info day 2 of the month
  (0x01F0) per channel info day 31 of the month
  (0x0200) leeg; letop op je leest gemakkelijk door naar 0x0210!
  (0x0210) titles day 1 of the month
  (0x0220) titles day 2 of the month
  (0x03F0) titles day 31 of the month
  (0x0400) channel info
  (0x0410) summaries day 1 of the month
  (0x0420) summaries day 2 of the month
  (0x05F0) summaries day 31 of the month
  (0x0610) themes/title reference sunday, correspond to titles 0x0400 lower...
  (0x0620) themes/title reference monday
  ... this goes on until 0x07f0
  (0x0810) bouquet info; references to channels within a package, day 1 of the month
  (0x0820) same day 2 of the month
  (0x09f0) same day 31 of the month
   */

  if (!(TBH->TableIdExtensionHigh >= 0x02 && TBH->TableIdExtensionHigh <= 0x07)) //here we regulate which tables we are testing
    return (1);
  if (TableIdExtension == 0x0200) //table 0x0200 only contains language code, because it is for day 0 of the month, and 0x0400 is used for channels
    return 1;
  if (TBH->SectionNumber == 0) { //first section, create a table
    buffer[TableIdExtension] = NULL;
    bufsize[TableIdExtension] = 0;
    NumberOfTables++;
  }
  //store all sections in core until last section is found; processing incomplete sections is very complex and doesnt save much memory,
  //since the data has to be stored anyway; summaries do not seem to have channelid included, so storing titles and separately storing summaries will not work...
  //GetEventId only works for a specific Schedule for a specific ChannelId....
  buffer[TableIdExtension] =
    (unsigned char *) REALLOC (buffer[TableIdExtension], SectionLength - 9 + bufsize[TableIdExtension]);
  memcpy (buffer[TableIdExtension] + bufsize[TableIdExtension], Data + 8, SectionLength - 9); //append new section
  bufsize[TableIdExtension] += SectionLength - 9;
  if (TBH->SectionNumber >= TBH->LastSectionNumber) {
    LogI(1, prep("found %04x lastsection nr:%i."), TableIdExtension, TBH->SectionNumber);
    // if (TBH->TableIdExtensionHigh == 0x04 || TBH->TableIdExtensionHigh == 0x05) {
    if (TableIdExtension == 0x0400) {
      int Result = GetChannelsNagra (buffer[TableIdExtension] + 4, bufsize[TableIdExtension] - 4); //TODO language code terminated by 0 is ignored
      free (buffer[TableIdExtension]);
      buffer[TableIdExtension] = NULL;
      NumberOfTables--;
      if (Result == 0)
        return 0; //fatal error; TODO this exit should also free all other, non-Channel sections that are stored!
    }
  } //if lastsection read
  return (1); //return and continue, nonfatal
}

void cFilterEEPG::ProcessNagra ()
{
  for (int i = 0; i < MAX_THEMES; i++) //clear all themes
    Themes[i][0] = '\0';

  for (int i = 0; i < NagraCounter; i++) { //first prcoess all themes, since they all use the same codes
    unsigned short int TableIdExtension = NagraTIE[i];
    int TIE = TableIdExtension + 0x0200; //from 0x0400 to 0x0600 -> themes
    LogI(3, prep("Processing Theme with TableIdExtension:%04x"), TIE);
    GetThemesNagra (buffer[TIE] + 4, bufsize[TIE] - 4, TableIdExtension - 0x0200); //assume theme is completed  //TODO Language code terminatd by 0 is ignored
    free (buffer[TIE]);
    buffer[TIE] = NULL;
    NumberOfTables--;
  }

  for (int i = 0; i < NagraCounter; i++) { //first process all themes, since they all use the same codes
    unsigned short int TableIdExtension = NagraTIE[i];
    int TIE = TableIdExtension - 0x0200; //from 0x0400 to 0x0200 -> titles
    LogI(0, prep("Processing TableIdExtension:%04x"), TableIdExtension);
    GetTitlesNagra (buffer[TIE] + 4, bufsize[TIE] - 4, TableIdExtension); //assume title-reading is completed  //TODO Language code terminated by 0 is ignored
    free (buffer[TIE]);
    buffer[TIE] = NULL;
    NumberOfTables--;

    free (buffer[TableIdExtension]); //summaries
    buffer[TableIdExtension] = NULL;
    NumberOfTables--;
  }
  if (NumberOfTables != 0)
    LogE(0, prep("ERROR, Not all tables processed and stream is already repeating. NumberOfTables = %i."),
      NumberOfTables);
}

/**
 * \brief Get MHW1 Titles
 *
 * \return 0 = fatal error, code 1 = success, code 2 = last item processed
 */
int cFilterEEPG::GetTitlesMHW1 (const u_char * Data, int Length)
{
  if (Length >= 42) {
    sTitleMHW1 *Title = (sTitleMHW1 *) Data;
    if (Title->ChannelId == 0xff) { //FF is separator packet
      if (memcmp (InitialTitle, Data, 46) == 0) { //data is the same as initial title //TODO use easier notation
        LogD(2, prep("End processing titles"));
        return 2;
      }
      if (nTitles == 0)
        memcpy (InitialTitle, Data, 46); //copy data into initial title
      int Day = Title->Day;
      int Hours = Title->Hours;
      if (Hours > 15)
        Hours -= 4;
      else if (Hours > 7)
        Hours -= 2;
      else
        Day++;
      if (Day > 6)
        Day = Day - 7;
      Day -= Yesterday;
      if (Day < 1)
        Day = 7 + Day;
      //if (Day == 1 && Hours < 6)
      if (Day == 0 && Hours < 6)
        Day = 7;
      //Day = 8;
      MHWStartTime = (Day * 86400) + (Hours * 3600) + YesterdayEpochUTC;
      LogI(3, prep("Titles: FF PACKET, seqnr:%02x."), Data[5]);
    } else if (InitialTitle[0] != 0x00) { //if initialized this should always be 0x90 = tableid!
      if (nTitles < MAX_TITLES) {
        Title_t *T;
        T = (Title_t *) malloc (sizeof (Title_t));
        Titles[nTitles] = T;
        int Minutes = Title->Minutes;
        int StartTime = MHWStartTime + (Minutes * 60);
        T->ChannelId = Title->ChannelId - 1;
        T->ThemeId = Title->ThemeId;
        T->TableId = Title->TableId;
        T->MjdTime = 0;  //only used at MHW2 and SKY
        T->EventId = HILO32 (Title->ProgramId);
        T->StartTime = LocalTime2UTC (StartTime); //here also Daylight Savings correction is done
        T->Duration = HILO16 (Title->Duration) * 60;
        T->SummaryAvailable = Title->SummaryAvailable;
        T->Text = (unsigned char *) malloc (47);
        if (T->Text == NULL) {
          LogE(0, prep("Titles memory allocation error."));
          return 0;
        }
        //T->Text[46] = '\0'; //end string with NULL character
        //memcpy (T->Text, &Title->Title, 23);
        decodeText2((unsigned char *)&Title->Title, 23, (char*)T->Text, 47);
        CleanString (T->Text);
        LogI(3, prep("EvId:%08x,ChanId:%x, Titlenr:%d:, StartTime(epoch):%i, SummAv:%x,Name:%s."), T->EventId,
             T->ChannelId, nTitles, T->StartTime, T->SummaryAvailable, T->Text);
        nTitles++;
      } //nTitles < MaxTitles
      else {
        LogE(0, prep("Error, %i titles found more than %i"), nTitles, MAX_TITLES);
        return 0;
      }
    } //else if InitialTitle
  } //Length==46
  else {
    LogE(0, prep("Error, length of title package < 42."));
    return 1; //non fatal
  }

  return 1;
}

/**
 * \brief Get MHW2 Titles
 *
 * \return 0 = fatal error, code 1 = success, code 2 = last item processed
 */
int cFilterEEPG::GetTitlesMHW2 (const u_char * Data, int Length)
{
  if (Length > 18) {
    int Pos = 18;
    int Len = 0;

    if (memcmp (InitialTitle, Data, 16) == 0) { //data is the same as initial title
      return 2; //last item processed
    } else {
      if (nTitles == 0)
        memcpy (InitialTitle, Data, 16); //copy data into initial title
      //Pos = 18;
      while (Pos < Length) {
        Title_t *T;
        T = (Title_t *) malloc (sizeof (Title_t));
        Titles[nTitles] = T;
        T->ChannelId = Data[Pos];
        Pos+=11;//The date time starts here
        //isyslog ("EEPGDebug: ChannelID:%d", T->ChannelId);
        T->MjdTime = 0; //(Data[Pos] << 8) | Data[Pos + 1] not used for matching MHW2
        T->StartTime = MjdToEpochTime(Data[Pos], Data[Pos + 1], Data[Pos + 2], Data[Pos + 3]);
        T->Duration = (((Data[Pos + 5] << 8) | Data[Pos + 6]) >> 4) * 60;
        Len = Data[Pos + 7] & 0x3f;
        //isyslog ("EEPGDebug: Len:%d", Len);
        T->Text = (unsigned char *) malloc (2 * Len + 1);
        if (T->Text == NULL) {
          LogE(0, prep("Titles memory allocation error."));
          return 0; //fatal error
        }
        decodeText2(&Data[Pos + 8],Len,(char*)T->Text,2 * Len + 1);
        //T->Text[Len] = '\0'; //end string with NULL character
        //memcpy (T->Text, &Data[Pos + 8], Len);
        CleanString (T->Text);
        Pos += Len + 8; // Sub Theme starts here
        T->ThemeId = ((Data[7] & 0x3f) << 6) | (Data[Pos] & 0x3f);
        T->TableId = DEFAULT_TABLE_ID; //TODO locate the actual table id
        T->EventId = (Data[Pos + 1] << 8) | Data[Pos + 2];
        T->SummaryAvailable = (T->EventId != 0xFFFF);
        LogI(3, prep("EventId %04x Titlenr %d:SummAv:%x,Name:%s."), T->EventId,
             nTitles, T->SummaryAvailable, T->Text);
        Pos += 3;
        nTitles++;
        if (nTitles > MAX_TITLES) {
          LogE(0, prep("Error, %i titles found more than %i"), nTitles, MAX_TITLES);
          return 0; //fatal error
        }
      }
      return 1; //success
    } //else memcmp
  } //if length
  return 1; //non fatal error
}

/**
 * \brief Get MHW1 Summaries
 *
 * \return 0 = fatal error, code 1 = success, code 2 = last item processed
 */
int cFilterEEPG::GetSummariesMHW1 (const u_char * Data, int Length)
{
  sSummaryMHW1 *Summary = (sSummaryMHW1 *) Data;
  if (Length >= 11) {
    if (Summary->NumReplays < 10) { //Why limit this at 10?
      if (Length >= (11 + (Summary->NumReplays * 7))) {
        if (Summary->Byte7 == 0xff && Summary->Byte8 == 0xff && Summary->Byte9 == 0xff) {
          if (memcmp (InitialSummary, Data, 20) == 0) { //data is equal to initial buffer
            return 2;
          } else if (nSummaries < MAX_TITLES) {
            if (nSummaries == 0)
              memcpy (InitialSummary, Data, 20); //copy this data in initial buffer
            int SummaryOffset = 11 + (Summary->NumReplays * 7);
            int SummaryLength = Length - SummaryOffset;
            unsigned char *Text = (unsigned char *) malloc (2*SummaryLength + 1);
            if (Text == NULL) {
              LogE(0, prep("Summaries memory allocation error."));
              return 0;
            }
            //Text[SummaryLength+1] = '\0'; //end string with NULL character
            //memcpy (Text, &Data[SummaryOffset], SummaryLength);
            decodeText2(&Data[SummaryOffset], SummaryLength, (char*)Text, 2*SummaryLength + 1);
//     CleanString (Text);
//          if (Summary->NumReplays != 0)
//          esyslog ("EEPG: Number of replays:%i.", Summary->NumReplays);
            //int Replays = Summary->NumReplays;
            int ReplayOffset = 11;
            Summary_t *S;
            S = (Summary_t *) malloc (sizeof (Summary_t));
            Summaries[nSummaries] = S;
            S->NumReplays = Summary->NumReplays;
            S->EventId = HILO32 (Summary->ProgramId);
//            unsigned short SectionLength = ((Summary->SectionLengthHigh & 0x0f) << 8) | Summary->SectionLengthLow;
//            Asprintf((char **)&Text, "%s \n\n SecLength:%d SLHigh:%d SLLow:%d", Text, SectionLength, Summary->SectionLengthHigh, Summary->SectionLengthLow);
            S->Text = Text;
            S->ShortTextLength = 0; //TODO DPE find for MHW1
            int i = 0;
            do {
              S->Replays[i].MjdTime = 0; //only used for SKY
              //if (Summary->NumReplays == 0) {
              //S->ChannelId = 0xFFFF; //signal that ChannelId is not known; 0 is bad signal value because it is a valid ChannelId...
              //S->StartTime = 0;
              //}
              //else {
              S->Replays[i].ChannelId = Data[ReplayOffset++] - 1;
              unsigned int Date_hi = Data[ReplayOffset++];
              unsigned int Date_lo = Data[ReplayOffset++];
              unsigned short int Hour = Data[ReplayOffset++];
              unsigned short int Minute = Data[ReplayOffset++];
              unsigned short int Sec = Data[ReplayOffset++];
              ReplayOffset++; //makes total of 7 bytes

              LogI(4, prep("EvId:%08x ChanId:%x, ChanName %s, Time: %02d:%02d:%02d."),
                   S->EventId, S->Replays[i].ChannelId,
                   sChannels[ChannelSeq[S->Replays[i].ChannelId]].Name, Hour, Minute, Sec);

	      S->Replays[i].StartTime = LocalTime2UTC (MjdToEpochTime (Date_hi, Date_lo, Hour, Minute, Sec));
              //}
              i++;
            } while (i < Summary->NumReplays);
            //} while (Replays-- >= 0);
            LogI(3, prep("EvId:%08x ChanId:%x, Replays:%d, Summnr %d:%.35s."), S->EventId,
                 S->Replays[0].ChannelId, S->NumReplays, nSummaries, S->Text);
            nSummaries++;
          } else {
            LogE(0, prep("Error, %i summaries found more than %i"), nSummaries, MAX_TITLES);
            return 0;
          }
        } //0xff
        else {
          LogE(0, prep("Warning, Summary bytes not as expected."));
          return 1; //it is not a success, but error is not fatal
        }
      } //numreplays length
      else {
        LogE(0, prep("Warning, number of replays is not conforming to length."));
        return 1; //nonfatal error
      }
    } //numreplays <10
    else {
      LogE(0, prep("Warning, number of replays %d > 10, cannot process."),
        Summary->NumReplays);
      return 1; //nonfatal error
    }
  } //length >11
  else {
    LogE(0, prep("Summary length too small:%s"), cs_hexdump (0, Data, Length));
    return 1; //nonfatal error
  }
  return 1; //success
}

/**
 * \brief Get MHW2 Summaries
 *
 * \return 0 = fatal error, code 1 = success, code 2 = last item processed
 */
int cFilterEEPG::GetSummariesMHW2 (const u_char * Data, int Length)
{
  if (Length > (Data[14] + 17)) {
    if (memcmp (InitialSummary, Data, 16) == 0) //data is equal to initial buffer
      return 2;
    else {
      if (nSummaries == 0)
        memcpy (InitialSummary, Data, 16); //copy this data in initial buffer
      if (nSummaries < MAX_TITLES) {
        int lenText = Data[14];
        int SummaryLength = lenText;
        int Pos = 15;
        int Loop = Data[Pos + SummaryLength] & 0x0f;
        Summary_t *S;
        S = (Summary_t *) malloc (sizeof (Summary_t));
        Summaries[nSummaries] = S;

        S->Replays[0].ChannelId = 0xFFFF; //signal that ChannelId is not known; 0 is bad signal value because it is a valid ChannelId...
        S->Replays[0].StartTime = 0; //not used
        S->Replays[0].MjdTime = 0;  //not used
        S->NumReplays = 0; //not used
        S->EventId = (Data[3] << 8) | Data[4];
        unsigned char tmp[4096]; //TODO do this smarter
        memcpy (tmp, &Data[Pos], lenText);
        tmp[SummaryLength] = '\n';
        SummaryLength += 1;
        Pos += (lenText + 1);
        if (Loop > 0) {
          while (Loop > 0) {
            lenText = Data[Pos];
            Pos += 1;
            if ((Pos + lenText) < Length) {
              memcpy (&tmp[SummaryLength], &Data[Pos], lenText);
              SummaryLength += lenText;
              if (Loop > 1) {
                tmp[SummaryLength] = ' ';// This is considered as an EPG bug in VDR '\n';
                SummaryLength += 1;
              }
            } else
              break;
            Pos += lenText;
            Loop--;
          }
        }
        S->Text = (unsigned char *) malloc (2 * SummaryLength + 1);
        if (S->Text == NULL) {
          LogE(0, prep("Summaries memory allocation error."));
          return 0; //fatal error
        }
        //memcpy (S->Text, tmp, SummaryLength);
        //S->Text[SummaryLength] = '\0'; //end string with NULL character
        decodeText2(tmp,SummaryLength,(char*)S->Text,2 * SummaryLength + 1);
        CleanString (S->Text);
        char * delim = strchr ( (char *)S->Text, '\n' );
        S->ShortTextLength = delim == NULL ? 0 : delim - (char *)S->Text;

        LogI(3, prep("EventId %08x Summnr %d:%.30s."), S->EventId, nSummaries, S->Text);
        nSummaries++;
      } else {
        LogE(0, prep("Error, %i summaries found more than %i"), nSummaries, MAX_TITLES);
        return 0; //fatal error
      }
    } //else
  } //if length
  return 1; //succes or nonfatal error
}

/**
 * \brief Get SKYBOX Channels
 *
 * \return 0 = fatal error, code 1 = success, code 2 = last item processed
 */
int cFilterEEPG::GetChannelsSKYBOX (const u_char * Data, int Length)
{

  if (memcmp (InitialChannel, Data, 8) == 0) { //data is the same as initial title
    //LoadEquivalentChannels ();
      EquivHandler->loadEquivalentChannelMap();
    return 2;
  } else {
    if (nChannels == 0)
      memcpy (InitialChannel, Data, 8); //copy data into initial title
    if (nChannels == 0) {
      LogI(1, "|  ID  | %-26.26s | %-22.22s | FND | %-8.8s |\n", "Channel ID", "Channel Name", "Sky Num.");
      LogI(1, "|------|-%-26.26s-|-%-22.22s-|-----|-%-8.8s-|\n", "------------------------------",
           "-----------------------------", "--------------------");
    }
//    unsigned short int BouquetId = (Data[3] << 8) | Data[4];
    int BouquetDescriptorsLength = ((Data[8] & 0x0f) << 8) | Data[9];
    int TransportStreamLoopLength =
      ((Data[BouquetDescriptorsLength + 10] & 0x0f) << 8) | Data[BouquetDescriptorsLength + 11];
    int p1 = (BouquetDescriptorsLength + 12);
    while (TransportStreamLoopLength > 0) {
      unsigned short int Tid = (Data[p1] << 8) | Data[p1 + 1];
      unsigned short int Nid = (Data[p1 + 2] << 8) | Data[p1 + 3];
      int TransportDescriptorsLength = ((Data[p1 + 4] & 0x0f) << 8) | Data[p1 + 5];
      int p2 = (p1 + 6);
      p1 += (TransportDescriptorsLength + 6);
      TransportStreamLoopLength -= (TransportDescriptorsLength + 6);
      while (TransportDescriptorsLength > 0) {
        unsigned char DescriptorTag = Data[p2];
        int DescriptorLength = Data[p2 + 1];
        int p3 = (p2 + 4);
        p2 += (DescriptorLength + 2);
        TransportDescriptorsLength -= (DescriptorLength + 2);
        if (DescriptorTag != 0xB1)
          continue;

        DescriptorLength -= 2;
        while (DescriptorLength > 0) {
          // 0x01 = Video Channel
          // 0x02 = Audio Channel
          // 0x05 = Other Channel
          //if( Data[p3+2] == 0x01 || Data[p3+2] == 0x02 || Data[p3+2] == 0x05 )
          //{
          unsigned short int Sid = (Data[p3] << 8) | Data[p3 + 1];
          unsigned short int ChannelId = (Data[p3 + 3] << 8) | Data[p3 + 4];
          unsigned short int SkyNumber = (Data[p3 + 5] << 8) | Data[p3 + 6];
          if (SkyNumber > 100 && SkyNumber < 1000) {
            if (ChannelId > 0) {
              sChannel *C;
              if (ChannelSeq.count (ChannelId) == 0) { //not found
                C = &sChannels[nChannels];
                C->ChannelId = ChannelId;
                //C->NumberOfEquivalences = 1; //there is always an original channel. every equivalence adds 1
                C->Src = Source (); //assume all EPG channels are on same satellite, if not, manage this via equivalents!!!
                C->Nid = Nid;
                C->Tid = Tid;
                C->Sid = Sid;
                C->SkyNumber = SkyNumber;
                tChannelID channelID = tChannelID (C->Src, C->Nid, C->Tid, C->Sid);
                const cChannel *VC = GetChannelByID (channelID, false);
                bool IsFound = (VC);
                if (IsFound)
                  strncpy ((char *) C->Name, VC->Name (), 64);
                else
                  C->Name[0] = '\0'; //empty string

                LogI(1, "|% 5d | %-26.26s | %-22.22s | %-3.3s |  % 6d  |\n", C->ChannelId
                     , *channelID.ToString(), C->Name, IsFound ? "YES" : "NO", C->SkyNumber);

                ChannelSeq[C->ChannelId] = nChannels; //fill lookup table to go from channel-id to sequence nr in table
                nChannels++;
                if (nChannels >= MAX_CHANNELS) {
                  LogE(0, prep("Error, %i channels found more than %i"), nChannels, MAX_CHANNELS);
                  return 0;
                }
              }
            }
          }
          p3 += 9;
          DescriptorLength -= 9;
        }
      }
    } //while
    return 1;
  } //else part of memcmp
}


/**
 * \brief Get SKYBOX Titles
 *
 * \return 0 = fatal error, code 1 = success, code 2 = last item processed
 */
int cFilterEEPG::GetTitlesSKYBOX (const u_char * Data, int Length)
{
  int p;
  unsigned short int ChannelId;
  unsigned short int MjdTime;
  int Len1;
  int Len2;

  if (Length < 20)
    return 1; //nonfatal error
  if (memcmp (InitialTitle, Data, 20) == 0) //data is the same as initial title
    return 2;
  else {
    if (nTitles == 0)
      memcpy (InitialTitle, Data, 20); //copy data into initial title
    ChannelId = (Data[3] << 8) | Data[4];
    MjdTime = (Data[8] << 8) | Data[9];
    if (ChannelId > 0) {
      if (MjdTime > 0) {
        p = 10;
        do {
          Title_t *T;
          T = (Title_t *) malloc (sizeof (Title_t));
          Titles[nTitles] = T;
          T->ChannelId = ChannelId;
          T->MjdTime = MjdTime; //only date, no time. Is used to match titles and summaries, SKYBOX only
          T->EventId = (Data[p] << 8) | Data[p + 1];
          Len1 = ((Data[p + 2] & 0x0f) << 8) | Data[p + 3];
          if (Data[p + 4] != 0xb5) {
            LogD(5, prep("Data error signature for title - Data[p + 4] != 0xb5"));
            break;
          }
          if (Len1 > Length) {
            LogD(5, prep("Data error signature for title - Len1 > Length"));
            break;
          }
          p += 4;
          Len2 = Data[p + 1] - 7;
          T->StartTime = MjdToEpochTime(MjdTime) + ((Data[p + 2] << 9) | (Data[p + 3] << 1));
          T->Duration = ((Data[p + 4] << 9) | (Data[p + 5] << 1));
          T->ThemeId = Data[p + 6];
          T->TableId = DEFAULT_TABLE_ID; //TODO locate the actual table id
          //TODO Data[p + 7] is Quality value add it to description
          T->Quality = Data[p + 7];
          switch (Data[p + 8] & 0x0F) {
          case 0x01:
            T->Rating = 0x00; //"U"
            break;
          case 0x02:
            T->Rating = 0x08; //"PG"
            break;
          case 0x03:
            T->Rating = 0x0C; //"12"
            break;
          case 0x04:
            T->Rating = 0x0F; //"15"
            break;
          case 0x05:
            T->Rating = 0x12; //"18"
            break;
          default:
            T->Rating = 0x00; //"-"
            break;
          }
          T->Unknown1 = Data[5]; //Bound to Channel/Date
          T->Unknown2 = Data[6]; //Bound to Channel/Date
          T->Unknown3 = Data[7]; //Bound to Channel/Date
          unsigned char tmp[4096]; //TODO smarter
          Len2 = sky_huffman_decode (&Data[p + 9], Len2, tmp);
          if (Len2 == 0) {
            LogE(0, prep("Warning, could not huffman-decode title-text, skipping title."));
            return 1; //non-fatal error
          }
          T->Text = (unsigned char *) malloc (Len2 + 1);
          if (T->Text == NULL) {
            LogE(0, prep("Titles memory allocation error."));
            return 0;
          }
          T->Text[Len2] = '\0'; //end string with NULL character
          memcpy (T->Text, tmp, Len2);
          CleanString (T->Text);
          T->SummaryAvailable = 1; //TODO I assume this is true?

          LogI(3, prep("EventId %08x Titlenr %d,Un1:%x,Un2:%x,Un3:%x,Quality:%x,Name:%s."),
               T->EventId, nTitles, T->Unknown1, T->Unknown2, T->Unknown3, T->Quality, T->Text);

          p += Len1;
          nTitles++;
          if (nTitles >= MAX_TITLES) {
            LogE(0, prep("Error, %i titles found more than %i"), nTitles, MAX_TITLES);
            return 0; //fatal error
          }
        } while (p < Length);
      }
    }
  }
  return 1; //success
}

/**
 * \brief Get SKYBOX Summaries
 *
 * \return 0 = fatal error, code 1 = success, code 2 = last item processed
 */
int cFilterEEPG::GetSummariesSKYBOX (const u_char * Data, int Length)
{
  int p;
  unsigned short int ChannelId;
  unsigned short int MjdTime;
  int Len1;
  int Len2;
  const char* STxtDelim = Format == SKY_UK?": ":" - ";

  if (Length < 20) {
    return 1; //non fatal error I assume
  }
  if (memcmp (InitialSummary, Data, 20) == 0) //data is equal to initial buffer
    return 2;
//        else if (nSummaries < MAX_SUMMARIES) {
  else {
    if (nSummaries == 0)
      memcpy (InitialSummary, Data, 20); //copy this data in initial buffer
    ChannelId = (Data[3] << 8) | Data[4];
    MjdTime = ((Data[8] << 8) | Data[9]);
    if (ChannelId > 0) {
      if (MjdTime > 0) {
        p = 10;
        do {
          Summary_t *S;
          S = (Summary_t *) malloc (sizeof (Summary_t));
          Summaries[nSummaries] = S;
          S->Replays[0].ChannelId = ChannelId;
          S->Replays[0].MjdTime = MjdTime;
          S->NumReplays = 0; //not used
          S->EventId = (Data[p] << 8) | Data[p + 1];
          S->ShortTextLength = 0;
          Len1 = ((Data[p + 2] & 0x0f) << 8) | Data[p + 3];
          if (Data[p + 4] != 0xb9) {
            LogD(5, prep("Data error signature for title - Data[p + 4] != 0xb5"));
            break;
          }
          if (Len1 > Length) {
            LogD(5, prep("Data error signature for title - Len1 > Length"));
            break;
          }
          p += 4;
          Len2 = Data[p + 1];
          unsigned char tmp[4096]; //TODO can this be done better?
          Len2 = sky_huffman_decode (&Data[p + 2], Len2, tmp);
          if (Len2 == 0) {
            LogE(0, prep("Warning, could not huffman-decode text, skipping summary."));
            return 1; //non-fatal error
          }
          S->Text = (unsigned char *) malloc (Len2 + 1);
          if (S->Text == NULL) {
            LogE(0, prep("Summaries memory allocation error."));
            return 0;
          }
          memcpy (S->Text, tmp, Len2);
          S->Text[Len2] = '\0'; //end string with NULL character
          CleanString (S->Text);
          char * delim = strstr ( (char *)S->Text, STxtDelim );
          //S->ShortTextLength = delim == NULL ? 0 : delim - (char *)S->Text;
          if (Format == SKY_UK && !delim && strncmp((char *)S->Text,"..",2) == 0) {
            char * tdlm = strpbrk ( (char *)(S->Text+3), ".?!" );
            if (tdlm && tdlm[0] != '.')
              tdlm++;
            if (tdlm)
              delim = tdlm;
	  }
          int shLen = delim - (char *)S->Text;
          S->ShortTextLength = delim == NULL || shLen > numeric_limits<u_char>::max() ? 0 : shLen;
          LogI(4, prep("EventId %08x Summnr %d:%.30s."), S->EventId, nSummaries, S->Text);
          p += Len1;
          nSummaries++;
          if (nSummaries >= MAX_TITLES) {
            LogI(0, prep("Error, %i summaries found more than %i"), nSummaries, MAX_TITLES);
            return 0;
          }
        } while (p < Length);
      }
    }
  }
  return 1;
}

void cFilterEEPG::FreeSummaries (void)
{
  if (Format == MHW1 || Format == MHW2 || Format == SKY_IT || Format == SKY_UK) {
    Summary_t *S; //TODO do I need this?
    Summary_t *S2; //TODO do I need this?
    for (int i = 0; i < nSummaries; i++) {
      S = Summaries[i];
      if (i < nSummaries - 1) {
        S2 = Summaries[i + 1]; //look at next summary
        //this is the last summary that points to this text block;
        // needed in case NumReplays > 1, multiple pointers to same textblock
        if (S->Text != S2->Text && S->Text != 0x00) 
          free (S->Text);
      } else if (S->Text != 0x00)
        free (S->Text);
      free (S);
    }
  }
  nSummaries = 0;
}

void cFilterEEPG::FreeTitles (void)
{
  if (Format == MHW1 || Format == MHW2 || Format == SKY_IT || Format == SKY_UK) {
    Title_t *T;
    for (int i = 0; i < nTitles; i++) {
      T = Titles[i];
      free (T->Text);
      free (T);
    }
  }
  nTitles = 0;
}

void cFilterEEPG::LoadIntoSchedule (void)
{
  int i, j;
  i = 0;
  j = 0;
  bool foundtitle;
  foundtitle = false;
  Title_t *T;
  Summary_t *S;
  int remembersummary = -1;
  //keep statistics
  int SummariesNotFound = 0;
  int NoSummary = 0;
  int NotMatching = 0;
  int LostSync = 0;
  int OldEvents = 0;
  int SummIndex = -1;
  bool isOTV = Format == SKY_IT || Format == SKY_UK;

  {
#if APIVERSNUM >= 20300
  LOCK_SCHEDULES_WRITE;
#else
  cSchedulesLock SchedulesLock (true);
  cSchedules *Schedules = (cSchedules *) cSchedules::Schedules (SchedulesLock);
#endif
  if (Schedules) {

    while (i < nTitles) {
      S = Summaries[j];
      foundtitle = false;

      while ((i < nTitles) && (!foundtitle)) { //find next title that has summary
        T = Titles[i];
        if (T->SummaryAvailable)
          foundtitle = true;
        else {
          NoSummary++;
          i++;
        }
      }
//esyslog("foundtitle %x for next title that has a summary:%d",foundtitle,i);

      if (!foundtitle)  //no more titles with summaries
        break;
      time_t too_old = time(NULL) - (Setup.EPGLinger * 60 + 3600); 
      if ((T->EventId == S->EventId && T->MjdTime == S->Replays[0].MjdTime) &&
          ((T->ChannelId == S->Replays[0].ChannelId) || !isOTV)) { //should always be true, titles and summaries are broadcasted in order...
        LogD(3, prep("T->EventId == S->EventId"));
        //MjdTime = 0 for all but SKY
        //S->ChannelId must be equal to T->ChannelId only for SKY; in MHW1 S->ChannelId overrides T->ChannelId when NumReplays > 1
        remembersummary = -1; //reset summary searcher
        //int Replays = S->NumReplays;
        if (isOTV)
           SummIndex = j;

        int index = 0;
        do {
          unsigned short int ChannelId;
          time_t StartTime;
          if (S->NumReplays == 0) {
            ChannelId = T->ChannelId;
            StartTime = T->StartTime;
          } else {
            ChannelId = S->Replays[index].ChannelId;
            StartTime = S->Replays[index].StartTime;
          }

          index++;
          if ((StartTime + (int)T->Duration) < too_old) {
            LogD(3, prep("Skipping old event '%s', start time:%s"), T->Text, ctime (&StartTime));
            OldEvents++;
            continue;
          }

          //channelids are sequentially numbered and sent in MHW1 and MHW2, but not in SKY, so we need to lookup the table index
          sChannel *C = &sChannels[ChannelSeq[ChannelId]]; //find channel
          //cSchedule *p;//[MAX_EQUIVALENCES];
          //PrepareToWriteToSchedule (C, Schedules, p);
          tChannelID chanID = tChannelID (C->Src, C->Nid, C->Tid, C->Sid);

          char rating = 0x00;
          if (isOTV &&  T->Rating) { //TODO only works on OTV for now
            rating = T->Rating;
          }
          unsigned short int TableId = DEFAULT_TABLE_ID;
          if (T->TableId) {
            TableId = T->TableId;
          }

//          LogD (0, prep("EventId %08x Titlenr:%d,SummNr:%d,SummAv:%x,Un1:%x,Un2:%x,Un3:%x,Name:%s,STxtLn:%d,Summary:%s."),
//                      T->EventId, i, j, T->SummaryAvailable, T->Unknown1, T->Unknown2, T->Unknown3, T->Text, S->ShortTextLength, S->Text);

          WriteToSchedule (chanID, Schedules, T->EventId, StartTime, T->Duration / 60, (char *) T->Text,
                           (char *) S->Text, T->ThemeId, TableId, 0, rating, S->ShortTextLength);
          sortSchedules(Schedules, chanID);
          //if (shortText != NULL) delete [] shortText;

          //FinishWriteToSchedule (C, Schedules, p);
          //Replays--;
          //if ((S->NumReplays != 0) && (Replays > 0)) { //when replays are used, all summaries of the replays are stored consecutively; currently only CSAT
          //j++;  //move to next summary
          //if (j >= nSummaries) //do not forget to look in beginning of (ring)buffer
          //j = 0;
          //S = Summaries[j]; //next summary within replay range
          //}
        }   //while
        while (index < S->NumReplays);

        //end of putting title and summary in schedule
        i++; //move to next title
      }    //if T->EventId == S->EventId
      else {
//      esyslog("EEPG ERROR: ProgramIds not matching, title:%d,summary%d, T->EventId:%u, S->Eventid:%u.",i,j,T->EventId,S->EventId);
        NotMatching++;
        if (remembersummary == -1) { //I am not in search loop yet
          remembersummary = j;
          if (remembersummary == 0)
            remembersummary = nSummaries; //or next test will never be succesfull for remembersummary = 0
          LostSync++;
//      esyslog("EEPG Error: lost sync at title %d, summary %d.",i,j);
        } else if (j == (remembersummary - 1) || SummIndex !=-1) { //the last summary to be checked has failed also
          //esyslog ("EEPG Error: could not find summary for summary-available Title %d.", i);
          if ((T->StartTime + T->Duration) < (uint)too_old) {
            LogD(3, prep("Skipping old event '%s' without summary, start time:%s"), T->Text, ctime ((time_t*)&T->StartTime));
            OldEvents++;
          } else {

            if (!isOTV) //No sense of logging for SKY when SummaryAvailable bit is not known
               esyslog("EEPG: Error, summary not found for EventId %08x Titlenr %d:SummAv:%x,Un1:%x,Un2:%x,Un3:%x,Name:%s.",
                 T->EventId, i, T->SummaryAvailable, T->Unknown1, T->Unknown2, T->Unknown3, T->Text);

            /* write Title info to schedule */
            sChannel *C = &sChannels[ChannelSeq[T->ChannelId]]; //find channel
            //cSchedule *p;//[MAX_EQUIVALENCES];
            tChannelID chanID = tChannelID (C->Src, C->Nid, C->Tid, C->Sid);
            //PrepareToWriteToSchedule (C, Schedules, p);
            char rating = 0x00;
            if (!isOTV &&  T->Rating) { //TODO only works on OTV for now
              rating = T->Rating;
            }
            WriteToSchedule (chanID, Schedules, T->EventId, T->StartTime, T->Duration / 60, (char *) T->Text,
                             NULL, T->ThemeId, DEFAULT_TABLE_ID, 0, rating, S->ShortTextLength);
            //FinishWriteToSchedule (C, Schedules, p);
            sortSchedules(Schedules, chanID);

            SummariesNotFound++;
          }
          i++; //move to next title, for this one no summary can be found
        }

        //Do not loop through summaries for OTV when sumaries and titles are in sync
        if (SummIndex != -1 && isOTV && nSummaries <= nTitles){
          j--;
        }

//      if (SummIndex != -1)
//        esyslog("Trying again for this title %d, remember summary %d, but advancing one Summary to %d.",i,remembersummary,j+1);
      }
      j++; //move to next summary
      if (j >= nSummaries) //do not forget to look in beginning of (ring)buffer
        j = 0;
    } //while title
  } // if s
  else
    LogE (0, prep("Error: could not lock schedules."));

  }//release ScheduleLock
  //TODO: Test if cleanup is needed now that old events are no more
  //one thing it Cleanup (true) does is write the epg.data immediately
  //cSchedules::Cleanup (true); //deletes all past events
  cSchedules::Cleanup (); //deletes all past events

  isyslog ("EEPG: found %i themes", nThemes);
  isyslog ("EEPG: found %i channels", nChannels);
  isyslog ("EEPG: found %i titles", nTitles);
  if (NoSummary)
    isyslog ("EEPG: of which %i reported to have no summary available; skipping these BIENTOT titles", NoSummary);
  isyslog ("EEPG: found %i summaries", nSummaries);
  if (SummariesNotFound) {
     if (nTitles-nSummaries!=SummariesNotFound) {
        esyslog ("EEPG: %i summaries not found", SummariesNotFound);
        esyslog ("EEPG: %i difference between No. of summaries and summaries not found", nTitles-nSummaries-SummariesNotFound);
     } else
        isyslog ("EEPG: %i titles without summaries", SummariesNotFound);
  }
  if (OldEvents)
    isyslog ("EEPG: Skipped %i old events", OldEvents);
  if (NotMatching > nSummaries)
    LogI (0, prep("Warning: lost sync %i times, summary did not match %i times."), LostSync, NotMatching);
  

  FreeSummaries (); //do NOT free channels, themes and bouquets here because they will be reused in SKY!
  FreeTitles ();
  if (!isOTV) { //everything but SKY; SKY is the only protocol where LoadIntoSchedule is called repeatedly
    ChannelSeq.clear ();
  }
}

void cFilterEEPG::AddFilter (u_short Pid, u_char Tid)
{
  if (!Matches (Pid, Tid)) {
    Add (Pid, Tid);
    esyslog (prep("Filter Pid:0x%x,Tid:0x%x added."), Pid, Tid);
  }
}

void cFilterEEPG::AddFilter (u_short Pid, u_char Tid, unsigned char Mask)
{
  if (!Matches (Pid, Tid)) {
    Add (Pid, Tid, Mask);
    esyslog (prep("Filter Pid:0x%x,Tid:0x%x,Mask:0x%x added."), Pid, Tid, Mask);
  }
}

void cFilterEEPG::ProcessNextFormat (bool FirstTime = false)
{
  /*  for (int i = 0; i <= HIGHEST_FORMAT; i++)
      esyslog ("EEPGDEBUG: Format %i on pid %x", i, UnprocessedFormat[i]); */

  if (!FirstTime) {
    //isyslog ("EEPG: found %i equivalents channels", equiChanMap.size());
    isyslog ("EEPG: found %i themes", nThemes);
    isyslog ("EEPG: found %i channels", nChannels);
    isyslog ("EEPG: found %i titles", nTitles);
    isyslog ("EEPG: found %i summaries", nSummaries);
    isyslog ("EEPG: written %i titles", TitleCounter);
    isyslog ("EEPG: written %i summaries", SummaryCounter);
    isyslog ("EEPG: rejected %i titles/summaries because of higher TableId", RejectTableId);
    //Send message when finished
    if (SetupPE->DisplayMessage) {
      char *mesg;
      Asprintf(&mesg, "EEPG: written %i summaries", SummaryCounter);
      Skins.QueueMessage(mtInfo, mesg, 2);
      free(mesg);
    }

    /*if (SummariesNotFound != 0)
       esyslog ("EEPG: %i summaries not found", SummariesNotFound);
       else if (VERBOSE >= 1)
       isyslog ("EEPG: %i summaries not found", SummariesNotFound); */
    UnprocessedFormat[Format] = 0; //clear previously processed format

    //Next few lines prevent eepg from reading multiple eepg-systems on one transponder e.g. CDNL
    //isyslog ("EEPG: Ended all processing");
    //return;
    //If you remove these lines, multiple systems WILL be read

  }
  TitleCounter = 0;
  SummaryCounter = 0;
  RejectTableId = 0;
  //cleanup mess of last processing
  ChannelSeq.clear ();
  FreeTitles ();
  FreeSummaries ();

  // Enable EIT scan for all except DISH_BEV since it is already enabled
  if (SetupPE->ProcessEIT && !UnprocessedFormat[EIT]
     && !UnprocessedFormat[FREEVIEW] && !UnprocessedFormat[DISH_BEV]) {
    UnprocessedFormat[EIT] = EIT_PID;
    EquivHandler->loadEquivalentChannelMap();
  }

  //now start looking for next format to process
  int pid;
  for (int i = 0; i <= HIGHEST_FORMAT; i++){ //find first format that is detected
    if (UnprocessedFormat[i]) {
      isyslog ("EEPG: %s Extended EPG detected on pid %x.", FormatName[i], UnprocessedFormat[i]);
      Format = (EFormat)i;
      // highest format is processed first this way
      // make sure that CONT protocols like Premiere, Freesat are processed
      // AFTER ONCE protocols like MHW, SKY and NAGRA
      break;
    }
    if (i == HIGHEST_FORMAT) { //there are no formats left to process
      isyslog ("EEPG: Ended all processing");
      return;
    }
  }

  pid = UnprocessedFormat[Format]; //and reinstall its pid

  memset (&InitialChannel, 0, 8);
  memset (&InitialTitle, 0, 64);
  memset (&InitialSummary, 0, 64);
  NagraCounter = 0;
  Version = -1; //because 0 can be a valid version number...
  nChannels = 0;
  nThemes = 0;
  EndChannels = false;
  EndThemes = false;
  switch (Format) {
  case PREMIERE:
    AddFilter (pid, SI::TableIdPremiereCIT);
    break;
  case MHW1:
    AddFilter (0xd3, SI::TableIdMHW1Themes); //ThemesMHW1//TODO: all filters are serialized, strictly speaking Themes is non-fatal...
    break;
  case MHW2:
    AddFilter (0x231, SI::TableIdMHW2ChannelsThemes); //MHW2 Channels & Themes
    break;
  case SKY_IT:
  case SKY_UK:
    GetThemesSKYBOX (); //Sky Themes from file
    AddFilter (0x11, SI::TableIdSKYChannels); //Sky Channels
    break;
  case FREEVIEW: //Freeview, CONT mode //TODO streamline this for other modes
    InitDictionary ();
    AddFilter (pid, SI::TableIdEIT_presentFollowing, 0xfe); //event info, actual(0x4e)/other(0x4f) TS, present/following
    AddFilter (pid, SI::TableIdEIT_schedule_first, 0xf0); //event info, actual TS, schedule(0x50)/schedule for future days(0x5X)
    AddFilter (pid, SI::TableIdEIT_schedule_Other_first, 0xf0); //event info, other  TS, schedule(0x60)/schedule for future days(0x6X)
    AddFilter (0x39, SI::TableIdEIT_schedule_first, 0xf0); //event info, actual TS, Viasat
    AddFilter (0x39, SI::TableIdEIT_schedule_Other_first, 0xf0); //event info, other  TS, Viasat
    break;
  case NAGRA:
    AddFilter (pid, SI::TableIdNagraCIT); //perhaps TID is equal to first data byte?
    break;
  case DISH_BEV:
#if APIVERSNUM < 10726
    AddFilter (EIT_PID, 0, 0); // event info, actual(0x4e)/other(0x4f) TS, present/following
#endif
    AddFilter (0x0300, 0, 0); // Dish Network EEPG event info, actual(0x4e)/other(0x4f) TS, present/following
    AddFilter (0x0441, 0, 0); // Dish Network EEPG event info, actual(0x4e)/other(0x4f) TS, present/following
//    AddFilter (0x0441, 0x50, 0xf0); // Bell ExpressVU EEPG
//    AddFilter (0x0441, 0x60, 0xf0); // Bell ExpressVU EEPG
    break;
  case EIT:
    AddFilter (pid, SI::TableIdEIT_presentFollowing, 0xfe); //event info, actual(0x4e)/other(0x4f) TS, present/following
    AddFilter (pid, SI::TableIdEIT_schedule_first, 0xf0); //event info, actual TS, schedule(0x50)/schedule for future days(0x5X)
    AddFilter (pid, SI::TableIdEIT_schedule_Other_first, 0xf0); //event info, other  TS, schedule(0x60)/schedule for future days(0x6X)
    break;
  default:
    break;
  }
}

void cFilterEEPG::ProccessContinuous(u_short Pid, u_char Tid, int Length, const u_char *Data)
{
  //0x39 Viasat, 0x0300 Dish Network EEPG, 0x0441 Bell ExpressVU EEPG
  LogD(4, prep("Pid: 0x%02x Tid: %d Length: %d"), Pid, Tid, Length);
  //Look for other satelite positions only if Dish/Bell ExpressVU for the moment hardcoded pid check
  SI::cEIT2 EIT(Source(), Tid, Data, Format, Pid == EIT_PID);
}

void cFilterEEPG::Process (u_short Pid, u_char Tid, const u_char * Data, int Length)
{
  int now = time (0);
//  LogD(2, prep("Pid: 0x%02x Tid: %d Length: %d PMT pid: 0x%04x"), Pid, Tid, Length, pmtpid);
//  LogD(2, prep("Source: %d Transponder: %d"), Source () , Transponder ());

  if (Pid == 0 && Tid == SI::TableIdPAT) {
    if (!pmtnext || now > pmtnext) {
      if (pmtpid)
        NextPmt ();
      if (!pmtpid) {
        SI::PAT pat (Data, false);
        if (pat.CheckCRCAndParse ()) {
          SI::PAT::Association assoc;
          int idx = 0;
          for (SI::Loop::Iterator it; pat.associationLoop.getNext (assoc, it);) {
            if (!assoc.isNITPid ()) {
              //if (!assoc.isNITPid () && Scanning) {
              if (idx++ == pmtidx) {
                pmtpid = assoc.getPid ();
                pmtsid = assoc.getServiceId ();
                Add (pmtpid, SI::TableIdPMT);
                pmtnext = now + PMT_SCAN_TIMEOUT;
                LogI(3, prep("PMT pid now 0x%04x (idx=%d)\n"), pmtpid, pmtidx);
                break;
              }
            }
          }
          if (!pmtpid) {
            pmtidx = 0;
            pmtnext = now + PMT_SCAN_IDLE;
            LogI(1, prep("PMT scan idle\n"));

            Del (0, 0);  //this effectively kills the PMT_SCAN_IDLE functionality

            //now after the scan is completed, start processing
            ProcessNextFormat (true); //FirstTime flag is set
          }
        }
      }
    }
  } else if (pmtpid > 0 && Pid == pmtpid && Tid == SI::TableIdPMT && Source () && Transponder ()) {
    SI::PMT pmt (Data, false);
    if (pmt.CheckCRCAndParse () && pmt.getServiceId () == pmtsid) {
      SI::PMT::Stream stream;
      for (SI::Loop::Iterator it; pmt.streamLoop.getNext (stream, it);) {
        LogD(4, prep("StreamType: 0x%02x"), stream.getStreamType ());
        if (stream.getStreamType () == 0x05 || stream.getStreamType () == 0xc1) { //0x05 = Premiere, SKY, Freeview, Nagra 0xc1 = MHW1,MHW2;
          SI::CharArray data = stream.getData ();
          if ((data[1] & 0xE0) == 0xE0 && (data[3] & 0xF0) == 0xF0) {
            bool prvData = false, usrData = false;
            bool prvOTV = false, prvFRV = false;
            int usrOTV = 0, usrFRV = 0;
            if (data[2]==0x39) {//TODO Test This
              prvFRV = true;
              usrFRV = 1;
              LogD(4, prep("if (data[2]==0x39) {//TODO Test This"));
            }

            SI::Descriptor * d;
            unsigned char nDescriptorTag;
            for (SI::Loop::Iterator it; (d = stream.streamDescriptors.getNext (it));) {
              LogD(4, prep("EEPGDEBUG:d->getDescriptorTAG():%x,SI::PrivateTag:%x\n"), d->getDescriptorTag (), SI::PrivateDataSpecifierDescriptorTag);
              nDescriptorTag = d->getDescriptorTag ();
              switch (nDescriptorTag) {
              case SI::PrivateDataSpecifierDescriptorTag:
                //esyslog ("prv: %d %08x\n", d->getLength (), d->getData ().FourBytes (2));
                if (d->getLength () == 6 && d->getData ().FourBytes (2) == 0x000000be)
                  prvData = true;
                if (d->getLength () == 6 && d->getData ().FourBytes (2) == 0x4f545600) //OpenTV
                  prvOTV = true;
                if (d->getLength () == 6 && d->getData ().FourBytes (2) == 0x46534154) //Freeview
                  prvFRV = true;
                break;
              case SI::StreamIdentifierDescriptorTag:
                //if (d->getLength () == 3 && d->getData ().FourBytes (2) == 0xb07ea882) {
                if (d->getLength () == 3 && ((d->getData ().TwoBytes (2) & 0xff00) == 0xb000))
                  UnprocessedFormat[NAGRA] = stream.getPid ();
                break;
              case SI::SkyOTVDescriptorTag:
                //esyslog ("usr: %d %08x\n", d->getLength (), d->getData ().FourBytes (2));
                if (d->getLength () == 6 && d->getData ().FourBytes (2) == 0x0000ffff)
                  usrData = true;
                if (d->getLength () == 3 && ((d->getData ().TwoBytes (2) & 0xff00) == 0xb600)) //SKY IT //TODO ugly!
                  usrOTV = SKY_IT;
                //Format = SKY_IT;
                if (d->getLength () == 3 && d->getData ().FourBytes (2) == 0xc004e288)       //SKY UK
                  usrOTV = SKY_UK;
                //Format = SKY_UK;
                break;
              case SI::MHW1DescriptorTag: //MHW1, MHW2
//      esyslog("EEPGDEBUG:d->getDescriptorTAG:%d %08x\n",d->getLength(),d->getData().FourBytes(2));
                if (d->getLength () == 10 && d->getData ().FourBytes (2) == 0x50555348) //MHw1 Cyfra
                  UnprocessedFormat[MHW1] = stream.getPid ();
                break;
              case SI::MHW1_2DescriptorTag: //MHW1, MHW2
                if (d->getLength () == 10 && d->getData ().FourBytes (2) == 0x45504700) //MHw1 CanDigNL and CSat
                  UnprocessedFormat[MHW1] = stream.getPid ();
                else if (d->getLength () == 10 && d->getData ().FourBytes (2) == 0x46494348) { //MHW2
                  UnprocessedFormat[MHW2] = stream.getPid ();
                }
                break;
              case SI::FreeviewDescriptorTag: //Freeview
                LogD(4, prep("case 0xd1: //Freeview"));
                if (d->getLength () == 3 && ((d->getData ().TwoBytes (2) & 0xff00) == 0x0100))
                  usrFRV = 0x01;
                //01 = EIT pid 3842
                //03 04 = SDT Service Description Table pid 3841
                //07 = still undocumented, definition of buch of transport streams pid 3840
                //02 = ATSC reserved, find out what huffman encoded text is sent here! pid 3843
                //05 06 = TOT Time Offset Table pid 3844
                break;

                /*       case 0xfe: //SKY_IT
                  if (d->getLength () == 6 && d->getData ().FourBytes (2) == 0x534b5900) { //SKY_IT
                    Format = SKY_IT;
                  }
                  break;*/
              default:
                break;
              }
              delete d;
            }
            if ((prvOTV) && ((usrOTV == SKY_IT) || (usrOTV == SKY_UK)))
              UnprocessedFormat[usrOTV] = stream.getPid ();
            else if (prvFRV)
              if (usrFRV == 0x01)
                UnprocessedFormat[FREEVIEW] = stream.getPid ();
            if (prvData && usrData)
              UnprocessedFormat[PREMIERE] = stream.getPid ();
            //TODO DPE DISH/BEV filters are always ON on provided transponders,
            // but there is no knowledge for the loop of the data at the moment.
            //EEPG:12472:H:S119.0W:20000:0:0:0:0:36862:4100:18:36862
            if (((Source() == cSource::FromString("S119.0W")
                && Transponder() == cChannel::Transponder(12472,'H'))
                || (Source() == cSource::FromString("S91.0W")
                && Transponder() == cChannel::Transponder(12224,'V')))
                && !UnprocessedFormat[DISH_BEV]) {
              UnprocessedFormat[DISH_BEV] = stream.getPid ();
            }

          }   //if data[1] && data [3]
        }   //if streamtype
      } //for loop that walks through streams
      NextPmt ();
      pmtnext = 0;
      /*      }
            else {
       Del (pmtpid, 0x02);
       Del (0, 0);
       pmtidx = 0;
       pmtnext = now + PMT_SCAN_IDLE;
       isyslog ("PMT scan forced idle\n");
            }*/
    } //checkCRC
  } //if pmtpid
  else if (Source ()) {

    if ( Pid == EIT_PID || Pid == 0x0300 || Pid == 0x0441 ) {
      if (Tid >= SI::TableIdEIT_presentFollowing)
        ProccessContinuous(Pid, Tid, Length, Data);
      return;
    }
    int Result;
    switch (Tid) {
    case SI::TableIdPremiereCIT:
      if ((Pid < 0x30) || (Pid > 0x37)) {

        ProcessPremiere(Data);
        break;
      } //if citpid == 0xb11 Premiere
      /* no break - used for sky also*/
    case SI::TableIdSKYTitles_first ... SI::TableIdSKYTitles_last:
      Result = GetTitlesSKYBOX (Data, Length - 4);
      if (Result != 1) //when fatal error or finished
        Del (Pid, SI::TableIdSKYTitlesA0, 0xfc); //kill filter
      if (Result == 0) { //fatal error
        esyslog ("EEPG: Fatal error reading titles.");
        ProcessNextFormat (); //and go process other formats
      }
      if (Result == 2)
        AddFilter (Pid + 0x10, SI::TableIdSKYSummaries_first, 0xfc); //Set filter that processes summaries of this batch
      break;
    case SI::TableIdSKYSummaries_first ... SI::TableIdSKYSummaries_last:
      Result = GetSummariesSKYBOX (Data, Length - 4);
      if (Result != 1) //when fatal error or finished
        Del (Pid, SI::TableIdSKYSummaries_first, 0xfc); //kill filter
      if (Result == 0) {
        esyslog ("EEPG: Fatal error reading summaries.");
        ProcessNextFormat ();
      }
      if (Result == 2) {
        LoadIntoSchedule ();
        if (Pid < 0x47) //this is not the last batch//FIXME chaining is easy on the PIDs and the CPU, but error when Pid,Tid is not used at the moment...
          AddFilter (Pid - 0x0F, SI::TableIdSKYTitlesA0, 0xfc); //next pid, first tid
        else //last pid was processed
          ProcessNextFormat ();
      }
      break;
    case SI::TableIdMHW1TitlesSummaries:
      if (Pid == 0xd2) {
        Result = GetTitlesMHW1 (Data, Length);
        if (Result != 1) //fatal error or last processed
          Del (Pid, Tid);
        if (Result == 0) {
          esyslog ("EEPG: Fatal error reading titles.");
          ProcessNextFormat ();
        }
        if (Result == 2)
          AddFilter (0xd3, SI::TableIdMHW1TitlesSummaries); //SummariesMHW1
      } else if (Pid == 0xd3) {
        Result = GetSummariesMHW1 (Data, Length);
        if (Result != 1) //fatal error or last processed
          Del (Pid, Tid);
        if (Result == 0) {
          esyslog ("EEPG: Fatal error reading summaries.");
        }
        if (Result == 2) {
          LoadIntoSchedule ();
        }
        if (Result != 1) {
          ProcessNextFormat ();
        }
      }
      break;
    case SI::TableIdMHW2ChannelsThemes: //GetChannelsMHW2 or GetThemesMHW2
      if (Pid == 0x231) {
        if (Data[3] == 0x01) { //Themes it will be
          Result = GetThemesMHW2 (Data, Length); //return code 0 = fatal error, code 1 = success, code 2 = last item processed
          //break;
          if (Result != 1)
            EndThemes = true; //also set Endthemes on true on fatal error
        } //if Data
        else if (Data[3] == 0x00) { //Channels it will be
          Result = GetChannelsMHW (Data, Length); //return code 0 = fatal error, code 1 = success, code 2 = last item processed
          if (Result != 1)
            EndChannels = true; //always remove filter, code 1 should never be returned since MHW2 always reads all channels..
          ChannelsOk = (Result == 2);
        }
        if (EndChannels && EndThemes) { //those are only set within MHW2
          Del (0x231, SI::TableIdMHW2ChannelsThemes); //stop reading MHW2 themes and channels
          if (ChannelsOk) //No channels = fatal, no themes = non fatal
            AddFilter (0x234, SI::TableIdMHW2Titles); //start reading MHW2 titles
          else {
            esyslog ("EEPG: Fatal error reading channels.");
            ProcessNextFormat ();
          }
        }
      }    //if Pid == 0x231
      break;
    case SI::TableIdMHW1Channels:
      Result = GetChannelsMHW (Data, Length); //return code 0 = fatal error, code 1 = success, code 2 = last item processed
      Del (Pid, Tid); //always remove filter, code 1 should never be returned since MHW1 always reads all channels...
      if (Result == 2)
        AddFilter (0xd2, SI::TableIdMHW1TitlesSummaries); //TitlesMHW1
      else {
        esyslog ("EEPG: Fatal error reading channels.");
        ProcessNextFormat ();
      }
      break;
    case SI::TableIdMHW1Themes:
      Result = GetThemesMHW1 (Data, Length); //return code 0 = fatal error, code 1 = success, code 2 = last item processed
      if (Result != 1)
        Del (Pid, Tid);
      if (Result == 2)
        AddFilter (0xd3, SI::TableIdMHW1Channels); //ChannelsMHW1
      else {
        esyslog ("EEPG: Fatal error reading themes."); //Doesn't have to be fatal...
        ProcessNextFormat ();
      }
      break;
    case SI::TableIdMHW2Titles: //TitlesMHW2
      if (Pid == 0x234) {
        Result = GetTitlesMHW2 (Data, Length); //return code 0 = fatal error, code 1 = success, code 2 = last item processed
        if (Result != 1)
          Del (Pid, Tid);
        if (Result == 0) {
          esyslog ("EEPG: Fatal error reading titles.");
          ProcessNextFormat ();
        }
        if (Result == 2)
          AddFilter (0x236, SI::TableIdMHW2Summaries); //start reading MHW2 summaries....
      }
      break;
    case SI::TableIdMHW2Summaries: //Summaries MHW2
      if (Pid == 0x236) {
        Result = GetSummariesMHW2 (Data, Length); //return code 0 = fatal error, code 1 = sucess, code 2 = last item processed
        if (Result != 1)
          Del (Pid, Tid);
        if (Result == 0) {
          esyslog ("EEPG: Fatal error reading summaries.");
        }
        if (Result == 2)
          LoadIntoSchedule ();
        if (Result != 1) {
          ProcessNextFormat ();
        }
      } //if pid
      break;
    case SI::TableIdSKYChannels: //Sky channels
      if (Pid == 0x11) {
        Result = GetChannelsSKYBOX (Data, Length - 4);
        if (Result != 1) //only breakoff on completion or error; do NOT clean up after success, because then not all bouquets will be read
          Del (Pid, Tid); //Read all channels, clean up filter
        if (Result == 2) {
          if (InitDictionary ())
            AddFilter (0x30, SI::TableIdSKYTitlesA0, 0xfc); //SKY Titles batch 0 of 7
          else {
            esyslog ("EEPG: Fatal error reading huffman table.");
            ProcessNextFormat ();
          }
        }

      } //if Pid == 0x11
      break;
    case SI::TableIdNagraCIT: //NagraGuide
      if (Pid == 0xc8) {
        Result = GetNagra (Data, Length);
        if (Result != 1)
          Del (Pid, Tid);
        if (Result == 0) {
          esyslog ("EEPG: Fatal error processing NagraGuide. End of processing.");
        }
        if (Result == 2) {
          ProcessNagra ();
          cSchedules::Cleanup (true); //deletes all past events
          isyslog ("EEPG: Ended processing Nagra");
        }
        if (Result != 1) {
          ProcessNextFormat ();
        }
      }
      break;

    case SI::TableIdEIT_presentFollowing:
    case SI::TableIdEIT_presentFollowing_other:
    case SI::TableIdEIT_schedule_first ... SI::TableIdEIT_schedule_Other_last:
      // Freesat:
      // Set(3842, 0x4E, 0xFE);  // event info, actual(0x4E)/other(0x4F) TS, present/following
      // Set(3842, 0x50, 0xF0);  // event info, actual TS, schedule(0x50)/schedule for future days(0x5X)
      // Set(3842, 0x60, 0xF0);  // event info, other  TS, schedule(0x60)/schedule for future days(0x6X)
      //
      // PID found: 3841 (0x0f01)  [SECTION: Service Description Table (SDT) - other transport stream]
      // PID found: 3842 (0x0f02)  [SECTION: Event Information Table (EIT) - other transport stream, schedule]
      // PID found: 3843 (0x0f03)  [SECTION: ATSC reserved] TODO find out what compressed text info is here!
      // PID found: 3844 (0x0f04)  [SECTION: Time Offset Table (TOT)]

      if (Pid == 3842 || Pid == 0x39) {//0x39 Viasat, 0x0300 Dish Network EEPG, 0x0441 Bell ExpressVU EEPG
        ProccessContinuous(Pid, Tid, Length, Data);
      }
      break;

    default:
      break;
    } //end switch
  } //closes SOURCE()
} //end of closing

void cFilterEEPG::ProcessPremiere(const u_char *& Data)

{

  SI::PremiereCIT cit(Data, false);

  if (cit.CheckCRCAndParse ()) {
#if APIVERSNUM >= 20300
    LOCK_SCHEDULES_WRITE;
#else
    cSchedulesLock SchedulesLock (true, 10);
    cSchedules *Schedules = (cSchedules *) cSchedules::Schedules (SchedulesLock);
#endif
    if (Schedules) {
      int now = time (0);
      int nCount = 0;
      int nRating = 0;
      unsigned char Tid = SI::TableIdPremiereCIT; // TODO maybe default TableID
      SI::ExtendedEventDescriptors * ExtendedEventDescriptors = 0;
      SI::ShortEventDescriptor * ShortEventDescriptor = 0;
      char *order = 0, *rating = 0;
      {
        time_t firstTime = 0;
        SI::Descriptor * d;
        unsigned char nDescriptorTag;
        bool UseExtendedEventDescriptor = false;
        int LanguagePreferenceShort = -1;
        int LanguagePreferenceExt = -1;
        for (SI::Loop::Iterator it; (d = cit.eventDescriptors.getNext (it));) {
          nDescriptorTag = d->getDescriptorTag ();
          switch (nDescriptorTag) {
          case SI::PremiereOrderInfoDescriptorTag: // order information
            if (SetupPE->OrderInfo) {
              static const char *text[] = {
                trNOOP ("Ordernumber"),
                trNOOP ("Price"),
                trNOOP ("Ordering"),
                trNOOP ("SMS"),
                trNOOP ("WWW")
              };
              char buff[512];
              int p = 0;
              const unsigned char *data = d->getData ().getData () + 2;
              for (int i = 0; i < 5; i++) {
                int l = data[0];
                if (l > 0)
                  p += snprintf (&buff[p], sizeof (buff) - p, "\n%s: %.*s", tr (text[i]), l, &data[1]);
                data += l + 1;
              }
              if (p > 0)
                order = strdup (buff);
            }
            break;
          case SI::PremiereRatingInfoDescriptorTag: // parental rating
            if (SetupPE->RatingInfo) {
              char buff[512];
              int p = 0;
              const unsigned char *data = d->getData ().getData () + 2;
              nRating = data[0] + 3;
              p += snprintf (&buff[p], sizeof (buff) - p, "\n%s: %d %s", tr ("Rating"), nRating, tr ("years"));
              data += 7;
              int l = data[0];
              if (l > 0)
                p += snprintf (&buff[p], sizeof (buff) - p, " (%.*s)", l, &data[1]);
              if (p > 0)
                rating = strdup (buff);
            }
            break;
          case SI::PremiereContentTransmissionDescriptorTag:
            if (nCount >= 0) {
              SI::PremiereContentTransmissionDescriptor * pct = (SI::PremiereContentTransmissionDescriptor *) d;
              nCount++;
              SI::PremiereContentTransmissionDescriptor::StartDayEntry sd;
              SI::Loop::Iterator it;
              if (pct->startDayLoop.getNext (sd, it)) {
                SI::PremiereContentTransmissionDescriptor::StartDayEntry::StartTimeEntry st;
                SI::Loop::Iterator it2;
                if (sd.startTimeLoop.getNext (st, it2)) {
                  time_t StartTime = st.getStartTime (sd.getMJD ());
                  if (nCount == 1)
                    firstTime = StartTime;
                  else if (firstTime < StartTime - 5 * 50 || firstTime > StartTime + 5 * 60)
                    nCount = -1;
                }
              }
            }
            break;
          case SI::ExtendedEventDescriptorTag: {
            SI::ExtendedEventDescriptor * eed = (SI::ExtendedEventDescriptor *) d;
            if (I18nIsPreferredLanguage (Setup.EPGLanguages, eed->languageCode, LanguagePreferenceExt)
              || !ExtendedEventDescriptors) {
              delete ExtendedEventDescriptors;
              ExtendedEventDescriptors = new SI::ExtendedEventDescriptors;
              UseExtendedEventDescriptor = true;
            }
            if (UseExtendedEventDescriptor) {
              ExtendedEventDescriptors->Add (eed);
              d = NULL; // so that it is not deleted
            }
            if (eed->getDescriptorNumber () == eed->getLastDescriptorNumber ())
              UseExtendedEventDescriptor = false;
          }
          break;
          case SI::ShortEventDescriptorTag: {
            SI::ShortEventDescriptor * sed = (SI::ShortEventDescriptor *) d;
            if (I18nIsPreferredLanguage (Setup.EPGLanguages, sed->languageCode, LanguagePreferenceShort)
              || !ShortEventDescriptor) {
              delete ShortEventDescriptor;
              ShortEventDescriptor = sed;
              d = NULL; // so that it is not deleted
            }
          }
          break;
          default:
            break;
          }
          delete d;
        }
      }

      {
        bool Modified = false;
        int optCount = 0;
        unsigned int crc[3];
        crc[0] = cit.getContentId ();
        SI::PremiereContentTransmissionDescriptor * pct;
        for (SI::Loop::Iterator it;
          (pct =
            (SI::PremiereContentTransmissionDescriptor *) cit.eventDescriptors.getNext (it,
              SI::
              PremiereContentTransmissionDescriptorTag));) {
          int nid = pct->getOriginalNetworkId ();
          int tid = pct->getTransportStreamId ();
          int sid = pct->getServiceId ();
          if (SetupPE->FixEpg) {
            if (nid == 133) {
              if (tid == SI::TableIdTSDT && sid == 0xf0) {
                tid = SI::TableIdPMT;
                sid = 0xe0;
              } else if (tid == SI::TableIdTSDT && sid == 0xf1) {
                tid = SI::TableIdPMT;
                sid = 0xe1;
              } else if (tid == SI::TableIdTSDT && sid == 0xf5) {
                sid = 0xdc;
              } else if (tid == 0x04 && sid == 0xd2) {
                tid = 0x11;
                sid = 0xe2;
              } else if (tid == 0x11 && sid == 0xd3) {
                tid = 0x11;
                sid = 0xe3;
              }
            }
          }
          tChannelID channelID (Source (), nid, tid, sid);
#if APIVERSNUM >= 20300
	  LOCK_CHANNELS_READ;
          const cChannel *channel = Channels->GetByChannelID (channelID, true);
#else
          cChannel *channel = Channels.GetByChannelID (channelID, true);
#endif

          if (!channel)
            continue;

          cSchedule *pSchedule = (cSchedule *) Schedules->GetSchedule (channelID);
          if (!pSchedule) {
            pSchedule = new cSchedule (channelID);
            Schedules->Add (pSchedule);
          }

          optCount++;
          SI::PremiereContentTransmissionDescriptor::StartDayEntry sd;
          int index = 0;
          for (SI::Loop::Iterator it; pct->startDayLoop.getNext (sd, it);) {
            int mjd = sd.getMJD ();
            SI::PremiereContentTransmissionDescriptor::StartDayEntry::StartTimeEntry st;
            for (SI::Loop::Iterator it2; sd.startTimeLoop.getNext (st, it2);) {
              time_t StartTime = st.getStartTime (mjd);
              time_t EndTime = StartTime + cit.getDuration ();
              int runningStatus =
                (StartTime < now && now < EndTime) ? SI::RunningStatusRunning :
                  ((StartTime - 30 < now && now < StartTime) ?
                    SI::RunningStatusStartsInAFewSeconds : SI::RunningStatusNotRunning);
              bool isOpt = false;
              if (index++ == 0 && nCount > 1)
                isOpt = true;
              crc[1] = isOpt ? optCount : 0;
              crc[2] = StartTime / STARTTIME_BIAS;
              tEventID EventId = ((('P' << 8) | 'W') << 16) | crc16 (0, (unsigned char *) crc, sizeof (crc));
              LogI(2, "%s R%d %04x/%.4x %d %d/%d %s +%d ", *channelID.ToString (), runningStatus,
                   EventId & 0xFFFF, cit.getContentId (), index, isOpt, optCount,
                   stripspace (ctime (&StartTime)), (int) cit.getDuration () / 60);
              if (EndTime + Setup.EPGLinger * 60 < now) {
                LogI(2, "(old)\n");
                continue;
              }

              bool newEvent = false;
              cEvent *pEvent = (cEvent *) pSchedule->GetEvent (EventId, -1);
              if (!pEvent) {
                LogI(2, "(new)\n");
                pEvent = new cEvent (EventId);
                if (!pEvent)
                  continue;
                newEvent = true;
              } else {
                LogI(2, "(upd)\n");
                pEvent->SetSeen ();
                if (pEvent->TableID () == SI::TableIdPAT || pEvent->Version () == cit.getVersionNumber ()) {
                  if (pEvent->RunningStatus () != runningStatus)
                    pSchedule->SetRunningStatus (pEvent, runningStatus, channel);
                  continue;
                }
              }
              pEvent->SetEventID (EventId);
              pEvent->SetTableID (Tid);
              pEvent->SetVersion (cit.getVersionNumber ());
              pEvent->SetStartTime (StartTime);
              pEvent->SetDuration (cit.getDuration ());

              if (ShortEventDescriptor) {
                char buffer[256];
                ShortEventDescriptor->name.getText (buffer, sizeof (buffer));
                if (isOpt) {
                  char buffer2[sizeof (buffer) + 32];
                  snprintf (buffer2, sizeof (buffer2), optPats[SetupPE->OptPat], buffer, optCount);
                  pEvent->SetTitle (buffer2);
                } else
                  pEvent->SetTitle (buffer);
                LogI(2, "title: %s\n", pEvent->Title ());
                pEvent->SetShortText (ShortEventDescriptor->text.getText (buffer, sizeof (buffer)));
              }
              if (ExtendedEventDescriptors) {
                char buffer[ExtendedEventDescriptors->getMaximumTextLength (": ") + 1];
                pEvent->SetDescription (ExtendedEventDescriptors->getText (buffer, sizeof (buffer), ": "));
              }
              if (order || rating) {
                int len = (pEvent->Description ()? strlen (pEvent->Description ()) : 0) +
                  (order ? strlen (order) : 0) + (rating ? strlen (rating) : 0);
                char buffer[len + 32];
                buffer[0] = 0;
                if (pEvent->Description ())
                  strcat (buffer, pEvent->Description ());
                if (rating) {
                  strcat (buffer, rating);
                  pEvent->SetParentalRating(nRating);
                }
                if (order)
                  strcat (buffer, order);
                pEvent->SetDescription (buffer);
              }

              if (newEvent)
                pSchedule->AddEvent (pEvent);

              pEvent->FixEpgBugs ();
              if (pEvent->RunningStatus () != runningStatus)
                pSchedule->SetRunningStatus (pEvent, runningStatus, channel);
              pSchedule->DropOutdated (StartTime, EndTime, Tid, cit.getVersionNumber ());
              Modified = true;
            }
          }
          if (Modified) {
            pSchedule->Sort ();
#if APIVERSNUM >= 20300
            pSchedule->SetModified();
#else
            Schedules->SetModified (pSchedule);
#endif
          }
          delete pct;
        }
      }
      delete ExtendedEventDescriptors;
      delete ShortEventDescriptor;
      free (order);
      free (rating);
    }
  }

}

// --- cPluginEEPG ------------------------------------------------------

class cPluginEEPG:public cPlugin
{
private:
  struct {
    cFilterEEPG *filter;
    cDevice *device;
  } epg[MAXDEVICES];

  void CheckCreateFile(const char* Name, const char *fileContent[]);

public:
  cPluginEEPG (void);
  virtual const char *Version (void) {
    return VERSION;
  }
  virtual const char *Description (void) {
    return tr (DESCRIPTION);
  }
  virtual bool Start (void);
  virtual void Stop (void);
  virtual cMenuSetupPage *SetupMenu (void);
  virtual bool SetupParse (const char *Name, const char *Value);
};

cPluginEEPG::cPluginEEPG (void)
{
  memset (epg, 0, sizeof (epg));
}

void cPluginEEPG::CheckCreateFile(const char* Name, const char *fileContent[])
{
  FILE *File;
  bool isSkyTheme = fileContent == SkyItThemes || fileContent == SkyUkThemes;
  string FileName = string(cSetupEEPG::getInstance()->getConfDir()) + "/" + Name;
  File = fopen(FileName.c_str(), "r");
  if (File == NULL) {
    LogE (0, prep("Error opening file '%s', %s"), FileName.c_str(), strerror (errno));
    File = fopen (FileName.c_str(), "w");
    if (File == NULL) {
      LogE (0, prep("Error creating file '%s', %s"), FileName.c_str(), strerror (errno));
    } else {
      if (!isSkyTheme) {
        int i = 0;
        while (fileContent[i] != NULL) {
          fprintf (File, "%s\n", fileContent[i]);
          i++;
        }
      } else {
        for (int i = 0; i < 256; i++) {
          if (fileContent[i]) {
            fprintf (File, "0x%02x=%s\n", i, fileContent[i]);
          }
          else {
            fprintf (File, "0x%02x=\n", i);
          }
        }
      }

      LogI (0, prep("Success creating file '%s'"), FileName.c_str());
      fclose (File);
    }
  } else {
    fclose (File);
  }
}

bool cPluginEEPG::Start (void)
{
#if APIVERSNUM < 10507
  RegisterI18n (Phrases);
#endif
  for (int i = 0; i < MAXDEVICES; i++) {
    cDevice *dev = cDevice::GetDevice (i);
    if (dev) {
      epg[i].device = dev;
      dev->AttachFilter (epg[i].filter = new cFilterEEPG);
      isyslog ("Attached EEPG filter to device %d", i);
    }
  }
  char *ConfDir = NULL;
  // Initialize any background activities the plugin shall perform.
  DIR *ConfigDir;
  //if (ConfDir == NULL) {
  Asprintf (&ConfDir, "%s/eepg", cPlugin::ConfigDirectory ());
  //}
  ConfigDir = opendir (ConfDir);
  if (ConfigDir == NULL) {
    esyslog ("EEPG: Error opening directory '%s', %s", ConfDir, strerror (errno));
    if (mkdir (ConfDir, 0777) < 0) {
      esyslog ("EEPG: Error creating directory '%s', %s", ConfDir, strerror (errno));
    } else {
      isyslog ("EEPG: Success creating directory '%s'", ConfDir);
    }
  }
  cSetupEEPG::getInstance()->setConfDir(ConfDir);

  CheckCreateFile(EEPG_FILE_EQUIV, FileEquivalences);
  CheckCreateFile("sky_it.dict", SkyItDictionary);
  CheckCreateFile("sky_uk.dict", SkyUkDictionary);
  CheckCreateFile("sky_it.themes", SkyItThemes);
  CheckCreateFile("sky_uk.themes", SkyUkThemes);
  CheckCreateFile("freesat.t1", FreesatT1);
  CheckCreateFile("freesat.t2", FreesatT2);
  load_theme_dictionaries();

  sky_tables[0] = NULL;
  sky_tables[1] = NULL;
  tables[0][0] = NULL;
  //store all available sources, so when a channel is not found on current satellite, we can look for alternate sat positions.
  //perhaps this can be done smarter through existing VDR function???
#if APIVERSNUM >= 20300
  LOCK_CHANNELS_READ;
  for (const cChannel * Channel = Channels->First(); Channel; Channel = Channels->Next(Channel)) {
#else
  for (const cChannel * Channel = Channels.First (); Channel; Channel = Channels.Next (Channel)) {
#endif
    if (!Channel->GroupSep ()) {
      bool found = false;
      for (int i = 0; (i < NumberOfAvailableSources) && (!found); i++)
        found = (Channel->Source () == AvailableSources[i]);
      if (!found)
        AvailableSources[NumberOfAvailableSources++] = Channel->Source ();
    }
  }
  if (CheckLevel(3))
    for (int i = 0; i < NumberOfAvailableSources; i++)
      isyslog ("EEPG: Available sources:%s.", *cSource::ToString (AvailableSources[i]));

#if APIVERSNUM > 10725
  new cEEpgHandler();
#endif
  EquivHandler =  new cEquivHandler();

  if (ConfDir) {
    free (ConfDir);
  }
  closedir(ConfigDir);
  return true;
}

void cPluginEEPG::Stop (void)
{
  for (int i = 0; i < MAXDEVICES; i++) {
    cDevice *dev = epg[i].device;
    if (dev)
      dev->Detach (epg[i].filter);
    delete epg[i].filter;
    epg[i].device = 0;
    epg[i].filter = 0;
  }

  // Clean up after yourself!
//  if (ConfDir) {
//    free (ConfDir);
//  }
  if (sky_tables[0]) {
    free(sky_tables[0]);
  }
  if (sky_tables[1]) {
    free(sky_tables[1]);
  }
  if (EquivHandler) {
      delete EquivHandler;
  }

}

cMenuSetupPage *cPluginEEPG::SetupMenu (void)
{
  return new cMenuSetupPremiereEpg;
}

bool cPluginEEPG::SetupParse (const char *Name, const char *Value)
{

  if (!strcasecmp (Name, "OptionPattern"))
    SetupPE->OptPat = atoi (Value);
  else if (!strcasecmp (Name, "OrderInfo"))
    SetupPE->OrderInfo = atoi (Value);
  else if (!strcasecmp (Name, "RatingInfo"))
    SetupPE->RatingInfo = atoi (Value);
  else if (!strcasecmp (Name, "FixEpg"))
    SetupPE->FixEpg = atoi (Value);
  else if (!strcasecmp (Name, "DisplayMessage"))
    SetupPE->DisplayMessage = atoi (Value);
  else if (!strcasecmp (Name, "ReplaceEmptyShText"))
    SetupPE->ReplaceEmptyShText = atoi (Value);
  else if (!strcasecmp (Name, "FixCharset"))
    SetupPE->FixCharset = atoi (Value);
#ifdef DEBUG
  else if (!strcasecmp (Name, "LogLevel"))
    SetupPE->LogLevel = atoi (Value);
  else if (!strcasecmp (Name, "ProcessEIT"))
    SetupPE->ProcessEIT = atoi (Value);
#endif
  else
    return false;
  return true;
}

VDRPLUGINCREATOR (cPluginEEPG); // Don't touch this!
