/*
 *  Copyright (C) 2005-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "ASAPCodec.h"

#include <kodi/Filesystem.h>

CASAPCodec::CASAPCodec(KODI_HANDLE instance, const std::string& version)
  : CInstanceAudioDecoder(instance, version)
{
}

CASAPCodec::~CASAPCodec()
{
  if (ctx.asap)
    ASAP_Delete(ctx.asap);
}

bool CASAPCodec::Init(const std::string& filename,
                      unsigned int filecache,
                      int& channels,
                      int& samplerate,
                      int& bitspersample,
                      int64_t& totaltime,
                      int& bitrate,
                      AudioEngineDataFormat& format,
                      std::vector<AudioEngineChannel>& channellist)
{
  int track = 0;
  std::string toLoad(filename);
  if (toLoad.find(".asapstream") != std::string::npos)
  {
    size_t iStart = toLoad.rfind('-') + 1;
    track = atoi(toLoad.substr(iStart, toLoad.size() - iStart - 11).c_str()) - 1;
    //  The directory we are in, is the file
    //  that contains the bitstream to play,
    //  so extract it
    size_t slash = toLoad.rfind('\\');
    if (slash == std::string::npos)
      slash = toLoad.rfind('/');
    toLoad = toLoad.substr(0, slash);
  }

  kodi::vfs::CFile file;
  if (!file.OpenFile(toLoad, 0))
    return false;

  int len = file.GetLength();
  uint8_t* data = new uint8_t[len];
  file.Read(data, len);
  file.Close();

  ctx.asap = ASAP_New();

  // Now load the module
  if (!ASAP_Load(ctx.asap, toLoad.c_str(), data, len))
  {
    delete[] data;
    return false;
  }
  delete[] data;

  const ASAPInfo* info = ASAP_GetInfo(ctx.asap);

  channels = ASAPInfo_GetChannels(info);
  samplerate = 44100;
  bitspersample = 16;
  totaltime = ASAPInfo_GetDuration(info, track);
  format = AUDIOENGINE_FMT_S16NE;
  if (channels == 1)
    channellist = {AUDIOENGINE_CH_FC};
  else
    channellist = {AUDIOENGINE_CH_FL, AUDIOENGINE_CH_FR};
  bitrate = 0;

  ASAP_PlaySong(ctx.asap, track, totaltime);

  return true;
}

int CASAPCodec::ReadPCM(uint8_t* buffer, int size, int& actualsize)
{
  actualsize = ASAP_Generate(ctx.asap, buffer, size, ASAPSampleFormat_S16_L_E);

  return actualsize == 0;
}

int64_t CASAPCodec::Seek(int64_t time)
{
  ASAP_Seek(ctx.asap, time);

  return time;
}

bool CASAPCodec::ReadTag(const std::string& filename, kodi::addon::AudioDecoderInfoTag& tag)
{
  int track = 0;
  std::string toLoad(filename);
  if (toLoad.find(".asapstream") != std::string::npos)
  {
    size_t iStart = toLoad.rfind('-') + 1;
    track = atoi(toLoad.substr(iStart, toLoad.size() - iStart - 11).c_str());
    //  The directory we are in, is the file
    //  that contains the bitstream to play,
    //  so extract it
    size_t slash = toLoad.rfind('\\');
    if (slash == std::string::npos)
      slash = toLoad.rfind('/');
    toLoad = toLoad.substr(0, slash);
  }

  kodi::vfs::CFile file;
  if (!file.OpenFile(toLoad, 0))
    return false;

  int len = file.GetLength();
  uint8_t* data = new uint8_t[len];
  file.Read(data, len);
  file.Close();

  ASAP* asap = ASAP_New();

  // Now load the module
  if (!ASAP_Load(asap, toLoad.c_str(), data, len))
  {
    delete[] data;
    return false;
  }

  delete[] data;

  const ASAPInfo* info = ASAP_GetInfo(asap);
  tag.SetArtist(ASAPInfo_GetAuthor(info));
  tag.SetTitle(ASAPInfo_GetTitleOrFilename(info));
  tag.SetDuration(ASAPInfo_GetDuration(info, track) / 1000);
  if (ASAPInfo_GetYear(info) > 0)
    tag.SetReleaseDate(std::to_string(ASAPInfo_GetYear(info)));
  tag.SetChannels(ASAPInfo_GetChannels(info));
  tag.SetSamplerate(44100);
  tag.SetTrack(track);

  ASAP_Delete(asap);

  return true;
}

int CASAPCodec::TrackCount(const std::string& fileName)
{
  kodi::vfs::CFile file;
  if (!file.OpenFile(fileName, 0))
    return 1;

  int len = file.GetLength();
  uint8_t* data = new uint8_t[len];
  file.Read(data, len);
  file.Close();

  ASAP* asap = ASAP_New();

  // Now load the module
  if (!ASAP_Load(asap, fileName.c_str(), data, len))
  {
    ASAP_Delete(asap);
    delete[] data;
    return 1;
  }
  delete[] data;

  const ASAPInfo* info = ASAP_GetInfo(asap);
  int result = ASAPInfo_GetSongs(info);
  ASAP_Delete(asap);

  return result;
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
    addonInstance = new CASAPCodec(instance, version);
    return ADDON_STATUS_OK;
  }
  virtual ~CMyAddon() = default;
};


ADDONCREATOR(CMyAddon)
