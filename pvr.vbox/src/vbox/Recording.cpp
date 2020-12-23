/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "Recording.h"

#include "../xmltv/Utilities.h"

using namespace vbox;

Recording::Recording(const std::string& channelId, const std::string& channelName, RecordingState state)
  : m_id(0), m_seriesId(0), m_channelId(channelId), m_channelName(channelName), m_state(state)
{
}

Recording::~Recording()
{
}

bool Recording::IsRunning(const std::time_t now, const std::string& channelName, std::time_t startTime) const
{
  time_t timerStartTime = xmltv::Utilities::XmltvToUnixTime(m_startTime);
  time_t timerEndTime = xmltv::Utilities::XmltvToUnixTime(m_endTime);
  if (!(timerStartTime <= now && now <= timerEndTime))
    return false;
  if (!channelName.empty() && m_channelName != channelName)
    return false;
  if (timerStartTime != startTime)
    return false;
  return true;
}
