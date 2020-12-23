/*
 * epgclone.h: EpgClone list item
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __EPGFIXER_EPGCLONE_H_
#define __EPGFIXER_EPGCLONE_H_

#include <vdr/epg.h>
#include <vdr/tools.h>
#include "tools.h"

class cEpgClone : public cListItem
{
private:
  int dest_num;
  char *dest_str;
  void CloneEvent(cEvent *Source, cEvent *Dest);

public:
  cEpgClone();
  virtual ~cEpgClone();
  using cListItem::Apply;
  virtual bool Apply(cEvent *Event);
  void SetFromString(char *string, bool Enabled);
};

// Global instance
extern cEpgfixerList<cEpgClone, cEvent> EpgfixerEpgClones;

#endif //__EPGFIXER_EPGCLONE_H_
