/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "SoftwareVersion.h"

using namespace vbox;

std::string SoftwareVersion::GetString() const
{
  return std::to_string(m_major) + "." +
         std::to_string(m_minor) + "." +
         std::to_string(m_revision);
}

SoftwareVersion SoftwareVersion::ParseString(const std::string& string)
{
  SoftwareVersion version;
  std::string format = "%d.%d.%d";

  if (string.substr(0, 1) == "V")
    format = string.substr(0, 2) + ".%d.%d.%d";

  sscanf(string.c_str(), format.c_str(), &version.m_major, &version.m_minor, &version.m_revision);

  return version;
}
