/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <atomic>
#include <mutex>
#include <string>
#include <thread>

#include <kodi/addon-instance/pvr/General.h>

namespace enigma2
{
  static const int FAST_RECONNECT_ATTEMPTS = 5;
  static const int SLEEP_INTERVAL_STEP_MS = 500;

  class IConnectionListener;

  class ATTRIBUTE_HIDDEN ConnectionManager
  {
  public:
    ConnectionManager(IConnectionListener& connectionListener);
    ~ConnectionManager();

    void Start();
    void Stop();
    void Disconnect();
    void Reconnect();

    void OnSleep();
    void OnWake();

  private:
    void Process();
    void SetState(PVR_CONNECTION_STATE state);
    void SteppedSleep(int intervalMs);

    IConnectionListener& m_connectionListener;
    std::atomic<bool> m_running = {false};
    std::thread m_thread;
    mutable std::mutex m_mutex;
    bool m_suspended;
    PVR_CONNECTION_STATE m_state;
  };
} // namespace enigma2
