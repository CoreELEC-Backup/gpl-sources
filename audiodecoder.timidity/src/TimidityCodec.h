/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <kodi/addon-instance/AudioDecoder.h>
#include <kodi/tools/DllHelper.h>

extern "C"
{
#include "timidity/timidity_codec.h"
}

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

class ATTRIBUTE_HIDDEN CTimidityCodec : public kodi::addon::CInstanceAudioDecoder,
                                        private kodi::tools::CDllHelper
{
public:
  CTimidityCodec(KODI_HANDLE instance, const std::string& version);
  ~CTimidityCodec();

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
  std::string m_tmpFileName;
  std::string m_soundfont;
  MidiSong* m_song = nullptr;
  int m_pos = 0;

  static unsigned int m_usedLib;

  int (*Timidity_Init)(
      int rate, int bits_per_sample, int channels, const char* soundfont_file, const char* cfgfile);
  void (*Timidity_Cleanup)();
  int (*Timidity_GetLength)(MidiSong* song);
  MidiSong* (*Timidity_LoadSong)(char* fn);
  void (*Timidity_FreeSong)(MidiSong* song);
  int (*Timidity_FillBuffer)(MidiSong* song, void* buf, unsigned int size);
  unsigned long (*Timidity_Seek)(MidiSong* song, unsigned long iTimePos);
  char* (*Timidity_ErrorMsg)();
};
