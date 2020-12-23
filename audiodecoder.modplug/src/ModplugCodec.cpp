/*
 *  Copyright (C) 2005-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

//! \file ModplugCodec.cpp
//! \author Arne Morten Kvarving
//! \brief Modplug audio decoder for Kodi

#include "ModplugCodec.h"

#include <kodi/Filesystem.h>

CModplugCodec::CModplugCodec(KODI_HANDLE instance, const std::string& version)
  : CInstanceAudioDecoder(instance, version)
{
}

CModplugCodec::~CModplugCodec()
{
  if (m_module)
    ModPlug_Unload(m_module);
}

bool CModplugCodec::Init(const std::string& filename,
                         unsigned int filecache,
                         int& channels,
                         int& samplerate,
                         int& bitspersample,
                         int64_t& totaltime,
                         int& bitrate,
                         AudioEngineDataFormat& format,
                         std::vector<AudioEngineChannel>& channellist)
{
  kodi::vfs::CFile file;
  if (!file.OpenFile(filename, 0))
    return false;

  int len = file.GetLength();
  char* data = new char[len];
  if (!data)
    return false;

  file.Read(data, len);
  file.Close();

  // Now load the m_module
  m_module = ModPlug_Load(data, len);
  delete[] data;

  if (!m_module)
    return false;

  channels = 2;
  samplerate = 44100;
  bitspersample = 16;
  totaltime = (int64_t)ModPlug_GetLength(m_module);
  format = AUDIOENGINE_FMT_S16NE;
  channellist = {AUDIOENGINE_CH_FL, AUDIOENGINE_CH_FR};
  bitrate, ModPlug_NumChannels(m_module);

  return true;
}

int CModplugCodec::ReadPCM(uint8_t* buffer, int size, int& actualsize)
{
  if (!m_module)
    return 1;

  if ((actualsize = ModPlug_Read(m_module, buffer, size)) == size)
    return 0;

  return 1;
}

int64_t CModplugCodec::Seek(int64_t time)
{
  if (!m_module)
    return -1;

  ModPlug_Seek(m_module, (int)time);
  return time;
}

//------------------------------------------------------------------------------

class ATTRIBUTE_HIDDEN CMyAddon : public kodi::addon::CAddonBase
{
public:
  CMyAddon() = default;
  ADDON_STATUS CreateInstance(int instanceType,
                              const std::string& instanceID,
                              KODI_HANDLE instance,
                              const std::string& version,
                              KODI_HANDLE& addonInstance) override
  {
    addonInstance = new CModplugCodec(instance, version);
    return ADDON_STATUS_OK;
  }
  ~CMyAddon() override = default;
};

ADDONCREATOR(CMyAddon)
