/*
 *  Copyright (C) 2014-2020 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2014 Stephen Denham
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <kodi/addon-instance/PVR.h>
#include <string>
#include <vector>

#define REQUEST_RETRIES 4

enum class FilmonTimerState
{
  NEW = 0,
  SCHEDULED = 1,
  RECORDING = 2,
  COMPLETED = 3,
};

struct FilmonRecording
{
  int iDuration;
  int iGenreType;
  int iGenreSubType;
  std::string strChannelName;
  std::string strPlotOutline;
  std::string strPlot;
  std::string strRecordingId;
  std::string strStreamURL;
  std::string strTitle;
  std::string strIconPath;
  std::string strThumbnailPath;
  time_t recordingTime;
};

struct FilmonTimer
{
  unsigned int iClientIndex;
  int iClientChannelUid;
  time_t startTime;
  time_t endTime;
  FilmonTimerState state;
  std::string strTitle;
  std::string strSummary;
  bool bIsRepeating;
  time_t firstDay;
  int iWeekdays;
  unsigned int iEpgUid;
  int iGenreType;
  int iGenreSubType;
  int iMarginStart;
  int iMarginEnd;
};

struct FilmonChannelGroup
{
  bool bRadio;
  int iGroupId;
  std::string strGroupName;
  std::vector<unsigned int> members;
};

struct FilmonEpgEntry
{
  unsigned int iBroadcastId;
  std::string strTitle;
  unsigned int iChannelId;
  time_t startTime;
  time_t endTime;
  std::string strPlotOutline;
  std::string strPlot;
  std::string strIconPath;
  int iGenreType;
  int iGenreSubType;
  time_t firstAired;
  int iParentalRating;
  int iStarRating;
  int iSeriesNumber;
  int iEpisodeNumber;
  int iEpisodePartNumber;
  std::string strEpisodeName;
};

struct FilmonChannel
{
  bool bRadio;
  unsigned int iUniqueId;
  unsigned int iChannelNumber;
  unsigned int iEncryptionSystem;
  std::string strChannelName;
  std::string strIconPath;
  std::string strStreamURL;
  std::vector<FilmonEpgEntry> epg;
};

class ATTRIBUTE_HIDDEN PVRFilmonAPI
{
public:
  PVRFilmonAPI(kodi::addon::CInstancePVRClient& client) : m_client(client) { }

  bool Create();
  void Delete();
  bool KeepAlive();
  bool Login(std::string username, std::string password, bool favouriteChannelsOnly);
  void GetUserStorage(uint64_t& iTotal, uint64_t& iUsed);
  bool DeleteTimer(unsigned int timerId, bool bForceDelete);
  bool AddTimer(int channelId, time_t startTime, time_t endTime);
  bool DeleteRecording(unsigned int recordingId);
  bool GetChannel(unsigned int channelId, FilmonChannel* channel, bool preferHd);
  std::vector<unsigned int> GetChannels();
  unsigned int GetChannelCount();
  std::vector<FilmonChannelGroup> GetChannelGroups();
  std::vector<FilmonRecording> GetRecordings();
  std::vector<FilmonTimer> GetTimers();
  std::string GetConnectionString();

private:
  bool DoRequest(std::string path, std::string params = "", unsigned int retries = REQUEST_RETRIES);
  void Logout();
  bool GetSessionKey();
  void GetSwfPlayer();
  int GetGenre(std::string group);
  std::string GetRtmpStream(std::string url, std::string name);
  bool GetRecordingsTimers(bool completed = false);
  void ClearResponse();
  std::string TimeToHourMin(unsigned int t);
  void SetTimerDefaults(FilmonTimer* t);

  std::string m_filmonUsername = "";
  std::string m_filmonPassword = "";
  bool m_favouriteChannelsOnly = false;
  std::string m_sessionKeyParam = "";
  std::string m_swfPlayer = "";

  long long m_storageUsed = 0;
  long long m_storageTotal = 0;

  std::vector<unsigned int> m_channelList;
  std::vector<FilmonChannelGroup> m_groups;
  std::vector<FilmonRecording> m_recordings;
  std::vector<FilmonTimer> m_timers;

  bool m_connected = false;

  std::string m_response;

  kodi::addon::CInstancePVRClient& m_client;
};
