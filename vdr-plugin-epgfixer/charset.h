/*
 * charset.h: Character set list item
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __EPGFIXER_CHARSET_H_
#define __EPGFIXER_CHARSET_H_

#include <vdr/epg.h>
#include <vdr/tools.h>
#include "tools.h"

class cCharSet : public cListItem
{
private:
  char *origcharset;
  char *realcharset;

public:
  cCharSet();
  virtual ~cCharSet();
  using cListItem::Apply;
  virtual bool Apply(cEvent *Event);
  void SetFromString(char *string, bool Enabled);
};

// Global instance
extern cEpgfixerList<cCharSet, cEvent> EpgfixerCharSets;

#endif //__EPGFIXER_CHARSET_H_
