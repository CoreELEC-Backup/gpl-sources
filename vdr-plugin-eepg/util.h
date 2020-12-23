/*
 * util.h
 *
 *  Created on: 23.5.2012
 *      Author: d.petrovski
 */

#ifndef UTIL_H_
#define UTIL_H_
#include <time.h>
#include <stdlib.h>
#include <string>
#include <map>

class cChannel;
struct tChannelID;
class cEvent;
class cEquivHandler;
class cSchedules;
class cCharSetConv;

#define START   '\0'
#define STOP    '\0'
#define ESCAPE  '\1'



#define Asprintf(a, b, c...) void( asprintf(a, b, c) < 0 ? esyslog("memory allocation error - %s", b) : void() )

namespace util
{

static const char CATEGORY[] = "Category: ";
static const char GENRE[] = "Genre: ";

extern int AvailableSources[32];
extern int NumberOfAvailableSources;

extern int Yesterday;
extern int YesterdayEpoch;
extern int YesterdayEpochUTC;

enum EFormat
{
//First all batchmode, load ONCE protocols:
  MHW1 = 0,
  MHW2,
  SKY_IT,
  SKY_UK,
  NAGRA,
//Than all CONTinuous protocols, so they will be processed LAST:
  PREMIERE,
  FREEVIEW,
  DISH_BEV,
  EIT,
//the highest number of EPG-formats that is supported by this plugin
  HIGHEST_FORMAT = EIT
};

extern cEquivHandler* EquivHandler;

void AddEvent(cEvent *event, tChannelID ChannelID);

const cChannel *GetChannelByID(const tChannelID & channelID, bool searchOtherPos);
time_t LocalTime2UTC(time_t t);
time_t UTC2LocalTime(time_t t);
void GetLocalTimeOffset(void);
void CleanString(unsigned char *String);
void decodeText2(const unsigned char *from, int len, char *buffer, int buffsize);
char *freesat_huffman_decode(const unsigned char *src, size_t size);
void sortSchedules(cSchedules * Schedules, tChannelID channelID);
/**
 * Locate the translation of a given Theme (Category, Genre) string in the translation map
 * @param the text to search
 * @return found translation or the source text if not found.
 */
std::string findThemeTr(const char*);

//struct sNode
//{
//  char *Value;
//  struct sNode *P0;
//  struct sNode *P1;
//};
//
//typedef struct sNode sNodeH;

template<class T> T REALLOC(T Var, size_t Size)
{
  T p = (T) realloc(Var, Size);
  if (!p) free(Var);
  return p;
}

struct hufftab
{
  unsigned int value;
  short bits;
  char next;
};

extern struct hufftab *tables[2][128];
extern int table_size[2][128];
//static sNodeH* sky_tables[2];
extern std::map<std::string,std::string> tableDict;


class cCharsetFixer
{
public:
  cCharsetFixer();
  virtual ~cCharsetFixer();
  const char*  FixCharset(const char* text);
  void InitCharsets(const cChannel* Channel);

private:
  cCharSetConv* conv_revert;//("UTF-8",CharsetOverride);
  cCharSetConv* conv_to;//(fixCharset.c_str());
  const char* charsetOverride;
  std::string fixedCharset;
  std::string initialCharset;

};

}
#endif /* UTIL_H_ */
