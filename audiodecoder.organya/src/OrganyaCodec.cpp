/*
 *  Copyright (C) 2014-2020 Arne Morten Kvarving
 *  Copyright (C) 2019-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "OrganyaCodec.h"

COrganyaCodec::~COrganyaCodec()
{
  if (m_tune)
    org_decoder_destroy(m_tune);
}

bool COrganyaCodec::Init(const std::string& strFile,
                         unsigned int filecache,
                         int& channels,
                         int& samplerate,
                         int& bitspersample,
                         int64_t& totaltime,
                         int& bitrate,
                         AudioEngineDataFormat& format,
                         std::vector<AudioEngineChannel>& channelinfo)
{
  if (!m_file.OpenFile(strFile, 0))
  {
    kodi::Log(ADDON_LOG_ERROR, "Failed to open: '%s'", strFile.c_str());
    return false;
  }

  kodi::CheckSettingBoolean("loopindefinitely", m_cfgLoopIndefinitely);
  kodi::CheckSettingInt("loopcount", m_cfgLoopCnt);
  kodi::CheckSettingInt("fadetime", m_cfgFadeTime);

  std::string temp = kodi::GetAddonPath("resources/samples");
  m_tune = org_decoder_create(&m_file, temp.c_str(), !m_cfgLoopIndefinitely ? 1 : m_cfgLoopCnt);
  if (!m_tune)
  {
    kodi::Log(ADDON_LOG_ERROR, "Failed to create organya decoder");
    return false;
  }

  m_tune->state.sample_rate = m_cfgSampleRate;
  m_length = org_decoder_get_total_samples(m_tune);
  m_fadeTime = mul_div(m_cfgFadeTime, m_cfgSampleRate, 1000);
  m_samplesDecoded = 0;

  totaltime = m_length * 1000 / m_cfgSampleRate;
  format = AUDIOENGINE_FMT_S16NE;
  channelinfo = {AUDIOENGINE_CH_FL, AUDIOENGINE_CH_FR};
  channels = 2;
  bitspersample = 16;
  bitrate = 0.0;
  samplerate = m_cfgSampleRate;

  Seek(0);

  return true;
}

int COrganyaCodec::ReadPCM(uint8_t* pBuffer, int size, int& actualsize)
{
  if (!m_cfgLoopIndefinitely && m_samplesDecoded > m_length)
    return -1;

  int16_t* ptr = reinterpret_cast<int16_t*>(pBuffer);

  unsigned int samples_todo = size / 2 / sizeof(int16_t);
  int64_t samplesDecodedFrame = org_decode_samples(m_tune, ptr, samples_todo);
  if (samplesDecodedFrame)
  {
    if (!m_cfgLoopIndefinitely)
    {
      int64_t fade_start = m_samplesDecoded;
      int64_t fade_end = m_samplesDecoded += samplesDecodedFrame;

      if (fade_start > m_length - m_fadeTime)
      {
        for (int n = fade_start; n < fade_end; ++n)
        {
          int bleh = m_length - n;
          ptr[0] = mul_div(ptr[0], bleh, m_fadeTime);
          ptr[1] = mul_div(ptr[1], bleh, m_fadeTime);
          ptr += 2;
        }
      }
    }

    actualsize = samplesDecodedFrame * 2 * sizeof(int16_t);
  }
  else
  {
    actualsize = 0;
    return -1;
  }

  return 0;
}

int64_t COrganyaCodec::Seek(int64_t time)
{
  int64_t pos = time * m_cfgSampleRate / 1000;
  org_decoder_seek_sample(m_tune, pos);
  m_samplesDecoded = pos;

  return time;
}

bool COrganyaCodec::ReadTag(const std::string& filename, kodi::addon::AudioDecoderInfoTag& tag)
{
  if (!m_file.OpenFile(filename, 0))
    return false;

  std::string temp = kodi::GetAddonPath("resources/samples");
  m_tune = org_decoder_create(&m_file, temp.c_str(), m_cfgLoopCnt > 1 ? m_cfgLoopCnt : 1);
  if (!m_tune)
    return false;

  m_tune->state.sample_rate = m_cfgSampleRate;
  tag.SetSamplerate(m_cfgSampleRate);
  tag.SetDuration(org_decoder_get_total_samples(m_tune) / m_cfgSampleRate);
  return true;
}

//------------------------------------------------------------------------------

class ATTRIBUTE_HIDDEN CMyAddon : public kodi::addon::CAddonBase
{
public:
  CMyAddon() = default;
  ~CMyAddon() override = default;
  ADDON_STATUS CreateInstance(int instanceType,
                              const std::string& instanceID,
                              KODI_HANDLE instance,
                              const std::string& version,
                              KODI_HANDLE& addonInstance) override
  {
    addonInstance = new COrganyaCodec(instance, version);
    return ADDON_STATUS_OK;
  }
};

ADDONCREATOR(CMyAddon) // Don't touch this!
