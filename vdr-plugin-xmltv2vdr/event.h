/*
 * event.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef _EVENT_H
#define _EVENT_H

#include <time.h>
#include <vdr/epg.h>

class cXMLTVStringList : public cVector<char *>
{
private:
    char *buf;
public:
    cXMLTVStringList(int Allocated = 10): cVector<char *>(Allocated)
    {
        buf=NULL;
    }
    virtual ~cXMLTVStringList();
    void Sort(void)
    {
        cVector<char *>::Sort(CompareStrings);
    }
    const char *toString();
    virtual void Clear(void);
};

class cXMLTVEvent
{
private:
    char *title;
    char *alttitle;
    char *shorttext;
    char *description;
    char *eitdescription;
    char *country;
    char *origtitle;
    char *audio;
    char *sql_insert;
    char *sql_update;
    char *channelid;
    char *source;
    int year;
    time_t starttime;
    int duration;
    int season;
    int episode;
    int episodeoverall;
    bool weakid;
    tEventID eventid;
    tEventID eiteventid;
    cXMLTVStringList video;
    cXMLTVStringList credits;
    cXMLTVStringList category;
    cXMLTVStringList review;
    cXMLTVStringList rating;
    cXMLTVStringList starrating;
    cXMLTVStringList pics;
    int parentalRating;
    char *removechar(char *s, char what);
public:
    cXMLTVEvent();
    ~cXMLTVEvent();
    void Clear();
    void SetSource(const char *Source);
    void SetChannelID(const char *ChannelID);
    void SetTitle(const char *Title);
    void SetAltTitle(const char *AltTitle);
    void SetOrigTitle(const char *OrigTitle);
    void SetShortText(const char *ShortText);
    void SetDescription(const char *Description);
    void SetEITDescription(const char *EITDescription);
    void SetCountry(const char *Country);
    void SetAudio(const char *Audio);
    void AddDescription(const char *Description);
    void AddVideo(const char *VType, const char *VContent);
    void AddCredits(const char *CreditType, const char *Credit, const char *Addendum=NULL);
    void AddCategory(const char *Category);
    void AddReview(const char *Review);
    void AddRating(const char *System, const char *Rating);
    void AddStarRating(const char *System, const char *Rating);
    void AddPics(const char *Pic);
    void SetCredits(const char *Credits);
    void SetCategory(const char *Category);
    void SetReview(const char *Review);
    void SetRating(const char *Rating);
    void SetStarRating(const char *StarRating);
    void SetVideo(const char *Video);
    void SetPics(const char *Pics);
    void CreateEventID(time_t StartTime);
    void GetSQL(const char *Source, int SrcIdx, const char *ChannelID, char **Insert, char **Update);
    bool WeakID()
    {
        return weakid;
    }
    cXMLTVStringList *Credits()
    {
        return &credits;
    }
    cXMLTVStringList *Category()
    {
        return &category;
    }
    cXMLTVStringList *Review()
    {
        return &review;
    }
    cXMLTVStringList *Rating()
    {
        return &rating;
    }
    cXMLTVStringList *StarRating()
    {
        return &starrating;
    }
    cXMLTVStringList *Video()
    {
        return &video;
    }
    cXMLTVStringList *Pics()
    {
        return &pics;
    }
    void SetSeason(int Season)
    {
        season=Season;
    }
    void SetEpisode(int Episode)
    {
        episode=Episode;
    }
    void SetEpisodeOverall(int EpisodeOverall)
    {
        episodeoverall=EpisodeOverall;
    }
    void SetYear(int Year)
    {
        year=Year;
    }
    void SetStartTime(time_t StartTime)
    {
        starttime=StartTime;
    }
    void SetDuration(int Duration)
    {
        duration=Duration;
    }
    void SetEventID(tEventID EventID)
    {
        eventid=EventID;
    }
    void SetEITEventID(tEventID EventID)
    {
        eiteventid=EventID;
    }
    int ParentalRating() const
    {
        return parentalRating;
    }
    int Duration() const
    {
        return duration;
    }
    time_t StartTime() const
    {
        return starttime;
    }
    bool HasTitle(void)
    {
        return (title!=NULL);
    }
    const char *ChannelID(void) const
    {
        return channelid;
    }
    const char *Source(void) const
    {
        return source;
    }
    const char *Title(void) const
    {
        return title;
    }
    const char *AltTitle(void) const
    {
        return alttitle;
    }
    const char *ShortText(void) const
    {
        return shorttext;
    }
    const char *Description(void) const
    {
        return description;
    }
    const char *EITDescription(void) const
    {
        return eitdescription;
    }
    const char *Country(void) const
    {
        return country;
    }
    int Year() const
    {
        return year;
    }
    const char *OrigTitle(void) const
    {
        return origtitle;
    }
    const char *Audio(void) const
    {
        return audio;
    }
    tEventID EventID(void) const
    {
        return eventid;
    }
    tEventID EITEventID(void) const
    {
        return eiteventid;
    }
    int Season(void)
    {
        return season;
    }
    int Episode(void)
    {
        return episode;
    }
    int EpisodeOverall(void)
    {
        return episodeoverall;
    }
};



#endif
