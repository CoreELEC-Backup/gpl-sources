/*
 *  Copyright (C) 2005-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include <kodi/addon-instance/AudioEncoder.h>
#include <stdio.h>
#include <string.h>

#define WAVE_FORMAT_PCM 0x0001

// structure for WAV
typedef struct
{
  uint8_t riff[4]; /* must be "RIFF"    */
  uint32_t len; /* #bytes + 44 - 8   */
  uint8_t cWavFmt[8]; /* must be "WAVEfmt " */
  uint32_t dwHdrLen;
  uint16_t wFormat;
  uint16_t wNumChannels;
  uint32_t dwSampleRate;
  uint32_t dwBytesPerSec;
  uint16_t wBlockAlign;
  uint16_t wBitsPerSample;
  uint8_t cData[4]; /* must be "data"   */
  uint32_t dwDataLen; /* #bytes           */
} WAVHDR;

class ATTRIBUTE_HIDDEN CEncoderWav : public kodi::addon::CInstanceAudioEncoder
{
public:
  CEncoderWav(KODI_HANDLE instance, const std::string& version);

  bool Start(int inChannels,
             int inRate,
             int inBits,
             const std::string& title,
             const std::string& artist,
             const std::string& albumartist,
             const std::string& album,
             const std::string& year,
             const std::string& track,
             const std::string& genre,
             const std::string& comment,
             int trackLength) override;
  int Encode(int numBytesRead, const uint8_t* stream) override;
  bool Finish() override;

private:
  WAVHDR m_wav;
  uint32_t m_audiosize;
};

CEncoderWav::CEncoderWav(KODI_HANDLE instance, const std::string& version)
  : CInstanceAudioEncoder(instance, version)
{
  memset(&m_wav, 0, sizeof(m_wav));
}

bool CEncoderWav::Start(int inChannels,
                        int inRate,
                        int inBits,
                        const std::string& title,
                        const std::string& artist,
                        const std::string& albumartist,
                        const std::string& album,
                        const std::string& year,
                        const std::string& track,
                        const std::string& genre,
                        const std::string& comment,
                        int trackLength)
{
  // we accept only 2ch / 16 bit atm
  if (inChannels != 2 || inBits != 16)
    return false;

  // setup and write out our wav header
  memcpy(m_wav.riff, "RIFF", 4);
  memcpy(m_wav.cWavFmt, "WAVEfmt ", 8);
  m_wav.dwHdrLen = 16;
  m_wav.wFormat = WAVE_FORMAT_PCM;
  m_wav.wBlockAlign = 4;
  memcpy(m_wav.cData, "data", 4);
  m_wav.wNumChannels = inChannels;
  m_wav.dwSampleRate = inRate;
  m_wav.wBitsPerSample = inBits;
  m_wav.dwBytesPerSec = inRate * inChannels * (inBits >> 3);

  Write((uint8_t*)&m_wav, sizeof(m_wav));
  return true;
}

int CEncoderWav::Encode(int numBytesRead, const uint8_t* stream)
{
  // write the audio directly out to the file is all we need do here
  Write(stream, numBytesRead);
  m_audiosize += numBytesRead;
  return numBytesRead;
}

bool CEncoderWav::Finish()
{
  // seek back and fill in the wav header size
  m_wav.len = m_audiosize + sizeof(m_wav) - 8;
  m_wav.dwDataLen = m_audiosize;

  if (Seek(0, 0) == 0)
  {
    Write((uint8_t*)&m_wav, sizeof(m_wav));
    return true;
  }
  return false;
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
                              KODI_HANDLE& addonInstance) override;
};

ADDON_STATUS CMyAddon::CreateInstance(int instanceType,
                                      const std::string& instanceID,
                                      KODI_HANDLE instance,
                                      const std::string& version,
                                      KODI_HANDLE& addonInstance)
{
  addonInstance = new CEncoderWav(instance, version);
  return ADDON_STATUS_OK;
}

ADDONCREATOR(CMyAddon)
