/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "Guide.h"

#include "../vbox/ContentIdentifier.h"
#include "Channel.h"
#include "Utilities.h"

#include <algorithm>

#include <kodi/tools/StringUtils.h>
#include <lib/tinyxml2/tinyxml2.h>

using namespace kodi::tools;
using namespace xmltv;
using namespace tinyxml2;

Guide::Guide(const XMLElement* m_content)
{
  for (const XMLElement* element = m_content->FirstChildElement("channel"); element != NULL;
       element = element->NextSiblingElement("channel"))
  {
    // Create the channel
    std::string channelId = Utilities::UrlDecode(element->Attribute("id"));
    const char* pChannelName = element->FirstChildElement("display-name")->GetText();
    std::string displayName = pChannelName ? pChannelName : "";
    ChannelPtr channel = ChannelPtr(new Channel(channelId, displayName));

    // Add channel icon if it exists
    auto* iconElement = element->FirstChildElement("icon");
    if (iconElement)
      channel->m_icon = iconElement->Attribute("src");

    // Populate the lookup table which maps XMLTV IDs to display names
    AddDisplayNameMapping(displayName, channelId);

    // Create a schedule for the channel
    m_schedules[channelId] = SchedulePtr(new Schedule(channel));
  }

  for (const XMLElement* element = m_content->FirstChildElement("programme"); element != NULL;
       element = element->NextSiblingElement("programme"))
  {
    // Extract the channel name and the programme
    std::string channelId = Utilities::UrlDecode(element->Attribute("channel"));
    xmltv::ProgrammePtr programme(new Programme(element));

    // Drop program if missing start/end times or channel
    if (programme->m_channelName.empty() || programme->m_startTime.empty() || programme->m_endTime.empty())
      continue;

    // Add the programme to the channel's schedule only if the title was parsable
    if (programme->m_title != Programme::STRING_FORMAT_NOT_SUPPORTED)
      m_schedules[channelId]->AddProgramme(programme);
  }
}

std::string Guide::GetChannelId(const std::string& displayName) const
{
  auto it = std::find_if(
    m_displayNameMappings.cbegin(),
    m_displayNameMappings.cend(),
    [displayName](const std::pair<std::string, std::string>& mapping) {
      return StringUtils::CompareNoCase(mapping.first, displayName) == 0;
    }
  );

  return it != m_displayNameMappings.cend() ? it->second : "";
}

const SchedulePtr Guide::GetSchedule(const std::string& channelId) const
{
  auto it = m_schedules.find(channelId);

  if (it != m_schedules.cend())
    return it->second;

  return nullptr;
}
