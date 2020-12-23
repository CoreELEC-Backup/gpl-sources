/*
 * equivhandler.cpp
 *
 *  Created on: 19.5.2012
 *      Author: d.petrovski
 */

#include "equivhandler.h"
#include "setupeepg.h"
#include "log.h"
#include "util.h"

#include <string>

using namespace util;

multimap<string, string> cEquivHandler::equiChanMap;
long cEquivHandler::equiChanFileTime = 0;

cEquivHandler::cEquivHandler()
{
  loadEquivalentChannelMap();
}

cEquivHandler::~cEquivHandler()
{
}

void cEquivHandler::loadEquivalentChannelMap (void)
{
  char Buffer[1024];
  char *Line;
  FILE *File;
  string FileName = string(cSetupEEPG::getInstance()->getConfDir()) + "/" + EEPG_FILE_EQUIV;
  multimap<string,string>::iterator it,it2;
  pair<multimap<string,string>::iterator,multimap<string,string>::iterator> ret;

  //Test if file is changed and reload
  struct stat st;
  if (stat(FileName.c_str(), &st)) {
      LogE(0, prep("Error obtaining stats for '%s' "), FileName.c_str());
      return;
  }

  if (equiChanMap.size() > 0 &&  equiChanFileTime == st.st_mtim.tv_nsec)
    return;
  else
    equiChanMap.clear();


  File = fopen (FileName.c_str(), "r");
  if (File) {
    memset (Buffer, 0, sizeof (Buffer));
    char origChanID[256];
    char equiChanID[256];
    char source[256];
    int nid, tid, sid, rid;
    while ((Line = fgets (Buffer, sizeof (Buffer), File)) != NULL) {
      Line = compactspace (skipspace (stripspace (Line)));
      //skip empty and commented lines
      if (isempty (Line) || Line[0] == '#' || Line[0] == ';') continue;
      //skip invalid line
      if (sscanf (Line, "%[^ ] %[^ ] %[^\n]\n", origChanID, equiChanID, source) != 3) continue;

      nid = 0;
      tid = 0;
      sid = 0;
      rid = 0;
      //skip invalid id formats
      if ((sscanf (origChanID, "%[^-]-%i -%i -%i ", source, &nid, &tid, &sid) != 4)
        && (sscanf (equiChanID, "%[^-]-%i -%i -%i ", source, &nid, &tid, &sid) != 4))
        continue;

      if (sscanf (origChanID, "%[^-]-%i -%i -%i -%i ", source, &nid, &tid, &sid, &rid) != 5) {
        rid = 0;
      }
      tChannelID OriginalChID = tChannelID (cSource::FromString (source), nid, tid, sid, rid);
      bool found = false;
      //int i = 0;
#if APIVERSNUM >= 20300
      LOCK_CHANNELS_READ;
      const cChannel *OriginalChannel = Channels->GetByChannelID(OriginalChID, false);
#else
      cChannel *OriginalChannel = Channels.GetByChannelID (OriginalChID, false);
#endif
      if (!OriginalChannel) {
        LogI(2, prep("Warning, not found EPG channel \'%s\' in channels.conf. Equivalence is assumed to be valid, but perhaps you should check the entry in the equivalents file"), origChanID);
        continue;
      }
      if (sscanf (equiChanID, "%[^-]-%i -%i -%i ", source, &nid, &tid, &sid) == 4) {

        if (sscanf (equiChanID, "%[^-]-%i -%i -%i -%i ", source, &nid, &tid, &sid, &rid)
          != 5) {
          rid = 0;
        }
        tChannelID EquivChID = tChannelID (cSource::FromString (source), nid, tid, sid, rid);
#if APIVERSNUM >= 20300
        const cChannel *EquivChannel = Channels->GetByChannelID(EquivChID, false);
#else
        cChannel *EquivChannel = Channels.GetByChannelID (EquivChID, false);
#endif
        if (!EquivChannel) {
          LogI(0, prep("Warning, not found equivalent channel \'%s\' in channels.conf"), equiChanID);
          continue;
        }

        //check if channel is already added
        ret = equiChanMap.equal_range(*OriginalChID.ToString());
        for (it=ret.first; it!=ret.second; ++it)
          if ((*it).second ==  *OriginalChID.ToString()) {
            found = true;
            break;
          }
        if (found)  continue;

        string origCh(*OriginalChID.ToString());
        string equiCh(*EquivChID.ToString());
        equiChanMap.insert(pair<string,string>(origCh.c_str(),equiCh.c_str()));
        LogD(4, prep("Found %s equivalent to %s. origCh %s"), *EquivChID.ToString(), *OriginalChID.ToString(), origCh.c_str());
        for ( it2=equiChanMap.begin() ; it2 != equiChanMap.end(); it2++ )
          LogD(3, prep("Original ID %s <-> Equivalent ID %s"), (*it2).first.c_str(), it2->second.c_str());
      }
    }      //while
    fclose (File);
    equiChanFileTime = st.st_mtim.tv_nsec;
    LogD(2, prep("Loaded %i equivalents."), equiChanMap.size());
    for ( it2=equiChanMap.begin() ; it2 != equiChanMap.end(); it2++ )
      LogD(2, prep("Original ID %s <-> Equivalent ID %s"), (*it2).first.c_str(), it2->second.c_str());
  }  //if file
}

void cEquivHandler::updateEquivalent(cSchedules * Schedules, tChannelID channelID, cEvent *pEvent){
  multimap<string,string>::iterator it;
  pair<multimap<string,string>::iterator,multimap<string,string>::iterator> ret;

  LogD(2, prep("Start updateEquivalent %s"), *channelID.ToString());

  ret = equiChanMap.equal_range(*channelID.ToString());
  for (it=ret.first; it!=ret.second; ++it) {
    LogD(2, prep("equivalent channel exists"));
    tChannelID equChannelID (tChannelID::FromString((*it).second.c_str()));
    const cChannel *equChannel = GetChannelByID (equChannelID, false);
    if (equChannel) {
      LogD(2, prep("found Equivalent channel %s"), *equChannelID.ToString());
      cSchedule *pSchedule = (cSchedule *) Schedules->GetSchedule (equChannel, true);
      cEvent *pEqvEvent = (cEvent *) pSchedule->GetEvent (pEvent->EventID(), pEvent->StartTime());
      if (pEqvEvent) {
        LogD(3, prep("equivalent event exists"));
        if (pEqvEvent == pEvent) {
          LogD(3, prep("equal event exists"));

        } else {
          LogD(2, prep("remove equivalent"));
          pSchedule->DelEvent(pEqvEvent);
          cEvent* newEvent = new cEvent (pEvent->EventID());
          cloneEvent(pEvent, newEvent);

          pSchedule->AddEvent(newEvent);

        }

      } else {
        LogD(3, prep("equivalent event does not exist"));
        cEvent* newEvent = new cEvent (pEvent->EventID());
        cloneEvent(pEvent, newEvent);

        pSchedule->AddEvent(newEvent);

      }
    }
  }
}

void cEquivHandler::updateEquivalent(tChannelID channelID, cEvent *pEvent){
  multimap<string,string>::iterator it;
  pair<multimap<string,string>::iterator,multimap<string,string>::iterator> ret;

  LogD(3, prep("Start updateEquivalent %s"), *channelID.ToString());

  ret = equiChanMap.equal_range(*channelID.ToString());
  for (it=ret.first; it!=ret.second; ++it) {
    tChannelID equChannelID (tChannelID::FromString((*it).second.c_str()));
    LogD(3, prep("equivalent channel '%s' exists"), *equChannelID.ToString());
    cEvent* newEvent = new cEvent (pEvent->EventID());
    cloneEvent(pEvent, newEvent);

    AddEvent(newEvent, equChannelID);
  }
}


void cEquivHandler::sortEquivalents(tChannelID channelID, cSchedules* Schedules)
{
  multimap<string, string>::iterator it;
  pair < multimap < string, string > ::iterator, multimap < string, string
      > ::iterator > ret;
  LogD(2, prep("sortEquivalents for channel %s count: %d"), *channelID.ToString(), cEquivHandler::getEquiChanMap().count(*channelID.ToString()));
  
  ret = equiChanMap.equal_range(*channelID.ToString());
  for (it = ret.first; it != ret.second; ++it)
  {
    LogD(3, prep("equivalent channel exists"));
    tChannelID equChannelID(tChannelID::FromString((*it).second.c_str()));
    const cChannel* pChannel = GetChannelByID(equChannelID, false);
    if (pChannel)
    {
      LogD(2, prep("found Equivalent channel %s"), *equChannelID.ToString());
      cSchedule* pSchedule = (cSchedule *) Schedules->GetSchedule(pChannel, true);

      pSchedule->Sort();
#if APIVERSNUM >= 20300
      pSchedule->SetModified();
#else
      Schedules->SetModified(pSchedule);
#endif
    }
  }
}

void cEquivHandler::cloneEvent(cEvent *Source, cEvent *Dest) {

  Dest->SetEventID(Source->EventID());
  Dest->SetTableID(Source->TableID());
  Dest->SetVersion(Source->Version());
  Dest->SetRunningStatus(Source->RunningStatus());
  Dest->SetTitle(Source->Title());
  Dest->SetShortText(Source->ShortText());
  Dest->SetDescription(Source->Description());
  cComponents *components = new cComponents();
  if (Source->Components()) {
     for (int i = 0; i < Source->Components()->NumComponents(); ++i)
         components->SetComponent(i, Source->Components()->Component(i)->ToString());
     }
  Dest->SetComponents(components);
  uchar contents[MaxEventContents];
  for (int i = 0; i < MaxEventContents; ++i)
      contents[i] = Source->Contents(i);
  Dest->SetContents(contents);
  Dest->SetParentalRating(Source->ParentalRating());
  Dest->SetStartTime(Source->StartTime());
  Dest->SetDuration(Source->Duration());
  Dest->SetVps(Source->Vps());
  if (Source->Seen())
     Dest->SetSeen();
}

