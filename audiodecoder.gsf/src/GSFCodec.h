/*
 *  Copyright (C) 2014-2020 Arne Morten Kvarving
 *  Copyright (C) 2016-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "psflib.h"
#include "vbam/gba/GBA.h"
#include "vbam/gba/Sound.h"

#include <kodi/addon-instance/AudioDecoder.h>

struct gsf_loader_state
{
  int entry_set;
  uint32_t entry;
  uint8_t* data;
  size_t data_size;
  gsf_loader_state() : entry_set(0), data(0), data_size(0) {}
  ~gsf_loader_state() { free(data); }
};

struct gsf_sound_out : public GBASoundOut
{
  gsf_sound_out() : head(0), bytes_in_buffer(0) {}
  size_t head;
  size_t bytes_in_buffer;
  std::vector<uint8_t> sample_buffer;
  virtual ~gsf_sound_out() {}
  // Receives signed 16-bit stereo audio and a byte count
  virtual void write(const void* samples, unsigned long bytes)
  {
    sample_buffer.resize(bytes_in_buffer + bytes);
    memcpy(&sample_buffer[bytes_in_buffer], samples, bytes);
    bytes_in_buffer += bytes;
  }
};

struct GSFContext
{
  gsf_loader_state state;
  GBASystem system;
  gsf_sound_out output;
  int64_t len;
  int sample_rate;
  int64_t pos;
  bool inited = false;
  std::string title;
  std::string artist;
  std::string year;
  std::string game;
  std::string comment;
};

class ATTRIBUTE_HIDDEN CGSFCodec : public kodi::addon::CInstanceAudioDecoder
{
public:
  CGSFCodec(KODI_HANDLE instance, const std::string& version);
  virtual ~CGSFCodec();

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
  GSFContext ctx;
};
