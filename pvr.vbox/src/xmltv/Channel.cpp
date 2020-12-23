/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "Channel.h"

using namespace xmltv;

Channel::Channel(const std::string& id, const std::string& displayName)
  : m_id(id), m_displayName(displayName), m_icon("")
{
}
