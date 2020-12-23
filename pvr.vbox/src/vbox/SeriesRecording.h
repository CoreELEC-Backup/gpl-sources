/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <memory>
#include <string>

namespace vbox
{

  class SeriesRecording;
  typedef std::unique_ptr<SeriesRecording> SeriesRecordingPtr;

  /**
  * Represents a series
  */
  class SeriesRecording
  {
  public:
    SeriesRecording(const std::string& channelId);
    ~SeriesRecording() = default;

    bool operator==(const SeriesRecording& other)
    {
      return m_id == other.m_id &&
             m_scheduledId == other.m_scheduledId &&
             m_channelId == other.m_channelId &&
             m_title == other.m_title &&
             m_description == other.m_description &&
             m_startTime == other.m_startTime &&
             m_endTime == other.m_endTime;
    }

    bool operator!=(const SeriesRecording& other)
    {
      return !(*this == other);
    }

    unsigned int m_id;
    unsigned int m_scheduledId;
    std::string m_channelId;
    std::string m_title;
    std::string m_description;
    bool m_fIsAuto;
    std::string m_startTime;
    std::string m_endTime;
    unsigned int m_weekdays;
  };

} // namespace vbox