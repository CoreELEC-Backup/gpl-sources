/*
 *  Copyright (C) 2019-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <kodi/General.h>
#include <stdint.h>
#include <string.h>
#include <vector>

class qsound_rom
{
public:
  struct valid_range
  {
    uint32_t start;
    uint32_t size;
  };

  std::vector<uint8_t> m_aKey;
  std::vector<valid_range> m_aKeyValid;
  std::vector<uint8_t> m_aZ80ROM;
  std::vector<valid_range> m_aZ80ROMValid;
  std::vector<uint8_t> m_aSampleROM;
  std::vector<valid_range> m_aSampleROMValid;

  qsound_rom() {}

  void superimpose_from(const qsound_rom& from)
  {
    superimpose_section_from("KEY", from.m_aKey, from.m_aKeyValid);
    superimpose_section_from("Z80", from.m_aZ80ROM, from.m_aZ80ROMValid);
    superimpose_section_from("SMP", from.m_aSampleROM, from.m_aSampleROMValid);
  }

  void upload_section(const char* section, uint32_t start, const uint8_t* data, uint32_t size)
  {
    std::vector<uint8_t>* pArray = nullptr;
    std::vector<valid_range>* pArrayValid = nullptr;
    uint32_t maxsize = 0x7FFFFFFF;

    if (!strcmp(section, "KEY"))
    {
      pArray = &m_aKey;
      pArrayValid = &m_aKeyValid;
      maxsize = 11;
    }
    else if (!strcmp(section, "Z80"))
    {
      pArray = &m_aZ80ROM;
      pArrayValid = &m_aZ80ROMValid;
    }
    else if (!strcmp(section, "SMP"))
    {
      pArray = &m_aSampleROM;
      pArrayValid = &m_aSampleROMValid;
    }
    else
    {
      kodi::Log(ADDON_LOG_ERROR, "Unknown tag: '%s'", section);
      return;
    }

    if ((start + size) < start)
    {
      kodi::Log(ADDON_LOG_ERROR, "Section '%s' is too large", section);
      return;
    }

    uint32_t newsize = start + size;
    uint32_t oldsize = pArray->size();
    if (newsize > maxsize)
    {
      kodi::Log(ADDON_LOG_ERROR, "Section '%s' is too large (max %i bytes)", section, maxsize);
      return;
    }

    if (newsize > oldsize)
    {
      pArray->resize(newsize);
      memset(pArray->data() + oldsize, 0, newsize - oldsize);
    }

    memcpy(&(*pArray)[start], data, size);

    oldsize = pArrayValid->size();
    pArrayValid->resize(oldsize + 1);
    pArrayValid->back().start = start;
    pArrayValid->back().size = size;
  }

  void clear()
  {
    m_aKey.resize(0);
    m_aKeyValid.resize(0);
    m_aZ80ROM.resize(0);
    m_aZ80ROMValid.resize(0);
    m_aSampleROM.resize(0);
    m_aSampleROMValid.resize(0);
  }

private:
  void superimpose_section_from(const char* section,
                                const std::vector<uint8_t>& from,
                                const std::vector<valid_range>& fromvalid)
  {
    for (unsigned i = 0; i < fromvalid.size(); i++)
    {
      const valid_range& range = fromvalid[i];
      uint32_t start = range.start;
      uint32_t size = range.size;
      if ((start >= from.size()) || (size >= from.size()) || ((start + size) > from.size()))
      {
        kodi::Log(ADDON_LOG_ERROR, "Invalid start/size in QSoundROM internal list (program error)");
        return;
      }

      upload_section(section, start, &from[start], size);
    }
  }
};
