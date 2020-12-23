/*
 *      vdr-plugin-vnsi - KODI server plugin for VDR
 *
 *      Copyright (C) 2005-2013 Team XBMC
 *      Copyright (C) 2015 Team KODI
 *
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with KODI; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "channelfilter.h"
#include "config.h"
#include "hash.h"
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vdr/tools.h>

cVNSIProvider::cVNSIProvider()
  :m_caid(0)
{

}

cVNSIProvider::cVNSIProvider(const cVNSIProvider &provider)
  :m_name(provider.m_name), m_caid(provider.m_caid)
{
}

cVNSIProvider::cVNSIProvider(std::string name, int caid)
  :m_name(name), m_caid(caid)
{
};

bool cVNSIProvider::operator==(const cVNSIProvider &rhs) const
{
  if (rhs.m_caid != m_caid)
    return false;
  if (m_name.empty())
    return false;
  if (rhs.m_name.compare(m_name) != 0)
    return false;
  return true;
}


bool cVNSIChannelFilter::IsRadio(const cChannel* channel)
{
  bool isRadio = false;

  // assume channels without VPID & APID are video channels
  if (channel->Vpid() == 0 && channel->Apid(0) == 0)
    isRadio = false;
  // channels without VPID are radio channels (channels with VPID 1 are encrypted radio channels)
  else if (channel->Vpid() == 0 || channel->Vpid() == 1 || channel->Rid() == 1)
    isRadio = true;

  return isRadio;
}

void cVNSIChannelFilter::Load()
{
  cMutexLock lock(&m_Mutex);

  cString filename;
  std::string line;
  std::ifstream rfile;

  filename = cString::sprintf("%s/videowhitelist.vnsi", *VNSIServerConfig.ConfigDirectory);
  m_providersVideo.clear();
  rfile.open(filename);
  if (rfile.is_open())
  {
    while(std::getline(rfile,line))
    {
      cVNSIProvider provider;
      size_t pos = line.find("|");
      if(pos == line.npos)
      {
        provider.m_name = line;
      }
      else
      {
        provider.m_name.assign(line, 0, pos);
        provider.m_caid = strtol(line.c_str() + pos + 1, nullptr, 10);
      }
      auto p_it = std::find(m_providersVideo.begin(), m_providersVideo.end(), provider);
      if(p_it == m_providersVideo.end())
      {
        m_providersVideo.emplace_back(std::move(provider));
      }
    }
    rfile.close();
  }

  filename = cString::sprintf("%s/radiowhitelist.vnsi", *VNSIServerConfig.ConfigDirectory);
  rfile.open(filename);
  m_providersRadio.clear();
  if (rfile.is_open())
  {
    while(std::getline(rfile,line))
    {
      cVNSIProvider provider;
      auto pos = line.find("|");
      if(pos == line.npos)
      {
        provider.m_name = line;
      }
      else
      {
        provider.m_name.assign(line, 0, pos);
        provider.m_caid = strtol(line.c_str() + pos + 1, nullptr, 10);
      }
      auto p_it = std::find(m_providersRadio.begin(), m_providersRadio.end(), provider);
      if(p_it == m_providersRadio.end())
      {
        m_providersRadio.emplace_back(std::move(provider));
      }
    }
    rfile.close();
  }

  filename = cString::sprintf("%s/videoblacklist.vnsi", *VNSIServerConfig.ConfigDirectory);
  rfile.open(filename);
  m_channelsVideo.clear();
  if (rfile.is_open())
  {
    while(getline(rfile,line))
    {
      int id = strtol(line.c_str(), nullptr, 10);
      m_channelsVideo.insert(id);
    }
    rfile.close();
  }

  filename = cString::sprintf("%s/radioblacklist.vnsi", *VNSIServerConfig.ConfigDirectory);
  rfile.open(filename);
  m_channelsRadio.clear();
  if (rfile.is_open())
  {
    while(getline(rfile,line))
    {
      int id = strtol(line.c_str(), nullptr, 10);
      m_channelsRadio.insert(id);
    }
    rfile.close();
  }
}

void cVNSIChannelFilter::StoreWhitelist(bool radio)
{
  {
    cMutexLock lock(&m_Mutex);

    cString filename;
    std::ofstream wfile;
    std::vector<cVNSIProvider> *whitelist;

    if (radio)
    {
      filename = cString::sprintf("%s/radiowhitelist.vnsi", *VNSIServerConfig.ConfigDirectory);
      whitelist = &m_providersRadio;
    }
    else
    {
      filename = cString::sprintf("%s/videowhitelist.vnsi", *VNSIServerConfig.ConfigDirectory);
      whitelist = &m_providersVideo;
    }

    wfile.open(filename);
    if(wfile.is_open())
    {
      for (const auto i : *whitelist)
      {
        wfile << i.m_name << '|' << i.m_caid << '\n';
      }
      wfile.close();
    }
  }

  SortChannels();
}

void cVNSIChannelFilter::StoreBlacklist(bool radio)
{
  {
    cMutexLock lock(&m_Mutex);

    cString filename;
    std::ofstream wfile;
    std::set<int> *blacklist;

    if (radio)
    {
      filename = cString::sprintf("%s/radioblacklist.vnsi", *VNSIServerConfig.ConfigDirectory);
      blacklist = &m_channelsRadio;
    }
    else
    {
      filename = cString::sprintf("%s/videoblacklist.vnsi", *VNSIServerConfig.ConfigDirectory);
      blacklist = &m_channelsVideo;
    }

    wfile.open(filename);
    if(wfile.is_open())
    {
      for (const auto i : *blacklist)
      {
        wfile << i << '\n';
      }
      wfile.close();
    }
  }

  SortChannels();
}

bool cVNSIChannelFilter::IsWhitelist(const cChannel &channel)
{
  cVNSIProvider provider;
  std::vector<cVNSIProvider> *providers;
  provider.m_name = channel.Provider();

  if (IsRadio(&channel))
    providers = &m_providersRadio;
  else
    providers = &m_providersVideo;

  if(providers->empty())
    return true;

  if (channel.Ca(0) == 0)
  {
    provider.m_caid = 0;
    auto p_it = std::find(providers->begin(), providers->end(), provider);
    if(p_it!=providers->end())
      return true;
    else
      return false;
  }

  int caid;
  int idx = 0;
  while((caid = channel.Ca(idx)) != 0)
  {
    provider.m_caid = caid;
    auto p_it = std::find(providers->begin(), providers->end(), provider);
    if(p_it!=providers->end())
      return true;

    idx++;
  }
  return false;
}

bool cVNSIChannelFilter::PassFilter(const cChannel &channel)
{
  cMutexLock lock(&m_Mutex);

  if(channel.GroupSep())
    return true;

  if (!IsWhitelist(channel))
    return false;

  if (IsRadio(&channel))
  {
    auto it = std::find(m_channelsRadio.begin(), m_channelsRadio.end(), CreateChannelUID(&channel));
    if(it!=m_channelsRadio.end())
      return false;
  }
  else
  {
    auto it = std::find(m_channelsVideo.begin(), m_channelsVideo.end(), CreateChannelUID(&channel));
    if(it!=m_channelsVideo.end())
      return false;
  }

  return true;
}

void cVNSIChannelFilter::SortChannels()
{
#if VDRVERSNUM >= 20301
  LOCK_CHANNELS_WRITE;
  for (cChannel *channel = Channels->First(); channel; channel = Channels->Next(channel))
#else
  Channels.IncBeingEdited();
  Channels.Lock(true);
  for (cChannel *channel = Channels.First(); channel; channel = Channels.Next(channel))
#endif
  {
    if(!PassFilter(*channel))
    {
#if VDRVERSNUM >= 20301
      for (cChannel *whitechan = Channels->Next(channel); whitechan; whitechan = Channels->Next(whitechan))
#else
      for (cChannel *whitechan = Channels.Next(channel); whitechan; whitechan = Channels.Next(whitechan))
#endif
      {
        if(PassFilter(*whitechan))
        {
#if VDRVERSNUM >= 20301
          Channels->Move(whitechan, channel);
#else
          Channels.Move(whitechan, channel);
#endif
          channel = whitechan;
          break;
        }
      }
    }
  }

#if VDRVERSNUM >= 20301
  Channels->SetModifiedByUser();
#else
  Channels.SetModified(true);
  Channels.Unlock();
  Channels.DecBeingEdited();
#endif
}

cVNSIChannelFilter VNSIChannelFilter;
