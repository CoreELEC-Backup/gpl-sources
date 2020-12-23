/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "TimidityCodec.h"

#include "MidiScan.h"

#include <kodi/Filesystem.h>
#include <kodi/General.h>
#include <sstream>

#if !defined(_WIN32)
#include <unistd.h>
#endif

unsigned int CTimidityCodec::m_usedLib = 0;

CTimidityCodec::CTimidityCodec(KODI_HANDLE instance, const std::string& version)
  : CInstanceAudioDecoder(instance, version)
{
  m_soundfont = kodi::GetSettingString("soundfont");
}

CTimidityCodec::~CTimidityCodec()
{
  if (m_song)
    Timidity_FreeSong(m_song);
  if (!m_tmpFileName.empty())
    kodi::vfs::DeleteFile(m_tmpFileName);
}

bool CTimidityCodec::Init(const std::string& filename,
                          unsigned int filecache,
                          int& channels,
                          int& samplerate,
                          int& bitspersample,
                          int64_t& totaltime,
                          int& bitrate,
                          AudioEngineDataFormat& format,
                          std::vector<AudioEngineChannel>& channellist)
{
  if (m_soundfont.empty())
  {
    kodi::QueueNotification(QUEUE_ERROR, kodi::GetLocalizedString(30010),
                            kodi::GetLocalizedString(30011));
    return false;
  }

  m_usedLib = !m_usedLib;
  std::string source = kodi::GetAddonPath(LIBRARY_PREFIX + std::string("timidity_") +
                                          std::to_string(m_usedLib) + LIBRARY_SUFFIX);

  // clang-format off
  if (!LoadDll(source)) return false;
  if (!REGISTER_DLL_SYMBOL(Timidity_Init)) return false;
  if (!REGISTER_DLL_SYMBOL(Timidity_Cleanup)) return false;
  if (!REGISTER_DLL_SYMBOL(Timidity_GetLength)) return false;
  if (!REGISTER_DLL_SYMBOL(Timidity_LoadSong)) return false;
  if (!REGISTER_DLL_SYMBOL(Timidity_FreeSong)) return false;
  if (!REGISTER_DLL_SYMBOL(Timidity_FillBuffer)) return false;
  if (!REGISTER_DLL_SYMBOL(Timidity_Seek)) return false;
  if (!REGISTER_DLL_SYMBOL(Timidity_ErrorMsg)) return false;
  // clang-format on

  int res;
  if (m_soundfont.find(".sf2") != std::string::npos)
    res = Timidity_Init(48000, 16, 2, m_soundfont.c_str(), nullptr); // real soundfont
  else
    res = Timidity_Init(48000, 16, 2, nullptr, m_soundfont.c_str()); // config file

  if (res != 0)
    return false;

  std::stringstream ss;
  ss << "timiditiy-" << static_cast<void*>(this) << ".mid";
  m_tmpFileName = kodi::GetTempAddonPath(ss.str());
  if (!kodi::vfs::CopyFile(filename, m_tmpFileName))
    return false;

  m_song = Timidity_LoadSong((char*)m_tmpFileName.c_str());
  if (!m_song)
    return false;

  m_pos = 0;

  channels = 2;
  samplerate = 48000;
  bitspersample = 16;
  totaltime = Timidity_GetLength(m_song);
  format = AUDIOENGINE_FMT_S16NE;
  channellist = {AUDIOENGINE_CH_FL, AUDIOENGINE_CH_FR};
  bitrate = 0;

  /*
  // Currently not usable as there nothing in Kodi or addon to show related song lyrics!
  // Thats why currently commented out.
  std::string filenameLyrics = filename.substr(0, filename.size() - 4) + ".lrc";
  if (!kodi::vfs::FileExists(filenameLyrics))
  {
    CMidiScan scan(filename);
    if (scan.Scan() && !scan.GetLyrics().empty())
    {
      kodi::vfs::CFile lyricsFile;
      lyricsFile.OpenFileForWrite(filenameLyrics);

      size_t written = 0;
      size_t size = scan.GetLyrics().length();
      while (written < size)
      {
        written += lyricsFile.Write(scan.GetLyrics().c_str() + written, size - written);
      }
    }
  }
  */

  return true;
}

int CTimidityCodec::ReadPCM(uint8_t* buffer, int size, int& actualsize)
{
  if (!buffer)
    return -1;

  if (m_pos > Timidity_GetLength(m_song) / 1000 * 48000 * 4)
    return -1;

  actualsize = Timidity_FillBuffer(m_song, buffer, size);
  if (actualsize == 0)
    return -1;

  m_pos += actualsize;

  return 0;
}

int64_t CTimidityCodec::Seek(int64_t time)
{
  return Timidity_Seek(m_song, time);
}

bool CTimidityCodec::ReadTag(const std::string& filename, kodi::addon::AudioDecoderInfoTag& tag)
{
  if (!kodi::GetSettingBoolean("scantext"))
    return false;

  CMidiScan scan(filename);
  scan.Scan();

  tag.SetArtist(scan.GetArtist());
  tag.SetTitle(scan.GetTitle());
  tag.SetLyrics(scan.GetLyrics());
  tag.SetDuration(scan.GetDuration());

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
    addonInstance = new CTimidityCodec(instance, version);
    return ADDON_STATUS_OK;
  }
};

ADDONCREATOR(CMyAddon)
