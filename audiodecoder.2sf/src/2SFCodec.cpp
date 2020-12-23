/*
 *  Copyright (C) 2014-2020 Arne Morten Kvarving
 *  Copyright (C) 2016-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "2SFCodec.h"

#include <algorithm>
#include <iostream>
#include <kodi/Filesystem.h>
#include <stdint.h>
#include <stdio.h>
#include <zlib.h>

extern "C"
{

  static void* psf_file_fopen(void* context, const char* path)
  {
    kodi::vfs::CFile* file = new kodi::vfs::CFile;
    if (!file->OpenFile(path, 0))
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

#define BORK_TIME 0xC0CAC01A
  static unsigned long parse_time_crap(const char* input)
  {
    unsigned long value = 0;
    unsigned long multiplier = 1000;
    const char* ptr = input;
    unsigned long colon_count = 0;

    while (*ptr && ((*ptr >= '0' && *ptr <= '9') || *ptr == ':'))
    {
      colon_count += *ptr == ':';
      ++ptr;
    }
    if (colon_count > 2)
      return BORK_TIME;
    if (*ptr && *ptr != '.' && *ptr != ',')
      return BORK_TIME;
    if (*ptr)
      ++ptr;
    while (*ptr && *ptr >= '0' && *ptr <= '9')
      ++ptr;
    if (*ptr)
      return BORK_TIME;

    ptr = strrchr(input, ':');
    if (!ptr)
      ptr = input;
    for (;;)
    {
      char* end;
      if (ptr != input)
        ++ptr;
      if (multiplier == 1000)
      {
        double temp = atof(ptr);
        if (temp >= 60.0)
          return BORK_TIME;
        value = (long)(temp * 1000.0f);
      }
      else
      {
        unsigned long temp = strtoul(ptr, &end, 10);
        if (temp >= 60 && multiplier < 3600000)
          return BORK_TIME;
        value += temp * multiplier;
      }
      if (ptr == input)
        break;
      ptr -= 2;
      while (ptr > input && *ptr != ':')
        --ptr;
      multiplier *= 60;
    }

    return value;
  }

  static int psf_info_meta(void* context, const char* name, const char* value)
  {
    TSFContext* tsf = (TSFContext*)context;
    if (!strcasecmp(name, "length"))
    {
      int temp = parse_time_crap(value);
      if (temp != BORK_TIME)
      {
        tsf->tagSongMs = temp;
      }
    }
    else if (!strcasecmp(name, "fade"))
    {
      int temp = parse_time_crap(value);
      if (temp != BORK_TIME)
      {
        tsf->tagFadeMs = temp;
      }
    }
    else if (!strcasecmp(name, "replaygain_"))
      tsf->replaygain = value;
    else if (!strcasecmp(name, "title"))
      tsf->title = value;
    else if (!strcasecmp(name, "artist"))
      tsf->artist = value;
    else if (!strcasecmp(name, "copyright"))
      tsf->copyright = value;
    else if (!strcasecmp(name, "year"))
      tsf->year = value;
    else if (!strcasecmp(name, "comment"))
      tsf->comment = value;
    else if (!strcasecmp(name, "game"))
      tsf->game = value;
    else if (!strcasecmp(name, "utf8"))
      tsf->utf8 = true;

    return 0;
  }

  inline unsigned get_le32(void const* p)
  {
    return (unsigned)((unsigned char const*)p)[3] << 24 |
           (unsigned)((unsigned char const*)p)[2] << 16 |
           (unsigned)((unsigned char const*)p)[1] << 8 | (unsigned)((unsigned char const*)p)[0];
  }

  static int load_twosf_map(struct twosf_loader_state* state,
                            int issave,
                            const unsigned char* udata,
                            unsigned usize)
  {
    if (usize < 8)
      return -1;

    unsigned char* iptr;
    size_t isize;
    unsigned char* xptr;
    unsigned xsize = get_le32(udata + 4);
    unsigned xofs = get_le32(udata + 0);
    if (issave)
    {
      iptr = state->state;
      isize = state->state_size;
      state->state = 0;
      state->state_size = 0;
    }
    else
    {
      iptr = state->rom;
      isize = state->rom_size;
      state->rom = 0;
      state->rom_size = 0;
    }
    if (!iptr)
    {
      size_t rsize = xofs + xsize;
      if (!issave)
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
      size_t rsize = xofs + xsize;
      if (!issave)
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
    memcpy(iptr + xofs, udata + 8, xsize);
    if (issave)
    {
      state->state = iptr;
      state->state_size = isize;
    }
    else
    {
      state->rom = iptr;
      state->rom_size = isize;
    }
    return 0;
  }

  static int load_twosf_mapz(struct twosf_loader_state* state,
                             int issave,
                             const unsigned char* zdata,
                             unsigned zsize,
                             unsigned zcrc)
  {
    int ret;
    int zerr;
    uLongf usize = 8;
    uLongf rsize = usize;
    unsigned char* udata;
    unsigned char* rdata;

    udata = (unsigned char*)malloc(usize);
    if (!udata)
      return -1;

    while (Z_OK != (zerr = uncompress(udata, &usize, zdata, zsize)))
    {
      if (Z_MEM_ERROR != zerr && Z_BUF_ERROR != zerr)
      {
        free(udata);
        return -1;
      }
      if (usize >= 8)
      {
        usize = get_le32(udata + 4) + 8;
        if (usize < rsize)
        {
          rsize += rsize;
          usize = rsize;
        }
        else
          rsize = usize;
      }
      else
      {
        rsize += rsize;
        usize = rsize;
      }
      rdata = (unsigned char*)realloc(udata, usize);
      if (!rdata)
      {
        free(udata);
        return -1;
      }
      udata = rdata;
    }

    rdata = (unsigned char*)realloc(udata, usize);
    if (!rdata)
    {
      free(udata);
      return -1;
    }

    if (0)
    {
      uLong ccrc = crc32(crc32(0L, Z_NULL, 0), rdata, (uInt)usize);
      if (ccrc != zcrc)
        return -1;
    }

    ret = load_twosf_map(state, issave, rdata, (unsigned)usize);
    free(rdata);
    return ret;
  }

  static int twosf_loader(void* context,
                          const uint8_t* exe,
                          size_t exe_size,
                          const uint8_t* reserved,
                          size_t reserved_size)
  {
    struct twosf_loader_state* state = (struct twosf_loader_state*)context;

    if (exe_size >= 8)
    {
      if (load_twosf_map(state, 0, exe, (unsigned)exe_size))
        return -1;
    }

    if (reserved_size)
    {
      size_t resv_pos = 0;
      if (reserved_size < 16)
        return -1;
      while (resv_pos + 12 < reserved_size)
      {
        unsigned save_size = get_le32(reserved + resv_pos + 4);
        unsigned save_crc = get_le32(reserved + resv_pos + 8);
        if (get_le32(reserved + resv_pos + 0) == 0x45564153)
        {
          if (resv_pos + 12 + save_size > reserved_size)
            return -1;
          if (load_twosf_mapz(state, 1, reserved + resv_pos + 12, save_size, save_crc))
            return -1;
        }
        resv_pos += 12 + save_size;
      }
    }

    return 0;
  }

  static int twosf_info(void* context, const char* name, const char* value)
  {
    struct twosf_loader_state* state = (struct twosf_loader_state*)context;
    char* end;

    if (!strcasecmp(name, "_frames"))
    {
      state->initial_frames = strtol(value, &end, 10);
    }
    else if (!strcasecmp(name, "_clockdown"))
    {
      state->clockdown = strtol(value, &end, 10);
    }
    else if (!strcasecmp(name, "_vio2sf_sync_type"))
    {
      state->sync_type = strtol(value, &end, 10);
    }
    else if (!strcasecmp(name, "_vio2sf_arm9_clockdown_level"))
    {
      state->arm9_clockdown_level = strtol(value, &end, 10);
    }
    else if (!strcasecmp(name, "_vio2sf_arm7_clockdown_level"))
    {
      state->arm7_clockdown_level = strtol(value, &end, 10);
    }

    return 0;
  }

  static void sf_status(void* context, const char* message)
  {
    if (message == nullptr || strlen(message) <= 1)
      return;

    std::string msg = message;
    std::replace(msg.begin(), msg.end(), '\n', '\0');

    kodi::Log(ADDON_LOG_DEBUG, "psf status: %s", msg.c_str());
  }

} /* extern "C" */

//------------------------------------------------------------------------------

C2SFCodec::C2SFCodec(KODI_HANDLE instance, const std::string& version)
  : CInstanceAudioDecoder(instance, version)
{
}

C2SFCodec::~C2SFCodec()
{
  Shutdown();
}

bool C2SFCodec::Init(const std::string& filename,
                     unsigned int filecache,
                     int& channels,
                     int& samplerate,
                     int& bitspersample,
                     int64_t& totaltime,
                     int& bitrate,
                     AudioEngineDataFormat& format,
                     std::vector<AudioEngineChannel>& channellist)
{
  TSFContext info_state;
  if (psf_load(filename.c_str(), &psf_file_system, 0x24, nullptr, nullptr, psf_info_meta,
               &info_state, 0, sf_status, nullptr) <= 0)
  {
    return false;
  }

  m_cfgSuppressOpeningSilence = kodi::GetSettingBoolean("suppressopeningsilence", true);
  m_cfgSuppressEndSilence = kodi::GetSettingBoolean("suppressendsilence", true);
  m_cfgEndSilenceSeconds = kodi::GetSettingInt("endsilenceseconds", 5);
  m_cfgResamplingQuality = kodi::GetSettingInt("resamplingquality", 4);

  m_tagSongMs = info_state.tagSongMs;
  m_tagFadeMs = info_state.tagFadeMs;

  if (!m_tagSongMs)
  {
    m_tagSongMs = kodi::GetSettingInt("defaultlength", 170) * 1000;
    m_tagFadeMs = kodi::GetSettingInt("defaultfade", 10000);
  }

  m_path = filename;

  if (!Load())
    return false;

  totaltime = m_songLength / m_cfgDefaultSampleRate * 1000 + m_tagFadeMs;
  format = AUDIOENGINE_FMT_S16NE;
  channellist = {AUDIOENGINE_CH_FL, AUDIOENGINE_CH_FR};
  channels = 2;
  bitspersample = 16;
  bitrate = 0.0;
  samplerate = m_cfgDefaultSampleRate;

  return true;
}

bool C2SFCodec::Load()
{
  Shutdown();

  m_emu = new NDS_state();

  if (state_init(m_emu))
    return false;

  if (!m_state.rom && !m_state.state)
  {
    if (psf_load(m_path.c_str(), &psf_file_system, 0x24, twosf_loader, &m_state, twosf_info,
                 &m_state, 1, sf_status, nullptr) < 0)
      return false;

    if (!m_state.arm7_clockdown_level)
      m_state.arm7_clockdown_level = m_state.clockdown;
    if (!m_state.arm9_clockdown_level)
      m_state.arm9_clockdown_level = m_state.clockdown;
  }

  m_emu->dwInterpolation = m_cfgResamplingQuality;
  m_emu->dwChannelMute = 0;

  m_emu->initial_frames = m_state.initial_frames;
  m_emu->sync_type = m_state.sync_type;
  m_emu->arm7_clockdown_level = m_state.arm7_clockdown_level;
  m_emu->arm9_clockdown_level = m_state.arm9_clockdown_level;

  if (m_state.rom)
    state_setrom(m_emu, m_state.rom, m_state.rom_size, 0);
  state_loadstate(m_emu, m_state.state, m_state.state_size);

  m_twosfEmuPosition = 0.;

  m_startSilence = 0;
  m_silence = 0;

  m_eof = false;
  m_dataWritten = 0;
  m_remainder = 0;
  m_posDelta = 0;
  m_noLoop = true;

  calcfade();

  unsigned int skip_max = m_cfgEndSilenceSeconds * m_cfgDefaultSampleRate;

  if (m_cfgSuppressOpeningSilence) // ohcrap
  {
    for (;;)
    {
      unsigned int skip_howmany = skip_max - m_silence;
      if (skip_howmany > 1024)
        skip_howmany = 1024;
      m_sampleBuffer.resize(skip_howmany * 2);
      state_render(m_emu, m_sampleBuffer.data(), skip_howmany);
      int16_t* foo = m_sampleBuffer.data();
      unsigned int i;
      for (i = 0; i < skip_howmany; ++i)
      {
        if (foo[0] || foo[1])
          break;
        foo += 2;
      }
      m_silence += i;
      if (i < skip_howmany)
      {
        m_remainder = skip_howmany - i;
        memmove(m_sampleBuffer.data(), foo, m_remainder * sizeof(int16_t) * 2);
        break;
      }
      if (m_silence >= skip_max)
      {
        m_eof = true;
        break;
      }
    }

    m_startSilence += m_silence;
    m_silence = 0;
  }

  if (m_cfgSuppressEndSilence)
    m_silenceTestBuffer.resize(skip_max * 2);

  return true;
}

void C2SFCodec::Shutdown()
{
  if (m_emu)
  {
    state_deinit(m_emu);
    delete m_emu;
    m_emu = nullptr;
  }
}

int C2SFCodec::ReadPCM(uint8_t* buffer, int size, int& actualsize)
{
  if (m_eof && !m_silenceTestBuffer.data_available())
    return 1;

  if (m_noLoop && m_tagSongMs &&
      (m_posDelta + mul_div(m_dataWritten, 1000, m_cfgDefaultSampleRate)) >=
          m_tagSongMs + m_tagFadeMs)
    return -1;

  unsigned int written = 0;

  int usedSize = size / 2 / sizeof(int16_t);

  int samples;

  if (m_noLoop)
  {
    samples = (m_songLength + m_fadeLength) - m_dataWritten;
    if (samples > usedSize)
      samples = usedSize;
  }
  else
  {
    samples = usedSize;
  }

  short* ptr;

  if (m_cfgSuppressEndSilence)
  {
    m_sampleBuffer.resize(usedSize * 2);

    if (!m_eof)
    {
      unsigned int free_space = m_silenceTestBuffer.free_space() / 2;
      while (free_space)
      {
        unsigned int samples_to_render;
        if (m_remainder)
        {
          samples_to_render = m_remainder;
          if (samples_to_render > free_space)
            samples_to_render = free_space;
          m_remainder -= samples_to_render;
        }
        else
        {
          samples_to_render = free_space;
          if (samples_to_render > usedSize)
            samples_to_render = usedSize;
          state_render(m_emu, m_sampleBuffer.data(), samples_to_render);
        }
        m_silenceTestBuffer.write(m_sampleBuffer.data(), samples_to_render * 2);
        free_space -= samples_to_render;
        if (m_remainder)
        {
          memmove(m_sampleBuffer.data(), m_sampleBuffer.data() + samples_to_render * 2,
                  m_remainder * sizeof(short) * 2);
        }
      }
    }

    if (m_silenceTestBuffer.test_silence())
    {
      m_eof = true;
      return -1;
    }

    written = m_silenceTestBuffer.data_available() / 2;
    if (written > samples)
      written = samples;
    m_sampleBuffer.resize((written + m_remainder) * 2);
    m_silenceTestBuffer.read(m_sampleBuffer.data(), written * 2);
    ptr = m_sampleBuffer.data() + m_remainder * 2;
  }
  else
  {
    m_sampleBuffer.resize(samples * 2);

    if (m_remainder)
    {
      written = m_remainder;
      m_remainder = 0;
    }
    else
    {
      written = samples;
      state_render(m_emu, m_sampleBuffer.data(), written);
    }

    ptr = m_sampleBuffer.data();
  }

  m_twosfEmuPosition += double(written) / double(m_cfgDefaultSampleRate);

  int d_start, d_end;
  d_start = m_dataWritten;
  m_dataWritten += written;
  d_end = m_dataWritten;

  if (m_tagSongMs && d_end > m_songLength && m_noLoop)
  {
    int16_t* foo = m_sampleBuffer.data();
    for (int n = d_start; n < d_end; ++n)
    {
      if (n > m_songLength)
      {
        if (n > m_songLength + m_fadeLength)
        {
          *(uint32_t*)foo = 0;
        }
        else
        {
          int bleh = m_songLength + m_fadeLength - n;
          foo[0] = mul_div(foo[0], bleh, m_fadeLength);
          foo[1] = mul_div(foo[1], bleh, m_fadeLength);
        }
      }
      foo += 2;
    }
  }

  if (!written)
  {
    m_eof = true;
    return -1;
  }


  actualsize = written * 2 * sizeof(int16_t);
  memcpy(buffer, ptr, actualsize);

  return 0;
}

int64_t C2SFCodec::Seek(int64_t time)
{
  double p_seconds = double(time) / 1000.0;
  m_eof = false;

  double buffered_time =
      (double)(m_silenceTestBuffer.data_available() / 2) / double(m_cfgDefaultSampleRate);

  m_twosfEmuPosition += buffered_time;

  m_silenceTestBuffer.reset();

  if (p_seconds < m_twosfEmuPosition)
  {
    Load();
  }
  unsigned int howmany =
      (int)(time_to_samples(p_seconds - m_twosfEmuPosition, m_cfgDefaultSampleRate));

  if (howmany <= m_remainder)
  {
    m_remainder -= howmany;
    memmove(m_sampleBuffer.data(), m_sampleBuffer.data() + howmany * 2,
            m_remainder * sizeof(short) * 2);
    howmany = 0;
  }
  else if (m_remainder)
  {
    howmany -= m_remainder;
    m_remainder = 0;
  }

  // more abortable, and emu doesn't like doing huge numbers of samples per call anyway
  int16_t buf[2048];
  while (howmany)
  {
    unsigned int samples = (howmany > 1024) ? 1024 : howmany;
    state_render(m_emu, buf, samples);
    howmany -= samples;
  }

  m_dataWritten = 0;
  m_posDelta = (int)(p_seconds * 1000.);
  m_twosfEmuPosition = p_seconds;

  calcfade();

  return time;
}

bool C2SFCodec::ReadTag(const std::string& file, kodi::addon::AudioDecoderInfoTag& tag)
{
  TSFContext info_state;
  if (psf_load(file.c_str(), &psf_file_system, 0x24, nullptr, nullptr, psf_info_meta, &info_state,
               0, sf_status, nullptr) <= 0)
  {
    return false;
  }

  tag.SetTitle(info_state.title);
  if (!info_state.artist.empty())
    tag.SetArtist(info_state.artist);
  else
    tag.SetArtist(info_state.game);
  tag.SetAlbum(info_state.game);
  tag.SetReleaseDate(info_state.year);
  tag.SetComment(info_state.comment);
  tag.SetDuration((info_state.tagSongMs + info_state.tagFadeMs) / 1000);
  tag.SetSamplerate(m_cfgDefaultSampleRate);
  tag.SetChannels(2);

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
    addonInstance = new C2SFCodec(instance, version);
    return ADDON_STATUS_OK;
  }
  virtual ~CMyAddon() = default;
};

ADDONCREATOR(CMyAddon)
