/*
 * parse.cpp: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <locale.h>
#include <langinfo.h>
#include <time.h>
#include <pwd.h>
#include <iconv.h>
#include <vdr/timers.h>
#include <vdr/tools.h>
#include <sqlite3.h>

#include "xmltv2vdr.h"
#include "parse.h"
#include "debug.h"

// -------------------------------------------------------

time_t cParse::ConvertXMLTVTime2UnixTime(char *xmltvtime)
{
    time_t offset=0;
    if (!xmltvtime) return (time_t) 0;
    char *withtz=strchr(xmltvtime,' ');
    int len;
    if (withtz)
    {
        len=strlen(xmltvtime)-(withtz-xmltvtime)-1;
        *withtz=':';
        if ((withtz[1]=='+') || (withtz[1]=='-'))
        {
            if (len==5)
            {
                int val=atoi(&withtz[1]);
                int h=val/100;
                int m=val-(h*100);
                offset=h*3600+m*60;
                setenv("TZ",":UTC",1);
            }
            else
            {
                setenv("TZ",":UTC",1);
            }
        }
        else
        {
            if (len>2)
            {
                setenv("TZ",withtz,1);
            }
            else
            {
                setenv("TZ",":UTC",1);
            }
        }
    }
    else
    {
        withtz=&xmltvtime[strlen(xmltvtime)];
        setenv("TZ",":UTC",1);
    }
    tzset();

    len=withtz-xmltvtime;
    if (len<4)
    {
        unsetenv("TZ");
        tzset();
        return (time_t) 0;
    }
    len-=2;
    char fmt[]="%Y%m%d%H%M%S";
    fmt[len]=0;

    struct tm tm;
    memset(&tm,0,sizeof(tm));
    if (!strptime(xmltvtime,fmt,&tm))
    {
        unsetenv("TZ");
        tzset();
        return (time_t) 0;
    }
    if (tm.tm_mday==0) tm.tm_mday=1;
    time_t ret=mktime(&tm);
    ret-=offset;
    unsetenv("TZ");
    tzset();
    return ret;
}

void cParse::RemoveNonAlphaNumeric(char *String, bool InDescription)
{
    if (!String) return;

    // remove " Teil " (special for .episodes files)
    int len=strlen(String);
    char *p=strstr(String," Teil ");
    if (!p) p=strstr(String,"(Teil ");
    if (p)
    {
        memmove(p,p+6,len-6);
    }

    len=strlen(String);
    p=String;
    // cut off " Folge XX" at end
    p=strstr(String," Folge ");
    if (p) *p=0;

    bool bCutNumbers=false;
    len=strlen(String);
    p=String;
    // cut off "Folge XX" at start
    if (!strncmp(String,"Folge ",6))
    {
        memmove(p,p+6,len-6);
        String[len-6]=0;
        bCutNumbers=true;
    }

    if (InDescription || bCutNumbers)
    {
        // remove leading numbers (inkl. roman numerals)
        len=strlen(String);
        p=String;
        while (*p)
        {
            // 0x30 - 0x39
            if (((*p>=0x30) && (*p<=0x39)) || (*p=='I') || (*p=='V') || (*p=='X') || (*p=='/'))
            {
                memmove(p,p+1,len);
                len--;
                continue;
            }
            else
            {
                break;
            }
        }
    }

    // remove non alphanumeric characters
    len=strlen(String);
    p=String;
    int pos=0;
    while (*p)
    {
        // 0x30 - 0x39
        // 0x41 - 0x5A
        // 0x61 - 0x7A
        if ((*p<0x30) || (*p>0x7a) || (*p>0x39 && *p<0x41) || (*p>0x5A && *p< 0x61))
        {
            memmove(p,p+1,len-pos);
            len--;
            continue;
        }
        if ((*p=='i') && (*(p+1) && *(p+1)=='e'))
        {
            memmove(p,p+1,len-pos);
            len--;
            *p='y';
            continue;
        }
        p++;
        pos++;
    }

    return;
}

bool cParse::FetchSeasonEpisode(iconv_t cEP2ASCII, iconv_t cUTF2ASCII, const char *EPDir,
                                const char *Title, const char *ShortText, const char *Description,
                                int &Season, int &Episode, int &EpisodeOverall, char **EPShortText,
                                char **EPTitle)
{
    EpisodeOverall=0;

    // Title and ShortText are always UTF8 !
    if (!EPDir) return false;
    if (!Title) return false;
    if (cEP2ASCII==(iconv_t) -1) return false;
    if (cUTF2ASCII==(iconv_t) -1) return false;

    DIR *dir=opendir(EPDir);
    if (!dir) return false;
    struct dirent dirent_buf,*dirent;
    char *fTitle=NULL;
    for (;;)
    {
        if (readdir_r(dir,&dirent_buf,&dirent)!=0) break;
        if (!dirent) break;
        if (dirent->d_name[0]=='.') continue;
        char *pt=strrchr(dirent->d_name,'.');
        if (pt) *pt=0;
        int dlen=strlen(dirent->d_name);
        if (!strncasecmp(dirent->d_name,Title,dlen))
        {
            int tlen=strlen(Title);
            if (tlen>dlen)
            {
                if (Title[dlen]==32)
                {
                    if (fTitle) free(fTitle);
                    fTitle=strdup(dirent->d_name);
                }
            }
            else
            {
                if (fTitle) free(fTitle);
                fTitle=strdup(dirent->d_name);
                break;
            }
        }
    }
    closedir(dir);

    int f_season=Season,f_episode=Episode;
    size_t slen;
    if (ShortText)
    {
        slen=strlen(ShortText);
        if ((slen<=7) && (ShortText[0]=='S') && (isdigit(ShortText[1])!=0) &&
                (isdigit(ShortText[2])!=0) && (ShortText[3]=='E') && (isdigit(ShortText[4])!=0) &&
                (isdigit(ShortText[5])!=0))
        {
            // special SxxExx-Format
            char buf[5];
            buf[0]=ShortText[1];
            buf[1]=ShortText[2];
            buf[2]=0;
            f_season=atoi(buf);
            buf[0]=ShortText[4];
            buf[1]=ShortText[5];
            if (slen==7)
            {
                buf[2]=ShortText[6];
                buf[3]=0;
            }
            else
            {
                buf[2]=0;
            }
            f_episode=atoi(buf);
        }
    }

    if (!fTitle)
    {
        if ((f_season>0) && (f_episode>0))
        {
            if (EPShortText)
            {
                if (*EPShortText) free(*EPShortText);
                *EPShortText=strdup("@");
            }
            Season=f_season;
            Episode=f_episode;
            return true;
        }
        return false;
    }

    char *epfile=NULL;
    if (asprintf(&epfile,"%s/%s.episodes",EPDir,fTitle)==-1)
    {
        free(fTitle);
        return false;
    }

    FILE *f=fopen(epfile,"r");
    if (!f)
    {
        free(epfile);
        free(fTitle);
        return false;
    }

    char dname[2048]="";
    if (readlink(epfile,dname,sizeof(dname)-1)!=-1)
    {
        char *ls=strrchr(dname,'/');
        if (ls)
        {
            ls++;
            memmove(dname,ls,strlen(ls)+1);
        }
        char *pt=strrchr(dname,'.');
        if (pt)
        {
            *pt=0;
        }
        else
        {
            dname[0]=0;
        }
    }
    else
    {
        dname[0]=0;
    }

    if (dname[0]==0) strn0cpy(dname,fTitle,sizeof(dname)-1);
    free(fTitle);

    if (EPTitle && strcasecmp(Title,dname)) *EPTitle=strdup(dname);

    if ((!ShortText) && (!Description))
    {
        fclose(f);
        free(epfile);
        return false;
    }
    if (!ShortText)
    {
        slen=strlen(Description);
        if (slen>60) slen=60;
        if (Season>0) f_season=Season;
        if (Episode>0) f_episode=Episode;
    }
    if (!slen)
    {
        fclose(f);
        free(epfile);
        return false;
    }

    size_t dlen=4*slen;
    char *dshorttext=(char *) calloc(dlen,1);
    if (!dshorttext)
    {
        fclose(f);
        free(epfile);
        return false;
    }
    char *FromPtr=(char *)(ShortText ? ShortText : Description);
    char *ToPtr=(char *) dshorttext;

    if (iconv(cUTF2ASCII,&FromPtr,&slen,&ToPtr,&dlen)==(size_t) -1)
    {
        tsyslog("failed to convert '%s'->'%s' (1)",ShortText,dshorttext);
        free(dshorttext);
        fclose(f);
        free(epfile);
        return false;
    }

    RemoveNonAlphaNumeric(dshorttext,!ShortText);

    if (!strlen(dshorttext))
    {
        strn0cpy(dshorttext,ShortText ? ShortText : Description,slen); // ok lets try with the original text
        tsyslog("Warning: removed all characters, now using '%s'",dshorttext);
    }
    if (!ShortText)
    {
        tsyslog("trying to find shorttext for '%s', with '%s'",Title,dshorttext);
    }

#undef DEBCMP

#ifdef DEBCMP
    tsyslog("dshorttext=%s",dshorttext);
#endif

    char *line=NULL;
    size_t length;
    bool found=false;
    if (EPShortText) *EPShortText=NULL;
    size_t charlen=0;
    int tmpSeason=-1,tmpEpisode=-1,tmpEpisodeOverall=-1;
    while (getline(&line,&length,f)!=-1)
    {
        if (line[0]=='#') continue;
        char epshorttext[256]="";
        if (sscanf(line,"%3d\t%3d\t%5d\t%255c",&Season,&Episode,&EpisodeOverall,epshorttext)==4)
        {
            char depshorttext[1024]="";
            char *lf=strchr(epshorttext,'\n');
            if (lf) *lf=0;
            char *tab=strchr(epshorttext,'\t');
            if (tab) *tab=0;
            slen=strlen(epshorttext);
            dlen=sizeof(depshorttext);
            FromPtr=(char *) epshorttext;
            ToPtr=(char *) depshorttext;
            if (iconv(cEP2ASCII,&FromPtr,&slen,&ToPtr,&dlen)!=(size_t) -1)
            {
                RemoveNonAlphaNumeric(depshorttext);
                if (!strlen(depshorttext))
                {
                    strcpy(depshorttext,epshorttext); // ok lets try with the original text
                }
#ifdef DEBCMP
                tsyslog("depshorttext=%s",depshorttext);
#endif
                if (!strcasecmp(dshorttext,depshorttext))
                {
                    // exact match
                    if (EPShortText)
                    {
                        if (*EPShortText) free(*EPShortText);
                        *EPShortText=strdup(epshorttext);
                    }
                    found=true;
                    break;
                }

                dlen=strlen(depshorttext);
                if (!strncasecmp(dshorttext,depshorttext,dlen))
                {
                    // not exact match -> maybe better match available?
                    if (dlen>charlen)
                    {
                        if (EPShortText)
                        {
                            if (*EPShortText) free(*EPShortText);
                            *EPShortText=strdup(epshorttext);
                            tmpSeason=Season;
                            tmpEpisode=Episode;
                            tmpEpisodeOverall=EpisodeOverall;
                        }
                        charlen=dlen;
                        found=true;
                    }
                }

                if ((f_season==Season) && (f_episode==Episode))
                {
                    if (!strcasecmp(epshorttext,"n.n."))
                    {
                        if (EPShortText)
                        {
                            if (*EPShortText) free(*EPShortText);
                            *EPShortText=strdup("@");
                        }
                        isyslog("failed to find '%s' for '%s' in eplists",ShortText,Title);
                    }
                    else
                    {
                        if (EPShortText)
                        {
                            if (*EPShortText) free(*EPShortText);
                            *EPShortText=strdup(epshorttext);
                        }
                    }
                    found=true;
                    break;
                }
            }
            else
            {
                tsyslog("failed to convert '%s'->'%s' (2)",epshorttext,depshorttext);
            }
        }
        else
        {
            tsyslog("failed to parse '%s' in '%s'",line,dirent->d_name);
        }
    }
    if (tmpEpisode!=-1)
    {
        Season=tmpSeason;
        Episode=tmpEpisode;
        EpisodeOverall=tmpEpisodeOverall;
    }

    if (!found)
    {
        Season=0;
        Episode=0;
        if (ShortText)
        {
            isyslog("failed to find '%s' for '%s' in eplists",ShortText,Title);
            if ((f_season>0) && (f_episode>0))
            {
                if (EPShortText)
                {
                    if (*EPShortText) free(*EPShortText);
                    *EPShortText=strdup("@");
                }
                found=true;
            }
        }
    }
    if (found)
    {
#ifdef DEBCMP
        tsyslog("found shorttext '%s'",*EPShortText);
#endif
        if (!ShortText)
        {
            tsyslog("found shorttext '%s' with description of '%s'",*EPShortText,Title);
        }
    }
    if (line) free(line);
    fclose(f);

    free(dshorttext);
    free(epfile);
    return found;
}

bool cParse::FetchEvent(xmlNodePtr enode, bool useeptext)
{
    char *slang=getenv("LANG");
    xmlNodePtr node=enode->xmlChildrenNode;
    while (node)
    {
        if (node->type==XML_COMMENT_NODE)
        {
            if (const xmlChar *pid=xmlStrstr(node->content,(const xmlChar *) "pid"))
            {
                char *eq=strchr((char *) pid,'=');
                if (eq)
                {
                    xevent.SetEventID((tEventID) atol(eq+1));
                }
            }
            if (const xmlChar *content=xmlStrstr(node->content,(const xmlChar *) "content"))
            {
                char *eq=strchr((char *) content,'=');
                if (eq)
                {
                    xevent.AddCategory(eq+1);
                }
            }
        }
        if (node->type==XML_ELEMENT_NODE)
        {
            if ((!xmlStrcasecmp(node->name, (const xmlChar *) "title")))
            {
                xmlChar *lang=xmlGetProp(node,(const xmlChar *) "lang");
                xmlChar *content=xmlNodeListGetString(node->doc,node->xmlChildrenNode,1);
                if (content)
                {
                    if (lang && slang && !xmlStrncasecmp(lang, (const xmlChar *) slang,2))
                    {
                        xevent.SetTitle((const char *) content);
                    }
                    else
                    {
                        if (!xevent.HasTitle())
                        {
                            xevent.SetTitle((const char *) content);
                        }
                        else
                        {
                            xevent.SetOrigTitle((const char *) content);
                        }
                    }
                    xmlFree(content);
                }
                if (lang) xmlFree(lang);
            }
            else if ((!xmlStrcasecmp(node->name, (const xmlChar *) "sub-title")))
            {
                // what to do with attribute lang?
                xmlChar *content=xmlNodeListGetString(node->doc,node->xmlChildrenNode,1);
                if (content)
                {
                    xevent.SetShortText((const char *) content);
                    xmlFree(content);
                }
            }
            else if ((!xmlStrcasecmp(node->name, (const xmlChar *) "desc")))
            {
                // what to do with attribute lang?
                xmlChar *content=xmlNodeListGetString(node->doc,node->xmlChildrenNode,1);
                if (content)
                {
                    xevent.AddDescription((const char *) content);
                    xmlFree(content);
                }
            }
            else if ((!xmlStrcasecmp(node->name, (const xmlChar *) "credits")))
            {
                xmlNodePtr vnode=node->xmlChildrenNode;
                while (vnode)
                {
                    if (vnode->type==XML_ELEMENT_NODE)
                    {
                        if ((!xmlStrcasecmp(vnode->name, (const xmlChar *) "actor")))
                        {
                            xmlChar *content=xmlNodeListGetString(vnode->doc,vnode->xmlChildrenNode,1);
                            if (content)
                            {
                                xmlChar *arole=xmlGetProp(node,(const xmlChar *) "actor role");
                                xevent.AddCredits((const char *) vnode->name,(const char *) content,(const char *) arole);
                                if (arole) xmlFree(arole);
                                xmlFree(content);
                            }
                        }
                        else
                        {
                            xmlChar *content=xmlNodeListGetString(vnode->doc,vnode->xmlChildrenNode,1);
                            if (content)
                            {
                                xevent.AddCredits((const char *) vnode->name,(const char *) content);
                                xmlFree(content);
                            }
                        }
                    }
                    vnode=vnode->next;
                }
            }
            else if ((!xmlStrcasecmp(node->name, (const xmlChar *) "date")))
            {
                xmlChar *content=xmlNodeListGetString(node->doc,node->xmlChildrenNode,1);
                if (content)
                {
                    xevent.SetYear(atoi((const char *) content));
                    xmlFree(content);
                }
            }
            else if ((!xmlStrcasecmp(node->name, (const xmlChar *) "category")))
            {
                // what to do with attribute lang?
                xmlChar *content=xmlNodeListGetString(node->doc,node->xmlChildrenNode,1);
                if (content)
                {
                    if (isdigit(content[0]))
                    {
                        if (!xevent.EventID())
                            xevent.SetEventID((tEventID) atol((const char *) content));
                    }
                    else
                    {
                        xevent.AddCategory((const char *) content);
                    }
                    xmlFree(content);
                }
            }
            else if ((!xmlStrcasecmp(node->name, (const xmlChar *) "country")))
            {
                xmlChar *content=xmlNodeListGetString(node->doc,node->xmlChildrenNode,1);
                if (content)
                {
                    xevent.SetCountry((const char *) content);
                    xmlFree(content);
                }
            }
            else if ((!xmlStrcasecmp(node->name, (const xmlChar *) "video")))
            {
                xmlNodePtr vnode=node->xmlChildrenNode;
                while (vnode)
                {
                    if (vnode->type==XML_ELEMENT_NODE)
                    {
                        if ((!xmlStrcasecmp(vnode->name, (const xmlChar *) "colour")))
                        {
                            xmlChar *content=xmlNodeListGetString(vnode->doc,vnode->xmlChildrenNode,1);
                            if (content)
                            {
                                xevent.AddVideo("colour",(const char *) content);
                                xmlFree(content);
                            }
                        }
                        if ((!xmlStrcasecmp(vnode->name, (const xmlChar *) "aspect")))
                        {
                            xmlChar *content=xmlNodeListGetString(vnode->doc,vnode->xmlChildrenNode,1);
                            if (content)
                            {
                                xevent.AddVideo("aspect",(const char *) content);
                                xmlFree(content);
                            }
                        }
                        if ((!xmlStrcasecmp(vnode->name, (const xmlChar *) "quality")))
                        {
                            xmlChar *content=xmlNodeListGetString(vnode->doc,vnode->xmlChildrenNode,1);
                            if (content)
                            {
                                xevent.AddVideo("quality",(const char *) content);
                                xmlFree(content);
                            }
                        }

                    }
                    vnode=vnode->next;
                }
            }
            else if ((!xmlStrcasecmp(node->name, (const xmlChar *) "audio")))
            {
                xmlNodePtr vnode=node->xmlChildrenNode;
                while (vnode)
                {
                    if (vnode->type==XML_ELEMENT_NODE)
                    {
                        if ((!xmlStrcasecmp(vnode->name, (const xmlChar *) "stereo")))
                        {
                            xmlChar *content=xmlNodeListGetString(vnode->doc,vnode->xmlChildrenNode,1);
                            if (content)
                            {
                                content=(xmlChar*)strreplace((char *)content," ","");
                                xevent.SetAudio((const char *) content);
                                xmlFree(content);
                            }
                        }
                    }
                    vnode=vnode->next;
                }
            }
            else if ((!xmlStrcasecmp(node->name, (const xmlChar *) "rating")))
            {
                xmlChar *system=xmlGetProp(node,(const xmlChar *) "system");
                if (system)
                {
                    xmlNodePtr vnode=node->xmlChildrenNode;
                    while (vnode)
                    {
                        if (vnode->type==XML_ELEMENT_NODE)
                        {
                            if ((!xmlStrcasecmp(vnode->name, (const xmlChar *) "value")))
                            {
                                xmlChar *content=xmlNodeListGetString(vnode->doc,vnode->xmlChildrenNode,1);
                                if (content)
                                {
                                    xevent.AddRating((const char *) system,(const char *) content);
                                    xmlFree(content);
                                }
                            }
                        }
                        vnode=vnode->next;
                    }
                    xmlFree(system);
                }
            }
            else if ((!xmlStrcasecmp(node->name, (const xmlChar *) "star-rating")))
            {
                xmlChar *system=xmlGetProp(node,(const xmlChar *) "system");
                xmlNodePtr vnode=node->xmlChildrenNode;
                while (vnode)
                {
                    if (vnode->type==XML_ELEMENT_NODE)
                    {
                        if ((!xmlStrcasecmp(vnode->name, (const xmlChar *) "value")))
                        {
                            xmlChar *content=xmlNodeListGetString(vnode->doc,vnode->xmlChildrenNode,1);
                            if (content)
                            {
                                xevent.AddStarRating((const char *) system,(const char *) content);
                                xmlFree(content);
                            }
                        }
                    }
                    vnode=vnode->next;
                }
                if (system) xmlFree(system);
            }
            else if ((!xmlStrcasecmp(node->name, (const xmlChar *) "review")))
            {
                xmlChar *type=xmlGetProp(node,(const xmlChar *) "type");
                if (type && !xmlStrcasecmp(type, (const xmlChar *) "text"))
                {
                    xmlChar *content=xmlNodeListGetString(node->doc,node->xmlChildrenNode,1);
                    if (content)
                    {
                        xevent.AddReview((const char *) content);
                        xmlFree(content);
                    }
                    xmlFree(type);
                }
            }
            else if ((!xmlStrcasecmp(node->name, (const xmlChar *) "icon")))
            {
                xmlChar *src=xmlGetProp(node,(const xmlChar *) "src");
                if (src)
                {
                    const xmlChar *f=xmlStrstr(src,(const xmlChar *) "://");
                    if (f)
                    {
                        // url: skip scheme and scheme-specific-part
                        f+=3;
                    }
                    else
                    {
                        // just try it
                        f=src;
                    }
                    struct stat statbuf;
                    if (stat((const char *) f,&statbuf)!=-1)
                    {
                        char *file=strrchr((char *) f,'/');
                        if (file)
                        {
                            file++;
                            xevent.AddPics(file);
                        }
                    }
                    xmlFree(src);
                }

            }
            else if ((!xmlStrcasecmp(node->name, (const xmlChar *) "episode-num")))
            {
                xmlChar *system=xmlGetProp(node,(const xmlChar *) "system");
                if (system && !xmlStrcasecmp(system,(const xmlChar *) "xmltv_ns"))
                {
                    xmlChar *content=xmlNodeListGetString(node->doc,node->xmlChildrenNode,1);
                    if (content)
                    {
                        // format is:  season[/max_season].episode[/max_episode_in_season].part[/max_part]
                        //             all numbers are zero based, overallepisode is not representable,
                        //             one or two (or even three?) numbers may be omitted, e.g. '0.5.'
                        //             means episode 6 in season 1
                        char *xmltv_ns=strdup((const char *) content);
                        if (xmltv_ns)
                        {
                            xmltv_ns=compactspace(xmltv_ns); // get rid of spaces
                            if (strlen(xmltv_ns)>1)
                            {
                                if (xmltv_ns[0]=='.')
                                {
                                    // no season
                                    if (xmltv_ns[1]!='.')
                                    {
                                        // extract episode
                                        int episode=atoi(xmltv_ns+1)+1;
                                        if (episode>0) xevent.SetEpisode(episode);
                                    }
                                }
                                else
                                {
                                    // extract season
                                    int season=atoi(xmltv_ns)+1;
                                    if (season>0) xevent.SetSeason(season);
                                    char *p=strchr(xmltv_ns,'.');
                                    if (*p)
                                    {
                                        p++;
                                        // extract episode
                                        int episode=atoi(p)+1;
                                        if (episode>0) xevent.SetEpisode(episode);
                                    }
                                }
                            }
                            free(xmltv_ns);
                        }
                        xmlFree(content);
                    }
                }
                if (system) xmlFree(system);
            }
            else if ((!xmlStrcasecmp(node->name, (const xmlChar *) "length")))
            {
                // length without advertisements -> just ignore
            }
            else if ((!xmlStrcasecmp(node->name, (const xmlChar *) "subtitles")))
            {
                // info about subtitles -> just ignore (till now)
            }
            else if ((!xmlStrcasecmp(node->name, (const xmlChar *) "new")))
            {
                // info if it's new -> just ignore (till now)
            }
            else if ((!xmlStrcasecmp(node->name, (const xmlChar *) "premiere")))
            {
                // premiere info -> just ignore (till now)
            }
            else if ((!xmlStrcasecmp(node->name, (const xmlChar *) "previously-shown")))
            {
                // info if it's old ;) -> just ignore (till now)
            }
            else
            {
                esyslogs(source,"unknown element %s, please report!",node->name);
            }
        }
        node=node->next;
    }

    int season=xevent.Season(),episode=xevent.Episode(),episodeoverall=0;
    char *epshorttext=NULL;
    char *eptitle=NULL;

    if (FetchSeasonEpisode(cep2ascii,cutf2ascii,g->EPDir(),xevent.Title(),xevent.ShortText(),
                           xevent.Description(),season,episode,episodeoverall,&epshorttext,
                           &eptitle))
    {
        xevent.SetSeason(season);
        xevent.SetEpisode(episode);
        xevent.SetEpisodeOverall(episodeoverall);
        if (epshorttext)
        {
            if (useeptext) xevent.SetShortText(epshorttext);
            free(epshorttext);
        }
    }
    if (eptitle)
    {
        if (useeptext) xevent.SetAltTitle(eptitle);
        free(eptitle);
    }
    return xevent.HasTitle();
}

int cParse::Process(cEPGExecutor &myExecutor,char *buffer, int bufsize)
{
    if (!buffer) return 134;
    if (!bufsize) return 134;

    dsyslogs(source,"parsing output");

    xmlDocPtr xmltv;
    xmltv=xmlReadMemory(buffer,bufsize,NULL,NULL,0);
    if (!xmltv)
    {
        esyslogs(source,"failed to parse xmltv");
        return 141;
    }

    xmlNodePtr rootnode=xmlDocGetRootElement(xmltv);
    if (!rootnode)
    {
        esyslogs(source,"no rootnode in xmltv");
        xmlFreeDoc(xmltv);
        return 141;
    }

    sqlite3 *db=NULL;
    if (sqlite3_open(g->EPGFile(),&db)!=SQLITE_OK)
    {
        esyslogs(source,"failed to open or create %s",g->EPGFile());
        xmlFreeDoc(xmltv);
        return 141;
    }

    char sql[]="CREATE TABLE IF NOT EXISTS epg (" \
               "src nvarchar(100), channelid nvarchar(255), eventid int, eiteventid int, "\
               "starttime datetime, duration int, title nvarchar(255), alttitle nvarchar(255), "\
               "origtitle nvarchar(255), shorttext nvarchar(255), description text, "\
               "eitdescription text, country nvarchar(255), year int, " \
               "credits text, category text, review text, rating text, " \
               "starrating text, video text, audio text, season int, episode int, " \
               "episodeoverall int, pics text, srcidx int," \
               "PRIMARY KEY(eventid, src, channelid)" \
               ");" \
               "CREATE INDEX IF NOT EXISTS idx1 on epg (starttime, eiteventid, channelid); " \
               "CREATE INDEX IF NOT EXISTS idx2 on epg (starttime, title, channelid); " \
               "CREATE INDEX IF NOT EXISTS idx3 on epg (starttime, duration, src); " \
               "BEGIN";

    char *errmsg;
    if (sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
    {
        esyslogs(source,"createdb: %s",errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        xmlFreeDoc(xmltv);
        return 141;
    }

    time_t begin=time(NULL)-7200;
    xmlNodePtr node=rootnode->xmlChildrenNode;

    int lerr=0,lweak=0;
    xmlChar *lastchannelid=NULL;
    int skipped=0;
    bool do_unlink=false;
    while (node)
    {
        if (node->type!=XML_ELEMENT_NODE)
        {
            node=node->next;
            continue;
        }
        if ((xmlStrcasecmp(node->name, (const xmlChar *) "programme")))
        {
            node=node->next;
            continue;
        }
        xmlChar *channelid=xmlGetProp(node,(const xmlChar *) "channel");
        if (!channelid)
        {
            if (lerr!=PARSE_NOCHANNELID)
                esyslogs(source,"missing channelid in xmltv file");
            lerr=PARSE_NOCHANNELID;
            node=node->next;
            skipped++;
            continue;
        }
        cEPGMapping *map=g->EPGMappings()->GetMap((const char *) channelid);
        if (!map)
        {
            if ((lerr!=PARSE_NOMAPPING) || (lastchannelid && xmlStrcmp(channelid,lastchannelid)))
                esyslogs(source,"no mapping for channelid %s",channelid);
            lerr=PARSE_NOMAPPING;
            if (lastchannelid) xmlFree(lastchannelid);
            lastchannelid=xmlStrdup(channelid);
            xmlFree(channelid);
            node=node->next;
            skipped++;
            continue;
        }
        if (lastchannelid) xmlFree(lastchannelid);
        lastchannelid=xmlStrdup(channelid);
        xmlFree(channelid);

        xmlChar *start=NULL,*stop=NULL;
        time_t starttime=(time_t) 0;
        time_t stoptime=(time_t) 0;
        start=xmlGetProp(node,(const xmlChar *) "start");
        if (start)
        {
            starttime=ConvertXMLTVTime2UnixTime((char *) start);
            if (starttime)
            {
                stop=xmlGetProp(node,(const xmlChar *) "stop");
                if (stop)
                {
                    stoptime=ConvertXMLTVTime2UnixTime((char *) stop);
                }
            }
        }

        if (!starttime)
        {
            if (lerr!=PARSE_XMLTVERR)
                esyslogs(source,"no starttime, check xmltv file");
            lerr=PARSE_XMLTVERR;
            node=node->next;
            skipped++;
            if (start) xmlFree(start);
            if (stop) xmlFree(stop);
            continue;
        }

        if (starttime<begin)
        {
            node=node->next;
            if (start) xmlFree(start);
            if (stop) xmlFree(stop);
            continue;
        }
        xevent.Clear();
        xevent.SetStartTime(starttime);
        if (stoptime)
        {
            if (stoptime<starttime)
            {
                if (lerr!=PARSE_XMLTVERR)
                    esyslogs(source,"stoptime (%s) < starttime(%s), check xmltv file", stop, start);
                lerr=PARSE_XMLTVERR;
                node=node->next;
                skipped++;
                if (start) xmlFree(start);
                if (stop) xmlFree(stop);
                continue;
            }
            xevent.SetDuration(stoptime-starttime);
        }

        if (start) xmlFree(start);
        if (stop) xmlFree(stop);

        if (!FetchEvent(node,(map->Flags() & OPT_SEASON_STEXTITLE)==OPT_SEASON_STEXTITLE)) // sets xevent
        {
            if (lerr!=PARSE_FETCHERR)
                esyslogs(source,"failed to fetch event");
            lerr=PARSE_FETCHERR;
            node=node->next;
            skipped++;
            continue;
        }
        xmlErrorPtr xmlerr=xmlGetLastError();
        if (xmlerr && xmlerr->code)
        {
            esyslogs(source,"%s",xmlerr->message);
        }

        if (!xevent.EventID())
        {
            if (lweak!=PARSE_NOEVENTID)
                isyslogs(source,"event without id, using starttime as id (weak)!");
            lweak=PARSE_NOEVENTID;
            xevent.CreateEventID(xevent.StartTime());
        }

        for (int i=0; i<map->NumChannelIDs(); i++)
        {
            char *isql,*usql;
            xevent.GetSQL(source->Name(),source->Index(),map->ChannelIDs()[i].ToString(),&isql,&usql);
            if (isql && usql)
            {
                int ret=sqlite3_exec(db,isql,NULL,NULL,&errmsg);
                if (ret!=SQLITE_OK)
                {
                    bool update_issued=false;
                    if (ret==SQLITE_CONSTRAINT)
                    {
                        sqlite3_free(errmsg);
                        ret=sqlite3_exec(db,usql,NULL,NULL,&errmsg);
                        update_issued=true;
                    }
                    if (ret!=SQLITE_OK)
                    {
                        if (lerr!=PARSE_SQLERR)
                        {
                            if (strstr(errmsg,"has no column named"))
                            {
                                esyslogs(source,"sqlite3: database schema changed, unlinking epg.db!");
                                do_unlink=true;
                            }
                            else
                            {
                                if (!xevent.WeakID())
                                {
                                    esyslogs(source,"sqlite3: %s (%u@%i)",errmsg,xevent.EventID(),node->line);
                                    tsyslogs(source,"sqlite3: %s",isql);
                                    if (update_issued) tsyslogs(source,"sqlite3: %s",usql);
                                }
                                else
                                {
                                    esyslogs(source,"sqlite3: %s ('%s'@%i)",errmsg,xevent.Title(),node->line);
                                    tsyslogs(source,"sqlite3: %s",isql);
                                    if (update_issued) tsyslogs(source,"sqlite3: %s",usql);
                                }
                            }
                        }
                        lerr=PARSE_SQLERR;
                        sqlite3_free(errmsg);
                        skipped++;
                        break;
                    }
                }
            }
        }
        node=node->next;
        if (!myExecutor.StillRunning())
        {
            isyslogs(source,"request to stop from vdr");
            break;
        }
        if (do_unlink) break;
    }

    if (sqlite3_exec(db,"COMMIT",NULL,NULL,&errmsg)!=SQLITE_OK)
    {
        esyslogs(source,"sqlite3: COMMIT %s",errmsg);
        sqlite3_free(errmsg);
    }

    int cnt=sqlite3_total_changes(db);

    if ((skipped) && (!do_unlink))
        isyslogs(source,"skipped %i xmltv events",skipped);

    if (!lerr)
    {
        isyslogs(source,"processed %i xmltv events",cnt);
    }
    else
    {
        isyslogs(source,"processed %i xmltv events - see ERRORs above!",cnt);
    }

    if (sqlite3_exec(db,"ANALYZE epg;",NULL,NULL,&errmsg)!=SQLITE_OK)
    {
        esyslogs(source,"sqlite3: ANALYZE %s",errmsg);
        sqlite3_free(errmsg);
    }

    sqlite3_close(db);

    xmlFreeDoc(xmltv);

    if (do_unlink) unlink(g->EPGFile());

    return 0;
}

void cParse::InitLibXML()
{
    xmlInitParser();
}

void cParse::CleanupLibXML()
{
    xmlCleanupParser();
}

cParse::cParse(cEPGSource *Source, cGlobals *Global)
{
    source=Source;
    g=Global;
    if (g->EPDir())
    {
        cep2ascii=iconv_open("ASCII//TRANSLIT",g->EPCodeset());
        cutf2ascii=iconv_open("ASCII//TRANSLIT","UTF-8");
    }
    else
    {
        cep2ascii=(iconv_t) -1;
        cutf2ascii=(iconv_t) -1;
    }
}

cParse::~cParse()
{
    if (cep2ascii!=(iconv_t) -1) iconv_close(cep2ascii);
    if (cutf2ascii!=(iconv_t) -1) iconv_close(cutf2ascii);
}
