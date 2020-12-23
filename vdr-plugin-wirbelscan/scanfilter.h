#ifndef __WIRBELSCAN_SCANFILTER_H_
#define __WIRBELSCAN_SCANFILTER_H_

#include <libsi/si.h>
#include <vdr/receiver.h>
#include <vdr/filter.h>
#include <vdr/pat.h>
#include <libsi/descriptor.h>
#include "si_ext.h"
#include "common.h"


#define HEXDUMP(d, l) \
  if (wSetup.verbosity > 5) hexdump(__PRETTY_FUNCTION__, d, l);


//---------cTransponders---------------------------------------------------------------------
class cTransponders : public TChannels {
private:
protected:
public:
   bool IsUniqueTransponder(const TChannel * NewTransponder);
   TChannel* GetByParams(const TChannel * NewTransponder);
   TChannel* NextTransponder(void);
};

//extern cTransponders NewTransponders;
//extern cTransponders ScannedTransponders;
//extern TChannels NewChannels;
extern int nextTransponders;

//--------------------------------------------------------------------------------------------

bool known_transponder(TChannel* newChannel, bool auto_allowed, TChannels* = NULL);
bool is_nearly_same_frequency(const TChannel* chan_a, const TChannel* chan_b, uint delta = 2001);
bool is_different_transponder_deep_scan(const TChannel* a, const TChannel* b, bool auto_allowed);
TChannel* GetByTransponder(const TChannel* Transponder);
void resetLists();

/*******************************************************************************
 * TThread
 ******************************************************************************/
class TThread {
private:
  bool running;
  pthread_t thread;
  static void* start_routine(TThread* Thread);
protected:
  virtual void Action(void) = 0;
public:
  TThread();
  virtual ~ TThread();
  bool Start(void);
  bool Running(void) { return running; }
  void Cancel(int WaitSeconds = 0);
};



struct service {
  uint16_t transport_stream_id;
  uint16_t program_map_PID;
  uint16_t program_number;
};

struct TPatData {
  uint16_t network_PID;
  TList<struct service> services;
};

struct TPmtData {
  uint16_t program_map_PID;
  uint16_t program_number;
  uint16_t PCR_PID;

  TPid Vpid;
  int Tpid;
  TList<TPid> Apids;
  TList<TPid> Dpids;
  TList<TPid> Spids;
  TList<int> Caids;
};

struct TCell {
  uint16_t network_id;
  uint16_t cell_id;
  uint32_t frequency;
  uint8_t subcellcount;
  struct {
     uint8_t cell_id_extension;
     uint32_t transposer_frequency;
     } subcells[51];
};

struct TServiceListItem {
  uint16_t network_id;
  uint16_t original_network_id;
  uint16_t transport_stream_id;
  uint16_t service_id;
  uint16_t service_type;
};

struct TFrequencyListItem {
  uint16_t network_id;
  uint32_t frequency;
};

struct TNitData {
  TList<TFrequencyListItem> frequency_list;
  TList<TCell> cell_frequency_links;
  TList<TServiceListItem> service_types;
  TList<TChannel*> transport_streams;
};

struct sdtservice {
  uint16_t transport_stream_id;
  uint16_t original_network_id;
  uint16_t service_id;
  uint16_t service_type;
  bool free_CA_mode;
  std::string Name;
  std::string Shortname;
  std::string Provider;
  bool reported;
};

struct TSdtData {
  uint16_t original_network_id;
  TList<sdtservice> services;
};

/*******************************************************************************
 * cPatScanner
 ******************************************************************************/
class cPatScanner : public TThread {
private:
  cDevice* device;
  struct TPatData& PatData;
  bool isActive;
  cSectionSyncer Sync;
  std::string s;
  cCondWait wait;
  TChannel channel;
  bool hasPAT;
protected:
  virtual void Process(const u_char* Data, int Length);
  virtual void Action(void);
public:
  cPatScanner(cDevice* Parent, struct TPatData& Dest);
  ~cPatScanner();
  bool HasPAT() { return hasPAT; };
  bool Active() { return isActive; };
};

/*******************************************************************************
 * cPmtScanner
 ******************************************************************************/
class cPmtScanner : public TThread {
private:
  cDevice* device;
  TPmtData* data;
  bool isActive;
  bool jobDone;
  std::string s;
  cCondWait wait;
protected:
  virtual void Process(const u_char* Data, int Length);
  virtual void Action(void);
public:
  cPmtScanner(cDevice* Parent, TPmtData* Data);
  ~cPmtScanner();
  bool Active() { return isActive; };
  bool Finished() { return jobDone; };
};

/*******************************************************************************
 * cNitScanner
 ******************************************************************************/
class cNitScanner : public TThread {
private:
  bool active;
  cDevice* device;
  uint16_t nit;
  std::string s;
  cCondWait wait;
  TNitData& data;
  uint32_t first_crc32;
  int type;

  void ParseCellFrequencyLinks(uint16_t network_id, const u_char* Data, TList<TCell>& list);
protected:
  virtual void Process(const u_char* Data, int Length);
  virtual void Action(void);

public:
  cNitScanner(cDevice* Parent, uint16_t network_PID, TNitData& Data, int Type);
  ~cNitScanner();
  bool Active() { return (active); };

};

/*******************************************************************************
 * cSdtScanner
 ******************************************************************************/
class cSdtScanner : public TThread {
private:
  bool active;
  cDevice* device;
  TSdtData& data;
  std::string s;
  cCondWait wait;
  uint32_t first_crc32;
protected:
  virtual void Process(const u_char* Data, int Length);
  virtual void Action(void);
public:
  cSdtScanner(cDevice* Parent, TSdtData& Data);
  ~cSdtScanner();
  bool Active(void) { return active; };
};

#endif
