/*
 *  Copyright (C) 2005-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <kodi/addon-instance/AudioDecoder.h>

extern "C"
{
#include "asap.h"
}

struct ASAPContext
{
  char author[ASAPInfo_MAX_TEXT_LENGTH + 1];
  char name[ASAPInfo_MAX_TEXT_LENGTH + 1];
  int year;
  int month;
  int day;
  int channels;
  int duration;
  ASAP* asap = nullptr;
};

class ATTRIBUTE_HIDDEN CASAPCodec : public kodi::addon::CInstanceAudioDecoder
{
public:
  CASAPCodec(KODI_HANDLE instance, const std::string& version);
  virtual ~CASAPCodec();

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
  int TrackCount(const std::string& fileName) override;

private:
  ASAPContext ctx;
};
