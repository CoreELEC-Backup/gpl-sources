/*
 * maps.cpp: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "maps.h"
#include <limits.h>

cTEXTMapping::cTEXTMapping(const char *Name, const char *Value)
{
    name=strdup(Name);
    value=strdup(Value);
}

cTEXTMapping::~cTEXTMapping()
{
    if (name) free((void *) name);
    if (value) free((void *) value);
}

void cTEXTMapping::ChangeValue(const char *Value)
{
    if (value) free((void *) value);
    value=strdup(Value);
}

// --------------------------------------------------------------------------------------------------------

void cTEXTMappings::Remove()
{
    cTEXTMapping *maps;
    while ((maps=Last())!=NULL)
    {
        Del(maps);
    }
}

cTEXTMapping* cTEXTMappings::GetMap(const char* Name)
{
    if (!Name) return NULL;
    if (!Count()) return NULL;
    for (int i=0; i<Count();i++)
    {
        if (!strcmp(Get(i)->Name(),Name)) return Get(i);
    }
    return NULL;
}


// --------------------------------------------------------------------------------------------------------

bool cEPGMappings::ProcessChannel(const tChannelID ChannelID)
{
    if (!Count()) return false;
    for (int i=0; i<Count();i++)
    {
        for (int x=0; x<Get(i)->NumChannelIDs(); x++)
        {
            if (Get(i)->ChannelIDs()[x]==ChannelID) return true;
        }
    }
    return false;
}

bool cEPGMappings::IgnoreChannel(const cChannel *Channel)
{
    if (!Channel) return false;
    if (!Count()) return false;
    tChannelID cid=Channel->GetChannelID();
    for (int i=0; i<Count();i++)
    {
        if ((Get(i)->Flags() & OPT_APPEND)==OPT_APPEND)
        {
            for (int x=0; x<Get(i)->NumChannelIDs(); x++)
            {
                if (Get(i)->ChannelIDs()[x]==cid) return true;
            }
        }
    }
    return false;
}

void cEPGMappings::Remove()
{
    cEPGMapping *maps;
    while ((maps=Last())!=NULL)
    {
        Del(maps);
    }
}

cEPGMapping* cEPGMappings::GetMap(const char* ChannelName)
{
    if (!ChannelName) return NULL;
    if (!Count()) return NULL;
    for (int i=0; i<Count();i++)
    {
        if (!strcmp(Get(i)->ChannelName(),ChannelName)) return Get(i);
    }
    return NULL;
}

cEPGMapping *cEPGMappings::GetMap(tChannelID ChannelID)
{
    if (!Count()) return NULL;
    for (int i=0; i<Count();i++)
    {
        for (int x=0; x<Get(i)->NumChannelIDs(); x++)
        {
            if (Get(i)->ChannelIDs()[x]==ChannelID) return Get(i);
        }
    }
    return NULL;
}

// --------------------------------------------------------------------------------------------------------

cEPGMapping::cEPGMapping(const char *ChannelName, const char *Flags_and_Channels)
{
    channelname=strdup(ChannelName);

    dsyslog("xmltv2vdr: added mapping for '%s'",channelname);

    flags=USE_SHORTTEXT;
    channelids=NULL;
    numchannelids=0;

    if (Flags_and_Channels)
    {
        char *flags_unused_p=(char *) strdup(Flags_and_Channels);
        if (!flags_unused_p) return;

        char *flags_p=strchr(flags_unused_p,';');
        if (flags_p)
        {
            *flags_p=0;
            flags_p++;

            char *channels_p=strchr(flags_p,';');
            if (channels_p)
            {
                *channels_p=0;
                channels_p++;
                flags=atoi(flags_p);
                addchannels(channels_p);
            }
        }
        free(flags_unused_p);
    }
}

cEPGMapping::~cEPGMapping()
{
    if (channelname) free((void *) channelname);
    if (channelids) free(channelids);
}

cEPGMapping::cEPGMapping(cEPGMapping&copy)
{
    channelname=strdup(copy.channelname);
    channelids=NULL;
    numchannelids=0;
    if (copy.numchannelids>0)
    {
        channelids=(tChannelID *) malloc((copy.numchannelids+1)*sizeof(tChannelID));
        if (!channelids) return;
        for (int i=0; i<copy.numchannelids; i++)
            channelids[i]=copy.channelids[i];
        numchannelids=copy.numchannelids;
    }
    flags=copy.flags;
}

int cEPGMapping::compare(const void *a, const void *b)
{
    tChannelID *v1=(tChannelID *) a;
    tChannelID *v2=(tChannelID *) b;
    int num1=0,num2=0;
    if (*v1==tChannelID::InvalidID)
    {
        num1=INT_MAX;
    }
    if (*v2==tChannelID::InvalidID)
    {
        num2=INT_MAX;
    }

#if VDRVERSNUM>=20301
    cStateKey StateKeyChan;
    const cChannels *Channels=cChannels::GetChannelsRead(StateKeyChan);
    if (Channels)
    {
#endif

        if (!num1)
        {
#if VDRVERSNUM>=20301
            const cChannel *c1=Channels->GetByChannelID(*v1);
#else
            cChannel *c1=Channels.GetByChannelID(*v1);
#endif
            if (c1) num1=c1->Number();
        }
        if (!num2)
        {
#if VDRVERSNUM>=20301
            const cChannel *c2=Channels->GetByChannelID(*v2);
#else
            cChannel *c2=Channels.GetByChannelID(*v2);
#endif
            if (c2) num2=c2->Number();
        }
#if VDRVERSNUM>=20301
        StateKeyChan.Remove();
    }
#endif
    if (num1>num2) return 1;
    else return -1;
}

void cEPGMapping::addchannels(const char *channels)
{
    char *tmp=(char *) strdup(channels);
    if (!tmp) return;

    char *token,*str1,*saveptr;
    str1=tmp;

    while (token=strtok_r(str1,";",&saveptr))
    {
        tChannelID ChannelID=tChannelID::FromString(token);
        if (!(ChannelID==tChannelID::InvalidID))
        {
            tChannelID *tmp_channelids=(tChannelID *) realloc(channelids,(numchannelids+1)*sizeof(struct tChannelID));
            if (!tmp_channelids)
            {
                free(tmp);
                return;
            }
            channelids=tmp_channelids;
            channelids[numchannelids]=ChannelID;
            numchannelids++;
        }
        str1=NULL;
    }
    free(tmp);
}

void cEPGMapping::AddChannel(int ChannelNumber)
{
#if VDRVERSNUM>=20301
    cStateKey StateKeyChan;
    const cChannels *Channels=cChannels::GetChannelsRead(StateKeyChan);
    if (Channels)
    {
        const cChannel *chan=Channels->GetByNumber(ChannelNumber);
#else
    cChannel *chan=Channels.GetByNumber(ChannelNumber);
#endif
        if (chan)
        {
            bool found=false;
            for (int i=0; i<numchannelids; i++)
            {
                if (channelids[i]==chan->GetChannelID())
                {
                    found=true;
                    break;
                }
            }
            if (!found)
            {
                tChannelID *tmp_channelids=static_cast<tChannelID *>(realloc(channelids,(numchannelids+1)*sizeof(struct tChannelID)));
                if (tmp_channelids)
                {
                    channelids=tmp_channelids;
                    channelids[numchannelids]=chan->GetChannelID();
                    numchannelids++;
                    qsort(channelids,numchannelids,sizeof(tChannelID),compare);
                }
            }
        }
#if VDRVERSNUM>=20301
        StateKeyChan.Remove();
    }
#endif
}

void cEPGMapping::ReplaceChannels(int NumChannelIDs, tChannelID *ChannelIDs)
{
    if (NumChannelIDs<0) return;
    free(channelids);
    channelids=NULL;
    numchannelids=0;
    if (!NumChannelIDs) return;
    if (!ChannelIDs) return;

    for (int i=0; i<NumChannelIDs; i++)
    {
        tChannelID *tmp_channelids=static_cast<tChannelID *>(realloc(channelids,(numchannelids+1)*sizeof(tChannelID)));
        if (tmp_channelids)
        {
            channelids=tmp_channelids;
            channelids[numchannelids]=ChannelIDs[i];
            numchannelids++;
            qsort(channelids,numchannelids,sizeof(tChannelID),compare);
        }
    }
}

void cEPGMapping::RemoveInvalidChannels()
{
    qsort(channelids,numchannelids,sizeof(tChannelID),compare);
    for (int i=0; i<numchannelids; i++)
    {
        if (channelids[i]==tChannelID::InvalidID)
        {
            numchannelids--;
        }
    }
}

void cEPGMapping::RemoveChannel(tChannelID ChannelID, bool MarkOnly)
{
    bool found=false;
    int i;
    for (i=0; i<numchannelids; i++)
    {
        if (channelids[i]==ChannelID)
        {
            found=true;
            break;
        }
    }
    if (found)
    {
        channelids[i]=tChannelID::InvalidID;
        if (!MarkOnly)
        {
            qsort(channelids,numchannelids,sizeof(tChannelID),compare);
            numchannelids--;
        }
    }
}

void cEPGMapping::RemoveChannel(int ChannelNumber, bool MarkOnly)
{
    if (!ChannelNumber) return;
#if VDRVERSNUM>=20301
    cStateKey StateKeyChan;
    const cChannels *Channels=cChannels::GetChannelsRead(StateKeyChan);
    if (Channels)
    {
        const cChannel *chan=Channels->GetByNumber(ChannelNumber);
        if (!chan)
        {
            StateKeyChan.Remove();
            return;
        }
#else
    cChannel *chan=Channels.GetByNumber(ChannelNumber);
    if (!chan) return;
#endif
        bool found=false;
        int i;
        for (i=0; i<numchannelids; i++)
        {
            if (channelids[i]==chan->GetChannelID())
            {
                found=true;
                break;
            }
        }
        if (found)
        {
            channelids[i]=tChannelID::InvalidID;
            if (!MarkOnly)
            {
                qsort(channelids,numchannelids,sizeof(tChannelID),compare);
                numchannelids--;
            }
        }
#if VDRVERSNUM>=20301
        StateKeyChan.Remove();
    }
#endif
}
