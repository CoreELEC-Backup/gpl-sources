/*
 *  Copyright (C) 2014-2020 Arne Morten Kvarving
 *  Copyright (C) 2016-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "USFCodec.h"

#include <iostream>
#include <kodi/Filesystem.h>

extern "C"
{

  int usf_load(void* ctx, const uint8_t*, size_t exe_size, const uint8_t* data, size_t size)
  {
    if (exe_size > 0)
      return -1;
    usf_upload_section(ctx, data, size);
    return 0;
  }

  static void* psf_file_fopen(void* context, const char* uri)
  {
    kodi::vfs::CFile* file = new kodi::vfs::CFile;
    if (!file->OpenFile(uri, 0))
    {
      delete file;
      return nullptr;
    }

    return file;
  }

  static size_t psf_file_fread(void* buffer, size_t size, size_t count, void* handle)
  {
    kodi::vfs::CFile* file = static_cast<kodi::vfs::CFile*>(handle);
    return file->Read(buffer, size * count);
  }

  static int psf_file_fseek(void* handle, int64_t offset, int whence)
  {
    kodi::vfs::CFile* file = static_cast<kodi::vfs::CFile*>(handle);
    return file->Seek(offset, whence) > -1 ? 0 : -1;
  }

  static int psf_file_fclose(void* handle)
  {
    delete static_cast<kodi::vfs::CFile*>(handle);

    return 0;
  }

  static long psf_file_ftell(void* handle)
  {
    kodi::vfs::CFile* file = static_cast<kodi::vfs::CFile*>(handle);
    return file->GetPosition();
  }

  const psf_file_callbacks psf_file_system = {"\\/",          nullptr,        psf_file_fopen,
                                              psf_file_fread, psf_file_fseek, psf_file_fclose,
                                              psf_file_ftell};

  static unsigned long parse_time_crap(const char* input)
  {
    if (!input)
      return 0;
    int len = strlen(input);
    if (!len)
      return 0;
    int value = 0;
    {
      int i;
      for (i = len - 1; i >= 0; i--)
      {
        if ((input[i] < '0' || input[i] > '9') && input[i] != ':' && input[i] != ',' &&
            input[i] != '.')
        {
          return 0;
        }
      }
    }
    std::string foo = input;
    char* bar = (char*)&foo[0];
    char* strs = bar + foo.size() - 1;
    while (strs > bar && (*strs >= '0' && *strs <= '9'))
    {
      strs--;
    }
    if (*strs == '.' || *strs == ',')
    {
      // fraction of a second
      strs++;
      if (strlen(strs) > 3)
        strs[3] = 0;
      value = atoi(strs);
      switch (strlen(strs))
      {
        case 1:
          value *= 100;
          break;
        case 2:
          value *= 10;
          break;
      }
      strs--;
      *strs = 0;
      strs--;
    }
    while (strs > bar && (*strs >= '0' && *strs <= '9'))
    {
      strs--;
    }
    // seconds
    if (*strs < '0' || *strs > '9')
      strs++;
    value += atoi(strs) * 1000;
    if (strs > bar)
    {
      strs--;
      *strs = 0;
      strs--;
      while (strs > bar && (*strs >= '0' && *strs <= '9'))
      {
        strs--;
      }
      if (*strs < '0' || *strs > '9')
        strs++;
      value += atoi(strs) * 60000;
      if (strs > bar)
      {
        strs--;
        *strs = 0;
        strs--;
        while (strs > bar && (*strs >= '0' && *strs <= '9'))
        {
          strs--;
        }
        value += atoi(strs) * 3600000;
      }
    }
    return value;
  }

  static int psf_info_meta(void* context, const char* name, const char* value)
  {
    USFContext* usf = (USFContext*)context;
    if (!strcasecmp(name, "length"))
      usf->len = parse_time_crap(value);
    else if (!strcasecmp(name, "fade"))
      usf->fade = atoi(value);
    else if (!strcasecmp(name, "title"))
      usf->title = value;
    else if (!strcasecmp(name, "artist"))
      usf->artist = value;
    else if (!strcasecmp(name, "game"))
      usf->game = value;
    else if (!strcasecmp(name, "genre"))
      usf->genre = value;
    else if (!strcasecmp(name, "year"))
      usf->year = value;
    else if (!strcasecmp(name, "usfby"))
      usf->usfby = value;
    else if (!strcasecmp(name, "copyright"))
      usf->copyright = value;
    else if (!strcasecmp(name, "comment"))
      usf->comment = value;

    return 0;
  }

} /* extern "C" */

//------------------------------------------------------------------------------

CUSFCodec::CUSFCodec(KODI_HANDLE instance, const std::string& version)
  : CInstanceAudioDecoder(instance, version)
{
}

CUSFCodec::~CUSFCodec()
{
  delete[] ctx.state;
}

bool CUSFCodec::Init(const std::string& filename,
                     unsigned int filecache,
                     int& channels,
                     int& samplerate,
                     int& bitspersample,
                     int64_t& totaltime,
                     int& bitrate,
                     AudioEngineDataFormat& format,
                     std::vector<AudioEngineChannel>& channellist)
{
  ctx.pos = 0;
  ctx.state = new char[usf_get_state_size()];
  usf_clear(ctx.state);
  if (psf_load(filename.c_str(), &psf_file_system, 0x21, usf_load, ctx.state, psf_info_meta, &ctx,
               0, nullptr, nullptr) < 0)
  {
    delete ctx.state;
    return false;
  }
  totaltime = ctx.len;
  usf_set_compare(ctx.state, 0);
  usf_set_fifo_full(ctx.state, 0);
  usf_set_hle_audio(ctx.state, 1);
  format = AUDIOENGINE_FMT_S16NE;
  channellist = {AUDIOENGINE_CH_FL, AUDIOENGINE_CH_FR};
  channels = 2;

  bitspersample = 16;
  bitrate = 0.0;

  int32_t srate;
  usf_render(ctx.state, NULL, 0, &srate);
  samplerate = ctx.sample_rate = srate;

  ctx.len = srate * 4 * (totaltime) / 1000;

  return true;
}

int CUSFCodec::ReadPCM(uint8_t* buffer, int size, int& actualsize)
{
  if (ctx.len > 0 && ctx.pos >= ctx.len)
    return 1;
  if (usf_render(ctx.state, (int16_t*)buffer, size / 4, &ctx.sample_rate))
    return 1;
  if (CheckEndReached(buffer, size))
    return 1;
  ctx.pos += size;
  actualsize = size;
  return 0;
}

int64_t CUSFCodec::Seek(int64_t time)
{
  if (time * ctx.sample_rate * 4 / 1000 < ctx.pos)
  {
    usf_restart(ctx.state);
    ctx.pos = 0;
  }

  int64_t left = time * ctx.sample_rate * 4 / 1000 - ctx.pos;
  while (left > 4096)
  {
    usf_render(ctx.state, nullptr, 1024, &ctx.sample_rate);
    ctx.pos += 4096;
    left -= 4096;
  }

  return ctx.pos / (ctx.sample_rate * 4) * 1000;
}

bool CUSFCodec::ReadTag(const std::string& filename, kodi::addon::AudioDecoderInfoTag& tag)
{
  USFContext usf;
  ctx.state = new char[usf_get_state_size()];
  usf_clear(ctx.state);

  if (psf_load(filename.c_str(), &psf_file_system, 0x21, nullptr, nullptr, psf_info_meta, &usf, 0,
               nullptr, nullptr) <= 0)
  {
    return false;
  }

  tag.SetTitle(usf.title);
  tag.SetArtist(usf.artist);
  tag.SetAlbum(usf.game);
  tag.SetGenre(usf.genre);
  tag.SetReleaseDate(usf.year);
  tag.SetComment(usf.comment);
  tag.SetSamplerate(usf.sample_rate);
  tag.SetDuration(usf.len / 1000);

  delete[] usf.state;
  return true;
}

// Hack way function
// Looked at the test that most miniusf files give no length.
// In addition, in the libs was no way to get the length, also runs
// "usf_render" on without giving a sign reached the end.
//
// This checks the buffer described by usf, if it is zero, the end has
// been reached.
//
bool CUSFCodec::CheckEndReached(uint8_t* buffer, int size)
{
  if (!m_firstRoundDone)
  {
    m_firstRoundDone = true;
    return false;
  }

  if (buffer && buffer[0] == 0)
  {
    for (unsigned int i = 0; i < size; i++)
    {
      if (buffer[i] != 0)
      {
        return false;
      }
    }
    return true;
  }

  return false;
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
    addonInstance = new CUSFCodec(instance, version);
    return ADDON_STATUS_OK;
  }
  virtual ~CMyAddon() = default;
};

ADDONCREATOR(CMyAddon)
