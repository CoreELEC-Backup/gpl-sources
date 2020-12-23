/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "addon.h"
#include "VBoxInstance.h"

using namespace vbox;

ADDON_STATUS CVBoxAddon::CreateInstance(int instanceType, const std::string& instanceID, KODI_HANDLE instance, const std::string& version, KODI_HANDLE& addonInstance)
{
  if (instanceType == ADDON_INSTANCE_PVR)
  {
    kodi::Log(ADDON_LOG_DEBUG, "creating VBox Gateway PVR addon");

    Settings settings;
    ReadSettings(settings);

    m_vbox = new CVBoxInstance(settings, instance, version);
    ADDON_STATUS status = m_vbox->Initialize();

    addonInstance = m_vbox;

    return status;
  }

  return ADDON_STATUS_UNKNOWN;
}

void CVBoxAddon::DestroyInstance(int instanceType, const std::string& instanceID, KODI_HANDLE addonInstance)
{
  if (instanceType == ADDON_INSTANCE_PVR)
  {
    m_vbox = nullptr;
  }
}

void CVBoxAddon::ReadSettings(Settings& settings)
{
  settings.m_internalConnectionParams.hostname = kodi::GetSettingString("hostname", "");
  settings.m_internalConnectionParams.httpPort = kodi::GetSettingInt("http_port", 80);
  settings.m_internalConnectionParams.httpsPort = kodi::GetSettingInt("https_port", 0);
  settings.m_internalConnectionParams.upnpPort = kodi::GetSettingInt("upnp_port", 55555);
  settings.m_internalConnectionParams.timeout = kodi::GetSettingInt("connection_timeout", 3);

  settings.m_externalConnectionParams.hostname = kodi::GetSettingString("external_hostname", "");
  settings.m_externalConnectionParams.httpPort = kodi::GetSettingInt("external_http_port", 19999);
  settings.m_externalConnectionParams.httpsPort = kodi::GetSettingInt("external_https_port", 0);
  settings.m_externalConnectionParams.upnpPort = kodi::GetSettingInt("external_upnp_port", 55555);
  settings.m_externalConnectionParams.timeout = kodi::GetSettingInt("connection_timeout", 10);

  settings.m_setChannelIdUsingOrder = kodi::GetSettingEnum<vbox::ChannelOrder>("set_channelid_using_order", CH_ORDER_BY_LCN);
  settings.m_skipInitialEpgLoad = kodi::GetSettingBoolean("skip_initial_epg_load", true);
  settings.m_timeshiftEnabled = kodi::GetSettingBoolean("timeshift_enabled", false);
  settings.m_timeshiftBufferPath = kodi::GetSettingString("timeshift_path", "");
}

ADDON_STATUS CVBoxAddon::SetSetting(const std::string& settingName, const kodi::CSettingValue& settingValue)
{
#define UPDATE_STR(key, var) \
  if (settingName == key) \
  { \
    if (var != settingValue.GetString()) \
    { \
      kodi::Log(ADDON_LOG_INFO, "updated setting %s from '%s' to '%s'", settingName.c_str(), var.c_str(), settingValue.GetString().c_str()); \
      return ADDON_STATUS_NEED_RESTART; \
    } \
    return ADDON_STATUS_OK; \
  }

#define UPDATE_INT(key, var) \
  if (settingName == key) \
  { \
    if (var != settingValue.GetInt()) \
    { \
      kodi::Log(ADDON_LOG_INFO, "updated setting %s from '%d' to '%d'", settingName.c_str(), var, settingValue.GetInt()); \
      return ADDON_STATUS_NEED_RESTART; \
    } \
    return ADDON_STATUS_OK; \
  }

#define UPDATE_BOOL(key, var) \
  if (settingName == key) \
  { \
    if (var != settingValue.GetBoolean()) \
    { \
      kodi::Log(ADDON_LOG_INFO, "updated setting %s from '%d' to '%d'", settingName.c_str(), var, settingValue.GetBoolean()); \
      return ADDON_STATUS_NEED_RESTART; \
    } \
    return ADDON_STATUS_OK; \
  }

  if (m_vbox)
  {
    const vbox::Settings& settings = m_vbox->GetSettings();

    UPDATE_STR("hostname", settings.m_internalConnectionParams.hostname);
    UPDATE_INT("http_port", settings.m_internalConnectionParams.httpPort);
    UPDATE_INT("https_port", settings.m_internalConnectionParams.httpsPort);
    UPDATE_INT("upnp_port", settings.m_internalConnectionParams.upnpPort);
    UPDATE_INT("connection_timeout", settings.m_internalConnectionParams.timeout);
    UPDATE_STR("external_hostname", settings.m_externalConnectionParams.hostname);
    UPDATE_INT("external_http_port", settings.m_externalConnectionParams.httpPort);
    UPDATE_INT("external_https_port", settings.m_externalConnectionParams.httpsPort);
    UPDATE_INT("external_upnp_port", settings.m_externalConnectionParams.upnpPort);
    UPDATE_INT("external_connection_timeout", settings.m_externalConnectionParams.timeout);
    UPDATE_INT("set_channelid_using_order", settings.m_setChannelIdUsingOrder);
    UPDATE_BOOL("skip_initial_epg_load", settings.m_skipInitialEpgLoad);
    UPDATE_BOOL("timeshift_enabled", settings.m_timeshiftEnabled);
    UPDATE_STR("timeshift_path", settings.m_timeshiftBufferPath);
  }

  return ADDON_STATUS_OK;
#undef UPDATE_BOOL
#undef UPDATE_INT
#undef UPDATE_STR
}

ADDONCREATOR(CVBoxAddon)
