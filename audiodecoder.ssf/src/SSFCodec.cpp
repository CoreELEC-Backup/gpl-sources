/*
 *  Copyright (C) 2014-2020 Arne Morten Kvarving
 *  Copyright (C) 2019-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

// Code based upon https://bitbucket.org/losnoco/foo_input_ht

#include "SSFCodec.h"

bool CSSFCodec::m_gInitialized = false;
std::mutex CSSFCodec::m_gSyncMutex;

extern "C"
{

  inline unsigned get_le32(void const* p)
  {
    return (unsigned)((unsigned char const*)p)[3] << 24 |
           (unsigned)((unsigned char const*)p)[2] << 16 |
           (unsigned)((unsigned char const*)p)[1] << 8 | (unsigned)((unsigned char const*)p)[0];
  }

  static int sdsf_load(void* context,
                       const uint8_t* exe,
                       size_t exe_size,
                       const uint8_t* reserved,
                       size_t reserved_size)
  {
    if (exe_size < 4)
      return -1;

    sdsf_load_state* state = static_cast<sdsf_load_state*>(context);

    std::vector<uint8_t>& dst = state->state;

    if (dst.size() < 4)
    {
      dst.resize(exe_size);
      memcpy(&dst[0], exe, exe_size);
      return 0;
    }

    uint32_t dst_start = get_le32(&dst[0]);
    uint32_t src_start = get_le32(exe);
    dst_start &= 0x7FFFFF;
    src_start &= 0x7FFFFF;
    size_t dst_len = dst.size() - 4;
    size_t src_len = exe_size - 4;
    if (dst_len > 0x800000)
      dst_len = 0x800000;
    if (src_len > 0x800000)
      src_len = 0x800000;

    if (src_start < dst_start)
    {
      size_t diff = dst_start - src_start;
      dst.resize(dst_len + 4 + diff);
      memmove(&dst[0] + 4 + diff, &dst[0] + 4, dst_len);
      memset(&dst[0] + 4, 0, diff);
      dst_len += diff;
      dst_start = src_start;
      *(uint32_t*)(&dst[0]) = get_le32(&dst_start);
    }
    if ((src_start + src_len) > (dst_start + dst_len))
    {
      size_t diff = (src_start + src_len) - (dst_start + dst_len);
      dst.resize(dst_len + 4 + diff);
      memset(&dst[0] + 4 + dst_len, 0, diff);
      dst_len += diff;
    }

    memcpy(&dst[0] + 4 + (src_start - dst_start), exe + 4, src_len);

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
        double temp = std::stod(ptr);
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
    psf_info_meta_state* state = static_cast<psf_info_meta_state*>(context);

    if (!strcasecmp(name, "artist") && state->artist.empty())
    {
      state->artist = value;
    }
    else if (!strcasecmp(name, "game"))
    {
      state->game = value;
    }
    else if (!strcasecmp(name, "title"))
    {
      state->title = value;
    }
    else if (!strcasecmp(name, "year"))
    {
      state->year = value;
    }
    else if (!strcasecmp(name, "genre"))
    {
      state->genre = value;
    }
    else if (!strcasecmp(name, "comment"))
    {
      state->comment = value;
    }
    else if (!strcasecmp(name, "replaygain_"))
    {
      state->replaygain = value;
    }
    else if (!strcasecmp(name, "length"))
    {
      int temp = parse_time_crap(value);
      if (temp != BORK_TIME)
      {
        state->tagSongMs = temp;
      }
    }
    else if (!strcasecmp(name, "fade"))
    {
      int temp = parse_time_crap(value);
      if (temp != BORK_TIME)
      {
        state->tagFadeMs = temp;
      }
    }
    else if (!strcasecmp(name, "utf8"))
    {
      state->utf8 = true;
    }
    else if (!strcasecmp(name, "_lib"))
    {
      // Unused, checked to prevent on next error
    }
    else if (name[0] == '_')
    {
      kodi::Log(ADDON_LOG_WARNING, "Unsupported tag found: '%s', required to play file", name);
      return -1;
    }

    return 0;
  }

} // extern "C"

//------------------------------------------------------------------------------

CSSFCodec::CSSFCodec(KODI_HANDLE instance, const std::string& version)
  : CInstanceAudioDecoder(instance, version)
{
}

CSSFCodec::~CSSFCodec()
{
  if (m_segaState.empty())
    return;

  void* yam = nullptr;
  if (m_xsfVersion == 0x12)
  {
    void* dcsound = sega_get_dcsound_state(m_segaState.data());
    yam = dcsound_get_yam_state(dcsound);
  }
  else
  {
    void* satsound = sega_get_satsound_state(m_segaState.data());
    yam = satsound_get_yam_state(satsound);
  }
  if (yam)
    yam_unprepare_dynacode(yam);
}

bool CSSFCodec::Init(const std::string& filename,
                     unsigned int filecache,
                     int& channels,
                     int& samplerate,
                     int& bitspersample,
                     int64_t& totaltime,
                     int& bitrate,
                     AudioEngineDataFormat& format,
                     std::vector<AudioEngineChannel>& channellist)
{
  m_path = filename;
  m_xsfVersion = psf_load(m_path.c_str(), &psf_file_system, 0, nullptr, nullptr, nullptr, nullptr,
                          0, SSFPrintMessage, this);
  if (m_xsfVersion <= 0 || (m_xsfVersion != 0x11 && m_xsfVersion != 0x12))
  {
    kodi::Log(ADDON_LOG_ERROR, "%s: Not a SSF or PSF file '%s'", __func__, m_path.c_str());
    return false;
  }

  psf_info_meta_state info_state;
  int ret = psf_load(m_path.c_str(), &psf_file_system, m_xsfVersion, nullptr, nullptr,
                     psf_info_meta, &info_state, 0, SSFPrintMessage, this);
  if (ret <= 0)
  {
    kodi::Log(ADDON_LOG_ERROR, "%s: Failed to load tags from '%s'", __func__, m_path.c_str());
    return false;
  }

  kodi::CheckSettingBoolean("suppressopeningsilence", m_cfgSuppressOpeningSilence);
  kodi::CheckSettingBoolean("suppressendsilence", m_cfgSuppressEndSilence);
  kodi::CheckSettingInt("endsilenceseconds", m_cfgEndSilenceSeconds);
  kodi::CheckSettingBoolean("dry", m_cfgDry);
  kodi::CheckSettingBoolean("dsp", m_cfgDSP);
  kodi::CheckSettingBoolean("dspdynamicrec", m_cfgDSPDynamicRec);

  m_tagSongMs = info_state.tagSongMs;
  m_tagFadeMs = info_state.tagFadeMs;

  if (!m_tagSongMs)
  {
    m_tagSongMs = kodi::GetSettingInt("defaultlength") * 1000;
    m_tagFadeMs = kodi::GetSettingInt("defaultfade");
  }

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

bool CSSFCodec::Load()
{
  {
    std::lock_guard<std::mutex> lock(m_gSyncMutex);
    if (!m_gInitialized)
    {
      if (sega_init())
      {
        kodi::Log(ADDON_LOG_ERROR, "%s: Sega emulator static initialization failed", __func__);
        return false;
      }
      m_gInitialized = true;
    }
  }

  if (!m_segaState.empty())
  {
    void* yam = nullptr;
    if (m_xsfVersion == 0x12)
    {
      void* dcsound = sega_get_dcsound_state(m_segaState.data());
      yam = dcsound_get_yam_state(dcsound);
    }
    else
    {
      void* satsound = sega_get_satsound_state(m_segaState.data());
      yam = satsound_get_yam_state(satsound);
    }
    if (yam)
      yam_unprepare_dynacode(yam);
  }

  m_segaState.resize(sega_get_state_size(m_xsfVersion - 0x10));

  void* pEmu = m_segaState.data();

  sega_clear_state(pEmu, m_xsfVersion - 0x10);

  sega_enable_dry(pEmu, m_cfgDry ? 1 : !m_cfgDSP);
  sega_enable_dsp(pEmu, m_cfgDSP);
  sega_enable_dsp_dynarec(pEmu, m_cfgDSPDynamicRec);

  if (m_cfgDSPDynamicRec)
  {
    void* yam = 0;
    if (m_xsfVersion == 0x12)
    {
      void* dcsound = sega_get_dcsound_state(pEmu);
      yam = dcsound_get_yam_state(dcsound);
    }
    else
    {
      void* satsound = sega_get_satsound_state(pEmu);
      yam = satsound_get_yam_state(satsound);
    }
    if (yam)
      yam_prepare_dynacode(yam);
  }

  sdsf_load_state state;
  int ret = psf_load(m_path.c_str(), &psf_file_system, m_xsfVersion, sdsf_load, &state, nullptr,
                     nullptr, 0, SSFPrintMessage, this);
  if (ret < 0)
  {
    kodi::Log(ADDON_LOG_ERROR, "%s: Invalid SSF/DSF from '%s'", __func__, m_path.c_str());
    return false;
  }

  uint32_t start = get_le32(state.state.data());
  size_t length = state.state.size();
  size_t max_length = (m_xsfVersion == 0x12) ? 0x800000 : 0x80000;
  if ((start + (length - 4)) > max_length)
  {
    length = max_length - start + 4;
  }
  sega_upload_program(pEmu, state.state.data(), length);

  m_xsfEmuPosition = 0.;

  m_startSilence = 0;
  m_silence = 0;

  m_eof = false;
  m_dataWritten = 0;
  m_remainder = 0;
  m_posDelta = 0;
  m_xsfEmuPosition = 0;
  m_noLoop = true;

  calcfade();

  unsigned int skip_max = m_cfgEndSilenceSeconds * m_cfgDefaultSampleRate;

  if (m_cfgSuppressOpeningSilence) // ohcrap
  {
    for (;;)
    {
      unsigned int skip_howmany = skip_max - m_silence;
      if (skip_howmany > 8192)
        skip_howmany = 8192;
      m_sampleBuffer.resize(skip_howmany * 2);
      int rtn = sega_execute(pEmu, 0x7FFFFFFF, m_sampleBuffer.data(), &skip_howmany);
      if (rtn < 0)
      {
        kodi::Log(ADDON_LOG_ERROR, "%s: Failed to call 'sega_execute'", __func__);
        return false;
      }
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

int CSSFCodec::ReadPCM(uint8_t* buffer, int size, int& actualsize)
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
          m_remainder = 0;
        }
        else
        {
          samples_to_render = free_space;
          if (samples_to_render > usedSize)
            samples_to_render = usedSize;
          int err = sega_execute(m_segaState.data(), 0x7FFFFFFF, m_sampleBuffer.data(),
                                 &samples_to_render);
          if (err < 0 || !samples_to_render)
          {
            kodi::Log(ADDON_LOG_ERROR, "%s: Execution halted with an error", __func__);
            return 1;
          }
        }
        m_silenceTestBuffer.write(m_sampleBuffer.data(), samples_to_render * 2);
        free_space -= samples_to_render;
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
    m_silenceTestBuffer.read(m_sampleBuffer.data(), written * 2);
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
      int err = sega_execute(m_segaState.data(), 0x7FFFFFFF, m_sampleBuffer.data(), &written);
      if (err < 0 || !written)
      {
        kodi::Log(ADDON_LOG_ERROR, "%s: Execution halted with an error", __func__);
        return 1;
      }
    }
  }

  m_xsfEmuPosition += double(written) / double(m_cfgDefaultSampleRate);

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
  memcpy(buffer, m_sampleBuffer.data(), actualsize);

  return 0;
}

int64_t CSSFCodec::Seek(int64_t time)
{
  double p_seconds = double(time) / 1000.0;
  m_eof = false;

  double buffered_time =
      (double)(m_silenceTestBuffer.data_available() / 2) / double(m_cfgDefaultSampleRate);

  m_xsfEmuPosition += buffered_time;

  m_silenceTestBuffer.reset();

  if (p_seconds < m_xsfEmuPosition)
  {
    Load();
  }
  unsigned int howmany =
      (int)(time_to_samples(p_seconds - m_xsfEmuPosition, m_cfgDefaultSampleRate));

  // more abortable, and emu doesn't like doing huge numbers of samples per call anyway
  void* pEmu = m_segaState.data();
  while (howmany)
  {
    unsigned todo = howmany;
    if (todo > 2048)
      todo = 2048;
    int rtn = sega_execute(pEmu, 0x7FFFFFFF, 0, &todo);
    if (rtn < 0 || !todo)
    {
      m_eof = true;
      return -1;
    }
    howmany -= todo;
  }

  m_dataWritten = 0;
  m_posDelta = (int)(p_seconds * 1000.);
  m_xsfEmuPosition = p_seconds;

  calcfade();

  return time;
}

bool CSSFCodec::ReadTag(const std::string& filename, kodi::addon::AudioDecoderInfoTag& tag)
{
  int xsfVersion = psf_load(filename.c_str(), &psf_file_system, 0, nullptr, nullptr, nullptr,
                            nullptr, 0, SSFPrintMessage, this);
  if (xsfVersion <= 0 || (xsfVersion != 0x11 && xsfVersion != 0x12))
  {
    kodi::Log(ADDON_LOG_ERROR, "%s: Not a SSF or PSF file '%s'", __func__, m_path.c_str());
    return false;
  }

  psf_info_meta_state info_state;
  if (psf_load(filename.c_str(), &psf_file_system, xsfVersion, nullptr, nullptr, psf_info_meta,
               &info_state, 0, SSFPrintMessage, this) <= 0)
  {
    kodi::Log(ADDON_LOG_ERROR, "%s: Failed to load %s information from '%s'", __func__,
              xsfVersion == 0x11 ? "SSF" : "DSF", filename.c_str());
    return false;
  }

  tag.SetTitle(info_state.title);
  if (!info_state.artist.empty())
    tag.SetArtist(info_state.artist);
  else
    tag.SetArtist(info_state.game);
  tag.SetAlbum(info_state.game);
  tag.SetGenre(info_state.genre);
  tag.SetReleaseDate(info_state.year);
  tag.SetComment(info_state.comment);
  tag.SetDuration((info_state.tagSongMs + info_state.tagFadeMs) / 1000);

  return true;
}

void CSSFCodec::SSFPrintMessage(void* context, const char* message)
{
  kodi::Log(ADDON_LOG_DEBUG, "NCFS codec message: '%s'", message);
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
    addonInstance = new CSSFCodec(instance, version);
    return ADDON_STATUS_OK;
  }
  virtual ~CMyAddon() = default;
};

ADDONCREATOR(CMyAddon)
