/*
 * epghandler.c: EpgHandler
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "blacklist.h"
#include "epgclone.h"
#include "tools.h"
#include "epghandler.h"

bool cEpgfixerEpgHandler::FixEpgBugs(cEvent *Event)
{
  FixOriginalEpgBugs(Event);
  FixCharSets(Event);
  StripHTML(Event);
  FixBugs(Event);
  return false;
}

bool cEpgfixerEpgHandler::HandleEvent(cEvent *Event)
{
  return EpgfixerEpgClones.Apply(Event);
}

bool cEpgfixerEpgHandler::IgnoreChannel(const cChannel *Channel)
{
  return EpgfixerBlacklists.Apply((cChannel *)Channel);
}
