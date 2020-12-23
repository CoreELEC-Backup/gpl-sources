/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "ChannelStreamingStatus.h"

#include <sstream>
#include <stdexcept>

using namespace vbox;

const int ChannelStreamingStatus::RFLEVEL_MIN = -96;
const int ChannelStreamingStatus::RFLEVEL_MAX = -60;

std::string ChannelStreamingStatus::GetServiceName() const
{
  if (!m_active)
    return "";

  std::stringstream ss;
  ss << "SID " << m_sid;

  return ss.str();
}

std::string ChannelStreamingStatus::GetMuxName() const
{
  if (!m_active)
    return "";

  std::stringstream ss;
  ss << m_lockedMode << " @ " << m_frequency << " (" << m_modulation << ")";

  return ss.str();
}

std::string ChannelStreamingStatus::GetTunerName() const
{
  if (!m_active)
    return "";

  std::stringstream ss;
  ss << m_tunerType << " tuner #" << m_tunerId;

  return ss.str();
}

unsigned int ChannelStreamingStatus::GetSignalStrength() const
{
  if (!m_active)
    return 0;

  unsigned int rfLevel = 0;

  try
  {
    // Convert the RF level to an integer
    rfLevel = static_cast<unsigned int>(std::stoi(m_rfLevel));

    // If the level is above the maximum we consider it to be perfect
    if (rfLevel > RFLEVEL_MAX)
      return 100;

    // Normalize the value to between 0 and 1
    // TODO: This is not very scientific
    double normalized = static_cast<double>(rfLevel - RFLEVEL_MIN) /
                        static_cast<double>(RFLEVEL_MAX - RFLEVEL_MIN);

    return static_cast<unsigned int>(normalized * 100);
  }
  catch (std::invalid_argument)
  {
  }

  return rfLevel;
}

long ChannelStreamingStatus::GetBer() const
{
  if (!m_active)
    return 0;

  try
  {
    // Make sure it's not detected as hexadecimal
    return std::stol(m_ber);
  }
  catch (std::invalid_argument)
  {
    return 0;
  }
}
