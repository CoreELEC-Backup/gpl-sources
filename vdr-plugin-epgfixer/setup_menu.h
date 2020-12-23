/*
 * setup_menu.h: Setup Menu
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __EPGFIXER_SETUP_MENU_H
#define __EPGFIXER_SETUP_MENU_H

#include <vdr/menuitems.h>
#include <vdr/tools.h>
#include "config.h"

class cMenuSetupEpgfixer : public cMenuSetupPage
{
private:
  cEpgfixerSetup newconfig;
  cVector<const char*> help;

protected:
  virtual void Store(void);
  void Set(void);

public:
  cMenuSetupEpgfixer(void);
  virtual eOSState ProcessKey(eKeys Key);
};

#endif //__EPGFIXER_SETUP_MENU_H
