/*
 * cEEpgHandler.c
 *
 *  Created on: 11.3.2012
 *      Author: d.petrovski
 */

#include "epghandler.h"
#if APIVERSNUM > 10725
#include "log.h"
#include "equivhandler.h"
#include "eit2.h"
#include "util.h"
#include <vdr/sources.h>
#include <libsi/si.h>

using namespace util;


cEEpgHandler::cEEpgHandler() {
  LogD(4, prep("cEEpgHandler()"));
  equivHandler = new cEquivHandler();
  modified = false;
  charsetFixer = new cCharsetFixer();
  schedule = NULL;
  searchDuplicates = true;

}

cEEpgHandler::~cEEpgHandler() {
  delete equivHandler;
  equivHandler = NULL;
}


bool cEEpgHandler::HandleEitEvent(cSchedule* Schedule,
                                  const SI::EIT::Event* EitEvent, uchar TableID, uchar Version) {
  schedule = Schedule;
  //LogD(1, prep("HandleEitEvent"));
  //DISH NID 0x1001 to 0x100B BEV 0x100 and 0x101
  int nid = schedule->ChannelID().Nid();
  if ((nid >= 0x1001 && nid <= 0x100B) || nid == 0x101 || nid == 0x100 || nid == 0x233A) {
    //Set the Format for Eit events so that the new lines are not erased with FixEpgBugs
    EFormat Format;
    if (nid == 0x233A) Format = FREEVIEW;
    else Format = DISH_BEV;

    SI::cEIT2 eit2(Schedule, Format);
    eit2.ProcessEitEvent(Schedule, EitEvent, TableID, Version);
    return true;
  }

  //TODO Should it be added in setup?
  if (EitEvent->getDurationHour() > _LONG_EVENT_HOURS) {
    LogD(4, prep("Event longer than 10h Duration:%d DurationHour:%d StartTimeHour:%d"), EitEvent->getDuration(), EitEvent->getDurationHour(), EitEvent->getStartTimeHour());
    const cEvent* exEvent = Schedule->GetEventAround(EitEvent->getStartTime()+EitEvent->getDuration()/3);
    if (exEvent) {
      const cEvent* exEvent2 = Schedule->GetEventAround(EitEvent->getStartTime()+EitEvent->getDuration()/3*2);
      if (exEvent2 && exEvent != exEvent2) {
        LogD(2, prep("EitEvent overrides existing events '%s', '%s' ... Skipping"), *exEvent->Title(), *exEvent2->Title());
        return true;
      }
    }
  }

  modified = false;
  //VDR creates new event if the EitEvent StartTime is different than EEPG time so
  //the EPG event has to be deleted but the data should be kept
  const cEvent* ev = schedule->GetEvent(EitEvent->getEventId(),EitEvent->getStartTime());
  searchDuplicates = !ev; //if the event exist with a same start time, it is handled by SetShortText/SetDescription
  if (!ev){
      ev = schedule->GetEvent(EitEvent->getEventId());
      // remove shifted duplicates with same ID
      if (ev && ((ev->StartTime()>EitEvent->getStartTime() && ev->StartTime() < EitEvent->getStartTime()+EitEvent->getDuration())
          || (EitEvent->getStartTime() > ev->StartTime() && EitEvent->getStartTime() < ev->EndTime()))) {
        LogD(0, prep("!!!Deleting Event id:%d title:%s start_time:%d new_start_time:%d duration:%d new_duration:%d"), 
            ev->EventID(), ev->Title(), ev->StartTime(), EitEvent->getStartTime(), ev->Duration(), EitEvent->getDuration());
        RemoveEvent((cEvent*)ev);
        searchDuplicates = false;
      }
  }

  return false;
  //	return true;
}

bool cEEpgHandler::SetEventID(cEvent* Event, tEventID EventID) {
  Event->SetEventID(EventID);
  return true;
}

void cEEpgHandler::RemoveEvent(cEvent* ev)
{
  if (ev->Description() && strcmp(ev->Description(), "") != 0)
    origDescription = ev->Description();

  if (ev->ShortText() && strcmp(ev->ShortText(), "") != 0)
    origShortText = ev->ShortText();

  LogD(4, prep("!!!Deleting Event id:%d title:%s start_time:%d duration:%d"),
      ev->EventID(), ev->Title(), ev->StartTime(), ev->Duration());
  schedule->DelEvent((ev)); //The equivalents should be handled on adding equivalent event.
}

//Find duplicate events by title / time
//Sometimes same events overlap and have different EventID
void cEEpgHandler::FindDuplicate(cEvent* Event, const char* newTitle)
{
  if (!newTitle || !searchDuplicates) return;

  for (const cEvent *ev = schedule->Events()->First(); ev; ev = schedule->Events()->Next(ev)) {
    //assume that events are already sorted.
    if (ev->StartTime() > Event->EndTime()) break;
    if (ev->Title() && strcasecmp(ev->Title(), newTitle) == 0
        && ((Event->StartTime() > ev->StartTime() && Event->StartTime() < ev->EndTime())
        || (ev->StartTime() > Event->StartTime() && ev->StartTime() < Event->EndTime()))){

      LogD(0, prep("!!!Deleting Event id o:%d n:%d; title o:%s n:%s; start_time o:%d n:%d; duration o:%d n:%d"),
          ev->EventID(), Event->EventID(), ev->Title(), newTitle, ev->StartTime(), Event->StartTime(), ev->Duration(), Event->Duration());
      RemoveEvent((cEvent*)ev);
      break;
    }
  }
}

bool cEEpgHandler::SetTitle(cEvent* Event, const char* Title) {
  LogD(3, prep("Event id:%d title:%s new title:%s"), Event->EventID(), Event->Title(), Title);

  const char* title = charsetFixer->FixCharset(Title);

  //Sometimes same events overlap and have different EventID
  //Find/Remove duplicates with same title/time
  FindDuplicate(Event, title);

  if (!Event->Title() || (title && (!strcmp(Event->Title(),"") || (strcmp(Title,"") && strcmp(Event->Title(),title))))) {
    //LogD(0, prep("Event id:%d title:%s new title:%s"), Event->EventID(), Event->Title(), Title);
    modified = true;
    Event->SetTitle(title);
  }
  return true;
}

bool cEEpgHandler::SetShortText(cEvent* Event, const char* ShortText) {
  LogD(3, prep("Event id:%d ShortText:%s new ShortText:%s"), Event->EventID(), Event->ShortText(), ShortText);

  if (Event->ShortText() && strcmp(Event->ShortText(),"") != 0) {
    origShortText = std::string(Event->ShortText());
  }
  else {
    origShortText.clear();
  }

  const char*  shText = charsetFixer->FixCharset(ShortText);

  //if (!Event->ShortText() || ShortText && (!strcmp(Event->ShortText(),"") || (strcmp(ShortText,"") && strcmp(Event->ShortText(),ShortText))))
  Event->SetShortText(shText);
  return true;
}

bool cEEpgHandler::SetDescription(cEvent* Event, const char* Description) {
  LogD(3, prep("Event id:%d Description:%s new Description:%s"), Event->EventID(), Event->Description(), Description);

  if (Event->Description() && strcmp(Event->Description(),"") != 0)
    origDescription = Event->Description();
  else
    origDescription.clear();

  const char*  desc = charsetFixer->FixCharset(Description);

  //Based on asumption that SetDescription is always called after SetTitle
  if (!modified && desc && (!Event->Description() || strcmp(Event->Description(),desc) ))
    modified = true;

  //if (!Event->Description() || Description && (!strcmp(Event->Description(),"") || (strcmp(Description,"") && strcmp(Event->Description(),Description))))
  Event->SetDescription(desc);
  return true;
}

bool cEEpgHandler::SetContents(cEvent* Event, uchar* Contents) {
  Event->SetContents(Contents);
  return true;
}

bool cEEpgHandler::SetParentalRating(cEvent* Event, int ParentalRating) {
  Event->SetParentalRating(ParentalRating);
  return true;
}

bool cEEpgHandler::SetStartTime(cEvent* Event, time_t StartTime) {
  Event->SetStartTime(StartTime);
  return true;
}

bool cEEpgHandler::SetDuration(cEvent* Event, int Duration) {
  Event->SetDuration(Duration);
  return true;
}

bool cEEpgHandler::SetVps(cEvent* Event, time_t Vps) {
  Event->SetVps(Vps);
  return true;
}

string cEEpgHandler::ExtractAttribute(const char* name)
{
  string attribute;
  size_t apos = origDescription.find(name);
  if (apos != string::npos) {
    apos += strlen(name);
    size_t npos = origDescription.find('\n', apos);
    attribute = origDescription.substr(apos, npos - apos);
    //LogD(0, prep("ExtractAttribute attribute:%s, apos:%i, pos:%i"),attribute.c_str(), catpos, pos);
  }
  return attribute;
}

bool cEEpgHandler::HandleEvent(cEvent* Event) {

  LogD(3, prep("HandleEvent st:%s ost:%s desc:%s odesc:%s"),Event->ShortText(),origShortText.c_str(),Event->Description(),origDescription.c_str());
  //After FixEpgBugs of cEvent set the original Short Text if empty
  if (!Event->ShortText() || !strcmp(Event->ShortText(),""))
    Event->SetShortText(origShortText.c_str());

  //Handle the Category and Genre, and optionally future tags
  if (!origDescription.empty() &&
      (!Event->Description() ||
          (Event->Description() && origDescription.find(Event->Description()) != string::npos))) {
    Event->SetDescription(origDescription.c_str());
  } else if (!origDescription.empty() && Event->Description()) {

    string category = ExtractAttribute(CATEGORY);
    string genre = ExtractAttribute(GENRE);

    string desc = Event->Description() ? Event->Description() : "";
    if (!category.empty()) desc += '\n' + string(CATEGORY) + category;
    if (!genre.empty()) desc += '\n' + string(GENRE) + genre;
    Event->SetDescription (desc.c_str());

  }

  if (equivHandler->getEquiChanMap().count(*Event->ChannelID().ToString()) <= 0)
    return true;

  if (modified)
    equivHandler->updateEquivalent(Event->ChannelID(), Event);

  //Release the schedule
  schedule = NULL;
  //TODO just to see the difference
  //else if (!origDescription.empty() && !origDescription.compare(Event->Description())) {
  //	origDescription.append(" | EIT: ");
  //	origDescription.append(Event->Description());
  //	Event->SetDescription(origDescription.c_str());
  //  }

  return true;
}


bool cEEpgHandler::SortSchedule(cSchedule* Schedule) {

  Schedule->Sort();

  return true;
}

bool cEEpgHandler::FixEpgBugs(cEvent* Event)
{
  //to see which channels have bugs - disable fixing with true
  return false;
}

bool cEEpgHandler::IgnoreChannel(const cChannel* Channel)
{
  charsetFixer->InitCharsets(Channel);

  return false;
}

bool cEEpgHandler::DropOutdated(cSchedule* Schedule, time_t SegmentStart,
                                time_t SegmentEnd, uchar TableID, uchar Version) {
  return false;
}

#endif
