/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "../xmltv/Guide.h"

#include <map>
#include <memory>
#include <string>

namespace vbox
{

  class GuideChannelMapper;
  typedef std::unique_ptr<GuideChannelMapper> GuideChannelMapperPtr;
  typedef std::map<std::string, std::string> ChannelMappings;

  /**
   * Provides functionality for mapping VBox channels into the channel names
   * used by external XMLTV guide data
   */
  class GuideChannelMapper
  {
  public:
    GuideChannelMapper(const ::xmltv::Guide& vboxGuide, const ::xmltv::Guide& externalGuide);
    ~GuideChannelMapper() = default;

    /**
     * Initializes the mapper by loading the mappings from disk. If no existing
     * mappings exist, a basic map is created.
     */
    void Initialize();

    /**
    * @param vboxName a VBox channel name
    * @return the corresponding channel name from the external guide
    */
    std::string GetExternalChannelName(const std::string& vboxName) const;

  private:
    /**
     * Creates a default mapping between the two guides using exact name matching
     * @eturn the mappings
     */
    ChannelMappings CreateDefaultMappings();

    /**
     * Loads the mappings from disk
     */
    void Load();

    /**
     * Saves the current mappings to disk
     */
    void Save();

    /**
     * The path to the mapping file
     */
    const static std::string MAPPING_FILE_PATH;

    /**
     * The internal guide data
     */
    const ::xmltv::Guide& m_vboxGuide;

    /**
     * The external guide data
     */
    const ::xmltv::Guide& m_externalGuide;

    /**
     * The channel name mappings
     */
    ChannelMappings m_channelMappings;
  };
} // namespace vbox
