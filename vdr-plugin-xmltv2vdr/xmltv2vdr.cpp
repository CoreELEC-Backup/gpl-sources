/*
 * xmltv2vdr.cpp: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <vdr/plugin.h>
#include <vdr/videodir.h>
#include <unistd.h>
#include <getopt.h>
#include <stdarg.h>
#include <locale.h>
#include <langinfo.h>
#include <sqlite3.h>
#include <time.h>
#include <sys/types.h>
#include <pwd.h>
#include <netdb.h>
#include <libgen.h>
#include <sys/vfs.h>

#include "setup.h"
#include "xmltv2vdr.h"
#include "debug.h"

int ioprio_set(int which, int who, int ioprio)
{
#if defined(__i386__)
#define __NR_ioprio_set  289
#elif defined(__ppc__)
#define __NR_ioprio_set  273
#elif defined(__x86_64__)
#define __NR_ioprio_set  251
#elif defined(__ia64__)
#define __NR_ioprio_set  1274
#else
#define __NR_ioprio_set  0
#endif
    if (__NR_ioprio_set)
    {
        return syscall(__NR_ioprio_set, which, who, ioprio);
    }
    else
    {
        return 0; // just do nothing
    }
}

char *logfile=NULL;

void logger(cEPGSource *source, char logtype, const char* format, ...)
{
    va_list ap;
    char fmt[255];
    if (source && logtype!='T')
    {
        if (logtype=='E')
        {
            if (snprintf(fmt,sizeof(fmt),"xmltv2vdr: '%s' ERROR %s",source->Name(),format)==-1) return;
        }
        else
        {
            if (snprintf(fmt,sizeof(fmt),"xmltv2vdr: '%s' %s",source->Name(),format)==-1) return;
        }
    }
    else
    {
        if (logtype=='E')
        {
            snprintf(fmt,sizeof(fmt),"xmltv2vdr: ERROR %s",format);
        }
        else
        {
            snprintf(fmt,sizeof(fmt),"xmltv2vdr: %s",format);
        }
    }

    va_start(ap, format);
    char *ptr;
    if (vasprintf(&ptr,fmt,ap)==-1) return;
    va_end(ap);

    struct tm tm;
    if (logfile || source)
    {
        time_t now=time(NULL);
        localtime_r(&now,&tm);
    }

    char *crlf=strchr(ptr,'\n');
    if (crlf) *crlf=0;
    crlf=strchr(ptr,'\r');
    if (crlf) *crlf=0;

    if (source && logtype!='T')
    {
        source->Add2Log(&tm,logtype,ptr);
    }

    if (logfile)
    {
        char dt[30];
        strftime(dt,sizeof(dt)-1,"%b %d %H:%M:%S",&tm);

        FILE *l=fopen(logfile,"a+");
        if (l)
        {
            fprintf(l,"%s [%i] %s\n",dt,cThread::ThreadId(),ptr);
            fclose(l);
        }
    }
    switch (logtype)
    {
    case 'E':
        if (SysLogLevel>0) syslog_with_tid(LOG_ERR,"%s",ptr);
        break;
    case 'I':
        if (SysLogLevel>1) syslog_with_tid(LOG_ERR,"%s",ptr);
        break;
    case 'D':
        if (SysLogLevel>2) syslog_with_tid(LOG_ERR,"%s",ptr);
        break;
    default:
        break;
    }

    free(ptr);
}

// -------------------------------------------------------------

bool cSVDRPMsg::readreply(int fd)
{
    usleep(400000);
    char c=' ';
    do
    {
        struct pollfd fds;
        fds.fd=fd;
        fds.events=POLLIN;
        fds.revents=0;
        int ret=poll(&fds,1,600);

        if (ret<=0) return false;
        if (fds.revents!=POLLIN) return false;
        if (read(fd,&c,1)<0) return false;
    }
    while (c!='\n');
    return true;
}

bool cSVDRPMsg::Send(const char *format, ...)
{
    char *msg;
    va_list ap;
    va_start(ap, format);
    if (vasprintf(&msg,format,ap)==-1) return false;
    va_end(ap);

    int port;
    struct servent *serv=getservbyname("svdrp","tcp");
    if (serv)
    {
        port=htons(serv->s_port);
    }
    else
    {
#if VDRVERSNUM < 10715
        port=2001;
#else
        port=6419;
#endif
    }

    struct sockaddr_in name;
    name.sin_family = AF_INET;
    name.sin_port = htons(port);
    name.sin_addr.s_addr=inet_addr("127.0.0.1");
    uint size = sizeof(name);

    int sock;
    sock=socket(PF_INET, SOCK_STREAM, 0);
    if (sock<0) return false;

    if (connect(sock, (struct sockaddr *)&name,size)!=0)
    {
        close(sock);
        free(msg);
        return false;
    }

    if (!readreply(sock))
    {
        close(sock);
        free(msg);
        return false;
    }

    ssize_t ret;
    ret=write(sock,"PLUG epgsearch ",15);
    if (ret!=(ssize_t)-1) ret=write(sock,msg,strlen(msg));
    if (ret!=(ssize_t)-1) ret=write(sock,"\r\n",2);

    if (!readreply(sock) || (ret==(ssize_t)-1))
    {
        close(sock);
        free(msg);
        return false;
    }

    ret=write(sock,"QUIT\r\n",6);

    if (ret!=(ssize_t)-1) readreply(sock);
    close(sock);
    free(msg);
    return true;
}

// -------------------------------------------------------------

cEPGSearch_Client::cEPGSearch_Client()
{
    plugin=cPluginManager::GetPlugin("epgsearch");
}

bool cEPGSearch_Client::EnableSearchTimer()
{
    if (!plugin) return false;
    Epgsearch_enablesearchtimers_v1_0 serviceData;
    serviceData.enable = true;
    if (!plugin->Service("Epgsearch-enablesearchtimers-v1.0", (void*) &serviceData))
    {
        // SVDRP fallback
        cSVDRPMsg msg;
        if (!msg.Send("SETS ON")) return false;
    }
    return true;
}

bool cEPGSearch_Client::DisableSearchTimer()
{
    if (!plugin) return false;
    Epgsearch_enablesearchtimers_v1_0 serviceData;
    serviceData.enable = false;
    if (!plugin->Service("Epgsearch-enablesearchtimers-v1.0", (void*) &serviceData))
    {
        // SVDRP fallback
        cSVDRPMsg msg;
        if (!msg.Send("SETS OFF")) return false;
    }
    return true;
}

// -------------------------------------------------------------

cGlobals::cGlobals()
{
    confdir=NULL;
    epgfile_store=NULL;
    epgfiledir=NULL;
    epgfile=NULL;
    epdir=NULL;
    epcodeset=NULL;
    imgdir=NULL;
    codeset=NULL;
    srcorder=NULL;
    wakeup=false;
    epghandler=NULL;
    epgtimer=NULL;
    epgseasonepisode=NULL;
    epall=0;
    order=strdup(GetDefaultOrder());
    imgdelafter=30;
    soundex=false;

#if APIVERSNUM > 20101
    if (asprintf(&epgfile_store,"%s/epg.db",cVideoDirectory::Name())==-1) {};
#else
    if (asprintf(&epgfile_store,"%s/epg.db",VideoDirectory)==-1) {};
#endif

    if (!CheckEPGDir("/var/run/vdr"))
    {
        if (!CheckEPGDir("/tmp"))
        {
            if (!CheckEPGDir("/dev/shm"))
            {
                epgfiledir=NULL;
                epgfile=strdup(epgfile_store);
            }
            else
            {
                epgfiledir=strdup("/dev/shm");
            }
        }
        else
        {
            epgfiledir=strdup("/tmp");
        }
    }
    else
    {
        epgfiledir=strdup("/var/run/vdr");
    }

    if (epgfiledir)
    {
        if (asprintf(&epgfile,"%s/epg.db",epgfiledir)==-1) {};
    }

    if (asprintf(&imgdir,"%s","/var/cache/vdr/epgimages")==-1) {};
    if (access(imgdir,R_OK|W_OK)==-1)
    {
        free(imgdir);
        imgdir=NULL;
    }

    if (setlocale(LC_CTYPE,""))
        codeset=strdup(nl_langinfo(CODESET));
    else
    {
        char *LangEnv=getenv("LANG");
        if (LangEnv)
        {
            char *codeset_p=strchr(LangEnv,'.');
            if (codeset_p)
            {
                codeset_p++; // skip dot
                codeset=strdup(codeset_p);
            }
        }
    }
    if (!codeset)
    {
        codeset=strdup("ASCII//TRANSLIT");
    }

    struct passwd pwd,*pwdbuf;
    char buf[1024];
    getpwuid_r(getuid(),&pwd,buf,sizeof(buf),&pwdbuf);
    if (pwdbuf)
    {
        if (asprintf(&epdir,"%s/.eplists/lists",pwdbuf->pw_dir)!=-1)
        {
            if (access(epdir,R_OK))
            {
                free(epdir);
                epdir=NULL;
            }
            else
            {
                epcodeset=codeset;
            }
        }
    }
}

cGlobals::~cGlobals()
{
    free(confdir);
    free(epgfile);
    free(epgfile_store);
    free(epgfiledir);
    free(epdir);
    free(imgdir);
    free(codeset);
    free(order);
    free(srcorder);
    if (epgtimer)
    {
        epgtimer->Stop();
        delete epgtimer;
    }
    if (epgseasonepisode)
    {
        epgseasonepisode->Stop();
        delete epgseasonepisode;
    }
    epgsources.Remove();
    epgmappings.Remove();
    textmappings.Remove();
}

bool cGlobals::CheckEPGDir(const char* EPGFileDir)
{
    struct statfs statfsbuf;
    if (statfs(EPGFileDir,&statfsbuf)==-1) return false;
    if ((statfsbuf.f_type!=0x01021994) && (statfsbuf.f_type!=0x28cd3d45)) return false;
    if (access(EPGFileDir,R_OK|W_OK)==-1) return false;
    return true;
}

void cGlobals::SetEPGFile(const char *EPGFile)
{
    free(epgfile_store);
    free(epgfile);
    epgfile_store=strdup(EPGFile);
    if (!epgfile_store)
    {
        epgfile=NULL;
        return;
    }

    char *tm2=strdup(epgfile_store);
    if (!tm2)
    {
        free(epgfile_store);
        epgfile_store=NULL;
        epgfile=NULL;
        return;
    }
    char *dn=dirname(tm2);
    bool usestore=CheckEPGDir(dn);
    free(tm2);

    if ((usestore) || (!epgfiledir))
    {
        epgfile=strdup(epgfile_store);
    }
    else
    {
        char *tmp=strdup(epgfile_store);
        if (!tmp)
        {
            free(epgfile_store);
            epgfile_store=NULL;
            epgfile=NULL;
            return;
        }
        char *bn=basename(tmp);
        if (asprintf(&epgfile,"%s/%s",epgfiledir,bn)==-1)
        {
            free(epgfile_store);
            free(tmp);
            epgfile_store=NULL;
            epgfile=NULL;
            return;
        }
        free(tmp);
    }
}


void cGlobals::CopyEPGFile(bool Init)
{
    if ((!epgfile) || (!epgfile_store)) return;
    if (!strcmp(epgfile,epgfile_store)) return; // same dir

    struct stat statbuf;
    char *tmpdstfile=NULL;
    int fd=-1;

    if (Init)
    {
        if (stat(epgfile_store,&statbuf)==-1) return; // no file?
        fd=open(epgfile_store,O_RDONLY);
    }
    else
    {
        if (stat(epgfile,&statbuf)==-1) return;
        fd=open(epgfile,O_RDONLY);
    }

    if (fd==-1) return; // no file??
    char *buf=(char *) malloc(statbuf.st_size+1);
    if (!buf)
    {
        close(fd);
        return;
    }
    if (read(fd,buf,statbuf.st_size)!=statbuf.st_size)
    {
        free(buf);
        close(fd);
        return;
    }
    close(fd);
    if (Init)
    {
        fd=creat(epgfile,0644);
    }
    else
    {
        if (asprintf(&tmpdstfile,"%s_",epgfile_store)==-1)
        {
            free(buf);
            return;
        }
        fd=creat(tmpdstfile,0644);
    }
    if (fd==-1)
    {
        if (!Init) free(tmpdstfile);
        free(buf);
        return;
    }
    if (write(fd,buf,statbuf.st_size)!=statbuf.st_size)
    {
        close(fd);
        if (Init)
        {
            unlink(epgfile);
        }
        else
        {
            unlink(tmpdstfile);
            free(tmpdstfile);
        }
        free(buf);
        return;
    }
    close(fd);
    free(buf);
    if (!Init)
    {
        if (rename(tmpdstfile,epgfile_store)!=-1)
        {
            unlink(epgfile);
        }
        free(tmpdstfile);
    }
}


char *cGlobals::GetDefaultOrder()
{
    return (char *) "LOT,CRS,CAD,ORT,CAT,VID,AUD,SEE,RAT,STR,REV";
}

void cGlobals::SetImgDir(const char* ImgDir)
{
    if (!ImgDir) return;
    if (access(ImgDir,R_OK|W_OK)==-1)
    {
        esyslog("cannot access %s",ImgDir);
        return;
    }
    free(imgdir);
    imgdir=strdup(ImgDir);
}


void cGlobals::SetEPDir(const char* EPDir)
{
    if (!EPDir) return;
    if (access(EPDir,R_OK)==-1)
    {
        esyslog("cannot access %s",EPDir);
        return;
    }
    free(epdir);
    epcodeset=codeset;
    epdir=strdup(EPDir);
    if (!epdir) return;
    epcodeset=strchr((char *) epdir,',');
    if (epcodeset)
    {
        *epcodeset=0;
    }
    else
    {
        epcodeset=(char *) codeset;
    }
}

bool cGlobals::DBExists()
{
    if (!epgfile) return true; // is this safe?
    struct stat statbuf;
    if (stat(epgfile,&statbuf)==-1) return false; // no database
    if (!statbuf.st_size) return false; // no database
    return true;
}

// -------------------------------------------------------------

cEPGHandler::cEPGHandler(cGlobals* Global): import(Global)
{
    epall=0;
    maps=Global->EPGMappings();
    sources=Global->EPGSources();
    db=NULL;
    now=0;
    if (ioprio_set(1,getpid(),7 | 3 << 13)==-1)
    {
        tsyslog("failed to set ioprio to 3,7");
    }
}

bool cEPGHandler::IgnoreChannel(const cChannel* Channel)
{
    now=time(NULL);
    if (!maps) return false;
    if (!Channel) return false;
    return maps->IgnoreChannel(Channel);
}

bool cEPGHandler::check4proc(cEvent *event, char **timerdescr, cEPGMapping **map)
{
    if (map) *map=NULL;
    if (timerdescr) *timerdescr=NULL;
    if (!event) return false;
    if (now>(event->StartTime()+event->Duration())) return false; // event in the past?
    if (!maps) return false;
    if (!import.DBExists()) return false;

    cEPGMapping *t_map=maps->GetMap(event->ChannelID());
    if (!t_map)
    {
        if (!epall) return false;
        if (!event->ShortText()) return false;
        if (!timerdescr) return false;
#if VDRVERSNUM <= 10732
        int TimerMatch = tmNone;
#else
        eTimerMatch TimerMatch = tmNone;
#endif

#if VDRVERSNUM>=20301
        cStateKey StateKey;
        if (const cTimers *Timers=cTimers::GetTimersRead(StateKey))
        {
            const cTimer *Timer=Timers->GetMatch(event,&TimerMatch);
            if ((Timer) && (TimerMatch==tmFull))
            {
                *timerdescr=strdup(Timer->ToDescr());
            }
            StateKey.Remove();
        }
#else
        cTimer *timer=Timers.GetMatch(event,&TimerMatch);
        if ((!timer) || (TimerMatch!=tmFull)) return false;
        *timerdescr=strdup(timer->ToDescr());
#endif
    }
    if (map) *map=t_map;
    return true;
}

bool cEPGHandler::SetShortText(cEvent* Event, const char* ShortText)
{
    // prevent setting empty shorttext
    if (!ShortText) return true;
    // prevent setting empty shorttext
    if (!strlen(ShortText)) return true;
    // prevent setting shorttext equal to title
    if (Event->Title() && !strcasecmp(Event->Title(),ShortText)) return true;
    if (!Event->ShortText())
    {
#if VDRDEBUG
        tsyslog("{%5i} setting stext (%s) of '%s'",Event->EventID(),
                ShortText,Event->Title());
#endif
        return false; // no shorttext? new event! let VDR handle this..
    }
    return true;
}

bool cEPGHandler::SetDescription(cEvent* Event, const char* Description)
{
    //cTimer *timer;
    char *timerdescr;
    if (!check4proc(Event,&timerdescr,NULL))
    {
        if (timerdescr) free(timerdescr);
        return false;
    }

    if (import.WasChanged(Event))
    {
        // ok we already changed this event!
        if (!Description)
        {
            if (timerdescr) free(timerdescr);
            return true; // prevent setting nothing to description
        }
        int len=strlen(Description);
        if (!len)
        {
            if (timerdescr) free(timerdescr);
            return true; // prevent setting nothing to description
        }
        if (!strcasestr(Event->Description(),Description))
        {
            // eit description changed -> set it
            tsyslog("{%5i} %schanging descr of '%s'",Event->EventID(),timerdescr ? "*" : "",
                    Event->Title());
            if (timerdescr) free(timerdescr);
            return false;
        }
#ifdef VDRDEBUG
        tsyslog("{%5i} %salready seen descr '%s'",Event->EventID(),timerdescr ? "*" : "",
                Event->Title());
#endif
        if (timerdescr) free(timerdescr);
        return true;
    }
    tsyslog("{%5i} %ssetting descr of '%s'",Event->EventID(),timerdescr ? "*" : "",
            Event->Title());
    if (timerdescr) free(timerdescr);
    return false;
}


bool cEPGHandler::HandleEvent(cEvent* Event)
{
    //cTimer *timer;
    char *timerdescr;
    cEPGMapping *map;
    if (!check4proc(Event,&timerdescr,&map))
    {
        if (timerdescr) free(timerdescr);
        return false;
    }

    int Flags=0;
    const char *ChannelID=strdup(*Event->ChannelID().ToString());
    if (!ChannelID)
    {
        if (timerdescr) free(timerdescr);
        return false;
    }

    if (timerdescr)
    {
        Flags=USE_SEASON;
    }
    else
    {
        // map is always set if seth==false
        Flags=map->Flags();
    }

    cEPGSource *source=NULL;
    cXMLTVEvent *xevent=import.SearchXMLTVEvent(&db,ChannelID,Event);
    if (!xevent)
    {
        if (!epall)
        {
            free((void*)ChannelID);
            if (timerdescr) free(timerdescr);
            return false;
        }
        if (!timerdescr)
        {
            free((void*)ChannelID);
            if (timerdescr) free(timerdescr);
            return false;
        }
        if (db && sqlite3_errcode(db)!=SQLITE_OK)
        {
            free((void*)ChannelID);
            if (timerdescr) free(timerdescr);
            return false;
        }

        source=sources->GetSource(EITSOURCE);
        if (!source) tsyslog("no source for %s",EITSOURCE);
        bool useeptext=((epall & EPLIST_USE_STEXTITLE)==EPLIST_USE_STEXTITLE);
        if (useeptext) Flags|=(USE_SHORTTEXT|OPT_SEASON_STEXTITLE);

        xevent=import.AddXMLTVEvent(source,db,ChannelID,Event,Event->Description(),useeptext);
        if (!xevent)
        {
            free((void*)ChannelID);
            if (timerdescr) free(timerdescr);
            return false;
        }
        else
        {
            tsyslog("{%5i} *adding '%s'/'%s' (%s)",Event->EventID(),xevent->Title(),xevent->ShortText(),timerdescr);
        }
    }
    else
    {
        source=sources->GetSource(xevent->Source());
    }
    free((void*)ChannelID);
    if (timerdescr) free(timerdescr);
    if (!source)
    {
        tsyslog("no source for %s",xevent->Source());
        delete xevent;
        return false;
    }

    if (xevent->Title() && Event->Title())
    {
        if (strcasecmp(xevent->Title(),Event->Title()))
        {
            bool tChanged=false;
            // Title maybe changed, check AltTitle if exists
            if (xevent->AltTitle())
            {
                if (strcasecmp(xevent->AltTitle(),Event->Title()))
                {
                    tChanged=true;
                }
            }
            else
            {
                tChanged=true;
            }

            if (tChanged)
            {
                tsyslog("{%5i} title changed from '%s'->'%s'",Event->EventID(),
                        xevent->Title(),Event->Title());
                xevent->SetEITEventID(0);
                tEventID oldID=Event->EventID();
                Event->SetEventID(0);
                import.UpdateXMLTVEvent(source,db,Event,xevent,NULL);
                Event->SetEventID(oldID);
                delete xevent;
                return false;
            }
        }
    }

    import.PutEvent(source,db,NULL,Event,xevent,Flags);
    delete xevent;
    return false; // let other handlers change this event
}

bool cEPGHandler::SortSchedule(cSchedule* UNUSED(Schedule))
{
    if (db)
    {
        import.Commit(NULL,db);
        sqlite3_close(db);
        db=NULL;
    }
    return false; // we dont sort!
}

// -------------------------------------------------------------

cEPGTimer::cEPGTimer(cGlobals *Global) :
        cThread("xmltv2vdr timer"),import(Global)
{
    sources=Global->EPGSources();
    maps=Global->EPGMappings();
    epall=0;
    SetPriority(19);
    if (ioprio_set(1,getpid(),7 | 3 << 13)==-1)
    {
        dsyslog("failed to set ioprio to 3,7");
    }
}

void cEPGTimer::Action()
{
    if (!import.DBExists()) return; // no database? -> exit immediately

#if VDRVERSNUM<20301
    cSchedulesLock schedulesLock(true,10); // wait 10ms for lock!
    const cSchedules *schedules = cSchedules::Schedules(schedulesLock);
    if (!schedules) return;

    if (Timers.BeingEdited()) return;
    Timers.IncBeingEdited();
#endif

    sqlite3 *db=NULL;
    cEPGSource *source=sources->GetSource(EITSOURCE);
    bool useeptext=((epall & EPLIST_USE_STEXTITLE)==EPLIST_USE_STEXTITLE);
    int Flags=USE_SEASON;
    if (useeptext) Flags|=(USE_SHORTTEXT|OPT_SEASON_STEXTITLE);

#if VDRVERSNUM<20301
    for (cTimer *Timer = Timers.First(); Timer; Timer = Timers.Next(Timer))
#else
    cStateKey StateKey;
    if (const cTimers *Timers=cTimers::GetTimersRead(StateKey))
    {
        for (const cTimer *Timer=Timers->First(); Timer; Timer=Timers->Next(Timer))
#endif
    {
        if (Timer->Recording()) continue; // to late ;)
        cEvent *event=(cEvent *) Timer->Event();
        if (!event) continue;
        if (!useeptext)
        {
            if (!event->ShortText() && !event->Description()) continue; // no text -> no episode
            if (event->ShortText() && event->Description())
            {
                if ((strlen(event->ShortText())+strlen(event->Description()))==0) continue; // no text -> no episode
            }
        }
        if (maps->ProcessChannel(event->ChannelID()))
        {
            if (event->ShortText()) continue; // already processed by xmltv2vdr
        }

        const char *ChannelID=strdup(*event->ChannelID().ToString());
        cXMLTVEvent *xevent=import.SearchXMLTVEvent(&db,ChannelID,event);
        if (!xevent)
        {
            xevent=import.AddXMLTVEvent(source,db,ChannelID,event,event->Description(),useeptext);
            if (!xevent)
            {
                free((void*)ChannelID);
                continue;
            }
            else
            {
                tsyslog("{%5i} +adding '%s'/'%s' (%s)",event->EventID(),xevent->Title(),xevent->ShortText(),*Timer->ToDescr());
            }
        }
        else
        {
            if (!event->ShortText() && event->Description())
            {
                if (import.AddShortTextFromEITDescription(xevent,event->Description()))
                {
                    import.UpdateXMLTVEvent(source,db,xevent);
                }
            }
        }
        free((void*)ChannelID);

        import.PutEvent(source,db,NULL,event,xevent,Flags);
        delete xevent;
    }
#if VDRVERSNUM>=20301
    StateKey.Remove();
}
#endif
if (db)
{
    import.Commit(source,db);
    sqlite3_close(db);
}

#if VDRVERSNUM<20301
Timers.DecBeingEdited();
#endif
}

// -------------------------------------------------------------

cHouseKeeping::cHouseKeeping(cGlobals *Global): cThread("xmltv2vdr housekeeping")
{
    global=Global;
}

void cHouseKeeping::checkdir(const char* imgdir, int age, int &cnt, int &lcnt)
{
    if (age<=0) return;
    DIR *dir=opendir(imgdir);
    if (!dir) return;
    time_t tmin=time(NULL);
    tmin-=(age*86400);
    struct dirent *dirent;

    while (dirent=readdir(dir))
    {
        if (dirent->d_name[0]=='.') continue;
        if ((dirent->d_type==DT_LNK) || (dirent->d_type==DT_REG))
        {
            struct stat statbuf;
            char *fpath;
            if (asprintf(&fpath,"%s/%s",imgdir,dirent->d_name)!=-1)
            {
                if (stat(fpath,&statbuf)!=-1)
                {
                    if (statbuf.st_mtime<tmin)
                    {
                        if (unlink(fpath)!=-1)
                        {
                            if (dirent->d_type==DT_LNK) lcnt++;
                            if (dirent->d_type==DT_REG) cnt++;
                        }
                    }
                }
                free(fpath);
            }
        }
    }
    closedir(dir);
    return;
}

int sd_select(const dirent* dirent)
{
    if (!dirent) return 0;
    if (strstr(dirent->d_name,"-img"))
    {
        if (dirent->d_type==DT_DIR) return 1;
    }
    return 0;
}

void cHouseKeeping::Action()
{
    if (global->ImgDelAfter() && global->ImgDir())
    {
        int cnt=0,lcnt=0;
        checkdir(global->ImgDir(),global->ImgDelAfter(),cnt,lcnt);
        struct dirent **names;
        int ret=scandir("/var/lib/epgsources",&names,sd_select,alphasort);
        if (ret>0)
        {
            for (int i=0; i<ret; i++)
            {
                char *newdir;
                if (asprintf(&newdir,"/var/lib/epgsources/%s",names[i]->d_name)!=-1)
                {
                    checkdir(newdir,global->ImgDelAfter(),cnt,lcnt);
                    free(newdir);
                }
            }
            free(names);
        }
        if (lcnt)
        {
            isyslog("removed %i links",lcnt);
        }
        if (cnt)
        {
            isyslog("removed %i pics",cnt);
        }
    }

    if (!global->DBExists()) return;

#if APIVERSNUM<20301
    cSchedulesLock schedulesLock(true,10); // wait 10ms for lock!
    const cSchedules *schedules = cSchedules::Schedules(schedulesLock);
    if (!schedules) return;
#endif

    sqlite3 *db=NULL;
    if (sqlite3_open_v2(global->EPGFile(),&db,SQLITE_OPEN_READWRITE,NULL)==SQLITE_OK)
    {
        char *sql;
        if (asprintf(&sql,"delete from epg where ((starttime+duration) < %li)",time(NULL))!=-1)
        {
            char *errmsg;
            if (sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
            {
                esyslog("%s",errmsg);
                sqlite3_free(errmsg);
            }
            else
            {
                int changes=sqlite3_changes(db);
                if (changes)
                {
                    isyslog("removed %i old entries from db",changes);
                    sqlite3_exec(db,"VACCUM;",NULL,NULL,NULL);
                }
            }
            free(sql);
        }
    }
    sqlite3_close(db);
}

// -------------------------------------------------------------

cEPGSeasonEpisode::cEPGSeasonEpisode(cGlobals *Global): cThread("xmltv2vdr seasonepisode")
{
    epgfile=Global->EPGFile();
}

void cEPGSeasonEpisode::Action()
{
    // TODO: update season, episode, episodeoverall fields in db
}

// -------------------------------------------------------------

cPluginXmltv2vdr::cPluginXmltv2vdr(void) : housekeeping(&g),epgexecutor(g.EPGSources())
{
    // Initialize any member variables here.
    // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
    // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
    logfile=NULL;
    last_maintime_t=0;
    last_epcheck_t=last_housetime_t=time(NULL); // start this threads later!
    last_timer_t=last_epcheck_t-(time_t) 540; // check timers in 60 seconds
    g.SetEPAll(0);
    g.TEXTMappings()->Add(new cTEXTMapping("country",tr("country")));
    g.TEXTMappings()->Add(new cTEXTMapping("year",tr("year")));
    g.TEXTMappings()->Add(new cTEXTMapping("originaltitle",tr("originaltitle")));
    g.TEXTMappings()->Add(new cTEXTMapping("category",tr("category")));
    g.TEXTMappings()->Add(new cTEXTMapping("actor",tr("actor")));
    g.TEXTMappings()->Add(new cTEXTMapping("adapter",tr("adapter")));
    g.TEXTMappings()->Add(new cTEXTMapping("commentator",tr("commentator")));
    g.TEXTMappings()->Add(new cTEXTMapping("composer",tr("composer")));
    g.TEXTMappings()->Add(new cTEXTMapping("director",tr("director")));
    g.TEXTMappings()->Add(new cTEXTMapping("editor",tr("editor")));
    g.TEXTMappings()->Add(new cTEXTMapping("guest",tr("guest")));
    g.TEXTMappings()->Add(new cTEXTMapping("presenter",tr("presenter")));
    g.TEXTMappings()->Add(new cTEXTMapping("producer",tr("producer")));
    g.TEXTMappings()->Add(new cTEXTMapping("writer",tr("writer")));
    g.TEXTMappings()->Add(new cTEXTMapping("video",tr("video")));
    g.TEXTMappings()->Add(new cTEXTMapping("blacknwhite",tr("blacknwhite")));
    g.TEXTMappings()->Add(new cTEXTMapping("audio",tr("audio")));
    g.TEXTMappings()->Add(new cTEXTMapping("dolby",tr("dolby")));
    g.TEXTMappings()->Add(new cTEXTMapping("dolbydigital",tr("dolbydigital")));
    g.TEXTMappings()->Add(new cTEXTMapping("bilingual",tr("bilingual")));
    g.TEXTMappings()->Add(new cTEXTMapping("review",tr("review")));
    g.TEXTMappings()->Add(new cTEXTMapping("starrating",tr("starrating")));
    g.TEXTMappings()->Add(new cTEXTMapping("season",tr("season")));
    g.TEXTMappings()->Add(new cTEXTMapping("episode",tr("episode")));
    g.TEXTMappings()->Add(new cTEXTMapping("episodeoverall",tr("episodeoverall")));
}

cPluginXmltv2vdr::~cPluginXmltv2vdr()
{
    // Clean up after yourself!
#if VDRVERSNUM < 10726 && (!EPGHANDLER)
    delete g.epghandler;
#endif
}

void cPluginXmltv2vdr::GetSqliteCompileOptions()
{
    sqlite3 *db=NULL;
    if (sqlite3_open(":memory:",&db)!=SQLITE_OK) return;

    char sql[]="pragma compile_options;";
    sqlite3_stmt *stmt;

    int ret=sqlite3_prepare_v2(db,sql,strlen(sql),&stmt,NULL);
    if (ret!=SQLITE_OK)
    {
        esyslog("%i %s (gsco)",ret,sqlite3_errmsg(db));
        sqlite3_close(db);
        return ;
    }

    for (;;)
    {
        if (sqlite3_step(stmt)==SQLITE_ROW)
        {
            const char *option=(const char *) sqlite3_column_text(stmt,0);
            tsyslog("option %s",option);
            if (!strncasecmp(option,"SOUNDEX",7)) g.SetSoundEx();
        }
        else
        {
            break;
        }
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return;
}

int cPluginXmltv2vdr::GetLastImportSource()
{
    sqlite3 *db=NULL;
    if (sqlite3_open_v2(g.EPGFile(),&db,SQLITE_OPEN_READWRITE,NULL)!=SQLITE_OK) return -1;

    char sql[]="select srcidx from epg where srcidx<>99 order by starttime desc limit 1";
    sqlite3_stmt *stmt;

    int ret=sqlite3_prepare_v2(db,sql,strlen(sql),&stmt,NULL);
    if (ret!=SQLITE_OK)
    {
        esyslog("%i %s (glis)",ret,sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    int idx=-1;
    if (sqlite3_step(stmt)==SQLITE_ROW)
    {
        idx=sqlite3_column_int(stmt,0);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    tsyslog("lastimportsource=%i",idx);
    return idx;
}

const char *cPluginXmltv2vdr::CommandLineHelp(void)
{
    // Return a string that describes all known command line options.
    return "  -e DIR,   --episodes=DIR location of episode files: VDRSeriesTimer .episodes\n"
           "                           or TheTVDB .xml (default is ~/.eplists/lists)\n"
           "                           Add UTF-8 or ISO8859-15 to the path to specify the\n"
           "                           charset used in the files, e.g. /vdr/myepisodes,UTF8\n"
           "  -E FILE,  --epgfile=FILE write the EPG data into the given FILE (default is\n"
           "                           'epg.db' in the video directory) - best performance\n"
           "                           if located on a ramdisk\n"
           "  -i DIR    --images=DIR   location of epgimages\n"
           "                           (default is /var/cache/vdr/epgimages)\n"
           "  -l FILE   --logfile=FILE write trace logs into the given FILE (default is\n"
           "                           no trace log\n";
}

bool cPluginXmltv2vdr::ProcessArgs(int argc, char *argv[])
{
    // Command line argument processing
    static struct option long_options[] =
    {
        { "episodes",     required_argument, NULL, 'e'},
        { "epgfile",      required_argument, NULL, 'E'},
        { "images",       required_argument, NULL, 'i'},
        { "logfile",      required_argument, NULL, 'l'},
        { 0,0,0,0 }
    };

    int c;
    while ((c = getopt_long(argc, argv, "l:e:E:i:", long_options, NULL)) != -1)
    {
        switch (c)
        {
        case 'e':
            g.SetEPDir(optarg);
            break;
        case 'E':
            g.SetEPGFile(optarg);
            break;
        case 'i':
            g.SetImgDir(optarg);
        case 'l':
            if (logfile) free(logfile);
            logfile=strdup(optarg);
            break;
        default:
            return false;
        }
    }
    return true;
}

bool cPluginXmltv2vdr::Initialize(void)
{
    // Initialize any background activities the plugin shall perform.
    return true;
}

bool cPluginXmltv2vdr::Start(void)
{
    // Start any background activities the plugin shall perform.
    g.SetConfDir(ConfigDirectory(PLUGIN_NAME_I18N));

    isyslog("using codeset '%s'",g.Codeset());
    isyslog("using file '%s' for epg database (storage)",g.EPGFileStore());
    isyslog("using file '%s' for epg database (runtime)",g.EPGFile());
    g.CopyEPGFile(true);
    if (g.EPDir())
    {
        isyslog("using dir '%s' (%s) for episodes",g.EPDir(),g.EPCodeset());
        g.AllocateEPGSeasonThread();
    }
    if (g.EPAll())
    {
        g.AllocateEPGTimerThread();
    }
    if (g.ImgDir()) isyslog("using dir '%s' for epgimages (%i)",g.ImgDir(),g.ImgDelAfter());

    g.EPGSources()->ReadIn(&g);
    g.epghandler = new cEPGHandler(&g);
    g.SetEPAll(g.EPAll());
    isyslog("using sqlite v%s",sqlite3_libversion());
    GetSqliteCompileOptions();
    if (sqlite3_threadsafe()==0) esyslog("sqlite3 not threadsafe!");
    sqlite3_enable_shared_cache(0);
    cParse::InitLibXML();
    return true;
}

void cPluginXmltv2vdr::Stop(void)
{
    // Stop any background activities the plugin is performing.
    epgexecutor.Stop();
    housekeeping.Stop();
    cParse::CleanupLibXML();
    if (logfile)
    {
        free(logfile);
        logfile=NULL;
    }
    g.CopyEPGFile(false);
}

void cPluginXmltv2vdr::Housekeeping(void)
{
    // Perform any cleanup or other regular tasks.
    time_t now=time(NULL);
    if (now>(last_housetime_t+3600))
    {
        if (!housekeeping.Active())
        {
            housekeeping.Start();
        }
        last_housetime_t=(now / 3600)*3600;
    }
}

void cPluginXmltv2vdr::MainThreadHook(void)
{
    // Perform actions in the context of the main program thread.
    // WARNING: Use with great care - see PLUGINS.html!
    time_t now=time(NULL);
    if (now>=(last_maintime_t+60))
    {
        if (!epgexecutor.Active())
        {
            if (g.EPGSources()->RunItNow()) epgexecutor.Start();
        }
        last_maintime_t=(now/60)*60;
    }
    if (g.EPDir())
    {
        /*
          if (now>=(last_epcheck_t+900))
          {
              if (g.EPGSeasonEpisode()) g.EPGSeasonEpisode()->Start();
              last_epcheck_t=(now/900)*900;
          }
        */
        if (g.EPAll())
        {
            if (now>=(last_timer_t+600))
            {
                if (g.EPGTimer()) g.EPGTimer()->Start();
                last_timer_t=(now/600)*600;
            }
        }
    }
}

cString cPluginXmltv2vdr::Active(void)
{
    // Return a message string if shutdown should be postponed
    if (epgexecutor.Active())
    {
        return tr("xmltv2vdr plugin still working");
    }
    return NULL;
}

time_t cPluginXmltv2vdr::WakeupTime(void)
{
    // Return custom wakeup time for shutdown script
    if (!g.WakeUp()) return (time_t) 0;
    time_t nextruntime=g.EPGSources()->NextRunTime();
    if (nextruntime) nextruntime-=(time_t) 180;
#ifdef VDRDBG
    tsyslog("reporting wakeuptime %s",ctime(&nextruntime));
#endif
    return nextruntime;
}

const char *cPluginXmltv2vdr::MainMenuEntry(void)
{
    // Return a main menu entry
    return NULL;
}

cOsdObject *cPluginXmltv2vdr::MainMenuAction(void)
{
    // Perform the action when selected from the main VDR menu.
    return NULL;
}

cMenuSetupPage *cPluginXmltv2vdr::SetupMenu(void)
{
    // Return a setup menu in case the plugin supports one.
    return new cMenuSetupXmltv2vdr(&g);
}

bool cPluginXmltv2vdr::SetupParse(const char *Name, const char *Value)
{
    // Parse your own setup parameters and store their values.
    if (!strncasecmp(Name,"channel",7))
    {
        if (strlen(Name)<10) return false;
        g.EPGMappings()->Add(new cEPGMapping(&Name[8],Value));
    }
    else if (!strncasecmp(Name,"textmap",7))
    {
        if (strlen(Name)<10) return false;
        cTEXTMapping *textmap=g.TEXTMappings()->GetMap(&Name[8]);
        if (textmap)
        {
            textmap->ChangeValue(Value);
        }
        else
        {
            g.TEXTMappings()->Add(new cTEXTMapping(&Name[8],Value));
        }
    }
    else if (!strcasecmp(Name,"options.epall"))
    {
        g.SetEPAll(atoi(Value));
    }
    else if (!strcasecmp(Name,"options.wakeup"))
    {
        g.SetWakeUp((bool) atoi(Value));
    }
    else if (!strcasecmp(Name,"options.imgdelafter"))
    {
        g.SetImgDelAfter(atoi(Value));
    }
    else if (!strcasecmp(Name,"options.order"))
    {
        g.SetOrder(Value);
    }
    else if (!strcasecmp(Name,"source.order"))
    {
        g.SetSrcOrder(Value);
    }
    else return false;
    return true;
}

bool cPluginXmltv2vdr::Service(const char *UNUSED(Id), void *UNUSED(Data))
{
    // Handle custom service requests from other plugins
    return false;
}

const char **cPluginXmltv2vdr::SVDRPHelpPages(void)
{
    // Returns help text
    static const char *HelpPages[]=
    {
        "UPDT [force]\n"
        "    Start epg update from db, with force download data before\n",
        "DELD\n"
        "    Delete xmltv2vdr epg database (triggers update)\n",
        "HOUS\n"
        "    Start housekeeping manually\n",
        "TIMR\n"
        "    Start timerthread manually\n",
        NULL
    };
    return HelpPages;
}

cString cPluginXmltv2vdr::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
    // Process SVDRP commands

    cString output=NULL;
    if (!strcasecmp(Command,"UPDT"))
    {
        if (!g.EPGSources()->Count())
        {
            ReplyCode=550;
            output="no epg sources installed\n";
        }
        else
        {
            if (epgexecutor.Active())
            {
                ReplyCode=550;
                output="update already running\n";
            }
            else
            {
                if (Option && strstr(Option,"force"))
                {
                    epgexecutor.SetForceDownload();
                }
                else
                {
                    epgexecutor.SetForceImport(GetLastImportSource());
                }
                if (epgexecutor.Start())
                {
                    ReplyCode=250;
                    output="update started\n";
                }
                else
                {
                    ReplyCode=550;
                    output="failed to start update\n";
                }
            }
        }
    }
    if (!strcasecmp(Command,"DELD"))
    {
        if (g.EPGFile())
        {
            if (unlink(g.EPGFile())==-1)
            {
                ReplyCode=550;
                output="failed to delete database\n";
            }
            else
            {
                ReplyCode=250;
                output="database deleted\n";
            }
        }
        else
        {
            ReplyCode=550;
            output="epgfile parameter not set\n";
        }
    }
    if (!strcasecmp(Command,"TIMR"))
    {
        if (!epgexecutor.Active() && g.EPGTimer() && !g.EPGTimer()->Active())
        {
            g.EPGTimer()->Start();
            last_timer_t=(time(NULL)/600)*600;
            ReplyCode=250;
            output="timerthread started\n";
        }
        else
        {
            if (g.EPGTimer())
            {
                ReplyCode=550;
                output="system busy\n";
            }
            else
            {
                ReplyCode=550;
                output="no timerthread\n";
            }
        }
    }
    if (!strcasecmp(Command,"HOUS"))
    {
        if (!epgexecutor.Active() && !housekeeping.Active())
        {
            housekeeping.Start();
            last_housetime_t=(time(NULL)/3600)*3600;
            ReplyCode=250;
            output="housekeeping started\n";
        }
        else
        {
            ReplyCode=550;
            output="system busy\n";
        }
    }
    return output;
}

VDRPLUGINCREATOR(cPluginXmltv2vdr) // Don't touch this!
