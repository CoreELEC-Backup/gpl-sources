/*
 * eit2.c
 *
 *  Created on: Oct 16, 2012
 *      Author: d.petrovski
 */
#include "eit2.h"

#include <string>
#include <vdr/config.h>
#include "log.h"
#include "dish.h"
#include "equivhandler.h"

using namespace std;
using namespace util;

namespace SI
{

cEvent* cEIT2::ProcessEitEvent(cSchedule* pSchedule,const SI::EIT::Event* EitEvent,
                               uchar Tid, uchar versionNumber)
{
  bool ExternalData = false;
  // Drop bogus events - but keep NVOD reference events, where all bits of the start time field are set to 1, resulting in a negative number.
  if (EitEvent->getStartTime () == 0 || (EitEvent->getStartTime () > 0 && EitEvent->getDuration () == 0))
    return NULL;
  Empty = false;
  if (!SegmentStart)
    SegmentStart = EitEvent->getStartTime ();
  SegmentEnd = EitEvent->getStartTime () + EitEvent->getDuration ();
  //    int versionNumber = getVersionNumber();

  cEvent *newEvent = NULL;
  cEvent *pEvent = (cEvent *) pSchedule->GetEvent (EitEvent->getEventId (), EitEvent->getStartTime ());
  if (!pEvent) {
    if (OnlyRunningStatus)
      return NULL;
    // If we don't have that event yet, we create a new one.
    // Otherwise we copy the information into the existing event anyway, because the data might have changed.
    pEvent = newEvent = new cEvent (EitEvent->getEventId ());
    if (!pEvent)
      return NULL;
  } else {
    //LogD(3, prep("existing event channelID: %s Title: %s TableID 0x%02X new TID 0x%02X Version %i, new version %i"), *channel->GetChannelID().ToString(), pEvent->Title(), pEvent->TableID(), Tid, pEvent->Version(), versionNumber);
    // We have found an existing event, either through its event ID or its start time.
    pEvent->SetSeen ();

    // If the existing event has a zero table ID it was defined externally and shall
    // not be overwritten.
    if (pEvent->TableID () == 0x00) {
      if (pEvent->Version () == versionNumber)
        return NULL;
      /*HasExternalData = */ExternalData = true;
    }
    // If the new event has a higher table ID, let's skip it.
    // The lower the table ID, the more "current" the information.
    else if (Tid > pEvent->TableID())
      return NULL;
    // If the new event comes from the same table and has the same version number
    // as the existing one, let's skip it to avoid unnecessary work.
    // Unfortunately some stations (like, e.g. "Premiere") broadcast their EPG data on several transponders (like
    // the actual Premiere transponder and the Sat.1/Pro7 transponder), but use different version numbers on
    // each of them :-( So if one DVB card is tuned to the Premiere transponder, while an other one is tuned
    // to the Sat.1/Pro7 transponder, events will keep toggling because of the bogus version numbers.
    else if (Tid == pEvent->TableID() && pEvent->Version() == versionNumber)
      return NULL;
  }
  if (!ExternalData) {
    pEvent->SetEventID (EitEvent->getEventId ()); // unfortunately some stations use different event ids for the same event in different tables :-(
    pEvent->SetTableID (Tid);
    pEvent->SetStartTime (EitEvent->getStartTime ());
    pEvent->SetDuration (EitEvent->getDuration ());
  }
  if (newEvent)
    pSchedule->AddEvent (newEvent);
  if (Tid == 0x4E) { // we trust only the present/following info on the actual TS
    if (EitEvent->getRunningStatus () >= SI::RunningStatusNotRunning)
    {
#if APIVERSNUM >= 20300
        pSchedule->SetRunningStatus (pEvent, EitEvent->getRunningStatus (), channel);
#else
        cChannel* chan = Channels.GetByChannelID(channel->GetChannelID());
        pSchedule->SetRunningStatus (pEvent, EitEvent->getRunningStatus (), chan);
#endif
    }
  }
  if (OnlyRunningStatus)
    return NULL;  // do this before setting the version, so that the full update can be done later
  pEvent->SetVersion (versionNumber);

  ProcessEventDescriptors(ExternalData, channel->Source(), Tid, EitEvent,
                          pEvent, channel->GetChannelID());

  Modified = true;
  return pEvent;
}

void cEIT2::ProcessEventDescriptors(bool ExternalData, int Source, u_char Tid, 
		const SI::EIT::Event* SiEitEvent, cEvent* pEvent, const tChannelID& channelId)
{

  const cEvent *rEvent = NULL;
  int LanguagePreferenceShort = -1;
  int LanguagePreferenceExt = -1;
  unsigned char nDescriptorTag;
  bool UseExtendedEventDescriptor = false;
  SI::Descriptor * d;
  SI::ExtendedEventDescriptors * ExtendedEventDescriptors = NULL;
  SI::ShortEventDescriptor * ShortEventDescriptor = NULL;
  //SI::DishDescriptor *DishExtendedEventDescriptor = NULL;
  SI::DishDescriptor *DishEventDescriptor = NULL;
  //uchar DishTheme = 0, DishCategory = 0;


  //cLinkChannels *LinkChannels = NULL;
  cComponents *Components = NULL;

/*#if APIVERSNUM >= 20300
  cChannel *channel = Channels->GetByChannelID(channelId);
#else
  cChannel *channel = Channels.GetByChannelID(channelId);
#endif*/

  DescriptorLoop dl = SiEitEvent->eventDescriptors;
  for (SI::Loop::Iterator it2; (d = dl.getNext(it2)); )
  {
    if (ExternalData && d->getDescriptorTag() != SI::ComponentDescriptorTag)
    {
      delete d;
      LogD(2, prep("continue:d->getDescriptorTAG():%x)"), d->getDescriptorTag ());
      continue;
    }

    //LogD(2, prep("EEPGDEBUG:d->getDescriptorTAG():%x)"), d->getDescriptorTag ());
    nDescriptorTag = d->getDescriptorTag();
    switch (nDescriptorTag) {
    case SI::ExtendedEventDescriptorTag: {
      SI::ExtendedEventDescriptor * eed = (SI::ExtendedEventDescriptor *) d;
      if (I18nIsPreferredLanguage(Setup.EPGLanguages, eed->languageCode, LanguagePreferenceExt)
        || !ExtendedEventDescriptors) {
        delete ExtendedEventDescriptors;
        ExtendedEventDescriptors = new SI::ExtendedEventDescriptors;
        UseExtendedEventDescriptor = true;
      }
      if (UseExtendedEventDescriptor) {
#if APIVERSNUM < 20109
        ExtendedEventDescriptors->Add(eed);
#else
        if (ExtendedEventDescriptors->Add(eed))
#endif
          d = NULL; // so that it is not deleted
      }
      if (eed->getDescriptorNumber() == eed->getLastDescriptorNumber())
        UseExtendedEventDescriptor = false;
    }
    break;
    case SI::ShortEventDescriptorTag: {
      SI::ShortEventDescriptor * sed = (SI::ShortEventDescriptor *) d;
      if (I18nIsPreferredLanguage(Setup.EPGLanguages, sed->languageCode, LanguagePreferenceShort)
        || !ShortEventDescriptor) {
        delete ShortEventDescriptor;
        ShortEventDescriptor = sed;
        d = NULL; // so that it is not deleted
      }
    }
    break;
#if APIVERSNUM > 10711
    case SI::ContentDescriptorTag: {
      SI::ContentDescriptor *cd = (SI::ContentDescriptor *) d;
      SI::ContentDescriptor::Nibble Nibble;
      int NumContents = 0;
      uchar Contents[MaxEventContents] = { 0 };
      for (SI::Loop::Iterator it3; cd->nibbleLoop.getNext(Nibble, it3);) {
        if (NumContents < MaxEventContents) {
          Contents[NumContents] = ((Nibble.getContentNibbleLevel1() & 0xF) << 4)
            | (Nibble.getContentNibbleLevel2() & 0xF);
          NumContents++;
        }
        if (DishEventDescriptor && NumContents == 1) {
          DishEventDescriptor->setContent(Nibble);
        }
        //          LogD(2, prep("EEPGDEBUG:Nibble:%x-%x-%x-%x)"), Nibble.getContentNibbleLevel1(),Nibble.getContentNibbleLevel2()
        //              , Nibble.getUserNibble1(), Nibble.getUserNibble2());

      }
      pEvent->SetContents(Contents);
    }
    break;
#endif
    case SI::ParentalRatingDescriptorTag: {
      int LanguagePreferenceRating = -1;
      SI::ParentalRatingDescriptor *prd = (SI::ParentalRatingDescriptor *) d;
      SI::ParentalRatingDescriptor::Rating Rating;
      for (SI::Loop::Iterator it3; prd->ratingLoop.getNext(Rating, it3);) {
        if (I18nIsPreferredLanguage(Setup.EPGLanguages, Rating.languageCode,
          LanguagePreferenceRating)) {
          int ParentalRating = (Rating.getRating() & 0xFF);
          switch (ParentalRating) {
          // values defined by the DVB standard (minimum age = rating + 3 years):
          case 0x01 ... 0x0F: ParentalRating += 3; break;
          // values defined by broadcaster CSAT (now why didn't they just use 0x07, 0x09 and 0x0D?):
          case 0x11: ParentalRating = 10; break;
          case 0x12: ParentalRating = 12; break;
          case 0x13: ParentalRating = 16; break;
          default: ParentalRating = 0;
          }
          pEvent->SetParentalRating(ParentalRating);
        }
      }
    }
    break;
    case SI::PDCDescriptorTag: {
      SI::PDCDescriptor * pd = (SI::PDCDescriptor *) d;
      time_t now = time(NULL);
      struct tm tm_r;
      struct tm t = *localtime_r(&now, &tm_r); // this initializes the time zone in 't'
      t.tm_isdst = -1; // makes sure mktime() will determine the correct DST setting
      int month = t.tm_mon;
      t.tm_mon = pd->getMonth() - 1;
      t.tm_mday = pd->getDay();
      t.tm_hour = pd->getHour();
      t.tm_min = pd->getMinute();
      t.tm_sec = 0;
      if (month == 11 && t.tm_mon == 0) // current month is dec, but event is in jan
        t.tm_year++;
      else if (month == 0 && t.tm_mon == 11) // current month is jan, but event is in dec
      t.tm_year--;
      time_t vps = mktime(&t);
      pEvent->SetVps(vps);
    }
    break;
    case SI::TimeShiftedEventDescriptorTag: {
#if APIVERSNUM >= 20300
      LOCK_SCHEDULES_READ;
#else
      cSchedulesLock SchedulesLock;
      cSchedules *Schedules = (cSchedules*)(cSchedules::Schedules(SchedulesLock));
#endif
      if (Schedules) {
        SI::TimeShiftedEventDescriptor * tsed = (SI::TimeShiftedEventDescriptor *) d;
        const cSchedule *rSchedule = Schedules->GetSchedule(
          tChannelID(Source, channel->Nid(), channel->Tid(), tsed->getReferenceServiceId()));
        if (!rSchedule)
          break;
        rEvent = rSchedule->GetEvent(tsed->getReferenceEventId());
        if (!rEvent)
          break;
        pEvent->SetTitle(rEvent->Title());
        pEvent->SetShortText(rEvent->ShortText());
        pEvent->SetDescription(rEvent->Description());
      }
    }
    break;
    case SI::LinkageDescriptorTag: {
    //Leave channel linking to VDR
/*      SI::LinkageDescriptor * ld = (SI::LinkageDescriptor *) d;
      tChannelID linkID(Source, ld->getOriginalNetworkId(), ld->getTransportStreamId(),
        ld->getServiceId());
      if (ld->getLinkageType() == 0xB0) { // Premiere World
        time_t now = time(NULL);
        bool hit = SiEitEvent->getStartTime() <= now
          && now < SiEitEvent->getStartTime() + SiEitEvent->getDuration();
        if (hit) {
          char linkName[ld->privateData.getLength() + 1];
          strn0cpy(linkName, (const char *) ld->privateData.getData(), sizeof(linkName));
          // TODO is there a standard way to determine the character set of this string?
#if APIVERSNUM >= 20300
          cChannel *link = Channels->GetByChannelID(linkID);
#else
          cChannel *link = Channels.GetByChannelID(linkID);
#endif
          if (link != channel) { // only link to other channels, not the same one
            //fprintf(stderr, "Linkage %s %4d %4d %5d %5d %5d %5d  %02X  '%s'\n", hit ? "*" : "", channel->Number(), link ? link->Number() : -1, SiEitEvent.getEventId(), ld->getOriginalNetworkId(), ld->getTransportStreamId(), ld->getServiceId(), ld->getLinkageType(), linkName);//XXX
            if (link) {
              if (Setup.UpdateChannels == 1 || Setup.UpdateChannels >= 3)
                link->SetName(linkName, "", "");
            }
            else if (Setup.UpdateChannels >= 4) {
              const cChannel *transponder = channel;
              if (channel->Tid() != ld->getTransportStreamId())
#if APIVERSNUM >= 20300
                transponder = Channels->GetByTransponderID(linkID);
              link = Channels->NewChannel(transponder, linkName, "", "", ld->getOriginalNetworkId(),
#else
                transponder = Channels.GetByTransponderID(linkID);
              link = Channels.NewChannel(transponder, linkName, "", "", ld->getOriginalNetworkId(),
#endif
                ld->getTransportStreamId(), ld->getServiceId());
            }
            if (link) {
              if (!LinkChannels) LinkChannels = new cLinkChannels;
              LinkChannels->Add(new cLinkChannel(link));
            }
          }
          else
            channel->SetPortalName(linkName);
        }
      }*/
    }
    break;
    case SI::ComponentDescriptorTag: {
      SI::ComponentDescriptor * cd = (SI::ComponentDescriptor *) d;
      uchar Stream = cd->getStreamContent();
      uchar Type = cd->getComponentType();
      //if (1 <= Stream && Stream <= 3 && Type != 0) { // 1=video, 2=audio, 3=subtitles
      if (1 <= Stream && Stream <= 6 && Type != 0) { // 1=MPEG2-video, 2=MPEG1-audio, 3=subtitles, 4=AC3-audio, 5=H.264-video, 6=HEAAC-audio
        if (!Components)
          Components = new cComponents;
        char buffer[Utf8BufSize (256)];
        Components->SetComponent(Components->NumComponents(), Stream,
          Type, I18nNormalizeLanguageCode(cd->languageCode),
          cd->description.getText(buffer, sizeof(buffer)));
      }
    }
    break;
    case SI::DishExtendedEventDescriptorTag: {
      SI::UnimplementedDescriptor *deed = (SI::UnimplementedDescriptor *) d;
      if (!DishEventDescriptor) {
        DishEventDescriptor = new SI::DishDescriptor();
      }
      DishEventDescriptor->setExtendedtData(Tid + 1, deed->getData());
      //          HasExternalData = true;
    }
    break;
    case SI::DishShortEventDescriptorTag: {
      SI::UnimplementedDescriptor *dsed = (SI::UnimplementedDescriptor *) d;
      if (!DishEventDescriptor) {
        DishEventDescriptor = new SI::DishDescriptor();
      }
      DishEventDescriptor->setShortData(Tid + 1, dsed->getData());
      //          HasExternalData = true;
    }
    break;
    case SI::DishRatingDescriptorTag: {
      if (d->getLength() == 4) {
        if (!DishEventDescriptor) {
          DishEventDescriptor = new SI::DishDescriptor();
        }
        uint16_t rating = d->getData().TwoBytes(2);
        DishEventDescriptor->setRating(rating);
      }
    }
    break;
    case SI::DishSeriesDescriptorTag: {
      if (d->getLength() == 10) {
        //LogD(2, prep("DishSeriesDescriptorTag: %s)"), (const char*) d->getData().getData());
        if (!DishEventDescriptor) {
          DishEventDescriptor = new SI::DishDescriptor();
        }
        DishEventDescriptor->setEpisodeInfo(d->getData());
      }
      //        else {
      //            LogD(2, prep("DishSeriesDescriptorTag length: %d)"), d->getLength());
      //        }
    }
    break;
    default:
      break;
    }
    delete d;
  }
  if (!rEvent) {
    if (ShortEventDescriptor) {
      char buffer[Utf8BufSize (256)];
      unsigned char *f;
      int l = ShortEventDescriptor->name.getLength();
      f = (unsigned char *) ShortEventDescriptor->name.getData().getData();
      decodeText2 (f, l, buffer, sizeof (buffer));
      //ShortEventDescriptor->name.getText(buffer, sizeof(buffer));
      LogD(2, prep("Title: %s Decoded: %s"), f, buffer);
      pEvent->SetTitle (buffer);
      LogD(3, prep("channelID: %s Title: %s"), *channel->GetChannelID().ToString(), pEvent->Title());
      l = ShortEventDescriptor->text.getLength();
      if (l > 0) { //Set the Short Text only if there is data so that we do not overwrite valid data
        f = (unsigned char *) ShortEventDescriptor->text.getData().getData();
        decodeText2 (f, l, buffer, sizeof (buffer));
        //ShortEventDescriptor->text.getText(buffer, sizeof(buffer));
        pEvent->SetShortText (buffer);
      }
      LogD(3, prep("ShortText: %s"), pEvent->ShortText());
      LogD(2, prep("ShortText: %s Decoded: %s"), f, buffer);
    } else if (/*!HasExternalData*/!DishEventDescriptor) {
      pEvent->SetTitle (NULL);
      pEvent->SetShortText (NULL);
      LogD(3, prep("SetTitle (NULL)"));
    }
    if (ExtendedEventDescriptors) {
      char buffer[Utf8BufSize (ExtendedEventDescriptors->getMaximumTextLength (": ")) + 1];
      pEvent->SetDescription (ExtendedEventDescriptors->getText (buffer, sizeof (buffer), ": "));
      LogD(3, prep("Description: %s"), pEvent->Description());
    } else if (!/*HasExternalData*/DishEventDescriptor)
      pEvent->SetDescription (NULL);

    if (DishEventDescriptor) {
      if (DishEventDescriptor->getName())
        pEvent->SetTitle(DishEventDescriptor->getName());
      //LogD(2, prep("channelID: %s DishTitle: %s"), *channel->GetChannelID().ToString(), DishEventDescriptor->getName());
      pEvent->SetShortText(DishEventDescriptor->getShortText());
      char *tmp;
      string fmt;

      const char * description = DishEventDescriptor->getDescription();
      //BEV sets the description previously with ExtendedEventDescriptor
      if (0 == strcmp(DishEventDescriptor->getDescription(),"") && pEvent->Description())
        description = pEvent->Description();


      fmt = "%s";
      if (DishEventDescriptor->hasTheme()) {
        fmt += "\nTheme: ";
      }
      fmt += "%s";
      if (DishEventDescriptor->hasCategory()) {
        fmt += "\nCategory: ";
      }
      fmt += "%s";

      if ((0 != strcmp(DishEventDescriptor->getRating(),"")
        || 0 != strcmp(DishEventDescriptor->getStarRating(),""))) {
        fmt += "\n\nRating: ";
      }
      fmt += "%s %s";
      if (0 != strcmp(DishEventDescriptor->getProgramId(),"")) {
        fmt += "\n\nProgram ID: ";
      }
      fmt += "%s %s%s";
      time_t orgAirDate = DishEventDescriptor->getOriginalAirDate();
      char datestr [80];
      bool dateok = false;
      if (orgAirDate != 0) {
        dateok = strftime (datestr,80," \nOriginal Air Date: %a %b %d %Y",gmtime(&orgAirDate)) > 0;
      }

      Asprintf (&tmp, fmt.c_str(), description
                , DishEventDescriptor->getTheme(), DishEventDescriptor->getCategory()
                , DishEventDescriptor->getRating(), DishEventDescriptor->getStarRating()
                , DishEventDescriptor->getProgramId(), DishEventDescriptor->getSeriesId()
                , orgAirDate == 0 || !dateok ? "" : datestr);
      pEvent->SetDescription(tmp);
      free(tmp);

      //LogD(2, prep("DishDescription: %s"), DishEventDescriptor->getDescription());
      //LogD(2, prep("DishShortText: %s"), DishEventDescriptor->getShortText());
    }

  }
  delete ExtendedEventDescriptors;
  delete ShortEventDescriptor;
  delete DishEventDescriptor;

  pEvent->SetComponents (Components);

  //    LogD(2, prep("channelID: %s Title: %s"), *channel->GetChannelID().ToString(), pEvent->Title());

  //    if (pEvent->ChannelID() == tChannelID::FromString("S119.0W-4100-6-110-110")) {
  //      LogD(2, prep("ID: %d Title: %s Time: %d Tid: 0x%x"), pEvent->EventID(), pEvent->Title(), pEvent->StartTime(), pEvent->TableID());
  //    }

  //FixEpgBugs removes newlines from description which is not wanted especially for DISH/BEV
  if (Format != DISH_BEV)
    pEvent->FixEpgBugs();

  //if (LinkChannels)
    //channel->SetLinkChannels (LinkChannels);
}

cEIT2::cEIT2 (int Source, u_char Tid, const u_char * Data, EFormat format, bool isEITPid)
:  SI::EIT (Data, false)
, OnlyRunningStatus(false)
, Format(format)
{

  //LogD(2, prep("cEIT2::cEIT2"));
  if (Tid > 0 && (Format == DISH_BEV || (cSetupEEPG::getInstance()->ProcessEIT && isEITPid))) Tid--;

  if (!CheckCRCAndParse ()) {
    LogD(2, prep("!CheckCRCAndParse ()"));
    return;
  }

  bool searchOtherSatPositions = Format == DISH_BEV;

  tChannelID channelID (Source, getOriginalNetworkId (), getTransportStreamId (), getServiceId ());
  channel = GetChannelByID (channelID, searchOtherSatPositions);
  if (!channel) {
    LogD(3, prep("!channel channelID: %s"), *channelID.ToString());
    return; // only collect data for known channels
  }

  //LogD(5, prep("channelID: %s format:%d"), *channel->GetChannelID().ToString(), Format);

#if APIVERSNUM >= 20300
  LOCK_CHANNELS_WRITE;
  if (!Channels) {
     LogD(3, prep("Error obtaining channels lock"));
     return;
  }
  LOCK_SCHEDULES_WRITE;
  if (!Schedules) {
     LogD(3, prep("Error obtaining schedules lock"));
     return;
  }
#else
  cChannelsLock ChannelsLock(true, 10), ChannelsLockR;
  cChannels *Channels = (cChannels*)(cChannels::Channels(ChannelsLock));
  cSchedulesLock SchedulesLock(true, 10), SchedulesLockR;
  cSchedules *Schedules = (cSchedules*)(cSchedules::Schedules(SchedulesLock));
  if (!Channels) {
     LogD(3, prep("Error obtaining channels lock"));
     OnlyRunningStatus = true;
     cChannels *Channels = (cChannels*)(cChannels::Channels(ChannelsLockR));
  }
  if (!Schedules) {
    // If we don't get a write lock, let's at least get a read lock, so
    // that we can set the running status and 'seen' timestamp (well, actually
    // with a read lock we shouldn't be doing that, but it's only integers that
    // get changed, so it should be ok)
     LogD(3, prep("Error obtaining schedules lock"));
     OnlyRunningStatus = true;
     cSchedules *Schedules = (cSchedules*)(cSchedules::Schedules(SchedulesLockR));
  }
  if (!Schedules || !Channels) {
     LogD(3, prep("Error obtaining read lock"));
     return;
  }
#endif
  cSchedule *pSchedule = (cSchedule *) Schedules->GetSchedule (channel, true);

  Empty = true;
  Modified = false;
  //    bool HasExternalData = false;
  SegmentStart = 0;
  SegmentEnd = 0;

  SI::EIT::Event SiEitEvent;
  for (SI::Loop::Iterator it; eventLoop.getNext (SiEitEvent, it);) {
    int versionNumber = getVersionNumber();
    //      bool ExternalData = false;
    //      // Drop bogus events - but keep NVOD reference events, where all bits of the start time field are set to 1, resulting in a negative number.
    //      if (SiEitEvent.getStartTime () == 0 || (SiEitEvent.getStartTime () > 0 && SiEitEvent.getDuration () == 0))
    //        continue;
    //      Empty = false;
    //      if (!SegmentStart)
    //        SegmentStart = SiEitEvent.getStartTime ();
    //      SegmentEnd = SiEitEvent.getStartTime () + SiEitEvent.getDuration ();
    //      int versionNumber = getVersionNumber();
    //
    //      cEvent *newEvent = NULL;
    //      cEvent *pEvent = (cEvent *) pSchedule->GetEvent (SiEitEvent.getEventId (), SiEitEvent.getStartTime ());
    //      if (!pEvent) {
    //        if (OnlyRunningStatus)
    //          continue;
    //        // If we don't have that event yet, we create a new one.
    //        // Otherwise we copy the information into the existing event anyway, because the data might have changed.
    //        pEvent = newEvent = new cEvent (SiEitEvent.getEventId ());
    //        if (!pEvent)
    //          continue;
    //      } else {
    //        //LogD(3, prep("existing event channelID: %s Title: %s TableID 0x%02X new TID 0x%02X Version %i, new version %i"), *channel->GetChannelID().ToString(), pEvent->Title(), pEvent->TableID(), Tid, pEvent->Version(), versionNumber);
    //        // We have found an existing event, either through its event ID or its start time.
    //        pEvent->SetSeen ();
    //
    //        // If the existing event has a zero table ID it was defined externally and shall
    //        // not be overwritten.
    //        if (pEvent->TableID () == 0x00) {
    //          if (pEvent->Version () == versionNumber)
    //            continue;
    //          /*HasExternalData = */ExternalData = true;
    //        }
    //        // If the new event has a higher table ID, let's skip it.
    //        // The lower the table ID, the more "current" the information.
    //        else if (Tid > pEvent->TableID())
    //          continue;
    //        // If the new event comes from the same table and has the same version number
    //        // as the existing one, let's skip it to avoid unnecessary work.
    //        // Unfortunately some stations (like, e.g. "Premiere") broadcast their EPG data on several transponders (like
    //        // the actual Premiere transponder and the Sat.1/Pro7 transponder), but use different version numbers on
    //        // each of them :-( So if one DVB card is tuned to the Premiere transponder, while an other one is tuned
    //        // to the Sat.1/Pro7 transponder, events will keep toggling because of the bogus version numbers.
    //        else if (Tid == pEvent->TableID() && pEvent->Version() == versionNumber)
    //          continue;
    //      }
    //      if (!ExternalData) {
    //        pEvent->SetEventID (SiEitEvent.getEventId ()); // unfortunately some stations use different event ids for the same event in different tables :-(
    //        pEvent->SetTableID (Tid);
    //        pEvent->SetStartTime (SiEitEvent.getStartTime ());
    //        pEvent->SetDuration (SiEitEvent.getDuration ());
    //      }
    //      if (newEvent)
    //        pSchedule->AddEvent (newEvent);
    //      if (Tid == 0x4E) { // we trust only the present/following info on the actual TS
    //        if (SiEitEvent.getRunningStatus () >= SI::RunningStatusNotRunning)
    //          pSchedule->SetRunningStatus (pEvent, SiEitEvent.getRunningStatus (), channel);
    //      }
    //      if (OnlyRunningStatus)
    //        continue;  // do this before setting the version, so that the full update can be done later
    //      pEvent->SetVersion (versionNumber);
    //
    //      ProcessEventDescriptors(ExternalData, Source, Tid, SiEitEvent,
    //            pEvent, Schedules, channel);
    //
    //      Modified = true;
    cEvent *pEvent = ProcessEitEvent(pSchedule, &SiEitEvent,
      Tid, versionNumber);
    if (pEvent)
      EquivHandler->updateEquivalent(Schedules, channel->GetChannelID(), pEvent);
  }

  ////

  if (Empty && Tid == 0x4E && getSectionNumber () == 0)
  // ETR 211: an empty entry in section 0 of table 0x4E means there is currently no event running
  {
#if APIVERSNUM >= 20300
      cChannel *chan = Channels->GetByChannelID(channel->GetChannelID());
#else
      cChannel *chan = Channels.GetByChannelID(channel->GetChannelID());
#endif
      pSchedule->ClrRunningStatus (chan);
  }
  if (Tid == 0x4E)
    pSchedule->SetPresentSeen ();
  if (OnlyRunningStatus) {
    LogD(4, prep("OnlyRunningStatus"));
    return;
  }
  if (Modified) {
    //      if (!HasExternalData)
    pSchedule->DropOutdated (SegmentStart, SegmentEnd, Tid, getVersionNumber ());
    sortSchedules(Schedules, channel->GetChannelID());
  }
  LogD(4, prep("end of cEIT2"));

}
//end of cEIT2

cEIT2::cEIT2 (cSchedule * Schedule, EFormat format)
: Empty(true)
, Modified(false)
, OnlyRunningStatus(false)
, SegmentStart(0)
, SegmentEnd(0)
, Format(format)
{
  //LogD(2, prep("cEIT2::cEIT2"));
  //    if (Tid > 0 && (Format == DISH_BEV || (SetupPE->ProcessEIT && isEITPid))) Tid--;

  bool searchOtherSatPositions = Format == DISH_BEV;

  tChannelID channelID = Schedule->ChannelID();
  channel = GetChannelByID (channelID, searchOtherSatPositions);
  if (!channel) {
    LogD(3, prep("!channel channelID: %s"), *channelID.ToString());
    return; // only collect data for known channels
  }
}
} //end namespace SI

