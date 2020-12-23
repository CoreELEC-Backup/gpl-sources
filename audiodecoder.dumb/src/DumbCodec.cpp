/*
 *  Copyright (C) 2014-2020 Arne Morten Kvarving
 *  Copyright (C) 2016-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "DumbCodec.h"

#include <kodi/Filesystem.h>

extern "C"
{
#include <dumb.h>

  struct dumbfile_mem_status
  {
    const uint8_t* ptr;
    unsigned offset, size;

    dumbfile_mem_status() : offset(0), size(0), ptr(NULL) {}

    ~dumbfile_mem_status() { delete[] ptr; }
  };

  static int dumbfile_mem_skip(void* f, dumb_off_t n)
  {
    dumbfile_mem_status* s = (dumbfile_mem_status*)f;
    s->offset += n;
    if (s->offset > s->size)
    {
      s->offset = s->size;
      return 1;
    }

    return 0;
  }

  static int dumbfile_mem_getc(void* f)
  {
    dumbfile_mem_status* s = (dumbfile_mem_status*)f;
    if (s->offset < s->size)
    {
      return *(s->ptr + s->offset++);
    }
    return -1;
  }

  static dumb_ssize_t dumbfile_mem_getnc(char* ptr, size_t n, void* f)
  {
    dumbfile_mem_status* s = (dumbfile_mem_status*)f;
    size_t max = s->size - s->offset;
    if (max > n)
      max = n;
    if (max)
    {
      memcpy(ptr, s->ptr + s->offset, max);
      s->offset += max;
    }
    return max;
  }

  static int dumbfile_mem_seek(void* f, dumb_off_t n)
  {
    dumbfile_mem_status* s = (dumbfile_mem_status*)f;
    if (n < 0 || n > s->size)
      return -1;
    s->offset = n;
    return 0;
  }

  static dumb_off_t dumbfile_mem_get_size(void* f)
  {
    dumbfile_mem_status* s = (dumbfile_mem_status*)f;
    return s->size;
  }

  static DUMBFILE_SYSTEM mem_dfs = {NULL, // open
                                    &dumbfile_mem_skip,
                                    &dumbfile_mem_getc,
                                    &dumbfile_mem_getnc,
                                    NULL, // close
                                    &dumbfile_mem_seek,
                                    &dumbfile_mem_get_size};

} /* extern "C" */

//------------------------------------------------------------------------------

CDumbCodec::CDumbCodec(KODI_HANDLE instance, const std::string& version)
  : CInstanceAudioDecoder(instance, version)
{
}

CDumbCodec::~CDumbCodec()
{
  // Free up resources and exit.
  if (m_sigSamples)
    destroy_sample_buffer(m_sigSamples);
  if (m_renderer)
    duh_end_sigrenderer(m_renderer);
  if (m_module)
    unload_duh(m_module);
}

bool CDumbCodec::Init(const std::string& filename,
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

  dumbfile_mem_status memdata;
  memdata.size = file.GetLength();
  memdata.ptr = new uint8_t[memdata.size];
  file.Read(const_cast<uint8_t*>(memdata.ptr), memdata.size);
  file.Close();

  DUMBFILE* f = dumbfile_open_ex(&memdata, &mem_dfs);
  if (!f)
  {
    delete[] memdata.ptr;
    return false;
  }

  m_module = dumb_read_any(f, -1, -1);

  dumbfile_close(f);
  if (!m_module)
    return false;

  m_renderer = duh_start_sigrenderer(m_module, 0, 2, 0);
  if (!m_renderer)
    return false;

  channels = 2;
  samplerate = m_samplerate;
  bitspersample = 16;
  m_totaltime = totaltime = duh_get_length(m_module) / 65536 * 1000;
  format = AUDIOENGINE_FMT_S16NE;
  channellist = {AUDIOENGINE_CH_FL, AUDIOENGINE_CH_FR};

  bitrate = duh_sigrenderer_get_n_channels(m_renderer);
  return true;
}

int CDumbCodec::ReadPCM(uint8_t* buffer, int size, int& actualsize)
{
  // Read samples from libdumb save them to the SDL buffer. Note that we are
  // reading SAMPLES, not bytes!
  int r_samples = size / 4;
  actualsize = duh_render_int(m_renderer, &m_sigSamples, &m_sigSamplesSize, 16, 0, 1.0f,
                              65536.0f / m_samplerate, r_samples, buffer) *
               4;

  // Get current position from libdumb for the playback display. If we get
  // position that is 0, it probably means that the song ended and
  // duh_sigrenderer_get_position points to the start of the file.
  m_position = duh_sigrenderer_get_position(m_renderer);
  if (m_position == 0)
    m_position = m_totaltime;

  if (actualsize == 0)
    return -1;

  return 0;
}

int64_t CDumbCodec::Seek(int64_t time)
{
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
    addonInstance = new CDumbCodec(instance, version);
    return ADDON_STATUS_OK;
  }
  virtual ~CMyAddon() = default;
};

ADDONCREATOR(CMyAddon)
