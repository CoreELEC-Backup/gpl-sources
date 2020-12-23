/*
 * config.h: Global configuration and user settings
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __EPGFIXER_CONFIG_H_
#define __EPGFIXER_CONFIG_H_

struct cEpgfixerSetup
{
  int quotedshorttext;
  int blankbeforedescription;
  int repeatedtitle;
  int doublequotedshorttext;
  int removeformatting;
  int longshorttext;
  int equalshorttextanddescription;
  int nobackticks;
  int components;
  int striphtml;
  cEpgfixerSetup();
};

// Global instance
extern cEpgfixerSetup EpgfixerSetup;

#endif //__EPGFIXER_CONFIG_H_
