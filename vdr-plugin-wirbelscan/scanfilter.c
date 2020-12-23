/*
 * scanfilter.c: wirbelscan - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 */

/* cNitScanner, cPatScanner, cSdtScanner, cEitScanner are modified versions of core VDR's
 * own classes cEIT, cNitFilter, cSdtFilter.
 * License GPL, original Copyright (C) Klaus Schmidinger, see www.tvdr.de/vdr
 */

#include <string>
#include <libsi/section.h>
#include <libsi/descriptor.h>
#include <vdr/filter.h>
#include <vdr/device.h>
////#include <vdr/channels.h>
#include <vdr/sources.h>
#include <vdr/tools.h>
///#include <vdr/config.h>
#include "scanfilter.h"
#include "common.h"
#include "dvb_wrapper.h"
#include "menusetup.h"
#include "si_ext.h"

using namespace SI_EXT;

extern TSdtData SdtData;
extern TNitData NitData;
TChannels NewChannels;
TChannels NewTransponders;
TChannels ScannedTransponders;

int nextTransponders;

void resetLists() { 
  NewChannels.Clear();
  NewTransponders.Clear();
  ScannedTransponders.Clear();
  SdtData.services.Clear();
  NitData.frequency_list.Clear();
  NitData.cell_frequency_links.Clear();
  NitData.service_types.Clear();
  for(int i = 0; i < NitData.transport_streams.Count(); i++)
     delete NitData.transport_streams[i];
  NitData.transport_streams.Clear();
  nextTransponders = 0;

  NewChannels.Capacity(2500);
  NewTransponders.Capacity(500);
  ScannedTransponders.Capacity(500);
}

bool known_transponder(TChannel* newChannel, bool auto_allowed, TChannels* list) {
  //dlog(4, "%s", __FUNCTION__);
  if (list == NULL) {
     return (known_transponder(newChannel, auto_allowed, &NewTransponders) ||
             known_transponder(newChannel, auto_allowed, &ScannedTransponders));
     }


  for(int idx = 0; idx < list->Count(); ++idx) {
     TChannel* channel = list->Items(idx);

     //std::string s1,s2;
     //newChannel->PrintTransponder(s1);
     //channel->PrintTransponder(s2);
     //dlog(0, "'%s' == '%s'? ", s1.c_str(), s2.c_str());

   /*if (channel->Tested and !channel->Tunable)
        continue;*/

     if (newChannel->Source[0] != channel->Source[0])
        continue;

     char c = newChannel->Source[0];
     if (c == 'T') {
        if (newChannel->DelSys and (channel->StreamId != newChannel->StreamId))
           continue; // may be multiple plps.
        if (newChannel->DelSys != channel->DelSys and !channel->Tunable)
           continue; // skip freqs with T!=T1, but not those which had success.
        if (is_nearly_same_frequency(channel, newChannel))
           return true;
        }
     else if (c == 'C') {
        if (is_nearly_same_frequency(channel, newChannel))
           return true;
        }
     else if (c == 'A') {
       if (is_nearly_same_frequency(channel, newChannel) &&  channel->Modulation == newChannel->Modulation)
          return true;
       }
     else if (c == 'S') {
        if (!is_different_transponder_deep_scan(newChannel, channel, auto_allowed))
           return true;
        }
     else
        dlog(0, "%s: source[0] = 0x%.02X", __FUNCTION__, (unsigned char) c);
     }
  return (false);
}

int FormatFreq(int f) {
  if (f < 1000)   f *= 1000;
  if (f > 999999) f /= 1000;
  return (f);
}

bool is_nearly_same_frequency(const TChannel* chan_a, const TChannel* chan_b, uint delta) {
  uint32_t diff;

  // s: 4000..13000 (MHz)
  // c,t,a: 95000 .. 870000 (kHz)
  int f1 = FormatFreq(chan_a->Frequency);
  int f2 = FormatFreq(chan_b->Frequency);

  if (f1 == f2)
     return true;

  diff = (f1 > f2) ? (f1 - f2) : (f2 - f1);
  //FIXME: use symbolrate etc. to estimate bandwidth
  if (diff <= delta) {
     //dlog(4, "f1 = %u is same TP as f2 = %u", f1, f2);
     return true;
     }
  return (false);
}

bool is_different_transponder_deep_scan(const TChannel* a, const TChannel* b, bool auto_allowed) {
#define IS_DIFFERENT(A, B, _ALLOW_AUTO_, _AUTO_)    ((A != B) && (!_ALLOW_AUTO_ || (_ALLOW_AUTO_ && (A != _AUTO_) && (B != _AUTO_))))

  if (a->Source != b->Source)
     return true;

  char asource = a->Source[0];
  int maxdelta = 500; //kHz

  if (asource == 'S')
     maxdelta = 2; //MHz
   else if (asource == 'T')
     maxdelta = 250; //kHz -> France


  if (!is_nearly_same_frequency(a, b, maxdelta))
     return true;

  switch(asource) {
     case 'T': {
        if (IS_DIFFERENT(a->Modulation, b->Modulation, auto_allowed, 999))
           return true;
        if (IS_DIFFERENT(a->Bandwidth, b->Bandwidth, auto_allowed, 8))
           return true;
        if (IS_DIFFERENT(a->FEC, b->FEC, auto_allowed, 999))
           return true;
        if (IS_DIFFERENT(a->Hierarchy, b->Hierarchy, auto_allowed, 999))
           return true;
        if (IS_DIFFERENT(a->FEC_low, b->FEC_low, auto_allowed, 999))
           return true;
        if (IS_DIFFERENT(a->Transmission, b->Transmission, auto_allowed, 999))
           return true;
        if (IS_DIFFERENT(a->Guard, b->Guard, auto_allowed, 999))
           return true;
        if (IS_DIFFERENT(a->DelSys, b->DelSys, false, 0))
           return true;
        return false;
        }
     case 'A': {
        if (IS_DIFFERENT(a->Modulation, b->Modulation, auto_allowed, 999))
           return true;
        return false;
        }
     case 'C': {
        if (IS_DIFFERENT(a->Modulation, b->Modulation, auto_allowed, 999))
           return true;
        if (IS_DIFFERENT(a->Symbolrate, b->Symbolrate, false, 6900))
           return true;
        if (IS_DIFFERENT(a->FEC, b->FEC, auto_allowed, 999))
           return true;
        if (IS_DIFFERENT(a->DelSys, b->DelSys, false, 0))
           return true;
        return false;
        }
     case 'S': {
        if (IS_DIFFERENT(a->Symbolrate, b->Symbolrate, false, 27500))
           return true;
        if (IS_DIFFERENT(a->Polarization, b->Polarization, false, 0))
           return true;
        if (IS_DIFFERENT(a->FEC, b->FEC, auto_allowed, 999))
           return true;
        if (IS_DIFFERENT(a->DelSys, b->DelSys, false, 0))
           return true;
        if (IS_DIFFERENT(a->Rolloff, b->Rolloff, auto_allowed, 999))
           return true;
        if (IS_DIFFERENT(a->Modulation, b->Modulation, auto_allowed, 999))
           return true;
        return false;
        }
     default:
        dlog(0, "%s: unknown source type '%s'", __FUNCTION__, a->Source.c_str());
     }
  return true;
}


/*******************************************************************************
 * TThread
 ******************************************************************************/
TThread::TThread() : running(false) {}

TThread:: ~TThread() {}

bool TThread::Start() {
  running = true;
  if (pthread_create(&thread, NULL, (void* (*)(void*)) &start_routine, (void*) this) == 0) {
     // When a detached thread terminates, its resources are *automatically*
     // released back to the system without the need for another thread to
     // join with the terminated thread.
     pthread_detach(thread);
     }
  else {
     running = false;
     return false;
     }
  return true;
}

void* TThread::start_routine(TThread* Thread) {
  int i;

  //ENABLING THE CANCEL FUNCTIONALITY
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &i);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &i);

  Thread->Action();
  Thread->running = false;
  return NULL;
}

void TThread::Cancel(int WaitSeconds) {
  if (!running) return;
  running = false;
}


/*******************************************************************************
 * cPatScanner
 ******************************************************************************/

cPatScanner::cPatScanner(cDevice* Parent, struct TPatData& Dest) :
  device(Parent), PatData(Dest), isActive(true)
{
  PatData.services.Clear();
  PatData.network_PID = 0;
  hasPAT = false;
  Sync.Reset();
  Start();
}

cPatScanner::~cPatScanner() {
  isActive = false;
  wait.Signal();
}

void cPatScanner::Action(void) {
  int count = 0;
  int nbytes = 0;
  int fd = device->OpenFilter(PID_PAT, TABLE_ID_PAT, 0xFF);
  unsigned char buffer[4096];

  while(Running() && isActive) {
     if (wait.Wait(10)) {
        dlog(0, "received signal (%p)", this);
        break;
        }
     if (count++ > 500) { // > 5sec
        isActive = false;
        break;
        }
     nbytes = device->ReadFilter(fd, buffer, sizeof(buffer));
     if (nbytes > 0)
        Process(buffer, nbytes);
     }

  isActive = false;
  device->CloseFilter(fd);
  fd = -1;
}


void cPatScanner::Process(const u_char* Data, int Length) {
  SI::PAT tsPAT(Data, false);
  if (!tsPAT.CheckCRCAndParse()) {
     hexdump("PAT CRC error", Data, Length);
     return;
     }

  if (!Sync.Sync(tsPAT.getVersionNumber(), tsPAT.getSectionNumber(), tsPAT.getLastSectionNumber()))
     return;

  if (wSetup.verbosity > 5)
     hexdump("PAT", Data, Length);
  SI::PAT::Association assoc;

  for (SI::Loop::Iterator it; tsPAT.associationLoop.getNext(assoc, it);) {
     if (assoc.getServiceId() == 0) {
        PatData.network_PID = assoc.getPid();
        continue;
        }
     struct service s;

     hasPAT = true;
     s.transport_stream_id = tsPAT.getTransportStreamId();
     s.program_map_PID = assoc.getPid();
     s.program_number = assoc.getServiceId();
     PatData.services.Add(s);
     }

  // all parts of PAT seen.
  if (tsPAT.getSectionNumber() == tsPAT.getLastSectionNumber())
     isActive = false;
}


/*******************************************************************************
 * cPmtScanner
 ******************************************************************************/

cPmtScanner::cPmtScanner(cDevice* Parent, TPmtData* Data) :
  device(Parent), data(Data), isActive(false), jobDone(false)
{
  data->program_number = 0;
  data->PCR_PID = 0;
  data->Vpid.PID = 0;
  data->Tpid = 0;
  data->Apids.Clear();
  data->Dpids.Clear();
  data->Spids.Clear();
  data->Caids.Clear();
}

cPmtScanner::~cPmtScanner() {
  isActive = false;
  wait.Signal();
}

void cPmtScanner::Action(void) {
  isActive = true;
  int count = 0;
  int nbytes = 0;
  int fd = device->OpenFilter(data->program_map_PID, TABLE_ID_PMT, 0xFF);
  unsigned char buffer[4096];

  while (Running() && isActive) {
     if (wait.Wait(10)) {
        break;
        }
     if (count++ > 500) { //>5sec
        isActive = false;
        break;
        }
     nbytes = device->ReadFilter(fd, buffer, sizeof(buffer));
     if (nbytes > 0)
        Process(buffer, nbytes);
     }

  jobDone = true;
  isActive = false;
  device->CloseFilter(fd);
  fd = -1;
}

void cPmtScanner::Process(const u_char* Data, int Length) {

  SI::PMT pmt(Data, false);
  if (!pmt.CheckCRCAndParse()/* || (pmt.getServiceId() != pmtSid)*/)
     return;

  data->program_number = pmt.getServiceId();

  SI::CaDescriptor* d;
  // Scan the common loop:
  for(SI::Loop::Iterator it; (d = (SI::CaDescriptor *) pmt.commonDescriptors.getNext(it, SI::CaDescriptorTag));) {
     int ca = d->getCaType();
     if (data->Caids.IndexOf(ca) < 0)
        data->Caids.Add(ca);
     DELETENULL(d);
     }

  SI::PMT::Stream stream;
  // Scan the stream-specific loop:
  for(SI::Loop::Iterator it; pmt.streamLoop.getNext(stream, it);) {
     int StreamType = stream.getStreamType();
     switch(StreamType) {
        case STREAMTYPE_11172_VIDEO:
        case STREAMTYPE_13818_VIDEO:
        case STREAMTYPE_14496_VISUAL:
        case STREAMTYPE_14496_H264_VIDEO:
        case STREAMTYPE_23008_H265_VIDEO:
           data->Vpid.PID  = stream.getPid();
           data->Vpid.Type = StreamType;
           data->PCR_PID   = pmt.getPCRPid();
           break;
        case STREAMTYPE_11172_AUDIO:
        case STREAMTYPE_13818_AUDIO:
        case STREAMTYPE_13818_AUDIO_ADTS: 
        case STREAMTYPE_14496_AUDIO_LATM:
        case STREAMTYPE_14496_H264_AUDIO:
           {
           TPid apid;
           apid.PID = stream.getPid();
           apid.Type = StreamType;
           SI::Descriptor * d;
           for(SI::Loop::Iterator it; (d = stream.streamDescriptors.getNext(it));) {
              switch(d->getDescriptorTag()) {
                 case SI::ISO639LanguageDescriptorTag: {
                    SI::ISO639LanguageDescriptor* ld = (SI::ISO639LanguageDescriptor*) d;
                    SI::ISO639LanguageDescriptor::Language l;
                    char b[16], *s = b;
                    int n = 0;
                    for(SI::Loop::Iterator it; ld->languageLoop.getNext(l, it);) {
                       if (*ld->languageCode != '-') {
                          // some use "---" to indicate "none"
                          if (n > 0)
                             *s++ = '+';
                          strn0cpy(s, I18nNormalizeLanguageCode(l.languageCode), MAXLANGCODE1);
                          s += strlen(s);
                          if (n++ > 1)
                             break;
                          }
                       }
                    apid.Lang = b;
                    }
                    break;
                 default:;
                 }
              DELETENULL(d);
              }
           data->Apids.Add(apid);
           }
           break;
        case STREAMTYPE_13818_PRIVATE:
        case STREAMTYPE_13818_PES_PRIVATE: {
           TPid dpid; 
           dpid.PID = 0;
           dpid.Type = 0;
           char lang[MAXLANGCODE1] = {0};
           SI::Descriptor * d;

           for(SI::Loop::Iterator it; (d = stream.streamDescriptors.getNext(it));) {
              switch (d->getDescriptorTag()) {
                 case SI::AC3DescriptorTag:
                 case EnhancedAC3DescriptorTag:
                 case DTSDescriptorTag:
                 case AACDescriptorTag:
                    dpid.PID = stream.getPid();
                    dpid.Type = d->getDescriptorTag();
                    break;
                 case SI::SubtitlingDescriptorTag: {
                    char b[16], *s = b;
                    TPid spid;
                    spid.PID = stream.getPid();
                    SI::SubtitlingDescriptor* sd = (SI::SubtitlingDescriptor*) d;
                    SI::SubtitlingDescriptor::Subtitling sub;
                    int n = 0;
                    for(SI::Loop::Iterator it; sd->subtitlingLoop.getNext(sub, it);) {
                       if (sub.languageCode[0]) {
                          if (n > 0)
                             *s++ = '+';
                          strn0cpy(s, I18nNormalizeLanguageCode(sub.languageCode), MAXLANGCODE1);
                          s += strlen(s);
                          if (n++ > 1)
                             break;
                          }
                       }
                    spid.Lang = b;
                    data->Spids.Add(spid);
                    }
                    break;
                 case SI::TeletextDescriptorTag:
                    data->Tpid = stream.getPid();
                    break;
                 case SI::ISO639LanguageDescriptorTag: {
                    SI::ISO639LanguageDescriptor* ld = (SI::ISO639LanguageDescriptor*) d;
                    strn0cpy(lang, I18nNormalizeLanguageCode(ld->languageCode), MAXLANGCODE1);
                    }
                    break;
                 default:;
                 }
              DELETENULL(d);
              }
           if (dpid.PID) {
              dpid.Lang = lang;
              data->Dpids.Add(dpid);
              }
           }
           break;
        case STREAMTYPE_13818_USR_PRIVATE_81: {
           //if (Channel->Source[0] == 'A') {
           char dlang[MAXLANGCODE1] = { 0 };
           SI::Descriptor *d;
           for(SI::Loop::Iterator it; (d = stream.streamDescriptors.getNext(it)); ) {
              switch(d->getDescriptorTag()) {
                 case SI::ISO639LanguageDescriptorTag: {
                    SI::ISO639LanguageDescriptor* ld = (SI::ISO639LanguageDescriptor*) d;
                    strn0cpy(dlang, I18nNormalizeLanguageCode(ld->languageCode), MAXLANGCODE1);
                    }
                    break;
                 default:;
                 }
              DELETENULL(d);
              }
           
           TPid dpid;
           dpid.PID = stream.getPid();
           dpid.Type = SI::AC3DescriptorTag;
           dpid.Lang = dlang;
           data->Dpids.Add(dpid);
           //   }
           }
           break;
        default:;
        }
     for(SI::Loop::Iterator it; (d = (SI::CaDescriptor*) stream.streamDescriptors.getNext(it, SI::CaDescriptorTag));) {
        int ca = d->getCaType();
        if (data->Caids.IndexOf(ca) < 0)
           data->Caids.Add(ca);
        DELETENULL(d);
        }
     }

  isActive = false;
}


/*******************************************************************************
 * cNitScanner
 * basically this is cNitFilter from older vdr/nit.{h,c} with some changes
 ******************************************************************************/

cNitScanner::cNitScanner(cDevice* Parent, uint16_t network_PID, TNitData& Data, int Type) :
  active(true), device(Parent), nit(network_PID), data(Data), type(Type)
{
  first_crc32 = 0;
  Start();
}

cNitScanner::~cNitScanner() {
  active = false;
  wait.Signal();
}

void cNitScanner::Action(void) {
  int count = 0;
  int nbytes = 0;
  int fd = device->OpenFilter(nit, TABLE_ID_NIT_ACTUAL, 0xFF);
  unsigned char buffer[4096];

  while(Running() && active) {
     if (wait.Wait(10)) {
        break;
        }
     if (count++ > 4000)     // 4000 x 10msec = 40sec
        active = false;
     nbytes = device->ReadFilter(fd, buffer, sizeof(buffer));
     if (nbytes > 0)
        Process(buffer, nbytes);  
     }
  active = false;
  device->CloseFilter(fd);
  Cancel();
}

/* std::sort */
bool operator<(TCell const& lhs, TCell const& rhs) {
  return lhs.cell_id < rhs.cell_id;
}

/* std::sort */
void swap(TCell& a, TCell& b) {
  TCell c;
  memcpy(&c, &a, sizeof(TCell));
  memcpy(&a, &b, sizeof(TCell));
  memcpy(&b, &c, sizeof(TCell));
}

/* std::sort */
bool operator<(TFrequencyListItem const& lhs, TFrequencyListItem const& rhs) {
  return ((lhs.network_id < rhs.network_id) or (lhs.frequency < rhs.frequency));
}

/* std::sort */
void swap(TFrequencyListItem& a, TFrequencyListItem& b) {
  TFrequencyListItem c;
  memcpy(&c, &a, sizeof(TFrequencyListItem));
  memcpy(&a, &b, sizeof(TFrequencyListItem));
  memcpy(&b, &c, sizeof(TFrequencyListItem));
}




void cNitScanner::ParseCellFrequencyLinks(uint16_t network_id, const u_char* Data, TList<TCell>& list) {
  int len = 2 + *(Data + 1);
  int offset = 2;
  TCell c;

  c.network_id = network_id;

  if (wSetup.verbosity > 5)
     hexdump(__PRETTY_FUNCTION__, Data, len);

  while(len >= 7) {
     c.cell_id = Data[offset + 0] << 8 |
                 Data[offset + 1];
     c.frequency = (Data[offset + 2] << 24 |
                    Data[offset + 3] << 16 |
                    Data[offset + 4] << 8  |
                    Data[offset + 5]) * 10;
     c.subcellcount = Data[offset + 6] / 5;
     offset += 7;
     len -= 7;
     
     for(int i = 0; i < c.subcellcount; i++) {
        c.subcells[i].cell_id_extension = Data[offset];
        c.subcells[i].transposer_frequency = (Data[offset + 1] << 24 |
                                              Data[offset + 2] << 16 |
                                              Data[offset + 3] << 8  |
                                              Data[offset + 4]) * 10;
        offset += 5;
        len -= 5;
        }
     
     bool found = false;
     for(int i = 0; i < list.Count(); i++) {
        if ((list[i].cell_id == c.cell_id) and
            (list[i].network_id == c.network_id)) {
           found = true;
           break;
           }
        }

     if (!found) {
        list.Add(c);
        list.Sort();
        }
     } 
}

void cNitScanner::Process(const u_char* Data, int Length) {
  SI::NIT nit(Data, false);

  if (!nit.CheckCRCAndParse())
     return;

  if (Data[0] != TABLE_ID_NIT_ACTUAL and
      Data[0] != TABLE_ID_NIT_OTHER)
     return;

  int len = nit.getLength();
  uint32_t crc32 = Data[len-4] << 24 | Data[len-3] << 16 | Data[len-2] << 8 | Data[len-1];

  if (first_crc32 == crc32) {
     active = false;
     return;
     }

  if (!first_crc32)
     first_crc32 = crc32;

  HEXDUMP(Data, Length);
  SI::NIT::TransportStream ts;
  for(SI::Loop::Iterator it; nit.transportStreamLoop.getNext(ts, it);) {
     SI::Descriptor* d;
     SI::Loop::Iterator it2;
     SI::FrequencyListDescriptor* fld = (SI::FrequencyListDescriptor*) ts.transportStreamDescriptors.getNext(it2, SI::FrequencyListDescriptorTag);
     if (fld) {
        int ct = fld->getCodingType();
        if (ct > 0) {
           for(SI::Loop::Iterator it3; fld->frequencies.hasNext(it3);) {
              uint32_t f = fld->frequencies.getNext(it3);
              switch(ct) {
                 case 1: f = BCD2INT(f) / 100;
                    break;                                  //satellite
                 case 2: f = BCD2INT(f) / 10;
                    break;                                  //cable
                 case 3: f *= 10;
                    break;                                  //terrestrial
                 default:;
                 }

              bool found = false;
              for(int i = 0; i < data.frequency_list.Count(); i++) {
                 if (data.frequency_list[i].frequency == f) {
                    found = true;
                    break;
                    }
                 }
              if (!found) {
                 TFrequencyListItem item;
                 item.network_id = nit.getNetworkId();
                 item.frequency = f;
                 data.frequency_list.Add(item);
                 data.frequency_list.Sort();
                 }
              }
           }
        }
     DELETENULL(fld);
     // dirty hack because libsi is missing needed cell_frequency_link_descriptor
     // and support is only possible with patching libsi :-((
     //  -> has to be removed as soon libsi supports cell_frequency_link_descriptor
     
     int offset = 16 + (((*(Data + 8) << 8) & 0x0F00) | *(Data + 9));
     int stop   = ((*(Data + offset) << 8) & 0x0F00) | *(Data + offset + 1);
     
     offset += 2;         // Transport_descriptor_length
     stop   += offset;

     while (offset < stop) {
        int len = *(Data + offset + 1);
        switch(*(Data + offset)) {
           case SI::CellFrequencyLinkDescriptorTag: { // cell_frequency_link_descriptor, DVB-T/T2 only.
              ParseCellFrequencyLinks(nit.getNetworkId(), Data + offset, data.cell_frequency_links);
              offset += 2 + *(Data + offset + 1);
              break;
              }
           case SI::CellListDescriptorTag:
              dlog(0, "SI::CellListDescriptorTag in first loop.");
              /* Falls through. */
           default:
              offset += len + 2; // all other descriptors handled regularly
           }
        if (!len)
           break;
        }
    // end dirty hack

     for(SI::Loop::Iterator it2; (d = ts.transportStreamDescriptors.getNext(it2));) {
        switch((unsigned) d->getDescriptorTag()) {
           case SI::SatelliteDeliverySystemDescriptorTag: {
              if (type != SCAN_SATELLITE)
                 continue;
              SI::SatelliteDeliverySystemDescriptor * sd = (SI::SatelliteDeliverySystemDescriptor *) d;
              int Source  = cSource::FromData(cSource::stSat, BCD2INT(sd->getOrbitalPosition()), sd->getWestEastFlag());
              int RollOff = 35;
              int Modulation = 2;
              int System = 0;

              if ((System = sd->getModulationSystem())) {
                 // {DVB-S2}
                 if      (sd->getModulationType() == 2) Modulation = 5;
                 else if (sd->getModulationType() == 3) Modulation = 16;

                 if      (sd->getRollOff() == 1) RollOff = 25;
                 else if (sd->getRollOff() == 2) RollOff = 20;
                 }
              uint32_t Frequency = BCD2INT(sd->getFrequency()) / 100;
              char Polarization = 'H';
              if      (sd->getPolarization() == 1) Polarization = 'V';
              else if (sd->getPolarization() == 2) Polarization = 'L';
              else if (sd->getPolarization() == 3) Polarization = 'R';

              int CodeRate;
              switch(sd->getFecInner()) {
                 case 1 : CodeRate = 12;  break;
                 case 2 : CodeRate = 23;  break;
                 case 3 : CodeRate = 34;  break;
                 case 4 : CodeRate = 56;  break;
                 case 5 : CodeRate = 78;  break;
                 case 6 : CodeRate = 89;  break;
                 case 7 : CodeRate = 35;  break;
                 case 8 : CodeRate = 45;  break;
                 case 9 : CodeRate = 910; break;
                 case 15: CodeRate = 0;   break;
                 default: CodeRate = 999;
                 }
              uint32_t SymbolRate = BCD2INT(sd->getSymbolRate()) / 10;

              TChannel* transponder = new TChannel;
              transponder->NID = nit.getNetworkId();
              transponder->ONID = ts.getOriginalNetworkId();
              transponder->TID = ts.getTransportStreamId();
              transponder->Source = *cSource::ToString(Source);
              transponder->Frequency = Frequency;
              transponder->Symbolrate = SymbolRate;
              transponder->Polarization = Polarization;
              transponder->Inversion = 999;
              transponder->FEC = CodeRate;
              transponder->Modulation = Modulation;
              transponder->DelSys = System;
              transponder->Rolloff = RollOff;
              transponder->OrbitalPos = BCD2INT(sd->getOrbitalPosition());
              if (!sd->getWestEastFlag())
                 transponder->OrbitalPos *= -1;

              bool found = false;
              for(int i = 0; !found and i < data.transport_streams.Count(); i++) {
                 std::string t;
                 transponder->PrintTransponder(t);
                 TChannel* ts = data.transport_streams[i];
                 ts->PrintTransponder(s);
                 if (s == t and ts->TID == transponder->TID and
                    (ts->NID == transponder->NID or ts->ONID == transponder->ONID))
                    found = true;
                 }
              if (!found)
                 data.transport_streams.Add(transponder);
              } // end SI::SatelliteDeliverySystemDescriptorTag
              break;

           case SI::S2SatelliteDeliverySystemDescriptorTag: { // only interesting if NBC-BS
              if (type != SCAN_SATELLITE)
                 continue;

              SI::S2SatelliteDeliverySystemDescriptor * sd = (SI::S2SatelliteDeliverySystemDescriptor *) d;
              #if 0                                                               //i have no idea wether i need the scrambling index and if so for what.., 0 is default anyway.
              int scrambling_sequence_index = (sd->getScramblingSequenceSelector()) ? sd->getScramblingSequenceIndex() : 0;
              #endif
              int DVBS_backward_compatibility = sd->getBackwardsCompatibilityIndicator();
              if (DVBS_backward_compatibility) {
                 // okay: we should add a dvb-s transponder
                 // with same source, same polarization and QPSK to list of transponders
                 // if this transponder isn't already marked as known.
                 //
                 }
                 // now we should re-check wether this s2 transponder is really known//
              } // end SI::S2SatelliteDeliverySystemDescriptorTag
              break;

           case SI::CableDeliverySystemDescriptorTag: {
              if (type != SCAN_CABLE)
                 continue;

              SI::CableDeliverySystemDescriptor* sd = (SI::CableDeliverySystemDescriptor*) d;

              uint32_t Frequency = BCD2INT(sd->getFrequency()) / 10;
              int CodeRate;
              switch(sd->getFecInner()) {
                 case 1 : CodeRate = 12;  break;
                 case 2 : CodeRate = 23;  break;
                 case 3 : CodeRate = 34;  break;
                 case 4 : CodeRate = 56;  break;
                 case 5 : CodeRate = 78;  break;
                 case 6 : CodeRate = 89;  break;
                 case 7 : CodeRate = 35;  break;
                 case 8 : CodeRate = 45;  break;
                 case 9 : CodeRate = 910; break;
                 case 15: CodeRate = 0;   break;
                 default: CodeRate = 999;
                 }
              int Modulation;
              switch(sd->getModulation()) {
                 case 1 : Modulation = 16;  break;
                 case 2 : Modulation = 32;  break;
                 case 3 : Modulation = 64;  break;
                 case 4 : Modulation = 128; break;
                 case 5 : Modulation = 256; break;
                 default: Modulation = 999;
                 }
              TChannel* transponder = new TChannel;
              uint32_t SymbolRate = BCD2INT(sd->getSymbolRate()) / 10;
              transponder->NID = nit.getNetworkId();
              transponder->ONID = ts.getOriginalNetworkId();
              transponder->TID = ts.getTransportStreamId();
              transponder->Source = "C";
              transponder->Frequency = Frequency;
              transponder->Symbolrate = SymbolRate;
              transponder->Inversion = 999;
              transponder->FEC = CodeRate;
              transponder->Modulation = Modulation;

              bool found = false;
              for(int i = 0; !found and i < data.transport_streams.Count(); i++) {
                 std::string t;
                 transponder->PrintTransponder(t);
                 TChannel* ts = data.transport_streams[i];
                 ts->PrintTransponder(s);
                 if (s == t and ts->TID == transponder->TID and
                    (ts->NID == transponder->NID or ts->ONID == transponder->ONID))
                    found = true;
                 }
              if (!found)
                 data.transport_streams.Add(transponder);
              } // end SI::CableDeliverySystemDescriptorTag
              break;

           case SI::TerrestrialDeliverySystemDescriptorTag: {
              if (type != SCAN_TERRESTRIAL)
                 continue;

              SI::TerrestrialDeliverySystemDescriptor* sd = (SI::TerrestrialDeliverySystemDescriptor*) d;
              int Source = cSource::FromData(cSource::stTerr);

              uint32_t Frequency = sd->getFrequency() * 10;
              int Bandwidth = 8;
              if      (sd->getBandwidth() == 1) Bandwidth = 7;
              else if (sd->getBandwidth() == 2) Bandwidth = 6; 
              else if (sd->getBandwidth() == 3) Bandwidth = 5;

              int Modulation = 999;
              if      (sd->getConstellation() == 0) Modulation = 2;
              else if (sd->getConstellation() == 1) Modulation = 16;
              else if (sd->getConstellation() == 2) Modulation = 64;

              int Hierarchy = 0;
              if      (sd->getHierarchy() == 1) Hierarchy = 1;
              else if (sd->getHierarchy() == 2) Hierarchy = 2;
              else if (sd->getHierarchy() == 3) Hierarchy = 4;

              int CodeRateHP = 999;
              switch(sd->getCodeRateHP()) {
                 case 0 : CodeRateHP = 12; break;
                 case 1 : CodeRateHP = 23; break;
                 case 2 : CodeRateHP = 34; break;
                 case 3 : CodeRateHP = 56; break;
                 case 4 : CodeRateHP = 78; break;
                 default: CodeRateHP = 999;
                 }

              int CodeRateLP = 0;
              switch(sd->getCodeRateLP()) {
                 case 0 : CodeRateLP = 12; break;
                 case 1 : CodeRateLP = 23; break;
                 case 2 : CodeRateLP = 34; break;
                 case 3 : CodeRateLP = 56; break;
                 case 4 : CodeRateLP = 78; break;
                 default: CodeRateLP = 999;
                 }

              int GuardInterval = 999;
              switch(sd->getGuardInterval()) {
                 case 0 : GuardInterval = 32; break;
                 case 1 : GuardInterval = 16; break;
                 case 2 : GuardInterval = 8;  break;
                 case 3 : GuardInterval = 4;  break;
                 default:;
                 }

              int TransmissionMode = 999;
              switch(sd->getTransmissionMode()) {
                 case 0 : TransmissionMode = 2; break;
                 case 1 : TransmissionMode = 8; break;
                 case 2 : TransmissionMode = 4; break;
                 default:;
                 }

              TChannel* transponder = new TChannel;
              transponder->NID = nit.getNetworkId();
              transponder->ONID = ts.getOriginalNetworkId();
              transponder->TID = ts.getTransportStreamId();
              transponder->Source = *cSource::ToString(Source);
              transponder->Frequency = Frequency;
              transponder->Symbolrate = 27500;
              transponder->Inversion = 999;
              transponder->Bandwidth = Bandwidth;
              transponder->FEC = CodeRateHP;
              transponder->FEC_low = CodeRateLP;
              transponder->Modulation = Modulation;
              transponder->DelSys = 0;
              transponder->Transmission = TransmissionMode;
              transponder->Guard = GuardInterval;
              transponder->Hierarchy = Hierarchy;

              bool found = false;
              for(int i = 0; !found and i < data.transport_streams.Count(); i++) {
                 std::string t;
                 transponder->PrintTransponder(t);
                 TChannel* ts = data.transport_streams[i];
                 ts->PrintTransponder(s);
                 if (s == t and ts->TID == transponder->TID and
                    (ts->NID == transponder->NID or ts->ONID == transponder->ONID))
                    found = true;
                 }
              if (!found)
                 data.transport_streams.Add(transponder);
              } // end SI::TerrestrialDeliverySystemDescriptorTag
              break;
           case SI::ExtensionDescriptorTag: {
              SI::ExtensionDescriptor* sd = (SI::ExtensionDescriptor*) d;
              switch(sd->getExtensionDescriptorTag()) {
                 case SI::T2DeliverySystemDescriptorTag: {
                    if (type != SCAN_TERRESTRIAL)
                       continue;

                    SI::T2DeliverySystemDescriptor* td = (SI::T2DeliverySystemDescriptor*) d;
                    TChannel* transponder = new TChannel;
                    transponder->NID        = nit.getNetworkId();
                    transponder->ONID       = ts.getOriginalNetworkId();
                    transponder->TID        = ts.getTransportStreamId();
                    transponder->Source     = "T";
                    transponder->DelSys     = 1;
                    transponder->Frequency  = 0;
                    transponder->Symbolrate = 27500;
                    transponder->Inversion  = 999;
                    transponder->Modulation = 999;      // not transmitted in SI
                    transponder->Hierarchy  = 0;        // not in use.
                    transponder->FEC        = 999;      // not transmitted in SI
                    transponder->FEC_low    = 0;        // not transmitted in SI
                    transponder->SystemId   = td->getT2SystemId();
                    transponder->StreamId   = td->getPlpId();
                    transponder->reported = false;

                    int Bandwidth = 8;
                    if      (td->getBandwidth() == 1) Bandwidth = 7;
                    else if (td->getBandwidth() == 2) Bandwidth = 6;
                    else if (td->getBandwidth() == 3) Bandwidth = 5;
                    else if (td->getBandwidth() == 4) Bandwidth = 10;
                    else if (td->getBandwidth() == 5) Bandwidth = 1712;
                    transponder->Bandwidth = Bandwidth;

                    int GuardInterval = 999;
                    switch(td->getGuardInterval()) {
                       case 0: GuardInterval = 32;    break;
                       case 1: GuardInterval = 16;    break;
                       case 2: GuardInterval = 8;     break;
                       case 3: GuardInterval = 4;     break;
                       case 4: GuardInterval = 128;   break;
                       case 5: GuardInterval = 19128; break;
                       case 6: GuardInterval = 19256; break;
                       default:;
                       }
                    transponder->Guard = GuardInterval;

                    int TransmissionMode = 999;
                    switch(td->getTransmissionMode()) {
                       case 0: TransmissionMode = 2;  break;
                       case 1: TransmissionMode = 8;  break;
                       case 2: TransmissionMode = 4;  break;
                       case 3: TransmissionMode = 1;  break;
                       case 4: TransmissionMode = 16; break;
                       case 5: TransmissionMode = 32; break;
                       default:;
                       }
                    transponder->Transmission = TransmissionMode;

                    int N = td->getLength() - 8;
                    transponder->cells.Clear();
                    if (N > 0) {
                       const unsigned char* bytes = d->getData().getData() + 8;
                       //hexdump("T2 freq loop", bytes, N);
                       while(N > 0) {
                          struct cell cell;
                          cell.num_center_frequencies = 0;
                          cell.num_transposers = 0;

                          cell.cell_id  = *bytes++ << 8;
                          cell.cell_id |= *bytes++;
                          N-=2;

                          if (td->getTfsFlag() > 0) {
                             int frequency_loop_length = *bytes++;
                             while(frequency_loop_length > 0) {
                                uint32_t cf;
                                cf  = *bytes++ << 24;
                                cf |= *bytes++ << 16;
                                cf |= *bytes++ << 8;
                                cf |= *bytes++;
                                frequency_loop_length -= 4;
                                N-=4;
                                cell.center_frequencies[cell.num_center_frequencies++] = cf * 10;
                                }
                             }
                          else {
                             uint32_t cf;
                             cf  = *bytes++ << 24;
                             cf |= *bytes++ << 16;
                             cf |= *bytes++ << 8;
                             cf |= *bytes++;
                             N-=4;
                             cell.center_frequencies[cell.num_center_frequencies++] = cf * 10;
                             }

                          uint8_t subcell_info_loop_length = *bytes++;
                          N-=1;

                          while(subcell_info_loop_length > 0) {
                             if (cell.num_transposers > 15) continue;
                             cell.transposers[cell.num_transposers].cell_id_extension = *bytes++;
                             uint32_t tf;
                             tf  = *bytes++ << 24;
                             tf |= *bytes++ << 16;
                             tf |= *bytes++ << 8;
                             tf |= *bytes++;
                             cell.transposers[cell.num_transposers].transposer_frequency = tf * 10;
                             cell.num_transposers++;
                             N -= 5;
                             }
                          transponder->cells.Add(cell);
                          }
                       }
                    if (transponder->cells.Count() > 0)                                    
                       transponder->Frequency = transponder->cells[0].center_frequencies[0];


                    bool found = false;
                    for(int i = 0; !found and i < data.transport_streams.Count(); i++) {
                       std::string t;
                       transponder->PrintTransponder(t);
                       TChannel* ts = data.transport_streams[i];
                       ts->PrintTransponder(s);
                       if (s == t and ts->TID == transponder->TID and
                          (ts->NID == transponder->NID or ts->ONID == transponder->ONID) and
                           ts->cells.Count() == transponder->cells.Count())
                          found = true;
                       }
                    if (!found)
                       data.transport_streams.Add(transponder);
                    } // SI::T2DeliverySystemDescriptorTag
                    break; // end T2 delsys
                 default:;
                 }
              } // SI::ExtensionDescriptorTag
              break;
           case SI::ServiceListDescriptorTag: {
              SI::ServiceListDescriptor* sd = (SI::ServiceListDescriptor*) d;
              SI::ServiceListDescriptor::Service Service;
              for(SI::Loop::Iterator it; sd->serviceLoop.getNext(Service, it);) {
                 TServiceListItem item;
                 item.network_id = nit.getNetworkId();
                 item.original_network_id = ts.getOriginalNetworkId();
                 item.transport_stream_id = ts.getTransportStreamId();
                 item.service_id = Service.getServiceId();
                 item.service_type = Service.getServiceType();
                 bool found = false;

                 for(int i = 0; i < data.service_types.Count(); i++) {
                    if ((data.service_types[i].service_id == item.service_id) and 
                        (data.service_types[i].network_id == item.network_id)) {
                       found = true;
                       break;
                       }
                    }
                 if (!found)
                    data.service_types.Add(item);
                 }
              } // end SI::ServiceListDescriptorTag
              break;
           case SI::CellFrequencyLinkDescriptorTag:
              if (type != SCAN_TERRESTRIAL)
                 continue;

              ParseCellFrequencyLinks(nit.getNetworkId(), d->getData().getData(), data.cell_frequency_links);
              break; // not implemented in libsi
           case SI::CellListDescriptorTag:
              if (type != SCAN_TERRESTRIAL)
                 continue;

              dlog(0, "SI::CellListDescriptorTag in second loop.");
              break;
           case SI::FrequencyListDescriptorTag:
              break; // already handled
           case SI::PrivateDataSpecifierDescriptorTag:
              break; // not usable
           case 0x80 ... 0xFE:
              break; // user defined 
           default:
              dlog(5, "   NIT: unknown descriptor tag 0x%.2x", d->getDescriptorTag());
           }
        DELETENULL(d);
        } // end TS descriptor loop
     } // end TS stream loop

  // we have all parts of nit seen.
  if (nit.getSectionNumber() == nit.getLastSectionNumber())
     active = false;
}












//--------TTransponders------------------------------------------------------------------------
// 
// TChannel* TTransponders::GetByParams(const TChannel* NewTransponder) {
//   if (v.size() < 1)
//      return NULL;
// 
//   for(size_t idx = 0; idx < v.size(); ++idx) {
//      TChannel* tr = v[idx];
//      if (!is_different_transponder_deep_scan(tr, NewTransponder, true))
//         return tr;
//      }
//   return (NULL);
// }
// 
// bool TTransponders::IsUniqueTransponder(const TChannel* NewTransponder) {
//   return (GetByParams(NewTransponder) == NULL);
// }


TChannel* GetByTransponder(const TChannel* Transponder) {
  int maxdelta = 500; // kHz. DVB-C 113MHz vs. 114MHz etc.
  char source = Transponder->Source[0];
  if (source == 'S')
     maxdelta = 2;    // MHz. LNB drift
  else if (source == 'T')
     maxdelta = 250;  // kHz -> France (UK: no longer)

  if (NewChannels.Count()) {
     for(int idx = 0; idx < NewChannels.Count(); ++idx) {
        TChannel* ch = NewChannels[idx];
        if (is_nearly_same_frequency(ch, Transponder, maxdelta) &&
            ch->Source == Transponder->Source &&
            ch->TID == Transponder->TID &&
            ch->SID == Transponder->SID) {
           //dlog(4, "   GetByTransponder: known channel %s", *PrintChannel(Transponder));
           return (ch);
           }
        }
     }
  return (NULL);
}




/*******************************************************************************
 * cSdtScanner
 ******************************************************************************/
cSdtScanner::cSdtScanner(cDevice * Parent, TSdtData& Data) : 
  active(true), device(Parent), data(Data)
{
  data.original_network_id = 0;
  first_crc32 = 0;
  Start();
}

cSdtScanner::~cSdtScanner() {
  active = false;
  wait.Signal();
}

void cSdtScanner::Action(void) {
  int count = 0;
  int nbytes = 0;
  unsigned char buffer[4096];

  int fd = device->OpenFilter(PID_SDT, TABLE_ID_SDT_ACTUAL, 0xFF);
  while(Running() && active) {
     if (wait.Wait(10)) {
        dlog(0, "received signal (%p)", this);
        break;
        }
     if (count++ > 4000) { //40sec
        dlog(2, "SDT timeout\n");
        active = false;
        break;
        }
     nbytes = device->ReadFilter(fd, buffer, sizeof(buffer));
     if (nbytes > 0)
        Process(buffer, nbytes);
     }
  active = false;

  device->CloseFilter(fd);
  fd = -1;
}

void cSdtScanner::Process(const u_char* Data, int Length) {

  SI::SDT sdt(Data, false);
  if (!sdt.CheckCRCAndParse())
     return;

  int len = sdt.getLength();
  uint32_t crc32 = Data[len-4] << 24 | Data[len-3] << 16 | Data[len-2] << 8 | Data[len-1];

  if (first_crc32 == crc32) {
     active = false;
     return;
     }

  if (!first_crc32)
     first_crc32 = crc32;

  if (data.original_network_id == 0)
     data.original_network_id = sdt.getOriginalNetworkId();

  HEXDUMP(Data, Length);

  SI::SDT::Service SiSdtService;
  for(SI::Loop::Iterator it; sdt.serviceLoop.getNext(SiSdtService, it);) {
     struct sdtservice service;
     service.transport_stream_id = sdt.getTransportStreamId();
     service.original_network_id = sdt.getOriginalNetworkId();
     service.service_id = SiSdtService.getServiceId();
     service.free_CA_mode = SiSdtService.getFreeCaMode();
     service.service_type = 0xFFFF;
     service.reported = false;

     SI::Descriptor* d;
     for(SI::Loop::Iterator it2; (d = SiSdtService.serviceDescriptors.getNext(it2));) {
        switch((unsigned) d->getDescriptorTag()) {
           case SI::ServiceDescriptorTag: {
              SI::ServiceDescriptor* sd = (SI::ServiceDescriptor*) d;
              char NameBuf[4096];
              char ShortNameBuf[4096];
              char ProviderNameBuf[4096];
              sd->serviceName.getText(NameBuf, ShortNameBuf, sizeof(NameBuf), sizeof(ShortNameBuf));
              char* pn = compactspace(NameBuf);
              char* ps = compactspace(ShortNameBuf);
              if (!*ps && (strchr(pn, '>') || strchr(pn, ','))) {
                 char* p = strchr(pn, '>'); // fix for UPC Wien: "name>short name"
                 if (!p)
                    p = strchr(pn, ',');    // fix for "Kabel Deutschland": "name, short name"
                 if (p && p > pn) {
                    *p++ = 0;
                    strcpy(ShortNameBuf, skipspace(p));
                    }
                 }
              sd->providerName.getText(ProviderNameBuf, sizeof(ProviderNameBuf));
              char* pp = compactspace(ProviderNameBuf);
              service.Name = pn;
              service.Shortname = ps;
              service.Provider = pp;
              service.service_type = sd->getServiceType();
              }
              break;

           case SI::ComponentDescriptorTag: {
              /*
              SI::ComponentDescriptor* cd = (SI::ComponentDescriptor*) d;
              const unsigned char* p = d->getData().getData();

              if (wSetup.verbosity > 4)
                 hexdump("ComponentDescriptor", p, 2 + *(p + 1));
              uint8_t stream_content = cd->getStreamContent();
              uint8_t stream_content_ext = *(p + 2) >> 4;
              uint8_t component_type = cd->getComponentType();
              uint8_t component_tag = cd->getComponentTag();
              std::string ISO_639_language_code = cd->languageCode;
              std::string text_char;
              char buf[256];

              cd->description.getText(buf, sizeof(buf));
              text_char = buf;

              dlog(0, "SI::ComponentDescriptor: stream_content %d, stream_content_ext %d, component_type %d, component_tag %d, lang %s, text %s",
                     stream_content, stream_content_ext, component_type, component_tag, ISO_639_language_code.c_str(), text_char.c_str()); 
              */
              break;
              }
           case SI::NVODReferenceDescriptorTag:
           case SI::BouquetNameDescriptorTag:
           case SI::MultilingualServiceNameDescriptorTag:
           case SI::ServiceIdentifierDescriptorTag:
           case SI::ServiceAvailabilityDescriptorTag:
           case SI::DefaultAuthorityDescriptorTag:
           case SI::AnnouncementSupportDescriptorTag:
           case SI::DataBroadcastDescriptorTag:
           case SI::TelephoneDescriptorTag:
           case SI::CaIdentifierDescriptorTag:
           case SI::PrivateDataSpecifierDescriptorTag:
           case SI::ContentDescriptorTag:
           case SI::LinkageDescriptorTag:
           case 0x80 ... 0xFE: // user defined //
              break;
           default: dlog(5, "SDT: unknown descriptor 0x%.2x", d->getDescriptorTag());
           }
        DELETENULL(d);
        }
     if (service.Name != "") {
        bool found = false;
        for(int i = 0; i < data.services.Count(); i++) {
           if (data.services[i].transport_stream_id == service.transport_stream_id and
               data.services[i].original_network_id == service.original_network_id and
               data.services[i].service_id          == service.service_id) {
              found = true;
              break;
              }
           }
        if (!found)
           data.services.Add(service);
        }
     }
}



/*
int AddChannels() {
  std::string s;
  int count = 0;


  for(int i = 0; i < NewChannels.Count(); i++) {
     TChannel* channel = NewChannels[i];
     channel->Print(s);
     dlog(0, "%s", s.c_str());
     }

//cStateKey StateKey;
//if (Channels *Channels = cChannels::GetChannelsWrite(StateKey)) {
//   // access the timers
//   StateKey.Remove();



  Channels.IncBeingEdited();

  for(TChannel* Channel = NewChannels.First(); Channel; Channel = NewChannels.Next(Channel)) {
     if (!Channel->Vpid() && ! Channel->Apid(0) && ! Channel->Dpid(0) && 
         !Channel->Tpid() && ! Channel->Ca() &&
         !strncasecmp(Channel->Name(),"???",3)) {
        dlog(4,"      skipped service %s", *PrintChannel(Channel));
        continue;
        }
     if (! Channels.HasUniqueChannelID(Channel)) {
        TChannel* ExistingChannel = Channels.GetByChannelID(Channel->GetChannelID(), false, false);
        if (ExistingChannel) {
           int i;
           char ALangs[MAXAPIDS][MAXLANGCODE2] = { "" };
           char DLangs[MAXDPIDS][MAXLANGCODE2] = { "" };
           char SLangs[MAXSPIDS][MAXLANGCODE2] = { "" };
           int  Atypes[MAXAPIDS + 1]           = {0};
           int  Dtypes[MAXDPIDS + 1]           = {0};

           for(i = 0; i < MAXAPIDS; i++) {
              int len = strlen(Channel->Alang(i));
              if (len < 1) break;
              strncpy (ALangs[i], Channel->Alang(i), min(len,MAXLANGCODE2));
              }
           for(i = 0; i < MAXAPIDS; i++) {
              int len = strlen(Channel->Dlang(i));
              if (len < 1) break;
              strncpy (DLangs[i], Channel->Dlang(i), min(len,MAXLANGCODE2));
              }
           for(i = 0; i < MAXAPIDS; i++) {
              int len = strlen(Channel->Slang(i));
              if (len < 1) break;
              strncpy (SLangs[i], Channel->Slang(i), min(len,MAXLANGCODE2));
              }
           for(i = 0; i < MAXAPIDS; i++)
              Atypes[i] = Channel->Atype(i);

           for(i = 0; i < MAXDPIDS; i++)
              Dtypes[i] = Channel->Dtype(i);

           if (Channel->Vpid() || Channel->Apid(0) || Channel->Dpid(0))
              SetPids(ExistingChannel, Channel->Vpid(), Channel->Ppid(),
                      Channel->Vtype(),
                      (int*) Channel->Apids(), Atypes, ALangs,
                      (int*) Channel->Dpids(), Dtypes, DLangs,
                      (int*) Channel->Spids(), SLangs,
                      Channel->Tpid());
           if (strcmp("???", Channel->Name()))
              ExistingChannel->SetName(Channel->Name(), Channel->ShortName(), Channel->Provider());
              dlog(4,"      updated (existing): %s", *PrintChannel(ExistingChannel));
           }
        else
           dlog(4,"      skipped (existing): %s", *PrintChannel(Channel));
        continue;
        }
     if (Channel->Ca() && ! (wSetup.scanflags & SCAN_SCRAMBLED)) {
        dlog(4,"      skipped (no scrambled channels) %s", *PrintChannel(Channel));
        continue;
        }
     if (! Channel->Ca() && ! (wSetup.scanflags & SCAN_FTA)) {
        dlog(4,"      skipped (no FTA channels) %s", *PrintChannel(Channel));
        continue;
        }
     if (! Channel->Vpid() && (Channel->Apid(0) || Channel->Dpid(0)) && ! (wSetup.scanflags & SCAN_RADIO)) {
        dlog(4,"      skipped (no radio channels) %s", *PrintChannel(Channel));
        continue;
        }
     if (Channel->Vpid() && ! (wSetup.scanflags & SCAN_TV)) {
        dlog(4,"      skipped (no tv channels) %s", *PrintChannel(Channel));
        continue;
        }
     if ((Channel->Vtype() > 2) && ! (wSetup.scanflags & SCAN_HD)) {
        dlog(4,"      skipped (no hdtv channels) %s", *PrintChannel(Channel));
        continue;
        }
     TChannel* aChannel = new TChannel(* Channel);
     Channels.Add(aChannel);
     count++;
     }
  NewChannels.Load(NULL, false, false);
  Channels.DecBeingEdited();
  Channels.ReNumber();
  Channels.SetModified(true);

  return (count);
}*/
