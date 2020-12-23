/*
 * parse.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef _PARSE_H
#define _PARSE_H

#include <vdr/epg.h>
#include <libxml/parser.h>
#include <time.h>

#include "maps.h"
#include "event.h"

class cEPGExecutor;
class cEPGSource;
class cEPGMappings;
class cGlobals;

class cParse
{
    enum
    {
        PARSE_NOERROR=0,
        PARSE_XMLTVERR,
        PARSE_NOMAPPING,
        PARSE_NOCHANNELID,
        PARSE_FETCHERR,
        PARSE_SQLERR,
        PARSE_NOEVENTID
    };

private:
    cGlobals *g;  
    iconv_t cep2ascii;
    iconv_t cutf2ascii;
    cEPGSource *source;
    cXMLTVEvent xevent;
    time_t ConvertXMLTVTime2UnixTime(char *xmltvtime);
    bool FetchEvent(xmlNodePtr node, bool useeptext);
public:
    cParse(cEPGSource *Source, cGlobals *Global);
    ~cParse();
    int Process(cEPGExecutor &myExecutor, char *buffer, int bufsize);
    static void RemoveNonAlphaNumeric(char *String, bool InDescription=false);
    static bool FetchSeasonEpisode(iconv_t cEP2ASCII, iconv_t cUTF2ASCII, const char *EPDir,
                                   const char *Title, const char *ShortText, const char *Description,
                                   int &Season, int &Episode, int &EpisodeOverall, char **EPShortText,
                                   char **EPTitle);
    static void InitLibXML();
    static void CleanupLibXML();
};

#endif
