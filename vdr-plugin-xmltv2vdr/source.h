/*
 * source.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __source_h
#define __source_h

#include <vdr/tools.h>

#include "maps.h"
#include "import.h"
#include "parse.h"
#include "debug.h"

#define EPGSOURCES "/var/lib/epgsources" // NEVER (!) CHANGE THIS

#define EITSOURCE "EIT"

class cEPGChannel : public cListObject
{
private:
    bool inuse;
    const char *name;
public:
    cEPGChannel(const char *Name, bool InUse=false);
    ~cEPGChannel();
    virtual int Compare(const cListObject &ListObject) const;
    bool InUse()
    {
        return inuse;
    }
    void SetUsage(bool InUse)
    {
        inuse=InUse;
    }
    const char *Name()
    {
        return name;
    }
};

class cEPGChannels : public cList<cEPGChannel> {};

class cImport;
class cGlobals;

class cEPGSource : public cListObject
{
private:
    const char *name;
    const char *confdir;
    const char *pin;
    const char *epgfile;
    int loglen;
    cParse *parse;
    cImport *import;
    bool ready2parse;
    bool usepipe;
    bool needpin;
    bool running;
    bool disabled;
    bool haspics;
    bool usepics;
    int daysinadvance;
    int exec_weekday;
    int exec_time;
    int daysmax;
    int lastretcode;
    bool ReadConfig();
    int ReadOutput(char *&result, size_t &l);
    cEPGChannels channels;
public:
    cEPGSource(const char *Name, cGlobals *Global);
    ~cEPGSource();
    bool Trace()
    {
        return (logfile!=NULL);
    }
    int Execute(cEPGExecutor &myExecutor);
    int Import(cEPGExecutor &myExecutor);
    bool RunItNow(bool ForceDownload=false);
    time_t NextRunTime(time_t Now=(time_t) 0);
    void Store(void);
    void ChangeChannelSelection(int *Selection);
    char *Log;
    bool Disabled()
    {
        return disabled;
    }
    void Disable()
    {
        disabled=true;
    }
    void Enable()
    {
        disabled=false;
    }
    cEPGChannels *ChannelList()
    {
        return &channels;
    }
    int LastRetCode()
    {
        return lastretcode;
    }
    int ExecTime()
    {
        return exec_time;
    }
    int ExecWeekDay()
    {
        return exec_weekday;
    }
    int DaysMax()
    {
        return daysmax;
    }
    int DaysInAdvance()
    {
        return daysinadvance;
    }
    bool NeedPin()
    {
        return needpin;
    }
    bool HasPics()
    {
        return haspics;
    }
    bool UsePics()
    {
        return usepics;
    }
    const char *Name()
    {
        return name;
    }
    const char *Pin()
    {
        return pin;
    }
    void ChangeExec(int Time, int WeekDay)
    {
        exec_time=Time;
        exec_weekday=WeekDay;
    }
    void ChangeDaysInAdvance(int NewDaysInAdvance)
    {
        daysinadvance=NewDaysInAdvance;
    }
    void ChangePin(const char *NewPin)
    {
        if (pin) free((void *) pin);
        pin=strdup(NewPin);
    }
    void ChangePics(bool NewVal)
    {
        usepics=NewVal;
    }
    void Add2Log(struct tm *Tm, const char Prefix, const char *Line);
    bool Active()
    {
        return running;
    }
};

class cEPGSources : public cList<cEPGSource>
{
public:
    void ReadIn(cGlobals *Global, bool Reload=false);
    bool RunItNow();
    time_t NextRunTime();
    bool Exists(const char *Name);
    cEPGSource *GetSource(const char *Name);
    cEPGSource *GetSourceDB(const char *EpgFile);
    int GetSourceIdx(const char *Name);
    void Remove();
    bool MoveEPGSource(cGlobals *Global, int From, int To);
};

class cPluginXmltv2vdr;

class cEPGExecutor : public cThread
{
private:
    cEPGSources *sources;
    bool forcedownload;
    int forceimportsrc;
public:
    cEPGExecutor(cEPGSources *Sources);
    bool StillRunning()
    {
        return Running();
    }
    void Stop()
    {
        Cancel(3);
    }
    void SetForceDownload()
    {
        forcedownload=true;
        forceimportsrc=-1;
    }
    void SetForceImport(int SourceIdx)
    {
        forceimportsrc=-1;
        if (!sources) return;
        if (SourceIdx>sources->Count()) return;
        if (SourceIdx<0) return;
        forceimportsrc=SourceIdx;
    }
    virtual void Action();
};

#endif
