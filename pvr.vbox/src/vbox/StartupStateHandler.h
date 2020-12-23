/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>

namespace vbox
{

  /**
    * The various startup states the addon can be in
    */
  enum StartupState
  {
    UNINITIALIZED,
    INITIALIZED,
    CHANNELS_LOADED,
    RECORDINGS_LOADED,
    GUIDE_LOADED,
    EXTERNAL_GUIDE_LOADED
  };

  /**
    * The startup state handler. It provides methods for waiting for a state
    * to be entered, e.g. if a method requires a method to run before itself it
    * can wait for a certain state change.
    */
  class StartupStateHandler
  {
  public:
    /**
      * Initializes the handler. The state is set to UNINITIALIZED by default.
      */
    StartupStateHandler() : m_state(StartupState::UNINITIALIZED) {}
    ~StartupStateHandler(){};

    /**
      * Waits for the specified state.
      *
      * @param state the desired state
      * @return whether the state was reached before the timeout
      */
    bool WaitForState(StartupState state) const
    {
      std::unique_lock<std::mutex> lock(m_mutex);

      // Wait for the state to change
      m_condition.wait_for(lock, std::chrono::seconds(STATE_WAIT_TIMEOUT),
                           [this, state]()
                           {
                             return m_state >= state;
                           });

      return m_state >= state;
    }

    /**
      * Enters the specified state
      *
      * @param state the state
      */
    void EnterState(StartupState state)
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_state = state;

      // Notify all waiters
      m_condition.notify_all();
    }

    /**
      * Returns the current state
      *
      * @return the state
      */
    StartupState GetState() const
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      return m_state;
    }

  private:
    /**
     * The maximum amount of seconds to block while waiting for a state
     * change
     */
    const static int STATE_WAIT_TIMEOUT;

    /**
      * The current state
      */
    StartupState m_state;

    /**
      * Mutex for m_state
      */
    mutable std::mutex m_mutex;

    /**
      * Condition variable for m_state
      */
    mutable std::condition_variable m_condition;
  };
} // namespace vbox
