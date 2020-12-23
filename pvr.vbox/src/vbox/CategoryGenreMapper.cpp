/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "CategoryGenreMapper.h"

#include "Exceptions.h"
#include "Utilities.h"

#include <algorithm>

#include <kodi/General.h>
#include <kodi/addon-instance/pvr/EPG.h>
#include <kodi/tools/StringUtils.h>
#include <lib/tinyxml2/tinyxml2.h>

// using namespace xmltv;
using namespace kodi::tools;
using namespace vbox;
using namespace tinyxml2;

CategoryGenreMapper::CategoryGenreMapper()
{
  m_genreMap["undefined"] = EPG_EVENT_CONTENTMASK_UNDEFINED;
  m_genreMap["movie"] = EPG_EVENT_CONTENTMASK_MOVIEDRAMA;
  m_genreMap["news"] = EPG_EVENT_CONTENTMASK_NEWSCURRENTAFFAIRS;
  m_genreMap["show"] = EPG_EVENT_CONTENTMASK_SHOW;
  m_genreMap["sports"] = EPG_EVENT_CONTENTMASK_SPORTS;
  m_genreMap["children"] = EPG_EVENT_CONTENTMASK_CHILDRENYOUTH;
  m_genreMap["music"] = EPG_EVENT_CONTENTMASK_MUSICBALLETDANCE;
  m_genreMap["arts"] = EPG_EVENT_CONTENTMASK_ARTSCULTURE;
  m_genreMap["documentary"] = EPG_EVENT_CONTENTMASK_SOCIALPOLITICALECONOMICS;
  m_genreMap["educational"] = EPG_EVENT_CONTENTMASK_EDUCATIONALSCIENCE;
  m_genreMap["leisure"] = EPG_EVENT_CONTENTMASK_LEISUREHOBBIES;
  m_genreMap["special"] = EPG_EVENT_CONTENTMASK_SPECIAL;
  m_genreMap["user"] = EPG_EVENT_CONTENTMASK_USERDEFINED;
}

void CategoryGenreMapper::Initialize(const std::string& xmlFileName)
{
  kodi::Log(ADDON_LOG_INFO, "Initializing genre mapper");
  LoadCategoryToGenreXML(xmlFileName);
}

bool CategoryGenreMapper::LoadCategoryToGenreXML(const std::string& xmlFileName)
{
  if (!kodi::vfs::FileExists(xmlFileName, false))
  {
    kodi::Log(ADDON_LOG_INFO, "No Category to Genre mapping XML found");
    return false;
  }
  else
  {
    kodi::Log(ADDON_LOG_INFO, "Found channel mapping file, attempting to load it");
    kodi::vfs::CFile fileHandle;

    if (!fileHandle.OpenFile(xmlFileName, ADDON_READ_NO_CACHE))
    {
      kodi::Log(ADDON_LOG_INFO, "Could not open Category to Genre mapping XML");
      return false;
    }
    // Read the XML
    tinyxml2::XMLDocument document;
    std::unique_ptr<std::string> contents = utilities::ReadFileContents(fileHandle);

    // Try to parse the document
    if (document.Parse(contents->c_str(), contents->size()) != XML_NO_ERROR)
      throw vbox::InvalidXMLException("XML parsing failed: " + std::string(document.ErrorName()));

    // Create mappings
    for (const XMLElement* element = document.RootElement()->FirstChildElement("category"); element != nullptr;
         element = element->NextSiblingElement("category"))
    {
      const char* pGenreAttr = element->Attribute("genre-type");

      if (!pGenreAttr)
        continue;
      m_categoryMap.insert(std::pair<std::string, int>(element->GetText(), m_genreMap[pGenreAttr]));
    }
  }
  return true;
}

static bool UpdateFullCatMatch(std::map<int, int>& rMatchesMap,
                               std::map<int, int>::iterator& rFinalMatch,
                               CategoryMap::iterator& rCatItr,
                               std::string& rCategoryStr)
{
  // return match if category string matches fully (after ignoring case)
  if (!StringUtils::CompareNoCase(rCatItr->first, rCategoryStr))
  {
    // if found - increment counter for that genre type in matches map
    auto match = rMatchesMap.find(rCatItr->second);
    rMatchesMap[rCatItr->second] = (match != rMatchesMap.end()) ? match->second + 1 : 1;
    // set final match to the first match, in the case no dominant genre set is found
    if (rFinalMatch == rMatchesMap.end())
      rFinalMatch = rMatchesMap.find(rCatItr->second);

    if (match != rMatchesMap.end())
      return true;
  }
  return false;
}

// genre-type match counting algorithm taken from Stalker PVR add-on
static void UpdatePartialCatMatch(std::map<int, int>& rMatchesMap,
                                  std::map<int, int>::iterator& rFinalMatch,
                                  CategoryMap::iterator& rCatItr,
                                  std::string& rCategoryStr)
{
  std::string lowerCategoryStr(rCategoryStr);
  std::string xmlCatStr(rCatItr->first);
  StringUtils::ToLower(lowerCategoryStr);
  StringUtils::ToLower(xmlCatStr);

  // return match if categories from the mapping XML exist as substring in the XMLTV's category
  if (strstr(lowerCategoryStr.c_str(), xmlCatStr.c_str()))
  {
    // find the genre match count, if found previously (if not, init as 1)
    auto match = rMatchesMap.find(rCatItr->second);
    rMatchesMap[rCatItr->second] = (match != rMatchesMap.end()) ? match->second + 1 : 1;
    // set final match to the first match, in the case no dominant genre set is found
    if (rFinalMatch == rMatchesMap.end())
      rFinalMatch = rMatchesMap.find(rCatItr->second);
  }
}

// genre-type match counting algorithm taken from Stalker PVR add-on
static void UpdateFinalMatch(std::map<int, int>& rMatchesMap, std::map<int, int>::iterator& rFinalMatch)
{
  // update final match as the match with the maximum counter
  for (std::map<int, int>::iterator match = rMatchesMap.begin(); match != rMatchesMap.end(); ++match)
  {
    // skip undefined tags
    if (match->first == EPG_EVENT_CONTENTMASK_UNDEFINED)
      continue;
    if (match->second > rFinalMatch->second)
      rFinalMatch = match;
  }
}

int CategoryGenreMapper::GetCategoriesGenreType(std::vector<std::string>& categories)
{
  std::map<int, int> matches;
  std::map<int, int>::iterator finalMatch = matches.end();

  // go over every category the programme has
  for (std::vector<std::string>::iterator category = categories.begin(); category != categories.end(); ++category)
  {
    bool fGenreFound = false;

    // find category string (as is) in the XML category map
    std::string categoryString = *category;
    for (auto catMapItr = m_categoryMap.begin(); catMapItr != m_categoryMap.end(); ++catMapItr)
    {
      if (UpdateFullCatMatch(matches, finalMatch, catMapItr, categoryString))
        fGenreFound = true;
    }
    // if no tag was matched - look for a partial match of category sting
    if (!fGenreFound)
    {
      for (auto catMapItr = m_categoryMap.begin(); catMapItr != m_categoryMap.end(); ++catMapItr)
        UpdatePartialCatMatch(matches, finalMatch, catMapItr, categoryString);
    }
  }
  //  if no category matches - use string and return no specific genre
  if (matches.empty())
    return EPG_GENRE_USE_STRING;

  UpdateFinalMatch(matches, finalMatch);

  kodi::Log(ADDON_LOG_DEBUG, "Final match is %d", finalMatch->first);
  return finalMatch->first;
}
