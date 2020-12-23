/*
 *  Copyright (C) 2014-2020 Arne Morten Kvarving
 *  Copyright (C) 2016-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <fluidsynth.h>
#include <kodi/addon-instance/AudioDecoder.h>

#define MIDI_HEADER 0x4D546864
#define MIDI_MTrk 0x4D54726B
#define MIDI_TEXT_EVENT 0xFF01
#define MIDI_COPYRIGHT 0xFF02
#define MIDI_TRACK_NAME 0xFF03
#define MIDI_INSTRUMENT_NAME 0xFF04
#define MIDI_LENGTH_TEXT_LYRIC 0xFF05
#define MIDI_LENGTH_TEXT_MARKER 0xFF06
#define MIDI_LENGTH_TEXT_CUE_POINT 0xFF07
#define MIDI_CHANNEL_PREFIX 0xFF20
#define MIDI_TEMPO_MICRO_SEC 0xFF51
#define MIDI_TIMESIGNATURE 0xFF58
#define MIDI_END_OF_TRACK 0xFF2F

struct FluidContext
{
  fluid_settings_t* settings = nullptr;
  fluid_synth_t* synth = nullptr;
  fluid_player_t* player = nullptr;
};

class ATTRIBUTE_HIDDEN CFluidCodec : public kodi::addon::CInstanceAudioDecoder
{
public:
  CFluidCodec(KODI_HANDLE instance, const std::string& version);
  ~CFluidCodec() override;

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
  int64_t Seek(int64_t time) override { return -1; }
  bool ReadTag(const std::string& filename, kodi::addon::AudioDecoderInfoTag& tag) override;

private:
  FluidContext ctx;
  std::string m_soundfont;
};
