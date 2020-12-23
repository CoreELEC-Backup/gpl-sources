/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  Copyright (C) 2011 Pulse-Eight
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "addon.h"

#include "PctvData.h"

#include <kodi/Filesystem.h>
#include <kodi/tools/StringUtils.h>

using kodi::tools::StringUtils;

ADDON_STATUS CPCTVAddon::CreateInstance(int instanceType,
                                        const std::string& instanceID,
                                        KODI_HANDLE instance,
                                        const std::string& version,
                                        KODI_HANDLE& addonInstance)
{
  if (instanceType == ADDON_INSTANCE_PVR)
  {
    kodi::Log(ADDON_LOG_DEBUG, "%s - Creating PCTV Systems PVR-Client", __func__);

    if (!kodi::vfs::DirectoryExists(kodi::GetBaseUserPath()))
    {
      kodi::vfs::CreateDirectory(kodi::GetBaseUserPath());
    }

    m_strHostname = kodi::GetSettingString("host", DEFAULT_HOST);
    m_iPortWeb = kodi::GetSettingInt("webport", DEFAULT_WEB_PORT);
    m_bUsePIN = kodi::GetSettingBoolean("usepin", DEFAULT_USEPIN);
    m_strPin = StringUtils::Format("%04i", kodi::GetSettingInt("pin", 0));
    m_bTranscode = kodi::GetSettingBoolean("transcode", DEFAULT_TRANSCODE);
    m_iBitrate = kodi::GetSettingInt("bitrate", DEFAULT_BITRATE);

    Pctv* usedInstance = new Pctv(m_strHostname, m_iPortWeb, m_strPin, m_iBitrate, m_bTranscode,
                                  m_bUsePIN, instance, version);
    addonInstance = usedInstance;
    m_usedInstances.emplace(instanceID, usedInstance);

    if (!usedInstance->Open())
      return ADDON_STATUS_LOST_CONNECTION;

    return ADDON_STATUS_OK;
  }

  return ADDON_STATUS_UNKNOWN;
}

void CPCTVAddon::DestroyInstance(int instanceType,
                                 const std::string& instanceID,
                                 KODI_HANDLE addonInstance)
{
  if (instanceType == ADDON_INSTANCE_PVR)
  {
    kodi::Log(ADDON_LOG_DEBUG, "%s: Destoying octonet pvr instance", __func__);

    const auto& it = m_usedInstances.find(instanceID);
    if (it != m_usedInstances.end())
    {
      m_usedInstances.erase(it);
    }
  }
}

ADDON_STATUS CPCTVAddon::SetSetting(const std::string& settingName,
                                    const kodi::CSettingValue& settingValue)
{
  if (settingName == "host")
  {
    if (m_strHostname != settingValue.GetString())
    {
      kodi::Log(ADDON_LOG_INFO, "%s - Changed Setting 'host' from %s to %s", __func__,
                m_strHostname.c_str(), settingValue.GetString().c_str());
      return ADDON_STATUS_NEED_RESTART;
    }
  }
  else if (settingName == "webport")
  {
    if (m_iPortWeb != settingValue.GetInt())
    {
      kodi::Log(ADDON_LOG_INFO, "%s - Changed Setting 'webport' from %u to %u", __func__,
                m_iPortWeb, settingValue.GetInt());
      return ADDON_STATUS_NEED_RESTART;
    }
  }
  else if (settingName == "usepin")
  {
    if (m_bUsePIN != settingValue.GetBoolean())
    {
      kodi::Log(ADDON_LOG_INFO, "%s - Changed Setting 'usepin'", __func__);
      return ADDON_STATUS_NEED_RESTART; // restart is needed
    }
  }
  else if (settingName == "pin")
  {
    if (m_strPin != StringUtils::Format("%04i", settingValue.GetInt()))
    {
      kodi::Log(ADDON_LOG_INFO, "%s - Changed Setting 'pin'", __func__);
      return ADDON_STATUS_NEED_RESTART;
    }
  }
  else if (settingName == "transcode")
  {
    if (m_bTranscode != settingValue.GetInt())
    {
      kodi::Log(ADDON_LOG_INFO, "%s - Changed Setting 'transcode'", __func__);
      return ADDON_STATUS_NEED_RESTART; // restart is needed to update strStreamURL in Channel entries
    }
  }
  else if (settingName == "bitrate")
  {
    if (m_iBitrate != settingValue.GetInt())
    {
      kodi::Log(ADDON_LOG_INFO, "%s - Changed Setting 'bitrate' from %u to %u", __func__,
                m_iBitrate, settingValue.GetInt());
      return ADDON_STATUS_NEED_RESTART; // restart is needed to update strStreamURL in Channel entries
    }
  }

  return ADDON_STATUS_OK;
}

ADDONCREATOR(CPCTVAddon)
