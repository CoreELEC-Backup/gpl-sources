/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <sstream>
#include <string>

namespace vbox
{

  enum ChannelOrder
  {
    CH_ORDER_BY_LCN = 0,
    CH_ORDER_BY_INDEX
  };

  /**
   * Represents a set of parameters required to make a connection
   */
  class ConnectionParameters
  {
  public:
    std::string hostname;
    int httpPort;
    int httpsPort;
    int upnpPort;
    int timeout;

    /**
     * @return whether the connection parameters appear valid
     */
    bool AreValid() const
    {
      return !hostname.empty() && httpPort > 0 && upnpPort > 0 && timeout > 0;
    }

    /**
     * @return the URI scheme to use
     */
    std::string GetUriScheme() const
    {
      return UseHttps() ? "https" : "http";
    }

    /**
     * @return the URI authority to use
     */
    std::string GetUriAuthority() const
    {
      std::stringstream ss;
      int port = UseHttps() ? httpsPort : httpPort;
      ss << hostname << ":" << port;

      return ss.str();
    }

    /**
     * @return whether HTTPS should be used or not
     */
    bool UseHttps() const { return httpsPort > 0; }
  };

  /**
   * Represents the settings for this addon
   */
  class Settings
  {
  public:
    ConnectionParameters m_internalConnectionParams;
    ConnectionParameters m_externalConnectionParams;
    ChannelOrder m_setChannelIdUsingOrder;
    bool m_skipInitialEpgLoad;
    bool m_timeshiftEnabled;
    std::string m_timeshiftBufferPath;
  };
} // namespace vbox
