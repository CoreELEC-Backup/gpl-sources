/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "PctvData.h"

#include "md5.h"

#include <algorithm>
#include <kodi/General.h>
#include <kodi/tools/StringUtils.h>

using kodi::tools::StringUtils;

/************************************************************/
/** Class interface */

Pctv::Pctv(const std::string strHostname,
           int iPortWeb,
           const std::string& strPin,
           int iBitrate,
           bool bTranscode,
           bool bUsePIN,
           KODI_HANDLE instance,
           const std::string& kodiVersion)
  : kodi::addon::CInstancePVRClient(instance, kodiVersion),
    m_strHostname(strHostname),
    m_iPortWeb(iPortWeb),
    m_strPin(strPin),
    m_iBitrate(iBitrate),
    m_bTranscode(bTranscode),
    m_bUsePIN(bUsePIN)
{
  srand(time(nullptr));
  m_iDataIdentifier = rand();
  m_strBackendUrlNoAuth = StringUtils::Format("http://%s:%u", m_strHostname.c_str(), m_iPortWeb);
}

void Pctv::Process()
{
  kodi::Log(ADDON_LOG_DEBUG, "%s - starting", __func__);

  std::lock_guard<std::mutex> lock(m_mutex);
  m_started.notify_all();

  return;
}

Pctv::~Pctv()
{
  std::lock_guard<std::mutex> lock(m_mutex);
  kodi::Log(ADDON_LOG_DEBUG, "%s Stopping update thread...", __func__);
  m_running = false;
  if (m_thread.joinable())
    m_thread.join();
}

bool Pctv::Open()
{
  std::lock_guard<std::mutex> lock(m_mutex);

  kodi::Log(ADDON_LOG_INFO, "%s - PCTV Systems Addon Configuration options", __func__);
  kodi::Log(ADDON_LOG_INFO, "%s - Hostname: '%s'", __func__, m_strHostname.c_str());
  kodi::Log(ADDON_LOG_INFO, "%s - WebPort: '%d'", __func__, m_iPortWeb);

  m_bIsConnected = GetFreeConfig();

  if (!m_bIsConnected)
  {
    kodi::Log(ADDON_LOG_ERROR,
              "%s It seem's that pctv cannot be reached. Make sure that you set the correct "
              "configuration options in the addon settings!",
              __func__);
    return false;
  }

  // add user:pin in front of the URL if PIN is set
  std::string strURL = "";
  std::string strAuth = "";
  if (m_bUsePIN)
  {
    std::string pinMD5 = XBMC_MD5::GetMD5(m_strPin);
    std::transform(pinMD5.begin(), pinMD5.end(), pinMD5.begin(), ::tolower);

    strURL = StringUtils::Format("User:%s@", pinMD5.c_str());

    if (IsSupported("broadway"))
    {
      strAuth = "/basicauth";
    }
  }

  strURL = StringUtils::Format("http://%s%s:%u%s", strURL.c_str(), m_strHostname.c_str(),
                               m_iPortWeb, strAuth.c_str());
  m_strBaseUrl = strURL;

  // request index.html to force wake-up from standby
  if (IsSupported("broadway"))
  {
    int retval;
    cRest rest;
    Json::Value response;

    std::string strUrl = m_strBaseUrl + URI_INDEX_HTML;
    retval = rest.Get(strUrl, "", response);
  }

  if (m_channels.size() == 0)
  {
    // Load the TV channels
    LoadChannels();
  }

  kodi::Log(ADDON_LOG_INFO, "%s Starting separate client update thread...", __func__);
  m_running = true;
  m_thread = std::thread([&] { Process(); });

  return m_running;
}

/************************************************************/
/** Channels  */

PVR_ERROR Pctv::GetChannelsAmount(int& amount)
{
  if (!IsConnected())
    return PVR_ERROR_SERVER_ERROR;
  amount = m_channels.size();
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR Pctv::GetChannels(bool radio, kodi::addon::PVRChannelsResultSet& results)
{
  if (!IsConnected())
    return PVR_ERROR_SERVER_ERROR;

  kodi::Log(ADDON_LOG_DEBUG, "%s", __func__);
  m_iNumChannels = 0;
  m_channels.clear();

  Json::Value data;
  int retval;
  retval = RESTGetChannelList(0, data); // all channels

  if (retval < 0)
  {
    kodi::Log(ADDON_LOG_ERROR, "No channels available.");
    return PVR_ERROR_SERVER_ERROR;
  }

  for (unsigned int index = 0; index < data.size(); ++index)
  {
    PctvChannel channel;
    Json::Value entry;

    entry = data[index];

    channel.iUniqueId = entry["Id"].asInt();
    channel.strChannelName = entry["DisplayName"].asString();
    if (entry["MajorChannelNo"] != Json::nullValue)
    {
      channel.iChannelNumber = entry["MajorChannelNo"].asInt();
    }
    else
    {
      channel.iChannelNumber = entry["Id"].asInt();
    }
    if (entry["MinorChannelNo"] != Json::nullValue)
    {
      channel.iSubChannelNumber = entry["MinorChannelNo"].asInt();
    }
    else
    {
      channel.iSubChannelNumber = 0;
    }
    channel.iEncryptionSystem = 0;
    std::string params;
    params = GetPreviewParams(m_iDataIdentifier, entry);
    channel.strStreamURL = GetPreviewUrl(params);
    channel.strLogoPath = GetChannelLogo(entry);
    m_iNumChannels++;
    m_channels.push_back(channel);

    kodi::Log(ADDON_LOG_DEBUG, "%s loaded Channel entry '%s'", __func__,
              channel.strChannelName.c_str());
  }

  if (m_channels.size() > 0)
  {
    std::sort(m_channels.begin(), m_channels.end());
  }

  kodi::QueueFormattedNotification(QUEUE_INFO, "%d channels loaded.", m_channels.size());

  TransferChannels(results);

  return PVR_ERROR_NO_ERROR;
}

void Pctv::TransferChannels(kodi::addon::PVRChannelsResultSet& results)
{
  for (unsigned int i = 0; i < m_channels.size(); i++)
  {
    std::string strTmp;
    PctvChannel& channel = m_channels.at(i);
    kodi::addon::PVRChannel tag;

    tag.SetUniqueId(channel.iUniqueId);
    tag.SetChannelNumber(channel.iChannelNumber);
    tag.SetSubChannelNumber(channel.iSubChannelNumber);
    tag.SetEncryptionSystem(channel.iEncryptionSystem);
    tag.SetChannelName(channel.strChannelName);
    tag.SetMimeType(m_strPreviewMode);
    tag.SetIconPath(channel.strLogoPath);

    results.Add(tag);
  }
}

bool Pctv::LoadChannels()
{
  kodi::addon::CInstancePVRClient::TriggerChannelGroupsUpdate();
  kodi::addon::CInstancePVRClient::TriggerChannelUpdate();
  return true;
}

PVR_ERROR Pctv::GetChannelStreamProperties(const kodi::addon::PVRChannel& channel,
                                           std::vector<kodi::addon::PVRStreamProperty>& properties)
{
  std::string strUrl;
  for (const auto& pctvChannel : m_channels)
  {
    if (pctvChannel.iUniqueId == channel.GetUniqueId())
    {
      strUrl = pctvChannel.strStreamURL;
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

/************************************************************/
/** Groups  */

PVR_ERROR Pctv::GetChannelGroupsAmount(int& amount)
{
  if (!IsConnected())
    return PVR_ERROR_SERVER_ERROR;
  amount = m_iNumGroups;
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR Pctv::GetChannelGroups(bool radio, kodi::addon::PVRChannelGroupsResultSet& results)

{
  if (!IsConnected())
    return PVR_ERROR_SERVER_ERROR;

  m_iNumGroups = 0;
  m_groups.clear();

  Json::Value data;
  int retval;
  retval = RESTGetChannelLists(data);

  if (retval < 0)
  {
    kodi::Log(ADDON_LOG_ERROR, "No channels available.");
    return PVR_ERROR_SERVER_ERROR;
  }

  for (unsigned int index = 0; index < data.size(); ++index)
  {
    PctvChannelGroup group;
    Json::Value entry;

    entry = data[index];
    int iChannelListId = entry["Id"].asInt();

    Json::Value channellistData;
    retval = RESTGetChannelList(iChannelListId, channellistData);
    if (retval > 0)
    {
      Json::Value channels = channellistData["Channels"];

      for (unsigned int i = 0; i < channels.size(); ++i)
      {

        Json::Value channel;

        channel = channels[i];
        group.members.push_back(channel["Id"].asInt());
      }
    }

    group.iGroupId = iChannelListId;
    group.strGroupName = entry["DisplayName"].asString();
    group.bRadio = false;

    m_groups.push_back(group);
    m_iNumGroups++;

    kodi::Log(ADDON_LOG_DEBUG, "%s loaded channelist entry '%s'", __func__,
              group.strGroupName.c_str());
  }

  kodi::QueueFormattedNotification(QUEUE_INFO, "%d groups loaded.", m_groups.size());

  TransferGroups(results);

  return PVR_ERROR_NO_ERROR;
}

void Pctv::TransferGroups(kodi::addon::PVRChannelGroupsResultSet& results)
{
  for (unsigned int i = 0; i < m_groups.size(); i++)
  {
    std::string strTmp;
    PctvChannelGroup& group = m_groups.at(i);

    kodi::addon::PVRChannelGroup tag;

    tag.SetIsRadio(false);
    tag.SetPosition(0); // groups default order, unused
    tag.SetGroupName(group.strGroupName);

    results.Add(tag);
  }
}

PVR_ERROR Pctv::GetChannelGroupMembers(const kodi::addon::PVRChannelGroup& group,
                                       kodi::addon::PVRChannelGroupMembersResultSet& results)
{
  if (group.GetIsRadio())
    return PVR_ERROR_NO_ERROR;

  if (!IsConnected())
    return PVR_ERROR_SERVER_ERROR;

  std::string strTmp = group.GetGroupName();
  for (unsigned int i = 0; i < m_groups.size(); i++)
  {
    PctvChannelGroup& g = m_groups.at(i);
    if (!strTmp.compare(g.strGroupName))
    {
      for (unsigned int i = 0; i < g.members.size(); i++)
      {
        kodi::addon::PVRChannelGroupMember tag;

        tag.SetChannelUniqueId(g.members[i]);
        tag.SetGroupName(g.strGroupName);

        results.Add(tag);
      }
    }
  }

  return PVR_ERROR_NO_ERROR;
}

int Pctv::RESTGetChannelLists(Json::Value& response)
{
  int retval;
  cRest rest;

  std::string strUrl = m_strBaseUrl + URI_REST_CHANNELLISTS;
  retval = rest.Get(strUrl, "", response);

  if (retval >= 0)
  {
    if (response.type() == Json::arrayValue)
    {
      int size = response.size();
      return size;
    }
    else
    {
      kodi::Log(ADDON_LOG_DEBUG, "Unknown response format. Expected Json::arrayValue\n");
      return -1;
    }
  }
  else
  {
    kodi::Log(ADDON_LOG_DEBUG, "Request Recordings failed. Return value: %i\n", retval);
  }

  return retval;
}

/************************************************************/
/** Recordings  */

PVR_ERROR Pctv::GetRecordingsAmount(bool deleted, int& amount)
{
  if (!IsConnected())
    return PVR_ERROR_SERVER_ERROR;

  amount = m_iNumRecordings;
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR Pctv::GetRecordings(bool deleted, kodi::addon::PVRRecordingsResultSet& results)
{
  if (!IsConnected())
    return PVR_ERROR_SERVER_ERROR;

  m_iNumRecordings = 0;
  m_recordings.clear();

  Json::Value data;
  int retval = RESTGetRecordings(data);
  if (retval > 0)
  {
    for (unsigned int index = 0; index < data["video"].size(); ++index)
    {
      PctvRecording recording;
      //Json::Value entry;

      //entry = data["video"][index];
      Json::Value entry(data["video"][index]);
      recording.strRecordingId = index;
      recording.strTitle = entry["DisplayName"].asString();
      recording.startTime = static_cast<time_t>(entry["RecDate"].asDouble() / 1000); // in seconds
      recording.iDuration = static_cast<time_t>(entry["Duration"].asDouble() / 1000); // in seconds
      recording.iLastPlayedPosition =
          static_cast<int>(entry["Resume"].asDouble() / 1000); // in seconds

      std::string params = GetPreviewParams(m_iDataIdentifier, entry);
      recording.strStreamURL = GetPreviewUrl(params);
      m_iNumRecordings++;
      m_recordings.push_back(recording);

      kodi::Log(ADDON_LOG_DEBUG, "%s loaded Recording entry '%s'", __func__,
                recording.strTitle.c_str());
    }
  }

  kodi::QueueFormattedNotification(QUEUE_INFO, "%d recordings loaded.", m_recordings.size());

  TransferRecordings(results);

  return PVR_ERROR_NO_ERROR;
}

void Pctv::TransferRecordings(kodi::addon::PVRRecordingsResultSet& results)
{
  for (unsigned int i = 0; i < m_recordings.size(); i++)
  {
    PctvRecording& recording = m_recordings.at(i);
    kodi::addon::PVRRecording tag;

    tag.SetRecordingId(recording.strRecordingId);
    tag.SetTitle(recording.strTitle);
    tag.SetPlotOutline(recording.strPlotOutline);
    tag.SetPlot(recording.strPlot);
    tag.SetChannelName(recording.strChannelName);
    tag.SetIconPath(recording.strIconPath);
    recording.strDirectory = "";
    tag.SetDirectory(recording.strDirectory);
    tag.SetRecordingTime(recording.startTime);
    tag.SetDuration(recording.iDuration);

    /* TODO: PVR API 5.0.0: Implement this */
    tag.SetChannelUid(PVR_CHANNEL_INVALID_UID);

    /* TODO: PVR API 5.1.0: Implement this */
    tag.SetChannelType(PVR_RECORDING_CHANNEL_TYPE_UNKNOWN);

    results.Add(tag);
  }
}

int Pctv::RESTGetRecordings(Json::Value& response)
{
  cRest rest;
  std::string strUrl = m_strBaseUrl + URI_REST_RECORDINGS;
  int retval = rest.Get(strUrl, "", response);
  if (retval >= 0)
  {
    if (response.type() == Json::objectValue)
    {
      return response["TotalCount"].asInt();
    }
    else
    {
      kodi::Log(ADDON_LOG_DEBUG, "Unknown response format. Expected Json::objectValue\n");
      return -1;
    }
  }
  else
  {
    kodi::Log(ADDON_LOG_DEBUG, "Request Recordings failed. Return value: %i\n", retval);
  }

  return retval;
}

PVR_ERROR Pctv::GetRecordingStreamProperties(
    const kodi::addon::PVRRecording& recording,
    std::vector<kodi::addon::PVRStreamProperty>& properties)
{
  if (!IsConnected())
    return PVR_ERROR_SERVER_ERROR;

  std::string strRecordingFile;
  for (const auto& PctvRec : m_recordings)
  {
    if (PctvRec.strRecordingId == recording.GetRecordingId())
    {
      strRecordingFile = PctvRec.strStreamURL;
    }
  }

  if (strRecordingFile.empty())
    return PVR_ERROR_SERVER_ERROR;

  properties.emplace_back(PVR_STREAM_PROPERTY_STREAMURL, strRecordingFile);
  return PVR_ERROR_NO_ERROR;
}

/************************************************************/
/** Timer */

PVR_ERROR Pctv::GetTimerTypes(std::vector<kodi::addon::PVRTimerType>& types)
{
  /* TODO: Implement this to get support for the timer features introduced with PVR API 1.9.7 */
  return PVR_ERROR_NOT_IMPLEMENTED;
}

PVR_ERROR Pctv::GetTimersAmount(int& amount)
{
  if (!IsConnected())
    return PVR_ERROR_SERVER_ERROR;

  amount = m_timer.size();
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR Pctv::GetTimers(kodi::addon::PVRTimersResultSet& results)
{
  if (!IsConnected())
    return PVR_ERROR_SERVER_ERROR;

  m_timer.clear();

  Json::Value data;
  int retval = RESTGetTimer(data);
  if (retval < 0)
  {
    kodi::Log(ADDON_LOG_ERROR, "No timer available.");
    return PVR_ERROR_SERVER_ERROR;
  }

  for (unsigned int index = 0; index < data.size(); ++index)
  {
    PctvTimer timer;
    Json::Value entry = data[index];

    timer.iId = entry["Id"].asInt();
    timer.strTitle = entry["DisplayName"].asString();
    timer.iChannelId = entry["ChannelId"].asInt();
    timer.startTime = static_cast<time_t>(entry["RealStartTime"].asDouble() / 1000);
    timer.endTime = static_cast<time_t>(entry["RealEndTime"].asDouble() / 1000);
    timer.iStartOffset = entry["StartOffset"].asInt();
    timer.iEndOffset = entry["EndOffset"].asInt();

    std::string strState = entry["State"].asString();
    if (strState == "Idle" || strState == "Prepared")
    {
      timer.state = PVR_TIMER_STATE_SCHEDULED;
    }
    else if (strState == "Running")
    {
      timer.state = PVR_TIMER_STATE_RECORDING;
    }
    else if (strState == "Done")
    {
      timer.state = PVR_TIMER_STATE_COMPLETED;
    }
    else
    {
      timer.state = PVR_TIMER_STATE_NEW; // default
    }

    m_timer.push_back(timer);

    kodi::Log(ADDON_LOG_DEBUG, "%s loaded Timer entry '%s'", __func__, timer.strTitle.c_str());
  }

  kodi::QueueFormattedNotification(QUEUE_INFO, "%d timer loaded.", m_timer.size());

  TransferTimer(results);

  return PVR_ERROR_NO_ERROR;
}

void Pctv::TransferTimer(kodi::addon::PVRTimersResultSet& results)
{
  for (unsigned int i = 0; i < m_timer.size(); i++)
  {
    std::string strTmp;
    PctvTimer& timer = m_timer.at(i);
    kodi::addon::PVRTimer tag;

    /* TODO: Implement own timer types to get support for the timer features introduced with PVR API 1.9.7 */
    tag.SetTimerType(PVR_TIMER_TYPE_NONE);

    tag.SetClientIndex(timer.iId);
    tag.SetClientChannelUid(timer.iChannelId);
    tag.SetTitle(timer.strTitle);
    tag.SetStartTime(timer.startTime);
    tag.SetEndTime(timer.endTime);
    tag.SetState(timer.state);
    tag.SetPriority(0);
    tag.SetLifetime(0);
    tag.SetEPGUid(0);

    results.Add(tag);
  }
}

int Pctv::RESTGetTimer(Json::Value& response)
{
  cRest rest;
  std::string strUrl = m_strBaseUrl + URI_REST_TIMER;
  int retval = rest.Get(strUrl, "", response);

  if (retval >= 0)
  {
    if (response.type() == Json::arrayValue)
    {
      return response.size();
    }
    else
    {
      kodi::Log(ADDON_LOG_DEBUG, "Unknown response format. Expected Json::arrayValue\n");
      return -1;
    }
  }
  else
  {
    kodi::Log(ADDON_LOG_DEBUG, "Request Timer failed. Return value: %i\n", retval);
  }

  return retval;
}

int Pctv::RESTAddTimer(const kodi::addon::PVRTimer& timer, Json::Value& response)
{
  std::string strQueryString;
  strQueryString = StringUtils::Format(
      "{\"Id\":0,\"ChannelId\":%i,\"State\":\"%s\",\"RealStartTime\":%llu,\"RealEndTime\":%llu,"
      "\"StartOffset\":%llu,\"EndOffset\":%llu,\"DisplayName\":\"%s\",\"Recurrence\":%i,"
      "\"ChannelListId\":%i,\"Profile\":\"%s\"}",
      timer.GetClientChannelUid(), "Idle",
      static_cast<unsigned long long>(timer.GetStartTime()) * 1000,
      static_cast<unsigned long long>(timer.GetEndTime()) * 1000,
      static_cast<unsigned long long>(timer.GetMarginStart()) * 1000,
      static_cast<unsigned long long>(timer.GetMarginEnd()) * 1000, timer.GetTitle().c_str(), 0, 0,
      DEFAULT_REC_PROFILE);

  cRest rest;
  std::string strUrl = m_strBaseUrl + URI_REST_TIMER;
  int retval = rest.Post(strUrl, strQueryString, response);

  if (retval >= 0)
  {
    if (response.type() == Json::objectValue)
    {
      retval = 0;
    }
    else
    {
      kodi::Log(ADDON_LOG_DEBUG, "Unknown response format. Expected Json::arrayValue\n");
      return -1;
    }
  }
  else
  {
    kodi::Log(ADDON_LOG_DEBUG, "Request Timer failed. Return value: %i\n", retval);
    return -1;
  }

  // Trigger a timer update to receive new timer from Broadway
  kodi::addon::CInstancePVRClient::TriggerTimerUpdate();
  if (timer.GetStartTime() <= 0)
  {
    // Refresh the recordings
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    kodi::addon::CInstancePVRClient::TriggerRecordingUpdate();
  }

  return retval;
}

PVR_ERROR Pctv::AddTimer(const kodi::addon::PVRTimer& timer)
{
  if (!IsConnected())
    return PVR_ERROR_SERVER_ERROR;

  kodi::Log(ADDON_LOG_DEBUG, "AddTimer iClientChannelUid: %i\n", timer.GetClientChannelUid());

  Json::Value data;
  int retval = RESTAddTimer(timer, data);
  if (retval == 0)
  {
    return PVR_ERROR_NO_ERROR;
  }

  return PVR_ERROR_SERVER_ERROR;
}

/************************************************************/
/** EPG  */

PVR_ERROR Pctv::GetEPGForChannel(int channelUid,
                                 time_t start,
                                 time_t end,
                                 kodi::addon::PVREPGTagsResultSet& results)
{
  if (!IsConnected())
    return PVR_ERROR_SERVER_ERROR;

  Json::Value data;
  for (const auto& myChannel : m_channels)
  {
    if (myChannel.iUniqueId != channelUid)
      continue;
    if (!GetEPG(channelUid, start, end, data))
      continue;
    if (data.size() <= 0)
      continue;

    for (unsigned int index = 0; index < data.size(); ++index)
    {
      Json::Value buffer = data[index];
      int iChannelId = buffer["Id"].asInt();
      Json::Value entries = buffer["Entries"];

      for (unsigned int i = 0; i < entries.size(); ++i)
      {
        Json::Value entry = entries[i];

        kodi::addon::PVREPGTag epg;

        epg.SetUniqueBroadcastId(IsSupported("broadway")
                                     ? entry["Id"].asUInt()
                                     : GetEventId((long long)entry["Id"].asDouble()));
        epg.SetTitle(entry["Title"].asCString());
        epg.SetUniqueChannelId(iChannelId);
        epg.SetStartTime(static_cast<time_t>(entry["StartTime"].asDouble() / 1000));
        epg.SetEndTime(static_cast<time_t>(entry["EndTime"].asDouble() / 1000));
        epg.SetPlotOutline(entry["LongDescription"].asCString());
        epg.SetPlot(entry["ShortDescription"].asCString());
        epg.SetFlags(EPG_TAG_FLAG_UNDEFINED);

        results.Add(epg);
      }
    }

    return PVR_ERROR_NO_ERROR;
  }

  return PVR_ERROR_SERVER_ERROR;
}

unsigned int Pctv::GetEventId(long long EntryId)
{
  return (unsigned int)((EntryId >> 32) & 0xFFFFFFFFL);
}

bool Pctv::GetEPG(int id, time_t iStart, time_t iEnd, Json::Value& data)
{
  int retval = RESTGetEpg(id, iStart, iEnd, data);
  if (retval < 0)
  {
    kodi::Log(ADDON_LOG_ERROR, "No EPG data retrieved.");
    return false;
  }

  kodi::Log(ADDON_LOG_INFO, "EPG Loaded.");
  return true;
}


int Pctv::RESTGetEpg(int id, time_t iStart, time_t iEnd, Json::Value& response)
{
  std::string strParams;
  //strParams= StringUtils::Format("?ids=%d&extended=1&start=%d&end=%d", id, iStart * 1000, iEnd * 1000);
  strParams = StringUtils::Format("?ids=%d&extended=1&start=%llu&end=%llu", id,
                                  static_cast<unsigned long long>(iStart) * 1000,
                                  static_cast<unsigned long long>(iEnd) * 1000);

  cRest rest;
  std::string strUrl = m_strBaseUrl + URI_REST_EPG;
  int retval = rest.Get(strUrl, strParams, response);
  if (retval >= 0)
  {
    if (response.type() == Json::arrayValue)
    {
      return response.size();
    }
    else
    {
      kodi::Log(ADDON_LOG_DEBUG, "Unknown response format. Expected Json::arrayValue\n");
      return -1;
    }
  }
  else
  {
    kodi::Log(ADDON_LOG_DEBUG, "Request EPG failed. Return value: %i\n", retval);
  }

  return retval;
}


std::string Pctv::GetPreviewParams(int dataIdentifier, Json::Value entry)
{
  std::string strStid = GetStid(dataIdentifier);
  std::string strTmp;
  if (entry["File"].isString())
  { // Gallery entry
    strTmp = StringUtils::Format(
        "stid=%s&galleryid=%.0f&file=%s&profile=%s", strStid.c_str(), entry["Id"].asDouble(),
        URLEncodeInline(entry["File"].asString()).c_str(), GetTranscodeProfileValue().c_str());
    return strTmp;
  }

  // channel entry
  strTmp = StringUtils::Format("channel=%i&mode=%s&profile=%s&stid=%s", entry["Id"].asInt(),
                               m_strPreviewMode.c_str(), GetTranscodeProfileValue().c_str(),
                               strStid.c_str());
  return strTmp;
}

std::string Pctv::GetTranscodeProfileValue()
{
  std::string strProfile;
  if (!m_bTranscode)
  {
    strProfile = StringUtils::Format("%s.Native.NR", m_strPreviewMode.c_str());
  }
  else
  {
    strProfile = StringUtils::Format("%s.%ik.HR", m_strPreviewMode.c_str(), m_iBitrate);
  }

  return strProfile;
}

std::string Pctv::GetPreviewUrl(std::string params)
{
  std::string strTmp;
  strTmp = StringUtils::Format("%s/TVC/Preview?%s", m_strBaseUrl.c_str(), params.c_str());
  return strTmp;
}

std::string Pctv::GetStid(int defaultStid)
{
  if (m_strStid == "")
  {
    m_strStid = StringUtils::Format("_xbmc%i", defaultStid);
  }

  return m_strStid;
}

std::string Pctv::GetChannelLogo(Json::Value entry)
{
  std::string strNameParam;
  strNameParam =
      StringUtils::Format("%s/TVC/Resource?type=1&default=emptyChannelLogo&name=%s",
                          m_strBaseUrl.c_str(), URLEncodeInline(GetShortName(entry)).c_str());
  return strNameParam;
}

std::string Pctv::GetShortName(Json::Value entry)
{
  std::string strShortName;
  if (entry["shortName"].isNull())
  {
    strShortName = entry["DisplayName"].asString();
    if (strShortName == "")
    {
      strShortName = entry["Name"].asString();
    }
    std::replace(strShortName.begin(), strShortName.end(), ' ', '_');
  }

  return strShortName;
}

bool Pctv::IsConnected()
{
  return m_bIsConnected;
}

bool Pctv::GetFreeConfig()
{
  std::string strConfig = "";

  cRest rest;
  Json::Value response;
  std::string strUrl = m_strBackendUrlNoAuth + URI_REST_CONFIG;
  int retval = rest.Get(strUrl, "", response);
  if (retval != E_FAILED)
  {
    if (response.type() == Json::objectValue)
    {
      m_config.init(response);
    }
    return true;
  }

  return false;
}

PVR_ERROR Pctv::GetCapabilities(kodi::addon::PVRCapabilities& capabilities)
{
  capabilities.SetSupportsEPG(true);
  capabilities.SetSupportsTV(true);
  capabilities.SetSupportsRadio(false);
  capabilities.SetSupportsChannelGroups(true);
  capabilities.SetSupportsRecordings(true);
  capabilities.SetSupportsRecordingsUndelete(false);
  capabilities.SetSupportsTimers(true);
  capabilities.SetSupportsChannelScan(false);
  capabilities.SetHandlesInputStream(false);
  capabilities.SetHandlesDemuxing(false);
  capabilities.SetSupportsLastPlayedPosition(false);
  capabilities.SetSupportsRecordingsRename(false);
  capabilities.SetSupportsRecordingsLifetimeChange(false);
  capabilities.SetSupportsDescrambleInfo(false);

  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR Pctv::GetBackendName(std::string& name)
{
  name = m_strBackendName;
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR Pctv::GetBackendVersion(std::string& version)
{
  version = m_strBackendVersion;
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR Pctv::GetConnectionString(std::string& connection)
{
  connection =
      StringUtils::Format("%s%s", m_strHostname.c_str(), IsConnected() ? "" : " (Not connected!)");
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR Pctv::GetBackendHostname(std::string& hostname)
{
  hostname = m_strHostname;
  return PVR_ERROR_NO_ERROR;
}

bool Pctv::GetChannel(const kodi::addon::PVRChannel& channel, PctvChannel& myChannel)
{
  for (unsigned int iChannelPtr = 0; iChannelPtr < m_channels.size(); iChannelPtr++)
  {
    PctvChannel& thisChannel = m_channels.at(iChannelPtr);
    if (thisChannel.iUniqueId == (int)channel.GetUniqueId())
    {
      myChannel.iUniqueId = thisChannel.iUniqueId;
      myChannel.bRadio = thisChannel.bRadio;
      myChannel.iChannelNumber = thisChannel.iChannelNumber;
      myChannel.iEncryptionSystem = thisChannel.iEncryptionSystem;
      myChannel.strChannelName = thisChannel.strChannelName;
      myChannel.strLogoPath = thisChannel.strLogoPath;
      myChannel.strStreamURL = thisChannel.strStreamURL;
      return true;
    }
  }

  return false;
}

bool Pctv::IsRecordFolderSet(std::string& partitionId)
{
  Json::Value data;
  int retval = RESTGetFolder(data); // get folder config
  if (retval <= 0)
    return false;

  for (unsigned int i = 0; i < data.size(); i++)
  {
    Json::Value folder = data[i];
    if (folder["Type"].asString() == "record")
    {
      partitionId = folder["DevicePartitionId"].asString();
      return true;
    }
  }

  return false;
}

int Pctv::RESTGetFolder(Json::Value& response)
{
  kodi::Log(ADDON_LOG_DEBUG, "%s - get folder config via REST interface", __func__);

  cRest rest;
  std::string strUrl = m_strBaseUrl + URI_REST_FOLDER;
  int retval = rest.Get(strUrl, "", response);
  if (retval >= 0)
  {
    if (response.type() == Json::arrayValue)
    {
      return response.size();
    }
    else
    {
      kodi::Log(ADDON_LOG_DEBUG, "Unknown response format. Expected Json::arrayValue\n");
      return -1;
    }
  }
  else
  {
    kodi::Log(ADDON_LOG_DEBUG, "Request folder data failed. Return value: %i\n", retval);
  }

  return retval;
}

int Pctv::RESTGetStorage(Json::Value& response)
{
  kodi::Log(ADDON_LOG_DEBUG, "%s - get storage data via REST interface", __func__);

  cRest rest;
  std::string strUrl = m_strBaseUrl + URI_REST_STORAGE;
  int retval = rest.Get(strUrl, "", response);
  if (retval >= 0)
  {
    if (response.type() == Json::arrayValue)
    {
      return response.size();
    }
    else
    {
      kodi::Log(ADDON_LOG_DEBUG, "Unknown response format. Expected Json::arrayValue\n");
      return -1;
    }
  }
  else
  {
    kodi::Log(ADDON_LOG_DEBUG, "Request storage data failed. Return value: %i\n", retval);
  }

  return retval;
}

PVR_ERROR Pctv::GetDriveSpace(uint64_t& total, uint64_t& used)
{
  if (!IsConnected())
    return PVR_ERROR_SERVER_ERROR;

  if (!IsSupported("storage"))
    return PVR_ERROR_NOT_IMPLEMENTED;

  m_partitions.clear();
  std::string strPartitionId = "";

  bool isRecordFolder = IsRecordFolderSet(strPartitionId);

  if (isRecordFolder)
  {
    Json::Value data;

    int retval = RESTGetStorage(data); // get storage data
    if (retval <= 0)
    {
      kodi::Log(ADDON_LOG_ERROR, "No storage available.");
      return PVR_ERROR_SERVER_ERROR;
    }

    for (unsigned int i = 0; i < data.size(); i++)
    {
      Json::Value storage = data[i];
      std::string deviceId = storage["Id"].asString();
      Json::Value devicePartitions = storage["Partitions"];

      int iCount = devicePartitions.size();
      if (iCount > 0)
      {
        for (int p = 0; p < iCount; p++)
        {
          Json::Value partition;
          partition = devicePartitions[p];

          std::string strDevicePartitionId;
          strDevicePartitionId =
              StringUtils::Format("%s.%s", deviceId.c_str(), partition["Id"].asString().c_str());

          if (strDevicePartitionId == strPartitionId)
          {
            uint32_t size = partition["Size"].asUInt();
            uint32_t available = partition["Available"].asUInt();

            total = size;
            used = (size - available);

            /* Convert from kBytes to Bytes */
            total *= 1024;
            used *= 1024;
            return PVR_ERROR_NO_ERROR;
          }
        }
      }
    }
  }

  return PVR_ERROR_SERVER_ERROR;
}

/* ################ misc ################ */

/*
* \brief Get a channel list from PCTV Device via REST interface
* \param id The channel list id
*/
int Pctv::RESTGetChannelList(int id, Json::Value& response)
{
  kodi::Log(ADDON_LOG_DEBUG, "%s - get channel list entries via REST interface", __func__);
  int retval = -1;
  cRest rest;

  if (id == 0) // all channels
  {
    std::string strUrl = m_strBaseUrl + URI_REST_CHANNELS;
    retval = rest.Get(strUrl, "?available=1", response);
    if (retval >= 0)
    {
      if (response.type() == Json::arrayValue)
      {
        return response.size();
      }
      else
      {
        kodi::Log(ADDON_LOG_DEBUG, "Unknown response format. Expected Json::arrayValue\n");
        return -1;
      }
    }
    else
    {
      kodi::Log(ADDON_LOG_DEBUG, "Request Channel List failed. Return value: %i\n", retval);
    }
  }
  else if (id > 0)
  {
    char url[255];
    sprintf(url, "%s%s/%i", m_strBaseUrl.c_str(), URI_REST_CHANNELLISTS, id);

    retval = rest.Get(url, "?available=1", response);
    if (retval >= 0)
    {
      if (response.type() == Json::objectValue)
      {
        return response.size();
      }
      else
      {
        kodi::Log(ADDON_LOG_DEBUG, "Unknown response format. Expected Json::objectValue\n");
        return -1;
      }
    }
    else
    {
      kodi::Log(ADDON_LOG_DEBUG, "Request Channel List failed. Return value: %i\n", retval);
    }
  }

  return retval;
}

bool Pctv::IsSupported(const std::string& cap)
{
  return m_config.hasCapability(cap);
}


const char SAFE[256] = {
    /*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
    /* 0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 1 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 2 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 3 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,

    /* 4 */ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* 5 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
    /* 6 */ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* 7 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,

    /* 8 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 9 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* A */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* B */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    /* C */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* D */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* E */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* F */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


std::string Pctv::URLEncodeInline(const std::string& sSrc)
{
  const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
  const unsigned char* pSrc = (const unsigned char*)sSrc.c_str();
  const int SRC_LEN = sSrc.length();
  unsigned char* const pStart = new unsigned char[SRC_LEN * 3];
  unsigned char* pEnd = pStart;
  const unsigned char* const SRC_END = pSrc + SRC_LEN;

  for (; pSrc < SRC_END; ++pSrc)
  {
    if (SAFE[*pSrc])
      *pEnd++ = *pSrc;
    else
    {
      // escape this char
      *pEnd++ = '%';
      *pEnd++ = DEC2HEX[*pSrc >> 4];
      *pEnd++ = DEC2HEX[*pSrc & 0x0F];
    }
  }

  std::string sResult((char*)pStart, (char*)pEnd);
  delete[] pStart;
  return sResult;
}
