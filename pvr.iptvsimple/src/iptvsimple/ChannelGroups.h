/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "Channels.h"
#include "data/ChannelGroup.h"

#include <string>
#include <vector>

#include <kodi/addon-instance/pvr/ChannelGroups.h>

namespace iptvsimple
{
  class ChannelGroups
  {
  public:
    ChannelGroups(const iptvsimple::Channels& channels);

    int GetChannelGroupsAmount() const;
    PVR_ERROR GetChannelGroups(kodi::addon::PVRChannelGroupsResultSet& results, bool radio) const;
    PVR_ERROR GetChannelGroupMembers(const kodi::addon::PVRChannelGroup& group, kodi::addon::PVRChannelGroupMembersResultSet& results);

    int AddChannelGroup(iptvsimple::data::ChannelGroup& channelGroup);
    iptvsimple::data::ChannelGroup* GetChannelGroup(int uniqueId);
    iptvsimple::data::ChannelGroup* FindChannelGroup(const std::string& name);
    const std::vector<data::ChannelGroup>& GetChannelGroupsList() const { return m_channelGroups; }
    void Clear();

  private:
    const iptvsimple::Channels& m_channels;
    std::vector<iptvsimple::data::ChannelGroup> m_channelGroups;
  };
} //namespace iptvsimple
