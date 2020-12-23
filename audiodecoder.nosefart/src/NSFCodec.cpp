/*
 *  Copyright (C) 2005-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "NSFCodec.h"

#include <algorithm>
#include <kodi/Filesystem.h>

unsigned int CNSFCodec::m_usedLib = 0;

CNSFCodec::CNSFCodec(KODI_HANDLE instance, const std::string& version)
  : CInstanceAudioDecoder(instance, version)
{
}

CNSFCodec::~CNSFCodec()
{
  if (m_dllInited)
  {
    log_shutdown();
    if (m_module)
      nsf_free(&m_module);
  }
  if (m_buffer)
    delete[] m_buffer;
}

bool CNSFCodec::Init(const std::string& filename,
                     unsigned int filecache,
                     int& channels,
                     int& samplerate,
                     int& bitspersample,
                     int64_t& totaltime,
                     int& bitrate,
                     AudioEngineDataFormat& format,
                     std::vector<AudioEngineChannel>& channellist)
{
  int track = 0;
  std::string toLoad(filename);
  if (toLoad.find(".nsfstream") != std::string::npos)
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

  m_module = LoadNSF(toLoad);
  if (!m_module)
  {
    return false;
  }

  m_len = m_pos = 0;
  m_track = track;
  m_limitFrames = GetTime(1, (char*)toLoad.c_str(), track);

  nsf_playtrack(m_module, track, 48000, 16, false);
  for (int i = 0; i < 6; i++)
    nsf_setchan(m_module, i, true);

  int bufferSize = 2 * 48000 / m_module->playback_rate * 2;
  m_head = m_buffer = new uint8_t[bufferSize];
  if (!m_buffer)
  {
    nsf_free(&m_module);
    return false;
  }

  memset(m_buffer, 0, bufferSize);

  channels = 1;
  samplerate = 48000;
  bitspersample = 16;
  totaltime =
      abs((int)((float)(m_limitFrames + m_module->playback_rate) / (float)m_module->playback_rate) -
          1) *
      1000;
  format = AUDIOENGINE_FMT_S16NE;
  bitrate = 0;
  channellist = {AUDIOENGINE_CH_FC};
  return true;
}

int CNSFCodec::ReadPCM(uint8_t* buffer, int size, int& actualsize)
{
  if (!buffer)
    return 1;

  actualsize = 0;
  while (size && m_module)
  {
    if (!m_len)
    {
      m_frames++;
      nsf_frame(m_module);
      m_module->process(m_buffer, 48000 / m_module->playback_rate);
      m_len = 2 * 48000 / m_module->playback_rate;
      m_head = m_buffer;
    }

    size_t tocopy = std::min(m_len, size_t(size));
    memcpy(buffer, m_head, tocopy);
    m_head += tocopy;
    m_len -= tocopy;
    m_pos += tocopy;
    actualsize += tocopy;
    buffer += tocopy;
    size -= tocopy;

    if (m_limitFrames != 0 && m_frames >= m_limitFrames)
    {
      return -1;
    }
  }

  return size != 0;
}

int64_t CNSFCodec::Seek(int64_t time)
{
  if (m_pos > time / 1000 * 48000 * 2)
  {
    m_pos = 0;
    m_len = 0;
    m_frames = 0;
  }
  while (m_module && m_pos + 2 * 48000 / m_module->playback_rate < time / 1000 * 48000 * 2)
  {
    m_frames++;
    nsf_frame(m_module);
    m_pos += 2 * 48000 / m_module->playback_rate;
  }

  if (!m_module || !m_buffer)
    return -1;

  m_module->process(m_buffer, 2 * 48000 / m_module->playback_rate);
  if (!m_buffer)
    return -1;

  m_len = 2 * 48000 / m_module->playback_rate - (time / 1000 * 48000 * 2 - m_pos);
  m_head = m_buffer + 2 * 48000 / m_module->playback_rate - m_len;
  m_pos += m_head - m_buffer;

  return time;
}

bool CNSFCodec::ReadTag(const std::string& filename, kodi::addon::AudioDecoderInfoTag& tag)
{
  nsf_t* module = LoadNSF(filename, true);
  if (module)
  {
    tag.SetTitle((const char*)module->song_name);
    if (tag.GetTitle() == "<?>")
      tag.SetTitle("");
    tag.SetArtist((const char*)module->artist_name);
    if (tag.GetArtist() == "<?>")
      tag.SetArtist("");
    tag.SetDuration(0);
    nsf_free(&module);

    return true;
  }

  return false;
}

int CNSFCodec::TrackCount(const std::string& fileName)
{
  nsf_t* module = LoadNSF(fileName, true);
  int result = 0;
  if (module)
  {
    result = module->num_songs;
    nsf_free(&module);
  }

  return result;
}

nsf_t* CNSFCodec::LoadNSF(const std::string& toLoad, bool forTag /* = false*/)
{
  if (!m_dllInited)
  {
    std::string source;
    if (forTag)
    {
      source = kodi::GetAddonPath(LIBRARY_PREFIX "nosefart_tag" LIBRARY_SUFFIX);
    }
    else
    {
      m_usedLib = !m_usedLib;
      source = kodi::GetAddonPath(LIBRARY_PREFIX + std::string("nosefart_") +
                                  std::to_string(m_usedLib) + LIBRARY_SUFFIX);
    }

    if (!LoadDll(source))
      return nullptr;
    if (!REGISTER_DLL_SYMBOL(nsf_init))
      return nullptr;
    if (!REGISTER_DLL_SYMBOL(nsf_load_extended))
      return nullptr;
    if (!REGISTER_DLL_SYMBOL(nsf_load))
      return nullptr;
    if (!REGISTER_DLL_SYMBOL(nsf_free))
      return nullptr;
    if (!REGISTER_DLL_SYMBOL(nsf_playtrack))
      return nullptr;
    if (!REGISTER_DLL_SYMBOL(nsf_frame))
      return nullptr;
    if (!REGISTER_DLL_SYMBOL(nsf_setchan))
      return nullptr;
    if (!REGISTER_DLL_SYMBOL(nsf_setfilter))
      return nullptr;
    if (!REGISTER_DLL_SYMBOL(nsf_nes6502_mem_access))
      return nullptr;
    if (!REGISTER_DLL_SYMBOL(log_init))
      return nullptr;
    if (!REGISTER_DLL_SYMBOL(log_shutdown))
      return nullptr;
    if (!REGISTER_DLL_SYMBOL(log_print))
      return nullptr;
    if (!REGISTER_DLL_SYMBOL(log_printf))
      return nullptr;

    m_dllInited = true;

    nsf_init();
    log_init();
  }

  kodi::vfs::CFile file;
  if (!file.OpenFile(toLoad, 0))
    return nullptr;

  int len = file.GetLength();
  char* data = new char[len];
  if (!data)
  {
    file.Close();
    return nullptr;
  }
  file.Read(data, len);
  file.Close();

  // Now load the module
  nsf_t* result = nsf_load(nullptr, data, len);
  delete[] data;

  return result;
}

// Code taken and changed from nsfinfo.c
unsigned int CNSFCodec::Calctime(int track, unsigned int frame_frag, bool force)
{
  unsigned int result1 = 0, result2 = 0;
  float sec = 0.0f;
  unsigned int playback_rate = PlaybackRate();

  // trouble with zelda2:7?
  int default_frag_size = 20 * playback_rate; // 2 * 60 * playback_rate;

  if (track < 0 || track > m_module->num_songs)
  {
    kodi::Log(ADDON_LOG_ERROR, "%s: calc time, track #%d out of range", __func__, track);
  }
  /* always called with frame_frag == 0 --matt s. */
  frame_frag = frame_frag ? frame_frag : default_frag_size;
  unsigned int max_frag = 60 * 60 * playback_rate;

  if (!m_module)
  {
    kodi::Log(ADDON_LOG_ERROR, "%s: load failed", __func__);
    return result1 * 0x1000 + 1000;
  }
  if (!force && m_module->song_frames && m_module->song_frames[track])
  {
    result1 = m_module->song_frames[track];
    return result1 * 0x1000 + 1000; /* Not en error :) */
  }

  /* ben : Don't care about sound format here,
   * since it will not be rendered.
   */
  int err = nsf_playtrack(m_module, track, 48000, 16, 1);
  if (err != track)
  {
    if (err == -1)
    {
      /* $$$ ben : Becoz nsf_playtrack() kicks nsf ass :( */
      m_module = nullptr;
    }
    kodi::Log(ADDON_LOG_ERROR, "%s: track %d not initialized", __func__, track);
    return result1 * 0x1000 + 1000;
  }

  /* the way this works is that it finds the last place that new memory
     is accessed.  The time it takes to do this is the length of the song.
     Well, sorta.  It's the time after which no new material is played.
     If the song has an intro which is not repeated, it counts that as part
     of the length.  (And if a song goes A B C B B B B ..., it will be the
     length of A B C.  I don't think I've encountered this, however,
     although I think it could happen.)
     -matt s.
  */
  {
    int done = 0;
    uint32_t last_accessed_frame = 0, prev_frag = 0, starting_frame = 0;

    while (!done && m_module)
    {
      nsf_frame(m_module); /* advance one frame. -matt s. */

      if (nsf_nes6502_mem_access())
        last_accessed_frame = m_module->cur_frame;

      if (m_module->cur_frame > frame_frag)
      {
        if (last_accessed_frame > prev_frag)
        {
          prev_frag = m_module->cur_frame;
          frame_frag += default_frag_size;
          if (frame_frag >= max_frag)
          {
            kodi::Log(ADDON_LOG_ERROR,
                      "%s: unable to find end of music within %u frames, giving up!", __func__,
                      max_frag);
            return result1 * 0x1000 + 1000;
          }
        }
        else
          done = 1;
      }
    }
    result1 = last_accessed_frame + 16 /* fudge room */;
    sec = (float)(result1 + m_module->playback_rate - 1) / (float)m_module->playback_rate;
  }

  /* This finds the length of the song _without_ the intro. -matt s */
  {
    /* don't want to count what we've already looked at */
    int starting_frame = m_module->cur_frame;

    int done = 0;
    uint32_t last_accessed_frame = 0, prev_frag = 0;

    while (!done && m_module)
    {
      nsf_frame(m_module); /* advance one frame. -matt s. */

      if (nsf_nes6502_mem_access())
        last_accessed_frame = m_module->cur_frame;

      if (m_module->cur_frame > frame_frag)
      {
        if (last_accessed_frame > prev_frag)
        {
          prev_frag = m_module->cur_frame;
          frame_frag += default_frag_size;

          if (frame_frag >= max_frag)
          {
            kodi::Log(ADDON_LOG_ERROR,
                      "%s: unable to find end of music within %u frames\n\tgiving up!", __func__,
                      max_frag);
            return result1 * 0x1000 + 1000;
          }
        }
        else
          done = 1;
      }
    }
    result2 = last_accessed_frame - starting_frame + 16 /* fudge room */;
    sec = (float)(result2 + m_module->playback_rate - 1) / (float)m_module->playback_rate;
  }

  /* Want to get both results back to nosefart.  Neither should be
     larger than 2^16, so let's shove them together into one int.
     the high order bits are result1 (with intro) and the lower order
     bits are result2 (without intro) */
  return (result1 * 0x1000 + result2);
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
    addonInstance = new CNSFCodec(instance, version);
    return ADDON_STATUS_OK;
  }
  virtual ~CMyAddon() = default;
};

ADDONCREATOR(CMyAddon)
