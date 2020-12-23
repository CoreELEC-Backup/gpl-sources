/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "SeriesRecording.h"

using namespace vbox;

SeriesRecording::SeriesRecording(const std::string& channelId)
  : m_id(0), m_scheduledId(0), m_channelId(channelId), m_fIsAuto(false), m_weekdays(0)
{
}