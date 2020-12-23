/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <kodi/addon-instance/PVR.h>
#include <vector>

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 80

class CCurlFile
{
public:
  CCurlFile() = default;
  ~CCurlFile() = default;

  bool Get(const std::string& strURL, std::string& strResult);
};

class ATTRIBUTE_HIDDEN N7Xml : public kodi::addon::CAddonBase,
                               public kodi::addon::CInstancePVRClient

{
public:
  N7Xml();
  ~N7Xml() override = default;

  ADDON_STATUS SetSetting(const std::string& settingName,
                          const kodi::CSettingValue& settingValue) override;

  PVR_ERROR GetCapabilities(kodi::addon::PVRCapabilities& capabilities) override;
  PVR_ERROR GetBackendName(std::string& name) override;
  PVR_ERROR GetBackendVersion(std::string& version) override;
  PVR_ERROR GetBackendHostname(std::string& hostname) override;
  PVR_ERROR GetConnectionString(std::string& connection) override;

  PVR_ERROR GetChannelsAmount(int& amount) override;
  PVR_ERROR GetChannels(bool bRadio, kodi::addon::PVRChannelsResultSet& results) override;
  PVR_ERROR GetChannelStreamProperties(
      const kodi::addon::PVRChannel& channel,
      std::vector<kodi::addon::PVRStreamProperty>& properties) override;

private:
  void list_channels();

  std::string m_strHostname = DEFAULT_HOST;
  int m_iPort = DEFAULT_PORT;

  std::vector<std::pair<std::string, kodi::addon::PVRChannel>> m_channels;
  bool m_connected = false;
};
