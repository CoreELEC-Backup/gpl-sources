/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "N7Xml.h"

#include <kodi/Filesystem.h>
#include <tinyxml.h>

bool CCurlFile::Get(const std::string& strURL, std::string& strResult)
{
  kodi::vfs::CFile fileHandle;
  if (fileHandle.OpenFile(strURL, 0))
  {
    std::string buffer;
    while (fileHandle.ReadLine(buffer))
      strResult.append(buffer);
    return true;
  }
  return false;
}

N7Xml::N7Xml()
{
  kodi::Log(ADDON_LOG_DEBUG, "Creating N7 PVR-Client");

  m_strHostname = kodi::GetSettingString("n7host", DEFAULT_HOST);
  m_iPort = kodi::GetSettingInt("n7port", DEFAULT_PORT);

  list_channels();
}

ADDON_STATUS N7Xml::SetSetting(const std::string& settingName,
                               const kodi::CSettingValue& settingValue)
{
  if (settingName == "n7host")
  {
    std::string tmp_sHostname;
    kodi::Log(ADDON_LOG_INFO, "Changed Setting 'host' from %s to %s", m_strHostname.c_str(),
              settingValue.GetString().c_str());
    tmp_sHostname = m_strHostname;
    m_strHostname = settingValue.GetString();
    if (tmp_sHostname != m_strHostname)
      return ADDON_STATUS_NEED_RESTART;
  }
  else if (settingName == "n7port")
  {
    kodi::Log(ADDON_LOG_INFO, "Changed Setting 'port' from %u to %u", m_iPort,
              settingValue.GetInt());
    if (m_iPort != settingValue.GetInt())
    {
      m_iPort = settingValue.GetInt();
      return ADDON_STATUS_NEED_RESTART;
    }
  }
  return ADDON_STATUS_OK;
}

PVR_ERROR N7Xml::GetCapabilities(kodi::addon::PVRCapabilities& capabilities)
{
  capabilities.SetSupportsTV(true);
  capabilities.SetSupportsRecordings(false);
  capabilities.SetSupportsRecordingsRename(false);
  capabilities.SetSupportsRecordingsLifetimeChange(false);
  capabilities.SetSupportsDescrambleInfo(false);

  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR N7Xml::GetBackendName(std::string& name)
{
  name = "NJoy N7";
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR N7Xml::GetBackendVersion(std::string& version)
{
  version = "-";
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR N7Xml::GetConnectionString(std::string& connection)
{
  connection = "connected";
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR N7Xml::GetBackendHostname(std::string& hostname)
{
  hostname = m_strHostname;
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR N7Xml::GetChannelsAmount(int& amount)
{
  amount = m_channels.size();
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR N7Xml::GetChannels(bool radio, kodi::addon::PVRChannelsResultSet& results)
{
  if (m_connected)
  {
    for (const auto& channel : m_channels)
    {
      kodi::Log(ADDON_LOG_DEBUG, "N7Xml - Loaded channel - %s.",
                channel.second.GetChannelName().c_str());
      results.Add(channel.second);
    }
  }
  else
  {
    kodi::Log(ADDON_LOG_DEBUG, "N7Xml - no channels loaded");
  }

  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR N7Xml::GetChannelStreamProperties(const kodi::addon::PVRChannel& channel,
                                            std::vector<kodi::addon::PVRStreamProperty>& properties)
{
  for (const auto& chan : m_channels)
  {
    if (chan.second.GetUniqueId() == channel.GetUniqueId())
    {
      properties.emplace_back(PVR_STREAM_PROPERTY_STREAMURL, chan.first);
      properties.emplace_back(PVR_STREAM_PROPERTY_ISREALTIMESTREAM, "true");
      return PVR_ERROR_NO_ERROR;
    }
  }

  return PVR_ERROR_UNKNOWN;
}

void N7Xml::list_channels()
{
  std::string strUrl =
      "http://" + m_strHostname + ":" + std::to_string(m_iPort) + "/n7channel_nt.xml";
  std::string strXML;

  CCurlFile http;
  if (!http.Get(strUrl, strXML))
  {
    kodi::Log(ADDON_LOG_DEBUG, "N7Xml - Could not open connection to N7 backend.");
    kodi::addon::CInstancePVRClient::ConnectionStateChange("", PVR_CONNECTION_STATE_SERVER_UNREACHABLE, "");
  }
  else
  {
    TiXmlDocument xml;
    xml.Parse(strXML.c_str());
    TiXmlElement* rootXmlNode = xml.RootElement();
    if (rootXmlNode == nullptr)
      return;
    TiXmlElement* channelsNode = rootXmlNode->FirstChildElement("channel");
    if (channelsNode)
    {
      kodi::Log(ADDON_LOG_DEBUG, "N7Xml - Connected to N7 backend.");
      m_connected = true;
      int iUniqueChannelId = 0;
      TiXmlNode* pChannelNode = nullptr;
      while ((pChannelNode = channelsNode->IterateChildren(pChannelNode)) != nullptr)
      {
        kodi::addon::PVRChannel channel;

        /* unique ID */
        channel.SetUniqueId(++iUniqueChannelId);

        /* channel number */
        const TiXmlNode* pNode = pChannelNode->FirstChild("number");
        if (pNode && pNode->FirstChild())
          channel.SetChannelNumber(atoi(pNode->FirstChild()->Value()));
        else
          channel.SetChannelNumber(channel.GetUniqueId());

        /* channel name */
        const TiXmlElement* pElement = pChannelNode->FirstChildElement("title");
        if (pElement && (pNode = pElement->FirstChild()))
          channel.SetChannelName(pNode->Value());
        else
          continue;

        /* icon path */
        pElement = pChannelNode->FirstChildElement("media:thumbnail");
        channel.SetIconPath(pElement->Attribute("url"));

        /* channel url */
        std::string url;
        pElement = pChannelNode->FirstChildElement("media:content");
        if (pElement)
        {
          url = pElement->Attribute("url");
          channel.SetMimeType(pElement->Attribute("type"));
        }

        if (url.empty())
        {
          pElement = pChannelNode->FirstChildElement("guid");
          if (pElement && (pNode = pElement->FirstChild()))
            url = pNode->Value();
        }

        m_channels.emplace_back(url, channel);
      }
    }

    kodi::addon::CInstancePVRClient::ConnectionStateChange("", PVR_CONNECTION_STATE_CONNECTED, "");
  }
}

ADDONCREATOR(N7Xml)
