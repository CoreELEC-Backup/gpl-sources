/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "VBox.h"

#include "../xmltv/Utilities.h"
#include "ContentIdentifier.h"
#include "Exceptions.h"
#include "Utilities.h"
#include "request/ApiRequest.h"
#include "request/FileRequest.h"
#include "request/Request.h"
#include "response/Content.h"
#include "response/Factory.h"

#include <algorithm>
#include <chrono>
#include <sstream>
#include <string>

#include <kodi/General.h>

using namespace vbox;

const char* VBox::MINIMUM_SOFTWARE_VERSION = "2.48";
const time_t STREAMING_STATUS_UPDATE_INTERVAL = 10;
const int CHANNELS_PER_CHANNELBATCH = 100;
const int CHANNELS_PER_EPGBATCH = 10;
const size_t VBOX_LOG_BUFFER = 16384;

VBox::VBox(const Settings& settings)
  : m_settings(settings),
    m_currentChannel(nullptr),
    m_categoryGenreMapper(nullptr),
    m_shouldSyncEpg(false),
    m_lastStreamStatus({ChannelStreamingStatus(), time(nullptr)})
{
}

VBox::~VBox()
{
  // Wait for the background thread to stop
  m_active = false;

  if (m_backgroundThread.joinable())
    m_backgroundThread.join();
}

void VBox::Initialize()
{
  // Determine which connection parameters should be used
  DetermineConnectionParams();

  // Query the software version, we need a few elements from that response
  request::ApiRequest versionRequest("QuerySwVersion", GetConnectionParams().hostname, GetConnectionParams().upnpPort);
  response::ResponsePtr response = PerformRequest(versionRequest);
  response::Content versionContent(response->GetReplyElement());

  // Query the board info, we need some elements from that as well
  request::ApiRequest boardRequest("QueryBoardInfo", GetConnectionParams().hostname, GetConnectionParams().upnpPort);
  response::ResponsePtr boardResponse = PerformRequest(boardRequest);
  response::Content boardInfo(boardResponse->GetReplyElement());

  // Construct the model string
  std::string model = versionContent.GetString("Custom"); // VBox
  model += " " + versionContent.GetString("DeviceType"); // e.g. XTI
  model += " " + boardInfo.GetString("ProductNumber"); // e.g. 3352

  kodi::Log(ADDON_LOG_INFO, "device information: ");
  kodi::Log(ADDON_LOG_INFO, std::string("                 model: " + model).c_str());
  kodi::Log(ADDON_LOG_INFO, std::string("     hardware revision: " + boardInfo.GetString("HWRev")).c_str());
  kodi::Log(ADDON_LOG_INFO, std::string("     firmware revision: " + boardInfo.GetString("FWRev")).c_str());
  kodi::Log(ADDON_LOG_INFO, std::string("         uboot version: " + boardInfo.GetString("UbootVersion")).c_str());
  kodi::Log(ADDON_LOG_INFO, std::string("        kernel version: " + boardInfo.GetString("KernelVersion")).c_str());
  kodi::Log(ADDON_LOG_INFO, std::string("      software version: " + boardInfo.GetString("SoftwareVersion")).c_str());
  kodi::Log(ADDON_LOG_INFO, std::string("      number of tuners: " + std::to_string(boardInfo.GetInteger("TunersNumber"))).c_str());

  // Construct backend information
  m_backendInformation.name = model;
  m_backendInformation.version = SoftwareVersion::ParseString(boardInfo.GetString("SoftwareVersion"));

  // Check that the backend uses a compatible software version
  if (m_backendInformation.version < SoftwareVersion::ParseString(MINIMUM_SOFTWARE_VERSION))
  {
    std::string error = std::string("Firmware version ") + MINIMUM_SOFTWARE_VERSION + " or higher is required";

    throw FirmwareVersionException(error);
  }

  // Query external media status. The request will error if no external media
  // is attached
  try
  {
    request::ApiRequest mediaRequest("QueryExternalMediaStatus", GetConnectionParams().hostname, GetConnectionParams().upnpPort);
    response::ResponsePtr mediaResponse = PerformRequest(mediaRequest);
    response::Content mediaStatus = response::Content(mediaResponse->GetReplyElement());

    ExternalMediaStatus externalMediaStatus;
    externalMediaStatus.present = true;
    externalMediaStatus.spaceTotal = static_cast<int64_t>(mediaStatus.GetInteger("TotalMem")) * 1048576;
    externalMediaStatus.spaceUsed = static_cast<int64_t>(mediaStatus.GetInteger("UsedMem")) * 1048576;

    m_backendInformation.externalMediaStatus = externalMediaStatus;
  }
  catch (VBoxException& e)
  {
    LogException(e);
  }

  // Query the timezone offset used
  request::ApiRequest timezoneRequest("QuerySystemTime", GetConnectionParams().hostname, GetConnectionParams().upnpPort);
  timezoneRequest.AddParameter("TimeFormat", "XMLTV");
  response::ResponsePtr timezoneResponse = PerformRequest(timezoneRequest);
  response::Content timezoneInfo(timezoneResponse->GetReplyElement());

  std::string timestamp = timezoneInfo.GetString("Time");
  m_backendInformation.timezoneOffset = ::xmltv::Utilities::GetTimezoneOffset(timestamp);

  // Consider the addon initialized
  m_stateHandler.EnterState(StartupState::INITIALIZED);

  m_skippingInitialEpgLoad = m_settings.m_skipInitialEpgLoad;

  RetrieveChannels(false);
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    for (auto channel : m_channels)
      m_unskippedInitialEpgChannelsMap.insert({channel->m_uniqueId, channel->m_uniqueId});
  }

  // Start the background updater thread
  m_active = true;
  m_backgroundThread = std::thread([this]()
  {
    BackgroundUpdater();
  });
}

void VBox::DetermineConnectionParams()
{
  // Attempt to perform a request using the internal connection parameters
  m_currentConnectionParameters = m_settings.m_internalConnectionParams;

  try
  {
    request::ApiRequest request("QuerySwVersion", GetConnectionParams().hostname, GetConnectionParams().upnpPort);
    request.SetTimeout(m_currentConnectionParameters.timeout);
    PerformRequest(request);
  }
  catch (VBoxException&)
  {
    // Retry the request with the external parameters
    if (m_settings.m_externalConnectionParams.AreValid())
    {
      kodi::Log(ADDON_LOG_INFO, "Unable to connect using internal connection settings, trying with external");
      m_currentConnectionParameters = m_settings.m_externalConnectionParams;

      request::ApiRequest request("QuerySwVersion", GetConnectionParams().hostname, GetConnectionParams().upnpPort);
      request.SetTimeout(m_currentConnectionParameters.timeout);
      PerformRequest(request);
    }
  }

  auto& params = m_currentConnectionParameters;
  kodi::Log(ADDON_LOG_INFO, "Connection parameters used: ");
  kodi::Log(ADDON_LOG_INFO, "    Hostname: %s", params.hostname.c_str());

  if (params.UseHttps())
    kodi::Log(ADDON_LOG_INFO, "    HTTPS port: %d", params.httpsPort);
  else
    kodi::Log(ADDON_LOG_INFO, "    HTTP port: %d", params.httpPort);

  kodi::Log(ADDON_LOG_INFO, "    UPnP port: %d", params.upnpPort);
}

void VBox::InitScanningEPG(std::string& rScanMethod, std::string& rGetStatusMethod, std::string& rfIsScanningFlag)
{
  // determine wether the device is in External XMLTV mode (internal, not through Kodi definitions)
  SendScanEPG(rScanMethod);
  GetEpgDetectionState(rGetStatusMethod, rfIsScanningFlag);
  // if state is not collecting EPG - we're not in External XMLTV  so
  // try the Live Signal mode syncing methods. If not - keep External XMLTV methods
  if (m_epgScanState != EPGSCAN_SCANNING)
  {
    // send EPG scanning request (live signal)
    rScanMethod = "ScanEPG";
    SendScanEPG(rScanMethod);
    rGetStatusMethod = "QueryEpgDetectionStatus";
    rfIsScanningFlag = "IsInDetection";
  }
  // set EPG scan state
  m_epgScanState = EPGSCAN_SCANNING;
}

void VBox::UpdateEpgScan(bool fRetrieveGuide)
{
  static std::string scanEpgMethod("SyncExternalXMLTVChannels");
  static std::string epgStatusCheckMethod("QueryExternalXMLTVSyncStatus");
  static std::string epgStatusCheckFlag("SyncInProgress");

  switch (m_epgScanState)
  {
    case EPGSCAN_SHOULD_SCAN:
      // find the correct methods for the guide's External XMLTV / Live Signal mode
      InitScanningEPG(scanEpgMethod, epgStatusCheckMethod, epgStatusCheckFlag);
    case EPGSCAN_SCANNING:
    case EPGSCAN_FINISHED:
      if (fRetrieveGuide)
      {
        // check for EPG detection state
        GetEpgDetectionState(epgStatusCheckMethod, epgStatusCheckFlag);
        // retrieve guide periodically (or when finished)
        RetrieveGuide();
        // if done detecting EPG - change flag to false
        if (m_epgScanState == EPGSCAN_FINISHED)
        {
          kodi::QueueNotification(QUEUE_INFO, "", "EPG scanned and synced with guide");
          m_epgScanState = EPGSCAN_NO_SCAN;
        }
      }
    case EPGSCAN_NO_SCAN:
      break;
  }
}

void VBox::BackgroundUpdater()
{
  // Keep count of how many times the loop has run so we can perform some
  // tasks only on some iterations
  static unsigned int lapCounter = 1;

  // Retrieve everything in order once before starting the loop, without
  // triggering the event handlers
  RetrieveChannels(false);

  InitializeGenreMapper();
  RetrieveRecordings(false);
  RetrieveGuide(false);

  // Wait for the initial EPG update to complete
  int totalWaitSecs = 0;
  while (m_active && totalWaitSecs < INITIAL_EPG_WAIT_SECS)
  {
    totalWaitSecs += INITIAL_EPG_STEP_SECS;

    if (!IsInitialEpgSkippingCompleted())
      std::this_thread::sleep_for(std::chrono::milliseconds(INITIAL_EPG_STEP_SECS * 1000));
  }

  m_skippingInitialEpgLoad = false;

  // Whether or not initial EPG updates occurred now Trigger "Real" EPG updates
  // This will regard Initial EPG as completed anyway.
  TriggerEpgUpdatesForChannels();

  while (m_active)
  {
    // Update recordings every 12 iterations = 1 minute
    if (lapCounter % 12 == 0)
      RetrieveRecordings();

    // Update channels every six iterations = 30 seocnds
    if (lapCounter % 6 == 0)
      RetrieveChannels();

    // if supposed to scan EPG - send scan API and get guide every 5 minutes, until done scanning
    if (m_epgScanState != EPGSCAN_NO_SCAN)
      UpdateEpgScan(lapCounter % (12 * 5) == 0);
    // one-time guide retrieval (from PVR manager settings)
    else if (m_shouldSyncEpg)
    {
      RetrieveGuide();
      m_shouldSyncEpg = false;
    }
    // if not collecting/syncing user's EPG - update the internal guide data every 12 * 60 iterations = 1 hour
    else if (lapCounter % (12 * 60) == 0)
      RetrieveGuide();

    lapCounter++;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  }
}

bool VBox::IsInitialEpgSkippingCompleted()
{
  kodi::Log(ADDON_LOG_DEBUG, "%s Waiting to Get Initial EPG for %d remaining channels", __FUNCTION__,
      m_unskippedInitialEpgChannelsMap.size());

  return m_unskippedInitialEpgChannelsMap.size() == 0;
}

void VBox::TriggerEpgUpdatesForChannels()
{
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    for (auto& channel : m_channels)
    {
      //We want to trigger full updates only so let's make sure it's not an initialEpg
      m_unskippedInitialEpgChannelsMap.erase(channel->m_uniqueId);

      kodi::Log(ADDON_LOG_DEBUG, "%s - Trigger EPG update for channel: %s (%s)", __FUNCTION__, channel->m_name.c_str(),
          channel->m_uniqueId.c_str());
    }
  }

  OnGuideUpdated();
}

void VBox::MarkChannelAsInitialEpgSkipped(unsigned int channelUid)
{
  const ChannelPtr channelPtr = GetChannel(channelUid);

  m_unskippedInitialEpgChannelsMap.erase(channelPtr->m_uniqueId);
}

bool VBox::ValidateSettings() const
{
  // Check connection settings
  if (!m_settings.m_internalConnectionParams.AreValid())
    return false;

  // Check timeshift settings
  std::vector<kodi::vfs::CDirEntry> items;
  if (m_settings.m_timeshiftEnabled && !kodi::vfs::GetDirectory(m_settings.m_timeshiftBufferPath, "", items))
    return false;

  return true;
}

const Settings& VBox::GetSettings() const
{
  return m_settings;
}

const ConnectionParameters& VBox::GetConnectionParams() const
{
  return m_currentConnectionParameters;
}

StartupStateHandler& VBox::GetStateHandler()
{
  return m_stateHandler;
}

std::string VBox::GetBackendName() const
{
  if (m_stateHandler.WaitForState(StartupState::INITIALIZED))
    return m_backendInformation.name;

  return "";
}

std::string VBox::GetBackendHostname() const
{
  return m_currentConnectionParameters.hostname;
}

std::string VBox::GetBackendVersion() const
{
  if (m_stateHandler.WaitForState(StartupState::INITIALIZED))
    return m_backendInformation.version.GetString();

  return "";
}

std::string VBox::GetConnectionString() const
{
  std::stringstream ss;
  ss << GetBackendHostname() << ":" << m_currentConnectionParameters.httpPort;

  return ss.str();
}

int VBox::GetChannelsAmount() const
{
  m_stateHandler.WaitForState(StartupState::CHANNELS_LOADED);
  std::unique_lock<std::mutex> lock(m_mutex);

  return m_channels.size();
}

const std::vector<ChannelPtr>& VBox::GetChannels() const
{
  m_stateHandler.WaitForState(StartupState::CHANNELS_LOADED);
  std::unique_lock<std::mutex> lock(m_mutex);

  return m_channels;
}

const ChannelPtr VBox::GetChannel(unsigned int uniqueId) const
{
  m_stateHandler.WaitForState(StartupState::CHANNELS_LOADED);
  std::unique_lock<std::mutex> lock(m_mutex);

  auto it = std::find_if(m_channels.cbegin(), m_channels.cend(),
    [uniqueId](const ChannelPtr& channel) {
      return uniqueId == ContentIdentifier::GetUniqueId(channel);
    }
  );

  if (it == m_channels.cend())
    return nullptr;

  return *it;
}

const ChannelPtr VBox::GetCurrentChannel() const
{
  return m_currentChannel;
}

void VBox::SetCurrentChannel(const ChannelPtr& channel)
{
  m_currentChannel = channel;
}

void VBox::StartEPGScan()
{
  m_epgScanState = EPGSCAN_SHOULD_SCAN;
}

void VBox::SyncEPGNow()
{
  m_shouldSyncEpg = true;
}

void VBox::SendScanEPG(std::string& rEpgDetectionCheckMethod) const
{
  // call method to send EPG detection command
  request::ApiRequest request(rEpgDetectionCheckMethod, GetConnectionParams().hostname, GetConnectionParams().upnpPort);
  request.AddParameter("ChannelID", "All");
  response::ResponsePtr response = PerformRequest(request);
  response::Content content(response->GetReplyElement());
}

void VBox::GetEpgDetectionState(std::string& methodName, std::string& flagName)
{
  // call method to check EPG detection status
  request::ApiRequest request(methodName, GetConnectionParams().hostname, GetConnectionParams().upnpPort);
  response::ResponsePtr response = PerformRequest(request);
  response::Content content(response->GetReplyElement());

  // set flag using a YES/NO flag
  std::string isInlDetection = content.GetString(flagName);
  m_epgScanState = (isInlDetection == "YES") ? EPGSCAN_SCANNING : EPGSCAN_FINISHED;
}

void VBox::SetChannelStreamingStatus(const ChannelPtr& channel)
{
  ChannelStreamingStatus status;

  request::ApiRequest request("QueryChannelStreamingStatus", GetConnectionParams().hostname, GetConnectionParams().upnpPort);
  request.AddParameter("ChannelID", channel->m_xmltvName);
  response::ResponsePtr response = PerformRequest(request);
  response::Content content(response->GetReplyElement());

  // Only attempt to parse the status if streaming is active
  std::string active = content.GetString("Active");

  if (active == "YES")
  {
    status.m_active = true;
    status.SetServiceId(content.GetUnsignedInteger("SID"));
    status.SetTunerId(content.GetString("TunerID"));
    status.SetTunerType(content.GetString("TunerType"));
    status.m_lockStatus = content.GetString("LockStatus");
    status.m_lockedMode = content.GetString("LockedMode");
    status.m_modulation = content.GetString("Modulation");
    status.m_frequency = content.GetString("Frequency");
    status.SetRfLevel(content.GetString("RFLevel"));
    status.m_signalQuality = content.GetUnsignedInteger("SignalQuality");
    status.SetBer(content.GetString("BER"));
  }

  m_lastStreamStatus.m_streamStatus = status;
  m_lastStreamStatus.m_timestamp = time(nullptr);
}

ChannelStreamingStatus VBox::GetChannelStreamingStatus(const ChannelPtr& channel)
{
  time_t lastUpdateTime = m_lastStreamStatus.m_timestamp;
  time_t currTime(time(nullptr));

  if (currTime - lastUpdateTime >= STREAMING_STATUS_UPDATE_INTERVAL)
    SetChannelStreamingStatus(channel);

  return m_lastStreamStatus.m_streamStatus;
}

bool VBox::SupportsRecordings() const
{
  return m_backendInformation.externalMediaStatus.present;
}

int64_t VBox::GetRecordingTotalSpace() const
{
  return m_backendInformation.externalMediaStatus.spaceTotal;
}

int64_t VBox::GetRecordingUsedSpace() const
{
  return m_backendInformation.externalMediaStatus.spaceUsed;
}

int VBox::GetRecordingsAmount() const
{
  m_stateHandler.WaitForState(StartupState::RECORDINGS_LOADED);
  std::unique_lock<std::mutex> lock(m_mutex);

  return std::count_if(m_recordings.begin(), m_recordings.end(),
                       [](const RecordingPtr& recording) { return recording->IsRecording(); });
}

request::ApiRequest VBox::CreateDeleteRecordingRequest(const RecordingPtr& recording) const
{
  RecordingState state = recording->GetState();
  unsigned int idToDelete = (recording->m_seriesId > 0) ? recording->m_seriesId : recording->m_id;

  // Determine the request method to use. If a recording is active we want to
  // cancel it instead of deleting it
  std::string requestMethod = "DeleteRecord";

  if (state == RecordingState::RECORDING)
    requestMethod = "CancelRecord";

  // Create the request
  request::ApiRequest request(requestMethod, GetConnectionParams().hostname, GetConnectionParams().upnpPort);

  // Determine request parameters
  request.AddParameter("RecordID", idToDelete);
  if (state == RecordingState::EXTERNAL)
    request.AddParameter("FileName", recording->m_filename);

  return request;
}

request::ApiRequest VBox::CreateDeleteSeriesRequest(const SeriesRecordingPtr& series) const
{
  kodi::Log(ADDON_LOG_DEBUG, "Removing series with ID %d", series->m_id);
  // For a series, CancelRecord cancels next episodes, and if there's a current
  // episode being recorded, it is stopped
  std::string requestMethod = "CancelRecord";

  // Create the request
  request::ApiRequest request(requestMethod, GetConnectionParams().hostname, GetConnectionParams().upnpPort);

  // Determine request parameters
  request.AddParameter("RecordID", series->m_id);

  return request;
}

bool VBox::DeleteRecordingOrTimer(unsigned int id)
{
  m_stateHandler.WaitForState(StartupState::RECORDINGS_LOADED);
  std::unique_lock<std::mutex> lock(m_mutex);

  // The request fails if the item doesn't exist
  try
  {
    // Find the recording/timer - look for a single recording
    auto it = std::find_if(m_recordings.begin(), m_recordings.end(), [id](const RecordingPtr& recording) { return id == recording->m_id; });

    // if it matches a single recording - create and send delete request for recording
    if (it != m_recordings.cend())
    {
      request::ApiRequest request = CreateDeleteRecordingRequest(*it);
      PerformRequest(request);
      // remove recording object from memory
      m_recordings.erase(it);
    }
    // if id doesn't match a recording, it's a series
    else
    {
      // look for a series with that ID and throw exception if not found
      auto seriesItr = std::find_if(m_series.begin(), m_series.end(), [id](const SeriesRecordingPtr& series) { return id == series->m_id; });
      if (seriesItr != m_series.end())
      {
        // create and send cancel request for that series recording
        request::ApiRequest request = CreateDeleteSeriesRequest(*seriesItr);
        PerformRequest(request);
        // remove series object from memory
        m_series.erase(seriesItr);
      }
      else
      {
        throw vbox::RequestFailedException("Could not find timer's ID in backend");
      }
    }

    // Fire events
    OnRecordingsUpdated();
    OnTimersUpdated();

    return true;
  }
  catch (VBoxException& e)
  {
    LogException(e);
  }

  return false;
}

const RecordingMargins VBox::GetRecordingMargins(bool fBackendSingleMargin) const
{
  RecordingMargins margins = {0};

  // get recording margins
  request::ApiRequest request("GetRecordingsTimeOffset", GetConnectionParams().hostname, GetConnectionParams().upnpPort);
  response::ResponsePtr response = PerformRequest(request);
  response::Content content(response->GetReplyElement());

  // If version < 2.57, there is one margin (for both before & after the program).
  // In that case, get the margin for both before/after
  if (fBackendSingleMargin)
  {
    margins.m_beforeMargin = content.GetUnsignedInteger("RecordingsTimeOffset");
    margins.m_afterMargin = content.GetUnsignedInteger("RecordingsTimeOffset");
  }
  // Otherwise - get the matching margins
  else
  {
    margins.m_beforeMargin = content.GetUnsignedInteger("MinutesPaddingBefore");
    margins.m_afterMargin = content.GetUnsignedInteger("MinutesPaddingAfter");
  }
  kodi::Log(ADDON_LOG_DEBUG, "GetRecordingMargins(): Current recording margins: %u and %u", margins.m_beforeMargin,
      margins.m_afterMargin);
  return margins;
}

void VBox::SetRecordingMargins(RecordingMargins margins, bool fBackendSingleMargin)
{
  request::ApiRequest request("SetRecordingsTimeOffset", GetConnectionParams().hostname, GetConnectionParams().upnpPort);

  // If version < 2.57, there is a single margin (for both before & after the program).
  // In that case, set either margin (they're the same)
  if (fBackendSingleMargin)
  {
    request.AddParameter("RecordingsTimeOffset", margins.m_beforeMargin);
  }
  // Otherwise, set the matching margins
  else
  {
    request.AddParameter("MinutesPaddingBefore", margins.m_beforeMargin);
    request.AddParameter("MinutesPaddingAfter", margins.m_afterMargin);
  }
  PerformRequest(request);
}

void VBox::UpdateRecordingMargins(RecordingMargins defaultMargins)
{
  // get  version from backend
  SoftwareVersion version(SoftwareVersion::ParseString(m_backendInformation.version.GetString()));
  RecordingMargins updatedMargins;
  bool fBackendSingleMargin = true;

  // if version < 2.57 (support only for 1 margin), set updated margin for both
  // before + after the program times as the larger margin of the timer margins (before / after)
  if (version < SoftwareVersion::ParseString("2.57"))
  {
    unsigned int maxMargin = std::max<unsigned int>(defaultMargins.m_beforeMargin, defaultMargins.m_afterMargin);
    updatedMargins.m_beforeMargin = maxMargin;
    updatedMargins.m_afterMargin = maxMargin;
  }
  // otherwise - set each margin as the matching default margin
  else
  {
    updatedMargins.m_beforeMargin = defaultMargins.m_beforeMargin;
    updatedMargins.m_afterMargin = defaultMargins.m_afterMargin;
    fBackendSingleMargin = false;
  }
  const RecordingMargins currMargins = GetRecordingMargins(fBackendSingleMargin);

  // set the updated margins in backend, if different from the current ones
  if (updatedMargins != currMargins)
    SetRecordingMargins(updatedMargins, fBackendSingleMargin);
}

void VBox::AddTimer(const ChannelPtr& channel, const ::xmltv::ProgrammePtr programme)
{
  // Add the timer
  request::ApiRequest request("ScheduleProgramRecord", GetConnectionParams().hostname, GetConnectionParams().upnpPort);
  request.AddParameter("ChannelID", channel->m_xmltvName);
  request.AddParameter("ProgramTitle", programme->m_title);
  request.AddParameter("StartTime", programme->m_startTime);
  PerformRequest(request);

  // Refresh the recordings and timers
  RetrieveRecordings();
}


static void AddWeekdays(request::ApiRequest& rRequest, const unsigned int weekdays)
{
  if (weekdays & PVR_WEEKDAY_SUNDAY)
  {
    rRequest.AddParameter("Day", "Sun");
  }
  if (weekdays & PVR_WEEKDAY_MONDAY)
  {
    rRequest.AddParameter("Day", "Mon");
  }
  if (weekdays & PVR_WEEKDAY_TUESDAY)
  {
    rRequest.AddParameter("Day", "Tue");
  }
  if (weekdays & PVR_WEEKDAY_WEDNESDAY)
  {
    rRequest.AddParameter("Day", "Wed");
  }
  if (weekdays & PVR_WEEKDAY_THURSDAY)
  {
    rRequest.AddParameter("Day", "Thu");
  }
  if (weekdays & PVR_WEEKDAY_FRIDAY)
  {
    rRequest.AddParameter("Day", "Fri");
  }
  if (weekdays & PVR_WEEKDAY_SATURDAY)
  {
    rRequest.AddParameter("Day", "Sat");
  }
}

void VBox::AddSeriesTimer(const ChannelPtr& channel, const ::xmltv::ProgrammePtr programme)
{
  kodi::Log(ADDON_LOG_DEBUG, "Series timer for channel %s, program %s", channel->m_name.c_str(), programme->m_title.c_str());

  // Add the timer
  request::ApiRequest request("ScheduleProgramRecord", GetConnectionParams().hostname, GetConnectionParams().upnpPort);
  request.AddParameter("ChannelID", channel->m_xmltvName);
  request.AddParameter("ProgramTitle", programme->m_title);
  request.AddParameter("StartTime", programme->m_startTime);
  request.AddParameter("SeriesRecording", "YES");
  PerformRequest(request);

  // Refresh the recordings and timers
  RetrieveRecordings();
}

void VBox::AddTimer(const ChannelPtr& channel, time_t startTime, time_t endTime,
                    const std::string title, const std::string description)
{
  kodi::Log(ADDON_LOG_DEBUG, "Adding Manual timer for channel %s", channel->m_name.c_str());
  // Add the timer
  request::ApiRequest request("ScheduleChannelRecord", GetConnectionParams().hostname, GetConnectionParams().upnpPort);
  request.AddParameter("ChannelID", channel->m_xmltvName);
  request.AddParameter("StartTime", CreateTimestamp(startTime));
  request.AddParameter("EndTime", CreateTimestamp(endTime));

  // Manually set title
  request.AddParameter("ProgramName", title);

  PerformRequest(request);

  // Refresh the recordings and timers
  RetrieveRecordings();
}

// implement timer with rule for manually defined series
void VBox::AddTimer(const ChannelPtr& channel,
                    time_t startTime,
                    time_t endTime,
                    const std::string title,
                    const std::string description,
                    const unsigned int weekdays)
{
  kodi::Log(ADDON_LOG_DEBUG, "Manual series timer for channel %s, weekdays = 0x%x", channel->m_name.c_str(), weekdays);
  // Add the timer
  request::ApiRequest request("ScheduleChannelRecord", GetConnectionParams().hostname, GetConnectionParams().upnpPort);
  request.AddParameter("ChannelID", channel->m_xmltvName);
  request.AddParameter("Periodic", "YES");
  request.AddParameter("FromTime", CreateDailyTime(startTime));
  request.AddParameter("ToTime", CreateDailyTime(endTime));

  // Manually set title
  request.AddParameter("ProgramName", title);

  AddWeekdays(request, weekdays);
  PerformRequest(request);

  // Refresh the recordings and timers
  RetrieveRecordings();
}

int VBox::GetTimersAmount() const
{
  m_stateHandler.WaitForState(StartupState::RECORDINGS_LOADED);
  std::unique_lock<std::mutex> lock(m_mutex);

  int count = std::count_if(m_recordings.begin(), m_recordings.end(),
                            [](const RecordingPtr& recording) { return recording->IsTimer(); });
  count += m_series.size();
  return count;
}

const std::vector<RecordingPtr>& VBox::GetRecordingsAndTimers() const
{
  m_stateHandler.WaitForState(StartupState::RECORDINGS_LOADED);
  std::unique_lock<std::mutex> lock(m_mutex);

  return m_recordings;
}


const std::vector<SeriesRecordingPtr>& VBox::GetSeriesTimers() const
{
  m_stateHandler.WaitForState(StartupState::RECORDINGS_LOADED);
  std::unique_lock<std::mutex> lock(m_mutex);

  return m_series;
}

const Schedule VBox::GetSchedule(const ChannelPtr& channel) const
{
  // Load the schedule from the internal guide
  m_stateHandler.WaitForState(StartupState::GUIDE_LOADED);
  std::unique_lock<std::mutex> lock(m_mutex);

  Schedule schedule;
  schedule.schedule = m_guide.GetSchedule(channel->m_xmltvName);

  return schedule;
}

std::string VBox::GetApiBaseUrl() const
{
  std::stringstream ss;
  ss << m_currentConnectionParameters.GetUriScheme() << "://";
  ss << m_currentConnectionParameters.GetUriAuthority();
  ss << "/cgi-bin/HttpControl/HttpControlApp?OPTION=1";

  return ss.str();
}

std::string VBox::CreateTimestamp(const time_t unixTimestamp) const
{
  std::string tzOffset = m_backendInformation.timezoneOffset;

  return ::xmltv::Utilities::UnixTimeToXmltv(unixTimestamp, tzOffset);
}

std::string VBox::CreateDailyTime(const time_t unixTimestamp) const
{
  std::string tzOffset = m_backendInformation.timezoneOffset;

  return ::xmltv::Utilities::UnixTimeToDailyTime(unixTimestamp, tzOffset);
}

unsigned int VBox::GetDBVersion(std::string& versionName) const
{
  // get the backend's database version
  request::ApiRequest request("QueryDataBaseVersion", GetConnectionParams().hostname, GetConnectionParams().upnpPort);
  response::ResponsePtr response = PerformRequest(request);
  response::Content content(response->GetReplyElement());
  return content.GetUnsignedInteger(versionName);
}

void VBox::RetrieveChannels(bool triggerEvent /* = true*/)
{
  try
  {
    std::string channelsDBVerName("ChannelsDataBaseVersion");
    unsigned int newDBversion = GetDBVersion(channelsDBVerName);
    // if same as last fetched channels, no need for fetching again
    if (newDBversion == m_channelsDBVersion)
      return;

    int lastChannelIndex;

    {
      request::ApiRequest request("QueryXmltvNumOfChannels", GetConnectionParams().hostname, GetConnectionParams().upnpPort);
      response::ResponsePtr response = PerformRequest(request);
      response::Content content(response->GetReplyElement());

      // get number of channels from backend
      std::unique_lock<std::mutex> lock(m_mutex);
      lastChannelIndex = content.GetUnsignedInteger("NumOfChannels");
    }

    std::vector<ChannelPtr> allChannels;

    // Get channels in batches of 100
    for (int fromIndex = 1; fromIndex <= lastChannelIndex; fromIndex += CHANNELS_PER_CHANNELBATCH)
    {
      // Abort immediately if the addon just got terminated
      if (!m_active)
        return;

      int toIndex = std::min(fromIndex + (CHANNELS_PER_CHANNELBATCH - 1), lastChannelIndex);
      // Swallow exceptions, we don't want channel loading to fail just because
      // one request failed
      try
      {
        request::ApiRequest request("GetXmltvChannelsList", GetConnectionParams().hostname, GetConnectionParams().upnpPort);
        request.AddParameter("FromChIndex", fromIndex);
        request.AddParameter("ToChIndex", toIndex);
        response::ResponsePtr response = PerformRequest(request);
        response::XMLTVResponseContent content(response->GetReplyElement());
        auto channels = content.GetChannels();

        // Add the batch to all channels
        allChannels.insert(allChannels.end(), channels.begin(), channels.end());
      }
      catch (VBoxException& e)
      {
        LogException(e);
      }
    }

    // Swap and notify if the contents have changed
    if (!utilities::deref_equals(m_channels, allChannels))
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_channels = allChannels;
      kodi::Log(ADDON_LOG_INFO, "Channels database version updated to %u", newDBversion);
      m_channelsDBVersion = newDBversion;

      if (triggerEvent)
        OnChannelsUpdated();
    }
  }
  catch (VBoxException& e)
  {
    LogException(e);
    return;
  }

  if (m_stateHandler.GetState() < StartupState::CHANNELS_LOADED)
    m_stateHandler.EnterState(StartupState::CHANNELS_LOADED);
}

void VBox::RetrieveRecordings(bool triggerEvent /* = true*/)
{
  // Only attempt to retrieve recordings when external media is present
  if (m_backendInformation.externalMediaStatus.present)
  {
    try
    {
      request::ApiRequest request("GetRecordsList", GetConnectionParams().hostname, GetConnectionParams().upnpPort);
      request.AddParameter("Externals", "YES");
      response::ResponsePtr response = PerformRequest(request);
      response::RecordingResponseContent content(response->GetReplyElement());

      // Compare the results
      auto recordings = content.GetRecordings();
      auto series = content.GetSeriesRecordings();
      std::unique_lock<std::mutex> lock(m_mutex);

      // Swap and notify if the contents have changed
      if (!utilities::deref_equals(m_recordings, recordings) || !utilities::deref_equals(m_series, series))
      {
        m_recordings = std::move(content.GetRecordings());
        m_series = std::move(content.GetSeriesRecordings());
        if (triggerEvent)
        {
          OnRecordingsUpdated();
          OnTimersUpdated();
        }
      }
    }
    catch (VBoxException& e)
    {
      // Intentionally don't return, the request fails if there are no
      // recordings (which is technically not an error)
      LogException(e);
    }
  }

  if (m_stateHandler.GetState() < StartupState::RECORDINGS_LOADED)
    m_stateHandler.EnterState(StartupState::RECORDINGS_LOADED);
}

void VBox::RetrieveGuide(bool triggerEvent /* = true*/)
{
  kodi::Log(ADDON_LOG_INFO, "Fetching guide data from backend (this will take a while)");

  try
  {
    std::string progsDBVerName("ProgramsDataBaseVersion");
    unsigned int newDBversion = GetDBVersion(progsDBVerName);

    // if same as last fetched guide, no need for fetching again (unless syncing EPG)
    if (!m_shouldSyncEpg && newDBversion == m_programsDBVersion)
      return;

    // Retrieving the whole XMLTV file is too slow so we fetch sections in
    // batches of 10 channels and merge the results
    int lastChannelIndex;

    {
      std::unique_lock<std::mutex> lock(m_mutex);
      lastChannelIndex = m_channels.size();
    }

    xmltv::Guide guide;

    for (int fromIndex = 1; fromIndex <= lastChannelIndex; fromIndex += CHANNELS_PER_EPGBATCH)
    {
      // Abort immediately if the addon just got terminated
      if (!m_active)
        return;

      int toIndex = std::min(fromIndex + (CHANNELS_PER_EPGBATCH - 1), lastChannelIndex);

      // Swallow exceptions, we don't want guide loading to fail just because
      // one request failed
      try
      {
        request::ApiRequest request("GetXmltvSection", GetConnectionParams().hostname, GetConnectionParams().upnpPort);
        request.AddParameter("FromChIndex", fromIndex);
        request.AddParameter("ToChIndex", toIndex);
        response::ResponsePtr response = PerformRequest(request);
        response::XMLTVResponseContent content(response->GetReplyElement());

        auto partialGuide = content.GetGuide();
        guide += partialGuide;
      }
      catch (VBoxException& e)
      {
        LogException(e);
      }
    }

    LogGuideStatistics(guide);

    // Swap the guide with the new one
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_guide = guide;
      kodi::Log(ADDON_LOG_INFO, "Guide database version updated to %u", newDBversion);
      m_programsDBVersion = newDBversion;
    }

    if (triggerEvent)
      OnGuideUpdated();
  }
  catch (VBoxException& e)
  {
    LogException(e);
    return;
  }

  if (m_stateHandler.GetState() < StartupState::GUIDE_LOADED)
    m_stateHandler.EnterState(StartupState::GUIDE_LOADED);
}

void VBox::InitializeGenreMapper()
{
  // Abort if we're already initialized or the external guide is not loaded
  if (m_categoryGenreMapper)
    return;

  kodi::Log(ADDON_LOG_INFO, "Loading category genre mapper");

  m_categoryGenreMapper = CategoryMapperPtr(new CategoryGenreMapper());

  try
  {
    m_categoryGenreMapper->Initialize(CATEGORY_TO_GENRE_XML_PATH);
  }
  catch (VBoxException& e)
  {
    LogException(e);
    kodi::Log(ADDON_LOG_INFO, "Failed to load the genre mapper");
  }
}

int VBox::GetCategoriesGenreType(std::vector<std::string>& categories) const
{
  return m_categoryGenreMapper->GetCategoriesGenreType(categories);
}

void VBox::SwapChannelIcons(std::vector<ChannelPtr>& channels)
{
  for (auto& channel : channels)
  {
    // Consult the channel mapper to find the corresponding external channel
    std::string displayName = m_guideChannelMapper->GetExternalChannelName(channel->m_name);
    std::string channelId = m_externalGuide.GetChannelId(displayName);
    const auto schedulePtr = m_externalGuide.GetSchedule(channelId);

    if (schedulePtr)
    {
      const auto xmltvChannel = schedulePtr->GetChannel();

      // Don't store bogus icons
      if (!xmltvChannel->m_icon.empty())
        channel->m_iconUrl = xmltvChannel->m_icon;
    }
  }
}

void VBox::LogGuideStatistics(const xmltv::Guide& guide) const
{
  for (const auto& schedule : guide.GetSchedules())
  {
    kodi::Log(ADDON_LOG_INFO, "Fetched %d events for channel %s", schedule.second->GetLength(), schedule.first.c_str());
  }
}

response::ResponsePtr VBox::PerformRequest(const request::Request& request) const
{
  // Attempt to open a HTTP file handle
  kodi::vfs::CFile fileHandle;

  if (fileHandle.OpenFile(request.GetLocation(GetApiBaseUrl()), ADDON_READ_NO_CACHE))
  {
    // Read the response string
    std::unique_ptr<std::string> responseContent = utilities::ReadFileContents(fileHandle);
    fileHandle.Close();

    // Parse the response
    response::ResponsePtr response = response::Factory::CreateResponse(request);
    response->ParseRawResponse(*responseContent.get());

    // Check if the response was successful
    if (!response->IsSuccessful())
    {
      std::stringstream ss;
      ss << response->GetErrorDescription();
      ss << " (error code: " << static_cast<int>(response->GetErrorCode()) << ")";

      throw InvalidResponseException(ss.str());
    }

    return response;
  }

  // The request failed completely
  throw RequestFailedException("Unable to perform request (" + request.GetIdentifier() + ")");
}

void VBox::LogException(VBoxException& e)
{
  std::string message = "Request failed: " + std::string(e.what());
  kodi::Log(ADDON_LOG_ERROR, message.c_str());
}
