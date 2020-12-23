/*
 *  Copyright (C) 2015-2020 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2015 Zoltan Csizmadia (zcsizmadia@gmail.com)
 *  Copyright (C) 2011 Pulse-Eight (https://www.pulse-eight.com)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "Settings.h"

SettingsType& SettingsType::Get()
{
  static SettingsType settings;
  return settings;
}

bool SettingsType::ReadSettings()
{
  bHideProtected = kodi::GetSettingBoolean("hide_protected", true);
  bHideDuplicateChannels = kodi::GetSettingBoolean("hide_duplicate", true);
  bMarkNew = kodi::GetSettingBoolean("mark_new", true);
  bDebug = kodi::GetSettingBoolean("debug", false);

  return true;
}

ADDON_STATUS SettingsType::SetSetting(const std::string& settingName,
                                      const kodi::CSettingValue& settingValue)
{
  if (settingName == "hide_protected")
  {
    bHideProtected = settingValue.GetBoolean();
    return ADDON_STATUS_NEED_RESTART;
  }
  else if (settingName == "hide_duplicate")
  {
    bHideDuplicateChannels = settingValue.GetBoolean();
    return ADDON_STATUS_NEED_RESTART;
  }
  else if (settingName == "mark_new")
    bMarkNew = settingValue.GetBoolean();
  else if (settingName == "debug")
    bDebug = settingValue.GetBoolean();

  return ADDON_STATUS_OK;
}
