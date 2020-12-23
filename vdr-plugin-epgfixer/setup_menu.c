/*
 * setup_menu.c: Setup Menu
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <stdio.h>
#include <vdr/config.h>
#include <vdr/eit.h>
#include <vdr/i18n.h>
#include <vdr/menu.h>
#include <vdr/skins.h>
#include "blacklist.h"
#include "charset.h"
#include "epgclone.h"
#include "regexp.h"
#include "tools.h"
#include "setup_menu.h"

//--- cMenuSetupConfigEditor ------------------------------------------------------

#define MAXREGEXPLENGTH 512

const char *RegexpChars =
  trNOOP("RegexpChars$ abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789%~\\/?!()[]{}<>$^*.,:;-=#");

template<class LISTITEM, class PARAMETER> class cMenuSetupConfigEditor : public cMenuSetupPage
{
private:
  cEpgfixerList<LISTITEM, PARAMETER> *list;
  const char *fileName;
  char **lines;
  char **numlines;
  virtual void LoadListToArray(void)
  {
    lines = (char **)malloc(sizeof(char *)*(list->Count()));
    int i = 0;
    LISTITEM *item = (LISTITEM *)list->First();
    while (item) {
          lines[i] = (char *)malloc(sizeof(char)*MAXREGEXPLENGTH);
          snprintf(lines[i], MAXREGEXPLENGTH, "%s", item->GetString());
          item = (LISTITEM *)item->Next();
          ++i;
          }
  }
  void FreeArray()
  {
    int i = 0;
    while (i < list->Count()) {
          free(lines[i]);
          ++i;
          }
    FREE(lines);
  }
  void Save()
  {
    // Store regular expressions to config file
    if (fileName && access(fileName, F_OK) == 0) {
       FILE *f = fopen(fileName, "w");
       if (f) {
          LISTITEM *item = (LISTITEM *)list->First();
          while (item) {
                item->PrintConfigLineToFile(f);
                item = (LISTITEM *)item->Next();
                }
          fclose(f);
          }
       }
  }
protected:
  virtual void Store(void)
  {
    // Store regular expressions back to list
    int i = 0;
    LISTITEM *item = (LISTITEM *)list->First();
    while (i < list->Count()) {
          item->SetFromString(lines[i], item->IsEnabled());
          item = (LISTITEM *)item->Next();
          ++i;
          }
  }
  void Set(void)
  {
    int current = Current();

    Clear();
    int i = 0;
    LISTITEM *item = (LISTITEM *)list->First();
    while (i < list->Count()) {
          Add(new cMenuEditStrItem(item->IsEnabled() ? "+" : "-", lines[i], MAXREGEXPLENGTH, tr(RegexpChars)));
          item = (LISTITEM *)item->Next();
          ++i;
          }
    if (list->Count() > 0)
        SetHelp(trVDR("Button$On/Off"), trVDR("Button$New"), trVDR("Button$Delete"), tr("Button$Cancel"));
    else
        SetHelp(NULL,trVDR("Button$New"), NULL, tr("Button$Cancel"));

    SetCurrent(Get(current));
    Display();
  }
public:
  cMenuSetupConfigEditor(cEpgfixerList<LISTITEM, PARAMETER> *l)
  {
    list = l;
    cEitFilter::SetDisableUntil(time(NULL) + 1000);
    SetCols(2);
    fileName = list->GetConfigFile();
    LoadListToArray();
    Set();
  }
  ~cMenuSetupConfigEditor(void)
  {
    FreeArray();
    cEitFilter::SetDisableUntil(time(NULL) + 5);
  }
  virtual eOSState ProcessKey(eKeys Key)
  {
    eOSState state = cOsdMenu::ProcessKey(Key);

    if (state == osUnknown) {
       switch (Key) {
         case kRed:
           if (list->Count() > 0) {
              list->Get(Current())->ToggleEnabled();
              Set();
              Display();
              }
           state = osContinue;
           break;
         case kGreen:
           Store();
           FreeArray();
           list->Add(new LISTITEM());
           LoadListToArray();
           Set();
           Display();
           state = osContinue;
           break;
         case kYellow:
           if (list->Count() > 0) {
              Store();
              FreeArray();
              list->Del(list->Get(Current()),true);
              LoadListToArray();
              Set();
              Display();
              }
           state = osContinue;
           break;
         case kBlue:
           list->ReloadConfigFile();
           state = osBack;
           break;
         case kOk:
           Store();
           Save();
           list->ReloadConfigFile();
           state = osBack;
           break;
         default:
           break;
         }
       }
    return state;
  }
};

//--- cMenuSetupEpgfixer ------------------------------------------------------

cMenuSetupEpgfixer::cMenuSetupEpgfixer(void)
{
  newconfig = EpgfixerSetup;
  Set();
}

void cMenuSetupEpgfixer::Set(void)
{
  int current = Current();

  Clear();
  help.Clear();
  Add(new cOsdItem(tr("Regular expressions"), osUser1));
  help.Append(tr("Edit regular expressions."));
  Add(new cOsdItem(tr("Character set conversions"), osUser2));
  help.Append(tr("Edit character set conversions."));
  Add(new cOsdItem(tr("EPG blacklists"), osUser3));
  help.Append(tr("Edit EPG blacklists."));
  Add(new cOsdItem(tr("EPG cloning"), osUser4));
  help.Append(tr("Edit EPG data cloning."));

  Add(new cOsdItem(tr("--- EPG bugfixes ---"), osUnknown, false));
  help.Append("");

  Add(new cMenuEditBoolItem(tr("Remove quotes from ShortText"),
                            &newconfig.quotedshorttext));
  help.Append(tr("EPG bugfix level >= 1: Some channels put the ShortText in quotes and use either the ShortText or the Description field, depending on how long the string is:\n\nTitle\n\"ShortText\". Description"));
  Add(new cMenuEditBoolItem(tr("Move Description from ShortText"),
                            &newconfig.blankbeforedescription));
  help.Append(tr("EPG bugfix level >= 1: Some channels put the Description into the ShortText (preceded by a blank) if there is no actual ShortText and the Description is short enough:\n\nTitle\n Description"));
  Add(new cMenuEditBoolItem(tr("Remove repeated title from ShortText"),
                            &newconfig.repeatedtitle));
  help.Append(tr("EPG bugfix level >= 1: Sometimes they repeat the Title in the ShortText:\n\nTitle\nTitle"));
  Add(new cMenuEditBoolItem(tr("Remove double quotes from ShortText"),
                            &newconfig.doublequotedshorttext));
  help.Append(tr("EPG bugfix level >= 1: Some channels put the ShortText between double quotes, which is nothing but annoying (some even put a '.' after the closing '\"'):\n\nTitle\n\"ShortText\"[.]"));
  Add(new cMenuEditBoolItem(tr("Remove useless formatting"),
                            &newconfig.removeformatting));
  help.Append(tr("EPG bugfix level >= 2: Some channels apparently try to do some formatting in the texts, which is a bad idea because they have no way of knowing the width of the window that will actually display the text. Remove excess whitespace."));
  Add(new cMenuEditBoolItem(tr("Move long ShortText to Description"),
                            &newconfig.longshorttext));
  help.Append(tr("EPG bugfix level >= 2: Some channels put a whole lot of information in the ShortText and leave the Description totally empty. So if the ShortText length exceeds 40, let's put this into the Description instead."));
  Add(new cMenuEditBoolItem(tr("Prevent equal ShortText and Description"),
                            &newconfig.equalshorttextanddescription));
  help.Append(tr("EPG bugfix level >= 2: Some channels put the same information into ShortText and Description. In that case we delete one of them."));
  Add(new cMenuEditBoolItem(tr("Replace backticks with single quotes"),
                            &newconfig.nobackticks));
  help.Append(tr("EPG bugfix level >= 2: Some channels use the ` (\"backtick\") character, where a ' (single quote) would be normally used. Actually, \"backticks\" in normal text don't make much sense, so let's replace them."));
  Add(new cMenuEditBoolItem(tr("Fix stream component descriptions"),
                            &newconfig.components));
  help.Append(tr("EPG bugfix level = 3: The stream components have a \"description\" field which some channels apparently have no idea of how to set correctly."));
  Add(new cMenuEditBoolItem(tr("Strip HTML entities"),
                            &newconfig.striphtml));
  help.Append(tr("Convert HTML entities from all fields to matching regular characters."));
  SetHelp(tr("Button$Load"),NULL,NULL, tr("Button$Clear EPG"));

  SetCurrent(Get(current));
  Display();
}

void cMenuSetupEpgfixer::Store(void)
{
  EpgfixerSetup = newconfig;

  SetupStore("RemoveQuotesFromShortText",           EpgfixerSetup.quotedshorttext);
  SetupStore("MoveDescriptionFromShortText",        EpgfixerSetup.blankbeforedescription);
  SetupStore("RemoveRepeatedTitleFromShortText",    EpgfixerSetup.repeatedtitle);
  SetupStore("RemoveDoubleQuotesFromShortText",     EpgfixerSetup.doublequotedshorttext);
  SetupStore("RemoveUselessFormatting",             EpgfixerSetup.removeformatting);
  SetupStore("MoveLongShortTextToDescription",      EpgfixerSetup.longshorttext);
  SetupStore("PreventEqualShortTextAndDescription", EpgfixerSetup.equalshorttextanddescription);
  SetupStore("ReplaceBackticksWithSingleQuotes",    EpgfixerSetup.nobackticks);
  SetupStore("FixStreamComponentDescriptions",      EpgfixerSetup.components);
  SetupStore("StripHTMLentities",                   EpgfixerSetup.striphtml);

  Setup.Save();
}

eOSState cMenuSetupEpgfixer::ProcessKey(eKeys Key)
{
  eOSState state = cMenuSetupPage::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kRed:
         Skins.Message(mtInfo, tr("Loading configuration files..."));
         EpgfixerRegexps.ReloadConfigFile();
         EpgfixerCharSets.ReloadConfigFile();
         EpgfixerBlacklists.ReloadConfigFile();
         EpgfixerEpgClones.ReloadConfigFile();
         Skins.Message(mtInfo, NULL);
         state = osContinue;
         break;
       case kBlue:
         Skins.Message(mtInfo, tr("Clearing EPG data..."));
#if VDRVERSNUM >= 20301
         {
         LOCK_TIMERS_WRITE;
         LOCK_SCHEDULES_WRITE;
         for (cTimer *Timer = Timers->First(); Timer; Timer = Timers->Next(Timer))
            Timer->SetEvent(NULL);
         for (cSchedule *Schedule = Schedules->First(); Schedule; Schedule = Schedules->Next(Schedule))
            Schedule->Cleanup(INT_MAX);
         }
         cEitFilter::SetDisableUntil(time(NULL) + 10);
#else
         cEitFilter::SetDisableUntil(time(NULL) + 10);
         if (cSchedules::ClearAll())
            cEitFilter::SetDisableUntil(time(NULL) + 10);
#endif
         Skins.Message(mtInfo, NULL);
         state = osContinue;
         break;
       case kInfo:
         if (Current() < help.Size())
            return AddSubMenu(new cMenuText(cString::sprintf("%s - %s '%s'", tr("Help"), trVDR("Plugin"), PLUGIN_NAME_I18N), help[Current()]));
         break;
       default:
         break;
       }
     }
  else if (state == osUser1)
     return AddSubMenu(new cMenuSetupConfigEditor<cRegexp, cEvent>(&EpgfixerRegexps));
  else if (state == osUser2)
     return AddSubMenu(new cMenuSetupConfigEditor<cCharSet, cEvent>(&EpgfixerCharSets));
  else if (state == osUser3)
     return AddSubMenu(new cMenuSetupConfigEditor<cBlacklist, cChannel>(&EpgfixerBlacklists));
  else if (state == osUser4)
     return AddSubMenu(new cMenuSetupConfigEditor<cEpgClone, cEvent>(&EpgfixerEpgClones));
  return state;
}
