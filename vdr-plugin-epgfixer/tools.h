/*
 * tools.h: Tools for handling configure files and strings
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __EPGFIXER_TOOLS_H_
#define __EPGFIXER_TOOLS_H_

#include <stdio.h>
#include <unistd.h>
#include <vdr/epg.h>
#include <vdr/tools.h>

#ifdef DEBUG
#define debug(x...) dsyslog("EPGFixer: " x);
#else
#define debug(x...) ;
#endif
#define error(x...) esyslog("ERROR: " x);

#define FREE(x) { free(x); x = NULL; }

// --- EPG bug fixes ----------------------------------------------------

void FixOriginalEpgBugs(cEvent *event);
bool FixCharSets(cEvent *Event);
bool FixBugs(cEvent *Event);
void StripHTML(cEvent *Event);

// --- Add event to schedule --------------------------------------------

void AddEvent(cEvent *event, tChannelID ChannelID);

char *striphtml(char *str);

class cListItem : public cListObject
{
protected:
  bool enabled;
  char *string;
  int *channels_num;
  tChannelID *channels_id;
  int numchannels;
  void Free();
  tChannelID *GetChannelID(int index);
  int GetChannelNum(int index);
  int LoadChannelsFromString(const char *string);
  bool IsActive(tChannelID ChannelID);

public:
  cListItem();
  virtual ~cListItem();
  virtual bool Apply(cChannel *Channel) { return 0; }
  virtual bool Apply(cEvent *Event) { return 0; }
  void SetFromString(char *string, bool Enabled);
  const char *GetString() { return string; }
  bool IsEnabled(void) { return enabled; }
  void ToggleEnabled(void);
  void PrintConfigLineToFile(FILE *f);
};

// --- cEpgfixerList ----------------------------------------------------

template<class LISTITEM, class PARAMETER> class cEpgfixerList : public cList<LISTITEM>
{
protected:
  char *fileName;
  bool LoadConfigFile(bool AllowComments = true);

public:
  cEpgfixerList() { fileName = NULL; }
  ~cEpgfixerList() { free(fileName); }
  void Clear(void) { cList<LISTITEM>::Clear(); }
  bool ReloadConfigFile(bool AllowComments = true);
  bool Apply(PARAMETER *Parameter);
  void SetConfigFile(const char *FileName) { fileName = strdup(FileName); }
  const char *GetConfigFile() { return fileName; }
};

template<class LISTITEM, class PARAMETER> bool cEpgfixerList<LISTITEM, PARAMETER>::LoadConfigFile(bool AllowComments)
{
  bool result = false;
  if (fileName && access(fileName, F_OK) == 0) {
     FILE *f = fopen(fileName, "r");
     if (f) {
        char *s;
        int line = 0;
        int count = 0;
        cReadLine ReadLine;
        cString logmsg("");
        logmsg = cString::sprintf("%s%s loaded. Active lines:", *logmsg, fileName);
        while ((s = ReadLine.Read(f)) != NULL) {
              ++line;
              if (!isempty(s)) {
                 this->Add(new LISTITEM());
                 cList<LISTITEM>::Last()->LISTITEM::SetFromString(s, true);
                 if (cList<LISTITEM>::Last()->IsEnabled()) {
                    ++count;
                    logmsg = cString::sprintf("%s%s%i", *logmsg, count == 1 ? " " : ",", line);
                    }
                 }
              }
        fclose(f);
        if (count == 0)
          logmsg = cString::sprintf("%s none", *logmsg);
        isyslog("%s", *logmsg);
        result = true;
        }
     else {
        LOG_ERROR_STR(fileName);
        result = false;
        }
     }
  return result;
}

template<class LISTITEM, class PARAMETER> bool cEpgfixerList<LISTITEM, PARAMETER>::ReloadConfigFile(bool AllowComments)
{
  Clear();
  return LoadConfigFile(AllowComments);
}

template<class LISTITEM, class PARAMETER> bool cEpgfixerList<LISTITEM, PARAMETER>::Apply(PARAMETER *Parameter)
{
  int res = false;
  LISTITEM *item = (LISTITEM *)(cList<LISTITEM>::First());
  while (item) {
        if (item->IsEnabled()) {
           int ret = item->LISTITEM::Apply(Parameter);
           if (ret && !res)
              res = true;
           }
        item = (LISTITEM *)(item->Next());
        }
  return res;
}

#endif //__EPGFIXER_TOOLS_H_
