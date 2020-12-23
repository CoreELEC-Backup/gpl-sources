/*
 *  Copyright (C) 2014-2020 Arne Morten Kvarving
 *  Copyright (C) 2019-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "NCSFCodec.h"

#include <algorithm>
#include <iostream>
#include <kodi/Filesystem.h>

extern "C"
{
#include "psflib/psflib.h"

#include <stdint.h>
#include <stdio.h>

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

  inline unsigned get_le32(void const* p)
  {
    return (unsigned)((unsigned char const*)p)[3] << 24 |
           (unsigned)((unsigned char const*)p)[2] << 16 |
           (unsigned)((unsigned char const*)p)[1] << 8 | (unsigned)((unsigned char const*)p)[0];
  }

} // extern "C"

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

//------------------------------------------------------------------------------

bool CNCSFCodec::Init(const std::string& filename,
                      unsigned int filecache,
                      int& channels,
                      int& samplerate,
                      int& bitspersample,
                      int64_t& totaltime,
                      int& bitrate,
                      AudioEngineDataFormat& format,
                      std::vector<AudioEngineChannel>& channellist)
{
  m_file = filename;

  NCSFContext ctx;
  int ret = psf_load(m_file.c_str(), &psf_file_system, 0x25, nullptr, nullptr, NCFSInfoMeta, &ctx,
                     0, NCFSPrintMessage, this);
  if (ret <= 0)
  {
    kodi::Log(ADDON_LOG_ERROR, "%s: Not an NCSF file (%s)", __func__, m_file.c_str());
    return false;
  }

  m_tagSongMs = ctx.tagSongMs;
  m_tagFadeMs = ctx.tagFadeMs;

  kodi::CheckSettingBoolean("suppressopeningsilence", m_cfgSuppressOpeningSilence);
  kodi::CheckSettingBoolean("suppressendsilence", m_cfgSuppressEndSilence);
  kodi::CheckSettingInt("endsilenceseconds", m_cfgEndSilenceSeconds);

  if (!m_tagSongMs)
  {
    m_tagSongMs = kodi::GetSettingInt("defaultlength") * 1000;
    m_tagFadeMs = kodi::GetSettingInt("defaultfade");
  }

  if (!Load())
    return false;

  totaltime = m_tagSongMs;

  format = AUDIOENGINE_FMT_S16NE;
  channellist = {AUDIOENGINE_CH_FL, AUDIOENGINE_CH_FR};
  channels = 2;
  bitspersample = 16;
  bitrate = 0.0;
  samplerate = m_cfgDefaultSampleRate;

  return true;
}

bool CNCSFCodec::Load()
{
  if (m_sseq.sdatData.empty())
  {
    int ret = psf_load(m_file.c_str(), &psf_file_system, 0x25, NCSFLoader, &m_sseq, nullptr,
                       nullptr, 0, NCFSPrintMessage, this);
    if (ret <= 0)
    {
      kodi::Log(ADDON_LOG_ERROR, "%s: Not an NCSF file (%s)", __func__, m_file.c_str());
      return false;
    }
  }
  else
  {
    m_player.Stop(true);
  }

  PseudoFile file;
  file.data = &m_sseq.sdatData;

  m_sseq.sdat.reset(new SDAT(file, m_sseq.sseq));

  auto* sseqToPlay = m_sseq.sdat->sseq.get();

  m_player.sseqVol = Cnv_Scale(sseqToPlay->info.vol);
  m_player.sampleRate = m_cfgDefaultSampleRate;
  m_player.interpolation = INTERPOLATION_SINC;
  m_player.Setup(sseqToPlay);
  m_player.Timer();

  m_startSilence = 0;
  m_silence = 0;

  m_eof = false;
  m_dataWritten = 0;
  m_remainder = 0;
  m_posDelta = 0;
  m_pos = 0;

  calcfade();

  m_sampleBuffer.resize(4096 * 2 * sizeof(int16_t), 0);

  unsigned skip_max = m_cfgEndSilenceSeconds * m_player.sampleRate;

  if (m_cfgSuppressOpeningSilence) // ohcrap
  {
    for (;;)
    {
      unsigned int skip_howmany = skip_max - m_silence;
      unsigned int unskippable = 0;
      if (skip_howmany > 1024)
        skip_howmany = 1024;
      m_player.GenerateSamples(m_sampleBuffer, 0, skip_howmany);
      int16_t* foo = reinterpret_cast<int16_t*>(m_sampleBuffer.data());
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
        m_remainder = skip_howmany - i + unskippable;
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

int CNCSFCodec::ReadPCM(uint8_t* buffer, int size, int& actualsize)
{
  if (m_eof && !m_silenceTestBuffer.data_available())
    return -1;

  if (m_tagSongMs &&
      (m_posDelta + mul_div(m_dataWritten, 1000, m_player.sampleRate)) >= m_tagSongMs + m_tagFadeMs)
    return 1;

  if (size > m_sampleBuffer.size())
    m_sampleBuffer.resize(size * 2);

  unsigned int written = 0;

  int usedSize = size / 2 / sizeof(int16_t);

  int samples = (m_songLength + m_fadeLength) - m_dataWritten;
  if (samples > usedSize)
    samples = usedSize;

  if (m_cfgSuppressEndSilence)
  {
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
          m_player.GenerateSamples(m_sampleBuffer, 0, samples_to_render);
        }
        m_silenceTestBuffer.write(reinterpret_cast<int16_t*>(m_sampleBuffer.data()),
                                  samples_to_render * 2);
        free_space -= samples_to_render;
        if (m_remainder)
        {
          memmove(m_sampleBuffer.data(),
                  reinterpret_cast<int16_t*>(m_sampleBuffer.data()) + samples_to_render * 2,
                  m_remainder * 4);
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
    m_silenceTestBuffer.read(reinterpret_cast<int16_t*>(m_sampleBuffer.data()), written * 2);
  }
  else
  {
    if (m_remainder)
    {
      written = m_remainder;
      m_remainder = 0;
    }
    else
    {
      written = samples;
      m_player.GenerateSamples(m_sampleBuffer, 0, written);
    }
  }

  m_pos += double(written) / double(m_player.sampleRate);

  int d_start, d_end;
  d_start = m_dataWritten;
  m_dataWritten += written;
  d_end = m_dataWritten;

  if (m_tagSongMs && d_end > m_songLength)
  {
    int16_t* foo = reinterpret_cast<int16_t*>(m_sampleBuffer.data());
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

  actualsize = written * 2 * sizeof(int16_t);
  memcpy(buffer, m_sampleBuffer.data(), actualsize);

  return 0;
}

int64_t CNCSFCodec::Seek(int64_t time)
{
  double p_seconds = double(time) / 1000.0;
  m_eof = false;

  double buffered_time =
      (double)(m_silenceTestBuffer.data_available() / 2) / m_cfgDefaultSampleRate;

  m_pos += buffered_time;

  m_silenceTestBuffer.reset();

  if (p_seconds < m_pos)
  {
    Load();
  }
  unsigned int howmany = (int)(time_to_samples(p_seconds - m_pos, m_cfgDefaultSampleRate));

  // more abortable, and emu doesn't like doing huge numbers of samples per call anyway
  while (howmany)
  {
    unsigned int samples = 1024;
    m_player.GenerateSamples(m_sampleBuffer, 0, samples);
    if (samples > howmany)
    {
      memmove(m_sampleBuffer.data(),
              reinterpret_cast<int16_t*>(m_sampleBuffer.data()) + howmany * 2,
              (samples - howmany) * 4);
      m_remainder = samples - howmany;
      samples = howmany;
    }
    howmany -= samples;
  }

  m_dataWritten = 0;
  m_posDelta = (int)(p_seconds * 1000.);
  m_pos = p_seconds;

  calcfade();

  return time;
}

bool CNCSFCodec::ReadTag(const std::string& filename, kodi::addon::AudioDecoderInfoTag& tag)
{
  NCSFContext result;
  int ret = psf_load(filename.c_str(), &psf_file_system, 0x25, nullptr, nullptr, NCFSInfoMeta,
                     &result, 0, NCFSPrintMessage, this);
  if (ret <= 0)
  {
    kodi::Log(ADDON_LOG_ERROR, "%s: Not an NCSF file (%s)", __func__, filename.c_str());
    return false;
  }

  if (result.title.empty())
  {
    std::string fileName = kodi::vfs::GetFileName(filename);
    size_t lastindex = fileName.find_last_of(".");
    tag.SetTitle(fileName.substr(0, lastindex));
  }
  else
  {
    tag.SetTitle(result.title);
  }

  if (!result.artist.empty())
    tag.SetArtist(result.artist);
  else
    tag.SetArtist(result.game);
  tag.SetAlbum(result.game);
  tag.SetReleaseDate(result.year);
  tag.SetComment(result.comment);
  tag.SetDisc(result.disc);
  tag.SetTrack(result.track);
  tag.SetSamplerate(m_cfgDefaultSampleRate);
  tag.SetChannels(2);
  tag.SetDuration(result.tagSongMs / 1000);
  return true;
}

void CNCSFCodec::NCFSPrintMessage(void* context, const char* message)
{
  kodi::Log(ADDON_LOG_DEBUG, "NCFS codec message: '%s'", message);
}

int CNCSFCodec::NCSFLoader(void* context,
                           const uint8_t* exe,
                           size_t exe_size,
                           const uint8_t* reserved,
                           size_t reserved_size)
{
  NCSFLoaderState* state = static_cast<NCSFLoaderState*>(context);

  if (reserved_size >= 4)
  {
    state->sseq = get_le32(reserved);
  }

  if (exe_size >= 12)
  {
    uint32_t sdat_size = get_le32(exe + 8);
    if (sdat_size > exe_size)
      return -1;

    if (state->sdatData.empty())
      state->sdatData.resize(sdat_size, 0);
    else if (state->sdatData.size() < sdat_size)
      state->sdatData.resize(sdat_size);
    memcpy(&state->sdatData[0], exe, sdat_size);
  }

  return 0;
}

int CNCSFCodec::NCFSInfoMeta(void* context, const char* name, const char* value)
{
  NCSFContext* ncsf = static_cast<NCSFContext*>(context);

  if (!strcasecmp(name, "artist"))
    ncsf->artist = value;
  else if (!strcasecmp(name, "title"))
    ncsf->title = value;
  else if (!strcasecmp(name, "game"))
    ncsf->game = value;
  else if (!strcasecmp(name, "copyright"))
    ncsf->copyright = value;
  else if (!strcasecmp(name, "comment"))
    ncsf->comment = value;
  else if (!strcasecmp(name, "year"))
    ncsf->year = value;
  else if (!strcasecmp(name, "disc"))
    ncsf->disc = atoi(value);
  else if (!strcasecmp(name, "track"))
    ncsf->track = atoi(value);
  else if (!strcasecmp(name, "length"))
  {
    int temp = parse_time_crap(value);
    if (temp != BORK_TIME)
      ncsf->tagSongMs = temp;
  }
  else if (!strcasecmp(name, "fade"))
  {
    int temp = parse_time_crap(value);
    if (temp != BORK_TIME)
      ncsf->tagFadeMs = temp;
  }

  return 0;
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
    addonInstance = new CNCSFCodec(instance, version);
    return ADDON_STATUS_OK;
  }
  ~CMyAddon() override = default;
};

ADDONCREATOR(CMyAddon)
