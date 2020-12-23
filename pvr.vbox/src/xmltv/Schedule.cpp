/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "Schedule.h"

#include "../vbox/ContentIdentifier.h"

#include <algorithm>
#include <iterator>

using namespace xmltv;

Schedule::Schedule(ChannelPtr& channel) : m_channel(channel)
{
}

void Schedule::AddProgramme(ProgrammePtr programme)
{
  m_programmes.push_back(programme);
}

const ProgrammePtr Schedule::GetProgramme(int programmeUniqueId) const
{
  auto it = std::find_if(
    m_programmes.cbegin(),
    m_programmes.cend(),
    [programmeUniqueId](const ProgrammePtr& programme) {
        return programmeUniqueId == vbox::ContentIdentifier::GetUniqueId(programme.get());
    }
  );

  if (it != m_programmes.cend())
    return *it;

  return nullptr;
}

const Segment Schedule::GetSegment(time_t startTime, time_t endTime) const
{
  Segment segment;

  // Copy matching programmes to the segment
  std::copy_if(
    m_programmes.cbegin(),
    m_programmes.cend(),
    std::back_inserter(segment),
    [startTime, endTime](const ProgrammePtr& programme) {
      time_t programmeStartTime = Utilities::XmltvToUnixTime(programme->m_startTime);
      time_t programmeEndTime = Utilities::XmltvToUnixTime(programme->m_endTime);

      return programmeStartTime >= startTime && programmeEndTime <= endTime;
    }
  );

  return segment;
}
