/*
 *  Copyright (C) 2016-2020 Garrett Brown
 *  Copyright (C) 2016-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "xarcade/XArcadeTypes.h"

#include <kodi/addon-instance/Peripheral.h>

namespace XARCADE
{
  class CXArcadeScanner;
}

class ATTRIBUTE_HIDDEN CPeripheralXArcade
  : public kodi::addon::CAddonBase,
    public kodi::addon::CInstancePeripheral
{
public:
  CPeripheralXArcade();
  ~CPeripheralXArcade() override;

  ADDON_STATUS Create() override;
  ADDON_STATUS GetStatus() override;
  ADDON_STATUS SetSetting(const std::string& settingName, const kodi::CSettingValue& settingValue) override;

  void GetCapabilities(kodi::addon::PeripheralCapabilities& capabilities) override;
  PERIPHERAL_ERROR PerformDeviceScan(std::vector<std::shared_ptr<kodi::addon::Peripheral>>& scan_results) override;
  PERIPHERAL_ERROR GetEvents(std::vector<kodi::addon::PeripheralEvent>& events) override;
  bool SendEvent(const kodi::addon::PeripheralEvent& event) override;
  PERIPHERAL_ERROR GetJoystickInfo(unsigned int index, kodi::addon::Joystick& info) override;

private:
  XARCADE::DeviceVector m_devices;
  std::unique_ptr<XARCADE::CXArcadeScanner> m_scanner;
};
