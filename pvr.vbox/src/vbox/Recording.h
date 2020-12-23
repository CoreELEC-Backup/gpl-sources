/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <ctime>
#include <memory>
#include <string>

namespace vbox
{

  /**
   * The possible states a recording can be in
   */
  enum RecordingState
  {
    SCHEDULED,
    RECORDED,
    RECORDING,
    RECORDING_ERROR, // Can't use just ERROR because of Windows
    EXTERNAL
  };

  class Recording;
  typedef std::unique_ptr<Recording> RecordingPtr;

  /**
   * Represents a recording
   */
  class Recording
  {
  public:
    Recording(const std::string& channelId, const std::string& channelName, RecordingState state);
    ~Recording();

    bool operator==(const Recording& other)
    {
      return m_id == other.m_id &&
        m_seriesId == other.m_seriesId &&
        m_channelId == other.m_channelId &&
        m_channelName == other.m_channelName &&
        m_url == other.m_url &&
        m_title == other.m_title &&
        m_description == other.m_description &&
        m_startTime == other.m_startTime &&
        m_endTime == other.m_endTime &&
        m_duration == other.m_duration &&
        m_state == other.m_state;
    }

    bool operator!=(const Recording& other)
    {
      return !(*this == other);
    }

    bool IsRunning(const std::time_t now, const std::string& channelName, std::time_t startTime) const;

    /**
     * Whether this object represents a timer
     * @return true if timer
     */
    bool IsTimer() const
    {
      return m_state == RecordingState::SCHEDULED || m_state == RecordingState::RECORDING;
    }

    /**
     * Whether this object represents a recording
     * @return true if recording
     */
    bool IsRecording() const
    {
      return m_state == RecordingState::RECORDED ||
             m_state == RecordingState::RECORDING ||
             m_state == RecordingState::RECORDING_ERROR ||
             m_state == RecordingState::EXTERNAL;
    }

    /**
     * Returns the state of the recording
     * @return the state
     */
    RecordingState GetState() const { return m_state; }

    unsigned int m_id;
    unsigned int m_seriesId;
    std::string m_channelId;
    std::string m_channelName;
    std::string m_url;
    std::string m_filename;
    std::string m_title;
    std::string m_description;
    std::string m_startTime;
    std::string m_endTime;
    int m_duration;

  private:
    RecordingState m_state;
  };
} // namespace vbox
