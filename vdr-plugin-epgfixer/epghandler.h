/*
 * epghandler.h: EpgHandler
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __EPGFIXER_EPGHANDLER_H
#define __EPGFIXER_EPGHANDLER_H

#include <vdr/channels.h>
#include <vdr/epg.h>

class cEpgfixerEpgHandler : public cEpgHandler
{
public:
  cEpgfixerEpgHandler(void) {};
#if VDRVERSNUM >= 20304
  virtual bool BeginSegmentTransfer(const cChannel *Channel, bool Dummy) { return true; }
#endif
  virtual bool HandleEvent(cEvent *Event);
  virtual bool IgnoreChannel(const cChannel *Channel);
  virtual bool FixEpgBugs(cEvent *Event);
};

#endif //__EPGFIXER_EPGHANDLER_H
