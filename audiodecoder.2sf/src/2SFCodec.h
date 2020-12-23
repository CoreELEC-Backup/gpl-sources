/*
 *  Copyright (C) 2014-2020 Arne Morten Kvarving
 *  Copyright (C) 2016-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "CircularBuffer.h"
#include "psflib.h"
#include "state.h"

#include <kodi/addon-instance/AudioDecoder.h>
#include <math.h>

struct twosf_loader_state
{
  uint8_t* rom;
  uint8_t* state;
  size_t rom_size;
  size_t state_size;

  int initial_frames;
  int sync_type;
  int clockdown;
  int arm9_clockdown_level;
  int arm7_clockdown_level;

  twosf_loader_state()
    : rom(nullptr),
      state(nullptr),
      rom_size(0),
      state_size(0),
      initial_frames(-1),
      sync_type(0),
      clockdown(0),
      arm9_clockdown_level(0),
      arm7_clockdown_level(0)
  {
  }

  ~twosf_loader_state()
  {
    free(rom);
    free(state);
  }
};

struct TSFContext
{
  std::string title;
  std::string artist;
  std::string game;
  std::string copyright;
  std::string year;
  std::string comment;
  std::string replaygain;

  bool utf8 = false;

  int tagSongMs = 0;
  int tagFadeMs = 0;
};

class ATTRIBUTE_HIDDEN C2SFCodec : public kodi::addon::CInstanceAudioDecoder
{
public:
  C2SFCodec(KODI_HANDLE instance, const std::string& version);

  virtual ~C2SFCodec();

  bool Init(const std::string& filename,
            unsigned int filecache,
            int& channels,
            int& samplerate,
            int& bitspersample,
            int64_t& totaltime,
            int& bitrate,
            AudioEngineDataFormat& format,
            std::vector<AudioEngineChannel>& channellist) override;
  int ReadPCM(uint8_t* buffer, int size, int& actualsize) override;
  int64_t Seek(int64_t time) override;
  bool ReadTag(const std::string& file, kodi::addon::AudioDecoderInfoTag& tag) override;

private:
  bool Load();
  void Shutdown();

  inline uint64_t time_to_samples(double p_time, uint32_t p_sample_rate)
  {
    return (uint64_t)floor((double)p_sample_rate * p_time + 0.5);
  }

  inline void calcfade()
  {
    m_songLength = mul_div(m_tagSongMs - m_posDelta, m_cfgDefaultSampleRate, 1000);
    m_fadeLength = mul_div(m_tagFadeMs, m_cfgDefaultSampleRate, 1000);
  }

  inline int mul_div(int number, int numerator, int denominator)
  {
    long long ret = number;
    ret *= numerator;
    ret /= denominator;
    return (int)ret;
  }

  circular_buffer<int16_t> m_silenceTestBuffer = 0;

  twosf_loader_state m_state;
  NDS_state* m_emu = nullptr;

  std::string m_path;

  int m_cfgDefaultSampleRate = 44100;
  int m_cfgDefaultInfinite = 0;
  int m_cfgDefaultLength = 170000;
  int m_cfgDefaultFade = 10000;
  bool m_cfgSuppressOpeningSilence = true;
  bool m_cfgSuppressEndSilence = true;
  int m_cfgEndSilenceSeconds = 5;
  int m_cfgResamplingQuality = 4;

  bool m_noLoop = true;
  bool m_eof;

  std::vector<int16_t> m_sampleBuffer;

  int m_dataWritten;
  int m_remainder;
  int m_posDelta;
  int m_startSilence;
  int m_silence;

  double m_twosfEmuPosition;

  int m_songLength;
  int m_fadeLength;
  int m_tagSongMs;
  int m_tagFadeMs;
};
