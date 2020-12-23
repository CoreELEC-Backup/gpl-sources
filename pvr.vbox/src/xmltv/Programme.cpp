/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "Programme.h"

#include "Utilities.h"

#include <lib/tinyxml2/tinyxml2.h>

using namespace xmltv;
using namespace tinyxml2;

const std::string Programme::STRING_FORMAT_NOT_SUPPORTED = "String format is not supported";

Programme::Programme(const tinyxml2::XMLElement* xml) : m_year(0)
{
  // Construct a basic event
  m_startTime = xmltv::Utilities::GetStdString(xml->Attribute("start"));
  m_endTime = xmltv::Utilities::GetStdString(xml->Attribute("stop"));
  m_channelName = Utilities::UrlDecode(xmltv::Utilities::GetStdString(xml->Attribute("channel")));

  // Title
  const XMLElement* element = xml->FirstChildElement("title");
  if (element)
    m_title = xmltv::Utilities::GetStdString(element->GetText());
  // Subtitle
  element = xml->FirstChildElement("sub-title");
  if (element)
    m_subTitle = xmltv::Utilities::GetStdString(element->GetText());
  // Description
  element = xml->FirstChildElement("desc");
  if (element)
    m_description = xmltv::Utilities::GetStdString(element->GetText());

  // Credits
  element = xml->FirstChildElement("credits");
  if (element)
    ParseCredits(element);

  // Date
  element = xml->FirstChildElement("date");
  if (element)
    m_year = Utilities::QueryIntText(element);

  // Icon
  element = xml->FirstChildElement("icon");
  if (element)
    m_icon = xmltv::Utilities::GetStdString(element->Attribute("src"));

  // Categories. Skip "movie" and "series" since most people treat categories
  // as genres
  for (element = xml->FirstChildElement("category"); element != NULL; element = element->NextSiblingElement("category"))
  {
    std::string category = xmltv::Utilities::GetStdString(element->GetText());
    if (category.empty())
      continue;

    std::string genre(category);
    if (genre == "movie" || genre == "series")
      continue;

    m_categories.push_back(genre);
  }

  // Star rating
  element = xml->FirstChildElement("star-rating");
  if (element)
  {
    element = element->FirstChildElement("value");
    if (element)
      m_starRating = xmltv::Utilities::GetStdString(element->GetText());
  }

  // series IDs
  for (element = xml->FirstChildElement("episode-num"); element != NULL;
       element = element->NextSiblingElement("episode-num"))
  {
    std::string seriesId = xmltv::Utilities::GetStdString(element->GetText());
    if (seriesId.empty())
      continue;
    std::string systemAttr = xmltv::Utilities::GetStdString(element->Attribute("system"));
    if (systemAttr.empty())
      systemAttr = "xmltv_ns";

    m_seriesIds.insert(std::pair<std::string, std::string>(systemAttr, seriesId));
  }
}

void Programme::ParseCredits(const XMLElement* creditsElement)
{
  // Actors
  for (const XMLElement* element = creditsElement->FirstChildElement("actor"); element != NULL;
       element = element->NextSiblingElement("actor"))
  {
    Actor actor;

    auto* name = element->GetText();
    auto* role = element->Attribute("role");

    if (name)
      actor.name = element->GetText();
    if (role)
      actor.role = role;

    m_credits.actors.push_back(actor);
  }

  // Directors
  for (const XMLElement* element = creditsElement->FirstChildElement("director"); element != NULL;
       element = element->NextSiblingElement("director"))
  {
    auto* director = element->GetText();
    if (director)
      m_credits.directors.push_back(director);
  }

  // Producers
  for (const XMLElement* element = creditsElement->FirstChildElement("producer"); element != NULL;
       element = element->NextSiblingElement("producer"))
  {
    auto* producer = element->GetText();
    if (producer)
      m_credits.producers.push_back(producer);
  }

  // Writers
  for (const XMLElement* element = creditsElement->FirstChildElement("writer"); element != NULL;
       element = element->NextSiblingElement("writer"))
  {
    auto* writer = element->GetText();
    if (writer)
      m_credits.writers.push_back(writer);
  }
}
