/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

// Forward declarations
namespace tinyxml2
{
  class XMLElement;
}

namespace xmltv
{

  class Programme;
  typedef std::shared_ptr<Programme> ProgrammePtr;
  typedef std::map<std::string, std::string> SeriesIDMap;
  /**
   * Represents an actor
   */
  struct Actor
  {
    std::string role;
    std::string name;
  };

  /**
   * Represents the credits for a programme (actors, director etc.)
   */
  struct Credits
  {
    std::vector<std::string> directors;
    std::vector<Actor> actors;
    std::vector<std::string> producers;
    std::vector<std::string> writers;
  };

  /**
   * Represents a single programme/event
   */
  class Programme
  {
  public:
    /**
     * Title used by programmes where the VBox cannot figure out the character encoding
     */
    static const std::string STRING_FORMAT_NOT_SUPPORTED;

    /**
     * Creates a programme from the specified <programme> element
     */
    Programme(const tinyxml2::XMLElement* xml);
    virtual ~Programme() = default;

    const std::vector<std::string>& GetDirectors() const { return m_credits.directors; }

    const std::vector<Actor>& GetActors() const { return m_credits.actors; }

    const std::vector<std::string>& GetProducers() const { return m_credits.producers; }

    const std::vector<std::string>& GetWriters() const { return m_credits.writers; }

    const std::vector<std::string>& GetCategories() const { return m_categories; }

    std::string m_startTime;
    std::string m_endTime;
    std::string m_channelName;
    std::string m_title;
    std::string m_description;
    std::string m_icon;
    std::string m_subTitle;
    SeriesIDMap m_seriesIds;
    int m_year;
    std::string m_starRating;

  private:
    /**
     * Parses the credits from the specified <credits> element
     */
    void ParseCredits(const tinyxml2::XMLElement* creditsElement);

    Credits m_credits;
    std::vector<std::string> m_categories;
  };
} // namespace xmltv
