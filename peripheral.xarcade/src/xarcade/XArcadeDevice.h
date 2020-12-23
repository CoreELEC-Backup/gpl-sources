/*
 *  Copyright (C) 2016-2020 Garrett Brown
 *  Copyright (C) 2016-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "XArcadeTypes.h"

#include <vector>

namespace kodi
{
namespace addon
{
  class PeripheralEvent;
}
}

namespace XARCADE
{
  class CXArcadeDevice
  {
  public:
    CXArcadeDevice(int fd, unsigned int index);
    ~CXArcadeDevice();

    bool Open();
    bool IsOpen() const { return m_bOpen; }
    void Close();

    void GetJoystickInfo(JoystickVector& joysticks);
    JoystickPtr GetJoystick(unsigned int index);
    unsigned int GetPeripheralIndex(unsigned int playerIndex);

    void GetEvents(std::vector<kodi::addon::PeripheralEvent>& events);

  private:
    struct KeyToButtonMap
    {
      unsigned int key;
      unsigned int playerIndex;
      unsigned int buttonIndex;
      unsigned int gamepadButton;
    };

    static const std::vector<KeyToButtonMap> m_keyMap;

    // Construction parameters
    int m_fd;
    const unsigned int m_index;

    bool m_bOpen;
  };
}
