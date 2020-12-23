/*
 *  Copyright (C) 2014-2020 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2014 Stephen Denham
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include "FilmonAPI.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <kodi/Filesystem.h>
#include <kodi/General.h>
#include <kodi/addon-instance/PVR.h>
#include <json/json.h>

#define FILMON_URL "http://www.filmon.com/"
#define FILMON_ONE_HOUR_RECORDING_SIZE 508831234
#define REQUEST_RETRY_TIMEOUT 500000 // 0.5s

#define RECORDED_STATUS "Recorded"
#define TIMER_STATUS "Accepted"
#define OFF_AIR "Off Air"

// Attempt at genres
typedef struct
{
  int genreType;
  const char* group;
} genreEntry;

static genreEntry genreTable[] = {
    {EPG_EVENT_CONTENTMASK_NEWSCURRENTAFFAIRS, "NEWS TV"},
    {EPG_EVENT_CONTENTMASK_SPORTS, "SPORTS"},
    {EPG_EVENT_CONTENTMASK_SHOW, "COMEDY"},
    {EPG_EVENT_CONTENTMASK_MOVIEDRAMA, "FEATURE MOVIE"},
    {EPG_EVENT_CONTENTMASK_MUSICBALLETDANCE, "MUSIC"},
    {EPG_EVENT_CONTENTMASK_EDUCATIONALSCIENCE, "DOCUMENTARY"},
    {EPG_EVENT_CONTENTMASK_LEISUREHOBBIES, "LIFESTYLE"},
    {EPG_EVENT_CONTENTMASK_MOVIEDRAMA, "SHORT FILMS"},
    {EPG_EVENT_CONTENTMASK_CHILDRENYOUTH, "KIDS"},
    {EPG_EVENT_CONTENTMASK_EDUCATIONALSCIENCE, "SCIENCE & TECHNOLOGY VOD"},
    {EPG_EVENT_CONTENTMASK_EDUCATIONALSCIENCE, "EDUCATION"},
    {EPG_EVENT_CONTENTMASK_NEWSCURRENTAFFAIRS, "BUSINESS TV"},
    {EPG_EVENT_CONTENTMASK_LEISUREHOBBIES, "CARS & AUTO"},
    {EPG_EVENT_CONTENTMASK_LEISUREHOBBIES, "FOOD AND WINE"},
    {EPG_EVENT_CONTENTMASK_SHOW, "CLASSIC TV"},
    {EPG_EVENT_CONTENTMASK_SHOW, "HORROR"}};

#define GENRE_TABLE_LEN sizeof(genreTable) / sizeof(genreTable[0])

std::string PVRFilmonAPI::TimeToHourMin(unsigned int t)
{
  time_t tt = (time_t)t;
  tm* gmtm = gmtime(&tt);
  return std::to_string(gmtm->tm_hour) + std::to_string(gmtm->tm_min);
}

// Timer settings not supported in Filmon
void PVRFilmonAPI::SetTimerDefaults(FilmonTimer* t)
{
  t->bIsRepeating = false;
  t->firstDay = 0;
  t->iWeekdays = 0;
  t->iEpgUid = PVR_TIMER_NO_EPG_UID;
  t->iGenreType = 0;
  t->iGenreSubType = 0;
  t->iMarginStart = 0;
  t->iMarginEnd = 0;
}

// Free response
void PVRFilmonAPI::ClearResponse()
{
  m_response.clear();
}

// Initialize connection
bool PVRFilmonAPI::Create(void)
{
  m_connected = true;
  return m_connected;
}

// Remove connection
void PVRFilmonAPI::Delete(void)
{
  m_connected = false;
}

// Connection URL
std::string PVRFilmonAPI::GetConnectionString()
{
  if (m_connected)
  {
    return std::string(FILMON_URL);
  }
  else
  {
    return std::string(OFF_AIR);
  }
}

// Make a request
bool PVRFilmonAPI::DoRequest(std::string path, std::string params, unsigned int retries)
{
  std::string request = FILMON_URL;

  // Add params
  request.append(path);
  if (params.length() > 0)
  {
    request.append("?");
    request.append(params);
  }

  // Allow request retries
  do
  {
    kodi::Log(ADDON_LOG_DEBUG, "request is %s", request.c_str());
    kodi::vfs::CFile fileHandle;
    if (!fileHandle.OpenFile(request, ADDON_READ_NO_CACHE))
    {
      kodi::Log(ADDON_LOG_ERROR, "request failure");
      m_client.ConnectionStateChange(request, PVR_CONNECTION_STATE_SERVER_UNREACHABLE, "");
      ClearResponse();
      std::this_thread::sleep_for(std::chrono::microseconds(REQUEST_RETRY_TIMEOUT));
    }
    else
    {
      m_client.ConnectionStateChange(request, PVR_CONNECTION_STATE_CONNECTED, "");
      char buffer[4096];
      while (int read = fileHandle.Read(buffer, 4096))
        m_response.append(buffer, read);

      kodi::Log(ADDON_LOG_DEBUG, "response is: %s", m_response.c_str());
    }
  } while (m_response.empty() && --retries > 0);

  if (!m_response.empty())
  {
    return true;
  }
  else
  {
    Delete();
    Create();
    return false;
  }
}

// Logout user
void PVRFilmonAPI::Logout(void)
{
  bool res = DoRequest("tv/api/logout");
  if (res == true)
  {
    ClearResponse();
  }
}

// Keepalive
bool PVRFilmonAPI::KeepAlive(void)
{
  bool res = DoRequest("tv/api/keep-alive", m_sessionKeyParam);
  if (!res)
  {
    // Login again if it failed
    Logout();
    Login(m_filmonUsername, m_filmonPassword, m_favouriteChannelsOnly);
  }
  else
  {
    ClearResponse();
  }
  return res;
}

// Session
bool PVRFilmonAPI::GetSessionKey(void)
{
  bool res =
      DoRequest("tv/api/"
                    "init?channelProvider=ipad&app_id="
                    "IGlsbSBuVCJ7UDwZBl0eBR4JGgEBERhRXlBcWl0CEw==|User-Agent=Mozilla%2F5.0%"
                    "20(Windows%3B%20U%3B%20Windows%20NT%205.1%3B%20en-GB%3B%20rv%3A1.9.0.3)%"
                    "20Gecko%2F2008092417%20Firefox%2F3.0.3");
  if (res == true)
  {
    Json::Value root;
    std::string jsonReaderError;
    Json::CharReaderBuilder jsonReaderBuilder;
    std::unique_ptr<Json::CharReader> const reader(jsonReaderBuilder.newCharReader());
    reader->parse(m_response.c_str(), m_response.c_str() + m_response.size(), &root, &jsonReaderError);
    Json::Value sessionKey = root["session_key"];
    m_sessionKeyParam = "session_key=";
    m_sessionKeyParam.append(sessionKey.asString());
    kodi::Log(ADDON_LOG_DEBUG, "got session key %s", sessionKey.asString().c_str());
    ClearResponse();
  }
  return res;
}

// Login subscriber
bool PVRFilmonAPI::Login(std::string username, std::string password, bool favouriteChannelsOnly)
{
  bool res = GetSessionKey();
  if (res)
  {
    kodi::Log(ADDON_LOG_DEBUG, "logging in user");
    m_filmonUsername = username;
    m_filmonPassword = password;
    m_favouriteChannelsOnly = favouriteChannelsOnly;

    std::string md5pwd = kodi::GetMD5(password);
    std::transform(md5pwd.begin(), md5pwd.end(), md5pwd.begin(), ::tolower);

    std::string params = "login=" + username + "&password=" + md5pwd;
    res = DoRequest("tv/api/login", m_sessionKeyParam + "&" + params, 1);
    if (res)
    {
      Json::Value root;
      std::string jsonReaderError;
      Json::CharReaderBuilder jsonReaderBuilder;
      std::unique_ptr<Json::CharReader> const reader(jsonReaderBuilder.newCharReader());
      reader->parse(m_response.c_str(), m_response.c_str() + m_response.size(), &root, &jsonReaderError);

      m_channelList.clear();

      if (m_favouriteChannelsOnly)
      {
        Json::Value favouriteChannels = root["favorite-channels"];
        unsigned int channelCount = favouriteChannels.size();
        for (unsigned int channel = 0; channel < channelCount; channel++)
        {
          Json::Value chId = favouriteChannels[channel]["channel"]["id"];
          m_channelList.push_back(chId.asUInt());
          kodi::Log(ADDON_LOG_INFO, "Adding favourite channel to list, id: %u", chId.asUInt());
        }
      }
      else // We want all the channels to be valid
      {
        // Need to clear the long response before we call the channle list API
        ClearResponse();

        res = DoRequest("tv/api/channels", m_sessionKeyParam);
        if (res)
        {
          Json::Value root;
          std::string jsonReaderError;
          Json::CharReaderBuilder jsonReaderBuilder;
          std::unique_ptr<Json::CharReader> const reader(jsonReaderBuilder.newCharReader());
          reader->parse(m_response.c_str(), m_response.c_str() + m_response.size(), &root, &jsonReaderError);
          for (unsigned int i = 0; i < root.size(); i++)
          {
            Json::Value channelId = root[i]["id"];
            Json::Value channelTitle = root[i]["title"];
            Json::Value channelGroup = root[i]["group"];

            unsigned int id = std::atoi(channelId.asString().c_str());
            m_channelList.emplace_back(id);
            kodi::Log(ADDON_LOG_DEBUG, "Adding channel to all channel list: id: %u, name: %s: group: %s", id, channelTitle.asString().c_str(), channelGroup.asString().c_str());
          }
        }
      }

      ClearResponse();
    }
  }
  return res;
}

// SWF player URL
void PVRFilmonAPI::GetSwfPlayer()
{
  m_swfPlayer = std::string("/tv/modules/FilmOnTV/files/flashapp/filmon/FilmonPlayer.swf?v=56");
  bool res = DoRequest("tv/", "");
  if (res == true)
  {
    char* resp = (char*)malloc(m_response.length());
    strcpy(resp, m_response.c_str());
    char* token = strtok(resp, " ");
    while (token != nullptr)
    {
      if (strcmp(token, "flash_config") == 0)
      {
        token = strtok(nullptr, " ");
        token = strtok(nullptr, " ");
        break;
      }
      token = strtok(nullptr, " ");
    }
    Json::Value root;
    std::string jsonReaderError;
    Json::CharReaderBuilder jsonReaderBuilder;
    std::unique_ptr<Json::CharReader> const reader(jsonReaderBuilder.newCharReader());
    if (reader->parse(std::string(token).c_str(),
                      std::string(token).c_str() + std::string(token).size(), &root,
                      &jsonReaderError))
    {
      Json::Value streamer = root["streamer"];
      m_swfPlayer = streamer.asString();
      kodi::Log(ADDON_LOG_DEBUG, "parsed flash config %s", m_swfPlayer.c_str());
    }
    ClearResponse();
  }
  m_swfPlayer = std::string("http://www.filmon.com") + m_swfPlayer;
  kodi::Log(ADDON_LOG_INFO, "swfPlayer is %s", m_swfPlayer.c_str());
}

int PVRFilmonAPI::GetGenre(std::string group)
{
  for (unsigned int i = 0; i < GENRE_TABLE_LEN; i++)
  {
    if (group.compare(std::string(genreTable[i].group)) == 0)
    {
      return genreTable[i].genreType;
    }
  }
  return EPG_EVENT_CONTENTMASK_UNDEFINED;
}

// Channel stream (RTMP)
std::string PVRFilmonAPI::GetRtmpStream(std::string url, std::string name)
{
  char urlDelimiter = '/';
  std::vector<std::string> streamElements;
  if (m_swfPlayer.empty())
  {
    GetSwfPlayer();
  }
  for (size_t p = 0, q = 0; p != url.npos; p = q)
  {
    std::string element =
        url.substr(p + (p != 0), (q = url.find(urlDelimiter, p + 1)) - p - (p != 0));
    streamElements.push_back(element);
  }
  if (streamElements.size() > 4)
  {
    std::string app = streamElements[3] + '/' + streamElements[4];
    std::string streamUrl = url + " playpath=" + name + " app=" + app + " swfUrl=" + m_swfPlayer +
                            " pageurl=http://www.filmon.com/" + " live=1 timeout=10 swfVfy=1";
    return streamUrl;
  }
  else
  {
    kodi::Log(ADDON_LOG_ERROR, "no stream available");
    return std::string("");
  }
}

// Channel
bool PVRFilmonAPI::GetChannel(unsigned int channelId, FilmonChannel* channel, bool preferHd)
{
  bool res = DoRequest("tv/api/channel/" + std::to_string(channelId), m_sessionKeyParam);
  if (res == true)
  {
    Json::Value root;
    std::string jsonReaderError;
    Json::CharReaderBuilder jsonReaderBuilder;
    std::unique_ptr<Json::CharReader> const reader(jsonReaderBuilder.newCharReader());
    reader->parse(m_response.c_str(), m_response.c_str() + m_response.size(), &root, &jsonReaderError);
    Json::Value title = root["title"];
    Json::Value group = root["group"];
    Json::Value icon = root["extra_big_logo"];
    Json::Value streams = root["streams"];
    Json::Value tvguide = root["tvguide"];
    std::string streamURL;

    unsigned int streamCount = streams.size();
    unsigned int stream = 0;
    std::string quality;
    kodi::Log(ADDON_LOG_DEBUG, "---------- Channel: %s ----------", title.asString().c_str());
    // Debug log all available qualities
    for (int i = 0; i < streamCount; i++)
    {
      kodi::Log(ADDON_LOG_DEBUG, "Channel: %s, Stream: %d, Quality: %s", title.asString().c_str(), i, streams[i]["quality"].asString().c_str());
    }
    // Now select stream
    for (stream = 0; stream < streamCount; stream++)
    {
      quality = streams[stream]["quality"].asString();
      if (quality == "high" || quality == "480p" || quality == "HD")
      {
        if (preferHd)
          break;
      }
      else
      {
        if (!preferHd)
          break;
      }
    }
    kodi::Log(ADDON_LOG_DEBUG, "Selected stream: %d, quality: '%s'", stream, quality.c_str());

    std::string chTitle = title.asString();
    std::string iconPath = icon.asString();
    streamURL = streams[stream]["url"].asString();
    if (streamURL.find("rtmp://") == 0)
    {
      streamURL = GetRtmpStream(streamURL, streams[stream]["name"].asString());
      kodi::Log(ADDON_LOG_DEBUG, "RTMP stream available: %s", streamURL.c_str());
    }
    else
    {
      kodi::Log(ADDON_LOG_DEBUG, "HLS stream available: %s", streamURL.c_str());
    }

    // Fix channel names logos
    if (chTitle.compare(std::string("CBEEBIES/BBC Four")) == 0)
    {
      chTitle = std::string("BBC Four/CBEEBIES");
      iconPath = std::string("https://dl.dropboxusercontent.com/u/3129606/tvicons/BBC%20FOUR.png");
    }
    if (chTitle.compare(std::string("CBBC/BBC Three")) == 0)
    {
      chTitle = std::string("BBC Three/CBBC");
      iconPath = std::string("https://dl.dropboxusercontent.com/u/3129606/"
                             "tvicons/BBC%20THREE.png");
    }
    kodi::Log(ADDON_LOG_DEBUG, "title is %s", chTitle.c_str());

    channel->bRadio = false;
    channel->iUniqueId = channelId;
    channel->iChannelNumber = channelId;
    channel->iEncryptionSystem = 0;
    channel->strChannelName = chTitle;
    channel->strIconPath = iconPath;
    channel->strStreamURL = streamURL;
    (channel->epg).clear();
    ClearResponse();

    bool res = DoRequest("tv/api/tvguide/" + std::to_string(channelId));
    if (res == true)
    {
      // Get EPG
      kodi::Log(ADDON_LOG_DEBUG, "building EPG");
      jsonReaderError = "";
      reader->parse(m_response.c_str(), m_response.c_str() + m_response.size(), &root, &jsonReaderError);
      unsigned int entries = 0;
      unsigned int programmeCount = root.size();
      std::string offAir = std::string("OFF_AIR");
      for (unsigned int p = 0; p < programmeCount; p++)
      {
        Json::Value broadcastId = root[p]["programme"];
        std::string programmeId = broadcastId.asString();
        Json::Value startTime = root[p]["startdatetime"];
        Json::Value endTime = root[p]["enddatetime"];
        Json::Value programmeName = root[p]["programme_name"];
        Json::Value plot = root[p]["programme_description"];
        Json::Value images = root[p]["images"];
        FilmonEpgEntry epgEntry;
        if (programmeId.compare(offAir) != 0)
        {
          epgEntry.strTitle = programmeName.asString();
          epgEntry.iBroadcastId = std::atoi(programmeId.c_str());
          if (plot.isNull() != true)
          {
            epgEntry.strPlot = plot.asString();
          }
          if (!images.empty())
          {
            Json::Value programmeIcon = images[(unsigned int)0]["url"];
            epgEntry.strIconPath = programmeIcon.asString();
          }
          else
          {
            epgEntry.strIconPath = "";
          }
        }
        else
        {
          epgEntry.strTitle = offAir;
          epgEntry.iBroadcastId = 0;
          epgEntry.strPlot = "";
          epgEntry.strIconPath = "";
        }
        epgEntry.iChannelId = channelId;
        if (startTime.isString())
        {
          epgEntry.startTime = std::atoi(startTime.asString().c_str());
          epgEntry.endTime = std::atoi(endTime.asString().c_str());
        }
        else
        {
          epgEntry.startTime = startTime.asUInt();
          epgEntry.endTime = endTime.asUInt();
        }
        epgEntry.strPlotOutline = "";
        epgEntry.iGenreType = GetGenre(group.asString());
        epgEntry.iGenreSubType = 0;
        (channel->epg).push_back(epgEntry);
        entries++;
      }
      kodi::Log(ADDON_LOG_DEBUG, "number of EPG entries is %u", entries);
      ClearResponse();
    }
  }
  return res;
}

// Channel groups
std::vector<FilmonChannelGroup> PVRFilmonAPI::GetChannelGroups()
{
  bool res = DoRequest("tv/api/groups", m_sessionKeyParam);
  if (res == true)
  {
    m_groups.clear();

    Json::Value root;
    std::string jsonReaderError;
    Json::CharReaderBuilder jsonReaderBuilder;
    std::unique_ptr<Json::CharReader> const reader(jsonReaderBuilder.newCharReader());
    reader->parse(m_response.c_str(), m_response.c_str() + m_response.size(), &root, &jsonReaderError);
    for (unsigned int i = 0; i < root.size(); i++)
    {
      Json::Value groupName = root[i]["group"];
      Json::Value groupId = root[i]["group_id"];
      Json::Value channels = root[i]["channels"];
      FilmonChannelGroup group;
      group.bRadio = false;
      group.iGroupId = std::atoi(groupId.asString().c_str());
      group.strGroupName = groupName.asString();
      std::vector<unsigned int> members;
      unsigned int membersCount = channels.size();
      for (unsigned int j = 0; j < membersCount; j++)
      {
        Json::Value member = channels[j];
        unsigned int ch = std::atoi(member.asString().c_str());
        if (std::find(m_channelList.begin(), m_channelList.end(), ch) != m_channelList.end())
        {
          members.push_back(ch);
          kodi::Log(ADDON_LOG_INFO, "added channel %u to group %s", ch, group.strGroupName.c_str());
        }
      }
      if (members.size() > 0)
      {
        group.members = members;
        m_groups.push_back(group);
        kodi::Log(ADDON_LOG_INFO, "added group %s", group.strGroupName.c_str());
      }
    }
    ClearResponse();
  }
  return m_groups;
}

// The list of subscriber channels
std::vector<unsigned int> PVRFilmonAPI::GetChannels(void)
{
  return m_channelList;
}

// The count of subscriber channels
unsigned int PVRFilmonAPI::GetChannelCount(void)
{
  if (m_channelList.empty())
  {
    return 0;
  }
  else
    return m_channelList.size();
}

// Gets all timers and recordings
bool PVRFilmonAPI::GetRecordingsTimers(bool completed)
{
  bool res = DoRequest("tv/api/dvr/list", m_sessionKeyParam);
  if (res == true)
  {
    Json::Value root;
    std::string jsonReaderError;
    Json::CharReaderBuilder jsonReaderBuilder;
    std::unique_ptr<Json::CharReader> const reader(jsonReaderBuilder.newCharReader());
    reader->parse(m_response.c_str(), m_response.c_str() + m_response.size(), &root, &jsonReaderError);

    // Usage
    Json::Value total = root["userStorage"]["total"];
    Json::Value used = root["userStorage"]["recorded"];
    m_storageTotal = (long long int)(total.asDouble() * FILMON_ONE_HOUR_RECORDING_SIZE); // bytes
    m_storageUsed = (long long int)(used.asDouble() * FILMON_ONE_HOUR_RECORDING_SIZE); // bytes
    kodi::Log(ADDON_LOG_DEBUG, "recordings total is %u", m_storageTotal);
    kodi::Log(ADDON_LOG_DEBUG, "recordings used is %u", m_storageUsed);

    bool timersCleared = false;
    bool recordingsCleared = false;

    Json::Value recordingsTimers = root["recordings"];
    for (unsigned int recordingId = 0; recordingId < recordingsTimers.size(); recordingId++)
    {
      std::string recTimId = recordingsTimers[recordingId]["id"].asString();
      std::string recTimTitle = recordingsTimers[recordingId]["title"].asString();
      unsigned int recTimStart =
          std::atoi(recordingsTimers[recordingId]["time_start"].asString().c_str());
      unsigned int recDuration = std::atoi(recordingsTimers[recordingId]["length"].asString().c_str());

      Json::Value status = recordingsTimers[recordingId]["status"];
      if (completed && status.asString().compare(std::string(RECORDED_STATUS)) == 0)
      {
        if (recordingsCleared == false)
        {
          m_recordings.clear();
          recordingsCleared = true;
        }
        FilmonRecording recording;
        recording.strRecordingId = recTimId;
        recording.strTitle = recTimTitle;
        recording.strStreamURL = recordingsTimers[recordingId]["download_link"].asString();
        recording.strPlot = recordingsTimers[recordingId]["description"].asString();
        recording.recordingTime = recTimStart;
        recording.iDuration = recDuration;
        recording.strIconPath = recordingsTimers[recordingId]["images"]["channel_logo"].asString();
        recording.strThumbnailPath = recordingsTimers[recordingId]["images"]["poster"].asString();

        m_recordings.push_back(recording);
        kodi::Log(ADDON_LOG_DEBUG, "found completed recording %s", recording.strTitle.c_str());
      }
      else if (status.asString().compare(std::string(TIMER_STATUS)) == 0)
      {
        if (timersCleared == false)
        {
          m_timers.clear();
          timersCleared = true;
        }

        FilmonTimer timer;
        timer.iClientIndex = std::atoi(recTimId.c_str());
        timer.iClientChannelUid =
            std::atoi(recordingsTimers[recordingId]["channel_id"].asString().c_str());
        timer.startTime = recTimStart;
        timer.endTime = timer.startTime + recDuration;
        timer.strTitle = recTimTitle;
        timer.state = FilmonTimerState::NEW;
        timer.strSummary = recordingsTimers[recordingId]["description"].asString();
        SetTimerDefaults(&timer);
        time_t t = time(nullptr);
        if (t >= timer.startTime && t <= timer.endTime)
        {
          kodi::Log(ADDON_LOG_DEBUG, "found active timer %s", timer.strTitle.c_str());
          timer.state = FilmonTimerState::RECORDING;
        }
        else if (t < timer.startTime)
        {
          kodi::Log(ADDON_LOG_DEBUG, "found scheduled timer %s", timer.strTitle.c_str());
          timer.state = FilmonTimerState::SCHEDULED;
        }
        else if (t > timer.endTime)
        {
          kodi::Log(ADDON_LOG_DEBUG, "found completed timer %s", timer.strTitle.c_str());
          timer.state = FilmonTimerState::COMPLETED;
        }
        m_timers.push_back(timer);
      }
    }
    ClearResponse();
  }
  return res;
}

// Wrapper to get recordings
std::vector<FilmonRecording> PVRFilmonAPI::GetRecordings(void)
{
  bool completed = true;
  if (GetRecordingsTimers(completed) != true)
  {
    kodi::Log(ADDON_LOG_ERROR, "failed to get recordings");
  }
  return m_recordings;
}

// Delete a recording
bool PVRFilmonAPI::DeleteRecording(unsigned int recordingId)
{
  bool res = false;
  kodi::Log(ADDON_LOG_DEBUG, "number recordings is %u", m_recordings.size());
  for (unsigned int i = 0; i < m_recordings.size(); i++)
  {
    kodi::Log(ADDON_LOG_DEBUG, "looking for recording %u", recordingId);
    if ((m_recordings[i].strRecordingId).compare(std::to_string(recordingId)) == 0)
    {
      std::string params = "record_id=" + m_recordings[i].strRecordingId;
      res = DoRequest("tv/api/dvr/remove", m_sessionKeyParam + "&" + params);
      if (res)
      {
        Json::Value root;
        std::string jsonReaderError;
        Json::CharReaderBuilder jsonReaderBuilder;
        std::unique_ptr<Json::CharReader> const reader(jsonReaderBuilder.newCharReader());
        reader->parse(m_response.c_str(), m_response.c_str() + m_response.size(), &root,
                      &jsonReaderError);
        if (root["success"].asBool())
        {
          m_recordings.erase(m_recordings.begin() + i);
          kodi::Log(ADDON_LOG_DEBUG, "deleted recording");
        }
        else
        {
          res = false;
        }
        ClearResponse();
      }
      break;
    }
    kodi::Log(ADDON_LOG_DEBUG, "found recording %u", m_recordings[i].strRecordingId.c_str());
  }
  return res;
}

// Get timers
std::vector<FilmonTimer> PVRFilmonAPI::GetTimers(void)
{
  if (GetRecordingsTimers() != true)
  {
    kodi::Log(ADDON_LOG_ERROR, "failed to get timers");
  }
  return m_timers;
}

// Add a timer
bool PVRFilmonAPI::AddTimer(int channelId, time_t startTime, time_t endTime)
{
  bool res = DoRequest("tv/api/tvguide/" + std::to_string(channelId), m_sessionKeyParam);
  if (res)
  {
    Json::Value root;
    std::string jsonReaderError;
    Json::CharReaderBuilder jsonReaderBuilder;
    std::unique_ptr<Json::CharReader> const reader(jsonReaderBuilder.newCharReader());
    reader->parse(m_response.c_str(), m_response.c_str() + m_response.size(), &root, &jsonReaderError);
    for (unsigned int i = 0; i < root.size(); i++)
    {
      Json::Value start = root[i]["startdatetime"];
      Json::Value end = root[i]["enddatetime"];
      time_t epgStartTime = 0;
      time_t epgEndTime = 0;
      if (start.isString())
      {
        epgStartTime = std::atoi(start.asString().c_str());
        epgEndTime = std::atoi(end.asString().c_str());
      }
      else
      {
        epgStartTime = start.asUInt();
        epgEndTime = end.asUInt();
      }
      if (epgStartTime == startTime || epgEndTime == endTime)
      {
        Json::Value broadcastId = root[i]["programme"];
        std::string programmeId = broadcastId.asString();
        Json::Value progName = root[i]["programme_name"];
        Json::Value progDesc = root[i]["programme_description"];
        std::string programmeName = progName.asString();
        std::string programmeDesc = progDesc.asString();

        std::string params = "channel_id=" + std::to_string(channelId) +
                             "&programme_id=" + programmeId +
                             "&start_time=" + std::to_string(epgStartTime);
        res = DoRequest("tv/api/dvr/add", m_sessionKeyParam + "&" + params);
        if (res)
        {
          Json::Value root;
          jsonReaderError = "";
          reader->parse(m_response.c_str(), m_response.c_str() + m_response.size(), &root,
                        &jsonReaderError);
          if (root["success"].asBool())
          {
            FilmonTimer timer;
            timer.iClientIndex = std::atoi(programmeId.c_str());
            timer.iClientChannelUid = channelId;
            timer.startTime = epgStartTime;
            timer.endTime = epgEndTime;
            timer.strTitle = programmeName;
            timer.strSummary = programmeDesc;
            time_t t = time(nullptr);
            if (t >= epgStartTime && t <= epgEndTime)
            {
              timer.state = FilmonTimerState::RECORDING;
            }
            else
            {
              timer.state = FilmonTimerState::SCHEDULED;
            }
            SetTimerDefaults(&timer);
            m_timers.push_back(timer);
            kodi::Log(ADDON_LOG_DEBUG, "addded timer");
          }
          else
          {
            res = false;
          }
        }
        break;
      }
    }
    ClearResponse();
  }
  return res;
}

// Delete a timer
bool PVRFilmonAPI::DeleteTimer(unsigned int timerId, bool bForceDelete)
{
  bool res = true;
  for (unsigned int i = 0; i < m_timers.size(); i++)
  {
    kodi::Log(ADDON_LOG_DEBUG, "looking for timer %u", timerId);
    if (m_timers[i].iClientIndex == timerId)
    {
      time_t t = time(nullptr);
      if ((t >= m_timers[i].startTime && t <= m_timers[i].endTime && bForceDelete) ||
          t < m_timers[i].startTime || t > m_timers[i].endTime)
      {
        std::string params = "record_id=" + std::to_string(timerId);
        res = DoRequest("tv/api/dvr/remove", m_sessionKeyParam + "&" + params);
        if (res)
        {
          Json::Value root;
          std::string jsonReaderError;
          Json::CharReaderBuilder jsonReaderBuilder;
          std::unique_ptr<Json::CharReader> const reader(jsonReaderBuilder.newCharReader());
          reader->parse(m_response.c_str(), m_response.c_str() + m_response.size(), &root,
                        &jsonReaderError);
          if (root["success"].asBool())
          {
            m_timers.erase(m_timers.begin() + i);
            kodi::Log(ADDON_LOG_DEBUG, "deleted timer");
          }
          else
          {
            res = false;
          }
          ClearResponse();
        }
      }
      break;
    }
    kodi::Log(ADDON_LOG_DEBUG, "found timer %u", timerId);
  }
  return res;
}

// Recording usage in bytes
void PVRFilmonAPI::GetUserStorage(uint64_t& iTotal, uint64_t& iUsed)
{
  iTotal = m_storageTotal;
  iUsed = m_storageUsed;
}

// int main(int argc, char *argv[]) {
//  Create();
//  std::string username = argv[1];
//  std::string password = argv[2];
//  Login(username, password);
//  std::vector<FilmonChannelGroup> grps = GetChannelGroups();
//  for (int i = 0; i < grps.size(); i++) {
//    std::cout << grps[i].strGroupName << std::endl;
//    for (int j = 0; j < grps[i].members.size();j++) {
//      std::cout << grps[i].members[j] << std::endl;
//    }
//  }
//  Delete();
//}
