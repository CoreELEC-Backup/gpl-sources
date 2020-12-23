/*
 *  Copyright (C) 2011-2020 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2011 Pulse-Eight (http://www.pulse-eight.com/)
 *  Copyright (C) 2014 Stephen Denham
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "FilmonAPI.h"

#include <mutex>

#include <kodi/addon-instance/PVR.h>

#define FILMON_CACHE_TIME 10800 // 3 hours

typedef FilmonEpgEntry PVRFilmonEpgEntry;
typedef FilmonChannel PVRFilmonChannel;
typedef FilmonRecording PVRFilmonRecording;
typedef FilmonTimer PVRFilmonTimer;
typedef FilmonChannelGroup PVRFilmonChannelGroup;

class ATTRIBUTE_HIDDEN PVRFilmonData : public kodi::addon::CAddonBase,
                                       public kodi::addon::CInstancePVRClient
{
public:
  PVRFilmonData(void);
  virtual ~PVRFilmonData(void);

  ADDON_STATUS Create() override;
  ADDON_STATUS SetSetting(const std::string& settingName,
                          const kodi::CSettingValue& settingValue) override;

  PVR_ERROR GetCapabilities(kodi::addon::PVRCapabilities& capabilities) override;
  PVR_ERROR GetBackendName(std::string& name) override;
  PVR_ERROR GetBackendVersion(std::string& version) override;
  PVR_ERROR GetConnectionString(std::string& connection) override;
  PVR_ERROR GetDriveSpace(uint64_t& total, uint64_t& used) override;

  PVR_ERROR GetChannelsAmount(int& amount) override;
  PVR_ERROR GetChannels(bool radio, kodi::addon::PVRChannelsResultSet& results) override;
  PVR_ERROR GetSignalStatus(int channelUid, kodi::addon::PVRSignalStatus& signalStatus) override;

  PVR_ERROR GetChannelGroupsAmount(int& amount) override;
  PVR_ERROR GetChannelGroups(bool radio, kodi::addon::PVRChannelGroupsResultSet& results) override;
  PVR_ERROR GetChannelGroupMembers(const kodi::addon::PVRChannelGroup& group,
                                   kodi::addon::PVRChannelGroupMembersResultSet& results) override;

  PVR_ERROR GetEPGForChannel(int channelUid, time_t start, time_t end,
                             kodi::addon::PVREPGTagsResultSet& results) override;

  PVR_ERROR GetRecordingsAmount(bool deleted, int& amount) override;
  PVR_ERROR GetRecordings(bool deleted, kodi::addon::PVRRecordingsResultSet& results) override;
  PVR_ERROR DeleteRecording(const kodi::addon::PVRRecording& recording) override;

  PVR_ERROR GetTimerTypes(std::vector<kodi::addon::PVRTimerType>& types) override;
  PVR_ERROR GetTimersAmount(int& amount) override;
  PVR_ERROR GetTimers(kodi::addon::PVRTimersResultSet& results) override;
  PVR_ERROR AddTimer(const kodi::addon::PVRTimer& timer) override;
  PVR_ERROR DeleteTimer(const kodi::addon::PVRTimer& timer, bool bForceDelete) override;
  PVR_ERROR UpdateTimer(const kodi::addon::PVRTimer& timer) override;

  PVR_ERROR GetChannelStreamProperties(const kodi::addon::PVRChannel& channel,
                                       std::vector<kodi::addon::PVRStreamProperty>& properties) override;
  PVR_ERROR GetRecordingStreamProperties(const kodi::addon::PVRRecording& recording,
                                         std::vector<kodi::addon::PVRStreamProperty>& properties) override;

private:
  bool Load();
  int UpdateChannel(unsigned int channelId);
  void ReadSettings();

  std::mutex m_mutex;
  std::vector<PVRFilmonChannelGroup> m_groups;
  std::vector<PVRFilmonChannel> m_channels;
  std::vector<PVRFilmonRecording> m_recordings;
  std::vector<PVRFilmonTimer> m_timers;
  time_t m_lastTimeGroups;
  time_t m_lastTimeChannels;
  bool m_onLoad = true;

  PVRFilmonAPI m_filmonApi;
  std::string m_username;
  std::string m_password;
  bool m_preferHd = false;
  bool m_favouriteChannelsOnly = false;
};
