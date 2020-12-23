/*
 *  Copyright (C) 2005-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "VGMCodec.h"

#include <kodi/Filesystem.h>
#include <kodi/General.h>

extern "C"
{

  static size_t read_VFS(struct _STREAMFILE* streamfile, uint8_t* dest, off_t offset, size_t length)
  {
    VGMContext* ctx = (VGMContext*)streamfile;
    if (ctx && ctx->file)
    {
      ctx->file->Seek(offset, SEEK_SET);
      return ctx->file->Read(dest, length);
    }
    return 0;
  }

  static void close_VFS(struct _STREAMFILE* streamfile)
  {
    VGMContext* ctx = (VGMContext*)streamfile;
    if (ctx && ctx->file)
      delete ctx->file;
    ctx->file = nullptr;
  }

  static size_t get_size_VFS(struct _STREAMFILE* streamfile)
  {
    VGMContext* ctx = (VGMContext*)streamfile;
    if (ctx && ctx->file)
      return ctx->file->GetLength();

    return 0;
  }

  static off_t get_offset_VFS(struct _STREAMFILE* streamfile)
  {
    VGMContext* ctx = (VGMContext*)streamfile;
    if (ctx && ctx->file)
      return ctx->file->GetPosition();

    return 0;
  }

  static void get_name_VFS(struct _STREAMFILE* streamfile, char* buffer, size_t length)
  {
    VGMContext* ctx = (VGMContext*)streamfile;
    if (ctx)
      strcpy(buffer, ctx->name);
  }

  static struct _STREAMFILE* open_VFS(struct _STREAMFILE* streamfile,
                                      const char* const filename,
                                      size_t buffersize)
  {
    VGMContext* ctx = (VGMContext*)streamfile;
    ctx->pos = 0;
    ctx->file = new kodi::vfs::CFile;
    ctx->file->OpenFile(filename, ADDON_READ_CACHED);
    ctx->sf.read = read_VFS;
    ctx->sf.get_size = get_size_VFS;
    ctx->sf.get_offset = get_offset_VFS;
    ctx->sf.get_name = get_name_VFS;
    ctx->sf.open = open_VFS;
    ctx->sf.close = close_VFS;
    strcpy(ctx->name, filename);

    return streamfile;
  }

} /* extern "C" */

//------------------------------------------------------------------------------

bool CVGMCodec::m_loopForEverActive = false;

CVGMCodec::CVGMCodec(KODI_HANDLE instance, const std::string& version)
  : CInstanceAudioDecoder(instance, version)
{
}

CVGMCodec::~CVGMCodec()
{
  if (ctx.stream)
    close_vgmstream(ctx.stream);

  delete ctx.file;

  // Set the static to false only from one where has set it before
  if (m_loopForEverInUse)
    m_loopForEverActive = false;
}

bool CVGMCodec::Init(const std::string& filename,
                     unsigned int filecache,
                     int& channels,
                     int& samplerate,
                     int& bitspersample,
                     int64_t& totaltime,
                     int& bitrate,
                     AudioEngineDataFormat& format,
                     std::vector<AudioEngineChannel>& channellist)
{
  open_VFS((struct _STREAMFILE*)&ctx, filename.c_str(), 0);

  ctx.stream = init_vgmstream_from_STREAMFILE((struct _STREAMFILE*)&ctx);
  if (!ctx.stream)
  {
    close_VFS((struct _STREAMFILE*)&ctx);
    return false;
  }

  channels = ctx.stream->channels;
  samplerate = ctx.stream->sample_rate;
  bitspersample = 16;
  totaltime = ctx.stream->num_samples / ctx.stream->sample_rate * 1000;
  format = AUDIOENGINE_FMT_S16NE;

  // clang-format off
  static std::vector<std::vector<enum AudioEngineChannel>> map = {
    {AUDIOENGINE_CH_FC},
    {AUDIOENGINE_CH_FL, AUDIOENGINE_CH_FR},
    {AUDIOENGINE_CH_FL, AUDIOENGINE_CH_FC, AUDIOENGINE_CH_FR},
    {AUDIOENGINE_CH_FL, AUDIOENGINE_CH_FR, AUDIOENGINE_CH_BL, AUDIOENGINE_CH_BR},
    {AUDIOENGINE_CH_FL, AUDIOENGINE_CH_FC, AUDIOENGINE_CH_FR, AUDIOENGINE_CH_BL, AUDIOENGINE_CH_BR},
    {AUDIOENGINE_CH_FL, AUDIOENGINE_CH_FC, AUDIOENGINE_CH_FR, AUDIOENGINE_CH_BL, AUDIOENGINE_CH_BR, AUDIOENGINE_CH_LFE},
    {AUDIOENGINE_CH_FL, AUDIOENGINE_CH_FC, AUDIOENGINE_CH_FR, AUDIOENGINE_CH_SL, AUDIOENGINE_CH_SR, AUDIOENGINE_CH_BL, AUDIOENGINE_CH_BR},
    {AUDIOENGINE_CH_FL, AUDIOENGINE_CH_FC, AUDIOENGINE_CH_FR, AUDIOENGINE_CH_SL, AUDIOENGINE_CH_SR, AUDIOENGINE_CH_BL, AUDIOENGINE_CH_BR, AUDIOENGINE_CH_LFE}
    };
  // clang-format on

  if (ctx.stream->channels <= 8)
    channellist = map[ctx.stream->channels - 1];

  bitrate = 0;
  m_loopForEver = kodi::GetSettingBoolean("loopforever");
  if (!m_loopForEverActive && m_loopForEver && ctx.stream->loop_flag)
  {
    m_loopForEverActive = true; // Set static to know on others that becomes active
    m_loopForEverInUse =
        true; // Set to class it's own, to know on desctruction that created from here
    kodi::QueueNotification(QUEUE_INFO, "", kodi::GetLocalizedString(30002));
  }

  m_endReached = false;

  return true;
}

int CVGMCodec::ReadPCM(uint8_t* buffer, int size, int& actualsize)
{
  if (m_endReached)
    return -1;

  bool loopForever = m_loopForEver && ctx.stream->loop_flag;
  if (!loopForever)
  {
    int decodePosSamples = size / (2 * ctx.stream->channels);
    if (decodePosSamples + ctx.stream->current_sample > ctx.stream->num_samples)
    {
      size = (decodePosSamples - ctx.stream->num_samples) * (ctx.stream->channels / 2);
      m_endReached = true;
    }
  }

  render_vgmstream((sample*)buffer, size / (2 * ctx.stream->channels), ctx.stream);
  actualsize = size;

  ctx.pos += size;
  return 0;
}

int64_t CVGMCodec::Seek(int64_t time)
{
  int16_t* buffer = new int16_t[576 * ctx.stream->channels];
  if (!buffer)
    return 0;

  long samples_to_do = (long)time * ctx.stream->sample_rate / 1000L;
  if (samples_to_do < ctx.stream->current_sample)
    reset_vgmstream(ctx.stream);
  else
    samples_to_do -= ctx.stream->current_sample;

  while (samples_to_do > 0)
  {
    long l = samples_to_do > 576 ? 576 : samples_to_do;
    render_vgmstream(buffer, l, ctx.stream);
    samples_to_do -= l;
  }
  delete[] buffer;

  return time;
}

bool CVGMCodec::ReadTag(const std::string& filename, kodi::addon::AudioDecoderInfoTag& tag)
{
  open_VFS((struct _STREAMFILE*)&ctx, filename.c_str(), 0);

  ctx.stream = init_vgmstream_from_STREAMFILE((struct _STREAMFILE*)&ctx);
  if (!ctx.stream)
  {
    close_VFS((struct _STREAMFILE*)&ctx);
    return false;
  }

  tag.SetDuration(ctx.stream->num_samples / ctx.stream->sample_rate);
  tag.SetSamplerate(ctx.stream->sample_rate);
  tag.SetChannels(ctx.stream->channels);
  return true;
}

//------------------------------------------------------------------------------

class ATTRIBUTE_HIDDEN CMyAddon : public kodi::addon::CAddonBase
{
public:
  CMyAddon() {}
  ADDON_STATUS CreateInstance(int instanceType,
                              const std::string& instanceID,
                              KODI_HANDLE instance,
                              const std::string& version,
                              KODI_HANDLE& addonInstance) override
  {
    addonInstance = new CVGMCodec(instance, version);
    return ADDON_STATUS_OK;
  }
  ~CMyAddon() override = default;
};

ADDONCREATOR(CMyAddon)
