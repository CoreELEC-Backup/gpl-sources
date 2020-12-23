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

namespace vbox
{

  typedef std::map<std::string, int> GenreMap;
  typedef std::map<std::string, int> CategoryMap;

  const std::string CATEGORY_TO_GENRE_XML_PATH = "special://userdata/addon_data/pvr.vbox/category_to_genre_types.xml";

  class CategoryGenreMapper
  {
  public:
    CategoryGenreMapper();
    ~CategoryGenreMapper() = default;
    void Initialize(const std::string& xmlFileName);
    bool LoadCategoryToGenreXML(const std::string& xmlFileName);
    int GetCategoriesGenreType(std::vector<std::string>& categories);

  private:
    GenreMap m_genreMap;
    CategoryMap m_categoryMap;
  };

  typedef std::unique_ptr<CategoryGenreMapper> CategoryMapperPtr;
} // namespace vbox
