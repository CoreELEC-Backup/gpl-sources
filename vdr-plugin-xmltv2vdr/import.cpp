/*
 * import.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <sqlite3.h>
#include <ctype.h>
#include <time.h>
#include <pcrecpp.h>
#include <unistd.h>
#include <sys/types.h>
#include <vdr/channels.h>

#include "xmltv2vdr.h"
#include "import.h"
#include "event.h"
#include "debug.h"

extern char *strcatrealloc(char *, const char*);

struct cImport::split cImport::split(char *in, char delim)
{
    struct split sp;
    sp.count=1;
    sp.pointers[0]=in;
    while (*++in)
    {
        if (*in==delim)
        {
            *in=0;
            sp.pointers[sp.count++]=in+1;
        }
    }
    return sp;
}

char *cImport::RemoveNonASCII(const char *src)
{
    if (!src) return NULL;
    int len=strlen(src);
    if (!len) return NULL;
    char *dst=(char *) malloc(len+1);
    if (!dst) return NULL;
    char *tmp=dst;
    bool lspc=false;
    while (*src)
    {
        // 0x20,0x30-0x39,0x41-0x5A,0x61-0x7A
        if ((*src==0x20) && (!lspc))
        {
            *tmp++=0x20;
            lspc=true;
        }
        if (*src==':')
        {
            *tmp++=0x20;
            lspc=true;
        }
        if ((*src>=0x30) && (*src<=0x39))
        {
            *tmp++=*src;
            lspc=false;
        }
        if ((*src>=0x41) && (*src<=0x5A))
        {
            *tmp++=tolower(*src);
            lspc=false;
        }
        if ((*src>=0x61) && (*src<=0x7A))
        {
            *tmp++=*src;
            lspc=false;
        }
        src++;
    }
    *tmp=0;
    return dst;
}

cEvent *cImport::SearchVDREventByTitle(cEPGSource *source, cSchedule* schedule, const char *Title, time_t StartTime,
                                       int Duration, int hint)
{
    const char *cxTitle=conv->Convert(Title);

    // 2nd with StartTime
    cEvent *f=(cEvent *) schedule->GetEvent((tEventID) 0,StartTime+hint);
    if (f)
    {
        if (!strcasecmp(f->Title(),cxTitle))
        {
            return f;
        }
        else
        {
            f=NULL;
        }
    }
    // 3rd with StartTime +/- TimeDiff
    int maxdiff=INT_MAX;
    int eventTimeDiff=720;
    if (Duration && eventTimeDiff>=Duration) eventTimeDiff/=3;
    if (eventTimeDiff<100) eventTimeDiff=100;

    for (cEvent *p =(cEvent *) schedule->Events()->First(); p; p = (cEvent *) schedule->Events()->Next(p))
    {
        int diff=abs((int) difftime(p->StartTime(),StartTime));
        if (diff<=eventTimeDiff)
        {
            // found event with exact the same title
            if (!strcasecmp(p->Title(),cxTitle))
            {
                if (diff<=maxdiff)
                {
                    f=p;
                    maxdiff=diff;
                }
            }
            else
            {
                if (f) continue; // we already have an event!
                // cut both titles into pieces and check
                // if we have at least one match with
                // minimum length of 4 characters

                // first remove all non ascii characters
                // we just want the following codes
                // 0x20,0x30-0x39,0x41-0x5A,0x61-0x7A
                int wfound=0;
                char *s1=RemoveNonASCII(p->Title());
                char *s2=RemoveNonASCII(cxTitle);
                if (s1 && s2)
                {
                    if (!strcmp(s1,s2))
                    {
                        wfound++;
                    }
                    else
                    {
                        struct split w1 = split(s1,' ');
                        struct split w2 = split(s2,' ');
                        if ((w1.count) && (w2.count))
                        {
                            for (int i1=0; i1<w1.count; i1++)
                            {
                                for (int i2=0; i2<w2.count; i2++)
                                {
                                    if (!strcmp(w1.pointers[i1],w2.pointers[i2]))
                                    {
                                        if (strlen(w1.pointers[i1])>3) wfound++;
                                    }
                                }
                            }
                        }
                    }
                }
                if (s1) free(s1);
                if (s2) free(s2);

                if (wfound)
                {
                    if (diff<=maxdiff)
                    {
                        if (!WasChanged(p))
                        {
                            tsyslogs(source,"found '%s' for '%s'",p->Title(),cxTitle);
                        }
                        f=p;
                        maxdiff=diff;
                    }
                }
            }
        }
    }

    return f;
}

cEvent *cImport::SearchVDREvent(cEPGSource *source, cSchedule* schedule, cXMLTVEvent *xevent, bool append, int hint)
{
    if (!source) return NULL;
    if (!schedule) return NULL;
    if (!xevent) return NULL;

    cEvent *f=NULL;

    // try to find an event,
    // 1st with our own EventID
    if (xevent->EITEventID()) f=(cEvent *) schedule->GetEvent(xevent->EITEventID());
    if (f) return f;

    if (xevent->EventID() && append) f=(cEvent *) schedule->GetEvent(xevent->EventID());
    if (f) return f;

    f=SearchVDREventByTitle(source, schedule, xevent->Title(), xevent->StartTime(),
                            xevent->Duration(), hint);
    if (f) return f;

    if (!xevent->AltTitle()) return NULL;

    return SearchVDREventByTitle(source, schedule, xevent->AltTitle(), xevent->StartTime(),
                                 xevent->Duration(), hint);
}

cEvent *cImport::GetEventBefore(cSchedule* schedule, time_t start)
{
    if (!schedule) return NULL;
    if (!schedule->Events()) return NULL;
    if (!schedule->Events()->Count()) return NULL;
    cEvent *last=(cEvent *) schedule->Events()->Last();
    if ((last) && (last->StartTime()<start)) return last;
    for (cEvent *p=(cEvent *) schedule->Events()->First(); p; p=(cEvent *) schedule->Events()->Next(p))
    {
        if (p->StartTime()>start)
        {
            return (cEvent *) p->Prev();
        }
    }
    if (last) return last;
    return NULL;
}

char *cImport::RemoveLastCharFromDescription(char *description)
{
    if (description)
    {
        int len=strlen(description);
        if (!len) return description;
        description[len-1]=0;
    }
    return description;
}

char *cImport::Add2Description(char *description, const char *Value)
{
    description = strcatrealloc(description,Value);
    return description;
}

char *cImport::Add2Description(char *description, const char *Name, const char *Value)
{
    description = strcatrealloc(description,Name);
    description = strcatrealloc(description,": ");
    description = strcatrealloc(description,Value);
    description = strcatrealloc(description,"\n");
    return description;
}

char *cImport::Add2Description(char *description, const char *Name, int Value)
{
    char *value=NULL;
    if (asprintf(&value,"%i",Value)==-1) return NULL;
    description = strcatrealloc(description,Name);
    description = strcatrealloc(description,": ");
    description = strcatrealloc(description,value);
    description = strcatrealloc(description,"\n");
    free(value);
    return description;
}

char *cImport::AddEOT2Description(char *description, bool checkutf8)
{
    const char nbspUTF8[]={-62,-96,0};

    if (checkutf8)
    {
        if (!g->Codeset())
        {
            description=strcatrealloc(description,nbspUTF8);
        }
        else
        {
            if (!strncasecmp(g->Codeset(),"UTF-8",5) || !strncasecmp(g->Codeset(),"UTF8",4))
            {
                description=strcatrealloc(description,nbspUTF8);
            }
            else
            {
                const char nbsp[]={-96,0};
                description=strcatrealloc(description,nbsp);
            }
        }
    }
    else
    {
        description=strcatrealloc(description,nbspUTF8);
    }
    return description;
}

bool cImport::WasChanged(cEvent* Event)
{
    if (!Event) return false;
    if (!Event->Description()) return false;
    if (!strchr(Event->Description(),0xA0)) return false;
    char *p=strchr((char *) Event->Description(),0xA0);
    if (!p) return false;
    if (!g->Codeset()) return false;
    if (!strncasecmp(g->Codeset(),"UTF-8",5) || !strncasecmp(g->Codeset(),"UTF8",4))
    {
        if ((unsigned char) p[-1]!=0xC2) return false;
    }
    return true;
}

void cImport::LinkPictures(const char *Source, cXMLTVStringList *Pics, tEventID DestID, tChannelID ChanID, bool MakeOld)
{
    // source-pics are located in /var/lib/epgsources/%SOURCE%-img/
    // dest-pics are located in imgdir (default /var/cache/vdr/epgimages)

    for (int i=0; i<Pics->Size(); i++)
    {
        char *pic=(*Pics)[i];

        char *ext=strrchr(pic,'.');
        if (!ext) continue;

        ext++;
        char *dst,*dstold=NULL;
        int ret;

        char *src;
        if (asprintf(&src,"/var/lib/epgsources/%s-img/%s",Source,pic)==-1) return;

        if (MakeOld)
        {
            if (!i)
            {
                ret=asprintf(&dstold,"%s/%i.%s",g->ImgDir(),DestID,ext);
            }
            else
            {
                ret=asprintf(&dstold,"%s/%i_%i.%s",g->ImgDir(),DestID,i,ext);
            }
            if (ret==-1)
            {
                free(src);
                return;
            }
        }

        if (!i)
        {
            ret=asprintf(&dst,"%s/%s_%i.%s",g->ImgDir(),*ChanID.ToString(),DestID,ext);
        }
        else
        {
            ret=asprintf(&dst,"%s/%s_%i_%i.%s",g->ImgDir(),*ChanID.ToString(),DestID,i,ext);
        }
        if (ret==-1)
        {
            free(src);
            if (dstold) free(dstold);
            return;
        }

        struct stat statbuf;
        if (MakeOld)
        {
            if (stat(dstold,&statbuf)!=-1)
            {
                unlink(dstold);
            }
            if (symlink(src,dstold)==-1)
            {
                if (!i)
                {
                    tsyslog("failed to link %s to %i.%s",pic,DestID,ext);
                }
                else
                {
                    tsyslog("failed to link %s to %i_%i.%s",pic,DestID,i,ext);
                }
            }
            else
            {
                if (!i)
                {
                    tsyslog("linked %s to %i.%s",pic,DestID,ext);
                }
                else
                {
                    tsyslog("linked %s to %i_%i.%s",pic,DestID,i,ext);
                }
            }
        }

        if (stat(dst,&statbuf)!=-1)
        {
            unlink(dst);
        }
        if (symlink(src,dst)==-1)
        {
            if (!i)
            {
                tsyslog("failed to link %s to %s_%i.%s",pic,*ChanID.ToString(),DestID,ext);
            }
            else
            {
                tsyslog("failed to link %s to %s_%i_%i.%s",pic,*ChanID.ToString(),DestID,i,ext);
            }
        }
        else
        {
            if (!i)
            {
                tsyslog("linked %s to %s_%i.%s",pic,*ChanID.ToString(),DestID,ext);
            }
            else
            {
                tsyslog("linked %s to %s_%i_%i.%s",pic,*ChanID.ToString(),DestID,i,ext);
            }
        }

        free(src);
        free(dst);
        if (dstold) free(dstold);

    }
}

char *cImport::Add2Description(char *description, cXMLTVEvent *xEvent, int Flags, int what)
{
    if (what==USE_LONGTEXT)
    {
        bool lta=false;
        if (((Flags & USE_LONGTEXT)==USE_LONGTEXT) || ((Flags & OPT_APPEND)==OPT_APPEND))
        {
            if (xEvent->Description() && (strlen(xEvent->Description())>0))
            {
                description=Add2Description(description,xEvent->Description());
                lta=true;
            }
        }

        if (!lta && xEvent->EITDescription() && (strlen(xEvent->EITDescription())>0))
        {
            description=Add2Description(description,xEvent->EITDescription());
        }
        description=Add2Description(description,"\n");
    }

    if ((what==USE_CREDITS) && ((Flags & USE_CREDITS)==USE_CREDITS))
    {
        cXMLTVStringList *credits=xEvent->Credits();
        if (credits->Size())
        {
            cTEXTMapping *oldtext=NULL;
            for (int i=0; i<credits->Size(); i++)
            {
                char *ctype=strdup((*credits)[i]);
                if (ctype)
                {
                    char *cval=strchr(ctype,'|');
                    if (cval)
                    {
                        *cval=0;
                        cval++;
                        bool add=true;
                        if (((Flags & CREDITS_ACTORS)!=CREDITS_ACTORS) &&
                                (!strcasecmp(ctype,"actor"))) add=false;
                        if (((Flags & CREDITS_DIRECTORS)!=CREDITS_DIRECTORS) &&
                                (!strcasecmp(ctype,"director"))) add=false;
                        if (((Flags & CREDITS_OTHERS)!=CREDITS_OTHERS) &&
                                (add) && (strcasecmp(ctype,"actor")) &&
                                (strcasecmp(ctype,"director"))) add=false;
                        if (add)
                        {
                            cTEXTMapping *text=g->TEXTMappings()->GetMap(ctype);
                            if ((Flags & CREDITS_LIST)==CREDITS_LIST)
                            {
                                if (oldtext!=text)
                                {
                                    if (oldtext)
                                    {
                                        description=RemoveLastCharFromDescription(description);
                                        description=RemoveLastCharFromDescription(description);
                                        description=Add2Description(description,"\n");
                                    }
                                    description=Add2Description(description,text->Value());
                                    description=Add2Description(description,": ");
                                }
                                description=Add2Description(description,cval);
                                description=Add2Description(description,", ");
                            }
                            else
                            {
                                if (text)
                                {
                                    description=Add2Description(description,text->Value(),cval);
                                }
                            }
                            oldtext=text;
                        }
                    }
                    free(ctype);
                }
            }
            if ((oldtext) && ((Flags & CREDITS_LIST)==CREDITS_LIST))
            {
                description=RemoveLastCharFromDescription(description);
                description=RemoveLastCharFromDescription(description);
                description=Add2Description(description,"\n");
            }
        }
    }

    if ((what==USE_COUNTRYDATE) && ((Flags & USE_COUNTRYDATE)==USE_COUNTRYDATE))
    {
        if (xEvent->Country())
        {
            cTEXTMapping *text=g->TEXTMappings()->GetMap("country");
            if (text) description=Add2Description(description,text->Value(),xEvent->Country());
        }

        if (xEvent->Year())
        {
            cTEXTMapping *text=g->TEXTMappings()->GetMap("year");
            if (text) description=Add2Description(description,text->Value(),xEvent->Year());
        }
    }
    if ((what==USE_ORIGTITLE) && ((Flags & USE_ORIGTITLE)==USE_ORIGTITLE) &&
            (xEvent->OrigTitle()))
    {
        cTEXTMapping *text=g->TEXTMappings()->GetMap("originaltitle");
        if (text) description=Add2Description(description,text->Value(),xEvent->OrigTitle());
    }
    if ((what==USE_CATEGORIES) && ((Flags & USE_CATEGORIES)==USE_CATEGORIES) &&
            (xEvent->Category()->Size()))
    {
        cTEXTMapping *text=g->TEXTMappings()->GetMap("category");
        if (text)
        {
            cXMLTVStringList *categories=xEvent->Category();
            // prevent duplicates
            if ((*categories)[0][0]!='G' && (*categories)[0][1]!=' ')
                description=Add2Description(description,text->Value(),(*categories)[0]);
            for (int i=1; i<categories->Size(); i++)
            {
                if (strcasecmp((*categories)[i],(*categories)[i-1]))
                {
                    if ((*categories)[i][0]!='G' && (*categories)[i][1]!=' ')
                        description=Add2Description(description,text->Value(),(*categories)[i]);
                }
            }
        }
    }

    if ((what==USE_VIDEO) && ((Flags & USE_VIDEO)==USE_VIDEO) && (xEvent->Video()->Size()))
    {
        cTEXTMapping *text=g->TEXTMappings()->GetMap("video");
        if (text)
        {
            description=Add2Description(description,text->Value());
            description=Add2Description(description,": ");
            cXMLTVStringList *video=xEvent->Video();
            for (int i=0; i<video->Size(); i++)
            {
                char *vtype=strdup((*video)[i]);
                if (vtype)
                {
                    char *vval=strchr(vtype,'|');
                    if (vval)
                    {
                        *vval=0;
                        vval++;

                        if (i)
                        {
                            description=Add2Description(description,", ");
                        }

                        if (!strcasecmp(vtype,"colour"))
                        {
                            if (!strcasecmp(vval,"no"))
                            {
                                cTEXTMapping *text=g->TEXTMappings()->GetMap("blacknwhite");
                                description=Add2Description(description,text->Value());
                            }
                        }
                        else
                        {
                            description=Add2Description(description,vval);
                        }
                    }
                    free(vtype);
                }
            }
            description=Add2Description(description,"\n");
        }
    }

    if ((what==USE_AUDIO) && ((Flags & USE_AUDIO)==USE_AUDIO))
    {
        if (xEvent->Audio())
        {
            cTEXTMapping *text=g->TEXTMappings()->GetMap("audio");
            if (text)
            {

                if ((!strcasecmp(xEvent->Audio(),"mono")) || (!strcasecmp(xEvent->Audio(),"stereo")))
                {
                    description=Add2Description(description,text->Value());
                    description=Add2Description(description,": ");
                    description=Add2Description(description,xEvent->Audio());
                    description=Add2Description(description,"\n");
                }
                else
                {
                    cTEXTMapping *atext=g->TEXTMappings()->GetMap(xEvent->Audio());
                    if (atext)
                    {
                        description=Add2Description(description,text->Value());
                        description=Add2Description(description,": ");
                        description=Add2Description(description,atext->Value());
                        description=Add2Description(description,"\n");
                    }
                }
            }
        }
    }
    if ((what==USE_SEASON) && ((Flags & USE_SEASON)==USE_SEASON))
    {
        if (xEvent->Season())
        {
            cTEXTMapping *text=g->TEXTMappings()->GetMap("season");
            if (text) description=Add2Description(description,text->Value(),
                                                      xEvent->Season());
        }

        if (xEvent->Episode())
        {
            cTEXTMapping *text=g->TEXTMappings()->GetMap("episode");
            if (text) description=Add2Description(description,text->Value(),
                                                      xEvent->Episode());
        }

        if (xEvent->EpisodeOverall())
        {
            cTEXTMapping *text=g->TEXTMappings()->GetMap("episodeoverall");
            if (text) description=Add2Description(description,text->Value(),
                                                      xEvent->EpisodeOverall());
        }
    }

    if ((what==USE_RATING) && ((Flags & USE_RATING)==USE_RATING) &&
            (xEvent->Rating()->Size()))
    {
#if VDRVERSNUM < 10711 && !EPGHANDLER
        Flags|=OPT_RATING_TEXT; // always add to text if we dont have the internal tag!
#endif
        if ((Flags & OPT_RATING_TEXT)==OPT_RATING_TEXT)
        {
            cXMLTVStringList *rating=xEvent->Rating();
            for (int i=0; i<rating->Size(); i++)
            {
                char *rtype=strdup((*rating)[i]);
                if (rtype)
                {
                    char *rval=strchr(rtype,'|');
                    if (rval)
                    {
                        *rval=0;
                        rval++;

                        description=Add2Description(description,rtype);
                        description=Add2Description(description,": ");
                        description=Add2Description(description,rval);
                        description=Add2Description(description,"\n");
                    }
                    free(rtype);
                }
            }
        }
    }

    if ((what==USE_STARRATING) && ((Flags & USE_STARRATING)==USE_STARRATING) &&
            (xEvent->StarRating()->Size()))
    {
        cTEXTMapping *text=g->TEXTMappings()->GetMap("starrating");
        if (text)
        {
            description=Add2Description(description,text->Value());
            description=Add2Description(description,": ");
            cXMLTVStringList *starrating=xEvent->StarRating();
            for (int i=0; i<starrating->Size(); i++)
            {
                char *rtype=strdup((*starrating)[i]);
                if (rtype)
                {
                    char *rval=strchr(rtype,'|');
                    if (rval)
                    {
                        *rval=0;
                        rval++;

                        if (i)
                        {
                            description=Add2Description(description,", ");
                        }
                        if (strcasecmp(rtype,"*"))
                        {
                            description=Add2Description(description,rtype);
                            description=Add2Description(description," ");
                        }
                        description=Add2Description(description,rval);
                    }
                    free(rtype);
                }
            }
            description=Add2Description(description,"\n");
        }
    }

    if ((what==USE_REVIEW) && ((Flags & USE_REVIEW)==USE_REVIEW) &&
            (xEvent->Review()->Size()))
    {
        cTEXTMapping *text=g->TEXTMappings()->GetMap("review");
        if (text)
        {
            cXMLTVStringList *review=xEvent->Review();
            for (int i=0; i<review->Size(); i++)
            {
                description=Add2Description(description,text->Value(),(*review)[i]);
            }
        }
    }

    return description;
}

bool cImport::PutEvent(cEPGSource *Source, sqlite3 *Db, cSchedule* Schedule,
                       cEvent *Event, cXMLTVEvent *xEvent,int Flags)
{
    if (!Source) return false;
    if (!Db) return false;
    if (!xEvent) return false;
    if (!g) return false;

#define CHANGED_NOTHING     0
#define CHANGED_TITLE       1
#define CHANGED_SHORTTEXT   2
#define CHANGED_DESCRIPTION 4

    struct tm tm;
    char from[80];
    char till[80];
    time_t start,end;

    int changed=CHANGED_NOTHING;
    bool append=false;
    bool retcode=false;
    bool added=false;

    if ((Flags & OPT_APPEND)==OPT_APPEND) append=true;

    if (append && !Event)
    {
        if (!Schedule) return false;
        start=xEvent->StartTime();
        end=start+xEvent->Duration();

        /* checking the "space" for our new event */
        cEvent *prev=GetEventBefore(Schedule,start);
        if (prev)
        {
            if (cEvent *next=(cEvent *) prev->Next())
            {
                if (prev->EndTime()==next->StartTime())
                {
                    // ok - no gap
                    localtime_r(&start,&tm);
                    strftime(from,sizeof(from)-1,"%b %d %H:%M",&tm);
                    localtime_r(&end,&tm);
                    strftime(till,sizeof(till)-1,"%b %d %H:%M",&tm);
                    esyslogs(Source,"cannot add '%s'@%s-%s",xEvent->Title(),from,till);

                    time_t pstart=prev->StartTime();
                    time_t pstop=prev->EndTime();
                    localtime_r(&pstart,&tm);
                    strftime(from,sizeof(from)-1,"%b %d %H:%M",&tm);
                    localtime_r(&pstop,&tm);
                    strftime(till,sizeof(till)-1,"%b %d %H:%M",&tm);
                    esyslogs(Source,"found '%s'@%s-%s",prev->Title(),from,till);

                    time_t nstart=next->StartTime();
                    time_t nstop=next->EndTime();
                    localtime_r(&nstart,&tm);
                    strftime(from,sizeof(from)-1,"%b %d %H:%M",&tm);
                    localtime_r(&nstop,&tm);
                    strftime(till,sizeof(till)-1,"%b %d %H:%M",&tm);
                    esyslogs(Source,"found '%s'@%s-%s",next->Title(),from,till);
                    return false;
                }

                if (end>next->StartTime())
                {
                    int diff=(int) difftime(prev->EndTime(),start);
                    if (diff>420)
                    {

                        localtime_r(&start,&tm);
                        strftime(from,sizeof(from)-1,"%b %d %H:%M",&tm);
                        localtime_r(&end,&tm);
                        strftime(till,sizeof(till)-1,"%b %d %H:%M",&tm);
                        esyslogs(Source,"cannot add '%s'@%s-%s",xEvent->Title(),from,till);

                        time_t nstart=next->StartTime();
                        time_t nstop=next->EndTime();
                        localtime_r(&nstart,&tm);
                        strftime(from,sizeof(from)-1,"%b %d %H:%M",&tm);
                        localtime_r(&nstop,&tm);
                        strftime(till,sizeof(till)-1,"%b %d %H:%M",&tm);
                        esyslogs(Source,"found '%s'@%s-%s",next->Title(),from,till);
                        return false;
                    }
                    else
                    {
                        xEvent->SetDuration(xEvent->Duration()-diff);
                    }
                }
            }
            else
            {
                // no next event, check for gaps
                if (prev->EndTime()!=start)
                {
                    tsyslogs(Source,"detected gap of %lis",(long int)(start-prev->EndTime()));
                }
            }

            if (prev->EndTime()>start)
            {
                int diff=(int) difftime(prev->EndTime(),start);
                if (diff>300)
                {
                    localtime_r(&start,&tm);
                    strftime(from,sizeof(from)-1,"%b %d %H:%M",&tm);
                    localtime_r(&end,&tm);
                    strftime(till,sizeof(till)-1,"%b %d %H:%M",&tm);
                    esyslogs(Source,"cannot add '%s'@%s-%s",xEvent->Title(),from,till);

                    time_t pstart=prev->StartTime();
                    time_t pstop=prev->EndTime();
                    localtime_r(&pstart,&tm);
                    strftime(from,sizeof(from)-1,"%b %d %H:%M",&tm);
                    localtime_r(&pstop,&tm);
                    strftime(till,sizeof(till)-1,"%b %d %H:%M",&tm);
                    esyslogs(Source,"found '%s'@%s-%s",prev->Title(),from,till);
                    return false;
                }
                else
                {
                    prev->SetDuration(prev->Duration()-diff);
                }
            }

            if (!xEvent->Duration())
            {
                if (!prev->Duration())
                {
                    prev->SetDuration(start-prev->StartTime());
                }
            }
        }
        /* add event */
        Event=new cEvent(xEvent->EventID());
        if (!Event) return false;
        Event->SetStartTime(start);
        Event->SetDuration(xEvent->Duration());
        Event->SetTitle(xEvent->Title());
        Event->SetVersion(0);
        Event->SetTableID(0);
        Schedule->AddEvent(Event);
        Schedule->Sort();
        added=true;
        if (xEvent->Pics()->Size() && Source->UsePics())
        {
            /* here's a good place to link pictures! */
            LinkPictures(xEvent->Source(),xEvent->Pics(),Event->EventID(),Event->ChannelID());
        }
        if (Source->Trace())
        {
            localtime_r(&start,&tm);
            strftime(from,sizeof(from)-1,"%b %d %H:%M",&tm);
            localtime_r(&end,&tm);
            strftime(till,sizeof(till)-1,"%b %d %H:%M",&tm);
            tsyslogs(Source,"{%5i} adding '%s'/'%s'@%s-%s",xEvent->EventID(),xEvent->Title(),
                     xEvent->ShortText(),from,till);
        }
        retcode=true;
    }

    if (!Event) return false;

    if ((Flags & USE_TITLE)==USE_TITLE)
    {
        if (xEvent->Title() && (strlen(xEvent->Title())>0))
        {
            const char *dp=conv->Convert(xEvent->Title());
            if (!Event->Title() || strcmp(Event->Title(),dp))
            {
                tsyslogs(Source,"{%5i} changing title from '%s' to '%s'",Event->EventID(),Event->Title(),dp);
                Event->SetTitle(dp);
                changed|=CHANGED_TITLE; // title really changed
            }
        }
    }

    if ((Flags & OPT_SEASON_STEXTITLE)==OPT_SEASON_STEXTITLE)
    {
        if (xEvent->AltTitle() && (strlen(xEvent->AltTitle())>0))
        {
            const char *dp=conv->Convert(xEvent->AltTitle());
            if (!Event->Title() || strcmp(Event->Title(),dp))
            {
                tsyslogs(Source,"{%5i} changing title from '%s' to '%s'",Event->EventID(),Event->Title(),dp);
                Event->SetTitle(dp);
                changed|=CHANGED_TITLE; // title really changed
            }
        }
    }

    if (((Flags & USE_SHORTTEXT)==USE_SHORTTEXT) || (append))
    {
        if (xEvent->ShortText() && (strlen(xEvent->ShortText())>0))
        {
            if (!strcasecmp(xEvent->ShortText(),Event->Title()))
            {
                tsyslogs(Source,"{%5i} title and subtitle equal, clearing subtitle",Event->EventID());
                Event->SetShortText(NULL);
            }
            else
            {
                if (!strcmp(xEvent->ShortText(),"@"))
                {
                    if (!Event->ShortText() || strcmp(Event->ShortText(),""))
                    {
                        Event->SetShortText("");
                        changed|=CHANGED_SHORTTEXT; // shorttext really changed
                    }
                }
                else
                {
                    const char *dp=conv->Convert(xEvent->ShortText());
                    if (!Event->ShortText() || strcmp(Event->ShortText(),dp))
                    {
                        Event->SetShortText(dp);
                        changed|=CHANGED_SHORTTEXT; // shorttext really changed
                    }
                }
            }
        }
    }

    if (!append)
    {
        const char *eitdescription=Event->Description();
        if (WasChanged(Event))
        {
            eitdescription=NULL; // we cannot use Event->Description() - it was already changed!
            if (!xEvent->EITDescription()) return false; // no eitdescription in db? -> cannot mix!
        }
        if (!xEvent->EITEventID() || eitdescription)
        {
            if (!xEvent->EITEventID() && xEvent->Pics()->Size() && Source->UsePics())
            {
                /* here's a good place to link pictures! */
                LinkPictures(xEvent->Source(),xEvent->Pics(),Event->EventID(),Event->ChannelID());
            }
            UpdateXMLTVEvent(Source,Db,Event,xEvent,eitdescription);
        }
    }

    char *description=NULL;

    const char *ot=g->Order();
    if (!ot) return false;

    while (*ot)
    {
        if (*ot==',') ot++;
        if (!strncmp(ot,"LOT",3)) description=Add2Description(description,xEvent,Flags,USE_LONGTEXT);
        if (!strncmp(ot,"CRS",3)) description=Add2Description(description,xEvent,Flags,USE_CREDITS);
        if (!strncmp(ot,"CAD",3)) description=Add2Description(description,xEvent,Flags,USE_COUNTRYDATE);
        if (!strncmp(ot,"ORT",3)) description=Add2Description(description,xEvent,Flags,USE_ORIGTITLE);
        if (!strncmp(ot,"CAT",3)) description=Add2Description(description,xEvent,Flags,USE_CATEGORIES);
        if (!strncmp(ot,"VID",3)) description=Add2Description(description,xEvent,Flags,USE_VIDEO);
        if (!strncmp(ot,"AUD",3)) description=Add2Description(description,xEvent,Flags,USE_AUDIO);
        if (!strncmp(ot,"SEE",3)) description=Add2Description(description,xEvent,Flags,USE_SEASON);
        if (!strncmp(ot,"RAT",3)) description=Add2Description(description,xEvent,Flags,USE_RATING);
        if (!strncmp(ot,"STR",3)) description=Add2Description(description,xEvent,Flags,USE_STARRATING);
        if (!strncmp(ot,"REV",3)) description=Add2Description(description,xEvent,Flags,USE_REVIEW);
        ot+=3;
    }

    if (description)
    {
        description=RemoveLastCharFromDescription(description);
        description=AddEOT2Description(description);
        const char *dp=conv->Convert(description);
        if (!Event->Description() || strcasecmp(Event->Description(),dp))
        {
            Event->SetDescription(dp);
            changed|=CHANGED_DESCRIPTION;
        }
        free(description);
    }

#if VDRVERSNUM >= 10711 || EPGHANDLER
    if ((Flags & USE_RATING)==USE_RATING)
    {
        if (xEvent->ParentalRating() && xEvent->ParentalRating()>Event->ParentalRating())
        {
            Event->SetParentalRating(xEvent->ParentalRating());
        }
    }
#endif

#if VDRVERSNUM >= 10712 || EPGHANDLER
    if ((Flags & USE_CONTENT)==USE_CONTENT)
    {
        uchar contents[MaxEventContents];
        for (int i=0; i<MaxEventContents; i++)
        {
            contents[i]=Event->Contents(i);
        }
        cXMLTVStringList *categories=xEvent->Category();
        for (int i=0; i<categories->Size(); i++)
        {
            char *tok,*sp;
            char delim[]=",";
            if ((*categories)[i][0]=='G' && (*categories)[i][1]==' ')
            {
                char *val=strdup(&(*categories)[i][2]);
                if (val)
                {
                    tok=strtok_r(val,delim,&sp);
                    while (tok)
                    {
                        unsigned int hval;
                        if (sscanf(tok,"%2x",&hval)==1)
                        {
                            uchar uval=(uchar) hval;
                            bool found=false;
                            for (int i=0; i<MaxEventContents; i++)
                            {
                                if (contents[i]==uval)
                                {
                                    found=true;
                                    break;
                                }
                            }
                            if (!found)
                            {
                                for (int i=0; i<MaxEventContents; i++)
                                {
                                    if (!contents[i])
                                    {
                                        contents[i]=uval;
                                        break;
                                    }
                                }
                            }

                        }
                        tok=strtok_r(NULL,delim,&sp);
                    }
                }
                free(val);
                Event->SetContents(contents);
            }
        }
    }
#endif

#if VDRVERSNUM < 10726 && (!EPGHANDLER)
    Event->SetTableID(0); // prevent EIT EPG to update this event
#endif

    if (!added && changed)
    {

        if (((changed & CHANGED_DESCRIPTION)==CHANGED_DESCRIPTION) && (WasChanged(Event)==false))
        {
            if (Event->Description()) description=strdup(Event->Description());
            if (description)
            {
                description=AddEOT2Description(description,true);
                tsyslogs(Source,"{%5i} adding EOT to '%s'",Event->EventID(),Event->Title());
                Event->SetDescription(description);
                free(description);
            }
        }

        if (Source->Trace())
        {
            start=Event->StartTime();
            end=Event->EndTime();
            localtime_r(&start,&tm);
            strftime(from,sizeof(from)-1,"%b %d %H:%M",&tm);
            localtime_r(&end,&tm);
            strftime(till,sizeof(till)-1,"%b %d %H:%M",&tm);

            char buf[256]="";
            if ((changed & CHANGED_TITLE)==CHANGED_TITLE) strcat(buf,"title,");
            if ((changed & CHANGED_SHORTTEXT)==CHANGED_SHORTTEXT) strcat(buf,"stext,");
            if ((changed & CHANGED_DESCRIPTION)==CHANGED_DESCRIPTION) strcat(buf,"descr,");
            int len=strlen(buf);
            if (len>0) buf[len-1]=0;

            tsyslogs(Source,"{%5i} changed %s of '%s'/'%s'@%s-%s",Event->EventID(),
                     buf,Event->Title(),Event->ShortText() ? Event->ShortText() : "",
                     from,till);
        }
        retcode=true;
    }
    return retcode;
}

bool cImport::FetchXMLTVEvent(sqlite3_stmt *stmt, cXMLTVEvent *xevent)
{
    if (!stmt) return false;
    if (!xevent) return false;
    xevent->Clear();
    int cols=sqlite3_column_count(stmt);
    for (int col=0; col<cols; col++)
    {
        switch (col)
        {
        case 0:
            xevent->SetChannelID((const char *) sqlite3_column_text(stmt,col));
            break;
        case 1:
            xevent->SetEventID(sqlite3_column_int(stmt,col));
            break;
        case 2:
            xevent->SetStartTime(sqlite3_column_int(stmt,col));
            break;
        case 3:
            xevent->SetDuration(sqlite3_column_int(stmt,col));
            break;
        case 4:
            xevent->SetTitle((const char *) sqlite3_column_text(stmt,col));
            break;
        case 5:
            xevent->SetOrigTitle((const char *) sqlite3_column_text(stmt,col));
            break;
        case 6:
            xevent->SetShortText((const char *) sqlite3_column_text(stmt,col));
            break;
        case 7:
            xevent->SetDescription((const char *) sqlite3_column_text(stmt,col));
            break;
        case 8:
            xevent->SetCountry((const char *) sqlite3_column_text(stmt,col));
            break;
        case 9:
            xevent->SetYear(sqlite3_column_int(stmt,col));
            break;
        case 10:
            xevent->SetCredits((const char *) sqlite3_column_text(stmt,col));
            break;
        case 11:
            xevent->SetCategory((const char *) sqlite3_column_text(stmt,col));
            break;
        case 12:
            xevent->SetReview((const char *) sqlite3_column_text(stmt,col));
            break;
        case 13:
            xevent->SetRating((const char *) sqlite3_column_text(stmt,col));
            break;
        case 14:
            xevent->SetStarRating((const char *) sqlite3_column_text(stmt,col));
            break;
        case 15:
            xevent->SetVideo((const char *) sqlite3_column_text(stmt,col));
            break;
        case 16:
            xevent->SetAudio((const char *) sqlite3_column_text(stmt,col));
            break;
        case 17:
            xevent->SetSeason(sqlite3_column_int(stmt,col));
            break;
        case 18:
            xevent->SetEpisode(sqlite3_column_int(stmt,col));
            break;
        case 19:
            xevent->SetEpisodeOverall(sqlite3_column_int(stmt,col));
            break;
        case 20:
            xevent->SetPics((const char *) sqlite3_column_text(stmt,col));
            break;
        case 21:
            xevent->SetSource((const char *) sqlite3_column_text(stmt,col));
            break;
        case 22:
            xevent->SetEITEventID(sqlite3_column_int(stmt,col));
            break;
        case 23:
            xevent->SetEITDescription((const char *) sqlite3_column_text(stmt,col));
            break;
        case 24:
            xevent->SetAltTitle((const char *) sqlite3_column_text(stmt,col));
            break;
        }
    }
    return true;
}

cXMLTVEvent *cImport::PrepareAndReturn(sqlite3 **db, char *sql)
{
    if (!db) return NULL;
    if (!*db) return NULL;
    if (!sql) return NULL;

    sqlite3_stmt *stmt=NULL;
    int ret=sqlite3_prepare_v2(*db,sql,strlen(sql),&stmt,NULL);
    if (ret!=SQLITE_OK)
    {
        const char *errmsg=sqlite3_errmsg(*db);
        if (errmsg)
        {
            if (strstr(errmsg,"no such column"))
            {
                esyslog("sqlite3: database schema changed, unlinking epg.db!");
                sqlite3_close(*db);
                *db=NULL;
                unlink(g->EPGFile());
            }
            else
            {
                if ((ret==SQLITE_BUSY) || (ret==SQLITE_LOCKED))
                {
                    tsyslog("sqlite3: %i %s (par)",ret,errmsg);
                }
                else
                {
                    esyslog("sqlite3: %i %s (par)",ret,errmsg);
                    tsyslog("sqlite3: %s",sql);
                }
            }
        }
        free(sql);
        return NULL;
    }

    cXMLTVEvent *xevent=NULL;
    if (sqlite3_step(stmt)==SQLITE_ROW)
    {
        xevent = new cXMLTVEvent();
        FetchXMLTVEvent(stmt,xevent);
    }
    sqlite3_finalize(stmt);
    free(sql);
    return xevent;
}

bool cImport::AddShortTextFromEITDescription(cXMLTVEvent *xEvent, const char *EITDescription)
{
    if (!g->EPDir()) return false;
    int season=0,episode=0,episodeoverall=0;
    char *epshorttext=NULL;
    if (!cParse::FetchSeasonEpisode(cep2ascii,cutf2ascii,g->EPDir(),xEvent->Title(),
                                    NULL,EITDescription,
                                    season,episode,episodeoverall,&epshorttext,
                                    NULL)) return false;

    if (epshorttext)
    {
        xEvent->SetShortText(epshorttext);
        free(epshorttext);
    }
    xEvent->SetSeason(season);
    xEvent->SetEpisode(episode);
    xEvent->SetEpisodeOverall(episodeoverall);
    return true;
}

cXMLTVEvent *cImport::AddXMLTVEvent(cEPGSource *Source,sqlite3 *Db, const char *ChannelID, const cEvent *Event,
                                    const char *EITDescription, bool UseEPText)
{
    if (!Db) return NULL;
    if (!Source) return NULL;
    if (!ChannelID) return NULL;
    if (!Event) return NULL;
    if (!g->EPDir()) return NULL;

    int season=0,episode=0,episodeoverall=0;
    char *epshorttext=NULL,*eptitle=NULL;
    if (!cParse::FetchSeasonEpisode(cep2ascii,cutf2ascii,g->EPDir(),Event->Title(),
                                    Event->ShortText(),Event->Description(),
                                    season,episode,episodeoverall,&epshorttext,
                                    &eptitle))
    {
#ifdef VDRDEBUG
        tsyslogs(Source,"no season/episode found for '%s'/'%s'",Event->Title(),Event->ShortText());
#endif
        if (!eptitle)
        {
            if (epshorttext) free(epshorttext);
            return NULL;
        }
        if (!UseEPText)
        {
            if (epshorttext) free(epshorttext);
            if (eptitle) free(eptitle);
            return NULL;
        }
    }

    cXMLTVEvent *xevent = new cXMLTVEvent();
    if (!xevent)
    {
        esyslogs(Source,"out of memory");
        if (epshorttext) free(epshorttext);
        if (eptitle) free(eptitle);
        return NULL;
    }

    if (UseEPText)
    {
        if (eptitle) xevent->SetAltTitle(eptitle);
        if (epshorttext) xevent->SetShortText(epshorttext);
    }
    else
    {
        xevent->SetShortText(Event->ShortText());
    }
    xevent->SetTitle(Event->Title());
    xevent->SetStartTime(Event->StartTime());
    xevent->SetDuration(Event->Duration());
    xevent->SetEventID(Event->EventID());
    xevent->SetEITEventID(Event->EventID());
    if (EITDescription)
    {
        xevent->SetDescription(EITDescription);
        xevent->SetEITDescription(EITDescription);
    }
    xevent->SetSeason(season);
    xevent->SetEpisode(episode);
    xevent->SetEpisodeOverall(episodeoverall);
    if (epshorttext) free(epshorttext);
    if (eptitle) free(eptitle);

    if (!Begin(Source,Db))
    {
        delete xevent;
        return NULL;
    }

    char *isql,*usql;
    xevent->GetSQL(Source->Name(),99,ChannelID,&isql,&usql);
    if (isql && usql)
    {
        char *errmsg;
        int ret=sqlite3_exec(Db,isql,NULL,NULL,&errmsg);
        if (ret!=SQLITE_OK)
        {
            if (ret==SQLITE_CONSTRAINT)
            {
                sqlite3_free(errmsg);
                ret=sqlite3_exec(Db,usql,NULL,NULL,&errmsg);
            }
            if (ret!=SQLITE_OK)
            {
                esyslogs(Source,"sqlite3: %s",errmsg);
                sqlite3_free(errmsg);
                delete xevent;
                return NULL;
            }
        }
        /*
        if (ret==SQLITE_OK)
        {
            tsyslogs(Source,"{%5i} adding '%s'/'%s' to db",xevent->EventID(),
                     xevent->Title(),xevent->ShortText());
        }
        */
    }
    return xevent;
}

bool cImport::UpdateXMLTVEvent(cEPGSource *Source, sqlite3 *Db, cXMLTVEvent *xEvent)
{
    if (!Source) return false;
    if (!Db) return false;
    if (!xEvent) return false;

    if (!Begin(Source,Db)) return false;

    char *sql=NULL;

    if (xEvent->ShortText())
    {
        char *shortdesc=strdup(xEvent->ShortText());
        if (!shortdesc)
        {
            esyslogs(Source,"out of memory");
            return false;
        }

        std::string ed=shortdesc;

        int reps;
        reps=pcrecpp::RE("'").GlobalReplace("''",&ed);
        if (reps)
        {
            shortdesc=(char *) realloc(shortdesc,ed.size()+1);
            strcpy(shortdesc,ed.c_str());
        }

        if (asprintf(&sql,"update epg set season=%li, episode=%li, episodeoverall=%li, shorttext='%s' "
                     " where eventid=%li and src='%s' and channelid='%s'", (long int) xEvent->Season(),
                     (long int) xEvent->Episode(), (long int) xEvent->EpisodeOverall()   ,shortdesc,
                     (long int) xEvent->EventID(),Source->Name(),xEvent->ChannelID())==-1)
        {
            free(shortdesc);
            esyslogs(Source,"out of memory");
            return false;
        }
        free(shortdesc);
    }
    else
    {
        if (asprintf(&sql,"update epg set season=%li, episode=%li, episodeoverall=%li "
                     " where eventid=%li and src='%s' and channelid='%s'", (long int) xEvent->Season(),
                     (long int) xEvent->Episode(), (long int) xEvent->EpisodeOverall(),
                     (long int) xEvent->EventID(),Source->Name(),xEvent->ChannelID())==-1)
        {
            esyslogs(Source,"out of memory");
            return false;
        }
    }

    if (Source->Trace())
    {
        char buf[80]="";
        if (xEvent->ShortText())
        {
            strcat(buf,"shorttext");
            if (xEvent->Season() || xEvent->Episode() || xEvent->EpisodeOverall()) strcat(buf,"/");
        }
        if (xEvent->Season())
        {
            strcat(buf,"season");
            if (xEvent->Episode() || xEvent->EpisodeOverall()) strcat(buf,"/");
        }
        if (xEvent->Episode())
        {
            strcat(buf,"episode");
            if (xEvent->EpisodeOverall()) strcat(buf,"/");
        }
        if (xEvent->EpisodeOverall())
        {
            strcat(buf,"episodeoverall");
        }

        if (xEvent->EITEventID())
        {
            tsyslogs(Source,"{%5i} updating %s of '%s' in db",xEvent->EITEventID(),buf,
                     xEvent->Title());
        }
    }

    char *errmsg;
    if (sqlite3_exec(Db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
    {
        esyslogs(Source,"%s -> %s",sql,errmsg);
        free(sql);
        sqlite3_free(errmsg);
        return false;
    }

    free(sql);
    return true;
}

bool cImport::UpdateXMLTVEvent(cEPGSource *Source, sqlite3 *Db, const cEvent *Event, cXMLTVEvent *xEvent,
                               const char *Description)
{
    if (!Source) return false;
    if (!Db) return false;
    if (!Event) return false;
    if (!xEvent) return false;

    // prevent unnecessary updates
    if (!Description && Event->EventID() && (xEvent->EITEventID()==Event->EventID())) return false;

    if (Description)
    {
        xEvent->SetEITDescription(Description);
    }
    bool eventid=false;
    if (!xEvent->EITEventID())
    {
        xEvent->SetEITEventID(Event->EventID());
        eventid=true;
    }

    if (!Begin(Source,Db)) return false;

    char *sql=NULL;
    if (Description)
    {
        char *eitdescription=strdup(Description);
        if (!eitdescription)
        {
            esyslogs(Source,"out of memory");
            return false;
        }

        std::string ed=eitdescription;

        int reps;
        reps=pcrecpp::RE("'").GlobalReplace("''",&ed);
        if (reps)
        {
            eitdescription=(char *) realloc(eitdescription,ed.size()+1);
            strcpy(eitdescription,ed.c_str());
        }

        if (asprintf(&sql,"update epg set eiteventid=%li, eitdescription='%s' where eventid=%li and "
                     "src='%s' and channelid='%s'",(long int) Event->EventID(),eitdescription,
                     (long int) xEvent->EventID(),Source->Name(),*Event->ChannelID().ToString())==-1)
        {
            free(eitdescription);
            esyslogs(Source,"out of memory");
            return false;
        }
        free(eitdescription);
    }
    else
    {
        if (asprintf(&sql,"update epg set eiteventid=%li where eventid=%li and src='%s' and "
                     "channelid='%s'",(long int) Event->EventID(),(long int) xEvent->EventID(),
                     Source->Name(),*Event->ChannelID().ToString())==-1)
        {
            esyslogs(Source,"out of memory");
            return false;
        }
    }

    if (Source->Trace())
    {
        char buf[80]="";
        if (Description)
        {
            strcat(buf,"eitdescription");
            if (eventid) strcat(buf,"/");
        }
        if (eventid)
        {
            strcat(buf,"eiteventid");
        }

        if (Event->EventID())
        {
            tsyslogs(Source,"{%5i} updating %s of '%s' in db",Event->EventID(),buf,
                     Event->Title());
        }
    }

    char *errmsg;
    if (sqlite3_exec(Db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
    {
        esyslogs(Source,"%s -> %s",sql,errmsg);
        free(sql);
        sqlite3_free(errmsg);
        return false;
    }

    free(sql);
    return true;
}

cXMLTVEvent *cImport::SearchXMLTVEvent(sqlite3 **Db,const char *ChannelID, const cEvent *Event)
{
    if (!Event) return NULL;
    if (!Db) return NULL;
    if (!*Db)
    {
        // we need READWRITE because the epg.db maybe updated later
        if (sqlite3_open_v2(g->EPGFile(),Db,SQLITE_OPEN_READWRITE,NULL)!=SQLITE_OK)
        {
            esyslog("failed to open %s",g->EPGFile());
            *Db=NULL;
            return NULL;
        }
    }

    cXMLTVEvent *xevent=NULL;
    char *sql=NULL;

    int eventTimeDiff=0;
    if (Event->Duration()) eventTimeDiff=Event->Duration()/4;
    if (eventTimeDiff<100) eventTimeDiff=100;
    if (eventTimeDiff>720) eventTimeDiff=720;

    if (asprintf(&sql,"select channelid,eventid,starttime,duration,title,origtitle,shorttext,description," \
                 "country,year,credits,category,review,rating,starrating,video,audio,season,episode," \
                 "episodeoverall,pics,src,eiteventid,eitdescription,alttitle,abs(starttime-%li) as diff from epg where " \
                 " (starttime>=%li and starttime<=%li) and eiteventid=%u and channelid='%s' " \
                 " order by diff,srcidx asc limit 1;",Event->StartTime(),Event->StartTime()-eventTimeDiff,
                 Event->StartTime()+eventTimeDiff,Event->EventID(),ChannelID)==-1)
    {
        esyslog("out of memory");
        return NULL;
    }

    xevent=PrepareAndReturn(Db,sql);
    if (xevent) return xevent;

    bool bUseRawTitle=false;
    if (g->SoundEx())
    {
        char wstr[128];
        if (SoundEx((char *) &wstr,(char *) Event->Title(),0,1)==0)
        {
            bUseRawTitle=true;
        }
        else
        {

            if (asprintf(&sql,"select channelid,eventid,starttime,duration,title,origtitle,shorttext,description," \
                         "country,year,credits,category,review,rating,starrating,video,audio,season,episode," \
                         "episodeoverall,pics,src,eiteventid,eitdescription,alttitle,abs(starttime-%li) as diff from epg where " \
                         " (starttime>=%li and starttime<=%li) and soundex(title)='%s' and channelid='%s' " \
                         " order by diff,srcidx asc limit 1;",Event->StartTime(),Event->StartTime()-eventTimeDiff,
                         Event->StartTime()+eventTimeDiff,wstr,ChannelID)==-1)
            {
                esyslog("out of memory");
                return NULL;
            }
        }
    }
    else
    {
        bUseRawTitle=true;
    }

    if (bUseRawTitle)
    {
        char *sqltitle=strdup(Event->Title());
        if (!sqltitle)
        {
            esyslog("out of memory");
            return NULL;
        }

        std::string st=sqltitle;

        int reps;
        reps=pcrecpp::RE("'").GlobalReplace("''",&st);
        if (reps)
        {
            char *tmp_sqltitle=(char *) realloc(sqltitle,st.size()+1);
            if (tmp_sqltitle)
            {
                sqltitle=tmp_sqltitle;
                strcpy(sqltitle,st.c_str());
            }
        }

        if (asprintf(&sql,"select channelid,eventid,starttime,duration,title,origtitle,shorttext,description," \
                     "country,year,credits,category,review,rating,starrating,video,audio,season,episode," \
                     "episodeoverall,pics,src,eiteventid,eitdescription,abs(starttime-%li) as diff from epg where " \
                     " (starttime>=%li and starttime<=%li) and title='%s' and channelid='%s' " \
                     " order by diff,srcidx asc limit 1;",Event->StartTime(),Event->StartTime()-eventTimeDiff,
                     Event->StartTime()+eventTimeDiff,sqltitle,ChannelID)==-1)
        {
            free(sqltitle);
            esyslog("out of memory");
            return NULL;
        }
        free(sqltitle);
    }

    return PrepareAndReturn(Db,sql);
}

bool cImport::Begin(cEPGSource *Source, sqlite3 *Db)
{
    if (!Source) return false;
    if (!Db) return false;
    if (!pendingtransaction)
    {
        char *errmsg;
        if (sqlite3_exec(Db,"BEGIN",NULL,NULL,&errmsg)!=SQLITE_OK)
        {
            esyslogs(Source,"sqlite3: BEGIN -> %s",errmsg);
            sqlite3_free(errmsg);
            return false;
        }
        else
        {
            pendingtransaction=true;
        }
    }
    return true;
}

bool cImport::Commit(cEPGSource *Source, sqlite3 *Db)
{
    if (!Db) return false;
    if (pendingtransaction)
    {
        char *errmsg;
        if (sqlite3_exec(Db,"COMMIT",NULL,NULL,&errmsg)!=SQLITE_OK)
        {
            if (Source)
            {
                esyslogs(Source,"sqlite3: COMMIT -> %s",errmsg);
            }
            else
            {
                esyslog("sqlite3: COMMIT -> %s",errmsg);
            }
            sqlite3_free(errmsg);
            return false;
        }
        pendingtransaction=false;
    }
    return true;
}

int cImport::Process(cEPGSource *Source, cEPGExecutor &myExecutor)
{
    if (!Source) return 0;
    time_t begin=time(NULL);
    time_t end=begin+(Source->DaysInAdvance()*86400);
#if VDRVERSNUM < 10726 && (!EPGHANDLER)
    time_t endoneday=begin+86400;
#endif

    const cSchedules *schedules=NULL;
    int l=0;
#if VDRVERSNUM<20301
    Timers.IncBeingEdited(); // prevent Timers.DeleteExpired() to execute
    cSchedulesLock *schedulesLock=NULL;

    while (l<300)
    {
        if (schedulesLock) delete schedulesLock;
        schedulesLock = new cSchedulesLock(true,200); // wait up to 60 secs for lock!
        schedules = cSchedules::Schedules(*schedulesLock);
        if (!myExecutor.StillRunning())
        {
            delete schedulesLock;
            Timers.DecBeingEdited();
            isyslogs(Source,"request to stop from vdr");
            return 0;
        }
        if (schedules) break;
        l++;
    }
#else
    cStateKey StateKey;
    while (l<300)
    {
        schedules=cSchedules::GetSchedulesWrite(StateKey,200);
        if (!myExecutor.StillRunning())
        {
            if (schedules) StateKey.Remove();
            isyslogs(Source,"request to stop from vdr");
            return 0;
        }
        if (schedules) break;
        l++;
    }
#endif

    if (!schedules)
    {
        esyslogs(Source,"failed to get schedules lock");
        return 141;
    }

    dsyslogs(Source,"importing from db");
    sqlite3 *db=NULL;
    if (sqlite3_open_v2(g->EPGFile(),&db,SQLITE_OPEN_READWRITE,NULL)!=SQLITE_OK)
    {
        esyslogs(Source,"failed to open %s",g->EPGFile());
#if VDRVERSNUM<20301
        delete schedulesLock;
        Timers.DecBeingEdited();
#else
        StateKey.Remove();
#endif
        return 141;
    }

    char *sql;
    if (asprintf(&sql,"select channelid,eventid,starttime,duration,title,origtitle,shorttext,description," \
                 "country,year,credits,category,review,rating,starrating,video,audio,season,episode,episodeoverall," \
                 "pics,src,eiteventid,eitdescription from epg where (starttime > %li or " \
                 " (starttime + duration) > %li) and (starttime + duration) < %li "\
                 " and src='%s' order by channelid,starttime;",begin,begin,end,Source->Name())==-1)
    {
        sqlite3_close(db);
        esyslogs(Source,"out of memory");
#if VDRVERSNUM<20301
        delete schedulesLock;
        Timers.DecBeingEdited();
#else
        StateKey.Remove();
#endif
        return 134;
    }

    sqlite3_stmt *stmt;
    int ret=sqlite3_prepare_v2(db,sql,strlen(sql),&stmt,NULL);
    if (ret!=SQLITE_OK)
    {
        esyslogs(Source,"%i %s (p)",ret,sqlite3_errmsg(db));
        sqlite3_close(db);
        free(sql);
#if VDRVERSNUM<20301
        delete schedulesLock;
        Timers.DecBeingEdited();
#else
        StateKey.Remove();
#endif
        return 141;
    }
    free(sql);

    int lerr=0;
    int cnt=0;
    char *lastChannelID=NULL;
    int flags=0,hint=0;
    bool addevents=false;
    cSchedule* schedule=NULL;
    for (;;)
    {
        if (sqlite3_step(stmt)==SQLITE_ROW)
        {
            cXMLTVEvent xevent;
            if (FetchXMLTVEvent(stmt,&xevent))
            {
                if (!lastChannelID || strcmp(lastChannelID,xevent.ChannelID()))
                {
                    cEPGMapping *map=g->EPGMappings()->GetMap(tChannelID::FromString(xevent.ChannelID()));
                    if (!map)
                    {
                        if (lerr!=IMPORT_NOMAPPING)
                            esyslogs(Source,"no mapping for channelid %s",xevent.ChannelID());
                        lerr=IMPORT_NOMAPPING;
                        if (lastChannelID)
                        {
                            free(lastChannelID);
                            lastChannelID=NULL;
                        }
                        continue;
                    }
                    flags=map->Flags();

                    bool addevents=false;
                    if ((flags & OPT_APPEND)==OPT_APPEND) addevents=true;

#if VDRVERSNUM>=20301
                    cStateKey StateKeyChan;
                    const cChannels *Channels=cChannels::GetChannelsRead(StateKeyChan);
                    if (Channels)
                    {
                        const cChannel *channel=Channels->GetByChannelID(tChannelID::FromString(xevent.ChannelID()));
#else

                    cChannel *channel=Channels.GetByChannelID(tChannelID::FromString(xevent.ChannelID()));
#endif
                        if (!channel)
                        {
                            if (lerr!=IMPORT_NOCHANNEL)
                                esyslogs(Source,"channel %s not found in channels.conf",
                                         xevent.ChannelID());
                            lerr=IMPORT_NOCHANNEL;
                            if (lastChannelID)
                            {
                                free(lastChannelID);
                                lastChannelID=NULL;
                            }
#if VDRVERSNUM>=20301
                            StateKeyChan.Remove();
#endif
                            continue;
                        }

                        schedule = (cSchedule *) schedules->GetSchedule(channel,addevents);
                        if (!schedule)
                        {
                            if (lerr!=IMPORT_NOSCHEDULE)
                                esyslogs(Source,"cannot get schedule for channel %s%s",
                                         channel->Name(),addevents ? "" : " - try add option");
                            lerr=IMPORT_NOSCHEDULE;
                            if (lastChannelID)
                            {
                                free(lastChannelID);
                                lastChannelID=NULL;
                            }
#if VDRVERSNUM>=20301
                            StateKeyChan.Remove();
#endif
                            continue;
                        }
                        if (lastChannelID) free(lastChannelID);
                        lastChannelID=strdup(xevent.ChannelID());
                        hint=0;
#if VDRVERSNUM>=20301
                        StateKeyChan.Remove();
                    }
#endif
                }

                cEvent *event=SearchVDREvent(Source, schedule, &xevent, addevents, hint);

                if (!addevents)
                {
                    if (event)
                    {
                        hint=(int)(event->StartTime()+event->Duration())-(int)(xevent.StartTime()+xevent.Duration());
                    }
                    else
                    {
                        hint=0;
                    }
                }
                else
                {
                    if (event && (event->EventID() != xevent.EventID()))
                    {
                        tsyslogs(Source,"{%5i} changing existing eventid to {%5i}",event->EventID(),xevent.EventID());
                        event->SetEventID(xevent.EventID());
                        event->SetVersion(0);
                        event->SetTableID(0);
                    }
                }

#if VDRVERSNUM < 10726 && (!EPGHANDLER)
                if ((!addevents) && (xevent.StartTime()>endoneday)) continue;
#endif
                if (PutEvent(Source, db, schedule, event, &xevent, flags))
                {
#if VDRVERSNUM>=20301
                    schedule->SetModified();
#else
                    schedules->SetModified(schedule);
#endif
                    cnt++;
                }
            }
        }
        else
        {
            break;
        }
    }

    if (Commit(Source,db))
    {
        if (cnt)
        {
            if (!lerr)
            {
                isyslogs(Source,"processed %i vdr events",cnt);
            }
            else
            {
                isyslogs(Source,"processed %i vdr events - see ERRORs above!",cnt);
            }
        }
        else
        {
            if (!lerr)
            {
                isyslogs(Source,"processed no vdr events - all up to date?");
            }
            else
            {
                isyslogs(Source,"processed no vdr events - see ERRORs above!");
            }
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
#if VDRVERSNUM<20301
    delete schedulesLock;
    Timers.SetEvents();
    Timers.DecBeingEdited();
#else
    StateKey.Remove();
#endif
    return 0;
}

bool cImport::DBExists()
{
    if (!g->EPGFile()) return true; // is this safe?
    struct stat statbuf;
    if (stat(g->EPGFile(),&statbuf)==-1) return false; // no database
    if (!statbuf.st_size) return false; // no database
    return true;
}

cImport::cImport(cGlobals *Global)
{
    g=Global;
    pendingtransaction=false;
    conv = new cCharSetConv("UTF-8",g->Codeset());

    if (Global->EPDir())
    {
        cep2ascii=iconv_open("ASCII//TRANSLIT",Global->EPCodeset());
        cutf2ascii=iconv_open("ASCII//TRANSLIT","UTF-8");
    }
    else
    {
        cep2ascii=(iconv_t) -1;
        cutf2ascii=(iconv_t) -1;
    }
}

cImport::~cImport()
{
    if (cep2ascii!=(iconv_t) -1) iconv_close(cep2ascii);
    if (cutf2ascii!=(iconv_t) -1) iconv_close(cutf2ascii);
    delete conv;
}
