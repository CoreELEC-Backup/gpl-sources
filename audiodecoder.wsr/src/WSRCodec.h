/*
 *  Copyright (C) 2014-2020 Arne Morten Kvarving
 *  Copyright (C) 2016-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <kodi/addon-instance/AudioDecoder.h>
#include <kodi/tools/DllHelper.h>

extern "C"
{
#include "in_wsr/wsr_player.h"

#include <memory.h>
#include <stdint.h>
#include <stdio.h>
}

struct ATTRIBUTE_HIDDEN WSRContext
{
  short sample_buffer[576 * 2];
  size_t pos;
  uint64_t timepos;
};


class ATTRIBUTE_HIDDEN CWSRCodec : public kodi::addon::CInstanceAudioDecoder,
                                   private kodi::tools::CDllHelper
{
public:
  CWSRCodec(KODI_HANDLE instance, const std::string& version);
  virtual ~CWSRCodec() = default;

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
  int TrackCount(const std::string& fileName) override;

private:
  int Load_WSR(const char* name);

  WSRContext ctx;

  // dll function pointers
  void (*Init_WSR)();
  void (*Reset_WSR)(int SongNo);
  void (*Update_WSR)(int cycles, int Length);
  int (*Get_FirstSong)();

  unsigned char** ROM;
  int* ROMSize;
  int* ROMBank;
  short** sample_buffer;

  static unsigned int m_usedLib;
};
