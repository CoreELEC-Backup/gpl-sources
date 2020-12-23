/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "../../xmltv/Guide.h"
#include "../Channel.h"
#include "../Recording.h"
#include "../SeriesRecording.h"

#include <string>
#include <vector>

namespace tinyxml2
{
  // Forward declarations
  class XMLElement;
} // namespace tinyxml2

namespace vbox
{
  namespace response
  {

    /**
     * The base class for all response contents
     */
    class Content
    {
    public:
      Content(tinyxml2::XMLElement* content) : m_content(content) {}
      virtual ~Content(){};

      /**
       * Returns the specified parameter as a string
       * @param name the parameter
       * @return the value
       */
      std::string GetString(const std::string& parameter) const;

      /**
      * Returns the specified parameter as an integer
      * @param name the parameter
      * @return the value
      */
      int GetInteger(const std::string& parameter) const;

      /**
       * Returns the specified parameter as an unsigned integer
       * @param name the parameter
       * @return the value
       */
      unsigned int GetUnsignedInteger(const std::string& parameter) const;

    protected:
      /**
       * The request content
       */
      tinyxml2::XMLElement* m_content;

    private:
      tinyxml2::XMLElement* GetParameterElement(const std::string& parameter) const;
    };

    /**
     * Represents the contents of an XMLTV response
     */
    class XMLTVResponseContent : public Content
    {
    public:
      XMLTVResponseContent(tinyxml2::XMLElement* content) : Content(content) {}
      virtual ~XMLTVResponseContent(){};

      /**
       * Returns the list of channels contained in the response
       * @return the channels
       */
      std::vector<ChannelPtr> GetChannels() const;

      /**
       * Returns the complete guide
       * @return the guide
       */
      ::xmltv::Guide GetGuide() const;

    private:
      ChannelPtr CreateChannel(const tinyxml2::XMLElement* xml) const;
    };

    /**
     * Represents the contents of a recording response
     */
    class RecordingResponseContent : public Content
    {
    public:
      RecordingResponseContent(tinyxml2::XMLElement* content) : Content(content) {}
      virtual ~RecordingResponseContent(){};

      std::vector<RecordingPtr> GetRecordings() const;
      std::vector<SeriesRecordingPtr> GetSeriesRecordings() const;

    private:
      RecordingPtr CreateRecording(const tinyxml2::XMLElement* xml) const;
      SeriesRecordingPtr CreateSeriesRecording(const tinyxml2::XMLElement* xml) const;
      RecordingState GetState(const std::string& state) const;
    };
  } // namespace response
} // namespace vbox
