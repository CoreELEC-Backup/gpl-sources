/*
 * setupeepg.c
 *
 *  Created on: 08.5.2012
 *      Author: d.petrovski
 */

#include <stddef.h>
#include "setupeepg.h"

// --- cSetupEEPG -------------------------------------------------------

cSetupEEPG* cSetupEEPG::_setupEEPG = NULL;

cSetupEEPG::cSetupEEPG (void)
:ConfDir(NULL)
{
  ConfDir = NULL;
  OptPat = 1;
  OrderInfo = 1;
  RatingInfo = 1;
  FixEpg = 0;
  DisplayMessage = 1;
  ProcessEIT = 0;
  ReplaceEmptyShText = 0;
  FixCharset = 0;

#ifdef DEBUG
  LogLevel = 0;
#endif
}

cSetupEEPG* cSetupEEPG::getInstance()
{
  if (!_setupEEPG)
    _setupEEPG = new cSetupEEPG();

  return _setupEEPG;
}

