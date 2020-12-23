/*
 *  Copyright (C) 2011-2020 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2011 Pulse-Eight (http://www.pulse-eight.com/)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include <string>
#include <vector>

#include <kodi/addon-instance/PVR.h>

struct PVRDemoEpgEntry
{
  int iBroadcastId;
  std::string strTitle;
  int iChannelId;
  time_t startTime;
  time_t endTime;
  std::string strPlotOutline;
  std::string strPlot;
  std::string strIconPath;
  int iGenreType;
  int iGenreSubType;
  //  time_t firstAired;
  //  int iParentalRating;
  //  int iStarRating;
  //  bool bNotify;
  int iSeriesNumber;
  int iEpisodeNumber;
  //  int iEpisodePartNumber;
  std::string strEpisodeName;
};

struct PVRDemoChannel
{
  bool bRadio;
  int iUniqueId;
  int iChannelNumber;
  int iSubChannelNumber;
  int iEncryptionSystem;
  std::string strChannelName;
  std::string strIconPath;
  std::string strStreamURL;
  std::vector<PVRDemoEpgEntry> epg;
};

struct PVRDemoRecording
{
  bool bRadio;
  int iDuration;
  int iGenreType;
  int iGenreSubType;
  int iSeriesNumber;
  int iEpisodeNumber;
  std::string strChannelName;
  std::string strPlotOutline;
  std::string strPlot;
  std::string strRecordingId;
  std::string strStreamURL;
  std::string strTitle;
  std::string strEpisodeName;
  std::string strDirectory;
  time_t recordingTime;
};

struct PVRDemoTimer
{
  int iChannelId;
  time_t startTime;
  time_t endTime;
  PVR_TIMER_STATE state;
  std::string strTitle;
  std::string strSummary;
};

struct PVRDemoChannelGroup
{
  bool bRadio;
  int iGroupId;
  std::string strGroupName;
  int iPosition;
  std::vector<int> members;
};

class TiXmlNode;

class ATTRIBUTE_HIDDEN CPVRDemo : public kodi::addon::CAddonBase,
                                  public kodi::addon::CInstancePVRClient
{
public:
  CPVRDemo();
  ~CPVRDemo() override;

  PVR_ERROR GetBackendName(std::string& name) override;
  PVR_ERROR GetBackendVersion(std::string& version) override;
  PVR_ERROR GetConnectionString(std::string& connection) override;
  PVR_ERROR GetBackendHostname(std::string& hostname) override;

  PVR_ERROR GetCapabilities(kodi::addon::PVRCapabilities& capabilities) override;
  PVR_ERROR GetDriveSpace(uint64_t& total, uint64_t& used) override;

  PVR_ERROR CallEPGMenuHook(const kodi::addon::PVRMenuhook& menuhook,
                            const kodi::addon::PVREPGTag& item) override;
  PVR_ERROR CallChannelMenuHook(const kodi::addon::PVRMenuhook& menuhook,
                                const kodi::addon::PVRChannel& item) override;
  PVR_ERROR CallTimerMenuHook(const kodi::addon::PVRMenuhook& menuhook,
                              const kodi::addon::PVRTimer& item) override;
  PVR_ERROR CallRecordingMenuHook(const kodi::addon::PVRMenuhook& menuhook,
                                  const kodi::addon::PVRRecording& item) override;
  PVR_ERROR CallSettingsMenuHook(const kodi::addon::PVRMenuhook& menuhook) override;

  PVR_ERROR GetEPGForChannel(int channelUid,
                             time_t start,
                             time_t end,
                             kodi::addon::PVREPGTagsResultSet& results) override;
  PVR_ERROR IsEPGTagPlayable(const kodi::addon::PVREPGTag& tag, bool& bIsPlayable) override;
  PVR_ERROR GetEPGTagStreamProperties(
      const kodi::addon::PVREPGTag& tag,
      std::vector<kodi::addon::PVRStreamProperty>& properties) override;
  PVR_ERROR GetChannelGroupsAmount(int& amount) override;
  PVR_ERROR GetChannelGroups(bool bRadio, kodi::addon::PVRChannelGroupsResultSet& results) override;
  PVR_ERROR GetChannelGroupMembers(const kodi::addon::PVRChannelGroup& group,
                                   kodi::addon::PVRChannelGroupMembersResultSet& results) override;
  PVR_ERROR GetChannelsAmount(int& amount) override;
  PVR_ERROR GetChannels(bool bRadio, kodi::addon::PVRChannelsResultSet& results) override;
  PVR_ERROR GetRecordingsAmount(bool deleted, int& amount) override;
  PVR_ERROR GetRecordings(bool deleted, kodi::addon::PVRRecordingsResultSet& results) override;
  PVR_ERROR GetTimerTypes(std::vector<kodi::addon::PVRTimerType>& types) override;
  PVR_ERROR GetTimersAmount(int& amount) override;
  PVR_ERROR GetTimers(kodi::addon::PVRTimersResultSet& results) override;
  PVR_ERROR GetSignalStatus(int channelUid, kodi::addon::PVRSignalStatus& signalStatus) override;
  PVR_ERROR GetChannelStreamProperties(
      const kodi::addon::PVRChannel& channel,
      std::vector<kodi::addon::PVRStreamProperty>& properties) override;
  PVR_ERROR GetRecordingStreamProperties(
      const kodi::addon::PVRRecording& recording,
      std::vector<kodi::addon::PVRStreamProperty>& properties) override;

protected:
  bool LoadDemoData(void);

  std::string GetRecordingURL(const kodi::addon::PVRRecording& recording);
  bool GetChannel(const kodi::addon::PVRChannel& channel, PVRDemoChannel& myChannel);

private:
  PVR_ERROR CallMenuHook(const kodi::addon::PVRMenuhook& menuhook);
  bool ScanXMLChannelData(const TiXmlNode* pChannelNode,
                          int iUniqueChannelId,
                          PVRDemoChannel& channel);
  bool ScanXMLChannelGroupData(const TiXmlNode* pGroupNode,
                               int iUniqueGroupId,
                               PVRDemoChannelGroup& group);
  bool ScanXMLEpgData(const TiXmlNode* pEpgNode);
  bool ScanXMLRecordingData(const TiXmlNode* pRecordingNode,
                            int iUniqueGroupId,
                            PVRDemoRecording& recording);
  bool ScanXMLTimerData(const TiXmlNode* pTimerNode, PVRDemoTimer& timer);

  bool XMLGetInt(const TiXmlNode* pRootNode, const std::string& strTag, int& iIntValue);
  bool XMLGetString(const TiXmlNode* pRootNode, const std::string& strTag, std::string& strStringValue);
  bool XMLGetBoolean(const TiXmlNode* pRootNode, const std::string& strTag, bool& bBoolValue);

  std::vector<PVRDemoChannelGroup> m_groups;
  std::vector<PVRDemoChannel> m_channels;
  std::vector<PVRDemoRecording> m_recordings;
  std::vector<PVRDemoRecording> m_recordingsDeleted;
  std::vector<PVRDemoTimer> m_timers;
  time_t m_iEpgStart;
  std::string m_strDefaultIcon;
  std::string m_strDefaultMovie;
};
