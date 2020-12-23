/*
 *  Copyright (C) 2016-2020 Garrett Brown
 *  Copyright (C) 2016-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <memory>
#include <vector>

namespace kodi
{
namespace addon
{
  class Joystick;
}
}

namespace XARCADE
{
  class CXArcadeDevice;
  typedef std::shared_ptr<CXArcadeDevice> DevicePtr;
  typedef std::vector<DevicePtr>          DeviceVector;

  typedef std::shared_ptr<kodi::addon::Joystick> JoystickPtr;
  typedef std::vector<JoystickPtr>         JoystickVector;
}
