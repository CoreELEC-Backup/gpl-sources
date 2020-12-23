/*
 * blacklist.h: Blacklist list item
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __EPGFIXER_BLACKLIST_H_
#define __EPGFIXER_BLACKLIST_H_

#include <vdr/channels.h>
#include <vdr/tools.h>

#include "tools.h"

class cBlacklist : public cListItem
{
public:
  cBlacklist() {}
  virtual ~cBlacklist() {}
  using cListItem::Apply;
  virtual bool Apply(cChannel *Channel);
  void SetFromString(char *string, bool Enabled);
};

// Global instance
extern cEpgfixerList<cBlacklist, cChannel> EpgfixerBlacklists;

#endif //__EPGFIXER_BLACKLIST_H_
