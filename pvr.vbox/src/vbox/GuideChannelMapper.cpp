/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "GuideChannelMapper.h"

#include "Exceptions.h"
#include "Utilities.h"

#include <memory>

#include <lib/tinyxml2/tinyxml2.h>

using namespace vbox;
using namespace tinyxml2;

const std::string GuideChannelMapper::MAPPING_FILE_PATH = "special://userdata/addon_data/pvr.vbox/channel_mappings.xml";

GuideChannelMapper::GuideChannelMapper(const ::xmltv::Guide& vboxGuide, const ::xmltv::Guide& externalGuide)
  : m_vboxGuide(vboxGuide), m_externalGuide(externalGuide)
{
}

void GuideChannelMapper::Initialize()
{
  kodi::Log(ADDON_LOG_INFO, "Initializing channel mapper with default mappings");

  // Generate default mappings
  m_channelMappings = CreateDefaultMappings();

  // Create a default mapping file if none exists, otherwise load it
  if (!kodi::vfs::FileExists(MAPPING_FILE_PATH, false))
  {
    kodi::Log(ADDON_LOG_INFO, "No external XMLTV channel mapping file found, saving default mappings");
    Save();
  }
  else
  {
    kodi::Log(ADDON_LOG_INFO, "Found channel mapping file, attempting to load it");
    Load();
  }
}

ChannelMappings GuideChannelMapper::CreateDefaultMappings()
{
  ChannelMappings mappings;
  std::vector<std::string> channelNames = m_vboxGuide.GetChannelNames();

  // Add a mapping for every channel which display names matches, otherwise
  // leave it empty
  for (const std::string& channelName : channelNames)
  {
    if (!m_externalGuide.GetChannelId(channelName).empty())
      mappings[channelName] = channelName;
    else
      mappings[channelName] = "";
  }

  return mappings;
}

void GuideChannelMapper::Load()
{
  kodi::vfs::CFile fileHandle;

  if (fileHandle.OpenFile(MAPPING_FILE_PATH, ADDON_READ_NO_CACHE))
  {
    // Read the XML
    tinyxml2::XMLDocument document;
    std::unique_ptr<std::string> contents = utilities::ReadFileContents(fileHandle);

    // Try to parse the document
    if (document.Parse(contents->c_str(), contents->size()) != XML_NO_ERROR)
      throw vbox::InvalidXMLException("XML parsing failed: " + std::string(document.ErrorName()));

    // Create mappings
    for (const XMLElement* element = document.RootElement()->FirstChildElement("mapping"); element != nullptr;
         element = element->NextSiblingElement("mapping"))
    {
      const std::string vboxName = element->Attribute("vbox-name");
      const std::string xmltvName = element->Attribute("xmltv-name");

      m_channelMappings[vboxName] = xmltvName;
    }
  }
}

void GuideChannelMapper::Save()
{
  // Create the document
  tinyxml2::XMLDocument document;
  XMLDeclaration* declaration = document.NewDeclaration();
  document.InsertEndChild(declaration);

  // Create the root node (<xmltvmap>)
  XMLElement* rootElement = document.NewElement("xmltvmap");
  document.InsertEndChild(rootElement);

  // Create one <mapping> for every channel
  for (const auto& mapping : m_channelMappings)
  {
    XMLElement* mappingElement = document.NewElement("mapping");
    mappingElement->SetAttribute("vbox-name", mapping.first.c_str());
    mappingElement->SetAttribute("xmltv-name", mapping.second.c_str());

    rootElement->InsertEndChild(mappingElement);
  }

  // Save the file
  kodi::vfs::CFile fileHandle;

  if (fileHandle.OpenFileForWrite(MAPPING_FILE_PATH, false))
  {
    XMLPrinter printer;
    document.Accept(&printer);

    std::string xml = printer.CStr();
    fileHandle.Write(xml.c_str(), xml.length());
  }
}

std::string GuideChannelMapper::GetExternalChannelName(const std::string& vboxName) const
{
  auto it = m_channelMappings.find(vboxName);

  return it != m_channelMappings.cend() ? it->second : "";
}
