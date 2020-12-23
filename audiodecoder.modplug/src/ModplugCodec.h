/*
 *  Copyright (C) 2005-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

//! \file ModplugCodec.h
//! \author Arne Morten Kvarving
//! \brief Modplug audio decoder for Kodi

#pragma once

#include <kodi/addon-instance/AudioDecoder.h>

extern "C"
{
#include <libmodplug/modplug.h>
}

class ATTRIBUTE_HIDDEN CModplugCodec : public kodi::addon::CInstanceAudioDecoder
{
public:
  CModplugCodec(KODI_HANDLE instance, const std::string& version);
  ~CModplugCodec() override;

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

private:
  ModPlugFile* m_module = nullptr;
};
