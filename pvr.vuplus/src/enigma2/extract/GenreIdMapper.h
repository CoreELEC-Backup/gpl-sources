/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "IExtractor.h"

#include <map>
#include <string>

namespace enigma2
{
  namespace extract
  {
    class ATTRIBUTE_HIDDEN GenreIdMapper : public IExtractor
    {
    public:
      GenreIdMapper();
      ~GenreIdMapper();

      void ExtractFromEntry(enigma2::data::BaseEntry& entry);
      bool IsEnabled();

    private:
      static int GetGenreTypeFromCombined(int combinedGenreType);
      static int GetGenreSubTypeFromCombined(int combinedGenreType);

      int LookupGenreIdInMap(const int genreId);

      void LoadGenreIdMapFile();
      bool LoadIdToIdGenreFile(const std::string& xmlFile, std::map<int, int>& map);
      void CreateGenreAddonDataDirectories();

      std::map<int, int> m_genreIdToDvbIdMap;
    };
  } //namespace extract
} //namespace enigma2
