/*
 *  Copyright (C) 2008-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "YMCodec.h"

#include <kodi/Filesystem.h>
#include <stdint.h>
#include <stdio.h>

CYMCodec::CYMCodec(KODI_HANDLE instance, const std::string& version)
  : CInstanceAudioDecoder(instance, version)
{
}

CYMCodec::~CYMCodec()
{
  if (m_tune)
  {
    ymMusicStop(m_tune);
    ymMusicDestroy(m_tune);
  }
}

bool CYMCodec::Init(const std::string& filename,
                    unsigned int filecache,
                    int& channels,
                    int& samplerate,
                    int& bitspersample,
                    int64_t& totaltime,
                    int& bitrate,
                    AudioEngineDataFormat& format,
                    std::vector<AudioEngineChannel>& channellist)
{
  m_tune = ymMusicCreate();
  if (!m_tune)
    return false;

  kodi::vfs::CFile file;
  if (!file.OpenFile(filename))
    return false;

  int len = file.GetLength();
  char* data = new char[len];
  if (!data)
  {
    file.Close();
    ymMusicDestroy(m_tune);
    return false;
  }
  file.Read(data, len);
  file.Close();

  int res = ymMusicLoadMemory(m_tune, data, len);
  delete[] data;
  if (res)
  {
    ymMusicSetLoopMode(m_tune, YMFALSE);
    ymMusicPlay(m_tune);
    ymMusicInfo_t info;
    ymMusicGetInfo(m_tune, &info);

    channels = 1;
    samplerate = 44100;
    bitspersample = 16;
    totaltime = info.musicTimeInSec * 1000;
    format = AUDIOENGINE_FMT_S16NE;
    channellist = {AUDIOENGINE_CH_FL, AUDIOENGINE_CH_FR};
    bitrate = 0;
    return true;
  }

  return false;
}

int CYMCodec::ReadPCM(uint8_t* buffer, int size, int& actualsize)
{
  if (ymMusicCompute(m_tune, (ymsample*)buffer, size / 2))
  {
    actualsize = size;
    return 0;
  }
  else
    return 1;
}

int64_t CYMCodec::Seek(int64_t time)
{
  if (ymMusicIsSeekable(m_tune))
  {
    ymMusicSeek(m_tune, time);
    return time;
  }

  return 0;
}

bool CYMCodec::ReadTag(const std::string& filename, kodi::addon::AudioDecoderInfoTag& tag)
{
  YMMUSIC* tune = ymMusicCreate();
  kodi::vfs::CFile file;
  if (!file.OpenFile(filename))
    return false;

  int len = file.GetLength();
  char* data = new char[len];
  if (!data)
  {
    file.Close();
    ymMusicDestroy(tune);
    return false;
  }
  file.Read(data, len);
  file.Close();

  tag.SetDuration(0);
  if (ymMusicLoadMemory(tune, data, len))
  {
    ymMusicInfo_t info;
    ymMusicGetInfo(tune, &info);
    tag.SetTitle(info.pSongName);
    tag.SetArtist(info.pSongAuthor);
    tag.SetDuration(info.musicTimeInSec);
  }
  delete[] data;

  ymMusicDestroy(tune);

  return tag.GetDuration() != 0;
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
    addonInstance = new CYMCodec(instance, version);
    return ADDON_STATUS_OK;
  }
  virtual ~CMyAddon() = default;
};

ADDONCREATOR(CMyAddon)
