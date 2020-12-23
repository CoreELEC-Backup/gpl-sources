/*
 *  Copyright (C) 2014-2020 Arne Morten Kvarving
 *  Copyright (C) 2016-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "FluidDecoder.h"

#include <kodi/Filesystem.h>
#include <kodi/General.h>

CFluidCodec::CFluidCodec(KODI_HANDLE instance, const std::string& version)
  : CInstanceAudioDecoder(instance, version)
{
  m_soundfont = kodi::GetSettingString("soundfont");
}

CFluidCodec::~CFluidCodec()
{
  if (ctx.player)
    delete_fluid_player(ctx.player);
  if (ctx.synth)
    delete_fluid_synth(ctx.synth);
  if (ctx.settings)
    delete_fluid_settings(ctx.settings);
}

bool CFluidCodec::Init(const std::string& filename,
                       unsigned int filecache,
                       int& channels,
                       int& samplerate,
                       int& bitspersample,
                       int64_t& totaltime,
                       int& bitrate,
                       AudioEngineDataFormat& format,
                       std::vector<AudioEngineChannel>& channellist)
{
  if (m_soundfont.empty() || m_soundfont == "OFF")
  {
    kodi::QueueNotification(QUEUE_ERROR, kodi::GetLocalizedString(30010),
                            kodi::GetLocalizedString(30011));
    return false;
  }
  kodi::vfs::CFile file;
  if (!file.OpenFile(filename, 0))
    return false;

  ctx.settings = new_fluid_settings();
  ctx.synth = new_fluid_synth(ctx.settings);
  fluid_synth_sfload(ctx.synth, m_soundfont.c_str(), 1);
  ctx.player = new_fluid_player(ctx.synth);

  size_t size = file.GetLength();
  char* temp = new char[size];
  file.Read(temp, size);
  file.Close();
  fluid_player_add_mem(ctx.player, temp, size);
  delete[] temp;
  fluid_player_play(ctx.player);
  format = AUDIOENGINE_FMT_FLOAT;
  channellist = {AUDIOENGINE_CH_FL, AUDIOENGINE_CH_FR};
  channels = 2;

  bitspersample = 32;
  bitrate = 0.0;

  samplerate = 44100;
  totaltime = 0;

  return true;
}

int CFluidCodec::ReadPCM(uint8_t* buffer, int size, int& actualsize)
{
  if (fluid_player_get_status(ctx.player) == FLUID_PLAYER_DONE)
    return 1;

  fluid_synth_write_float(ctx.synth, size / 8, buffer, 0, 2, buffer, 1, 2);
  actualsize = size;
  return 0;
}

bool CFluidCodec::ReadTag(const std::string& filename, kodi::addon::AudioDecoderInfoTag& tag)
{
  if (!kodi::GetSettingBoolean("scantext"))
    return false;

  kodi::vfs::CFile file;
  if (!file.OpenFile(filename))
    return false;

  int len = file.GetLength();
  uint8_t* data = new uint8_t[len];
  if (!data)
    return false;

  file.Read(data, len);

  uint32_t header = data[3] | data[2] << 8 | data[1] << 16 | data[0] << 24;
  uint32_t headerLength = data[7] | data[6] << 8 | data[5] << 16 | data[4] << 24;
  if (header != MIDI_HEADER || headerLength != 6)
    return false;

  std::vector<int> trackDataFormats;
  unsigned int ptr = 14;

  unsigned int trackNameCnt = 0;
  std::string firstTextEvent;
  std::string title;
  while (ptr < len)
  {
    uint32_t trackHeader =
        data[ptr + 3] | data[ptr + 2] << 8 | data[ptr + 1] << 16 | data[ptr] << 24;
    int32_t trackHeaderLength =
        data[ptr + 7] | data[ptr + 6] << 8 | data[ptr + 5] << 16 | data[ptr + 4] << 24;

    if (trackHeader != MIDI_MTrk)
      break;

    unsigned int blockPtr = 0;
    while (blockPtr < trackHeaderLength)
    {
      uint32_t blockIdentifier = data[blockPtr + ptr + 10] | data[blockPtr + ptr + 9] << 8 |
                                 data[blockPtr + ptr + 8] << 16;
      uint8_t blockLength = data[blockPtr + ptr + 11];
      if (blockLength == 0 || blockIdentifier == MIDI_CHANNEL_PREFIX)
        break;

      if (blockIdentifier == MIDI_TEXT_EVENT)
      {
        char* name = new char[blockLength + 1];
        memset(name, 0, blockLength + 1);
        strncpy(name, reinterpret_cast<const char*>(data + blockPtr + ptr + 12), blockLength);
        if (strncmp(name, "untitled", blockLength) != 0)
        {
          if (title.empty())
            title += name;

          if (firstTextEvent.empty())
            firstTextEvent = name;
        }
        delete[] name;
      }
      else if (blockIdentifier == MIDI_TRACK_NAME)
      {
        char* name = new char[blockLength + 1];
        memset(name, 0, blockLength + 1);
        strncpy(name, reinterpret_cast<const char*>(data + blockPtr + ptr + 12), blockLength);
        if (strncmp(name, "untitled", blockLength) != 0)
        {
          if (!title.empty())
            title += " - ";
          title += name;

          trackNameCnt++;
        }
        delete[] name;
      }

      blockPtr += blockLength + 4;
    }

    ptr += trackHeaderLength + 8;
  }

  // Prevent the case the track i used for instruments
  if (trackNameCnt > 3)
    title = firstTextEvent;

  tag.SetTitle(title);
  tag.SetDuration(-1);
  delete[] data;
  return true;
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
                              KODI_HANDLE& addonInstance) override
  {
    addonInstance = new CFluidCodec(instance, version);
    return ADDON_STATUS_OK;
  }
  ~CMyAddon() = default;
};

ADDONCREATOR(CMyAddon)
