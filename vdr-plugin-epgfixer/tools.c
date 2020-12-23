/*
 * tools.c: Tools for handling configuration files and strings
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <vdr/thread.h>
#include "charset.h"
#include "config.h"
#include "regexp.h"
#include "tools.h"

//
// Original VDR bug fixes adapted from epg.c of VDR
// by Klaus Schmidinger
//

static void StripControlCharacters(char *s)
{
  if (s) {
     int len = strlen(s);
     while (len > 0) {
           int l = Utf8CharLen(s);
           uchar *p = (uchar *)s;
           if (l == 2 && *p == 0xC2) // UTF-8 sequence
              p++;
           if (*p == 0x86 || *p == 0x87) {
              memmove(s, p + 1, len - l + 1); // we also copy the terminating 0!
              len -= l;
              l = 0;
              }
           s += l;
           len -= l;
           }
     }
}

void FixOriginalEpgBugs(cEvent *event)
{
  // Copy event title, shorttext and description to temporary variables
  // we don't want any "(null)" titles
  char *title = event->Title() ? strdup(event->Title()) : strdup("No title");
  char *shortText = event->ShortText() ? strdup(event->ShortText()) : NULL;
  char *description = event->Description() ? strdup(event->Description()) : NULL;

  // Some TV stations apparently have their own idea about how to fill in the
  // EPG data. Let's fix their bugs as good as we can:

  // Some channels put the ShortText in quotes and use either the ShortText
  // or the Description field, depending on how long the string is:
  //
  // Title
  // "ShortText". Description
  //
  if (EpgfixerSetup.quotedshorttext && (shortText == NULL) != (description == NULL)) {
     char *p = shortText ? shortText : description;
     if (*p == '"') {
        const char *delim = "\".";
        char *e = strstr(p + 1, delim);
        if (e) {
           *e = 0;
           char *s = strdup(p + 1);
           char *d = strdup(e + strlen(delim));
           free(shortText);
           free(description);
           shortText = s;
           description = d;
           }
        }
     }

  // Some channels put the Description into the ShortText (preceded
  // by a blank) if there is no actual ShortText and the Description
  // is short enough:
  //
  // Title
  //  Description
  //
  if (EpgfixerSetup.blankbeforedescription && shortText && !description) {
     if (*shortText == ' ') {
        memmove(shortText, shortText + 1, strlen(shortText));
        description = shortText;
        shortText = NULL;
        }
     }

  // Sometimes they repeat the Title in the ShortText:
  //
  // Title
  // Title
  //
  if (EpgfixerSetup.repeatedtitle && shortText && strcmp(title, shortText) == 0) {
     free(shortText);
     shortText = NULL;
     }

  // Some channels put the ShortText between double quotes, which is nothing
  // but annoying (some even put a '.' after the closing '"'):
  //
  // Title
  // "ShortText"[.]
  //
  if (EpgfixerSetup.doublequotedshorttext && shortText && *shortText == '"') {
     int l = strlen(shortText);
     if (l > 2 && (shortText[l - 1] == '"' || (shortText[l - 1] == '.' && shortText[l - 2] == '"'))) {
        memmove(shortText, shortText + 1, l);
        char *p = strrchr(shortText, '"');
        if (p)
           *p = 0;
        }
     }

  // Some channels apparently try to do some formatting in the texts,
  // which is a bad idea because they have no way of knowing the width
  // of the window that will actually display the text.
  // Remove excess whitespace:
  if (EpgfixerSetup.removeformatting) {
     title = compactspace(title);
     shortText = compactspace(shortText);
     description = compactspace(description);
     }

#define MAX_USEFUL_EPISODE_LENGTH 40
  // Some channels put a whole lot of information in the ShortText and leave
  // the Description totally empty. So if the ShortText length exceeds
  // MAX_USEFUL_EPISODE_LENGTH, let's put this into the Description
  // instead:
  if (EpgfixerSetup.longshorttext && !isempty(shortText) && isempty(description)) {
     if (strlen(shortText) > MAX_USEFUL_EPISODE_LENGTH) {
        free(description);
        description = shortText;
        shortText = NULL;
        }
     }

  // Some channels put the same information into ShortText and Description.
  // In that case we delete one of them:
  if (EpgfixerSetup.equalshorttextanddescription && shortText && description && strcmp(shortText, description) == 0) {
     if (strlen(shortText) > MAX_USEFUL_EPISODE_LENGTH) {
        free(shortText);
        shortText = NULL;
        }
     else {
        free(description);
        description = NULL;
        }
     }

  // Some channels use the ` ("backtick") character, where a ' (single quote)
  // would be normally used. Actually, "backticks" in normal text don't make
  // much sense, so let's replace them:
  if (EpgfixerSetup.nobackticks) {
     strreplace(title, '`', '\'');
     strreplace(shortText, '`', '\'');
     strreplace(description, '`', '\'');
     }

  // The stream components have a "description" field which some channels
  // apparently have no idea of how to set correctly:
  const cComponents *components = event->Components();
  if (EpgfixerSetup.components && components) {
     for (int i = 0; i < components->NumComponents(); ++i) {
         tComponent *p = components->Component(i);
         switch (p->stream) {
           case 0x01: { // video
                if (p->description) {
                   if (strcasecmp(p->description, "Video") == 0 ||
                        strcasecmp(p->description, "Bildformat") == 0) {
                      // Yes, we know it's video - that's what the 'stream' code
                      // is for! But _which_ video is it?
                      free(p->description);
                      p->description = NULL;
                      }
                   }
                if (!p->description) {
                   switch (p->type) {
                     case 0x01:
                     case 0x05: p->description = strdup("4:3"); break;
                     case 0x02:
                     case 0x03:
                     case 0x06:
                     case 0x07: p->description = strdup("16:9"); break;
                     case 0x04:
                     case 0x08: p->description = strdup(">16:9"); break;
                     case 0x09:
                     case 0x0D: p->description = strdup("HD 4:3"); break;
                     case 0x0A:
                     case 0x0B:
                     case 0x0E:
                     case 0x0F: p->description = strdup("HD 16:9"); break;
                     case 0x0C:
                     case 0x10: p->description = strdup("HD >16:9"); break;
                     default: ;
                     }
                   }
                }
                break;
           case 0x02: { // audio
                if (p->description) {
                   if (strcasecmp(p->description, "Audio") == 0) {
                      // Yes, we know it's audio - that's what the 'stream' code
                      // is for! But _which_ audio is it?
                      free(p->description);
                      p->description = NULL;
                      }
                   }
                if (!p->description) {
                   switch (p->type) {
                     case 0x05: p->description = strdup("Dolby Digital"); break;
                     default: ; // all others will just display the language
                     }
                   }
                }
                break;
           default: ;
           }
         }
     }

  // VDR can't usefully handle newline characters in the title, shortText or component description of EPG
  // data, so let's always convert them to blanks (independent of the setting of EPGBugfixLevel):
  strreplace(title, '\n', ' ');
  strreplace(shortText, '\n', ' ');
  if (components) {
     for (int i = 0; i < components->NumComponents(); ++i) {
         tComponent *p = components->Component(i);
         if (p->description)
            strreplace(p->description, '\n', ' ');
         }
     }
  // Same for control characters:
  StripControlCharacters(title);
  StripControlCharacters(shortText);
  StripControlCharacters(description);
  // Set modified data back to event
  event->SetTitle(title);
  event->SetShortText(shortText);
  event->SetDescription(description);

  free(title);
  free(shortText);
  free(description);
}

bool FixBugs(cEvent *Event)
{
  return EpgfixerRegexps.Apply(Event);
}

bool FixCharSets(cEvent *Event)
{
  return EpgfixerCharSets.Apply(Event);
}

void StripHTML(cEvent *Event)
{
  if (EpgfixerSetup.striphtml) {
     char *tmpstring = NULL;
     tmpstring = Event->Title() ? strdup(Event->Title()) : NULL;
     Event->SetTitle(striphtml(tmpstring));
     FREE(tmpstring);
     tmpstring = Event->ShortText() ? strdup(Event->ShortText()) : NULL;
     Event->SetShortText(striphtml(tmpstring));
     FREE(tmpstring);
     tmpstring = Event->Description() ? strdup(Event->Description()) : NULL;
     Event->SetDescription(striphtml(tmpstring));
     FREE(tmpstring);
     }
}

//
// HTML conversion code taken from RSS Reader plugin for VDR
// http://www.saunalahti.fi/~rahrenbe/vdr/rssreader/
// by Rolf Ahrenberg
//

// --- Static -----------------------------------------------------------

#define ELEMENTS(x) (sizeof(x) / sizeof(x[0]))

struct conv_table {
  const char *from;
  const char *to;
};

static struct conv_table pre_conv_table[] =
{
  // 'to' field must be smaller than 'from'
  {"<br />",   "\n"}
};

// Conversion page: http://www.ltg.ed.ac.uk/~richard/utf-8.cgi

static struct conv_table post_conv_table[] =
{
  // 'to' field must be smaller than 'from'
  {"&quot;",   "\x22"},
  {"&#34;",    "\x22"},
  {"&amp;",    "\x26"},
  {"&#38;",    "\x26"},
  {"&#038;",   "\x26"},
  {"&#039;",   "\x27"},
  {"&#40;",    "\x28"},
  {"&#41;",    "\x29"},
  {"&#58;",    "\x3a"},
  {"&lt;",     "\x3c"},
  {"&#60;",    "\x3c"},
  {"&gt;",     "\x3e"},
  {"&#62;",    "\x3e"},
  {"&#91;",    "\x5b"},
  {"&#93;",    "\x5d"},
  {"&nbsp;",   "\xc2\xa0"},
  {"&#160;",   "\xc2\xa0"},
  {"&deg;",    "\xc2\xb0"},
  {"&#176;",   "\xc2\xb0"},
  {"&acute;",  "\xc2\xb4"},
  {"&#180;",   "\xc2\xb4"},
  {"&Auml;",   "\xc3\x84"},
  {"&#196;",   "\xc3\x84"},
  {"&Aring;",  "\xc3\x85"},
  {"&#197;",   "\xc3\x85"},
  {"&Ouml;",   "\xc3\x96"},
  {"&#214;",   "\xc3\x96"},
  {"&Uuml;",   "\xc3\x9c"},
  {"&#220;",   "\xc3\x9c"},
  {"&szlig;",  "\xc3\x9f"},
  {"&#223;",   "\xc3\x9f"},
  {"&acirc;",  "\xc3\xa2"},
  {"&#226;",   "\xc3\xa2"},
  {"&auml;",   "\xc3\xa4"},
  {"&#228;",   "\xc3\xa4"},
  {"&aring;",  "\xc3\xa5"},
  {"&#229;",   "\xc3\xa5"},
  {"&ccedil;", "\xc3\xa7"},
  {"&#231;",   "\xc3\xa7"},
  {"&eacute;", "\xc3\xa9"},
  {"&#233;",   "\xc3\xa9"},
  {"&ecirc;",  "\xc3\xaa"},
  {"&#234;",   "\xc3\xaa"},
  {"&ouml;",   "\xc3\xb6"},
  {"&#246;",   "\xc3\xb6"},
  {"&uuml;",   "\xc3\xbc"},
  {"&#252;",   "\xc3\xbc"},
  {"&ndash;",  "\xe2\x80\x93"},
  {"&#8211;",  "\xe2\x80\x93"},
  {"&mdash;",  "\xe2\x80\x94"},
  {"&#8212;",  "\xe2\x80\x94"},
  {"&lsquo;",  "\xe2\x80\x98"},
  {"&#8216;",  "\xe2\x80\x98"},
  {"&rsquo;",  "\xe2\x80\x99"},
  {"&#8217;",  "\xe2\x80\x99"},
  {"&sbquo;",  "\xe2\x80\x9a"},
  {"&#8218;",  "\xe2\x80\x9a"},
  {"&ldquo;",  "\xe2\x80\x9c"},
  {"&#8220;",  "\xe2\x80\x9c"},
  {"&rdquo;",  "\xe2\x80\x9d"},
  {"&#8221;",  "\xe2\x80\x9d"},
  {"&bdquo;",  "\xe2\x80\x9e"},
  {"&#8222;",  "\xe2\x80\x9e"},
  {"&prime;",  "\xe2\x80\xb3"},
  {"&#8243;",  "\xe2\x80\xb3"},
  {"&euro;",   "\xe2\x82\xac"},
  {"&#8364;",  "\xe2\x82\xac"},
  {"\n\n",     "\n"}, // let's also strip multiple linefeeds
};

static char *htmlcharconv(char *str, struct conv_table *conv, unsigned int elem)
{
  if (str && conv) {
     for (unsigned int i = 0; i < elem; ++i) {
         char *ptr = strstr(str, conv[i].from);
         while (ptr) {
               long of = ptr - str;
               size_t l  = strlen(str);
               size_t l1 = strlen(conv[i].from);
               size_t l2 = strlen(conv[i].to);
               if (l2 > l1) {
                  error("htmlcharconv(): cannot reallocate string");
                  return str;
                  }
               if (l2 != l1)
                  memmove(str + of + l2, str + of + l1, l - of - l1 + 1);
               strncpy(str + of, conv[i].to, l2);
               ptr = strstr(str, conv[i].from);
               }
         }
     return str;
     }
  return NULL;
}

// --- General functions ------------------------------------------------

char *striphtml(char *str)
{
  if (str) {
     char *c, t = 0, *r;
     str = htmlcharconv(str, pre_conv_table, ELEMENTS(pre_conv_table));
     c = str;
     r = str;
     while (*str != '\0') {
           if (*str == '<')
              t++;
           else if (*str == '>')
              t--;
           else if (t < 1)
              *(c++) = *str;
           str++;
           }
     *c = '\0';
     return htmlcharconv(r, post_conv_table, ELEMENTS(post_conv_table));
     }
  return NULL;
}

// --- cAddEventThread --------------------------------------------------

class cAddEventListItem : public cListObject
{
protected:
  cEvent *event;
  tChannelID channelID;

public:
  cAddEventListItem(cEvent *Event, tChannelID ChannelID) { event = Event; channelID = ChannelID; }
  tChannelID GetChannelID() { return channelID; }
  cEvent *GetEvent() { return event; }
  ~cAddEventListItem() { }
};

class cAddEventThread : public cThread
{
private:
  cTimeMs LastHandleEvent;
  cList<cAddEventListItem> *list;
  enum { INSERT_TIMEOUT_IN_MS = 10000 };

protected:
  virtual void Action(void);

public:
  cAddEventThread(void);
  ~cAddEventThread(void);
  void AddEvent(cEvent *Event, tChannelID ChannelID);
};

cAddEventThread::cAddEventThread(void)
:cThread("cAddEventThread"), LastHandleEvent()
{
  list = new cList<cAddEventListItem>;
}

cAddEventThread::~cAddEventThread(void)
{
  LOCK_THREAD;
  list->cList::Clear();
  Cancel(3);
}

void cAddEventThread::Action(void)
{
  SetPriority(19);
  for (; Running() && !LastHandleEvent.TimedOut(); cCondWait::SleepMs(10)) {
        if (list->First() == NULL)
           continue;
        cAddEventListItem *e = NULL;
#if VDRVERSNUM >= 20301
        cStateKey StateKey;
        cSchedules *schedules = cSchedules::GetSchedulesWrite(StateKey, 10);
        LOCK_CHANNELS_READ;
#else
        cSchedulesLock SchedulesLock(true, 10);
        cSchedules *schedules = (cSchedules *)cSchedules::Schedules(SchedulesLock);
#endif
        Lock();
        while (schedules && (e = list->First()) != NULL) {
              tChannelID chanid = e->GetChannelID();
#if VDRVERSNUM >= 20301
              const cChannel *chan = Channels->GetByChannelID(chanid);
#else
              cChannel *chan = Channels.GetByChannelID(chanid);
#endif
              if (!chan) {
                 error("Destination channel %s not found for cloning!", *chanid.ToString());
                 }
              else {
                 cSchedule *schedule = (cSchedule *)schedules->GetSchedule(chan, true);
                 if (schedule) {
                    schedule->AddEvent(e->GetEvent());
                    EpgHandlers.SortSchedule(schedule);
                    EpgHandlers.DropOutdated(schedule, e->GetEvent()->StartTime(), e->GetEvent()->EndTime(), e->GetEvent()->TableID(), e->GetEvent()->Version());
                    }
                 }
              list->Del(e);
              }
        Unlock();
#if VDRVERSNUM >= 20301
        if (schedules)
           StateKey.Remove();
#endif
        }
}

void cAddEventThread::AddEvent(cEvent *Event, tChannelID ChannelID)
{
  LOCK_THREAD;
  list->Add(new cAddEventListItem(Event, ChannelID));
  LastHandleEvent.Set(INSERT_TIMEOUT_IN_MS);
}

static cAddEventThread AddEventThread;

// --- Add event to schedule --------------------------------------------

void AddEvent(cEvent *Event, tChannelID ChannelID)
{
  AddEventThread.AddEvent(Event, ChannelID);
  if (!AddEventThread.Active())
     AddEventThread.Start();
}

// --- Listitem ---------------------------------------------------------

cListItem::cListItem()
{
  enabled = false;
  string = NULL;
  numchannels = 0;
  channels_num = NULL;
  channels_id = NULL;
}

cListItem::~cListItem(void)
{
  Free();
}

void cListItem::Free(void)
{
  FREE(channels_num);
  FREE(channels_id);
  FREE(string);
  numchannels = 0;
  enabled = false;
}

tChannelID *cListItem::GetChannelID(int index)
{
  if (channels_id && index >= 0 && index < numchannels)
     return &channels_id[index];
  else
     return NULL;
}

int cListItem::GetChannelNum(int index)
{
  if (channels_num && index >= 0 && index < numchannels)
     return channels_num[index];
  else
     return 0;
}

bool cListItem::IsActive(tChannelID ChannelID)
{
  bool active = false;
  if (numchannels > 0) {
     int i = 0;
#if VDRVERSNUM >= 20301
     LOCK_CHANNELS_READ;
     int channel_number = Channels->GetByChannelID(ChannelID)->Number();
#else
     int channel_number = Channels.GetByChannelID(ChannelID)->Number();
#endif
     while (i < numchannels) {
           if ((channel_number == GetChannelNum(i)) ||
               (GetChannelID(i) && (ChannelID == *GetChannelID(i)))) {
              active = true;
              break;
              }
           ++i;
           }
     }
  else
     active = true;
  return active;
}

int cListItem::LoadChannelsFromString(const char *string)
{
  numchannels = 0;
  bool numbers = false;
  if (string != NULL) {
     if (atoi(string))
        numbers = true;
     char *tmpstring = strdup(string);
     char *c = strtok(tmpstring, ",");
     while (c) {
           ++numchannels;
           char *d = 0;
           if (numbers && (d = strchr(c, '-')))// only true if numbers are used
              numchannels = numchannels + atoi(d + 1) - atoi(c);
           c = strtok(NULL, ",");
           }
     free(tmpstring);
     }
  if (numchannels > 0) {
     char *tmpstring = strdup(string);
     // Use channel numbers
     if (numbers)
        channels_num = (int *)malloc(sizeof(int)*numchannels);
     else// use channel IDs
        channels_id = (tChannelID *)malloc(sizeof(tChannelID)*numchannels);
     int i = 0;
     char *c = strtok(tmpstring, ",");
     while (i < numchannels) {
           // Use channel numbers
           if (numbers) {
              channels_num[i] = atoi(c);
              if (char *d = strchr(c, '-')) {
                 int count = atoi(d + 1) - channels_num[i] + 1;
                 int j = 1;
                 while (j < count) {
                       channels_num[i + j] = channels_num[i] + j;
                       ++j;
                       }
                 i = i + count;
                 }
              }
           else // use channel IDs
              channels_id[i] = tChannelID::FromString(c);
           c = strtok(NULL, ",");
           ++i;
           }
     free(tmpstring);
     }
  return numchannels;
}

void cListItem::SetFromString(char *s, bool Enabled)
{
  enabled = Enabled;
  if (s[0] == '!')
     string = strdup(s + 1);
  else
     string = strdup(s);
  // disable comments and inactive lines
  if (s[0] == '!' || s[0] == '#')
     enabled = false;
}

void cListItem::ToggleEnabled(void)
{
  enabled = !enabled;
}

void cListItem::PrintConfigLineToFile(FILE *f)
{
  if (f)
     fprintf(f, "%s%s\n", (!enabled && string && *string != '#')  ? "!" : "", string);
}
