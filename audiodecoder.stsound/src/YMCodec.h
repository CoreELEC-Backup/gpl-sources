/*
 *  Copyright (C) 2008-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "StSoundLibrary.h"
#include "YmMusic.h"

#include <kodi/addon-instance/AudioDecoder.h>

class ATTRIBUTE_HIDDEN CYMCodec : public kodi::addon::CInstanceAudioDecoder
{
public:
  CYMCodec(KODI_HANDLE instance, const std::string& version);
  virtual ~CYMCodec();

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
  YMMUSIC* m_tune = nullptr;
};
