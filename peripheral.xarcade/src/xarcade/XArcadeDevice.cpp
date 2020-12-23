/*
 *  Copyright (C) 2016-2020 Garrett Brown
 *  Copyright (C) 2016-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "XArcadeDevice.h"
#include "XArcadeDefines.h"
#include "utils/CommonDefines.h" // for INVALID_FD

#include <kodi/addon-instance/peripheral/PeripheralUtils.h>

#include <errno.h>
#include <linux/input.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace XARCADE;

const std::vector<CXArcadeDevice::KeyToButtonMap> CXArcadeDevice::m_keyMap = {
  // Player 1
  { KEY_LEFTCTRL,   0, 0,  BTN_A },
  { KEY_LEFTALT,    0, 1,  BTN_B },
  { KEY_SPACE,      0, 2,  BTN_C },
  { KEY_LEFTSHIFT,  0, 3,  BTN_X },
  { KEY_Z,          0, 4,  BTN_Y },
  { KEY_X,          0, 5,  BTN_Z },
  { KEY_C,          0, 6,  BTN_TL },
  { KEY_5,          0, 7,  BTN_TR },
  { KEY_1,          0, 8,  BTN_START },
  { KEY_3,          0, 9,  BTN_SELECT },
  { KEY_KP4,        0, 10, KEY_LEFT },
  { KEY_LEFT,       0, 10, KEY_LEFT },
  { KEY_KP6,        0, 11, KEY_RIGHT },
  { KEY_RIGHT,      0, 11, KEY_RIGHT },
  { KEY_KP8,        0, 12, KEY_UP },
  { KEY_UP,         0, 12, KEY_UP },
  { KEY_KP2,        0, 13, KEY_DOWN },
  { KEY_DOWN,       0, 13, KEY_DOWN },

  // Player 2
  { KEY_A,          1, 0,  BTN_A },
  { KEY_S,          1, 1,  BTN_B },
  { KEY_Q,          1, 2,  BTN_C },
  { KEY_W,          1, 3,  BTN_X },
  { KEY_E,          1, 4,  BTN_Y },
  { KEY_LEFTBRACE,  1, 5,  BTN_Z },
  { KEY_RIGHTBRACE, 1, 6,  BTN_TL },
  { KEY_6,          1, 7,  BTN_TR },
  { KEY_2,          1, 8,  BTN_START },
  { KEY_4,          1, 9,  BTN_SELECT },
  { KEY_D,          1, 10, KEY_LEFT },
  { KEY_G,          1, 11, KEY_RIGHT },
  { KEY_R,          1, 12, KEY_UP },
  { KEY_F,          1, 13, KEY_DOWN },
};

CXArcadeDevice::CXArcadeDevice(int fd, unsigned int index) :
  m_fd(fd),
  m_index(index),
  m_bOpen(false)
{
}

CXArcadeDevice::~CXArcadeDevice()
{
  Close();
}

bool CXArcadeDevice::Open()
{
  if (!m_bOpen)
  {
    if (m_fd != INVALID_FD)
    {
      int res = ioctl(m_fd, EVIOCGRAB, 1);
      if (res == 0)
        m_bOpen = true;
    }
  }

  return m_bOpen;
}

void CXArcadeDevice::Close()
{
  if (m_bOpen)
  {
    m_bOpen = false;
    ioctl(m_fd, EVIOCGRAB, 0);
    close(m_fd);
    m_fd = INVALID_FD;
  }
}

void CXArcadeDevice::GetJoystickInfo(JoystickVector& joysticks)
{
  joysticks.push_back(GetJoystick(0));
  joysticks.push_back(GetJoystick(1));
}

JoystickPtr CXArcadeDevice::GetJoystick(unsigned int index)
{
  if (index % 2 == 0)
  {
    JoystickPtr player1 = std::make_shared<kodi::addon::Joystick>(XARCADE_TANKSTICK_PROVIDER, XARCADE_TANKSTICK_NAME_PLAYER_1);
    player1->SetVendorID(XARCADE_TANKSTICK_VENDOR_ID);
    player1->SetProductID(XARCADE_TANKSTICK_PRODUCT_ID);
    player1->SetIndex(GetPeripheralIndex(index));
    player1->SetRequestedPort(0);
    player1->SetButtonCount(XARCADE_TANKSTICK_BUTTON_COUNT);
    return player1;
  }
  else
  {
    JoystickPtr player2 = std::make_shared<kodi::addon::Joystick>(XARCADE_TANKSTICK_PROVIDER, XARCADE_TANKSTICK_NAME_PLAYER_2);
    player2->SetVendorID(XARCADE_TANKSTICK_VENDOR_ID);
    player2->SetProductID(XARCADE_TANKSTICK_PRODUCT_ID);
    player2->SetIndex(GetPeripheralIndex(index));
    player2->SetRequestedPort(1);
    player2->SetButtonCount(XARCADE_TANKSTICK_BUTTON_COUNT);
    return player2;
  }
}

unsigned int CXArcadeDevice::GetPeripheralIndex(unsigned int playerIndex)
{
  if (playerIndex % 2 == 0)
    return m_index * 2;
  else
    return m_index * 2 + 1;
}

void CXArcadeDevice::GetEvents(std::vector<kodi::addon::PeripheralEvent>& events)
{
  if (!IsOpen())
    return;

  input_event ev[64];

  int rd = read(m_fd, ev, sizeof(ev));
  if (rd < 0)
  {
    // No events
    return;
  }

  const unsigned int count = rd / sizeof(ev[0]);
  for (unsigned int i = 0; i < count; i++)
  {
    const input_event& event = ev[i];

    if (event.type != EV_KEY)
      continue;

    int playerIndex = -1;
    int buttonIndex = -1;

    for (auto& keyProperties : m_keyMap)
    {
      if (event.code == keyProperties.key)
      {
        playerIndex = keyProperties.playerIndex;
        buttonIndex = keyProperties.buttonIndex;
        break;
      }
    }

    if (playerIndex != -1 && buttonIndex != -1)
    {
      JOYSTICK_STATE_BUTTON state = event.value > 0 ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED;
      events.push_back(kodi::addon::PeripheralEvent(GetPeripheralIndex(playerIndex), buttonIndex, state));
    }
  }
}
