/*
 * util.c
 *
 *  Created on: 23.5.2012
 *      Author: d.petrovski
 */
#include "util.h"
#include "log.h"
#include "equivhandler.h"
#include "setupeepg.h"
#include <vdr/channels.h>
#include <vdr/thread.h>
#include <vdr/epg.h>

#include <map>

namespace util
{

int AvailableSources[32];
int NumberOfAvailableSources = 0;

int Yesterday;
int YesterdayEpoch;
int YesterdayEpochUTC;

struct hufftab *tables[2][128];
int table_size[2][128];
map<string,string> tableDict;

cEquivHandler* EquivHandler;

const cChannel *GetChannelByID(const tChannelID & channelID, bool searchOtherPos)
{
#if APIVERSNUM >= 20300
  LOCK_CHANNELS_READ;
  const cChannel *VC = Channels->GetByChannelID(channelID, true);
#else
  const cChannel *VC = Channels.GetByChannelID(channelID, true);
#endif
  if(!VC && searchOtherPos){
    //look on other satpositions
    for(int i = 0;i < NumberOfAvailableSources;i++){
      tChannelID chID = tChannelID(AvailableSources[i], channelID.Nid(), channelID.Tid(), channelID.Sid());
#if APIVERSNUM >= 20300
      VC = Channels->GetByChannelID(chID, true);
#else
      VC = Channels.GetByChannelID(chID, true);
#endif
      if(VC){
        //found this actually on satellite nextdoor...
        break;
      }
    }
  }

  return VC;
}

/*
 * Convert local time to UTC
 */
time_t LocalTime2UTC (time_t t)
{
  struct tm *temp;

  temp = gmtime (&t);
  temp->tm_isdst = -1;
  return mktime (temp);
}

/*
 * Convert UTC to local time
 */
time_t UTC2LocalTime (time_t t)
{
  return 2 * t - LocalTime2UTC (t);
}

void GetLocalTimeOffset (void)
{
  time_t timeLocal;
  struct tm *tmCurrent;

  timeLocal = time (NULL);
  timeLocal -= 86400;
  tmCurrent = gmtime (&timeLocal);
  Yesterday = tmCurrent->tm_wday;
  tmCurrent->tm_hour = 0;
  tmCurrent->tm_min = 0;
  tmCurrent->tm_sec = 0;
  tmCurrent->tm_isdst = -1;
  YesterdayEpoch = mktime (tmCurrent);
  YesterdayEpochUTC = UTC2LocalTime (mktime (tmCurrent));
}

void CleanString (unsigned char *String)
{

//  LogD (1, prep("Unclean: %s"), String);
  unsigned char *Src;
  unsigned char *Dst;
  int Spaces;
  int pC;
  Src = String;
  Dst = String;
  Spaces = 0;
  pC = 0;
  while (*Src) {
    // corrections
    if (*Src == 0x8c) { // iso-8859-2 LATIN CAPITAL LETTER S WITH ACUTE
      *Src = 0xa6;
    }
    if (*Src == 0x8f) { // iso-8859-2 LATIN CAPITAL LETTER Z WITH ACUTE
      *Src = 0xac;
    }

    if (*Src!=0x0A &&  *Src < 0x20) { //don't remove newline
      *Src = 0x20;
    }
    if (*Src == 0x20) {
      Spaces++;
      if (pC == 0) {
        Spaces++;
      }
    } else {
      Spaces = 0;
    }
    if (Spaces < 2) {
      *Dst = *Src;
      Dst++;
      pC++;
    }
    Src++;
  }
  if (Spaces > 0 && String && String < Dst) {
    Dst--;
    *Dst = 0;
  } else {
    *Dst = 0;
  }
//  LogD (1, prep("Clean: %s"), String);
}

struct tChannelIDCompare
{
   bool operator() (const tChannelID& lhs, const tChannelID& rhs) const
   {
     if (lhs.Source() < rhs.Source()) return true;
     bool eq = lhs.Source() == rhs.Source();
     if (eq && lhs.Nid() < rhs.Nid()) return true;
     eq &= lhs.Nid() == rhs.Nid();
     if (eq && lhs.Tid() < rhs.Tid()) return true;
     eq &= lhs.Tid() == rhs.Tid();
     if (eq && lhs.Sid() < rhs.Sid()) return true;
     eq &= lhs.Sid() == rhs.Sid();
     if (eq && lhs.Rid() < rhs.Rid()) return true;
     return false;
   }
};

cTimeMs LastAddEventThread;
enum { INSERT_TIMEOUT_IN_MS = 5000 };

class cAddEventThread : public cThread
{
private:
  cTimeMs LastHandleEvent;
  std::map<tChannelID,cList<cEvent>*,tChannelIDCompare> *map_list;
//  enum { INSERT_TIMEOUT_IN_MS = 10000 };
  void MergeEquivalents(cEvent* dest, const cEvent* src);
protected:
  virtual void Action(void);
public:
  cAddEventThread(void);
  ~cAddEventThread(void);
  void AddEvent(cEvent *Event, tChannelID ChannelID);
};

cAddEventThread::cAddEventThread(void)
:cThread("cAddEEPGEventThread"), LastHandleEvent()
{
  map_list = new std::map<tChannelID,cList<cEvent>*,tChannelIDCompare>;
}

cAddEventThread::~cAddEventThread(void)
{
//  LOCK_THREAD;
  Cancel(3);
  std::map<tChannelID,cList<cEvent>*,tChannelIDCompare>::iterator it;
  for ( it=map_list->begin() ; it != map_list->end(); it++ )
    (*it).second->Clear();
}

void cAddEventThread::Action(void)
{
  SetPriority(19);
  while (Running() && !LastHandleEvent.TimedOut()) {
    std::map<tChannelID, cList<cEvent>*, tChannelIDCompare>::iterator it;

#if APIVERSNUM >= 20300
    LOCK_SCHEDULES_WRITE;
    cSchedules *schedules = Schedules;
#else
    cSchedulesLock SchedulesLock(true, 10);
    cSchedules *schedules = (cSchedules*)(cSchedules::Schedules(SchedulesLock));
#endif
   Lock();

    it = map_list->begin();
    while (schedules && it != map_list->end()) {
      cSchedule *schedule = (cSchedule *) schedules->GetSchedule(
        GetChannelByID((*it).first, false), true);
      while (((*it).second->First()) != NULL) {
        cEvent* event = (*it).second->First();

        cEvent *pEqvEvent = (cEvent *) schedule->GetEvent (event->EventID(), event->StartTime());
        if (pEqvEvent){
          (*it).second->Del(event);
        } else {

           (*it).second->Del(event, false);

           for (const cEvent *ev = schedule->Events()->First(); ev; ev = schedule->Events()->Next(ev)) {
             if (ev->StartTime() > event->EndTime()) {
                   break;
             }
             if (ev && (ev->EventID() == event->EventID() || (event->Title() && strcasecmp(ev->Title(), event->Title()) == 0))
                 && ((event->StartTime() >= ev->StartTime() && event->StartTime() < ev->EndTime())
                 || (ev->StartTime() >= event->StartTime() && ev->StartTime() < event->EndTime()))){
               MergeEquivalents(event, ev);
               schedule->DelEvent((cEvent*)ev);
               break;
             }
           }

           event = schedule->AddEvent(event);
           EpgHandlers.DropOutdated(schedule, event->StartTime(), event->EndTime(), event->TableID(),
             event->Version());

        }
      }
      EpgHandlers.SortSchedule(schedule);
      //sortSchedules(schedules, (*it).first);
      //schedule->Sort();
      delete (*it).second;
      map_list->erase(it);
      it = map_list->begin();

    }
    Unlock();
    cCondWait::SleepMs(10);
  }
}

void cAddEventThread::AddEvent(cEvent *Event, tChannelID ChannelID)
{
  LOCK_THREAD;
  if (map_list->count(ChannelID) == 0) {
      cList<cEvent>* list = new cList<cEvent>;
      list->Add(Event);
      map_list->insert(std::make_pair(ChannelID, list));
  } else {
      map_list->find(ChannelID)->second->Add(Event);
  }
//  LogD (0, prep("AddEventT %s channel: <%s> map size:%d"), Event->Title(), *ChannelID.ToString(), map_list->size());
  LastHandleEvent.Set(INSERT_TIMEOUT_IN_MS);
}

string ExtractAttributes(string text) {
  string attribute;
  size_t apos = 0;
  while ((apos = text.find('\n',apos)) != string::npos) {
    size_t npos = text.find('\n', apos);
    string subs = text.substr(apos, npos - apos);
    if (subs.find(": ") != string::npos)
      attribute += subs;
    apos = npos;
    //LogD(0, prep("ExtractAttribute attribute:%s, apos:%i, pos:%i"),attribute.c_str(), catpos, pos);
  }
  return attribute;

}

inline void cAddEventThread::MergeEquivalents(cEvent* dest, const cEvent* src)
{
  if (!dest->ShortText() || !strcmp(dest->ShortText(),""))
    dest->SetShortText(src->ShortText());

  //Handle the Category and Genre, and optionally future tags
  if (!src->Description() || !strcmp(src->Description(),""))
      return;

  if ((!dest->Description() || strstr(src->Description(),dest->Description()))) {
    dest->SetDescription(src->Description());
  } else if (dest->Description()) {

    string desc = dest->Description() ? dest->Description() : "";
    desc += ExtractAttributes(desc);
    dest->SetDescription (desc.c_str());

  }

}

static cAddEventThread AddEventThread;

// ---

void AddEvent(cEvent *Event, tChannelID ChannelID)
{
//  LogD (0, prep("AddEvent %s channel: <%s>"), Event->Title(), *ChannelID.ToString());
  AddEventThread.AddEvent(Event, ChannelID);
//  if (!AddEventThread.Active())
//     AddEventThread.Start();
  if (!AddEventThread.Active() && LastAddEventThread.TimedOut()){
    LastAddEventThread.Set(INSERT_TIMEOUT_IN_MS * 1.5);
    AddEventThread.Start();
  }

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

void decodeText2 (const unsigned char *from, int len, char *buffer, int buffsize)
{
  if (from[0] == 0x1f) {
    char *temp = freesat_huffman_decode (from, len);
    if (temp) {
      len = strlen (temp);
      len = len < buffsize - 1 ? len : buffsize - 1;
      strncpy (buffer, temp, len);
      buffer[len] = 0;
      free (temp);
      return;
    }
  }

  SI::String convStr;
  SI::CharArray charArray;
  charArray.assign(from, len);
  convStr.setData(charArray, len);
  //LogE(5, prep("decodeText2 from %s - length %d"), from, len);
  convStr.getText(buffer,  buffsize);
  //LogE(5, prep("decodeText2 buffer %s - buffsize %d"), buffer, buffsize);
}

void sortSchedules(cSchedules * Schedules, tChannelID channelID){

  LogD(3, prep("Start sortEquivalent %s"), *channelID.ToString());

  const cChannel *pChannel = GetChannelByID (channelID, false);
  cSchedule *pSchedule;
  if (pChannel) {
    pSchedule = (cSchedule *) (Schedules->GetSchedule(pChannel, true));
      pSchedule->Sort();
#if APIVERSNUM >= 20300
      pSchedule->SetModified();
#else
      Schedules->SetModified(pSchedule);
#endif
    }
  if (EquivHandler->getEquiChanMap().count(*channelID.ToString()) > 0)
    EquivHandler->sortEquivalents(channelID, Schedules);
}

cCharsetFixer::cCharsetFixer()
{
  charsetOverride = getenv("VDR_CHARSET_OVERRIDE");
  if (!charsetOverride) charsetOverride = "ISO6937";
  initialCharset = charsetOverride;
  fixedCharset = "";
  conv_revert = NULL;
  conv_to = NULL;

}

cCharsetFixer::~cCharsetFixer()
{
}

const char* cCharsetFixer::FixCharset(const char* text)
{
  if (!text || !conv_revert || !conv_to) return text;

  //LogD(0, prep("FixCharset fixCharset:%s charsetOverride:%s text:%s"), fixCharset.c_str(), CharsetOverride, text);
  const char* fixed = NULL;
  if (!fixedCharset.empty()) {

    if (fixedCharset != charsetOverride) {
      //LogD(0, prep("FixCharset2 fixCharset:%s charsetOverride:%s text:%s"), fixCharset.c_str(), CharsetOverride, text);
      fixed = conv_revert->Convert(text);
      //LogD(0, prep("conv 1 fixed:%s"),fixed);
      fixed = conv_to->Convert(fixed);
      //LogD(0, prep("Fixed text:%s"), fixed);
    }
  }
  if (!fixed) fixed = text;
  return fixed;

}

void cCharsetFixer::InitCharsets(const cChannel* Channel)
{
  if (!cSetupEEPG::getInstance()->FixCharset) return;

  if (strcasecmp( Channel->Provider(), "Skylink") == 0 || strcasecmp( Channel->Provider(), "UPC Direct") == 0
      || strcasecmp( Channel->Provider(), "CYFRA +") == 0) {
    fixedCharset = "ISO6937";
  } else if (strcasestr( Channel->Provider(), "Polsat") != 0) {
    fixedCharset = "ISO-8859-2";
  } else if (Channel->Nid() == 0x01) {
    fixedCharset = "ISO-8859-9";
  } else {
    fixedCharset = "";
  }


  if (!fixedCharset.empty()) {
    if (conv_to)
       delete conv_to;
    conv_to = new cCharSetConv(fixedCharset.c_str());


    const char* chrs;
    if (strcasecmp( Channel->Provider(), "CYFRA +") == 0) {
      chrs = "ISO-8859-5";
    }else {
      chrs = charsetOverride;
    }

    if (initialCharset != chrs) {
      delete conv_revert;
      conv_revert = NULL;
    }
    if (!conv_revert || initialCharset != chrs)
      conv_revert = new cCharSetConv(NULL,chrs);

    initialCharset = chrs;
  }

}

string findThemeTr(const char* text)
{
  map<string,string>::iterator it;
  string trans = text;
  if ((it = tableDict.find(trans)) != tableDict.end())
    trans = it->second;
  LogD(4, prep("original:%s translated:%s map size:%d"), text, trans.c_str(), tableDict.size());
  return trans;
}

}
