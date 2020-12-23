/***************************************************************************
 *                                                                         *
 *   These routines decompress Huffman coded Dish Network EIT data.        *
 *   The implementation is based on the algorithm presentend in            *
 *                                                                         *
 *     "A memory-efficient Huffman decoding algorithm"                     *
 *     Pi-Chung Wang, Yuan-Rung Yang, Chun-Liang Lee, Hung-Yi Chang        *
 *     Proceedings of the 19th International Conference on Advanced        *
 *     Information Networking and Applications (AINA'05)                   *
 *                                                                         *
 ***************************************************************************/

#include "dish.h"
#include <libsi/si.h>
//#include <libsi/descriptor.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <vdr/tools.h>

namespace SI
{

  // returns the value of a sequence of bits in the byte array
  static unsigned int getBits(int bitIndex, int bitCount, const unsigned char *byteptr,
      int length) {
    union
    {
      unsigned char b[4];
      unsigned long val;
    } chunk;

    int offset = bitIndex >> 3;
    int bitnum = bitIndex - (offset << 3);
    int rightend = 32 - bitnum - bitCount;

    chunk.b[3] = byteptr[offset];
    chunk.b[2] = (offset + 1 < length) ? byteptr[offset + 1] : 0;
    chunk.b[1] = (offset + 2 < length) ? byteptr[offset + 2] : 0;
    chunk.b[0] = 0;  // Never need to look this far ahead.

    return (unsigned int) ((((((chunk.val & (0xFFFFFFFF >> bitnum)) >> rightend)))));
    }

    DishDescriptor::DishDescriptor()
    {
        name = NULL;
        shortText = NULL;
        description = NULL;
        decompressedShort = NULL;
        decompressedExtended = NULL;
        DishTheme = 0;
        DishCategory = 0;
        mpaaRating = 0;
        starRating = 0;
        originalAirDate = 0;
        programId = NULL;
        seriesId = NULL;
        ratingStr = NULL;
    }

    DishDescriptor::~DishDescriptor()
    {
        delete [] decompressedShort;
        decompressedShort = NULL;
        delete [] decompressedExtended;
        decompressedExtended = NULL;
        delete [] programId;
        programId = NULL;
        delete [] seriesId;
        seriesId = NULL;
    }

    const char *DishDescriptor::getTheme()
    {
      const char* theme;
      using namespace DISH_THEMES;

        switch (DishTheme){
        case Movie:
          theme = "Movie";
          break;
        case Sports:
          theme = "Sports";
            break;
        case News_Business:
          theme = "News/Business";
            break;
        case Family_Children:
          theme = "Family/Children";
            break;
        case Education:
          theme = "Education";
            break;
        case Series_Special:
          theme = "Series/Special";
            break;
        case Music_Art:
          theme = "Music/Art";
            break;
        case Religious:
          theme = "Religious";
            break;
        default:
          theme = "";
          break;
      }
      return theme;
    }

    const char *DishDescriptor::getCategory()
    {
        using namespace DISH_CATEGORIES;

      switch (DishCategory) {
      case Action: return "Action";
      case ActionSports:              return "Action Sports";
      case Adults_only:               return "Adults only";
      case Adventure:                 return "Adventure";
      case Agriculture:               return "Agriculture";
      case AirRacing:                 return "Air racing";
      case Animals:                   return "Animals";
      case Animated:                  return "Animated";
      case Anime:                     return "Anime";
      case Anthology:                 return "Anthology";
      case ArmWrestling:              return "Arm wrestling";
      case Art:                       return "Art";
      case Arts_crafts:               return "Arts/crafts";
      case Auction:                   return "Auction";
      case Auto:                      return "Auto";
      case AutoRacing:                return "Auto racing";
      case Awards:                    return "Awards";
      case Badminton:                 return "Badminton";
      case Ballet:                    return "Ballet";
      case Baseball:                  return "Baseball";
      case Basketball:                return "Basketball";
      case BicycleRacing:             return "Bicycle racing";
      case Biography:                 return "Biography";
      case Boat:                      return "Boat";
      case BoatRacing:                return "Boat racing";
      case Bowling:                   return "Bowling";
      case Boxing:                    return "Boxing";
      case Bus_financial:             return "Bus./financial";
      case CardGames:                 return "Card games";
      case Children:                  return "Children";
      case ChildrenMusic:             return "Children music";
      case ChildrenNews:              return "Children news";
      case ChildrenSpecial:           return "Children special";
      case Collectibles:              return "Collectibles";
      case Comedy:                    return "Comedy";
      case ComedyDrama:               return "Comedy-drama";
      case Community:                 return "Community";
      case Computers:                 return "Computers";
      case Consumer:                  return "Consumer";
      case Cooking:                   return "Cooking";
      case Crime:                     return "Crime";
      case CrimeDrama:                return "Crime drama";
      case Dance:                     return "Dance";
      case Debate:                    return "Debate";
      case DishNetwork:               return "Dish Network";
      case Docudrama:                 return "Docudrama";
      case Documentary:               return "Documentary";
      case DogShow:                   return "DogShow";
      case DragRacing:                return "DragRacing";
      case Drama:                     return "Drama";
      case Educational:               return "Educational";
      case Entertainment:             return "Entertainment";
      case Environment:               return "Environment";
      case Equestrian:                return "Equestrian";
      case Excercise:                 return "Excercise";
      case Fantasy:                   return "Fantasy";
      case Fashion:                   return "Fashion";
      case FieldHockey:               return "Field hockey";
      case Fishing:                   return "Fishing";
      case Football:
      case Football2:                 return "Football";
      case French:                    return "French";
      case Fundraiser:                return "Fundraiser";
      case GameShow:                  return "GameShow";
      case Gay_lesbian:               return "Gay/lesbian";
      case Golf:                      return "Golf";
      case Gymnastics:                return "Gymnastics";
      case Handball:                  return "Handball";
      case Health:                    return "Health";
      case HistoricalDrama:           return "Historical drama";
      case History:                   return "History";
      case Hockey:                    return "Hockey";
      case Holiday:                   return "Holiday";
      case HolidayChildren:           return "Holiday children";
      case HolidayChildrenSpecial:    return "Holiday children special";
      case HolidaySpecial:            return "Holiday special";
      case HomeImprovement:           return "Home improvement";
      case Horror:                    return "Horror";
      case HorseRacing:               return "Horse racing";
      case House_garden:              return "House/garden";
      case HowTo:                     return "HowTo";
      case Hunting:                   return "Hunting";
      case HydroplaneRacing:          return "Hydroplane racing";
      case Interview:                 return "Interview";
      case Lacrosse:                  return "Lacrosse";
      case Law:                       return "Law";
      case MartialArts:               return "Martial arts";
      case Medical:                   return "Medical";
      case Military:                  return "Military";
      case Miniseries:                return "Miniseries";
      case MixedMartialArts:          return "Mixed martial arts";
      case Motorcycle:                return "Motorcycle";
      case MotorcycleRacing:          return "Motorcycle racing";
      case Motorsports:               return "Motorsports";
      case Music:                     return "Music";
      case MusicSpecial:              return "Music special";
      case MusicTalk:                 return "Music talk";
      case Musical:                   return "Musical";
      case MusicalComedy:             return "Musical comedy";
      case Mystery:                   return "Mystery";
      case Nature:                    return "Nature";
      case News:                      return "News";
      case Newsmagazine:              return "Newsmagazine";
      case Opera:                     return "Opera";
      case Outdoors:                  return "Outdoors";
      case Paranormal:                return "Paranormal";
      case Parenting:                 return "Parenting";
      case PerformingArts:            return "Performing arts";
      case Poker:                     return "Poker";
      case Politics:                  return "Politics";
      case ProWrestling:              return "Pro wrestling";
      case PublicAffairs:             return "Public affairs";
      case Reality:                   return "Reality";
      case Religious:                 return "Religious";
      case Rodeo:                     return "Rodeo";
      case Romance:                   return "Romance";
      case RomanceComedy:             return "Romance comedy";
      case Rugby:                     return "Rugby";
      case Running:                   return "Running";
      case Sailing:                   return "Sailing";
      case Science:                   return "Science";
      case ScienceFiction:            return "Science fiction";
      case SelfImprovement:           return "Self improvement";
      case Shooting:                  return "Shooting";
      case Shopping:                  return "Shopping";
      case Sitcom:                    return "Sitcom";
      case Skateboarding:             return "Skateboarding";
      case Skiing:                    return "Skiing";
      case Snowboarding:              return "Snowboarding";
      case Soap:                      return "Soap";
      case Soccor:                    return "Soccor";
      case Softball:                  return "Softball";
      case Spanish:                   return "Spanish";
      case Special:                   return "Special";
      case SportsNonEvent:            return "SportsNonEvent";
      case SportsTalk:                return "SportsTalk";
      case Standup:                   return "Standup";
      case Surfing:                   return "Surfing";
      case Suspense:                  return "Suspense";
      case Swimming:                  return "Swimming";
      case Talk:                      return "Talk";
      case Technology:                return "Technology";
      case Tennis:
      case Tennis2:                   return "Tennis";
      case Track_field:               return "Track/field";
      case Travel:                    return "Travel";
      case Triathlon:                 return "Triathlon";
      case Variety:                   return "Variety";
      case Volleyball:                return "Volleyball";
      case War:                       return "War";
      case Watersports:               return "Watersports";
      case Weather:                   return "Weather";
      case Western:                   return "Western";
      case Wrestling:                 return "Wrestling";
      case Yoga:                      return "Yoga";
      default: return "";
      }
    }

    void DishDescriptor::setShortData(unsigned char Tid, CharArray data)
    {
      decompressedShort = Decompress(Tid, data);
      if (decompressedShort) {
        name = (char*)decompressedShort;
      }
    }

    void DishDescriptor::setExtendedtData(unsigned char Tid, CharArray data)
    {
      decompressedExtended = Decompress(Tid, data);
        if (decompressedExtended) {
        char *split = strchr((char*)((decompressedExtended)), 0x0D); // Look for carriage return
        //LogD(2, prep("dLength:%d, length:%d, count:%d, decompressed: %s"), dLength, length, count, decompressed);
        if(split){
            *split = 0;
            shortText = (char*)decompressedExtended;
            description = (split[1] == 0x20) ? split + 2 : split + 1;
        }else{
          description = (char*)decompressedExtended;
        }
      }
    }

    const char *DishDescriptor::getShortText(void)
    {
//        string tmp = "";
////        if (shortText != NULL) tmp += *shortText;
//        tmp += shortText;
//        if(DishTheme > 0){
//            if(tmp != "") tmp += " - ";
//
//            tmp += getTheme();
//        }
//        if(DishCategory > 0){
//            if(tmp != "") tmp += " - ";
//
//            tmp += getCategory();
//        }
//        return tmp.c_str();
      return shortText?shortText:"";
    }

    const char *DishDescriptor::getDescription(void) {
//      string tmp = "";
////      if (description != NULL) tmp += *description;
//      tmp += description;
//      const char* rating = getRating();
//      if (rating && strcmp(rating,"") != 0) {
//        if(tmp != "") tmp += " ~ ";
//        tmp += rating;
//      }
//      if (starRating > 0) {
//        if(tmp != "") tmp += " ~ ";
//        tmp += getStarRating();
//      }
//      return tmp.c_str();
      return description?description:"";
    }

    const char *DishDescriptor::getProgramId(void) {
      return programId?programId:"";
    }

    const char *DishDescriptor::getSeriesId(void) {
      return seriesId?seriesId:"";
    }

    void DishDescriptor::setEpisodeInfo(CharArray data)
    {
      data.addOffset(2);
      int series = (data[1] << 0x12) | (data[2] << 0x0a) | (data[3] << 0x02) | ((data[4] & 0xc0) >> 0x06);
      int episode = ((data[4] & 0x3f) << 0x08) | data[5];
      const char* prefix;

      if (data[0] == 0x7c)
           prefix = "MV";
      else if (data[0] == 0x7d)
           prefix = "SP";
      else if (data[0] == 0x7e)
           prefix = "EP";
      else
           prefix ="";

      programId = new char[17];

      sprintf(programId, "%s%08d%04d", (data[0] == 0x7e && episode == 0 ? "SH" : prefix), series, episode);

      if (data[0] == 0x7e) {
        seriesId = new char[11];
        sprintf(seriesId, "%s%08d", prefix, series);
      }

      if (data.TwoBytes(6) != 0 && data.TwoBytes(6) != 0x9e8b ) {
          originalAirDate = ((data[6] << 0x08 | data[7]) - 40587) * 86400;
      }
    }

    void DishDescriptor::setContent(ContentDescriptor::Nibble Nibble)
    {
      DishTheme = Nibble.getContentNibbleLevel2() & 0xF;
      DishCategory = ((Nibble.getUserNibble1() & 0xF) << 4) | (Nibble.getUserNibble2() & 0xF);
    }

    void DishDescriptor::setRating(uint16_t rating)
    {
      uint16_t newRating = (rating >> 10) & 0x07;
      if (newRating == 0) newRating = 5;
      if (newRating == 6) newRating = 0;
      mpaaRating = (newRating << 10) | (rating & 0x3FF);
      starRating = (rating >> 13) & 0x07;
    }

    const char* DishDescriptor::getRating(){
      static const char *const ratings[8] =  { "", "G", "PG", "PG-13", "R", "NR/AO", "", "NC-17" };

      if (mpaaRating == 0) {
        return ratings[mpaaRating];
      }

      std::string str = ratings[(mpaaRating >> 10) & 0x07];
//      char buffer[19];
//      buffer[0] = 0;
//      strcpy(buffer, ratings[(mpaaRating >> 10) & 0x07]);
      if (mpaaRating & 0x3A7F) {
          str += " [";
//         strcat(buffer, " [");
         if (mpaaRating & 0x0230)
           str += "V,";
//            strcat(buffer, "V,");
         if (mpaaRating & 0x000A)
           str += "L,";
//            strcat(buffer, "L,");
         if (mpaaRating & 0x0044)
           str += "N,";
//            strcat(buffer, "N,");
         if (mpaaRating & 0x0101)
           str += "SC,";
//            strcat(buffer, "SC,");
//         if (char *s = strrchr(buffer, ','))
//            s[0] = ']';
         if (str.find(',') != std::string::npos) {
           str.erase(str.find_last_of(','));
         }
         str += "]";
       }

      if (!ratingStr) ratingStr = new char[19];
      if (ratingStr) strcpy(ratingStr,str.c_str());
      return ratingStr;

//      return isempty(buffer) ? "" : buffer;
    }

    const char* DishDescriptor::getStarRating(){
      static const char *const critiques[8] = { "", "*", "*+", "**", "**+", "***", "***+", "****" };
      return critiques[starRating & 0x07];
    }

    unsigned char* DishDescriptor::Decompress(unsigned char Tid, CharArray data)
    {
        const unsigned char *str = data.getData();
        const unsigned char *cmp = NULL;
        int length = 0; // Length of compressed data
        unsigned int dLength = 0; // Length of decompressed data
        if((str[3] & 0xFC) == 0x80){
            length = str[1] - 2;
            dLength = (str[2] & 0x40) ? ((str[3] << 6) & 0xFF) | (str[2] & 0x3F) : str[2] & 0x3F;
            cmp = str + 4;
        }else{
            length = str[1] - 1;
            dLength = str[2] & 0x7F;
            cmp = str + 3;
        }
        if(length <= 0 || !dLength)
            return NULL;

        unsigned char* decompressed = new unsigned char[dLength + 1];
        HuffmanTable *table;
        unsigned int tableSize, numBits;
        if (Tid > 0x80) {
        table = Table255;
        tableSize = SIZE_TABLE_255;
        numBits = 13;
    } else {
        table = Table128;
        tableSize = SIZE_TABLE_128;
        numBits = 11;
     }
        unsigned int bLength = length << 3; // number of bits
        unsigned int currentBit = 0, count = 0;
        while(currentBit < bLength - 1 && count < dLength){
            // Find the interval containing the sequence of length numBits starting
            // at currentBit. The corresponding character will  be the one encoded
            // at the begin of the sequence.
            unsigned int code = getBits(currentBit, numBits, cmp, length);
            // We could use a binary search, but in practice this linear search is faster.
            unsigned int index = 0;
            while(table[index].startingAddress <= code && index < tableSize){
                index++;
            }
            index--;
            decompressed[count++] = table[index].character;
            currentBit += table[index].numberOfBits;
        }

        decompressed[count] = 0;
        return decompressed;
    }

struct DishDescriptor::HuffmanTable DishDescriptor::Table128[SIZE_TABLE_128] = {
   { 0x0000, 0x20, 0x03 }, { 0x0100, 0x65, 0x04 }, { 0x0180, 0x74, 0x04 }, 
   { 0x0200, 0x61, 0x04 }, { 0x0280, 0x6F, 0x04 }, { 0x0300, 0x73, 0x04 }, 
   { 0x0380, 0x6E, 0x04 }, { 0x0400, 0x72, 0x06 }, { 0x0420, 0x69, 0x06 }, 
   { 0x0440, 0x6C, 0x06 }, { 0x0460, 0x63, 0x06 }, { 0x0480, 0x68, 0x06 }, 
   { 0x04A0, 0x75, 0x06 }, { 0x04C0, 0x64, 0x06 }, { 0x04E0, 0x70, 0x06 }, 
   { 0x0500, 0x6D, 0x06 }, { 0x0520, 0x67, 0x06 }, { 0x0540, 0x79, 0x06 }, 
   { 0x0560, 0x76, 0x06 }, { 0x0580, 0x0A, 0x06 }, { 0x05A0, 0x2E, 0x06 }, 
   { 0x05C0, 0x77, 0x06 }, { 0x05E0, 0x66, 0x06 }, { 0x0600, 0x53, 0x07 }, 
   { 0x0610, 0x62, 0x07 }, { 0x0620, 0x54, 0x07 }, { 0x0630, 0x22, 0x07 }, 
   { 0x0640, 0x6B, 0x07 }, { 0x0650, 0x50, 0x07 }, { 0x0660, 0x41, 0x07 }, 
   { 0x0670, 0x43, 0x07 }, { 0x0680, 0x44, 0x07 }, { 0x0690, 0x4C, 0x07 }, 
   { 0x06A0, 0x4D, 0x07 }, { 0x06B0, 0x49, 0x07 }, { 0x06C0, 0x4E, 0x07 }, 
   { 0x06D0, 0x3A, 0x07 }, { 0x06E0, 0x52, 0x07 }, { 0x06F0, 0x2C, 0x07 }, 
   { 0x0700, 0x45, 0x08 }, { 0x0708, 0x55, 0x08 }, { 0x0710, 0x46, 0x08 }, 
   { 0x0718, 0x48, 0x08 }, { 0x0720, 0x59, 0x08 }, { 0x0728, 0x56, 0x08 }, 
   { 0x0730, 0x2D, 0x08 }, { 0x0738, 0x7A, 0x08 }, { 0x0740, 0x78, 0x08 }, 
   { 0x0748, 0x2F, 0x08 }, { 0x0750, 0x4F, 0x08 }, { 0x0758, 0x3F, 0x08 }, 
   { 0x0760, 0x57, 0x08 }, { 0x0768, 0x47, 0x08 }, { 0x0770, 0x42, 0x08 }, 
   { 0x0778, 0x33, 0x08 }, { 0x0780, 0x31, 0x09 }, { 0x0784, 0x71, 0x09 }, 
   { 0x0788, 0x30, 0x09 }, { 0x078C, 0x21, 0x09 }, { 0x0790, 0x6A, 0x09 }, 
   { 0x0794, 0x5A, 0x09 }, { 0x0798, 0x39, 0x09 }, { 0x079C, 0x34, 0x09 }, 
   { 0x07A0, 0x4B, 0x09 }, { 0x07A4, 0x2A, 0x09 }, { 0x07A8, 0x37, 0x09 }, 
   { 0x07AC, 0x36, 0x09 }, { 0x07B0, 0x35, 0x09 }, { 0x07B4, 0x4A, 0x09 }, 
   { 0x07B8, 0x38, 0x09 }, { 0x07BC, 0x29, 0x09 }, { 0x07C0, 0x28, 0x0A }, 
   { 0x07C2, 0x58, 0x0A }, { 0x07C4, 0x51, 0x0A }, { 0x07C6, 0x3C, 0x0A }, 
   { 0x07C8, 0x32, 0x0A }, { 0x07CA, 0x27, 0x0A }, { 0x07CC, 0x26, 0x0A }, 
   { 0x07CE, 0x7F, 0x0B }, { 0x07CF, 0x7E, 0x0B }, { 0x07D0, 0x7D, 0x0B }, 
   { 0x07D1, 0x7C, 0x0B }, { 0x07D2, 0x7B, 0x0B }, { 0x07D3, 0x60, 0x0B }, 
   { 0x07D4, 0x5F, 0x0B }, { 0x07D5, 0x5E, 0x0B }, { 0x07D6, 0x5D, 0x0B }, 
   { 0x07D7, 0x5C, 0x0B }, { 0x07D8, 0x5B, 0x0B }, { 0x07D9, 0x40, 0x0B }, 
   { 0x07DA, 0x3E, 0x0B }, { 0x07DB, 0x3D, 0x0B }, { 0x07DC, 0x3B, 0x0B }, 
   { 0x07DD, 0x2B, 0x0B }, { 0x07DE, 0x25, 0x0B }, { 0x07DF, 0x24, 0x0B }, 
   { 0x07E0, 0x23, 0x0B }, { 0x07E1, 0x1F, 0x0B }, { 0x07E2, 0x1E, 0x0B }, 
   { 0x07E3, 0x1D, 0x0B }, { 0x07E4, 0x1C, 0x0B }, { 0x07E5, 0x1B, 0x0B }, 
   { 0x07E6, 0x1A, 0x0B }, { 0x07E7, 0x19, 0x0B }, { 0x07E8, 0x18, 0x0B }, 
   { 0x07E9, 0x17, 0x0B }, { 0x07EA, 0x16, 0x0B }, { 0x07EB, 0x15, 0x0B }, 
   { 0x07EC, 0x14, 0x0B }, { 0x07ED, 0x13, 0x0B }, { 0x07EE, 0x12, 0x0B }, 
   { 0x07EF, 0x11, 0x0B }, { 0x07F0, 0x10, 0x0B }, { 0x07F1, 0x0F, 0x0B }, 
   { 0x07F2, 0x0E, 0x0B }, { 0x07F3, 0x0D, 0x0B }, { 0x07F4, 0x0C, 0x0B }, 
   { 0x07F5, 0x0B, 0x0B }, { 0x07F6, 0x09, 0x0B }, { 0x07F7, 0x08, 0x0B }, 
   { 0x07F8, 0x07, 0x0B }, { 0x07F9, 0x06, 0x0B }, { 0x07FA, 0x05, 0x0B }, 
   { 0x07FB, 0x04, 0x0B }, { 0x07FC, 0x03, 0x0B }, { 0x07FD, 0x02, 0x0B }, 
   { 0x07FE, 0x01, 0x0B }, { 0x07FF, 0x00, 0x0B }
};

struct DishDescriptor::HuffmanTable DishDescriptor::Table255[SIZE_TABLE_255] = {
   { 0x0000, 0x20, 0x02 }, { 0x0800, 0x65, 0x04 }, { 0x0A00, 0x72, 0x04 }, 
   { 0x0C00, 0x6E, 0x04 }, { 0x0E00, 0x61, 0x04 }, { 0x1000, 0x74, 0x05 }, 
   { 0x1100, 0x6F, 0x05 }, { 0x1200, 0x73, 0x05 }, { 0x1300, 0x69, 0x05 }, 
   { 0x1400, 0x6C, 0x05 }, { 0x1500, 0x75, 0x05 }, { 0x1600, 0x63, 0x05 }, 
   { 0x1700, 0x64, 0x05 }, { 0x1800, 0x70, 0x07 }, { 0x1840, 0x6D, 0x07 }, 
   { 0x1880, 0x76, 0x07 }, { 0x18C0, 0x67, 0x07 }, { 0x1900, 0x68, 0x07 }, 
   { 0x1940, 0x2E, 0x07 }, { 0x1980, 0x66, 0x07 }, { 0x19C0, 0x0A, 0x07 }, 
   { 0x1A00, 0x53, 0x07 }, { 0x1A40, 0x41, 0x07 }, { 0x1A80, 0x45, 0x07 }, 
   { 0x1AC0, 0x43, 0x07 }, { 0x1B00, 0x27, 0x07 }, { 0x1B40, 0x7A, 0x07 }, 
   { 0x1B80, 0x52, 0x07 }, { 0x1BC0, 0x22, 0x07 }, { 0x1C00, 0x4C, 0x08 }, 
   { 0x1C20, 0x49, 0x08 }, { 0x1C40, 0x4F, 0x08 }, { 0x1C60, 0x62, 0x08 }, 
   { 0x1C80, 0x54, 0x08 }, { 0x1CA0, 0x4E, 0x08 }, { 0x1CC0, 0x55, 0x08 }, 
   { 0x1CE0, 0x79, 0x08 }, { 0x1D00, 0x44, 0x08 }, { 0x1D20, 0x50, 0x08 }, 
   { 0x1D40, 0x71, 0x08 }, { 0x1D60, 0x56, 0x08 }, { 0x1D80, 0x2D, 0x08 }, 
   { 0x1DA0, 0x3A, 0x08 }, { 0x1DC0, 0x2C, 0x08 }, { 0x1DE0, 0x48, 0x08 }, 
   { 0x1E00, 0x4D, 0x09 }, { 0x1E10, 0x78, 0x09 }, { 0x1E20, 0x77, 0x09 }, 
   { 0x1E30, 0x42, 0x09 }, { 0x1E40, 0x47, 0x09 }, { 0x1E50, 0x46, 0x09 }, 
   { 0x1E60, 0x30, 0x09 }, { 0x1E70, 0x3F, 0x09 }, { 0x1E80, 0x33, 0x09 }, 
   { 0x1E90, 0x2F, 0x09 }, { 0x1EA0, 0x39, 0x09 }, { 0x1EB0, 0x31, 0x09 }, 
   { 0x1EC0, 0x38, 0x09 }, { 0x1ED0, 0x6B, 0x09 }, { 0x1EE0, 0x6A, 0x09 }, 
   { 0x1EF0, 0x21, 0x09 }, { 0x1F00, 0x36, 0x0A }, { 0x1F08, 0x35, 0x0A }, 
   { 0x1F10, 0x59, 0x0A }, { 0x1F18, 0x51, 0x0A }, { 0x1F20, 0x34, 0x0B }, 
   { 0x1F24, 0x58, 0x0B }, { 0x1F28, 0x32, 0x0B }, { 0x1F2C, 0x2B, 0x0B }, 
   { 0x1F30, 0x2A, 0x0B }, { 0x1F34, 0x5A, 0x0B }, { 0x1F38, 0x4A, 0x0B }, 
   { 0x1F3C, 0x29, 0x0B }, { 0x1F40, 0x28, 0x0C }, { 0x1F42, 0x23, 0x0C }, 
   { 0x1F44, 0x57, 0x0C }, { 0x1F46, 0x4B, 0x0C }, { 0x1F48, 0x3C, 0x0C }, 
   { 0x1F4A, 0x37, 0x0C }, { 0x1F4C, 0x7D, 0x0C }, { 0x1F4E, 0x7B, 0x0C }, 
   { 0x1F50, 0x60, 0x0C }, { 0x1F52, 0x26, 0x0C }, { 0x1F54, 0xFE, 0x0D }, 
   { 0x1F55, 0xFD, 0x0D }, { 0x1F56, 0xFC, 0x0D }, { 0x1F57, 0xFB, 0x0D }, 
   { 0x1F58, 0xFA, 0x0D }, { 0x1F59, 0xF9, 0x0D }, { 0x1F5A, 0xF8, 0x0D }, 
   { 0x1F5B, 0xF7, 0x0D }, { 0x1F5C, 0xF6, 0x0D }, { 0x1F5D, 0xF5, 0x0D }, 
   { 0x1F5E, 0xF4, 0x0D }, { 0x1F5F, 0xF3, 0x0D }, { 0x1F60, 0xF2, 0x0D }, 
   { 0x1F61, 0xF1, 0x0D }, { 0x1F62, 0xF0, 0x0D }, { 0x1F63, 0xEF, 0x0D }, 
   { 0x1F64, 0xEE, 0x0D }, { 0x1F65, 0xED, 0x0D }, { 0x1F66, 0xEC, 0x0D }, 
   { 0x1F67, 0xEB, 0x0D }, { 0x1F68, 0xEA, 0x0D }, { 0x1F69, 0xE9, 0x0D }, 
   { 0x1F6A, 0xE8, 0x0D }, { 0x1F6B, 0xE7, 0x0D }, { 0x1F6C, 0xE6, 0x0D }, 
   { 0x1F6D, 0xE5, 0x0D }, { 0x1F6E, 0xE4, 0x0D }, { 0x1F6F, 0xE3, 0x0D }, 
   { 0x1F70, 0xE2, 0x0D }, { 0x1F71, 0xE1, 0x0D }, { 0x1F72, 0xE0, 0x0D }, 
   { 0x1F73, 0xDF, 0x0D }, { 0x1F74, 0xDE, 0x0D }, { 0x1F75, 0xDD, 0x0D }, 
   { 0x1F76, 0xDC, 0x0D }, { 0x1F77, 0xDB, 0x0D }, { 0x1F78, 0xDA, 0x0D }, 
   { 0x1F79, 0xD9, 0x0D }, { 0x1F7A, 0xD8, 0x0D }, { 0x1F7B, 0xD7, 0x0D }, 
   { 0x1F7C, 0xD6, 0x0D }, { 0x1F7D, 0xD5, 0x0D }, { 0x1F7E, 0xD4, 0x0D }, 
   { 0x1F7F, 0xD3, 0x0D }, { 0x1F80, 0xD2, 0x0D }, { 0x1F81, 0xD1, 0x0D }, 
   { 0x1F82, 0xD0, 0x0D }, { 0x1F83, 0xCF, 0x0D }, { 0x1F84, 0xCE, 0x0D }, 
   { 0x1F85, 0xCD, 0x0D }, { 0x1F86, 0xCC, 0x0D }, { 0x1F87, 0xCB, 0x0D }, 
   { 0x1F88, 0xCA, 0x0D }, { 0x1F89, 0xC9, 0x0D }, { 0x1F8A, 0xC8, 0x0D }, 
   { 0x1F8B, 0xC7, 0x0D }, { 0x1F8C, 0xC6, 0x0D }, { 0x1F8D, 0xC5, 0x0D }, 
   { 0x1F8E, 0xC4, 0x0D }, { 0x1F8F, 0xC3, 0x0D }, { 0x1F90, 0xC2, 0x0D }, 
   { 0x1F91, 0xC1, 0x0D }, { 0x1F92, 0xC0, 0x0D }, { 0x1F93, 0xBF, 0x0D }, 
   { 0x1F94, 0xBE, 0x0D }, { 0x1F95, 0xBD, 0x0D }, { 0x1F96, 0xBC, 0x0D }, 
   { 0x1F97, 0xBB, 0x0D }, { 0x1F98, 0xBA, 0x0D }, { 0x1F99, 0xB9, 0x0D }, 
   { 0x1F9A, 0xB8, 0x0D }, { 0x1F9B, 0xB7, 0x0D }, { 0x1F9C, 0xB6, 0x0D }, 
   { 0x1F9D, 0xB5, 0x0D }, { 0x1F9E, 0xB4, 0x0D }, { 0x1F9F, 0xB3, 0x0D }, 
   { 0x1FA0, 0xB2, 0x0D }, { 0x1FA1, 0xB1, 0x0D }, { 0x1FA2, 0xB0, 0x0D }, 
   { 0x1FA3, 0xAF, 0x0D }, { 0x1FA4, 0xAE, 0x0D }, { 0x1FA5, 0xAD, 0x0D }, 
   { 0x1FA6, 0xAC, 0x0D }, { 0x1FA7, 0xAB, 0x0D }, { 0x1FA8, 0xAA, 0x0D }, 
   { 0x1FA9, 0xA9, 0x0D }, { 0x1FAA, 0xA8, 0x0D }, { 0x1FAB, 0xA7, 0x0D }, 
   { 0x1FAC, 0xA6, 0x0D }, { 0x1FAD, 0xA5, 0x0D }, { 0x1FAE, 0xA4, 0x0D }, 
   { 0x1FAF, 0xA3, 0x0D }, { 0x1FB0, 0xA2, 0x0D }, { 0x1FB1, 0xA1, 0x0D }, 
   { 0x1FB2, 0xA0, 0x0D }, { 0x1FB3, 0x9F, 0x0D }, { 0x1FB4, 0x9E, 0x0D }, 
   { 0x1FB5, 0x9D, 0x0D }, { 0x1FB6, 0x9C, 0x0D }, { 0x1FB7, 0x9B, 0x0D }, 
   { 0x1FB8, 0x9A, 0x0D }, { 0x1FB9, 0x99, 0x0D }, { 0x1FBA, 0x98, 0x0D }, 
   { 0x1FBB, 0x97, 0x0D }, { 0x1FBC, 0x96, 0x0D }, { 0x1FBD, 0x95, 0x0D }, 
   { 0x1FBE, 0x94, 0x0D }, { 0x1FBF, 0x93, 0x0D }, { 0x1FC0, 0x92, 0x0D }, 
   { 0x1FC1, 0x91, 0x0D }, { 0x1FC2, 0x90, 0x0D }, { 0x1FC3, 0x8F, 0x0D }, 
   { 0x1FC4, 0x8E, 0x0D }, { 0x1FC5, 0x8D, 0x0D }, { 0x1FC6, 0x8C, 0x0D }, 
   { 0x1FC7, 0x8B, 0x0D }, { 0x1FC8, 0x8A, 0x0D }, { 0x1FC9, 0x89, 0x0D }, 
   { 0x1FCA, 0x88, 0x0D }, { 0x1FCB, 0x87, 0x0D }, { 0x1FCC, 0x86, 0x0D }, 
   { 0x1FCD, 0x85, 0x0D }, { 0x1FCE, 0x84, 0x0D }, { 0x1FCF, 0x83, 0x0D }, 
   { 0x1FD0, 0x82, 0x0D }, { 0x1FD1, 0x81, 0x0D }, { 0x1FD2, 0x80, 0x0D }, 
   { 0x1FD3, 0x7F, 0x0D }, { 0x1FD4, 0x7E, 0x0D }, { 0x1FD5, 0x7C, 0x0D }, 
   { 0x1FD6, 0x5F, 0x0D }, { 0x1FD7, 0x5E, 0x0D }, { 0x1FD8, 0x5D, 0x0D }, 
   { 0x1FD9, 0x5C, 0x0D }, { 0x1FDA, 0x5B, 0x0D }, { 0x1FDB, 0x40, 0x0D }, 
   { 0x1FDC, 0x3E, 0x0D }, { 0x1FDD, 0x3D, 0x0D }, { 0x1FDE, 0x3B, 0x0D }, 
   { 0x1FDF, 0x25, 0x0D }, { 0x1FE0, 0x24, 0x0D }, { 0x1FE1, 0x1F, 0x0D }, 
   { 0x1FE2, 0x1E, 0x0D }, { 0x1FE3, 0x1D, 0x0D }, { 0x1FE4, 0x1C, 0x0D }, 
   { 0x1FE5, 0x1B, 0x0D }, { 0x1FE6, 0x1A, 0x0D }, { 0x1FE7, 0x19, 0x0D }, 
   { 0x1FE8, 0x18, 0x0D }, { 0x1FE9, 0x17, 0x0D }, { 0x1FEA, 0x16, 0x0D }, 
   { 0x1FEB, 0x15, 0x0D }, { 0x1FEC, 0x14, 0x0D }, { 0x1FED, 0x13, 0x0D }, 
   { 0x1FEE, 0x12, 0x0D }, { 0x1FEF, 0x11, 0x0D }, { 0x1FF0, 0x10, 0x0D }, 
   { 0x1FF1, 0x0F, 0x0D }, { 0x1FF2, 0x0E, 0x0D }, { 0x1FF3, 0x0D, 0x0D }, 
   { 0x1FF4, 0x0C, 0x0D }, { 0x1FF5, 0x0B, 0x0D }, { 0x1FF6, 0x09, 0x0D }, 
   { 0x1FF7, 0x08, 0x0D }, { 0x1FF8, 0x07, 0x0D }, { 0x1FF9, 0x06, 0x0D }, 
   { 0x1FFA, 0x05, 0x0D }, { 0x1FFB, 0x04, 0x0D }, { 0x1FFC, 0x03, 0x0D }, 
   { 0x1FFD, 0x02, 0x0D }, { 0x1FFE, 0x01, 0x0D }, { 0x1FFF, 0x00, 0x0D }
};

}  //end of namespace
