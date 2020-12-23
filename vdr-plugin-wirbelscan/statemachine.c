/*
 * statemachine.c: wirbelscan - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 */

#include <stdarg.h>
#include <stdlib.h>
#include <string>
#include <vdr/tools.h>
#include "statemachine.h"
#include "scanfilter.h"
#include "common.h"
#include "dvb_wrapper.h"
#include "menusetup.h"
#include "si_ext.h"
using namespace SI_EXT;


extern TChannels NewChannels;
extern TChannels NewTransponders;
extern TChannels ScannedTransponders;



//  v 0.0.5, a dummy receiver. Might be used real later.
class cScanReceiver : public cReceiver, cThread {
private:
protected:
  virtual void Activate(bool On);
  virtual void Receive(const uchar* Data, int Length);
  virtual void Action(void);
public:
  cScanReceiver();
  virtual ~cScanReceiver() {cReceiver::Detach(); };
};

cScanReceiver::cScanReceiver() : cReceiver(NULL, 99), cThread("dummy receiver") { }

void cScanReceiver::Receive(const uchar* Data, int Length) {}

void cScanReceiver::Action(void) {
  while (Running()) cCondWait::SleepMs(5);
  /*TODO: check here periodically for lock and wether we got any data!*/
};

void cScanReceiver::Activate(bool On) {
  if (On) Start();
  else Cancel(0);
};


//  v 0.0.5, store state in lastState if different and print state
void cStateMachine::Report(eState State) {
  const char * stateMsg[] = { // be careful here: same order as eState
     "------- Start -------",
     "------- Stop -------",
     "------- Tune -------",
     "------- NextTransponder -------",
     "------- DetachReceiver -------",
     "------- ScanPat -------",
     "------- ScanPmt -------",
     "------- ScanNit -------",
     "------- ScanSdt -------",
     "------- ScanEit -------",
     "------- ERROR IN STATEMACHINE, UNKNOWN STATE. -------",
     "------- AddChannels -------",
     "------- GetTables -------",
     "------- NULL -------"
     };

  if ((State != lastState) and (wSetup.verbosity > 4))
     dlog(5, "%s", stateMsg[lastState = State]);
};

//  v 0.0.5, StateMachine constructor
cStateMachine::cStateMachine(cDevice* Dev, TChannel* InitialTransponder, bool UseNit, void* Parent) {
  dev       = Dev;
  stop      = false;
  initial   = InitialTransponder;
  useNit    = UseNit;
  state     = eStart;
  parent    = Parent;
  
  Start();
}

// v 0.0.5, StateMachine destructor
cStateMachine::~cStateMachine(void) {
  stop = true;
}

TSdtData SdtData;
TNitData NitData;

// v 0.0.5, StateMachine itself
void cStateMachine::Action(void) {
  TChannel* Transponder = NULL;
  cScanReceiver* aReceiver = NULL;
  cPatScanner* PatScanner = NULL;
  cNitScanner* NitScanner = NULL;
  cSdtScanner* SdtScanner = NULL;
  eState newState = state;
  cScanner* scanner = (cScanner*)parent;
  int dvbtype = scanner->DvbType();
  dvbdevice = scanner->DvbDevice();
  std::string s;

  TList<cPmtScanner*> PmtScanners;
  struct TPatData PatData;
  TList<TPmtData*> PmtData;

  bool pmtstart = false;
  bool tblstart = false;

  while (Running() && !stop) {
     cWait.Wait(10);
     Report(state);
     switch (state) {
        case eStart:
           Transponder = initial;
           if (known_transponder(Transponder, false, &ScannedTransponders))
              newState = eStop;
           else
              newState = eTune;
           break;

        case eStop:
           stop = true;
           goto DIRECT_EXIT;
           break;

        case eTune: {
           Transponder->PrintTransponder(s);
           dlog(4, "tuning to %s", s.c_str());
           lTransponder = s;

           if (MenuScanning)
              MenuScanning->SetTransponder(Transponder);

           //scanner->SetCounter(ScannedTransponders.Count(), NewTransponders.Count());
           if (MenuScanning)
              MenuScanning->SetStr(0, false);

           cChannel c;

           // skip ChannelID check in cChannel::Parse()
           // we just want to tune here, nothing else.
           int nid = Transponder->NID;
           int sid = Transponder->SID;
           Transponder->NID = 0x2000;
           Transponder->SID = 0x2000;

           Transponder->VdrChannel(c);
           dev->SwitchChannel(&c, false);

           Transponder->NID = nid;
           Transponder->SID = sid;

           aReceiver = new cScanReceiver();
           dev->AttachReceiver(aReceiver);

           cCondWait::SleepMs(1000);
           if (dev->HasLock(3000)) {
              dev->SetOccupied(90);
              newState = eScanPat;
              dlog(4, "lock.");
              }
           else {
              DELETENULL(aReceiver);
              // add to scanned tp (avoid retry on this tp)
              TChannel* tp = new TChannel;
              tp->CopyTransponderData(Transponder);
              tp->PrintTransponder(s);
              tp->Tested = true;
              tp->Tunable = false;
              //dlog(5, "Add TP: '%s', NID = %d, TID = %d", s.c_str(), tp->NID, tp->TID);
              ScannedTransponders.Add(tp);
              newState = eNextTransponder;
              }
           if (dvbdevice)
              lStrength = dev->SignalStrength();
           else
              lStrength = 100; // non-dvb hardware - no info, assume perfect.
           if (lStrength < 0 or lStrength > 100)
              lStrength = 0;
           if (MenuScanning)
              MenuScanning->SetStr(lStrength, dev->HasLock(1));
           break;
           }
        case eNextTransponder: {
           nextTransponders = NewTransponders.Count();
           //scanner->SetCounter(ScannedTransponders.Count(), NewTransponders.Count());
           if (! useNit)
               goto DIRECT_EXIT;

           /*
           if (Transponder != initial)
              DELETENULL(Transponder);

           if (NewTransponders.Count()) {
              Transponder = NewTransponders[0];
              NewTransponders.Delete(0);
              newState = eTune;
              }
           else
              newState = eStop;*/

           newState = eStop;
           if (NewTransponders.Count()) {
              Transponder = NULL;
              for(int i = 0; i < NewTransponders.Count(); i++) {
                 if (NewTransponders[i]->Tested)
                    continue;
                 NewTransponders[i]->Tested = true;
                 Transponder = NewTransponders[i];
                 newState = eTune;
                 break;
                 }
              }

           lProgress = 0.5 + (100.0 * (scanner->ThisChannel() + ScannedTransponders.Count()) / (NewTransponders.Count() + scanner->InitialTransponders()));
           if (MenuScanning) {
              MenuScanning->SetCounters(scanner->ThisChannel() + ScannedTransponders.Count(), NewTransponders.Count() + scanner->InitialTransponders());
              MenuScanning->SetProgress(lProgress);
              }

           break;
           }
        case eDetachReceiver:
           if (dev) {
              dev->DetachAllReceivers();
              dev->SetOccupied(0);
              }
           DELETENULL(aReceiver);

           if (stop)
              newState = eStop;
           else
              newState = eNextTransponder;
           break;

        case eScanPat:
           if (PatScanner == NULL)
              PatScanner = new cPatScanner(dev, PatData);
           else if (!PatScanner->Active()) {
              pmtstart = true;
              dlog(4, "searching %d services", PatData.services.Count());
              if (stop or !PatScanner->HasPAT())
                 newState = eDetachReceiver;
              else
                 newState = eScanPmt;
              break;
              }
           break;

        case eScanPmt:
           if (pmtstart) {
              PatScanner = NULL;
              pmtstart = false;
              PmtScanners.Clear();
              PmtData.Clear();
              for(int i = 0; i < PatData.services.Count(); i++) {
                 TPmtData* d = new TPmtData;
                 d->program_map_PID = PatData.services[i].program_map_PID;
                 PmtData.Add(d);
                 cPmtScanner* p = new cPmtScanner(dev, PmtData[i]);
                 PmtScanners.Add(p);
                 }
              }
           else {
              // run up to 16 filters in parallel; up to 32 should be safe.
              int activePmts = 0;
              int finished = 0;
              for(int i = 0; i < PmtScanners.Count() and !stop; i++) {
                 cPmtScanner* p = (cPmtScanner*) PmtScanners[i];
                 if (p->Finished()) {
                    finished++;
                    continue;
                    }
                 if (p->Active()) {
                    if (++activePmts > 16)
                       break;
                    }
                 else {
                    p->Start();
                    if (++activePmts > 16)
                       break;
                    }
                 }

              if (finished < PmtScanners.Count())
                 break;

              PmtScanners.Clear();
              tblstart = true;
              if (stop)
                 newState = eDetachReceiver;
              else
                 newState = eGetTables;
              }
           break;

        case eGetTables: {
           static int tm;
           if (tblstart) {
              tblstart = false;
              tm = time(0);
              // some stupid cable providers use non-standard PID for NIT; sometimes called 'Setup-PID'.
              if (wSetup.DVBC_Network_PID != 0x10)
                 PatData.network_PID = wSetup.DVBC_Network_PID;
              SdtData.original_network_id = 0;
              NitScanner = new cNitScanner(dev, PatData.network_PID, NitData, dvbtype);
              SdtScanner = new cSdtScanner(dev, SdtData);
              }
           else {
              if (!NitScanner->Active() and !SdtScanner->Active())
                 if (stop)
                    newState = eDetachReceiver;
                 else
                    newState = eAddChannels;
              if (time(0) != tm) {
                 if (MenuScanning)
                    MenuScanning->SetProgress(lProgress);
                 tm = time(0);
                 }
              }
           }
           break;

        case eAddChannels: {
           if (wSetup.verbosity > 4) {
              for(int i = 0; i < PmtData.Count(); i++)
                 dlog(0, "PMT %d: program_number = %d; Vpid = %d (%d); Tpid = %d Apid = %d Dpid = %d",
                         PmtData[i]->program_map_PID,
                         PmtData[i]->program_number,
                         PmtData[i]->Vpid.PID,PmtData[i]->PCR_PID,
                         PmtData[i]->Tpid,
                         PmtData[i]->Apids.Count()?PmtData[i]->Apids[0].PID:0,
                         PmtData[i]->Dpids.Count()?PmtData[i]->Dpids[0].PID:0);

              for(int i = 0; i < SdtData.services.Count(); i++) {
                 if (SdtData.services[i].reported)
                    continue;
                 SdtData.services[i].reported = true;
                 dlog(0, "SDT: ONID = %d, TID = %d, SID = %d, FreeCA = %d, Name = '%s'",
                      SdtData.services[i].original_network_id,
                      SdtData.services[i].transport_stream_id,
                      SdtData.services[i].service_id,
                      SdtData.services[i].free_CA_mode,
                      SdtData.services[i].Name.c_str());
                 }

              for(int i = 0; i < NitData.transport_streams.Count(); i++) {
                 if (NitData.transport_streams[i]->reported)
                    continue;
                 NitData.transport_streams[i]->reported = true;
                 NitData.transport_streams[i]->PrintTransponder(s);
                 dlog(0, "NIT: %s'%s', NID = %d, ONID = %d, TID = %d",
                    abs(NitData.transport_streams[i]->OrbitalPos - initial->OrbitalPos) > 5?"WRONG SATELLITE: ":"",
                    s.c_str(), NitData.transport_streams[i]->NID, NitData.transport_streams[i]->ONID, NitData.transport_streams[i]->TID);
                 if (NitData.transport_streams[i]->Source == "T" and NitData.transport_streams[i]->DelSys == 1) {
                    for(int c = 0; c < NitData.transport_streams[i]->cells.Count(); c++) {
                       for(int cf = 0; cf < NitData.transport_streams[i]->cells[c].num_center_frequencies; cf++)
                          dlog(0, "   center%d = %u (cell_id %u)", c+1, NitData.transport_streams[i]->cells[c].center_frequencies[cf], NitData.transport_streams[i]->cells[c].cell_id);
                       for(int tf = 0; tf < NitData.transport_streams[i]->cells[c].num_transposers; tf++)
                          dlog(0, "      transposer%d = %u (cell_id_extension %u)", tf+1, NitData.transport_streams[i]->cells[c].transposers[tf].transposer_frequency, NitData.transport_streams[i]->cells[c].transposers[tf].cell_id_extension);
                       }
                    }
                 }
              }

           // PAT: transport_stream_id, network_pid (0x10), program_map_PIDs
           // PMT: program_number(service_id), PCR_PID, [stream_type, elementary_PID]
           // NIT: network_id, [transport_stream_id, original_network_id]
           // SDT: transport_stream_id, original_network_id, [service_id, free_CA_mode]
           
           Transponder->TID = PatData.services[0].transport_stream_id;
           if (SdtData.original_network_id) // update onid, if sdt found. 
              Transponder->ONID = SdtData.original_network_id;

           for(int i = 0; i < NitData.transport_streams.Count(); i++) {
              if ((NitData.transport_streams[i]->NID == Transponder->NID or
                  NitData.transport_streams[i]->ONID == Transponder->ONID) and
                  NitData.transport_streams[i]->TID == Transponder->TID) {
                 uint32_t f = Transponder->Frequency;
                 uint32_t center_freq = NitData.transport_streams[i]->Frequency;

                 Transponder->CopyTransponderData(NitData.transport_streams[i]);

                 if ((center_freq < 100000000) or (center_freq > 858000000) or (abs((int)center_freq - (int)f) > 2000000))
                    Transponder->Frequency = f;
                 break;
                 }
              }

           if (!known_transponder(Transponder, false, &ScannedTransponders)) {
              TChannel* tp = new TChannel;
              tp->CopyTransponderData(Transponder);
              tp->NID = Transponder->NID;
              tp->ONID = Transponder->ONID;
              tp->TID = Transponder->TID;
              tp->Tested = true;
              tp->Tunable = true;
              tp->PrintTransponder(s);
              dlog(5, "ScannedTransponders.Add: '%s', NID = %d, ONID = %d, TID = %d",
                   s.c_str(), tp->NID, tp->ONID, tp->TID);
              ScannedTransponders.Add(tp);
              }

           for(int i = 0; i < PmtData.Count(); i++) {
              TChannel* n = new TChannel;
              n->CopyTransponderData(Transponder);
              n->NID = Transponder->NID;
              n->ONID = Transponder->ONID;
              n->TID = Transponder->TID;
              n->SID = PmtData[i]->program_number;
              n->VPID.PID  = PmtData[i]->Vpid.PID;
              n->VPID.Type = PmtData[i]->Vpid.Type;
              n->VPID.Lang = PmtData[i]->Vpid.Lang;
              n->PCR   = PmtData[i]->PCR_PID;
              n->TPID  = PmtData[i]->Tpid;
              n->APIDs = PmtData[i]->Apids;
              n->DPIDs = PmtData[i]->Dpids;
              n->SPIDs = PmtData[i]->Spids;
              n->CAIDs = PmtData[i]->Caids;

              if (!n->VPID.PID and !n->APIDs.Count() and !n->DPIDs.Count()) {
                 delete n;
                 continue;
                 }

              for(int j = 0; j < SdtData.services.Count(); j++) {
                 if (n->TID == SdtData.services[j].transport_stream_id and
                     n->SID == SdtData.services[j].service_id) {
                    n->Name         = SdtData.services[j].Name;
                    n->Shortname    = SdtData.services[j].Shortname;
                    n->Provider     = SdtData.services[j].Provider;
                    n->free_CA_mode = SdtData.services[j].free_CA_mode;
                    n->service_type = SdtData.services[j].service_type;
                    n->ONID         = SdtData.services[j].original_network_id;
                    break;
                    }
                 }

              if (n->service_type == Teletext_service or
                  n->service_type == DVB_SRM_service or
                  n->service_type == mosaic_service or
                  n->service_type == data_broadcast_service or
                  n->service_type == reserved_Common_Interface_Usage_EN50221 or
                  n->service_type == RCS_Map_EN301790 or
                  n->service_type == RCS_FLS_EN301790 or
                  n->service_type == DVB_MHP_service or
                  n->service_type == H264_AVC_codec_mosaic_service) {
                 dlog(5, "skip service %d '%s' (no Audio/Video)", n->SID, n->Name.c_str());
                 continue;
                 }

              #define PMT_ALL (SCAN_TV | SCAN_RADIO | SCAN_SCRAMBLED | SCAN_FTA)

              if ((wSetup.scanflags & PMT_ALL) != PMT_ALL and n->service_type < 0xFFFF) {
                 if ((wSetup.scanflags & SCAN_SCRAMBLED) != SCAN_SCRAMBLED and n->free_CA_mode) {
                    dlog(5, "skip service %d '%s' (encrypted)", n->SID, n->Name.c_str());
                    continue;
                    }
                 if ((wSetup.scanflags & SCAN_FTA) != SCAN_FTA and !n->free_CA_mode) {
                    dlog(5, "skip service %d '%s' (FTA)", n->SID, n->Name.c_str());
                    continue;
                    }

                 if ((wSetup.scanflags & SCAN_TV) != SCAN_TV) {
                    if (n->service_type == digital_television_service or
                        n->service_type == digital_television_NVOD_reference_service or
                        n->service_type == digital_television_NVOD_timeshifted_service or
                        n->service_type == MPEG2_HD_digital_television_service or
                        n->service_type == H264_AVC_SD_digital_television_service or
                        n->service_type == H264_AVC_SD_NVOD_timeshifted_service or
                        n->service_type == H264_AVC_SD_NVOD_reference_service or
                        n->service_type == H264_AVC_HD_digital_television_service or
                        n->service_type == H264_AVC_HD_NVOD_timeshifted_service or
                        n->service_type == H264_AVC_HD_NVOD_reference_service or
                        n->service_type == H264_AVC_frame_compat_plano_stereoscopic_HD_digital_television_service or
                        n->service_type == H264_AVC_frame_compat_plano_stereoscopic_HD_NVOD_timeshifted_service or
                        n->service_type == H264_AVC_frame_compat_plano_stereoscopic_HD_NVOD_reference_service or
                        n->service_type == HEVC_digital_television_service) {
                       dlog(5, "skip service %d '%s' (tv)", n->SID, n->Name.c_str());
                       continue;
                       }
                    }
                 if ((wSetup.scanflags & SCAN_RADIO) != SCAN_RADIO) {
                    if (n->service_type == digital_radio_sound_service or
                        n->service_type == FM_radio_service or
                        n->service_type == advanced_codec_digital_radio_sound_service) {
                       dlog(5, "skip service %d '%s' (radio)", n->SID, n->Name.c_str());
                       continue;
                       }
                    }
                 }
              if (wSetup.verbosity > 4) {
                 n->Print(s);
                 dlog(4, "NewChannels.Add: '%s'", s.c_str());
                 }
              else {
                 if (n->Name != "???") dlog(0, "%s", n->Name.c_str());
                 }
              NewChannels.Add(n);
              if (MenuScanning)
                 MenuScanning->SetChan(NewChannels.Count()); 
              }

           for(int i = 0; i < NewChannels.Count(); i++) {
              if (NewChannels[i]->Name != "???")
                 continue;
              for(int j = 0; j < SdtData.services.Count(); j++) {
                 if (NewChannels[i]->TID == SdtData.services[j].transport_stream_id and
                   /*NewChannels[i]->NID == SdtData.services[j].original_network_id and*/
                     NewChannels[i]->SID == SdtData.services[j].service_id) {
                    NewChannels[i]->Name         = SdtData.services[j].Name;
                    NewChannels[i]->Shortname    = SdtData.services[j].Shortname;
                    NewChannels[i]->Provider     = SdtData.services[j].Provider;
                    NewChannels[i]->free_CA_mode = SdtData.services[j].free_CA_mode;
                    NewChannels[i]->Print(s);
                    dlog(5, "Update: '%s'", s.c_str());
                    break;
                    }
                 }
              }

           for(int i = 0; i < NitData.transport_streams.Count(); i++) {
              if (abs(NitData.transport_streams[i]->OrbitalPos - initial->OrbitalPos) > 5)
                 continue;
              if (!known_transponder(NitData.transport_streams[i], true)) {
                 TChannel* tp = new TChannel;
                 tp->CopyTransponderData(NitData.transport_streams[i]);
                 tp->NID = NitData.transport_streams[i]->NID;
                 tp->ONID = NitData.transport_streams[i]->ONID;
                 tp->TID = NitData.transport_streams[i]->TID;
                 tp->PrintTransponder(s);
                 dlog(4, "NewTransponders.Add: '%s', NID = %d, ONID = %d, TID = %d",
                      s.c_str(), tp->NID, tp->ONID, tp->TID);
                 NewTransponders.Add(tp);
                 }

              if (NitData.transport_streams[i]->Source == "T" and NitData.transport_streams[i]->DelSys == 1) {
                 for(int c = 0; c < NitData.transport_streams[i]->cells.Count(); c++) {
                    for(int cf = 0; cf < NitData.transport_streams[i]->cells[c].num_center_frequencies; cf++) {
                       TChannel* tp = new TChannel;
                       tp->CopyTransponderData(NitData.transport_streams[i]);
                       tp->NID = NitData.transport_streams[i]->NID;
                       tp->TID = NitData.transport_streams[i]->TID;
                       tp->Frequency = NitData.transport_streams[i]->cells[c].center_frequencies[cf];
                       if (!known_transponder(tp, true)) {
                          tp->PrintTransponder(s);
                          dlog(4, "NewTransponders.Add: '%s', NID = %d, ONID = %d, TID = %d", s.c_str(), tp->NID, tp->ONID, tp->TID);
                          NewTransponders.Add(tp);
                          }
                       else
                          delete tp;
                       }
                    for(int tf = 0; tf < NitData.transport_streams[i]->cells[c].num_transposers; tf++) {
                       TChannel* tp = new TChannel;
                       tp->CopyTransponderData(NitData.transport_streams[i]);
                       tp->NID = NitData.transport_streams[i]->NID;
                       tp->TID = NitData.transport_streams[i]->TID;
                       tp->Frequency = NitData.transport_streams[i]->cells[c].transposers[tf].transposer_frequency;
                       if (!known_transponder(tp, true)) {
                          tp->PrintTransponder(s);
                          dlog(5, "NewTransponders.Add: '%s', NID = %d, ONID = %d, TID = %d", s.c_str(), tp->NID, tp->ONID, tp->TID);
                          NewTransponders.Add(tp);
                          }
                       else
                          delete tp;
                       }
                    }
                 }
              }

           for(int i = 0; i < NitData.cell_frequency_links.Count(); i++) {
              TChannel t;

              if (wSetup.verbosity > 5)
                 dlog(0, "NIT: cell_id %u, frequency %7.3fMHz network_id %d",
                       NitData.cell_frequency_links[i].cell_id,
                       NitData.cell_frequency_links[i].frequency/1e6,
                       NitData.cell_frequency_links[i].network_id);
              t.Source       = 'T';
              t.Frequency    = NitData.cell_frequency_links[i].frequency;
              t.Bandwidth    = t.Frequency <= 226500000 ? 7 : 8;
              t.Inversion    = 999;
              t.FEC          = 999;
              t.FEC_low      = 999;
              t.Modulation   = 999;
              t.Transmission = 999;
              t.Guard        = 999;
              t.Hierarchy    = 999;
              t.DelSys       = 0;

              if (!known_transponder(&t, true)) {
                 TChannel* n = new TChannel;
                 n->CopyTransponderData(&t);
                 n->PrintTransponder(s);
                 dlog(4, "NewTransponders.Add: '%s', NID = %d, TID = %d", s.c_str(), n->NID, n->TID);
                 NewTransponders.Add(n);
                 }

              t.DelSys = 1;
              if (!known_transponder(&t, true)) {
                 TChannel* n = new TChannel;
                 n->CopyTransponderData(&t);
                 n->PrintTransponder(s);
                 dlog(4, "NewTransponders.Add: '%s', NID = %d, TID = %d", s.c_str(), n->NID, n->TID);
                 NewTransponders.Add(n);
                 }

              for(int j = 0; j < NitData.cell_frequency_links[i].subcellcount; j++) {
                 dlog(5, "NIT:    cell_id_extension %u, frequency %7.3fMHz",
                      NitData.cell_frequency_links[i].subcells[j].cell_id_extension,
                      NitData.cell_frequency_links[i].subcells[j].transposer_frequency/1e6);
                 t.Frequency = NitData.cell_frequency_links[i].subcells[j].transposer_frequency;
                 t.Bandwidth = t.Frequency <= 226500000 ? 7 : 8;
                 t.DelSys    = 0;
                 
                 if (!known_transponder(&t, true)) {
                    TChannel* tp = new TChannel;
                    tp->CopyTransponderData(&t);
                    tp->PrintTransponder(s);
                    dlog(5, "NewTransponders.Add: '%s', NID = %d, TID = %d", s.c_str(), tp->NID, tp->TID);
                    NewTransponders.Add(tp);
                    }
                 
                 t.DelSys = 1;
                 if (!known_transponder(&t, true)) {
                    TChannel* tp = new TChannel;
                    tp->CopyTransponderData(&t);
                    tp->PrintTransponder(s);
                    dlog(4, "NewTransponders.Add: '%s', NID = %d, TID = %d", s.c_str(), tp->NID, tp->TID);
                    NewTransponders.Add(tp);
                    }
                 }
              }


//           if ((count = AddChannels()))
//              dlog(4, "added %d channels", count);
//
//           if (MenuScanning)
//              MenuScanning->SetChan(count); 


           // delete data from current tp
           PatData.network_PID = 0x10;
           PatData.services.Clear();
           for(int i = 0; i < PmtData.Count(); i++)
              delete PmtData[i];
           PmtData.Clear();
           NitData.frequency_list.Clear();
           NitData.cell_frequency_links.Clear();

           newState = eDetachReceiver;
           }
           break;

        case eUnknown:
           newState = eStop;
           break;
        default: newState = eUnknown; //every unhandled state should come here.
        }
     state = newState;
     }
  dlog(0, "%s\n", "DIRECT_EXIT");
  DIRECT_EXIT:
  Cancel();
}
