/*
 * xmltv2vdr.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef _XMLTV2VDR_H
#define _XMLTV2VDR_H

#include <sqlite3.h>
#include <vdr/plugin.h>
#include "maps.h"
#include "parse.h"
#include "import.h"
#include "source.h"

#if __GNUC__ > 3
#define UNUSED(v) UNUSED_ ## v __attribute__((unused))
#else
#define UNUSED(x) x
#endif

static const char *VERSION        = "0.2.1";
static const char *DESCRIPTION    = trNOOP("Imports xmltv epg into vdr");

int ioprio_set(int which, int who, int ioprio);

// Data structure for service "Epgsearch-enablesearchtimers-v1.0"
struct Epgsearch_enablesearchtimers_v1_0
{
// in
    bool enable;           // enable search timer thread?
};

class cSVDRPMsg
{
private:
    bool readreply(int fd);
public:
    bool Send(const char *format, ...);
};

class cEPGSearch_Client
{
private:
    cPlugin *plugin;
public:
    cEPGSearch_Client();
    bool Installed()
    {
        return (plugin!=NULL);
    }
    bool EnableSearchTimer();
    bool DisableSearchTimer();
};

class cEPGTimer : public cThread
{
private:
    cEPGSources *sources;
    cEPGMappings *maps;
    cImport import;
    int epall;
public:
    cEPGTimer(cGlobals *Global);
    void Stop()
    {
        Cancel(3);
    }
    void SetEPAll(int Value)
    {
        epall=Value;
    }
    virtual void Action();
};

#if VDRVERSNUM < 10726 && !EPGHANDLER
class cEpgHandler : public cListObject
{
public:
    cEpgHandler(void) {}
    virtual ~cEpgHandler() {}
    virtual bool IgnoreChannel(const cChannel *UNUSED(Channel))
    {
        return false;
    }
    virtual bool SetShortText(cEvent *UNUSED(Event),const char *UNUSED(ShortText))
    {
        return false;
    }
    virtual bool SetDescription(cEvent *UNUSED(Event),const char *UNUSED(Description))
    {
        return false;
    }
    virtual bool HandleEvent(cEvent *UNUSED(Event))
    {
        return false;
    }
    virtual bool SortSchedule(cSchedule *UNUSED(Schedule))
    {
        return false;
    }
};
#endif

class cEPGSources;
class cImport;
class cPluginXmltv2vdr;

class cEPGHandler : public cEpgHandler
{
private:
    cEPGMappings *maps;
    cEPGSources *sources;
    cImport import;
    int epall;
    sqlite3 *db;
    time_t now;
    bool check4proc(cEvent *event, char **timerdescr, cEPGMapping **map);
public:
    cEPGHandler(cGlobals *Global);
    void SetEPAll(int Value)
    {
        epall=Value;
    }
    bool Active()
    {
        return (db!=NULL);
    }
    virtual bool IgnoreChannel(const cChannel *Channel);
    virtual bool SetShortText(cEvent *Event,const char *ShortText);
    virtual bool SetDescription(cEvent *Event,const char *Description);
    virtual bool HandleEvent(cEvent *Event);
    virtual bool SortSchedule(cSchedule *Schedule);
};

class cHouseKeeping : public cThread
{
private:
    cGlobals *global;
    void checkdir(const char *imgdir, int age, int &cnt, int &lcnt);
public:
    cHouseKeeping(cGlobals *Global);
    void Stop()
    {
        Cancel(3);
    }
    virtual void Action();
};

class cEPGSeasonEpisode : public cThread
{
private:
    const char *epgfile;
public:
    cEPGSeasonEpisode(cGlobals *Global);
    void Stop()
    {
        Cancel(3);
    }
    virtual void Action();
};

class cGlobals
{
private:
    char *confdir;
    char *epgfile;
    char *epgfile_store;
    char *epgfiledir;
    char *epdir;
    char *epcodeset;
    char *imgdir;
    char *codeset;
    char *order;
    char *srcorder;
    int epall;
    int imgdelafter;
    bool wakeup;
    bool soundex;
    cEPGMappings epgmappings;
    cTEXTMappings textmappings;
    cEPGSources epgsources;
    cEPGTimer *epgtimer;
    cEPGSeasonEpisode *epgseasonepisode;
public:
    cGlobals();
    ~cGlobals();
    cEPGHandler *epghandler;
    bool DBExists();
    char *GetDefaultOrder();
    void AllocateEPGTimerThread()
    {
        if (!epgtimer) epgtimer=new cEPGTimer(this);
    }
    void AllocateEPGSeasonThread()
    {
        if (!epgseasonepisode) epgseasonepisode=new cEPGSeasonEpisode(this);
    }
    cEPGSeasonEpisode *EPGSeasonEpisode()
    {
        return epgseasonepisode;
    }
    cEPGTimer *EPGTimer()
    {
        return epgtimer;
    }
    cEPGMappings *EPGMappings()
    {
        return &epgmappings;
    }
    cTEXTMappings *TEXTMappings()
    {
        return &textmappings;
    }
    cEPGSources *EPGSources()
    {
        return &epgsources;
    }
    void SetConfDir(const char *ConfDir)
    {
        free(confdir);
        confdir=strdup(ConfDir);
    }
    const char *ConfDir()
    {
        return confdir;
    }
    void CopyEPGFile(bool Init);
    bool CheckEPGDir(const char *EPGFileDir);
    void SetEPGFile(const char *EPGFile);
    const char *EPGFile()
    {
        return epgfile;
    }
    const char *EPGFileStore()
    {
        return epgfile_store;
    }
    void SetEPDir(const char *EPDir);
    const char *EPDir()
    {
        return epdir;
    }
    const char *EPCodeset()
    {
        return epcodeset;
    }
    const char *Codeset()
    {
        return codeset;
    }
    void SetImgDir(const char *ImgDir);
    const char *ImgDir()
    {
        return imgdir;
    }
    void SetImgDelAfter(int Value)
    {
        imgdelafter=Value;
    }
    int ImgDelAfter()
    {
        return imgdelafter;
    }
    void SetSrcOrder(const char *NewOrder)
    {
        free(srcorder);
        srcorder=strdup(NewOrder);
    }
    const char *SrcOrder()
    {
        return srcorder;
    }
    void SetOrder(const char *NewOrder)
    {
        free(order);
        order=strdup(NewOrder);
    }
    const char *Order()
    {
        return order;
    }
    void SetEPAll(int Value)
    {
        epall=Value;
        if (epghandler) epghandler->SetEPAll(Value);
        if (!epgtimer)
        {
            if (Value!=0)
            {
                AllocateEPGTimerThread();
            }
        }
        if (epgtimer) epgtimer->SetEPAll(Value);
    }
    int EPAll()
    {
        return epall;
    }
    void SetWakeUp(bool Value)
    {
        wakeup=Value;
    }
    bool WakeUp()
    {
        return wakeup;
    }
    void SetSoundEx()
    {
        soundex=true;
    }
    bool SoundEx()
    {
        return soundex;
    }
};

class cPluginXmltv2vdr : public cPlugin
{
private:
    cGlobals g;
    cHouseKeeping housekeeping;
    cEPGExecutor epgexecutor;
    time_t last_housetime_t;
    time_t last_maintime_t;
    time_t last_timer_t;
    time_t last_epcheck_t;
    void GetSqliteCompileOptions();
    int GetLastImportSource();
public:
    cPluginXmltv2vdr(void);
    virtual ~cPluginXmltv2vdr();
    virtual const char *Version(void)
    {
        return VERSION;
    }
    virtual const char *Description(void)
    {
        return tr(DESCRIPTION);
    }
    virtual const char *CommandLineHelp(void);
    virtual bool ProcessArgs(int argc, char *argv[]);
    virtual bool Initialize(void);
    virtual bool Start(void);
    virtual void Stop(void);
    virtual void Housekeeping(void);
    virtual void MainThreadHook(void);
    virtual cString Active(void);
    virtual time_t WakeupTime(void);
    virtual const char *MainMenuEntry(void);
    virtual cOsdObject *MainMenuAction(void);
    virtual cMenuSetupPage *SetupMenu(void);
    virtual bool SetupParse(const char *Name, const char *Value);
    virtual bool Service(const char *Id, void *Data = NULL);
    virtual const char **SVDRPHelpPages(void);
    virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
};

#endif
