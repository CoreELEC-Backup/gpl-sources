/*
 *  Copyright (C) 2014-2020 Arne Morten Kvarving
 *  Copyright (C) 2016-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <kodi/addon-instance/AudioDecoder.h>

extern "C"
{
#include "psflib.h"
#include "usf.h"

#include <stdint.h>
#include <stdio.h>

  struct ATTRIBUTE_HIDDEN USFContext
  {
    char* state = nullptr;
    int64_t len = 0;
    int sample_rate = 0;
    int fade = 0;
    int64_t pos = 0;
    std::string title;
    std::string artist;
    std::string game;
    std::string genre;
    std::string year;
    std::string usfby;
    std::string copyright;
    std::string comment;
  };

} /* extern "C" */

class ATTRIBUTE_HIDDEN CUSFCodec : public kodi::addon::CInstanceAudioDecoder
{
public:
  CUSFCodec(KODI_HANDLE instance, const std::string& version);
  virtual ~CUSFCodec();

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
  bool CheckEndReached(uint8_t* buffer, int size);
  USFContext ctx;
  bool m_firstRoundDone = false;
};
