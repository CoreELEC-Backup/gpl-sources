/*
 * setupeepg.h
 *
 *  Created on: 08.5.2012
 *      Author: d.petrovski
 */

#ifndef SETUPEEPG_H_
#define SETUPEEPG_H_
#include <string.h>

class cSetupEEPG
{
public:
  int OptPat;
  int OrderInfo;
  int RatingInfo;
  int FixEpg;
  int DisplayMessage;
  int ProcessEIT;
  int ReplaceEmptyShText;
  int FixCharset;
#ifdef DEBUG
  int LogLevel;
#endif

public:
  static cSetupEEPG* getInstance();

  char*  getConfDir() const
  {
    return ConfDir;
  }

  void  setConfDir(char* confDir)
  {
    if (ConfDir)
      delete [] ConfDir;
    ConfDir = new char[strlen(confDir)+1];
    strcpy(ConfDir, confDir);
  }

private:
  cSetupEEPG (void);
  cSetupEEPG(cSetupEEPG const&);             // copy constructor is private
  cSetupEEPG& operator=(cSetupEEPG const&);  // assignment operator is private
  static cSetupEEPG* _setupEEPG;

private:
  char *ConfDir;

};

#endif /* SETUPEEPG_H_ */
