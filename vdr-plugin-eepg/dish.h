/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LIBSI_DISH_H
#define LIBSI_DISH_H

#include <libsi/util.h>
#include <libsi/descriptor.h>
#include <time.h>

namespace SI
{

   namespace DISH_THEMES {
     enum eDishThemes {
      Movie           = 0x01,
      Sports          = 0x02,
      News_Business   = 0x03,
      Family_Children = 0x04,
      Education       = 0x05,
      Series_Special  = 0x06,
      Music_Art       = 0x07,
      Religious       = 0x08
     };
   };

   namespace DISH_CATEGORIES {
     enum eDishCategories {
        Action                   = 0x01,
        Adults_only              = 0x02,
        Adventure                = 0x03,
        Animals                  = 0x04,
        Animated                 = 0x05,
     //  Anime
        Anthology                = 0x07,
        Art                      = 0x08,
        Auto                     = 0x09,
        Awards                   = 0x0a,
        Ballet                   = 0x0b,
        Baseball                 = 0x0c,
        Basketball               = 0x0d,
     //  Beach soccer
     //  Beach volleyball
     //  Biathlon
        Biography                = 0x11,
        Boat                     = 0x12,
     //  Boat racing
        Bowling                  = 0x14,
        Boxing                   = 0x15,
        Bus_financial            = 0x16,
        Children                 = 0x1a,
        ChildrenSpecial          = 0x1b,
        ChildrenNews             = 0x1c,
        ChildrenMusic            = 0x1d,
        Collectibles             = 0x1f,
        Comedy                   = 0x20,
        ComedyDrama              = 0x21,
        Computers                = 0x22,
        Cooking                  = 0x23,
        Crime                    = 0x24,
        CrimeDrama               = 0x25,
     //  Curling
        Dance                    = 0x27,
     //  Dark comedy
        Docudrama                = 0x29,
        Documentary              = 0x2a,
        Drama                    = 0x2b,
        Educational              = 0x2c,
     //  Erotic
        Excercise                = 0x2f,
        Fantasy                  = 0x31,
        Fashion                   = 0x32,
     //  Fencing
        Fishing                  = 0x34,
        Football                 = 0x35,
        French                   = 0x36,
        Fundraiser               = 0x37,
        GameShow                 = 0x38,
        Golf                     = 0x39,
        Gymnastics               = 0x3a,
        Health                   = 0x3b,
        History                  = 0x3c,
        HistoricalDrama          = 0x3d,
        Hockey                   = 0x3e,
        Holiday                  = 0x3f,
        HolidayChildren          = 0x40,
        HolidayChildrenSpecial   = 0x41,
     //  Holiday music
     //  Holiday music special
        HolidaySpecial           = 0x44,
        Horror                   = 0x45,
        HorseRacing              = 0x46,
        House_garden             = 0x47,
        HowTo                    = 0x49,
        Interview                = 0x4b,
        Lacrosse                 = 0x4d,
        MartialArts              = 0x4f,
        Medical                  = 0x50,
        Miniseries               = 0x51,
        Motorsports              = 0x52,
        Motorcycle               = 0x53,
        Music                    = 0x54,
        MusicSpecial             = 0x55,
        MusicTalk                = 0x56,
        Musical                  = 0x57,
        MusicalComedy            = 0x58,
        Mystery                  = 0x5a,
        Nature                   = 0x5b,
        News                     = 0x5c,
     //  Olympics
        Opera                    = 0x5f,
        Outdoors                 = 0x60,
     //  Parade
     //  Politics                =  0x62,
        PublicAffairs            = 0x63,
        Reality                  = 0x66,
        Religious                = 0x67,
        Rodeo                    = 0x68,
        Romance                  = 0x69,
        RomanceComedy            = 0x6a,
        Rugby                    = 0x6b,
        Running                  = 0x6c,
        Science                  = 0x6e,
        ScienceFiction           = 0x6f,
        SelfImprovement          = 0x70,
        Shopping                 = 0x71,
        Skiing                   = 0x74,
        Soap                     = 0x77,
     //  Soap special
     //  Soap talk
        Soccor                   = 0x7b,
        Softball                 = 0x7c,
        Spanish                  = 0x7d,
        Special                  = 0x7e,
     //  Speedskating
     //  Sports event
        SportsNonEvent           = 0x81,
        SportsTalk               = 0x82,
        Suspense                 = 0x83,
        Swimming                 = 0x85,
        Talk                     = 0x86,
        Tennis                   = 0x87,
     //  Theater
     //  Thriller
        Track_field              = 0x89,
        Travel                   = 0x8a,
        Variety                  = 0x8b,
        Volleyball               = 0x8c,
        War                      = 0x8d,
        Watersports              = 0x8e,
        Weather                  = 0x8f,
        Western                  = 0x90,
        Wrestling                = 0x92,
        Yoga                     = 0x93,
        Agriculture              = 0x94,
        Anime                    = 0x95,
        ArmWrestling             = 0x97,
        Arts_crafts              = 0x98,
        Auction                  = 0x99,
        AutoRacing               = 0x9a,
        AirRacing                = 0x9b,
        Badminton                = 0x9c,
     //  Bicycle
        BicycleRacing            = 0xa0,
        BoatRacing               = 0xa1,
     //  Bobsled
     //  Bodybilding
     //  Canoe
     //  Cheerleading
        Community                = 0xa6,
        Consumer                 = 0xa7,
     //  Darts
        Debate                   = 0xaa,
     //  Diving
        DogShow                  = 0xac,
        DragRacing               = 0xad,
        Entertainment            = 0xae,
        Environment              = 0xaf,
        Equestrian               = 0xb0,
     //  Event
        FieldHockey              = 0xb3,
     //  Figure skating
        Football2                = 0xb5,
        Gay_lesbian              = 0xb6,
        Handball                 = 0xb7,
        HomeImprovement          = 0xb8,
        Hunting                  = 0xb9,
     //  Hurling
        HydroplaneRacing         = 0xbb,
     //  Indoor soccer
     //  Intl hockey
     //  Intl soccer
     //  Kayaking
        Law                      = 0xc1,
     //  Luge
     //  Mountain biking
        MotorcycleRacing         = 0xc3,
        Newsmagazine             = 0xc5,
        Paranormal               = 0xc7,
        Parenting                = 0xc8,
        PerformingArts           = 0xca,
     //  Playoff sports
        Politics                 = 0xcc,
     //  Polo
     //  Pool
        ProWrestling             = 0xcf,
     //  Ringuette
     //  Roller derby
     //  Rowing
        Sailing                  = 0xd3,
        Shooting                 = 0xd4,
        Sitcom                   = 0xd5,
        Skateboarding            = 0xd6,
     //  Skating
     //  Skeleton
        Snowboarding             = 0xd9,
     //  Snowmobile              =  0xda,
        Standup                  = 0xdd,
     //  Sumo wrestling
        Surfing                  = 0xdf,
        Tennis2                  = 0xe0,
        Triathlon                = 0xe1,
     //  Water polo
     //  Water skiing
     //  Weightlifting
     //  Yacht racing
        CardGames                = 0xe6 ,
        Poker                    = 0xe7 ,
     //  Musical                 =  0xe9,
        Military                 = 0xea,
        Technology               = 0xeb,
        MixedMartialArts         = 0xec,
        ActionSports             = 0xed,
        DishNetwork              = 0xff
     };
   };
#define SIZE_TABLE_128 128
#define SIZE_TABLE_255 255

using namespace std;

class DishDescriptor {
public:
    DishDescriptor();
    virtual ~DishDescriptor();
    const char* getName(void) const { return name; }
    const char* getShortText(void);
    const char *getDescription(void);
    //   const char* getShortText(void) const { return shortText?shortText->c_str():""; }
    //   const char* getDescription(void) const { return description?description->c_str():""; }
    const char *getTheme();
    const char *getCategory();
    const char *getRating();
    const char *getStarRating();
    const char *getSeriesId();
    const char *getProgramId();
    time_t getOriginalAirDate() { return originalAirDate; }
    bool hasTheme() {return DishTheme > 0;}
    bool hasCategory() {return DishCategory > 0;}
    void setShortData(unsigned char Tid, CharArray data);
    void setExtendedtData(unsigned char Tid, CharArray data);
    void setRating(uint16_t value);
    void setContent(ContentDescriptor::Nibble Nibble);
    void setEpisodeInfo(CharArray data);

protected:
    // Decompress the byte array and stores the result to a text string
    unsigned char* Decompress(unsigned char Tid, CharArray data);
    const char* name; // name of the event
    const char* shortText; // usually the episode name
    const char* description; // description of the event
    unsigned char* decompressedShort;
    unsigned char* decompressedExtended;
    unsigned char DishTheme;
    unsigned char DishCategory;
    uint16_t mpaaRating;
    uint8_t starRating;
    time_t originalAirDate;
    char* seriesId;
    char* programId;
    char* ratingStr;

    struct HuffmanTable
    {
        unsigned int startingAddress;
        unsigned char character;
        unsigned char numberOfBits;
    };
    static HuffmanTable Table128[SIZE_TABLE_128];
    static HuffmanTable Table255[SIZE_TABLE_255];
};

} /* namespace SI */
#endif /* LIBSI_DISH_H */
