#ifndef CEIT2_H_
#define CEIT2_H_
#include <libsi/section.h>
#include <libsi/descriptor.h>
#include <libsi/si.h>
#include <vdr/epg.h>
#include "util.h"

using namespace util;
namespace SI
{

enum TableIdExt {
  TableIdSKYChannels = TableIdBAT, //SKYBOX channels information section 0x4A same as TableIdBAT
  TableIdMHW1TitlesSummaries = 0x90, //MHW1 titles and summaries information section
  TableIdMHW1Channels = 0x91, //MHW1 channels information section
  TableIdMHW1Themes = 0x92, //MHW1 themes information section
  TableIdMHW2Summaries = 0x96, //MHW2 summaries information section
  //SKYBOX Titles range from 0xA0 to 0xA3. 0xA0 is also TableIdPremiereCIT
  TableIdSKYTitlesA0 = TableIdPremiereCIT, //SKYBOX titles information section start
  TableIdSKYTitles_first = 0xA1, //SKYBOX titles information section start
  TableIdSKYTitles_last = 0xA3, //SKYBOX titles information section end
  //SKYBOX Summaries range from 0xA8 to 0xAB.
  TableIdSKYSummaries_first = 0xA8, //SKYBOX Summaries information section start
  TableIdSKYSummaries_last = 0xAB, //SKYBOX Summaries information section end
  TableIdNagraCIT = 0xB0, //NagraGuide content information section
  TableIdMHW2ChannelsThemes = 0xC8, //MHW1 channels and themes information section
  TableIdMHW2Titles = 0xE6 //MHW2 titles information section

};


enum DescriptorTagExt {
  DishRatingDescriptorTag = 0x89,
  SkyOTVDescriptorTag = 0x90,
  DishShortEventDescriptorTag = 0x91,
  DishExtendedEventDescriptorTag = 0x92,
  DishSeriesDescriptorTag = 0x96,
  MHW1DescriptorTag = 0xC1,
  MHW1_2DescriptorTag = 0xC2,
  FreeviewDescriptorTag = 0xD1,
  PremiereOrderInfoDescriptorTag = 0xF0,
  PremiereRatingInfoDescriptorTag = 0xF1,
};

// typedef InheritEnum< DescriptorTagExt, SI::DescriptorTag > ExtendedDescriptorTag;

/*extern const char *getCharacterTable(const unsigned char *&buffer, int &length, bool *isSingleByte = NULL);
extern bool convertCharacterTable(const char *from, size_t fromLength, char *to, size_t toLength, const char *fromCode);
extern bool SystemCharacterTableIsSingleByte;*/
class cEIT2:public SI::EIT
{
public:
  cEIT2 (int Source, u_char Tid, const u_char * Data, EFormat format, bool isEITPid = false);
  cEIT2 (cSchedule * Schedule, EFormat format);
  //protected:
  //  void updateEquivalent(cSchedules * Schedules, tChannelID channelID, cEvent *pEvent);
  cEvent* ProcessEitEvent(cSchedule *Schedule, const SI::EIT::Event *EitEvent, uchar TableID, uchar Version);

private:
  void ProcessEventDescriptors(bool ExternalData, int Source, u_char Tid,
                               const SI::EIT::Event* SiEitEvent, cEvent* pEvent,
                               const tChannelID& channelID);

private:
  bool Empty;
  bool Modified;
  //    bool HasExternalData = false;
  bool OnlyRunningStatus;
  time_t SegmentStart;
  time_t SegmentEnd;
  EFormat Format;
  const cChannel* channel;
};
} //end namespace SI

#endif /* CEIT2_H_ */
