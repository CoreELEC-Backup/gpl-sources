/*
 *      Copyright (C) 2005-2020 Team Kodi
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Kodi; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include <kodi/addon-instance/AudioDecoder.h>
#include <kodi/tools/DllHelper.h>

extern "C"
{
#include "log.h"
#include "machine/nsf.h"
#include "types.h"
#include "version.h"
}

class ATTRIBUTE_HIDDEN CNSFCodec : public kodi::addon::CInstanceAudioDecoder,
                                   private kodi::tools::CDllHelper
{
public:
  CNSFCodec(KODI_HANDLE instance, const std::string& version);
  virtual ~CNSFCodec();

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
  int TrackCount(const std::string& fileName) override;

private:
  int GetTime(int repetitions, char* filename, int track)
  {
    /* raw result, with intro, without intro */
    int result, wintro, wointro;

    result = Calctime(track, 0, true);

    wintro = result / 0x1000;
    wointro = result % 0x1000;

    return wintro + (repetitions - 1) * wointro;
  }

  int PlaybackRate()
  {
    uint8_t* p;
    unsigned int def, v;

    if (m_module->pal_ntsc_bits & NSF_DEDICATED_PAL)
    {
      p = (uint8_t*)&m_module->pal_speed;
      def = 50;
    }
    else
    {
      p = (uint8_t*)&m_module->ntsc_speed;
      def = 60;
    }
    v = p[0] | (p[1] << 8);
    return v ? 1000000 / v : def;
  }

  nsf_t* LoadNSF(const std::string& toLoad, bool forTag = false);
  unsigned int Calctime(int track, unsigned int frame_frag, bool force);

  nsf_t* m_module = nullptr;
  uint8_t* m_buffer = nullptr;
  uint8_t* m_head = nullptr;
  size_t m_len;
  size_t m_pos;
  size_t m_track;

  bool m_dllInited = false;

  int m_frames = 0;
  int m_limitFrames;

  static unsigned int m_usedLib;

  int (*nsf_init)(void);
  nsf_t* (*nsf_load_extended)(struct nsf_loader_t* loader);
  nsf_t* (*nsf_load)(const char* filename, void* source, int length);
  void (*nsf_free)(nsf_t** nsf_info);
  int (*nsf_playtrack)(nsf_t* nsf, int track, int sample_rate, int sample_bits, boolean stereo);
  void (*nsf_frame)(nsf_t* nsf);
  int (*nsf_setchan)(nsf_t* nsf, int chan, boolean enabled);
  int (*nsf_setfilter)(nsf_t* nsf, int filter_type);
  uint8 (*nsf_nes6502_mem_access)();

  int (*log_init)(void);
  void (*log_shutdown)(void);
  void (*log_print)(const char* string);
  void (*log_printf)(const char* format, ...);
};
