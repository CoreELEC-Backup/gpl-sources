/*
 *  Copyright (C) 2014-2020 Arne Morten Kvarving
 *  Copyright (C) 2016-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "GSFCodec.h"

#include <algorithm>
#include <kodi/Filesystem.h>
#include <stdint.h>
#include <stdio.h>

extern "C"
{

  inline unsigned get_le32(void const* p)
  {
    return (unsigned)((unsigned char const*)p)[3] << 24 |
           (unsigned)((unsigned char const*)p)[2] << 16 |
           (unsigned)((unsigned char const*)p)[1] << 8 | (unsigned)((unsigned char const*)p)[0];
  }

  int gsf_loader(void* context,
                 const uint8_t* exe,
                 size_t exe_size,
                 const uint8_t* reserved,
                 size_t reserved_size)
  {
    if (exe_size < 12)
      return -1;

    struct gsf_loader_state* state = (struct gsf_loader_state*)context;

    unsigned char* iptr;
    unsigned isize;
    unsigned char* xptr;
    unsigned xentry = get_le32(exe + 0);
    unsigned xsize = get_le32(exe + 8);
    unsigned xofs = get_le32(exe + 4) & 0x1ffffff;
    if (xsize < exe_size - 12)
      return -1;
    if (!state->entry_set)
    {
      state->entry = xentry;
      state->entry_set = 1;
    }
    {
      iptr = state->data;
      isize = state->data_size;
      state->data = 0;
      state->data_size = 0;
    }
    if (!iptr)
    {
      unsigned rsize = xofs + xsize;
      {
        rsize -= 1;
        rsize |= rsize >> 1;
        rsize |= rsize >> 2;
        rsize |= rsize >> 4;
        rsize |= rsize >> 8;
        rsize |= rsize >> 16;
        rsize += 1;
      }
      iptr = (unsigned char*)malloc(rsize + 10);
      if (!iptr)
        return -1;
      memset(iptr, 0, rsize + 10);
      isize = rsize;
    }
    else if (isize < xofs + xsize)
    {
      unsigned rsize = xofs + xsize;
      {
        rsize -= 1;
        rsize |= rsize >> 1;
        rsize |= rsize >> 2;
        rsize |= rsize >> 4;
        rsize |= rsize >> 8;
        rsize |= rsize >> 16;
        rsize += 1;
      }
      xptr = (unsigned char*)realloc(iptr, xofs + rsize + 10);
      if (!xptr)
      {
        free(iptr);
        return -1;
      }
      iptr = xptr;
      isize = rsize;
    }
    memcpy(iptr + xofs, exe + 12, xsize);
    {
      state->data = iptr;
      state->data_size = isize;
    }
    return 0;
  }

  static void* psf_file_fopen(void* context, const char* uri)
  {
    fprintf(stderr, "-aaa------------< 1\n");
    kodi::vfs::CFile* file = new kodi::vfs::CFile;
    if (!file->OpenFile(uri, 0))
    {
      delete file;
      return nullptr;
    }
fprintf(stderr, "-aaa------------< 1aa\n");
    return file;
  }

  static size_t psf_file_fread(void* buffer, size_t size, size_t count, void* handle)
  {
    fprintf(stderr, "-aaa------------< 2\n");
    kodi::vfs::CFile* file = static_cast<kodi::vfs::CFile*>(handle);
    return file->Read(buffer, size * count);
  }

  static int psf_file_fseek(void* handle, int64_t offset, int whence)
  {
    fprintf(stderr, "-aaa------------< 3\n");
    kodi::vfs::CFile* file = static_cast<kodi::vfs::CFile*>(handle);
    return file->Seek(offset, whence) > -1 ? 0 : -1;
  }

  static int psf_file_fclose(void* handle)
  {
    fprintf(stderr, "-aaa------------< 4\n");
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

#define BORK_TIME 0xC0CAC01A
  static unsigned long parse_time_crap(const char* input)
  {
    if (!input)
      return BORK_TIME;
    int len = strlen(input);
    if (!len)
      return BORK_TIME;
    int value = 0;
    {
      int i;
      for (i = len - 1; i >= 0; i--)
      {
        if ((input[i] < '0' || input[i] > '9') && input[i] != ':' && input[i] != ',' &&
            input[i] != '.')
        {
          return BORK_TIME;
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
    GSFContext* gsf = (GSFContext*)context;
    if (!strcasecmp(name, "length"))
      gsf->len = parse_time_crap(value);
    else if (!strcasecmp(name, "title"))
      gsf->title = value;
    else if (!strcasecmp(name, "artist"))
      gsf->artist = value;
    else if (!strcasecmp(name, "year"))
      gsf->year = value;
    else if (!strcasecmp(name, "game"))
      gsf->game = value;
    else if (!strcasecmp(name, "comment"))
      gsf->comment = value;

    return 0;
  }

} /* extern "C" */

//------------------------------------------------------------------------------

CGSFCodec::CGSFCodec(KODI_HANDLE instance, const std::string& version)
  : CInstanceAudioDecoder(instance, version)
{
}

CGSFCodec::~CGSFCodec()
{
  if (ctx.inited)
  {
    soundShutdown(&ctx.system);
    CPUCleanUp(&ctx.system);
  }
}

bool CGSFCodec::Init(const std::string& filename,
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

  if (psf_load(filename.c_str(), &psf_file_system, 0x22, 0, 0, psf_info_meta, &ctx, 0, nullptr,
               nullptr) <= 0)
    return false;

  if (psf_load(filename.c_str(), &psf_file_system, 0x22, gsf_loader, &ctx.state, 0, 0, 0, nullptr,
               nullptr) < 0)
    return false;

  ctx.system.cpuIsMultiBoot = (ctx.state.entry >> 24 == 2);
  CPULoadRom(&ctx.system, ctx.state.data, ctx.state.data_size);
  soundInit(&ctx.system, &ctx.output);
  soundReset(&ctx.system);
  CPUInit(&ctx.system);
  CPUReset(&ctx.system);

  format = AUDIOENGINE_FMT_S16NE;
  channellist = {AUDIOENGINE_CH_FL, AUDIOENGINE_CH_FR};
  channels = 2;
  bitspersample = 16;
  bitrate = 0.0;
  samplerate = ctx.sample_rate = 44100;
  totaltime = ctx.len;
  ctx.len = ctx.sample_rate * 4 * totaltime / 1000;

  return true;
}

int CGSFCodec::ReadPCM(uint8_t* buffer, int size, int& actualsize)
{
  if (ctx.pos >= ctx.len)
    return 1;

  if (ctx.output.bytes_in_buffer == 0)
  {
    ctx.output.head = 0;
    CPULoop(&ctx.system, 250000);
  }

  int tocopy = std::min(size, (int)ctx.output.bytes_in_buffer);
  memcpy(buffer, &ctx.output.sample_buffer[ctx.output.head], tocopy);
  ctx.pos += tocopy;
  ctx.output.bytes_in_buffer -= tocopy;
  ctx.output.head += tocopy;
  actualsize = tocopy;
  return 0;
}

int64_t CGSFCodec::Seek(int64_t time)
{
  if (time * ctx.sample_rate * 4 / 1000 < ctx.pos)
  {
    CPUReset(&ctx.system);
    ctx.pos = 0;
  }

  int64_t left = time * ctx.sample_rate * 4 / 1000 - ctx.pos;
  while (left > 1024)
  {
    CPULoop(&ctx.system, 250000);
    ctx.pos += ctx.output.bytes_in_buffer;
    left -= ctx.output.bytes_in_buffer;
    ctx.output.head = ctx.output.bytes_in_buffer = 0;
  }

  return ctx.pos / (ctx.sample_rate * 4) * 1000;
}

bool CGSFCodec::ReadTag(const std::string& filename, kodi::addon::AudioDecoderInfoTag& tag)
{
  GSFContext gsf;
fprintf(stderr, "-------------< 1\n");
  if (psf_load(filename.c_str(), &psf_file_system, 0x22, 0, 0, psf_info_meta, &gsf, 0, nullptr,
               nullptr) <= 0)
  {
    return false;
  }
fprintf(stderr, "-------------< 2\n");
  if (!gsf.title.empty())
    tag.SetArtist(gsf.artist);
  else
    tag.SetArtist(gsf.artist);
  tag.SetTitle(gsf.title);
  tag.SetAlbum(gsf.game);
  tag.SetReleaseDate(gsf.year);
  tag.SetComment(gsf.comment);
  tag.SetChannels(2);
  tag.SetSamplerate(44100);
  tag.SetDuration(gsf.len / 1000);

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
    addonInstance = new CGSFCodec(instance, version);
    return ADDON_STATUS_OK;
  }
  virtual ~CMyAddon() = default;
};

ADDONCREATOR(CMyAddon)
