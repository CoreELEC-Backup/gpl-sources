/*
 * maps.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef _MAPS_H
#define _MAPS_H

#include <vdr/channels.h>
#include <vdr/tools.h>

// Flags field definition

// Bit  0-23  USE_ flags
// Bit 24-30  OPT_ flags
// Bit 31     always zero

#define USE_NOTHING            0

#define USE_SHORTTEXT          0x1
#define USE_LONGTEXT           0x2
#define USE_COUNTRYDATE        0x4
#define USE_ORIGTITLE          0x8
#define USE_CATEGORIES         0x10
#define USE_CREDITS            0x20
#define USE_RATING             0x40
#define USE_REVIEW             0x80
#define USE_VIDEO              0x100
#define USE_AUDIO              0x200
#define USE_SEASON             0x400
#define USE_STARRATING         0x800
#define USE_TITLE              0x1000
#define USE_CONTENT            0x2000

#define CREDITS_ACTORS         0x100000
#define CREDITS_DIRECTORS      0x200000
#define CREDITS_OTHERS         0x400000
#define CREDITS_LIST           0x800000

#define OPT_RATING_TEXT        0x1000000
#define OPT_SEASON_STEXTITLE   0x4000000
#define OPT_APPEND             0x40000000

#define EPLIST_USE_SEASON      0x1
#define EPLIST_USE_STEXTITLE   0x2

class cTEXTMapping : public cListObject
{
private:
    const char *name;
    const char *value;
public:
    cTEXTMapping(const char *Name, const char *Value);
    ~cTEXTMapping();
    void ChangeValue(const char *Value);
    const char *Name(void)
    {
        return name;
    }
    const char *Value(void)
    {
        return value;
    }
};

class cTEXTMappings : public cList<cTEXTMapping>
{
public:
    cTEXTMapping *GetMap(const char *Name);
    void Remove();
};

class cEPGMapping : public cListObject
{
private:
    static int compare(const void *a, const void *b);
    const char *channelname;
    tChannelID *channelids;
    int numchannelids;
    int flags;
    void addchannels(const char *channels);
public:
    cEPGMapping(const char *ChannelName, const char *Flags_and_Mappings);
    cEPGMapping(cEPGMapping&copy);
    ~cEPGMapping();
    void ChangeFlags(int NewFlags)
    {
        flags=NewFlags;
    }
    void ReplaceChannels(int NumChannelIDs, tChannelID *ChannelIDs);
    void AddChannel(int ChannelNumber);
    void RemoveChannel(int ChannelNumber, bool MarkOnly=false);
    void RemoveChannel(tChannelID ChannelID, bool MarkOnly=false);
    void RemoveInvalidChannels();
    int Flags()
    {
        return flags;
    }
    const char *ChannelName()
    {
        return channelname;
    }
    int NumChannelIDs()
    {
        return numchannelids;
    }
    tChannelID *ChannelIDs()
    {
        return channelids;
    }
};

class cEPGMappings : public cList<cEPGMapping>
{
public:
    cEPGMapping *GetMap(const char *ChannelName);
    cEPGMapping *GetMap(tChannelID ChannelID);
    bool ProcessChannel(tChannelID ChannelID);
    bool IgnoreChannel(const cChannel *Channel);
    void Remove();
};

#endif
