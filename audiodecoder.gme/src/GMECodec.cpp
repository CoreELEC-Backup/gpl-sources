/*
 *  Copyright (C) 2014-2020 Arne Morten Kvarving
 *  Copyright (C) 2016-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "GMECodec.h"

#include <kodi/Filesystem.h>

CGMECodec::CGMECodec(KODI_HANDLE instance, const std::string& version)
  : CInstanceAudioDecoder(instance, version)
{
}

CGMECodec::~CGMECodec()
{
  if (ctx.gme)
    gme_delete(ctx.gme);
}

bool CGMECodec::Init(const std::string& filename,
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
  if (toLoad.rfind("stream") != std::string::npos)
  {
    size_t iStart = toLoad.rfind('-') + 1;
    track = atoi(toLoad.substr(iStart, toLoad.size() - iStart - 10).c_str()) - 1;
    //  The directory we are in, is the file
    //  that contains the bitstream to play,
    //  so extract it
    size_t slash = toLoad.rfind('\\');
    if (slash == std::string::npos)
      slash = toLoad.rfind('/');
    toLoad = toLoad.substr(0, slash);
  }

  gme_open_file(toLoad.c_str(), &ctx.gme, 48000);
  if (!ctx.gme)
    return false;

  channels = 2;
  samplerate = 48000;
  bitspersample = 16;
  bitrate = 0.0;
  format = AUDIOENGINE_FMT_S16NE;
  gme_info_t* out;
  gme_track_info(ctx.gme, &out, track);
  totaltime = ctx.len = out->play_length;
  channellist = {AUDIOENGINE_CH_FL, AUDIOENGINE_CH_FR};
  gme_start_track(ctx.gme, track);

  return true;
}

int CGMECodec::ReadPCM(uint8_t* buffer, int size, int& actualsize)
{
  if (gme_tell(ctx.gme) >= ctx.len)
    return -1;
  actualsize = size;
  gme_play(ctx.gme, size / 2, (short*)buffer);
  return 0;
}

int64_t CGMECodec::Seek(int64_t time)
{
  gme_seek(ctx.gme, time);
  return gme_tell(ctx.gme);
}

bool CGMECodec::ReadTag(const std::string& filename, kodi::addon::AudioDecoderInfoTag& tag)
{
  gme_t* gme = nullptr;
  gme_open_file(filename.c_str(), &gme, 48000);
  if (!gme)
    return false;

  gme_info_t* out;
  gme_track_info(gme, &out, 0);
  tag.SetSamplerate(48000);
  tag.SetChannels(2);
  tag.SetDuration(out->play_length / 1000);
  if (out->song)
    tag.SetTitle(out->song);
  if (out->game && tag.GetTitle().empty())
    tag.SetTitle(out->game);
  if (out->author)
    tag.SetArtist(out->author);
  if (out->game)
    tag.SetAlbum(out->game);
  if (out->comment)
    tag.SetComment(out->comment);
  gme_delete(gme);
  return true;
}

int CGMECodec::TrackCount(const std::string& fileName)
{
  gme_t* gme = nullptr;
  gme_open_file(fileName.c_str(), &gme, 48000);
  if (!gme)
    return 1;

  int result = gme_track_count(gme);
  gme_delete(gme);

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
    addonInstance = new CGMECodec(instance, version);
    return ADDON_STATUS_OK;
  }
  virtual ~CMyAddon() = default;
};

ADDONCREATOR(CMyAddon)
