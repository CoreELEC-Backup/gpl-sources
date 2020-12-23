/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "VBoxInstance.h"

#include "timeshift/DummyBuffer.h"
#include "timeshift/FilesystemBuffer.h"
#include "vbox/ContentIdentifier.h"
#include "vbox/RecordingReader.h"
#include "vbox/Settings.h"

#include <algorithm>

#include <kodi/General.h>

using namespace vbox;

// settings context menu
unsigned int MENUHOOK_ID_RESCAN_EPG = 1;
unsigned int MENUHOOK_ID_SYNC_EPG = 2;

CVBoxInstance::CVBoxInstance(const Settings& settings, KODI_HANDLE instance, const std::string& version)
  : kodi::addon::CInstancePVRClient(instance, version),
    VBox(settings)
{

}

CVBoxInstance::~CVBoxInstance()
{
  delete m_timeshiftBuffer;
}

ADDON_STATUS CVBoxInstance::Initialize()
{
  ADDON_STATUS status = ADDON_STATUS_UNKNOWN;

  // Validate settings
  if (VBox::ValidateSettings())
  {
    // Start the addon
    try
    {
      VBox::Initialize();
      status = ADDON_STATUS_OK;

      // Attach event handlers
      VBox::OnChannelsUpdated = [this]() { kodi::addon::CInstancePVRClient::TriggerChannelUpdate(); };
      VBox::OnRecordingsUpdated = [this]() { kodi::addon::CInstancePVRClient::TriggerRecordingUpdate(); };
      VBox::OnTimersUpdated = [this]() { kodi::addon::CInstancePVRClient::TriggerTimerUpdate(); };
      VBox::OnGuideUpdated = [this]()
      {
        for (const auto& channel : VBox::GetChannels())
        {
          kodi::addon::CInstancePVRClient::TriggerEpgUpdate(ContentIdentifier::GetUniqueId(channel));
        }
      };

      // Create the timeshift buffer
      if (VBox::GetSettings().m_timeshiftEnabled)
        m_timeshiftBuffer = new timeshift::FilesystemBuffer(VBox::GetSettings().m_timeshiftBufferPath);
      else
        m_timeshiftBuffer = new timeshift::DummyBuffer();

      m_timeshiftBuffer->SetReadTimeout(VBox::GetConnectionParams().timeout);

      // initializing TV Settings Client Specific menu hooks
      std::vector<kodi::addon::PVRMenuhook> hooks = {{MENUHOOK_ID_RESCAN_EPG, 30106, PVR_MENUHOOK_SETTING},
                                                     {MENUHOOK_ID_SYNC_EPG, 30107, PVR_MENUHOOK_SETTING}};

      for (auto& hook : hooks)
        kodi::addon::CInstancePVRClient::AddMenuHook(hook);
    }
    catch (FirmwareVersionException& e)
    {
      kodi::QueueNotification(QUEUE_ERROR, "", e.what());
      status = ADDON_STATUS_PERMANENT_FAILURE;
    }
    catch (VBoxException& e)
    {
      VBox::LogException(e);
      status = ADDON_STATUS_LOST_CONNECTION;
    }
  }
  else
    status = ADDON_STATUS_NEED_SETTINGS;

  return status;
}

PVR_ERROR CVBoxInstance::GetCapabilities(kodi::addon::PVRCapabilities& capabilities)
{
  capabilities.SetSupportsTV(true);
  capabilities.SetSupportsRadio(true);
  capabilities.SetSupportsChannelGroups(false);
  capabilities.SetSupportsEPG(true);
  capabilities.SetHandlesInputStream(true);

  // Recording capability is determined further down, we'll assume false
  // in case the real capabilities cannot be determined for some reason
  capabilities.SetSupportsRecordings(false);
  capabilities.SetSupportsTimers(false);

  // Unsupported features
  capabilities.SetSupportsRecordingsUndelete(false);
  capabilities.SetSupportsChannelScan(false);
  capabilities.SetSupportsChannelSettings(false);
  capabilities.SetHandlesDemuxing(false);
  capabilities.SetSupportsRecordingPlayCount(false);
  capabilities.SetSupportsLastPlayedPosition(false);
  capabilities.SetSupportsRecordingEdl(false);

  // Wait for initialization until we decide if we support recordings or not.
  // Recording is only possible when external media is present
  if (VBox::GetStateHandler().WaitForState(StartupState::INITIALIZED) && VBox::SupportsRecordings())
  {
    capabilities.SetSupportsRecordings(true);
    capabilities.SetSupportsTimers(true);
  }

  capabilities.SetSupportsRecordingsRename(false);
  capabilities.SetSupportsRecordingsLifetimeChange(false);
  capabilities.SetSupportsDescrambleInfo(false);

  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR CVBoxInstance::GetBackendName(std::string& name)
{
  name = VBox::GetBackendName();
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR CVBoxInstance::GetBackendVersion(std::string& version)
{
  version = VBox::GetBackendVersion();
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR CVBoxInstance::GetBackendHostname(std::string& hostname)
{
  hostname = VBox::GetBackendHostname();
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR CVBoxInstance::GetConnectionString(std::string& connection)
{
  connection = VBox::GetConnectionString();
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR CVBoxInstance::GetDriveSpace(uint64_t& total, uint64_t& used)
{
  total = VBox::GetRecordingTotalSpace() / 1024;
  used = VBox::GetRecordingUsedSpace() / 1024;
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR CVBoxInstance::CallSettingsMenuHook(const kodi::addon::PVRMenuhook& menuhook)
{
  if (menuhook.GetHookId() == MENUHOOK_ID_RESCAN_EPG)
  {
    kodi::QueueNotification(QUEUE_INFO, "", "Rescanning EPG, this will take a while");
    VBox::StartEPGScan();
    return PVR_ERROR_NO_ERROR;
  }
  else if (menuhook.GetHookId() == MENUHOOK_ID_SYNC_EPG)
  {
    kodi::QueueNotification(QUEUE_INFO, "", "Getting EPG from VBox device");
    VBox::SyncEPGNow();
    return PVR_ERROR_NO_ERROR;
  }
  return PVR_ERROR_INVALID_PARAMETERS;
}

PVR_ERROR CVBoxInstance::GetChannelsAmount(int& amount)
{
  try
  {
    amount = VBox::GetChannelsAmount();
    return PVR_ERROR_NO_ERROR;
  }
  catch (VBoxException& e)
  {
    VBox::LogException(e);
    return PVR_ERROR_SERVER_ERROR;
  }
}

PVR_ERROR CVBoxInstance::GetChannels(bool radio, kodi::addon::PVRChannelsResultSet& results)
{
  auto& channels = VBox::GetChannels();
  unsigned int i = 0;

  for (const auto& item : channels)
  {
    // Skip those that are not of the correct type
    if (item->m_radio != radio)
      continue;

    kodi::addon::PVRChannel channel;

    channel.SetUniqueId(ContentIdentifier::GetUniqueId(item));
    channel.SetIsRadio(item->m_radio);

    // Override LCN if backend channel order should be forced
    ++i;
    if (VBox::GetSettings().m_setChannelIdUsingOrder == CH_ORDER_BY_INDEX)
      channel.SetChannelNumber(i);
    // default - CH_ORDER_BY_LCN
    else
      channel.SetChannelNumber(item->m_number);

    channel.SetEncryptionSystem(item->m_encrypted ? 0xFFFF : 0x0000);

    channel.SetChannelName(item->m_name);
    channel.SetIconPath(item->m_iconUrl);

    // Set stream format for TV channels
    if (!item->m_radio)
      channel.SetMimeType("video/mp2t");

    kodi::Log(ADDON_LOG_INFO, "Adding channel %d: %s. Icon: %s", channel.GetChannelNumber(), channel.GetChannelName().c_str(),
              channel.GetIconPath().c_str());

    results.Add(channel);
  }

  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR CVBoxInstance::GetSignalStatus(int channelUid, kodi::addon::PVRSignalStatus& signalStatus)
{
  const ChannelPtr currentChannel = VBox::GetCurrentChannel();

  if (!currentChannel)
    return PVR_ERROR_NO_ERROR;

  try
  {
    ChannelStreamingStatus status = VBox::GetChannelStreamingStatus(currentChannel);

    // Adjust for Kodi's weird handling of the signal strength
    signalStatus.SetSignal(static_cast<int>(status.GetSignalStrength()) * 655);
    signalStatus.SetSNR(static_cast<int>(status.m_signalQuality) * 655);
    signalStatus.SetBER(status.GetBer());

    signalStatus.SetAdapterName(status.GetTunerName());
    signalStatus.SetAdapterStatus(status.m_lockStatus);
    signalStatus.SetServiceName(status.GetServiceName());
    signalStatus.SetMuxName(status.GetMuxName());
  }
  catch (VBoxException& e)
  {
    VBox::LogException(e);
  }

  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR CVBoxInstance::GetRecordingsAmount(bool deleted, int& amount)
{
  amount = VBox::GetRecordingsAmount();
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR CVBoxInstance::GetRecordings(bool deleted, kodi::addon::PVRRecordingsResultSet& results)
{
  auto& recordings = VBox::GetRecordingsAndTimers();

  for (const auto& item : recordings)
  {
    // Skip timers
    if (!item->IsRecording())
      continue;

    kodi::addon::PVRRecording recording;
    recording.SetSeriesNumber(PVR_RECORDING_INVALID_SERIES_EPISODE);
    recording.SetEpisodeNumber(PVR_RECORDING_INVALID_SERIES_EPISODE);

    time_t startTime = xmltv::Utilities::XmltvToUnixTime(item->m_startTime);
    time_t endTime = xmltv::Utilities::XmltvToUnixTime(item->m_endTime);
    unsigned int id = item->m_id;

    recording.SetRecordingTime(startTime);
    std::time_t now = std::time(nullptr);
    if (now > endTime)
      recording.SetDuration(static_cast<int>(endTime - startTime));
    else
      recording.SetDuration(static_cast<int>(now - startTime));
    recording.SetEPGEventId(id);

    recording.SetChannelName(item->m_channelName);
    recording.SetRecordingId(std::to_string(id));
    recording.SetTitle(item->m_title);
    recording.SetPlot(item->m_description);

    recording.SetChannelUid(PVR_CHANNEL_INVALID_UID);
    recording.SetChannelType(PVR_RECORDING_CHANNEL_TYPE_UNKNOWN);

    // Find the recordings channel and use its unique ID if we find one
    auto& channels = VBox::GetChannels();
    auto it = std::find_if(
      channels.cbegin(),
      channels.cend(),
      [&item](const ChannelPtr& channel) {
        return channel->m_xmltvName == item->m_channelId;
      }
    );

    if (it != channels.cend())
    {
      ChannelPtr channel = *it;
      recording.SetChannelUid(ContentIdentifier::GetUniqueId(channel));
      if (channel->m_radio)
        recording.SetChannelType(PVR_RECORDING_CHANNEL_TYPE_RADIO);
      else
        recording.SetChannelType(PVR_RECORDING_CHANNEL_TYPE_TV);
    }

    results.Add(recording);
  }

  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR CVBoxInstance::DeleteRecording(const kodi::addon::PVRRecording& recording)
{
  try
  {
    unsigned int id = static_cast<unsigned int>(std::stoi(recording.GetRecordingId()));

    if (VBox::DeleteRecordingOrTimer(id))
      return PVR_ERROR_NO_ERROR;
    else
      return PVR_ERROR_FAILED;
  }
  catch (...)
  {
    return PVR_ERROR_INVALID_PARAMETERS;
  }
}

PVR_ERROR CVBoxInstance::GetTimerTypes(std::vector<kodi::addon::PVRTimerType>& types)
{
  // EPG based single recording
  {
    kodi::addon::PVRTimerType type;
    type.SetId(vbox::TIMER_VBOX_TYPE_EPG_BASED_SINGLE);
    type.SetDescription("EPG-based one time recording");
    type.SetAttributes(
        PVR_TIMER_TYPE_REQUIRES_EPG_TAG_ON_CREATE |
        PVR_TIMER_TYPE_SUPPORTS_START_TIME |
        PVR_TIMER_TYPE_SUPPORTS_END_TIME);
    types.emplace_back(type);
  }

  // episode recording
  {
    kodi::addon::PVRTimerType type;
    type.SetId(TIMER_VBOX_TYPE_EPISODE);
    type.SetDescription("Episode recording");
    type.SetAttributes(
        PVR_TIMER_TYPE_IS_READONLY |
        PVR_TIMER_TYPE_SUPPORTS_START_TIME |
        PVR_TIMER_TYPE_SUPPORTS_END_TIME);
    types.emplace_back(type);
  }

  // manual recording
  {
    kodi::addon::PVRTimerType type;
    type.SetId(TIMER_VBOX_TYPE_MANUAL_SINGLE);
    type.SetDescription("Manual one time recording");
    type.SetAttributes(
        PVR_TIMER_TYPE_IS_MANUAL |
        PVR_TIMER_TYPE_FORBIDS_EPG_TAG_ON_CREATE |
        PVR_TIMER_TYPE_SUPPORTS_CHANNELS |
        PVR_TIMER_TYPE_SUPPORTS_START_TIME |
        PVR_TIMER_TYPE_SUPPORTS_END_TIME);
    types.emplace_back(type);
  }

  //Automatic series recording
  {
    kodi::addon::PVRTimerType type;
    type.SetId(TIMER_VBOX_TYPE_EPG_BASED_AUTO_SERIES);
    type.SetDescription("EPG-based automatic series recording");
    type.SetAttributes(
        PVR_TIMER_TYPE_REQUIRES_EPG_SERIES_ON_CREATE);
    types.emplace_back(type);
  }

  // EPG based series recording
  {
    kodi::addon::PVRTimerType type;
    type.SetId(TIMER_VBOX_TYPE_EPG_BASED_MANUAL_SERIES);
    type.SetDescription("EPG-based manual series recording");
    type.SetAttributes(
        PVR_TIMER_TYPE_IS_REPEATING |
        PVR_TIMER_TYPE_REQUIRES_EPG_TAG_ON_CREATE |
        PVR_TIMER_TYPE_SUPPORTS_START_TIME |
        PVR_TIMER_TYPE_SUPPORTS_END_TIME |
        PVR_TIMER_TYPE_SUPPORTS_WEEKDAYS);
    types.emplace_back(type);
  }

  //Manual series recording
  {
    kodi::addon::PVRTimerType type;
    type.SetId(TIMER_VBOX_TYPE_MANUAL_SERIES);
    type.SetDescription("Manual series recording");
    type.SetAttributes(
        PVR_TIMER_TYPE_IS_MANUAL |
        PVR_TIMER_TYPE_IS_REPEATING |
        PVR_TIMER_TYPE_FORBIDS_EPG_TAG_ON_CREATE |
        PVR_TIMER_TYPE_SUPPORTS_CHANNELS |
        PVR_TIMER_TYPE_SUPPORTS_START_TIME |
        PVR_TIMER_TYPE_SUPPORTS_END_TIME |
        PVR_TIMER_TYPE_SUPPORTS_WEEKDAYS);
    types.emplace_back(type);
  }

  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR CVBoxInstance::GetTimersAmount(int& amount)
{
  amount = VBox::GetTimersAmount();
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR CVBoxInstance::GetTimers(kodi::addon::PVRTimersResultSet& results)
{
  /* TODO: Change implementation to get support for the timer features introduced with PVR API 1.9.7 */
  auto& recordings = VBox::GetRecordingsAndTimers();

  // first get timers from single recordings (scheduled)
  for (const auto& item : recordings)
  {
    // Skip recordings
    if (!item->IsTimer())
      continue;

    kodi::addon::PVRTimer timer;
    timer.SetTimerType((item->m_seriesId > 0) ? vbox::TIMER_VBOX_TYPE_EPISODE : vbox::TIMER_VBOX_TYPE_MANUAL_SINGLE);
    timer.SetStartTime(xmltv::Utilities::XmltvToUnixTime(item->m_startTime));
    timer.SetEndTime(xmltv::Utilities::XmltvToUnixTime(item->m_endTime));
    timer.SetClientIndex(item->m_id);

    // Convert the internal timer state to PVR_TIMER_STATE
    switch (item->GetState())
    {
      case RecordingState::SCHEDULED:
        timer.SetState(PVR_TIMER_STATE_SCHEDULED);
        break;
      case RecordingState::RECORDED:
      case RecordingState::EXTERNAL:
        timer.SetState(PVR_TIMER_STATE_COMPLETED);
        break;
      case RecordingState::RECORDING:
        timer.SetState(PVR_TIMER_STATE_RECORDING);
        break;
      case RecordingState::RECORDING_ERROR:
        break;
    }

    // Find the timer's channel and use its unique ID
    auto& channels = VBox::GetChannels();
    auto it = std::find_if(channels.cbegin(), channels.cend(),
      [&item](const ChannelPtr& channel) {
        return channel->m_xmltvName == item->m_channelId;
      }
    );

    if (it != channels.cend())
      timer.SetClientChannelUid(ContentIdentifier::GetUniqueId(*it));
    else
      continue;

    timer.SetTitle(item->m_title);
    timer.SetSummary(item->m_description);

    kodi::Log(ADDON_LOG_DEBUG, "GetTimers(): adding timer to show %s", item->m_title.c_str());
    // TODO: Set margins to whatever the API reports
    results.Add(timer);
  }
  // second: get timer rules for series
  auto& series = VBox::GetSeriesTimers();
  for (const auto& item : series)
  {
    kodi::addon::PVRTimer timer;

    timer.SetTimerType((item->m_fIsAuto) ? vbox::TIMER_VBOX_TYPE_EPG_BASED_AUTO_SERIES : vbox::TIMER_VBOX_TYPE_MANUAL_SERIES);
    timer.SetClientIndex(item->m_id);
    timer.SetState(PVR_TIMER_STATE_SCHEDULED);

    // Find the timer's channel and use its unique ID
    auto& channels = VBox::GetChannels();
    auto it = std::find_if(channels.cbegin(), channels.cend(),
      [&item](const ChannelPtr& channel) {
        return channel->m_xmltvName == item->m_channelId;
      }
    );

    if (it != channels.cend())
      timer.SetClientChannelUid(ContentIdentifier::GetUniqueId(*it));

    unsigned int nextScheduledId = item->m_scheduledId;
    // Find next recording of the series
    auto recIt = std::find_if(recordings.begin(), recordings.end(),
      [nextScheduledId](const RecordingPtr& recording) {
        return nextScheduledId == recording->m_id;
      }
    );
    // if it doesn't exist (canceled) - don't add series
    if (recIt == recordings.end())
      continue;

    timer.SetStartTime(xmltv::Utilities::XmltvToUnixTime(item->m_startTime));
    // automatic starts & stops whenever detected (will appear as episode)
    if (item->m_fIsAuto)
    {
      timer.SetStartAnyTime(true);
      timer.SetEndAnyTime(true);
    }
    else
    {
      // set periodic times
      timer.SetFirstDay(xmltv::Utilities::XmltvToUnixTime(item->m_startTime));
      timer.SetWeekdays(item->m_weekdays);
      timer.SetEndTime(xmltv::Utilities::XmltvToUnixTime(item->m_endTime));
    }
    timer.SetTitle(item->m_title);

    timer.SetSummary(item->m_description);

    // TODO: Set margins to whatever the API reports
    results.Add(timer);
  }
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR CVBoxInstance::AddTimer(const kodi::addon::PVRTimer& timer)
{
  kodi::Log(ADDON_LOG_DEBUG, "AddTimer() : entering with timer type 0x%x", timer.GetTimerType());
  // Find the channel the timer is for
  auto& channels = VBox::GetChannels();
  auto it = std::find_if(channels.cbegin(), channels.cend(),
    [&timer](const ChannelPtr& channel) {
      return timer.GetClientChannelUid() == ContentIdentifier::GetUniqueId(channel);
    }
  );

  if (it == channels.end())
    return PVR_ERROR_INVALID_PARAMETERS;

  const ChannelPtr channel = *it;

  // Find the channel's schedule
  const Schedule schedule = VBox::GetSchedule(channel);

  try
  {
    // update the recording margins in the backend
    VBox::UpdateRecordingMargins({timer.GetMarginStart(), timer.GetMarginEnd()});
    // Set start time to now if it's missing
    time_t startTime = timer.GetStartTime();
    time_t endTime = timer.GetEndTime();
    std::string title(timer.GetTitle());
    std::string desc(timer.GetSummary());

    if (startTime == 0)
      startTime = time(nullptr);

    // Add a programme-based timer if the programme exists in the schedule
    const xmltv::ProgrammePtr programme =
        (schedule.schedule) ? schedule.schedule->GetProgramme(timer.GetEPGUid()) : nullptr;

    switch (timer.GetTimerType())
    {
      case TIMER_VBOX_TYPE_EPG_BASED_SINGLE:
      case TIMER_VBOX_TYPE_EPISODE:
        if (programme)
        {
          switch (schedule.origin)
          {
            case Schedule::Origin::INTERNAL_GUIDE:
              VBox::AddTimer(channel, programme);
              break;
            case Schedule::Origin::EXTERNAL_GUIDE:
              title = programme->m_title;
              desc = programme->m_description;
              VBox::AddTimer(channel, startTime, endTime, title, desc);
              break;
          }
          return PVR_ERROR_NO_ERROR;
        }
        else
        {
          VBox::AddTimer(channel, startTime, endTime, title, desc);
          return PVR_ERROR_NO_ERROR;
        }
      case TIMER_VBOX_TYPE_MANUAL_SINGLE:
        VBox::AddTimer(channel, startTime, endTime, title, desc);
        return PVR_ERROR_NO_ERROR;
      case TIMER_VBOX_TYPE_EPG_BASED_AUTO_SERIES:
      {
        if (!programme)
          return PVR_ERROR_INVALID_PARAMETERS;
        VBox::AddSeriesTimer(channel, programme);
        return PVR_ERROR_NO_ERROR;
      }
      case TIMER_VBOX_TYPE_EPG_BASED_MANUAL_SERIES:
      {
        if (!programme)
          return PVR_ERROR_INVALID_PARAMETERS;
        VBox::AddTimer(channel, startTime, endTime, title, desc, timer.GetWeekdays());
        return PVR_ERROR_NO_ERROR;
      }
      case TIMER_VBOX_TYPE_MANUAL_SERIES:
      {
        VBox::AddTimer(channel, startTime, endTime, title, desc, timer.GetWeekdays());
        return PVR_ERROR_NO_ERROR;
      }
      default:
        // any other timer type is wrong
        return PVR_ERROR_INVALID_PARAMETERS;
    }
  }
  catch (VBoxException& e)
  {
    VBox::LogException(e);
    return PVR_ERROR_FAILED;
  }
}

PVR_ERROR CVBoxInstance::DeleteTimer(const kodi::addon::PVRTimer& timer, bool forceDelete)
{
  if (VBox::DeleteRecordingOrTimer(timer.GetClientIndex()))
    return PVR_ERROR_NO_ERROR;

  return PVR_ERROR_FAILED;
}

PVR_ERROR CVBoxInstance::UpdateTimer(const kodi::addon::PVRTimer& timer)
{
  PVR_ERROR err = DeleteTimer(timer, true);

  if (err == PVR_ERROR_NO_ERROR)
    return AddTimer(timer);
  return err;
}

PVR_ERROR CVBoxInstance::GetEPGForChannel(int channelUid, time_t start, time_t end, kodi::addon::PVREPGTagsResultSet& results)
{
  const ChannelPtr channelPtr = VBox::GetChannel(channelUid);

  if (!channelPtr)
    return PVR_ERROR_INVALID_PARAMETERS;

  if (m_skippingInitialEpgLoad)
  {
    kodi::Log(ADDON_LOG_DEBUG, "%s Skipping Initial EPG for channel: %s", __FUNCTION__, channelPtr->m_name.c_str());
    VBox::MarkChannelAsInitialEpgSkipped(channelUid);
    return PVR_ERROR_NO_ERROR;
  }

  // Retrieve the schedule
  const auto schedule = VBox::GetSchedule(channelPtr);

  if (!schedule.schedule)
    return PVR_ERROR_NO_ERROR;

  // Transfer the programmes between the start and end times
  for (const auto& programme : schedule.schedule->GetSegment(start, end))
  {
    kodi::addon::PVREPGTag event;

    event.SetStartTime(xmltv::Utilities::XmltvToUnixTime(programme->m_startTime));
    event.SetEndTime(xmltv::Utilities::XmltvToUnixTime(programme->m_endTime));
    event.SetUniqueChannelId(channelUid);
    event.SetUniqueBroadcastId(ContentIdentifier::GetUniqueId(programme.get()));
    event.SetTitle(programme->m_title);
    event.SetPlot(programme->m_description);
    event.SetYear(programme->m_year);
    event.SetEpisodeName(programme->m_subTitle);
    event.SetIconPath(programme->m_icon);
    event.SetSeriesNumber(EPG_TAG_INVALID_SERIES_EPISODE);
    event.SetEpisodeNumber(EPG_TAG_INVALID_SERIES_EPISODE);
    event.SetEpisodePartNumber(EPG_TAG_INVALID_SERIES_EPISODE);

    std::string directors = xmltv::Utilities::ConcatenateStringList(programme->GetDirectors());
    std::string writers = xmltv::Utilities::ConcatenateStringList(programme->GetWriters());
    std::vector<std::string> categories = programme->GetCategories();
    std::string catStrings = xmltv::Utilities::ConcatenateStringList(categories);

    event.SetDirector(directors);
    event.SetWriter(writers);

    // use genre mapper to find the most common genre type (from categories)
    event.SetGenreType(VBox::GetCategoriesGenreType(categories));
    event.SetGenreDescription(catStrings);

    // Extract up to five cast members only
    std::vector<std::string> actorNames;
    const auto& actors = programme->GetActors();
    int numActors = std::min(static_cast<int>(actors.size()), 5);

    for (unsigned int i = 0; i < numActors; i++)
      actorNames.push_back(actors.at(i).name);

    std::string cast = xmltv::Utilities::ConcatenateStringList(actorNames);
    event.SetCast(cast);

    unsigned int flags = EPG_TAG_FLAG_UNDEFINED;

    if (!programme->m_seriesIds.empty())
    {
      kodi::Log(ADDON_LOG_DEBUG, "GetEPGForChannel():programme %s marked as belonging to a series", programme->m_title.c_str());
      flags |= EPG_TAG_FLAG_IS_SERIES;
    }
    event.SetFlags(flags);

    results.Add(event);
  }

  return PVR_ERROR_NO_ERROR;
}

bool CVBoxInstance::OpenLiveStream(const kodi::addon::PVRChannel& channel)
{
  // Find the channel
  const ChannelPtr channelPtr = VBox::GetChannel(channel.GetUniqueId());

  if (!channelPtr)
    return false;

  // Remember the current channel if the buffer was successfully opened
  if (m_timeshiftBuffer->Open(channelPtr->m_url))
  {
    VBox::SetCurrentChannel(channelPtr);
    return true;
  }

  CloseLiveStream();
  VBox::SetChannelStreamingStatus(channelPtr);
  return false;
}

void CVBoxInstance::CloseLiveStream()
{
  m_timeshiftBuffer->Close();
  VBox::SetCurrentChannel(nullptr);
}

int CVBoxInstance::ReadLiveStream(unsigned char* pBuffer, unsigned int iBufferSize)
{
  return m_timeshiftBuffer->Read(pBuffer, iBufferSize);
}

int64_t CVBoxInstance::LengthLiveStream()
{
  return m_timeshiftBuffer->Length();
}

int64_t CVBoxInstance::SeekLiveStream(int64_t iPosition, int iWhence /* = SEEK_SET */)
{
  return m_timeshiftBuffer->Seek(iPosition, iWhence);
}

bool CVBoxInstance::CanPauseStream()
{
  return VBox::GetSettings().m_timeshiftEnabled;
}

bool CVBoxInstance::CanSeekStream()
{
  return VBox::GetSettings().m_timeshiftEnabled;
}

bool CVBoxInstance::IsRealTimeStream()
{
  const ChannelPtr currentChannel = VBox::GetCurrentChannel();

  return currentChannel != nullptr;
}

// Stream times
PVR_ERROR CVBoxInstance::GetStreamTimes(kodi::addon::PVRStreamTimes& times)
{
  // Addon API 5.8.0
  if (IsRealTimeStream() && m_timeshiftBuffer && VBox::GetSettings().m_timeshiftEnabled)
  {
    times.SetStartTime(m_timeshiftBuffer->GetStartTime());
    times.SetPTSStart(0);
    times.SetPTSBegin(0);
    times.SetPTSEnd((!m_timeshiftBuffer->CanSeekStream()) ? 0
        : (m_timeshiftBuffer->GetEndTime() - m_timeshiftBuffer->GetStartTime()) * STREAM_TIME_BASE);

    return PVR_ERROR_NO_ERROR;
  }
  else if (m_recordingReader)
  {
    times.SetStartTime(0);
    times.SetPTSStart(0);
    times.SetPTSBegin(0);
    times.SetPTSEnd(static_cast<int64_t>(m_recordingReader->CurrentDuration()) * STREAM_TIME_BASE);

    return PVR_ERROR_NO_ERROR;
  }

  return PVR_ERROR_NOT_IMPLEMENTED;
}

void PauseStream(bool bPaused)
{
  // TODO: Setting for timeshift on pause as well as always
}

// Recording stream methods
bool CVBoxInstance::OpenRecordedStream(const kodi::addon::PVRRecording & recording)
{
  CloseRecordedStream();

  unsigned int id = static_cast<unsigned int>(std::stoi(recording.GetRecordingId()));
  auto& recordings = VBox::GetRecordingsAndTimers();
  auto recIt = std::find_if(recordings.begin(), recordings.end(),
    [id](const RecordingPtr& item) {
      return item->IsRecording() && id == item->m_id;
    }
  );

  if (recIt == recordings.end())
    return PVR_ERROR_SERVER_ERROR;

  std::time_t now = std::time(nullptr), start = 0, end = 0;
  std::string channelName = recording.GetChannelName();
  time_t recordingTime = recording.GetRecordingTime();
  auto timerIt = std::find_if(recordings.begin(), recordings.end(),
    [now, channelName, recordingTime](const RecordingPtr& item) {
      return item->IsTimer() && item->IsRunning(now, channelName, recordingTime);
    }
  );
  if (timerIt != recordings.end())
  {
    auto& timer = *timerIt;
    start = xmltv::Utilities::XmltvToUnixTime(timer->m_startTime);
    end = xmltv::Utilities::XmltvToUnixTime(timer->m_endTime);
  }

  m_recordingReader = new RecordingReader((*recIt)->m_url.c_str(), start, end, recording.GetDuration());

  return m_recordingReader->Start();
}

void CVBoxInstance::CloseRecordedStream()
{
  delete m_recordingReader;
  m_recordingReader = nullptr;
}

int CVBoxInstance::ReadRecordedStream(unsigned char* buffer, unsigned int size)
{
  if (!m_recordingReader)
    return 0;

  return m_recordingReader->ReadData(buffer, size);
}

int64_t CVBoxInstance::SeekRecordedStream(int64_t position, int whence)
{
  if (!m_recordingReader)
    return 0;

  return m_recordingReader->Seek(position, whence);
}

int64_t CVBoxInstance::LengthRecordedStream()
{
  if (!m_recordingReader)
    return -1;

  return m_recordingReader->Length();
}
