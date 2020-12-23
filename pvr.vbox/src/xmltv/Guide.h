/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "Programme.h"
#include "Schedule.h"

#include <map>
#include <string>
#include <vector>

// Forward declarations
namespace tinyxml2
{
  class XMLElement;
}

namespace xmltv
{

  typedef std::map<std::string, xmltv::SchedulePtr> Schedules;

  /**
    * Represents a set of guide data. A guide has many schedules (one per
    * channel) and each schedule has many programmes
    */
  class Guide
  {
  public:
    Guide() = default;
    ~Guide() = default;

    /**
      * Creates a guide from the specified XMLTV contents
      */
    explicit Guide(const tinyxml2::XMLElement* m_content);

    /**
      * For combining the other guide into this one
      */
    Guide& operator+=(Guide& other)
    {
      // Add all schedules from the other object
      for (auto& entry : other.m_schedules)
        AddSchedule(entry.first, entry.second);

      // Merge the display name mappings
      m_displayNameMappings.insert(other.m_displayNameMappings.begin(), other.m_displayNameMappings.end());

      return *this;
    }

    /**
     * Adds the specified schedule on the specified channel
     * @param channelId the channel name
     * @param schedule the schedule (ownership is taken)
     */
    void AddSchedule(const std::string& channelId, SchedulePtr schedule) { m_schedules[channelId] = schedule; }

    /**
      * @param channelId the channel ID
      * @return the schedule for the specified channel, or nullptr if the
      * channel has no schedule
      */
    const SchedulePtr GetSchedule(const std::string& channelId) const;

    /**
      * Adds a new mapping between the display name and the channel ID
      * @param displayName the display name
      * @param channelId the channel ID
      */
    void AddDisplayNameMapping(const std::string& displayName, const std::string& channelId)
    {
      m_displayNameMappings[displayName] = channelId;
    }

    /**
     * Maps a channel display name to its XMLTV name (case-insensitive)
     * @param displayName the display name
     * @return the corresponding channel ID for specified display name
     */
    std::string GetChannelId(const std::string& displayName) const;

    /**
     * @return all the channel names in the guide
     */
    std::vector<std::string> GetChannelNames() const
    {
      std::vector<std::string> channelNames;

      for (const auto& mapping : m_displayNameMappings)
        channelNames.push_back(mapping.first);

      return channelNames;
    }

    /**
      * @return the schedules
      */
    const Schedules& GetSchedules() const { return m_schedules; }

  private:
    /**
      * The schedules
      */
    Schedules m_schedules;

    /**
      * Maps a display name to an XMLTV channel ID
      */
    std::map<std::string, std::string> m_displayNameMappings;
  };
} // namespace xmltv
