/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "../xmltv/Guide.h"
#include "../xmltv/Programme.h"
#include "../xmltv/Schedule.h"
#include "CategoryGenreMapper.h"
#include "Channel.h"
#include "ChannelStreamingStatus.h"
#include "Exceptions.h"
#include "GuideChannelMapper.h"
#include "Recording.h"
#include "SeriesRecording.h"
#include "Settings.h"
#include "SoftwareVersion.h"
#include "StartupStateHandler.h"
#include "request/ApiRequest.h"
#include "request/Request.h"
#include "response/Response.h"

#include <atomic>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <kodi/addon-instance/PVR.h>

namespace vbox
{

  /**
   * Represents the status of any external media attached to the gateway
   */
  struct ExternalMediaStatus
  {
    bool present = false;
    int64_t spaceTotal = 0;
    int64_t spaceUsed = 0;
  };

  /**
   * Represents various pieces of information about the connected backend
   */
  struct BackendInformation
  {
    std::string name = "";
    std::string timezoneOffset = "";
    SoftwareVersion version;
    ExternalMediaStatus externalMediaStatus;
  };

  /**
   * Represents a schedule. It contains an actual schedule and an indicator
   * which tells if the schedule is from the internal or external guide
   */
  struct Schedule
  {
    enum Origin
    {
      INTERNAL_GUIDE,
      EXTERNAL_GUIDE
    };

    ::xmltv::SchedulePtr schedule = nullptr;
    Origin origin = Origin::INTERNAL_GUIDE;
  };

  enum TimerTypes
  {
    TIMER_VBOX_TYPE_NONE = 0,
    TIMER_VBOX_TYPE_EPG_BASED_SINGLE,
    TIMER_VBOX_TYPE_EPISODE,
    TIMER_VBOX_TYPE_MANUAL_SINGLE,
    TIMER_VBOX_TYPE_EPG_BASED_AUTO_SERIES,
    TIMER_VBOX_TYPE_EPG_BASED_MANUAL_SERIES,
    TIMER_VBOX_TYPE_MANUAL_SERIES
  };

  enum EPGScanState
  {
    EPGSCAN_NO_SCAN = 0,
    EPGSCAN_SHOULD_SCAN,
    EPGSCAN_SCANNING,
    EPGSCAN_FINISHED
  };

  struct TimedStreamingStatus
  {
    ChannelStreamingStatus m_streamStatus;
    time_t m_timestamp;
  };

  /**
  * Represents the margin (in minutes) of the recordings' start & end times
  */
  struct RecordingMargins
  {
    unsigned int m_beforeMargin;
    unsigned int m_afterMargin;
    bool operator!=(const RecordingMargins& other)
    {
      return (!(m_beforeMargin == other.m_beforeMargin && m_afterMargin == other.m_afterMargin));
    }
  };

  /**
   * The main class for interfacing with the VBox Gateway
   */
  class VBox
  {
  public:
    /**
     * The minimum backend software version required to use the addon
     */
    static const char* MINIMUM_SOFTWARE_VERSION;

    VBox(const Settings& settings);
    ~VBox();

    /**
     * Initializes the addon
     */
    void Initialize();
    void DetermineConnectionParams();
    bool ValidateSettings() const;
    const Settings& GetSettings() const;
    const ConnectionParameters& GetConnectionParams() const;
    StartupStateHandler& GetStateHandler();
    std::string GetApiBaseUrl() const;

    /**
     * Converts a UTC UNIX timestamp to an XMLTV timestamp localized for the
     * backends timezone offset
     * @param unixTimestamp a UTC UNIX timestamp
     * @return XMLTV timestamp localized for the current backend
     */
    std::string CreateTimestamp(const time_t unixTimestamp) const;
    std::string CreateDailyTime(const time_t unixTimestamp) const;

    // General API methods
    std::string GetBackendName() const;
    std::string GetBackendHostname() const;
    std::string GetBackendVersion() const;
    std::string GetConnectionString() const;

    // Channel methods
    int GetChannelsAmount() const;
    const std::vector<ChannelPtr>& GetChannels() const;
    const ChannelPtr GetChannel(unsigned int uniqueId) const;
    const ChannelPtr GetCurrentChannel() const;
    void SetCurrentChannel(const ChannelPtr& channel);
    ChannelStreamingStatus GetChannelStreamingStatus(const ChannelPtr& channel);
    void SetChannelStreamingStatus(const ChannelPtr& channel);

    // Recording methods
    bool SupportsRecordings() const;
    int64_t GetRecordingTotalSpace() const;
    int64_t GetRecordingUsedSpace() const;
    int GetRecordingsAmount() const;
    int GetTimersAmount() const;
    request::ApiRequest CreateDeleteRecordingRequest(const RecordingPtr& recording) const;
    request::ApiRequest CreateDeleteSeriesRequest(const SeriesRecordingPtr& series) const;
    bool DeleteRecordingOrTimer(unsigned int id);
    // for TIMER_VBOX_TYPE_EPG_BASED_SINGLE timer
    void AddTimer(const ChannelPtr& channel, const ::xmltv::ProgrammePtr programme);
    // for TIMER_VBOX_TYPE_MANUAL_SINGLE timer
    void AddTimer(const ChannelPtr& channel, time_t startTime, time_t endTime,
                  const std::string title, const std::string description);
    // for TIMER_VBOX_TYPE_EPG_BASED_MANUAL_SERIES timer
    void AddTimer(const ChannelPtr& channel, time_t startTime, time_t endTime,
                  const std::string title, const std::string description, const unsigned int weekdays);
    // for TIMER_VBOX_TYPE_EPG_BASED_AUTO_SERIES timer
    void AddSeriesTimer(const ChannelPtr& channel, const ::xmltv::ProgrammePtr programme);
    const std::vector<RecordingPtr>& GetRecordingsAndTimers() const;
    const std::vector<SeriesRecordingPtr>& GetSeriesTimers() const;
    void UpdateRecordingMargins(RecordingMargins defaultMargins);

    // EPG methods
    const Schedule GetSchedule(const ChannelPtr& channel) const;
    int GetCategoriesGenreType(std::vector<std::string>& categories) const;
    void StartEPGScan();
    void SyncEPGNow();
    void TriggerEpgUpdatesForChannels();
    bool IsInitialEpgSkippingCompleted();
    void MarkChannelAsInitialEpgSkipped(unsigned int channelUid);

    // Helpers
    static void LogException(VBoxException& e);

    // Event handlers
    std::function<void()> OnChannelsUpdated;
    std::function<void()> OnRecordingsUpdated;
    std::function<void()> OnTimersUpdated;
    std::function<void()> OnGuideUpdated;

  protected:
    bool m_skippingInitialEpgLoad = false;

  private:
    static const int INITIAL_EPG_WAIT_SECS = 60;
    static const int INITIAL_EPG_STEP_SECS = 5;

    void BackgroundUpdater();
    unsigned int GetDBVersion(std::string& versionName) const;
    void RetrieveChannels(bool triggerEvent = true);
    void RetrieveRecordings(bool triggerEvent = true);
    void RetrieveGuide(bool triggerEvent = true);
    void InitializeGenreMapper();
    void SwapChannelIcons(std::vector<ChannelPtr>& channels);
    void SendScanEPG(std::string& rEpgDetectionCheckMethod) const;
    void GetEpgDetectionState(std::string& methodName, std::string& flagName);
    void InitScanningEPG(std::string& rScanMethod, std::string& rGetStatusMethod, std::string& rfIsScanningFlag);
    void UpdateEpgScan(bool fRetrieveGuide);
    const RecordingMargins GetRecordingMargins(bool fBackendSingleMargin) const;
    void SetRecordingMargins(RecordingMargins margin, bool fBackendSingleMargin);

    void LogGuideStatistics(const ::xmltv::Guide& guide) const;
    response::ResponsePtr PerformRequest(const request::Request& request) const;

    /**
     * The addons settings
     */
    const Settings m_settings;

    /**
     * The connection parameters to use for requests
     */
    ConnectionParameters m_currentConnectionParameters;

    /**
     * The backend information
     */
    BackendInformation m_backendInformation;

    /**
     * The list of channels
     */
    std::vector<ChannelPtr> m_channels;

    /**
     * The list of recordings, including timeres
     */
    std::vector<RecordingPtr> m_recordings;

    /**
    * The list of recordings, including timeres
    */
    std::vector<SeriesRecordingPtr> m_series;

    /**
     * The guide data. The XMLTV channel name is the key, the value is the
     * schedule for the channel
     */
    ::xmltv::Guide m_guide;

    /**
     * The external guide data
     */
    ::xmltv::Guide m_externalGuide;

    /**
     * The guide channel mapper
     */
    GuideChannelMapperPtr m_guideChannelMapper;

    /**
     * The category<->genre mapper
     */
    CategoryMapperPtr m_categoryGenreMapper;

    /**
     * Handler for the startup state
     */
    StartupStateHandler m_stateHandler;

    /**
     * The background update thread
     */
    std::thread m_backgroundThread;

    /**
    * The state of EPG scanning - if set to EPGSCAN_SHOULD_SCAN --> EPG scanning starts
    */
    EPGScanState m_epgScanState;

    /**
    * The streaming status and the timestamp of when it was taken from the backend
    */
    TimedStreamingStatus m_lastStreamStatus;

    /**
    * Contains the channel's database version (as they were last updated)
    */
    std::atomic<unsigned int> m_channelsDBVersion;

    /**
    * Contains the guide's database version, as it was last updated (0 before update)
    */
    std::atomic<unsigned int> m_programsDBVersion;

    /**
     * Controls whether the background update thread should keep running or not
     */
    std::atomic<bool> m_active;

    /**
    * Controls whether the add-on should sync its EPG with the backend
    */
    std::atomic<bool> m_shouldSyncEpg;

    /**
     * The currently active channel, or the last active channel when no
     * channel is playing
     */
    ChannelPtr m_currentChannel;

    /**
     * Map of channels to be skipped on inital EPG load
     */
    std::map<std::string, std::string> m_unskippedInitialEpgChannelsMap;

    /**
     * Mutex for protecting access to m_channels and m_recordings
     */
    mutable std::mutex m_mutex;
  };
} // namespace vbox
