/*
 *  Copyright (C) 2016-2020 Garrett Brown
 *  Copyright (C) 2016-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#define PERIPHERAL_ADDON_JOYSTICKS

#include "addon.h"

#include "utils/CommonMacros.h"
#include "xarcade/XArcadeDefines.h"
#include "xarcade/XArcadeDevice.h"
#include "xarcade/XArcadeScanner.h"
#include "xarcade/XArcadeTypes.h"
#include "xarcade/XArcadeUtils.h"

#include <algorithm>

using namespace XARCADE;

CPeripheralXArcade::CPeripheralXArcade() :
  m_scanner(new CXArcadeScanner)
{
}

ADDON_STATUS CPeripheralXArcade::Create()
{
  return GetStatus();
}

CPeripheralXArcade::~CPeripheralXArcade() = default;

ADDON_STATUS CPeripheralXArcade::GetStatus()
{
  return ADDON_STATUS_OK;
}

ADDON_STATUS CPeripheralXArcade::SetSetting(const std::string& settingName, const kodi::CSettingValue& settingValue)
{
  return ADDON_STATUS_OK;
}

void CPeripheralXArcade::GetCapabilities(kodi::addon::PeripheralCapabilities& capabilities)
{
  capabilities.SetProvidesJoysticks(true);
  capabilities.SetProvidesJoystickRumble(false);
  capabilities.SetProvidesJoystickPowerOff(false);
  capabilities.SetProvidesButtonmaps(false);
}

PERIPHERAL_ERROR CPeripheralXArcade::PerformDeviceScan(std::vector<std::shared_ptr<kodi::addon::Peripheral>>& scan_results)
{
  // Close disconnected devices
  m_devices.erase(std::remove_if(m_devices.begin(), m_devices.end(),
    [](const DevicePtr& device)
    {
      return !device->IsOpen();
    }), m_devices.end());

  // Open new devices
  DeviceVector newDevices = m_scanner->GetDevices();
  for (auto& device : newDevices)
  {
    if (device->Open())
      m_devices.emplace_back(std::move(device));
  }

  // Get peripheral info
  JoystickVector joysticks;
  for (auto& device : m_devices)
    device->GetJoystickInfo(joysticks);

  // Upcast array pointers
  std::vector<kodi::addon::Peripheral*> peripherals;
  for (auto& joystick : joysticks)
    scan_results.emplace_back(joystick);

  return PERIPHERAL_NO_ERROR;
}

PERIPHERAL_ERROR CPeripheralXArcade::GetEvents(std::vector<kodi::addon::PeripheralEvent>& events)
{
  for (auto& device : m_devices)
    device->GetEvents(events);

  return PERIPHERAL_NO_ERROR;
}

bool CPeripheralXArcade::SendEvent(const kodi::addon::PeripheralEvent& event)
{
  return false;
}

PERIPHERAL_ERROR CPeripheralXArcade::GetJoystickInfo(unsigned int index, kodi::addon::Joystick& info)
{
  JoystickPtr joystick;

  for (auto& device : m_devices)
  {
    if (device->GetPeripheralIndex(0) == index ||
        device->GetPeripheralIndex(1) == index)
    {
      joystick = device->GetJoystick(index);
      break;
    }
  }

  if (joystick)
  {
    info = *joystick;
    return PERIPHERAL_NO_ERROR;
  }

  return PERIPHERAL_ERROR_NOT_CONNECTED;
}

ADDONCREATOR(CPeripheralXArcade) // Don't touch this!
