/*
 * equivhandler.h
 *
 *  Created on: 19.5.2012
 *      Author: d.petrovski
 */

#ifndef EQUIVHANDLER_H_
#define EQUIVHANDLER_H_

#include <vdr/epg.h>
#include <vdr/channels.h>
#include <map>
#include <string>

#define EEPG_FILE_EQUIV "eepg.equiv"

using namespace std;

class cEquivHandler
{
public:
  cEquivHandler();
  virtual ~cEquivHandler();

  void loadEquivalentChannelMap (void);
  void updateEquivalent(cSchedules * Schedules, tChannelID channelID, cEvent *pEvent);
  void updateEquivalent(tChannelID channelID, cEvent *pEvent);
  void sortEquivalents(tChannelID channelID, cSchedules* Schedules);
  void cloneEvent(cEvent *Source, cEvent *Dest);

  static multimap<string, string> getEquiChanMap() { return cEquivHandler::equiChanMap; };

private:
  static multimap<string, string> equiChanMap;
  static long equiChanFileTime;

};

#endif /* EQUIVHANDLER_H_ */
