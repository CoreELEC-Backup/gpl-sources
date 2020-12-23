/*
 *  Copyright (C) 2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#ifndef CLIENT_H
#define CLIENT_H

#include "settings.h"

#include <kodi/AddonBase.h>

class PVRClientMythTV;
class PVRClientLauncher;

class ATTRIBUTE_HIDDEN CPVRMythTVAddon : public kodi::addon::CAddonBase
{
public:
  CPVRMythTVAddon() = default;

  ADDON_STATUS SetSetting(const std::string& settingName,
                          const kodi::CSettingValue& settingValue) override;
  ADDON_STATUS CreateInstance(int instanceType,
                              const std::string& instanceID,
                              KODI_HANDLE instance,
                              const std::string& version,
                              KODI_HANDLE& addonInstance) override;
  void DestroyInstance(int instanceType,
                       const std::string& instanceID,
                       KODI_HANDLE addonInstance) override;

private:
  CMythSettings m_settings;
  PVRClientMythTV* m_client = nullptr;
  PVRClientLauncher* m_launcher = nullptr;
};

#endif /* CLIENT_H */
