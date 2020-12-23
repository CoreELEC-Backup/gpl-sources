/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once


#pragma once

#include <kodi/AddonBase.h>
#include <unordered_map>

#define DEFAULT_HOST "192.168.1.20"
#define DEFAULT_WEB_PORT 80
#define DEFAULT_PIN "0000"
#define DEFAULT_TRANSCODE false
#define DEFAULT_USEPIN false
#define DEFAULT_BITRATE 1200

class Pctv;

class ATTRIBUTE_HIDDEN CPCTVAddon : public kodi::addon::CAddonBase
{
public:
  CPCTVAddon() = default;

  ADDON_STATUS CreateInstance(int instanceType,
                              const std::string& instanceID,
                              KODI_HANDLE instance,
                              const std::string& version,
                              KODI_HANDLE& addonInstance) override;
  void DestroyInstance(int instanceType,
                       const std::string& instanceID,
                       KODI_HANDLE addonInstance) override;

  ADDON_STATUS SetSetting(const std::string& settingName,
                          const kodi::CSettingValue& settingValue) override;

private:
  std::unordered_map<std::string, Pctv*> m_usedInstances;

  std::string m_strHostname = DEFAULT_HOST;
  int m_iPortWeb = DEFAULT_WEB_PORT;
  std::string m_strPin = DEFAULT_PIN;
  bool m_bTranscode = DEFAULT_TRANSCODE;
  bool m_bUsePIN = DEFAULT_USEPIN;
  int m_iBitrate = DEFAULT_BITRATE;
};
