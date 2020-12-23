/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "Epg.h"

#include "../Enigma2.h"
#include "utilities/Logger.h"
#include "utilities/WebUtils.h"
#include "utilities/XMLUtils.h"

#include <chrono>
#include <cmath>
#include <regex>

#include <kodi/tools/StringUtils.h>
#include <nlohmann/json.hpp>

using namespace enigma2;
using namespace enigma2::data;
using namespace enigma2::extract;
using namespace enigma2::utilities;
using namespace kodi::tools;
using json = nlohmann::json;

Epg::Epg(IConnectionListener& connectionListener, enigma2::extract::EpgEntryExtractor& entryExtractor, int epgMaxDays)
      : m_connectionListener(connectionListener), m_entryExtractor(entryExtractor), m_epgMaxDays(epgMaxDays) {}

Epg::Epg(const Epg& epg) : m_connectionListener(epg.m_connectionListener), m_entryExtractor(epg.m_entryExtractor) {}

bool Epg::Initialise(enigma2::Channels& channels, enigma2::ChannelGroups& channelGroups)
{
  SetEPGTimeFrame(m_epgMaxDays);

  auto started = std::chrono::high_resolution_clock::now();
  Logger::Log(LEVEL_DEBUG, "%s Initial EPG Load Start", __func__);

  //clear current data structures
  m_epgChannels.clear();
  m_epgChannelsMap.clear();
  m_readInitialEpgChannelsMap.clear();
  m_needsInitialEpgChannelsMap.clear();
  m_initialEpgReady = false;

  //add an initial EPG data per channel uId, sref and initial EPG
  for (auto& channel : channels.GetChannelsList())
  {
    std::shared_ptr<data::EpgChannel> newEpgChannel = std::make_shared<EpgChannel>();

    newEpgChannel->SetRadio(channel->IsRadio());
    newEpgChannel->SetUniqueId(channel->GetUniqueId());
    newEpgChannel->SetChannelName(channel->GetChannelName());
    newEpgChannel->SetServiceReference(channel->GetServiceReference());

    m_epgChannels.emplace_back(newEpgChannel);

    m_epgChannelsMap.insert({newEpgChannel->GetServiceReference(), newEpgChannel});
    m_readInitialEpgChannelsMap.insert({newEpgChannel->GetServiceReference(), newEpgChannel});
    m_needsInitialEpgChannelsMap.insert({newEpgChannel->GetServiceReference(), newEpgChannel});
  }

  int lastScannedIgnoreSuccessCount = std::round((1 - LAST_SCANNED_INITIAL_EPG_SUCCESS_PERCENT) * m_epgChannels.size());

  std::vector<std::shared_ptr<ChannelGroup>> groupList;

  std::shared_ptr<ChannelGroup> newChannelGroup = std::make_shared<ChannelGroup>();
  newChannelGroup->SetRadio(false);
  newChannelGroup->SetGroupName("Last Scanned"); //Name not important
  newChannelGroup->SetServiceReference("1:7:1:0:0:0:0:0:0:0:FROM BOUQUET \"userbouquet.LastScanned.tv\" ORDER BY bouquet");
  newChannelGroup->SetLastScannedGroup(true);

  groupList.emplace_back(newChannelGroup);
  for (auto& group : channelGroups.GetChannelGroupsList())
  {
    if (!group->IsLastScannedGroup())
      groupList.emplace_back(group);
  }

  //load each group and if we don't already have it's intial EPG then load those entries
  for (auto& group : groupList)
  {
    LoadInitialEPGForGroup(group);

    //Remove channels that now have an initial EPG
    for (auto& epgChannel : m_epgChannels)
    {
      if (epgChannel->GetInitialEPG().size() > 0)
        InitialEpgLoadedForChannel(epgChannel->GetServiceReference());
    }

    Logger::Log(LEVEL_DEBUG, "%s Initial EPG Progress - Remaining channels %d, Min Channels for completion %d", __func__, m_needsInitialEpgChannelsMap.size(), lastScannedIgnoreSuccessCount);

    for (auto pair : m_needsInitialEpgChannelsMap)
      Logger::Log(LEVEL_DEBUG, "%s - Initial EPG Progress - Remaining channel: %s - sref: %s", __func__, pair.second->GetChannelName().c_str(), pair.first.c_str());

    if (group->IsLastScannedGroup() && m_needsInitialEpgChannelsMap.size() <= lastScannedIgnoreSuccessCount)
      break;
  }

  int milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - started).count();

  Logger::Log(LEVEL_INFO, "%s Initial EPG Loaded - %d (ms)", __func__, milliseconds);

  m_initialEpgReady = true;

  return true;
}

std::shared_ptr<data::EpgChannel> Epg::GetEpgChannel(const std::string& serviceReference)
{
  std::shared_ptr<data::EpgChannel> epgChannel = std::make_shared<data::EpgChannel>();

  auto epgChannelSearch = m_epgChannelsMap.find(serviceReference);
  if (epgChannelSearch != m_epgChannelsMap.end())
    epgChannel = epgChannelSearch->second;

  return epgChannel;
}

std::shared_ptr<data::EpgChannel> Epg::GetEpgChannelNeedingInitialEpg(const std::string& serviceReference)
{
  std::shared_ptr<data::EpgChannel> epgChannel = std::make_shared<data::EpgChannel>();

  auto initialEpgChannelSearch = m_needsInitialEpgChannelsMap.find(serviceReference);
  if (initialEpgChannelSearch != m_needsInitialEpgChannelsMap.end())
    epgChannel = initialEpgChannelSearch->second;

  return epgChannel;
}

bool Epg::ChannelNeedsInitialEpg(const std::string& serviceReference)
{
  auto needsInitialEpgSearch = m_needsInitialEpgChannelsMap.find(serviceReference);

  return needsInitialEpgSearch != m_needsInitialEpgChannelsMap.end();
}

bool Epg::InitialEpgLoadedForChannel(const std::string& serviceReference)
{
  return m_needsInitialEpgChannelsMap.erase(serviceReference) == 1;
}

bool Epg::IsInitialEpgCompleted()
{
  Logger::Log(LEVEL_DEBUG, "%s Waiting to Get Initial EPG for %d remaining channels", __func__, m_readInitialEpgChannelsMap.size());

  return m_readInitialEpgChannelsMap.size() == 0;
}

void Epg::TriggerEpgUpdatesForChannels()
{
  for (auto& epgChannel : m_epgChannels)
  {
    //We want to trigger full updates only so let's make sure it's not an initialEpg
    if (epgChannel->RequiresInitialEpg())
    {
      epgChannel->SetRequiresInitialEpg(false);
      epgChannel->GetInitialEPG().clear();
      m_readInitialEpgChannelsMap.erase(epgChannel->GetServiceReference());
    }

    Logger::Log(LEVEL_DEBUG, "%s - Trigger EPG update for channel: %s (%d)", __func__, epgChannel->GetChannelName().c_str(), epgChannel->GetUniqueId());
    m_connectionListener.TriggerEpgUpdate(epgChannel->GetUniqueId());
  }
}

void Epg::MarkChannelAsInitialEpgRead(const std::string& serviceReference)
{
  std::shared_ptr<data::EpgChannel> epgChannel = GetEpgChannel(serviceReference);

  if (epgChannel->RequiresInitialEpg())
  {
    epgChannel->SetRequiresInitialEpg(false);
    epgChannel->GetInitialEPG().clear();
    m_readInitialEpgChannelsMap.erase(epgChannel->GetServiceReference());
  }
}

PVR_ERROR Epg::GetEPGForChannel(const std::string& serviceReference, time_t start, time_t end, kodi::addon::PVREPGTagsResultSet& results)
{
  std::shared_ptr<data::EpgChannel> epgChannel = GetEpgChannel(serviceReference);

  if (epgChannel)
  {
    Logger::Log(LEVEL_DEBUG, "%s Getting EPG for channel '%s'", __func__, epgChannel->GetChannelName().c_str());

    if (epgChannel->RequiresInitialEpg())
    {
      epgChannel->SetRequiresInitialEpg(false);

      return TransferInitialEPGForChannel(results, epgChannel, start, end);
    }

    const std::string url = StringUtils::Format("%s%s%s", Settings::GetInstance().GetConnectionURL().c_str(),
                                                "web/epgservice?sRef=", WebUtils::URLEncodeInline(serviceReference).c_str());

    const std::string strXML = WebUtils::GetHttpXML(url);

    int iNumEPG = 0;

    TiXmlDocument xmlDoc;
    if (!xmlDoc.Parse(strXML.c_str()))
    {
      Logger::Log(LEVEL_ERROR, "%s Unable to parse XML: %s at line %d", __func__, xmlDoc.ErrorDesc(), xmlDoc.ErrorRow());
      return PVR_ERROR_SERVER_ERROR;
    }

    TiXmlHandle hDoc(&xmlDoc);

    TiXmlElement* pElem = hDoc.FirstChildElement("e2eventlist").Element();

    if (!pElem)
    {
      Logger::Log(LEVEL_WARNING, "%s could not find <e2eventlist> element for channel: %s", __func__, epgChannel->GetChannelName().c_str());
      // Return "NO_ERROR" as the EPG could be empty for this channel
      return PVR_ERROR_NO_ERROR;
    }

    TiXmlHandle hRoot = TiXmlHandle(pElem);

    TiXmlElement* pNode = hRoot.FirstChildElement("e2event").Element();

    if (!pNode)
    {
      Logger::Log(LEVEL_WARNING, "%s Could not find <e2event> element for channel: %s", __func__, epgChannel->GetChannelName().c_str());
      // RETURN "NO_ERROR" as the EPG could be empty for this channel
      return PVR_ERROR_NO_ERROR;
    }

    for (; pNode != nullptr; pNode = pNode->NextSiblingElement("e2event"))
    {
      EpgEntry entry;

      if (!entry.UpdateFrom(pNode, epgChannel, start, end))
        continue;

      if (m_entryExtractor.IsEnabled())
        m_entryExtractor.ExtractFromEntry(entry);

      kodi::addon::PVREPGTag broadcast;

      entry.UpdateTo(broadcast);

      results.Add(broadcast);

      iNumEPG++;

      Logger::Log(LEVEL_TRACE, "%s loaded EPG entry '%d:%s' channel '%d' start '%d' end '%d'", __func__, broadcast.GetUniqueBroadcastId(), broadcast.GetTitle().c_str(), entry.GetChannelId(), entry.GetStartTime(), entry.GetEndTime());
    }

    iNumEPG += TransferTimerBasedEntries(results, epgChannel->GetUniqueId());

    Logger::Log(LEVEL_DEBUG, "%s Loaded %u EPG Entries for channel '%s'", __func__, iNumEPG, epgChannel->GetChannelName().c_str());
  }
  else
  {
    Logger::Log(LEVEL_DEBUG, "%s EPG requested for unknown channel reference: '%s'", __func__, serviceReference.c_str());
  }

  return PVR_ERROR_NO_ERROR;
}

void Epg::SetEPGTimeFrame(int epgMaxDays)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  m_epgMaxDays = epgMaxDays;

  if (m_epgMaxDays > 0)
    m_epgMaxDaysSeconds = m_epgMaxDays * 24 * 60 * 60;
  else
    m_epgMaxDaysSeconds = DEFAULT_EPG_MAX_DAYS * 24 * 60 * 60;
}

PVR_ERROR Epg::TransferInitialEPGForChannel(kodi::addon::PVREPGTagsResultSet& results, const std::shared_ptr<EpgChannel>& epgChannel, time_t iStart, time_t iEnd)
{
  for (const auto& entry : epgChannel->GetInitialEPG())
  {
    kodi::addon::PVREPGTag broadcast;

    entry.UpdateTo(broadcast);

    results.Add(broadcast);
  }

  epgChannel->GetInitialEPG().clear();
  m_readInitialEpgChannelsMap.erase(epgChannel->GetServiceReference());

  TransferTimerBasedEntries(results, epgChannel->GetUniqueId());

  return PVR_ERROR_NO_ERROR;
}

std::string Epg::LoadEPGEntryShortDescription(const std::string& serviceReference, unsigned int epgUid)
{
  std::string shortDescription;

  const std::string jsonUrl = StringUtils::Format("%sapi/event?sref=%s&idev=%u", Settings::GetInstance().GetConnectionURL().c_str(),
                                                  WebUtils::URLEncodeInline(serviceReference).c_str(), epgUid);

  const std::string strJson = WebUtils::GetHttpXML(jsonUrl);

  try
  {
    auto jsonDoc = json::parse(strJson);

    if (!jsonDoc["event"].empty())
    {
      for (const auto& element : jsonDoc["event"].items())
      {
        if (element.key() == "shortdesc")
        {
          Logger::Log(LEVEL_DEBUG, "%s Loaded EPG event short description for sref: %s, epgId: %u - '%s'", __func__, serviceReference.c_str(), epgUid, element.value().get<std::string>().c_str());
          shortDescription = element.value().get<std::string>();
        }
      }
    }
  }
  catch (nlohmann::detail::parse_error& e)
  {
    Logger::Log(LEVEL_ERROR, "%s Invalid JSON received, cannot load short descrption from OpenWebIf for sref: %s, epgId: %u - JSON parse error - message: %s, exception id: %d", __func__, serviceReference.c_str(), epgUid, e.what(), e.id);
  }
  catch (nlohmann::detail::type_error& e)
  {
    Logger::Log(LEVEL_ERROR, "%s JSON type error - message: %s, exception id: %d", __func__, e.what(), e.id);
  }

  return shortDescription;
}

EpgPartialEntry Epg::LoadEPGEntryPartialDetails(const std::string& serviceReference, unsigned int epgUid)
{
  EpgPartialEntry partialEntry;

  Logger::Log(LEVEL_DEBUG, "%s Looking for EPG event partial details for sref: %s, epgUid: %u", __func__, serviceReference.c_str(), epgUid);

  const std::string jsonUrl = StringUtils::Format("%sapi/event?sref=%s&idev=%u", Settings::GetInstance().GetConnectionURL().c_str(),
                                                  WebUtils::URLEncodeInline(serviceReference).c_str(), epgUid);

  const std::string strJson = WebUtils::GetHttpXML(jsonUrl);

  try
  {
    auto jsonDoc = json::parse(strJson);

    if (!jsonDoc["event"].empty())
    {
      for (const auto& element : jsonDoc["event"].items())
      {
        if (element.key() == "shortdesc")
          partialEntry.SetPlotOutline(element.value().get<std::string>());
        if (element.key() == "longdesc")
          partialEntry.SetPlot(element.value().get<std::string>());
        else if (element.key() == "title")
          partialEntry.SetTitle(element.value().get<std::string>());
        else if (element.key() == "id")
          partialEntry.SetEpgUid(element.value().get<unsigned int>());
        else if (element.key() == "genreid")
        {
          int genreId = element.value().get<int>();
          partialEntry.SetGenreType(genreId & 0xF0);
          partialEntry.SetGenreSubType(genreId & 0x0F);
        }
      }

      if (partialEntry.EntryFound())
      {
        Logger::Log(LEVEL_DEBUG, "%s Loaded EPG event partial details for sref: %s, epgId: %u - title: %s - '%s'", __func__, serviceReference.c_str(), epgUid, partialEntry.GetTitle().c_str(), partialEntry.GetPlotOutline().c_str());
      }
    }
  }
  catch (nlohmann::detail::parse_error& e)
  {
    Logger::Log(LEVEL_ERROR, "%s Invalid JSON received, cannot event details from OpenWebIf for sref: %s, epgId: %u - JSON parse error - message: %s, exception id: %d", __func__, serviceReference.c_str(), epgUid, e.what(), e.id);
  }
  catch (nlohmann::detail::type_error& e)
  {
    Logger::Log(LEVEL_ERROR, "%s JSON type error - message: %s, exception id: %d", __func__, e.what(), e.id);
  }

  return partialEntry;
}

EpgPartialEntry Epg::LoadEPGEntryPartialDetails(const std::string& serviceReference, time_t startTime)
{
  EpgPartialEntry partialEntry;

  Logger::Log(LEVEL_DEBUG, "%s Looking for EPG event partial details for sref: %s, time: %lld", __func__, serviceReference.c_str(), static_cast<long long>(startTime));

  const std::string jsonUrl = StringUtils::Format("%sapi/epgservice?sRef=%s&time=%lld&endTime=1", Settings::GetInstance().GetConnectionURL().c_str(), WebUtils::URLEncodeInline(serviceReference).c_str(), static_cast<long long>(startTime));

  const std::string strJson = WebUtils::GetHttpXML(jsonUrl);

  try
  {
    auto jsonDoc = json::parse(strJson);

    if (!jsonDoc["events"].empty())
    {
      for (const auto& event : jsonDoc["events"].items())
      {
        for (const auto& element : event.value().items())
        {
          if (element.key() == "shortdesc")
            partialEntry.SetPlotOutline(element.value().get<std::string>());
          if (element.key() == "longdesc")
            partialEntry.SetPlot(element.value().get<std::string>());
          else if (element.key() == "title")
            partialEntry.SetTitle(element.value().get<std::string>());
          else if (element.key() == "id")
            partialEntry.SetEpgUid(element.value().get<unsigned int>());
          else if (element.key() == "genreid")
          {
            int genreId = element.value().get<int>();
            partialEntry.SetGenreType(genreId & 0xF0);
            partialEntry.SetGenreSubType(genreId & 0x0F);
          }
        }

        if (partialEntry.EntryFound())
          Logger::Log(LEVEL_DEBUG, "%s Loaded EPG event partial details for sref: %s, time: %lld - title: %s, epgId: %u - '%s'", __func__, serviceReference.c_str(), static_cast<long long>(startTime), partialEntry.GetTitle().c_str(), partialEntry.GetEpgUid(), partialEntry.GetPlotOutline().c_str());

        break; //We only want first event
      }
    }
  }
  catch (nlohmann::detail::parse_error& e)
  {
    Logger::Log(LEVEL_ERROR, "%s Invalid JSON received, cannot event details from OpenWebIf for sref: %s, time: %lld - JSON parse error - message: %s, exception id: %d", __func__, serviceReference.c_str(), static_cast<long long>(startTime), e.what(), e.id);
  }
  catch (nlohmann::detail::type_error& e)
  {
    Logger::Log(LEVEL_ERROR, "%s JSON type error - message: %s, exception id: %d", __func__, e.what(), e.id);
  }

  return partialEntry;
}

std::string Epg::FindServiceReference(const std::string& title, int epgUid, time_t startTime, time_t endTime) const
{
  std::string serviceReference;

  const auto started = std::chrono::high_resolution_clock::now();

  const std::string jsonUrl = StringUtils::Format("%sapi/epgsearch?search=%s&endtime=%lld", Settings::GetInstance().GetConnectionURL().c_str(), WebUtils::URLEncodeInline(title).c_str(), static_cast<long long>(endTime));

  const std::string strJson = WebUtils::GetHttpXML(jsonUrl);

  try
  {
    auto jsonDoc = json::parse(strJson);

    if (!jsonDoc["events"].empty())
    {
      for (const auto& event : jsonDoc["events"].items())
      {
        if (event.value()["title"].get<std::string>() == title &&
            event.value()["id"].get<int>() == epgUid &&
            event.value()["begin_timestamp"].get<time_t>() == startTime &&
            event.value()["duration_sec"].get<int>() == (endTime - startTime))
        {
          serviceReference = Channel::NormaliseServiceReference(event.value()["sref"].get<std::string>());

          break; //We only want first event
        }
      }
    }
  }
  catch (nlohmann::detail::parse_error& e)
  {
    Logger::Log(LEVEL_ERROR, "%s Invalid JSON received, cannot retrieve service reference from OpenWebIf for: %s, epgUid: %d, start time: %lld, end time: %lld  - JSON parse error - message: %s, exception id: %d", __func__, title.c_str(), epgUid, static_cast<long long>(startTime), static_cast<long long>(endTime), e.what(), e.id);
  }
  catch (nlohmann::detail::type_error& e)
  {
    Logger::Log(LEVEL_ERROR, "%s JSON type error - message: %s, exception id: %d", __func__, e.what(), e.id);
  }

  int milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - started).count();

  Logger::Log(LEVEL_DEBUG, "%s Service reference search time - %d (ms)", __func__, milliseconds);

  return serviceReference;
}

bool Epg::LoadInitialEPGForGroup(const std::shared_ptr<ChannelGroup> group)
{
  const std::string url = StringUtils::Format("%s%s%s", Settings::GetInstance().GetConnectionURL().c_str(),
                                              "web/epgnownext?bRef=", WebUtils::URLEncodeInline(group->GetServiceReference()).c_str());

  const std::string strXML = WebUtils::GetHttpXML(url);

  int iNumEPG = 0;

  TiXmlDocument xmlDoc;
  if (!xmlDoc.Parse(strXML.c_str()))
  {
    Logger::Log(LEVEL_ERROR, "%s Unable to parse XML: %s at line %d", __func__, xmlDoc.ErrorDesc(), xmlDoc.ErrorRow());
    return false;
  }

  TiXmlHandle hDoc(&xmlDoc);

  TiXmlElement* pElem = hDoc.FirstChildElement("e2eventlist").Element();

  if (!pElem)
  {
    Logger::Log(LEVEL_INFO, "%s could not find <e2eventlist> element!", __func__);
    // Return "NO_ERROR" as the EPG could be empty for this channel
    return false;
  }

  TiXmlHandle hRoot = TiXmlHandle(pElem);

  TiXmlElement* pNode = hRoot.FirstChildElement("e2event").Element();

  if (!pNode)
  {
    Logger::Log(LEVEL_DEBUG, "%s Could not find <e2event> element", __func__);
    // RETURN "NO_ERROR" as the EPG could be empty for this channel
    return false;
  }

  for (; pNode != nullptr; pNode = pNode->NextSiblingElement("e2event"))
  {
    EpgEntry entry;

    if (!entry.UpdateFrom(pNode, m_needsInitialEpgChannelsMap))
      continue;

    std::shared_ptr<data::EpgChannel> epgChannel = GetEpgChannelNeedingInitialEpg(entry.GetServiceReference());

    if (m_entryExtractor.IsEnabled())
      m_entryExtractor.ExtractFromEntry(entry);

    iNumEPG++;

    epgChannel->GetInitialEPG().emplace_back(entry);
    Logger::Log(LEVEL_TRACE, "%s Added Initial EPG Entry for: %s, %d, %s", __func__, epgChannel->GetChannelName().c_str(), epgChannel->GetUniqueId(), epgChannel->GetServiceReference().c_str());
  }

  Logger::Log(LEVEL_INFO, "%s Loaded %u EPG Entries for group '%s'", __func__, iNumEPG, group->GetGroupName().c_str());
  return true;
}

void Epg::UpdateTimerEPGFallbackEntries(const std::vector<enigma2::data::EpgEntry>& timerBasedEntries)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  time_t now = std::time(nullptr);
  time_t until = now + m_epgMaxDaysSeconds;

  m_timerBasedEntries.clear();

  for (auto& timerBasedEntry : timerBasedEntries)
  {
    if (timerBasedEntry.GetEndTime() < now || timerBasedEntry.GetEndTime() > until)
      m_timerBasedEntries.emplace_back(timerBasedEntry);
  }
}

int Epg::TransferTimerBasedEntries(kodi::addon::PVREPGTagsResultSet& results, int epgChannelId)
{
  int numTransferred = 0;

  std::lock_guard<std::mutex> lock(m_mutex);
  for (auto& timerBasedEntry : m_timerBasedEntries)
  {
    if (epgChannelId == timerBasedEntry.GetChannelId())
    {
      kodi::addon::PVREPGTag broadcast;

      timerBasedEntry.UpdateTo(broadcast);

      results.Add(broadcast);

      numTransferred++;
    }
  }

  return numTransferred;
}
