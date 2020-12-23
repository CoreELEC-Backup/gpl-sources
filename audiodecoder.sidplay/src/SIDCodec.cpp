/*
 *  Copyright (C) 2005-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "SIDCodec.h"

#include <kodi/Filesystem.h>

CSIDCodec::CSIDCodec(KODI_HANDLE instance, const std::string& version)
  : CInstanceAudioDecoder(instance, version)
{
}

CSIDCodec::~CSIDCodec()
{
  if (tune)
    delete tune;
}

bool CSIDCodec::Init(const std::string& filename,
                     unsigned int filecache,
                     int& channels,
                     int& samplerate,
                     int& bitspersample,
                     int64_t& totaltime,
                     int& bitrate,
                     AudioEngineDataFormat& format,
                     std::vector<AudioEngineChannel>& channellist)
{
  int track = 1;
  std::string toLoad(filename);
  if (toLoad.find(".sidstream") != std::string::npos)
  {
    size_t iStart = toLoad.rfind('-') + 1;
    track = atoi(toLoad.substr(iStart, toLoad.size() - iStart - 10).c_str());
    //  The directory we are in, is the file
    //  that contains the bitstream to play,
    //  so extract it
    size_t slash = toLoad.rfind('\\');
    if (slash == std::string::npos)
      slash = toLoad.rfind('/');
    toLoad = toLoad.substr(0, slash);
  }

  kodi::vfs::CFile file;
  if (!file.OpenFile(toLoad.c_str(), 0))
    return false;

  int len = file.GetLength();
  uint8_t* data = new uint8_t[len];
  file.Read(data, len);
  file.Close();

  // Now load the module
  tune = new SidTune(data, len);
  delete[] data;

  if (!tune)
    return false;

  tune->selectSong(track);
  player.load(tune);
  config.clockDefault = SID2_CLOCK_PAL;
  config.clockForced = false;
  config.clockSpeed = SID2_CLOCK_CORRECT;
  config.emulateStereo = false;
  config.environment = sid2_envR;
  config.forceDualSids = false;
  config.frequency = 48000;
  config.leftVolume = 255;
  config.optimisation = SID2_DEFAULT_OPTIMISATION;
  config.playback = sid2_mono;
  config.powerOnDelay = SID2_DEFAULT_POWER_ON_DELAY;
  config.precision = 16;
  config.rightVolume = 255;
  config.sampleFormat = SID2_LITTLE_SIGNED;
  ReSIDBuilder* rs = new ReSIDBuilder("Resid Builder");
  rs->create(player.info().maxsids);
  rs->filter(true);
  rs->sampling(48000);
  config.sidEmulation = rs;
  pos = 0;
  track = track;

  player.config(config);

  channels = 1;
  samplerate = 48000;
  bitspersample = 16;
  totaltime = 4 * 60 * 1000;
  format = AUDIOENGINE_FMT_S16NE;
  channellist = {AUDIOENGINE_CH_FC};
  bitrate = 0;

  return true;
}

int CSIDCodec::ReadPCM(uint8_t* buffer, int size, int& actualsize)
{
  if ((actualsize = player.play(buffer, size)))
  {
    pos += actualsize;
    return 0;
  }

  return 1;
}

int64_t CSIDCodec::Seek(int64_t time)
{
  uint8_t temp[3840 * 2];
  if (pos > time / 1000 * 48000 * 2)
  {
    tune->selectSong(track);
    player.load(tune);
    player.config(config);
    pos = 0;
  }

  while (pos < time / 1000 * 48000 * 2)
  {
    int64_t iRead = time / 1000 * 48000 * 2 - pos;
    if (iRead > 3840 * 2)
    {
      player.fastForward(32 * 100);
      iRead = 3840 * 2;
    }
    else
      player.fastForward(100);

    int dummy;
    ReadPCM(temp, int(iRead), dummy);
    if (!dummy)
      break; // get out of here
  }
  return time;
}

int CSIDCodec::TrackCount(const std::string& fileName)
{
  kodi::vfs::CFile file;
  if (!file.OpenFile(fileName, 0))
    return 1;

  int len = file.GetLength();
  uint8_t* data = new uint8_t[len];
  file.Read(data, len);
  file.Close();

  SidTune tune(data, len);
  delete[] data;

  return tune.getInfo().songs;
}

bool CSIDCodec::ReadTag(const std::string& filename, kodi::addon::AudioDecoderInfoTag& tag)
{
  tag.SetDuration(-1);
  SidTuneMod st(filename.c_str());
  if (!st)
    return false;

  const SidTuneInfo sti = st.getInfo();

  if (sti.numberOfInfoStrings != 0)
  {
    tag.SetTitle(sti.infoString[0]);
    if (tag.GetTitle() == "<?>")
    {
      // Fallback to filename if title is set as "<?>"
      std::string fileName = kodi::vfs::GetFileName(filename);
      size_t lastindex = fileName.find_last_of(".");
      tag.SetTitle(fileName.substr(0, lastindex));
    }
    // Add Artist if present and ignore the rest (TODO: give also another strings, not only title and artist?)
    if (sti.numberOfInfoStrings > 1)
    {
      tag.SetArtist(sti.infoString[1]);
      if (tag.GetArtist() == "<?>")
        tag.SetArtist("");
    }
  }

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
    addonInstance = new CSIDCodec(instance, version);
    return ADDON_STATUS_OK;
  }
  virtual ~CMyAddon() = default;
};

ADDONCREATOR(CMyAddon)
