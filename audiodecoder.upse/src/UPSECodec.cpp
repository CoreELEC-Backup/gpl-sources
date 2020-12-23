/*
 *  Copyright (C) 2014-2020 Arne Morten Kvarving
 *  Copyright (C) 2016-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "UPSECodec.h"

#include <kodi/Filesystem.h>

extern "C"
{

  void* kodi_vfs_open(const char* path, const char* mode)
  {
    kodi::vfs::CFile* file = new kodi::vfs::CFile;
    if (!file->OpenFile(path, 0))
    {
      delete file;
      return nullptr;
    }
    return file;
  }

  size_t kodi_vfs_read(void* ptr, size_t size, size_t nmemb, void* file)
  {
    kodi::vfs::CFile* cfile = static_cast<kodi::vfs::CFile*>(file);
    return cfile->Read(ptr, size * nmemb) / size;
  }

  int kodi_vfs_seek(void* file, long offset, int whence)
  {
    kodi::vfs::CFile* cfile = static_cast<kodi::vfs::CFile*>(file);
    return cfile->Seek(offset, whence);
  }

  int kodi_vfs_close(void* file)
  {
    delete static_cast<kodi::vfs::CFile*>(file);
    return 0;
  }

  long kodi_vfs_tell(void* file)
  {
    kodi::vfs::CFile* cfile = static_cast<kodi::vfs::CFile*>(file);
    return cfile->GetPosition();
  }

  const upse_iofuncs_t upse_io = {kodi_vfs_open, kodi_vfs_read, kodi_vfs_seek, kodi_vfs_close,
                                  kodi_vfs_tell};

} /* extern "C" */

//------------------------------------------------------------------------------

CUPSECodec::CUPSECodec(KODI_HANDLE instance, const std::string& version)
  : CInstanceAudioDecoder(instance, version)
{
}

CUPSECodec::~CUPSECodec()
{
  if (ctx.mod)
  {
    upse_eventloop_stop(ctx.mod);
    if (!m_endWasReached)
      upse_eventloop_render(ctx.mod, (int16_t**)&ctx.buf);
    upse_module_close(ctx.mod);
  }
}

bool CUPSECodec::Init(const std::string& filename,
                      unsigned int filecache,
                      int& channels,
                      int& samplerate,
                      int& bitspersample,
                      int64_t& totaltime,
                      int& bitrate,
                      AudioEngineDataFormat& format,
                      std::vector<AudioEngineChannel>& channellist)
{
  upse_module_init();
  upse_module_t* upse = upse_module_open(filename.c_str(), &upse_io);
  if (!upse)
  {
    m_endWasReached = true;
    return false;
  }

  ctx.mod = upse;
  ctx.size = 0;
  ctx.head = ctx.buf;

  upse_spu_state_t* p_spu = reinterpret_cast<upse_spu_state_t*>(upse->instance.spu);
  upse_ps1_spu_setvolume(p_spu, 32);

  totaltime = upse->metadata->length;
  format = AUDIOENGINE_FMT_S16NE;
  channellist = {AUDIOENGINE_CH_FL, AUDIOENGINE_CH_FR};
  channels = 2;
  bitspersample = 16;
  bitrate = 0.0;
  samplerate = 44100;

  return true;
}

int CUPSECodec::ReadPCM(uint8_t* buffer, int size, int& actualsize)
{
  if (ctx.size == 0)
  {
    ctx.size = 4 * upse_eventloop_render(ctx.mod, (int16_t**)&ctx.buf);
    ctx.head = ctx.buf;

    // If return against 0, the end of stream is reached
    if (ctx.size == 0)
    {
      m_endWasReached = true;
      return 1;
    }
  }
#undef min
  actualsize = std::min(ctx.size, size);
  memcpy(buffer, ctx.head, actualsize);
  ctx.head += actualsize / 2;
  ctx.size -= actualsize;
  return 0;
}

int64_t CUPSECodec::Seek(int64_t time)
{
  upse_eventloop_seek(ctx.mod, time);
  return time;
}

bool CUPSECodec::ReadTag(const std::string& filename, kodi::addon::AudioDecoderInfoTag& tag)
{
  upse_psf_t* upseTag = upse_get_psf_metadata(filename.c_str(), &upse_io);
  if (upseTag)
  {
    if (upseTag->title && strcmp(upseTag->title, "n/a") != 0 && strcmp(upseTag->title, "-") != 0)
      tag.SetTitle(upseTag->title);
    if (upseTag->artist && strcmp(upseTag->artist, "n/a") != 0 && strcmp(upseTag->artist, "-") != 0)
      tag.SetArtist(upseTag->artist);
    if (upseTag->game && strcmp(upseTag->game, "n/a") != 0 && strcmp(upseTag->game, "-") != 0)
    {
      tag.SetAlbum(upseTag->game);
      if (tag.GetArtist().empty())
        tag.SetArtist(upseTag->game);
    }
    if (upseTag->year && strcmp(upseTag->year, "n/a") != 0 && strcmp(upseTag->year, "-") != 0)
      tag.SetReleaseDate(upseTag->year);
    if (upseTag->comment && strcmp(upseTag->comment, "n/a") != 0 && strcmp(upseTag->comment, "-") != 0)
      tag.SetComment(upseTag->comment);
    tag.SetDuration(upseTag->length / 1000);
    tag.SetSamplerate(44100);
    tag.SetChannels(2);
    delete upseTag;
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------

class ATTRIBUTE_HIDDEN CMyAddon : public kodi::addon::CAddonBase
{
public:
  CMyAddon() = default;
  virtual ADDON_STATUS CreateInstance(int instanceType,
                                      const std::string& instanceID,
                                      KODI_HANDLE instance,
                                      const std::string& version,
                                      KODI_HANDLE& addonInstance) override
  {
    addonInstance = new CUPSECodec(instance, version);
    return ADDON_STATUS_OK;
  }
  virtual ~CMyAddon() = default;
};

ADDONCREATOR(CMyAddon)
