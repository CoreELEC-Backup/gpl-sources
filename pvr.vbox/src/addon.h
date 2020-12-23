/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <kodi/AddonBase.h>

namespace vbox
{
class Settings;
}

class CVBoxInstance;

class ATTRIBUTE_HIDDEN CVBoxAddon : public kodi::addon::CAddonBase
{
public:
  CVBoxAddon() = default;

  ADDON_STATUS CreateInstance(int instanceType, const std::string& instanceID, KODI_HANDLE instance, const std::string& version, KODI_HANDLE& addonInstance) override;
  void DestroyInstance(int instanceType, const std::string& instanceID, KODI_HANDLE addonInstance) override;
  ADDON_STATUS SetSetting(const std::string& settingName, const kodi::CSettingValue& settingValue) override;

private:
  void ReadSettings(vbox::Settings& settings);

  CVBoxInstance* m_vbox = nullptr;
};
