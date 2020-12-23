/*
 *  Copyright (C) 2005-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <kodi/Filesystem.h>
#include <kodi/addon-instance/AudioDecoder.h>
#include <libopenmpt/libopenmpt.h>

struct ATTRIBUTE_HIDDEN MPTContext
{
  openmpt_module* module = nullptr;
  kodi::vfs::CFile file;
};

class ATTRIBUTE_HIDDEN CMPTCodec : public kodi::addon::CInstanceAudioDecoder
{
public:
  CMPTCodec(KODI_HANDLE instance, const std::string& version);
  virtual ~CMPTCodec();

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
  MPTContext ctx;
};
