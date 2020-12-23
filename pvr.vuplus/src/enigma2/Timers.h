/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "Epg.h"
#include "data/AutoTimer.h"
#include "data/Timer.h"
#include "extract/EpgEntryExtractor.h"

#include <atomic>
#include <ctime>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>

#include <tinyxml.h>

namespace enigma2
{
  class IConnectionListener;

  class ATTRIBUTE_HIDDEN Timers
  {
  public:
    Timers(IConnectionListener& connectionListener, Channels& channels, ChannelGroups& channelGroups, std::vector<std::string>& locations, Epg& epg, enigma2::extract::EpgEntryExtractor& entryExtractor)
      : m_connectionListener(connectionListener), m_channels(channels), m_channelGroups(channelGroups), m_locations(locations), m_epg(epg), m_entryExtractor(entryExtractor)
    {
      m_clientIndexCounter = 1;
    };

    void GetTimerTypes(std::vector<kodi::addon::PVRTimerType>& types) const;

    int GetTimerCount() const;
    int GetAutoTimerCount() const;

    void GetTimers(std::vector<kodi::addon::PVRTimer>& timers) const;
    void GetAutoTimers(std::vector<kodi::addon::PVRTimer>& timers) const;

    enigma2::data::Timer* GetTimer(std::function<bool(const enigma2::data::Timer&)> func);
    enigma2::data::AutoTimer* GetAutoTimer(std::function<bool(const enigma2::data::AutoTimer&)> func);

    PVR_ERROR AddTimer(const kodi::addon::PVRTimer& timer);
    PVR_ERROR AddAutoTimer(const kodi::addon::PVRTimer& timer);

    PVR_ERROR UpdateTimer(const kodi::addon::PVRTimer& timer);
    PVR_ERROR UpdateAutoTimer(const kodi::addon::PVRTimer& timer);

    PVR_ERROR DeleteTimer(const kodi::addon::PVRTimer& timer);
    PVR_ERROR DeleteAutoTimer(const kodi::addon::PVRTimer& timer);

    void ClearTimers();
    bool TimerUpdates();
    void RunAutoTimerListCleanup();
    void AddTimerChangeWatcher(std::atomic_bool* watcher);

  private:
    //templates
    template<typename T>
    T* GetTimer(std::function<bool(const T&)> func, std::vector<T>& timerlist);

    // functions
    bool LoadTimers(std::vector<enigma2::data::Timer>& timers) const;
    void GenerateChildManualRepeatingTimers(std::vector<enigma2::data::Timer>* timers, enigma2::data::Timer* timer) const;
    static std::string ConvertToAutoTimerTag(std::string tag);
    static std::string RemovePaddingTag(std::string tag);
    bool LoadAutoTimers(std::vector<enigma2::data::AutoTimer>& autoTimers) const;
    bool IsAutoTimer(const kodi::addon::PVRTimer& timer) const;
    bool TimerUpdatesRegular();
    bool TimerUpdatesAuto();
    std::string BuildAddUpdateAutoTimerLimitGroupsParams(const std::shared_ptr<data::Channel>& channel);
    static std::string BuildAddUpdateAutoTimerIncludeParams(int weekdays);

    // members
    unsigned int m_clientIndexCounter;
    std::vector<std::atomic_bool*> m_timerChangeWatchers;

    enigma2::extract::EpgEntryExtractor& m_entryExtractor;

    enigma2::Settings& m_settings = enigma2::Settings::GetInstance();
    std::vector<enigma2::data::Timer> m_timers;
    std::vector<enigma2::data::AutoTimer> m_autotimers;

    IConnectionListener& m_connectionListener;
    Channels& m_channels;
    ChannelGroups& m_channelGroups;
    std::vector<std::string>& m_locations;
    Epg& m_epg;
  };
} // namespace enigma2
