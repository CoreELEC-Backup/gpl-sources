/*
 *  Copyright (C) 2005-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "OpenMptCodec.h"

extern "C"
{

  static size_t vfs_file_fread(void* handle, void* dst, size_t size)
  {
    kodi::vfs::CFile* file = static_cast<kodi::vfs::CFile*>(handle);
    return file->Read(dst, size);
  }

  static int vfs_file_fseek(void* handle, int64_t offset, int whence)
  {
    kodi::vfs::CFile* file = static_cast<kodi::vfs::CFile*>(handle);
    return file->Seek(offset, whence) > -1 ? 0 : -1;
  }

  static int64_t vfs_file_ftell(void* handle)
  {
    kodi::vfs::CFile* file = static_cast<kodi::vfs::CFile*>(handle);
    return file->GetPosition();
  }

} /* extern "C" */

//------------------------------------------------------------------------------

CMPTCodec::CMPTCodec(KODI_HANDLE instance, const std::string& version)
  : CInstanceAudioDecoder(instance, version)
{
}

CMPTCodec::~CMPTCodec()
{
  if (ctx.module)
    openmpt_module_destroy(ctx.module);
}

bool CMPTCodec::Init(const std::string& filename,
                     unsigned int filecache,
                     int& channels,
                     int& samplerate,
                     int& bitspersample,
                     int64_t& totaltime,
                     int& bitrate,
                     AudioEngineDataFormat& format,
                     std::vector<AudioEngineChannel>& channellist)
{
  if (!ctx.file.OpenFile(filename, ADDON_READ_CACHED))
    return false;

  static openmpt_stream_callbacks callbacks = {vfs_file_fread, vfs_file_fseek, vfs_file_ftell};

  ctx.module = openmpt_module_create2(callbacks, &ctx.file, nullptr, nullptr, nullptr, nullptr,
                                      nullptr, nullptr, nullptr);
  if (!ctx.module)
    return false;

  channels = 2;
  samplerate = 48000;
  bitspersample = 32;
  totaltime = openmpt_module_get_duration_seconds(ctx.module) * 1000;
  format = AUDIOENGINE_FMT_FLOAT;
  channellist = {AUDIOENGINE_CH_FL, AUDIOENGINE_CH_FR};
  bitrate = openmpt_module_get_num_channels(ctx.module);

  return true;
}

int CMPTCodec::ReadPCM(uint8_t* buffer, int size, int& actualsize)
{
  if ((actualsize = openmpt_module_read_interleaved_float_stereo(ctx.module, 48000, size / 8,
                                                                 (float*)buffer) *
                    8) == size)
    return 0;

  return 1;
}

int64_t CMPTCodec::Seek(int64_t time)
{
  return openmpt_module_set_position_seconds(ctx.module, time / 1000.0) * 1000.0;
}

bool CMPTCodec::ReadTag(const std::string& filename, kodi::addon::AudioDecoderInfoTag& tag)
{
  if (!ctx.file.OpenFile(filename, ADDON_READ_CACHED))
    return false;

  static openmpt_stream_callbacks callbacks = {vfs_file_fread, vfs_file_fseek, vfs_file_ftell};

  ctx.module = openmpt_module_create2(callbacks, &ctx.file, nullptr, nullptr, nullptr, nullptr,
                                      nullptr, nullptr, nullptr);
  if (!ctx.module)
    return false;

  const char* ckeys = openmpt_module_get_metadata_keys(ctx.module);
  std::string keys = ckeys;
  openmpt_free_string(ckeys);

  if (keys.find("artist") != std::string::npos)
  {
    const char* text = openmpt_module_get_metadata(ctx.module, "artist");
    if (text)
    {
      tag.SetArtist(text);
      openmpt_free_string(text);
    }
  }
  if (keys.find("title") != std::string::npos)
  {
    const char* text = openmpt_module_get_metadata(ctx.module, "title");
    if (text)
    {
      tag.SetTitle(text);
      openmpt_free_string(text);
    }
  }
  if (keys.find("album") != std::string::npos)
  {
    const char* text = openmpt_module_get_metadata(ctx.module, "album");
    if (text)
    {
      tag.SetAlbum(text);
      openmpt_free_string(text);
    }
  }
  if (keys.find("year") != std::string::npos)
  {
    const char* text = openmpt_module_get_metadata(ctx.module, "year");
    if (text)
    {
      tag.SetReleaseDate(text);
      openmpt_free_string(text);
    }
  }
  if (keys.find("genre") != std::string::npos)
  {
    const char* text = openmpt_module_get_metadata(ctx.module, "genre");
    if (text)
    {
      tag.SetGenre(text);
      openmpt_free_string(text);
    }
  }
  if (keys.find("track number") != std::string::npos)
  {
    const char* text = openmpt_module_get_metadata(ctx.module, "track number");
    if (text)
    {
      tag.SetTrack(atoi(text));
      openmpt_free_string(text);
    }
  }
  if (keys.find("comments") != std::string::npos)
  {
    const char* text = openmpt_module_get_metadata(ctx.module, "comments");
    if (text)
    {
      tag.SetComment(text);
      openmpt_free_string(text);
    }
  }

  tag.SetChannels(2);
  tag.SetSamplerate(48000);
  tag.SetDuration(openmpt_module_get_duration_seconds(ctx.module));

  return true;
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
    addonInstance = new CMPTCodec(instance, version);
    return ADDON_STATUS_OK;
  }
  virtual ~CMyAddon() = default;
};

ADDONCREATOR(CMyAddon)
