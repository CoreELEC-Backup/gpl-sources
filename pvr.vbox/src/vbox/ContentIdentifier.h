/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "../xmltv/Programme.h"
#include "../xmltv/Utilities.h"
#include "Channel.h"
#include "Recording.h"
#include "SeriesRecording.h"

#include <functional>

namespace vbox
{
  /**
   * Static helper class for creating unique identifiers for arbitrary objects
   */
  class ContentIdentifier
  {
  public:
    /**
     * @return a unique ID for the channel
     */
    static unsigned int GetUniqueId(const vbox::ChannelPtr& channel)
    {
      std::hash<std::string> hasher;
      int uniqueId = hasher(channel->m_uniqueId);
      return std::abs(uniqueId);
    }

    /**
     * @return a unique ID for the recording. This implementation must match
     * that of xmltv::Programme so that recordings can be linked to programmes.
     */
    static unsigned int GetUniqueId(const vbox::Recording* recording)
    {
      std::hash<std::string> hasher;
      std::string timestamp = std::to_string(::xmltv::Utilities::XmltvToUnixTime(recording->m_endTime));
      int uniqueId = hasher(std::string(recording->m_title) + timestamp);
      return std::abs(uniqueId);
    }

    /**
    * @return a unique ID for the series. This implementation must match
    * that of xmltv::Programme so that series can be linked to programmes.
    */
    static unsigned int GetUniqueId(const vbox::SeriesRecording* series)
    {
      std::hash<std::string> hasher;
      std::string timestamp = std::to_string(::xmltv::Utilities::XmltvToUnixTime(series->m_endTime));
      int uniqueId = hasher(std::string(series->m_title) + timestamp);
      return std::abs(uniqueId);
    }

    /**
     * @return a unique ID for the programme
     */
    static unsigned int GetUniqueId(const xmltv::Programme* programme)
    {
      std::hash<std::string> hasher;
      std::string timestamp = std::to_string(::xmltv::Utilities::XmltvToUnixTime(programme->m_endTime));
      int uniqueId = hasher(std::string(programme->m_title) + timestamp);
      return std::abs(uniqueId);
    }
  };
} // namespace vbox
