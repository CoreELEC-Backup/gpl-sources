/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "rest.h"

#include <atomic>
#include <condition_variable>
#include <json/json.h>
#include <kodi/addon-instance/PVR.h>
#include <mutex>
#include <thread>

#define PCTV_REST_INTERFACE false

#define CHANNELDATAVERSION 2

#define URI_REST_CONFIG "/TVC/free/data/config"
#define URI_REST_CHANNELS "/TVC/user/data/tv/channels"
#define URI_REST_CHANNELLISTS "/TVC/user/data/tv/channellists"
#define URI_REST_RECORDINGS "/TVC/user/data/gallery/video"
#define URI_REST_TIMER "/TVC/user/data/recordingtasks"
#define URI_REST_EPG "/TVC/user/data/epg"
#define URI_REST_STORAGE "/TVC/user/data/storage"
#define URI_REST_FOLDER "/TVC/user/data/folder"

#define DEFAULT_TV_PIN "0000"

#define URI_INDEX_HTML "/TVC/common/Login.html"

#define DEFAULT_PREVIEW_MODE "m2ts"
#define DEFAULT_PROFILE "m2ts.Native.NR"
#define DEFAULT_REC_PROFILE "m2ts.4000k.HR"

typedef enum PCTV_UPDATE_STATE
{
  PCTV_UPDATE_STATE_NONE,
  PCTV_UPDATE_STATE_FOUND,
  PCTV_UPDATE_STATE_UPDATED,
  PCTV_UPDATE_STATE_NEW
} PCTV_UPDATE_STATE;


struct PctvChannel
{
  bool bRadio;
  int iUniqueId;
  int iChannelNumber;
  int iSubChannelNumber;
  int iEncryptionSystem;
  std::string strChannelName;
  std::string strLogoPath;
  std::string strStreamURL;

  bool operator<(const PctvChannel& channel) const
  {
    return (strChannelName.compare(channel.strChannelName) < 0);
  }
};

struct PctvChannelGroup
{
  bool bRadio;
  int iGroupId;
  std::string strGroupName;
  std::vector<int> members;
};

struct PctvEpgEntry
{
  int iBroadcastId;
  int iChannelId;
  int iGenreType;
  int iGenreSubType;
  time_t startTime;
  time_t endTime;
  std::string strTitle;
  std::string strPlotOutline;
  std::string strPlot;
  std::string strIconPath;
  std::string strGenreString;
};

struct PctvEpgChannel
{
  std::string strId;
  std::string strName;
  std::vector<PctvEpgEntry> epg;
};

struct PctvTimer
{
  int iId;
  std::string strTitle;
  int iChannelId;
  time_t startTime;
  time_t endTime;
  int iStartOffset;
  int iEndOffset;
  std::string strProfile;
  std::string strResult;
  PVR_TIMER_STATE state;
};

struct PctvRecording
{
  std::string strRecordingId;
  time_t startTime;
  int iDuration;
  int iLastPlayedPosition;
  std::string strTitle;
  std::string strStreamURL;
  std::string strPlot;
  std::string strPlotOutline;
  std::string strChannelName;
  std::string strDirectory;
  std::string strIconPath;
};

struct PctvConfig
{

  std::string Brand;
  std::string Caps;
  std::string Hostname;
  int Port;
  std::string GuestLink;

  void init(const Json::Value& data)
  {
    Brand = data["Brand"].asString();
    Caps = data["Caps"].asString();
    Hostname = data["Hostname"].asString();
    Port = data["Port"].asInt();
    GuestLink = data["GuestLink"].asString();
  }

  bool hasCapability(const std::string& cap)
  {
    std::string caps = ";" + Caps + ";";
    if (caps.find(";" + cap + ";") != std::string::npos)
    {
      return true;
    }
    return false;
  }
};

class ATTRIBUTE_HIDDEN Pctv : public kodi::addon::CInstancePVRClient
{
public:
  /* Class interface */
  Pctv(const std::string strHostname,
       int iPortWeb,
       const std::string& strPin,
       int iBitrate,
       bool bTranscode,
       bool bUsePIN,
       KODI_HANDLE instance,
       const std::string& kodiVersion);
  ~Pctv();

  /* Server */
  bool Open();
  bool IsConnected();

  /* Common */
  PVR_ERROR GetCapabilities(kodi::addon::PVRCapabilities& capabilities) override;
  PVR_ERROR GetBackendName(std::string& name) override;
  PVR_ERROR GetBackendVersion(std::string& version) override;
  PVR_ERROR GetBackendHostname(std::string& hostname) override;
  PVR_ERROR GetConnectionString(std::string& connection) override;
  bool IsSupported(const std::string& cap);

  /* Channels */
  PVR_ERROR GetChannelsAmount(int& amount) override;
  PVR_ERROR GetChannels(bool radio, kodi::addon::PVRChannelsResultSet& results) override;
  PVR_ERROR GetChannelStreamProperties(
      const kodi::addon::PVRChannel& channel,
      std::vector<kodi::addon::PVRStreamProperty>& properties) override;

  /* Groups */
  PVR_ERROR GetChannelGroupsAmount(int& amount) override;
  PVR_ERROR GetChannelGroups(bool radio, kodi::addon::PVRChannelGroupsResultSet& results) override;
  PVR_ERROR GetChannelGroupMembers(const kodi::addon::PVRChannelGroup& group,
                                   kodi::addon::PVRChannelGroupMembersResultSet& results) override;

  /* Recordings */
  PVR_ERROR GetRecordingsAmount(bool deleted, int& amount) override;
  PVR_ERROR GetRecordings(bool deleted, kodi::addon::PVRRecordingsResultSet& results) override;
  PVR_ERROR GetRecordingStreamProperties(
      const kodi::addon::PVRRecording& recording,
      std::vector<kodi::addon::PVRStreamProperty>& properties) override;

  /* Timer */
  PVR_ERROR GetTimerTypes(std::vector<kodi::addon::PVRTimerType>& types) override;
  PVR_ERROR GetTimersAmount(int& amount) override;
  PVR_ERROR GetTimers(kodi::addon::PVRTimersResultSet& results) override;
  PVR_ERROR AddTimer(const kodi::addon::PVRTimer& timer) override;

  /* EPG */
  PVR_ERROR GetEPGForChannel(int channelUid,
                             time_t start,
                             time_t end,
                             kodi::addon::PVREPGTagsResultSet& results) override;
  bool GetEPG(int id, time_t iStart, time_t iEnd, Json::Value& data);

  /* Storage */
  PVR_ERROR GetDriveSpace(uint64_t& total, uint64_t& used) override;

private:
  // helper functions
  bool LoadChannels();
  bool GetFreeConfig();
  unsigned int GetEventId(long long EntryId);
  /*
  * \brief Get a channel list from PCTV Device via REST interface
  * \param id The channel list id
  */
  int RESTGetChannelList(int id, Json::Value& response);
  int RESTGetRecordings(Json::Value& response);
  int RESTGetChannelLists(Json::Value& response);
  int RESTGetTimer(Json::Value& response);
  int RESTGetEpg(int id, time_t iStart, time_t iEnd, Json::Value& response);
  int RESTGetStorage(Json::Value& response);
  int RESTGetFolder(Json::Value& response);

  int RESTAddTimer(const kodi::addon::PVRTimer& timer, Json::Value& response);

  // helper functions
  std::string URLEncodeInline(const std::string& sSrc);
  void TransferChannels(kodi::addon::PVRChannelsResultSet& results);
  void TransferRecordings(kodi::addon::PVRRecordingsResultSet& results);
  void TransferTimer(kodi::addon::PVRTimersResultSet& results);
  void TransferGroups(kodi::addon::PVRChannelGroupsResultSet& results);
  bool IsRecordFolderSet(std::string& partitionId);
  bool GetRecordingFromLocation(std::string strRecordingFolder);
  bool GetChannel(const kodi::addon::PVRChannel& channel, PctvChannel& myChannel);
  std::string GetPreviewParams(int dataIdentifier, Json::Value entry);
  std::string GetPreviewUrl(std::string params);
  std::string GetTranscodeProfileValue();
  std::string GetStid(int id);
  std::string GetChannelLogo(Json::Value entry);
  std::string GetShortName(Json::Value entry);

  void Process();

  // members
  std::thread m_thread;
  std::mutex m_mutex;
  std::condition_variable m_started;
  std::atomic<bool> m_running = {false};

  // Setting values
  std::string m_strHostname;
  int m_iPortWeb;
  std::string m_strPin;
  int m_iBitrate;
  bool m_bTranscode;
  bool m_bUsePIN;

  int m_iDataIdentifier;
  bool m_bIsConnected = false;
  std::string m_strBaseUrl;
  std::string m_strBackendName;
  std::string m_strBackendVersion;
  PctvConfig m_config = {"", "", "", 0, ""};
  int m_iNumChannels = 0;
  int m_iNumRecordings = 0;
  int m_iNumGroups = 0;
  std::string m_strPreviewMode = DEFAULT_PREVIEW_MODE;
  std::string m_strStid;
  bool m_bUpdating = false;
  std::string m_strBackendUrlNoAuth;

  std::vector<PctvEpgChannel> m_epg;
  std::vector<PctvChannel> m_channels;
  std::vector<PctvChannelGroup> m_groups;
  std::vector<PctvRecording> m_recordings;
  std::vector<PctvTimer> m_timer;
  std::vector<std::string> m_partitions;
};
