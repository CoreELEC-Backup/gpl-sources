/*
 *  Copyright (C) 2014-2020 Arne Morten Kvarving
 *  Copyright (C) 2016-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "CircularBuffer.h"
#include "QSoundRom.h"

#include <kodi/addon-instance/AudioDecoder.h>
#include <math.h>

struct QSFContext
{
  int length = 0;
  int fade = 0;
  std::string year;
  std::string title;
  std::string artist;
  std::string album;
  std::string comment;
};

class ATTRIBUTE_HIDDEN CQSFCodec : public kodi::addon::CInstanceAudioDecoder
{
public:
  CQSFCodec(KODI_HANDLE instance, const std::string& version)
    : CInstanceAudioDecoder(instance, version)
  {
  }

  virtual ~CQSFCodec() = default;

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
  bool ReadTag(const std::string& filename, kodi::addon::AudioDecoderInfoTag& tag) override;

private:
  bool Load();

  inline uint64_t time_to_samples(double p_time, uint32_t p_sample_rate)
  {
    return (uint64_t)floor((double)p_sample_rate * p_time + 0.5);
  }

  inline void calcfade()
  {
    m_songLength = mul_div(m_tagSong_ms - m_posDelta, 24038, 1000);
    m_fadeLength = mul_div(m_tagFade_ms, 24038, 1000);
  }

  inline int mul_div(int number, int numerator, int denominator)
  {
    long long ret = number;
    ret *= numerator;
    ret /= denominator;
    return (int)ret;
  }

  int m_cfgEndSilenceSeconds = 5;
  bool m_cfgSuppressOpeningSilence = true;
  bool m_cfgSuppressEndSilence = true;
  int m_cfgDefaultLength = 170000;
  int m_cfgDefaultFade = 10000;

  bool m_noLoop = true;
  bool m_eof = false;

  std::string m_usedFilename;
  std::vector<uint8_t> m_qsoundState;
  std::vector<int16_t> m_sampleBuffer;
  circular_buffer<int16_t> m_silenceTestBuffer = 0;
  qsound_rom m_rom;
  int m_err = 0;
  int64_t m_qsfEmuPos = 0;
  int m_dataWritten = 0;
  int m_remainder = 0;
  int m_posDelta = 0;
  int m_startsilence = 0;
  int m_silence = 0;

  int m_songLength;
  int m_fadeLength;
  int m_tagSong_ms;
  int m_tagFade_ms;

  QSFContext m_ctx;
};
