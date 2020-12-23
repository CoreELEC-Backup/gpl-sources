/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <string>

namespace vbox
{
  /**
  * Represents a software version
  */
  class SoftwareVersion
  {
  public:
    bool operator==(const SoftwareVersion& other) const
    {
      return m_major == other.m_major &&
             m_minor == other.m_minor &&
             m_revision == other.m_revision;
    }

    bool operator>(const SoftwareVersion& other) const
    {
      return m_major > other.m_major ||
             m_minor > other.m_minor ||
             m_revision > other.m_revision;
    }

    bool operator<(const SoftwareVersion& other) const
    {
      return !(*this > other) && !(*this == other);
    }

    bool operator>=(const SoftwareVersion& other) const
    {
      return !(*this < other);
    }

    bool operator<=(const SoftwareVersion& other) const
    {
      return !(*this > other);
    }

    /**
     * @return the software version as a string
     */
    std::string GetString() const;

    /**
     * @return a SoftwareVersion object representing the specified version
     * string. The version string should either be e.g. "2.46.20" or "VB.2.46.20"
     */
    static SoftwareVersion ParseString(const std::string& string);

  private:
    unsigned int m_major = 0;
    unsigned int m_minor = 0;
    unsigned int m_revision = 0;
  };
} // namespace vbox
