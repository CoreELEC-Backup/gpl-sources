/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <memory>
#include <string>

namespace vbox
{

  class Channel;
  typedef std::shared_ptr<Channel> ChannelPtr;

  /**
  * Represents a channel
  */
  class Channel
  {
  public:
    Channel(const std::string& uniqueId, const std::string& xmltvName, const std::string& name, const std::string& url)
      : m_uniqueId(uniqueId),
        m_index(0),
        m_xmltvName(xmltvName),
        m_name(name),
        m_number(0),
        m_radio(false),
        m_url(url),
        m_encrypted(false)
    {
    }
    ~Channel() {}

    bool operator==(const Channel& other)
    {
      return m_index == other.m_index &&
        m_xmltvName == other.m_xmltvName &&
        m_name == other.m_name &&
        m_number == other.m_number &&
        m_iconUrl == other.m_iconUrl &&
        m_radio == other.m_radio &&
        m_url == other.m_url &&
        m_encrypted == other.m_encrypted &&
        m_uniqueId == other.m_uniqueId;
    }

    bool operator!=(const Channel& other)
    {
      return !(*this == other);
    }

    /**
     * The internal name used by VBox
     */
    std::string m_uniqueId;

    /**
    * The index of the channel, as it appears in the API results. Needed for
    * some API requests.
    */
    unsigned int m_index;

    /**
    * The XMLTV channel ID
    */
    std::string m_xmltvName;

    std::string m_name;
    unsigned int m_number;
    std::string m_iconUrl;
    bool m_radio;
    std::string m_url;
    bool m_encrypted;
  };
} // namespace vbox
