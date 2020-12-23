/*
 * import.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef _IMPORT_H
#define _IMPORT_H

#include <vdr/epg.h>
#include <vdr/channels.h>
#include <sqlite3.h>

#include "event.h"
#include "source.h"
#include "maps.h"

class cEPGSource;
class cEPGExecutor;
class cGlobals;

class cImport
{
private:
    struct split
    {
        char *pointers[256];
        int count;
    };
    enum
    {
        IMPORT_NOERROR=0,
        IMPORT_NOSCHEDULE,
        IMPORT_NOCHANNEL,
        IMPORT_XMLTVERR,
        IMPORT_NOMAPPING,
        IMPORT_NOCHANNELID,
        IMPORT_EMPTYSCHEDULE
    };
    cGlobals *g;
    cCharSetConv *conv;
    iconv_t cep2ascii;
    iconv_t cutf2ascii;
    bool pendingtransaction;
    char *RemoveLastCharFromDescription(char *description);
    char *Add2Description(char *description, const char *value);
    char *Add2Description(char *description, const char *name, const char *value);
    char *Add2Description(char *description, const char *name, int value);
    char *Add2Description(char *description, cXMLTVEvent *xEvent, int Flags, int what);
    char *AddEOT2Description(char *description, bool checkutf8=false);
    struct split split(char *in, char delim);
    cEvent *GetEventBefore(cSchedule* schedule, time_t start);
    cEvent *SearchVDREvent(cEPGSource *source, cSchedule* schedule, cXMLTVEvent *event, bool append, int hint);
    cEvent *SearchVDREventByTitle(cEPGSource *source, cSchedule* schedule, const char *Title, time_t StartTime,
                                  int Duration, int hint);
    bool FetchXMLTVEvent(sqlite3_stmt *stmt, cXMLTVEvent *xevent);
    char *RemoveNonASCII(const char *src);
    cXMLTVEvent *PrepareAndReturn(sqlite3 **db, char *sql);
    int SoundEx(char *SoundEx,char *WordString,int LengthOption,int CensusOption);
public:
    cImport(cGlobals *Global);
    ~cImport();
    void LinkPictures(const char *Source, cXMLTVStringList *Pics, tEventID DestID,
                      tChannelID ChanID, bool MakeOld=true);
    int Process(cEPGSource *Source, cEPGExecutor &myExecutor);
    bool Begin(cEPGSource *Source, sqlite3 *Db);
    bool Commit(cEPGSource *Source, sqlite3 *Db);
    bool DBExists();
    bool PutEvent(cEPGSource *Source, sqlite3 *Db, cSchedule* Schedule, cEvent *Event,
                  cXMLTVEvent *xEvent, int Flags);
    bool UpdateXMLTVEvent(cEPGSource *Source, sqlite3 *Db, cXMLTVEvent *xEvent);
    bool UpdateXMLTVEvent(cEPGSource *Source, sqlite3 *Db, const cEvent *Event, cXMLTVEvent *xEvent,
                          const char *Description);
    cXMLTVEvent *SearchXMLTVEvent(sqlite3 **Db, const char *ChannelID, const cEvent *Event);
    cXMLTVEvent *AddXMLTVEvent(cEPGSource *Source, sqlite3 *Db, const char *ChannelID,
                               const cEvent *Event, const char *EITDescription, bool UseEPText);
    bool AddShortTextFromEITDescription(cXMLTVEvent *xEvent, const char *EITDescription);
    bool WasChanged(cEvent *Event);
};

#endif
