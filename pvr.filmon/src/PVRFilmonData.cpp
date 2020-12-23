/*
 *  Copyright (C) 2011-2020 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2011 Pulse-Eight (http://www.pulse-eight.com/)
 *  Copyright (C) 2014 Stephen Denham
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "PVRFilmonData.h"

#include <iostream>
#include <sstream>
#include <string>

#include <kodi/General.h>

PVRFilmonData::PVRFilmonData(void) : m_filmonApi(*this)
{
}

PVRFilmonData::~PVRFilmonData(void)
{
  m_filmonApi.Delete();
}

ADDON_STATUS PVRFilmonData::Create()
{
  kodi::Log(ADDON_LOG_DEBUG, "%s - Creating the PVR Filmon add-on", __FUNCTION__);

  ReadSettings();

  ADDON_STATUS curStatus;
  if (Load())
  {
    kodi::Log(ADDON_LOG_DEBUG, "%s - Created the PVR Filmon add-on", __FUNCTION__);
    curStatus = ADDON_STATUS_OK;
  }
  else
  {
    kodi::Log(ADDON_LOG_ERROR, "%s - Failed to connect to Filmon, check settings", __FUNCTION__);
    curStatus = ADDON_STATUS_LOST_CONNECTION;
  }

  return curStatus;
}

void PVRFilmonData::ReadSettings()
{
  kodi::Log(ADDON_LOG_DEBUG, "%s - read PVR Filmon settings", __FUNCTION__);

  m_username = kodi::GetSettingString("username");
  m_password = kodi::GetSettingString("password");
  m_preferHd = kodi::GetSettingBoolean("preferhd");
  m_favouriteChannelsOnly = kodi::GetSettingBoolean("favouritechannelsonly");
}

ADDON_STATUS PVRFilmonData::SetSetting(const std::string& settingName,
                                       const kodi::CSettingValue& settingValue)
{
  if (settingName == "username")
  {
    std::string tmp_sUsername = m_username;
    m_username = settingValue.GetString();
    if (tmp_sUsername != m_username)
    {
      kodi::Log(ADDON_LOG_INFO, "%s - Changed Setting 'username'", __FUNCTION__);
      return ADDON_STATUS_NEED_RESTART;
    }
  }
  else if (settingName == "password")
  {
    std::string tmp_sPassword = m_password;
    m_password = settingValue.GetString();
    if (tmp_sPassword != m_password)
    {
      kodi::Log(ADDON_LOG_INFO, "%s - Changed Setting 'password'", __FUNCTION__);
      return ADDON_STATUS_NEED_RESTART;
    }
  }
  else if (settingName == "preferhd")
  {
    bool tmp_boolPreferHd = m_preferHd;
    m_preferHd = settingValue.GetBoolean();
    if (tmp_boolPreferHd != m_preferHd)
    {
      kodi::Log(ADDON_LOG_INFO, "%s - Changed Setting 'preferhd'", __FUNCTION__);
      return ADDON_STATUS_NEED_RESTART;
    }
  }
  else if (settingName == "favouritechannelsonly")
  {
    bool tmp_boolFavouriteChannelsOnly = m_favouriteChannelsOnly;
    m_favouriteChannelsOnly = settingValue.GetBoolean();
    if (tmp_boolFavouriteChannelsOnly != m_favouriteChannelsOnly)
    {
      kodi::Log(ADDON_LOG_INFO, "%s - Changed Setting 'favouritechannelsonly'", __FUNCTION__);
      return ADDON_STATUS_NEED_RESTART;
    }
  }
  return ADDON_STATUS_OK;
}

bool PVRFilmonData::Load()
{
  std::lock_guard<std::mutex> lock(m_mutex);
  bool res = m_filmonApi.Create();
  if (res)
  {
    res = m_filmonApi.Login(m_username, m_password, m_favouriteChannelsOnly);
    if (res)
    {
      kodi::addon::CInstancePVRClient::ConnectionStateChange("", PVR_CONNECTION_STATE_CONNECTED, "");
      m_lastTimeChannels = 0;
      m_lastTimeGroups = 0;
    }
    else
    {
      kodi::addon::CInstancePVRClient::ConnectionStateChange("", PVR_CONNECTION_STATE_ACCESS_DENIED, "");
    }
  }

  m_onLoad = true;
  return res;
}

PVR_ERROR PVRFilmonData::GetCapabilities(kodi::addon::PVRCapabilities& capabilities)
{
  capabilities.SetSupportsTV(true);
  capabilities.SetSupportsEPG(true);
  capabilities.SetSupportsRecordings(true);
  capabilities.SetSupportsRecordingsUndelete(false);
  capabilities.SetSupportsTimers(true);
  capabilities.SetSupportsChannelGroups(true);
  capabilities.SetSupportsRadio(false);
  capabilities.SetHandlesInputStream(false);
  capabilities.SetHandlesDemuxing(false);
  capabilities.SetSupportsChannelScan(false);
  capabilities.SetSupportsLastPlayedPosition(false);
  capabilities.SetSupportsRecordingEdl(false);
  capabilities.SetSupportsRecordingsRename(false);
  capabilities.SetSupportsRecordingsLifetimeChange(false);
  capabilities.SetSupportsDescrambleInfo(false);

  kodi::Log(ADDON_LOG_DEBUG, "%s - got PVR Filmon capabilities", __FUNCTION__);

  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR PVRFilmonData::GetBackendName(std::string& name)
{
  name = "Filmon API";
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR PVRFilmonData::GetBackendVersion(std::string& version)
{
  version = "2.2";
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR PVRFilmonData::GetConnectionString(std::string& connection)
{
  connection = m_filmonApi.GetConnectionString();
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR PVRFilmonData::GetDriveSpace(uint64_t& total, uint64_t& used)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  kodi::Log(ADDON_LOG_DEBUG, "getting user storage from API");
  m_filmonApi.GetUserStorage(total, used);
  total = total / 10;
  used = used / 10;
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR PVRFilmonData::GetChannelsAmount(int& amount)
{
  unsigned int chCount = m_filmonApi.GetChannelCount();
  kodi::Log(ADDON_LOG_DEBUG, "channel count is %d ", chCount);
  amount = chCount;
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR PVRFilmonData::GetChannels(bool radio, kodi::addon::PVRChannelsResultSet& results)
{
  kodi::Log(ADDON_LOG_DEBUG, "%s - getting PVR Filmon channels", __FUNCTION__);

  std::lock_guard<std::mutex> lock(m_mutex);
  bool res = false;
  bool expired = false;
  if (std::time(nullptr) - m_lastTimeChannels > FILMON_CACHE_TIME)
  {
    kodi::Log(ADDON_LOG_DEBUG, "cache expired, getting channels from API");
    m_channels.clear();
    expired = true;
  }

  std::vector<unsigned int> channelList = m_filmonApi.GetChannels();
  unsigned int channelCount = channelList.size();
  unsigned int channelId = 0;

  for (unsigned int i = 0; i < channelCount; i++)
  {
    FilmonChannel channel;
    channelId = channelList[i];
    if (expired)
    {
      res = m_filmonApi.GetChannel(channelId, &channel, m_preferHd);
      if (m_onLoad == true)
      {
        kodi::QueueFormattedNotification(QUEUE_INFO, "Filmon loaded %s", channel.strChannelName.c_str());
      }
    }
    else
    {
      for (unsigned int j = 0; j < m_channels.size(); j++)
      {
        if (m_channels[j].iUniqueId == channelId)
        {
          channel = m_channels[j];
          res = true;
          break;
        }
      }
    }
    if (res && channel.bRadio == radio)
    {
      kodi::addon::PVRChannel xbmcChannel;

      xbmcChannel.SetUniqueId(channel.iUniqueId);
      xbmcChannel.SetIsRadio(false);
      xbmcChannel.SetChannelNumber(channel.iChannelNumber);
      xbmcChannel.SetChannelName(channel.strChannelName);
      xbmcChannel.SetEncryptionSystem(channel.iEncryptionSystem);
      xbmcChannel.SetIconPath(channel.strIconPath);
      xbmcChannel.SetIsHidden(false);
      if (expired)
      {
        m_channels.emplace_back(channel);
      }
      results.Add(xbmcChannel);
    }
  }
  if (m_lastTimeChannels == 0)
  {
    kodi::QueueFormattedNotification(QUEUE_INFO, "Filmon loaded %d channels", m_channels.size());
  }
  if (expired)
  {
    m_lastTimeChannels = std::time(nullptr);
  }
  m_onLoad = false;
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR PVRFilmonData::GetSignalStatus(int channelUid, kodi::addon::PVRSignalStatus& signalStatus)
{
  signalStatus.SetAdapterName("Filmon API");
  signalStatus.SetAdapterStatus("OK");

  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR PVRFilmonData::GetChannelGroupsAmount(int& amount)
{
  kodi::Log(ADDON_LOG_DEBUG, "getting number of groups");
  amount = static_cast<int>(m_groups.size());
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR PVRFilmonData::GetChannelStreamProperties(const kodi::addon::PVRChannel& channel,
                                                    std::vector<kodi::addon::PVRStreamProperty>& properties)
{
  std::string strUrl;
  std::lock_guard<std::mutex> lock(m_mutex);
  for (const auto& FilMonchannel : m_channels)
  {
    if (channel.GetUniqueId() == FilMonchannel.iUniqueId)
    {
      strUrl = FilMonchannel.strStreamURL;
      break;
    }
  }

  if (strUrl.empty())
  {
    return PVR_ERROR_FAILED;
  }
  properties.emplace_back(PVR_STREAM_PROPERTY_STREAMURL, strUrl);
  properties.emplace_back(PVR_STREAM_PROPERTY_ISREALTIMESTREAM, "true");

  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR PVRFilmonData::GetChannelGroups(bool radio, kodi::addon::PVRChannelGroupsResultSet& results)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  if (radio == false)
  {
    if (std::time(nullptr) - m_lastTimeGroups > FILMON_CACHE_TIME)
    {
      kodi::Log(ADDON_LOG_DEBUG, "cache expired, getting channel groups from API");
      m_groups = m_filmonApi.GetChannelGroups();
      m_lastTimeGroups = std::time(nullptr);
    }
    for (unsigned int grpId = 0; grpId < m_groups.size(); grpId++)
    {
      PVRFilmonChannelGroup group = m_groups[grpId];
      kodi::addon::PVRChannelGroup xbmcGroup;
      xbmcGroup.SetIsRadio(radio);
      xbmcGroup.SetPosition(0); // groups default order, unused
      xbmcGroup.SetGroupName(group.strGroupName);
      results.Add(xbmcGroup);
      kodi::Log(ADDON_LOG_DEBUG, "found group %s", xbmcGroup.GetGroupName().c_str());
    }
  }
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR PVRFilmonData::GetChannelGroupMembers(const kodi::addon::PVRChannelGroup& group,
                                                kodi::addon::PVRChannelGroupMembersResultSet& results)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  if (std::time(nullptr) - m_lastTimeGroups > FILMON_CACHE_TIME)
  {
    kodi::Log(ADDON_LOG_DEBUG, "cache expired, getting channel groups members from API");
    m_groups = m_filmonApi.GetChannelGroups();
    m_lastTimeGroups = std::time(nullptr);
  }
  for (unsigned int grpId = 0; grpId < m_groups.size(); grpId++)
  {
    PVRFilmonChannelGroup grp = m_groups.at(grpId);
    if (grp.strGroupName == group.GetGroupName())
    {
      for (unsigned int chId = 0; chId < grp.members.size(); chId++)
      {
        kodi::addon::PVRChannelGroupMember xbmcGroupMember;
        xbmcGroupMember.SetGroupName(group.GetGroupName());
        xbmcGroupMember.SetChannelUniqueId(grp.members[chId]);
        xbmcGroupMember.SetChannelNumber(grp.members[chId]);
        kodi::Log(ADDON_LOG_DEBUG, "add member %d", grp.members[chId]);
        results.Add(xbmcGroupMember);
      }
      break;
    }
  }
  return PVR_ERROR_NO_ERROR;
}

// Returns index into channel vector, refreshes channel entry
int PVRFilmonData::UpdateChannel(unsigned int channelId)
{
  int index = -1;
  kodi::Log(ADDON_LOG_DEBUG, "updating channel %d ", channelId);
  for (unsigned int i = 0; i < m_channels.size(); i++)
  {
    if (m_channels[i].iUniqueId == channelId)
    {
      if (std::time(nullptr) - m_lastTimeChannels > FILMON_CACHE_TIME)
      {
        kodi::Log(ADDON_LOG_DEBUG, "cache expired, getting channel from API");
        m_filmonApi.GetChannel(channelId, &m_channels[i], m_preferHd);
      }
      index = i;
      break;
    }
  }
  return index;
}

// Called periodically to refresh EPG
PVR_ERROR PVRFilmonData::GetEPGForChannel(int channelUid, time_t start, time_t end,
                                          kodi::addon::PVREPGTagsResultSet& results)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  kodi::Log(ADDON_LOG_DEBUG, "getting EPG for channel");
  unsigned int broadcastIdCount = m_lastTimeChannels;
  int chIndex = PVRFilmonData::UpdateChannel(channelUid);
  if (chIndex >= 0)
  {
    PVRFilmonChannel ch = m_channels[chIndex];
    for (unsigned int epgId = 0; epgId < ch.epg.size(); epgId++)
    {
      PVRFilmonEpgEntry& epgEntry = ch.epg.at(epgId);
      if (epgEntry.startTime >= start && epgEntry.endTime <= end)
      {
        kodi::addon::PVREPGTag tag;
        tag.SetUniqueBroadcastId(broadcastIdCount++);
        tag.SetTitle(epgEntry.strTitle);
        tag.SetUniqueChannelId(epgEntry.iChannelId);
        tag.SetStartTime(epgEntry.startTime);
        tag.SetEndTime(epgEntry.endTime);
        tag.SetPlotOutline(epgEntry.strPlotOutline);
        tag.SetPlot(epgEntry.strPlot);
        tag.SetOriginalTitle(""); // unused
        tag.SetCast(""); // unused
        tag.SetDirector(""); // unused
        tag.SetWriter(""); // unused
        tag.SetYear(0); // unused
        tag.SetIMDBNumber(""); // unused
        tag.SetIconPath(epgEntry.strIconPath);
        tag.SetGenreType(epgEntry.iGenreType);
        tag.SetGenreSubType(epgEntry.iGenreSubType);
        tag.SetGenreDescription("");
        tag.SetFirstAired("");
        tag.SetParentalRating(0);
        tag.SetStarRating(0);
        tag.SetSeriesNumber(EPG_TAG_INVALID_SERIES_EPISODE);
        tag.SetEpisodeNumber(EPG_TAG_INVALID_SERIES_EPISODE);
        tag.SetEpisodePartNumber(EPG_TAG_INVALID_SERIES_EPISODE);
        tag.SetEpisodeName("");
        tag.SetFlags(EPG_TAG_FLAG_UNDEFINED);
        results.Add(tag);
      }
    }
    if (std::time(nullptr) - m_lastTimeChannels > FILMON_CACHE_TIME)
    {
      // Get PVR to re-read refreshed channels
      if (m_filmonApi.Login(m_username, m_password, m_favouriteChannelsOnly))
      {
        kodi::addon::CInstancePVRClient::TriggerChannelGroupsUpdate();
        kodi::addon::CInstancePVRClient::TriggerChannelUpdate();
      }
    }
    return PVR_ERROR_NO_ERROR;
  }
  else
  {
    return PVR_ERROR_SERVER_ERROR;
  }
}

PVR_ERROR PVRFilmonData::GetRecordingsAmount(bool deleted, int& amount)
{
  kodi::Log(ADDON_LOG_DEBUG, "getting number of recordings");
  amount = static_cast<int>(m_recordings.size());
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR PVRFilmonData::GetRecordings(bool deleted, kodi::addon::PVRRecordingsResultSet& results)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  kodi::Log(ADDON_LOG_DEBUG, "getting recordings from API");
  m_recordings = m_filmonApi.GetRecordings();
  for (std::vector<PVRFilmonRecording>::iterator it = m_recordings.begin();
       it != m_recordings.end(); it++)
  {
    PVRFilmonRecording& recording = *it;
    kodi::addon::PVRRecording xbmcRecording;
    xbmcRecording.SetSeriesNumber(PVR_RECORDING_INVALID_SERIES_EPISODE);
    xbmcRecording.SetEpisodeNumber(PVR_RECORDING_INVALID_SERIES_EPISODE);

    xbmcRecording.SetDuration(recording.iDuration);
    xbmcRecording.SetGenreType(recording.iGenreType);
    xbmcRecording.SetGenreSubType(recording.iGenreSubType);
    xbmcRecording.SetRecordingTime(recording.recordingTime);

    xbmcRecording.SetChannelName(recording.strChannelName);
    xbmcRecording.SetPlotOutline(recording.strPlotOutline);
    xbmcRecording.SetPlot(recording.strPlot);
    xbmcRecording.SetRecordingId(recording.strRecordingId);
    xbmcRecording.SetTitle(recording.strTitle);
    xbmcRecording.SetDirectory("Filmon");
    xbmcRecording.SetIconPath(recording.strIconPath);
    xbmcRecording.SetThumbnailPath(recording.strThumbnailPath);

    /* TODO: PVR API 5.0.0: Implement this */
    xbmcRecording.SetChannelUid(PVR_CHANNEL_INVALID_UID);

    /* TODO: PVR API 5.1.0: Implement this */
    xbmcRecording.SetChannelType(PVR_RECORDING_CHANNEL_TYPE_UNKNOWN);

    results.Add(xbmcRecording);
  }
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR PVRFilmonData::GetRecordingStreamProperties(const kodi::addon::PVRRecording& recording,
                                                      std::vector<kodi::addon::PVRStreamProperty>& properties)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  std::string strRecordingFile;
  m_recordings = m_filmonApi.GetRecordings();
  for (const auto& FilMonRecording : m_recordings)
  {
    if (FilMonRecording.strRecordingId == recording.GetRecordingId())
    {
      strRecordingFile = FilMonRecording.strStreamURL;
      break;
    }
  }

  if (strRecordingFile.empty())
    return PVR_ERROR_SERVER_ERROR;

  properties.emplace_back(PVR_STREAM_PROPERTY_STREAMURL, strRecordingFile);
  properties.emplace_back(PVR_STREAM_PROPERTY_ISREALTIMESTREAM, "false");
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR PVRFilmonData::DeleteRecording(const kodi::addon::PVRRecording& recording)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  kodi::Log(ADDON_LOG_DEBUG, "deleting recording %s", recording.GetRecordingId().c_str());
  if (m_filmonApi.DeleteRecording(std::atoi(recording.GetRecordingId().c_str())))
  {
    kodi::addon::CInstancePVRClient::TriggerRecordingUpdate();
    return PVR_ERROR_NO_ERROR;
  }
  else
  {
    return PVR_ERROR_SERVER_ERROR;
  }
}

PVR_ERROR PVRFilmonData::GetTimerTypes(std::vector<kodi::addon::PVRTimerType>& types)
{
  /* TODO: Implement this to get support for the timer features introduced with
   * PVR API 1.9.7 */
  return PVR_ERROR_NOT_IMPLEMENTED;
}

PVR_ERROR PVRFilmonData::GetTimersAmount(int& amount)
{
  kodi::Log(ADDON_LOG_DEBUG, "getting number of timers");
  amount = static_cast<int>(m_timers.size());
  return PVR_ERROR_NO_ERROR;
}

// Gets called every 5 minutes, same as Filmon session lifetime
PVR_ERROR PVRFilmonData::GetTimers(kodi::addon::PVRTimersResultSet& results)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  kodi::Log(ADDON_LOG_DEBUG, "getting timers from API");
  if (m_filmonApi.KeepAlive())
  { // Keeps session alive
    m_timers = m_filmonApi.GetTimers();
    for (std::vector<PVRFilmonTimer>::iterator it = m_timers.begin(); it != m_timers.end(); it++)
    {
      PVRFilmonTimer& timer = *it;
      if ((PVR_TIMER_STATE)timer.state < PVR_TIMER_STATE_COMPLETED)
      {
        kodi::addon::PVRTimer xbmcTimer;

        /* TODO: Implement own timer types to get support for the timer features
          * introduced with PVR API 1.9.7 */
        xbmcTimer.SetTimerType(PVR_TIMER_TYPE_NONE);

        xbmcTimer.SetClientIndex(timer.iClientIndex);
        xbmcTimer.SetClientChannelUid(timer.iClientChannelUid);
        xbmcTimer.SetTitle(timer.strTitle);
        xbmcTimer.SetSummary(timer.strSummary);
        xbmcTimer.SetStartTime(timer.startTime);
        xbmcTimer.SetEndTime(timer.endTime);
        xbmcTimer.SetState((PVR_TIMER_STATE)timer.state);
        xbmcTimer.SetFirstDay(timer.firstDay);
        xbmcTimer.SetWeekdays(timer.iWeekdays);
        xbmcTimer.SetEPGUid(timer.iEpgUid);
        xbmcTimer.SetGenreType(timer.iGenreType);
        xbmcTimer.SetGenreSubType(timer.iGenreSubType);
        xbmcTimer.SetMarginStart(timer.iMarginStart);
        xbmcTimer.SetMarginEnd(timer.iMarginEnd);

        results.Add(xbmcTimer);
      }
    }
    kodi::addon::CInstancePVRClient::TriggerRecordingUpdate();
    return PVR_ERROR_NO_ERROR;
  }
  else
  {
    return PVR_ERROR_SERVER_ERROR;
  }
}

PVR_ERROR PVRFilmonData::AddTimer(const kodi::addon::PVRTimer& timer)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  kodi::Log(ADDON_LOG_DEBUG, "adding timer");
  if (m_filmonApi.AddTimer(timer.GetClientChannelUid(), timer.GetStartTime(), timer.GetEndTime()))
  {
    kodi::addon::CInstancePVRClient::TriggerTimerUpdate();
    return PVR_ERROR_NO_ERROR;
  }
  else
  {
    return PVR_ERROR_SERVER_ERROR;
  }
}

PVR_ERROR PVRFilmonData::DeleteTimer(const kodi::addon::PVRTimer& timer, bool bForceDelete)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  kodi::Log(ADDON_LOG_DEBUG, "deleting timer %d", timer.GetClientIndex());
  if (m_filmonApi.DeleteTimer(timer.GetClientIndex(), bForceDelete))
  {
    kodi::addon::CInstancePVRClient::TriggerTimerUpdate();
    return PVR_ERROR_NO_ERROR;
  }
  else
  {
    return PVR_ERROR_SERVER_ERROR;
  }
}

PVR_ERROR PVRFilmonData::UpdateTimer(const kodi::addon::PVRTimer& timer)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  kodi::Log(ADDON_LOG_DEBUG, "updating timer");
  if (m_filmonApi.DeleteTimer(timer.GetClientIndex(), true) &&
      m_filmonApi.AddTimer(timer.GetClientChannelUid(), timer.GetStartTime(), timer.GetEndTime()))
  {
    kodi::addon::CInstancePVRClient::TriggerTimerUpdate();
    return PVR_ERROR_NO_ERROR;
  }
  else
  {
    return PVR_ERROR_SERVER_ERROR;
  }
}

ADDONCREATOR(PVRFilmonData)
