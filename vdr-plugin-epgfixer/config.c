/*
 * config.c: Global configuration and user settings
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "config.h"

/* Global instance */
cEpgfixerSetup EpgfixerSetup;

cEpgfixerSetup::cEpgfixerSetup()
{
  quotedshorttext = 0;
  blankbeforedescription = 0;
  repeatedtitle = 0;
  doublequotedshorttext = 0;
  removeformatting = 0;
  longshorttext = 0;
  equalshorttextanddescription = 0;
  nobackticks = 0;
  components = 0;
  striphtml = 0;
}
