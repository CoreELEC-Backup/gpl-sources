/*
 * epgclone.c: EpgClone list item
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <stdlib.h>
#include <string.h>
#include "epgclone.h"

/* Global instance */
cEpgfixerList<cEpgClone, cEvent> EpgfixerEpgClones;

cEpgClone::cEpgClone()
{
  dest_num = 0;
  dest_str = NULL;
}

cEpgClone::~cEpgClone()
{
  free(dest_str);
}

void cEpgClone::CloneEvent(cEvent *Source, cEvent *Dest) {
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
  tChannelID channelID;
  if (dest_num) {
#if VDRVERSNUM >= 20301
     LOCK_CHANNELS_READ;
     const cChannel *dest_chan = Channels->GetByNumber(dest_num);
     if (dest_chan)
        channelID = Channels->GetByNumber(dest_num)->GetChannelID();
#else
     cChannel *dest_chan = Channels.GetByNumber(dest_num);
     if (dest_chan)
        channelID = Channels.GetByNumber(dest_num)->GetChannelID();
#endif
     else
        channelID = tChannelID::InvalidID;
     }
  else
     channelID = tChannelID::FromString(dest_str);
  if (channelID == tChannelID::InvalidID) {
     enabled = false;
     delete Dest;
     error("Destination channel %s not found for cloning, disabling cloning!", (dest_num ? *itoa(dest_num) : dest_str));
     }
  else
     AddEvent(Dest, channelID);
}

bool cEpgClone::Apply(cEvent *Event)
{
  if (Event && enabled && IsActive(Event->ChannelID())) {
     cEvent *event = new cEvent(Event->EventID());
     CloneEvent(Event, event);
     return true;
     }
  return false;
}

void cEpgClone::SetFromString(char *s, bool Enabled)
{
  dest_num = 0;
  FREE(dest_str);
  Free();
  cListItem::SetFromString(s, Enabled);
  if (enabled) {
     char *p = (s[0] == '!') ? s + 1 : s;
     char *f = strchr(p, '=');
     if (f) {
        *f = 0;
        if (atoi(f + 1))
           dest_num = atoi(f + 1);
        else
           dest_str = strdup(f + 1);
        numchannels = LoadChannelsFromString(p);
        }
     }
}
