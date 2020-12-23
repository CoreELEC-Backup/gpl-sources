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
   * Represents the streaming status of a channel. Only the fields required by
   * Kodi are currently implemented.
   */
  class ChannelStreamingStatus
  {
  public:
    ChannelStreamingStatus() = default;
    ~ChannelStreamingStatus() = default;

    /**
     * @return the service name (SID XXX)
     */
    std::string GetServiceName() const;

    /**
     * @return the mux name (XXX MHz (MODULATION))
     */
    std::string GetMuxName() const;

    /**
     * @return the tuner name
     */
    std::string GetTunerName() const;

    /**
     * @return the signal strength (between 0 and 100)
     */
    unsigned int GetSignalStrength() const;

    /**
     * @return the bit error rate
     */
    long GetBer() const;

    void SetServiceId(unsigned int sid) { m_sid = sid; }
    void SetTunerId(const std::string& tunerId) { m_tunerId = tunerId; }
    void SetTunerType(const std::string& tunerType) { m_tunerType = tunerType; }
    void SetRfLevel(const std::string& rfLevel) { m_rfLevel = rfLevel; }
    void SetBer(const std::string& ber) { m_ber = ber; }

  public:
    bool m_active = false;
    std::string m_lockStatus;
    std::string m_lockedMode;
    std::string m_modulation;
    std::string m_frequency;
    unsigned int m_signalQuality = 0;

  private:
    const static int RFLEVEL_MIN;
    const static int RFLEVEL_MAX;

    unsigned int m_sid = 0;
    std::string m_tunerId;
    std::string m_tunerType;
    std::string m_rfLevel;
    std::string m_ber;
  };
} // namespace vbox
