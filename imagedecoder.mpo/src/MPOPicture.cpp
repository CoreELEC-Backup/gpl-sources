/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include <iostream>
#include <kodi/addon-instance/ImageDecoder.h>
extern "C"
{
#include "../lib/libmpo/include/libmpo/dmpo.h"
}

class ATTRIBUTE_HIDDEN MPOPicture : public kodi::addon::CInstanceImageDecoder
{
public:
  MPOPicture(KODI_HANDLE instance, const std::string& version)
    : CInstanceImageDecoder(instance, version)
  {
  }

  ~MPOPicture() override
  {
    if (m_allocated)
      mpo_destroy_decompress(&m_mpoinfo);
    m_allocated = false;
  }

  bool LoadImageFromMemory(unsigned char* buffer,
                           unsigned int bufSize,
                           unsigned int& width,
                           unsigned int& height) override
  {
    // make a copy of data as we need it at decode time.
    m_data.resize(bufSize);
    std::copy(buffer, buffer + bufSize, m_data.begin());
    mpo_create_decompress(&m_mpoinfo);
    mpo_mem_src(&m_mpoinfo, m_data.data(), m_data.size());
    if (!mpo_read_header(&m_mpoinfo))
    {
      mpo_destroy_decompress(&m_mpoinfo);
      return false;
    }
    m_allocated = true;
    m_images = mpo_get_number_images(&m_mpoinfo);
    m_width = width = m_mpoinfo.cinfo.cinfo.image_width * m_images;
    m_height = height = m_mpoinfo.cinfo.cinfo.image_height;

    return true;
  }

  bool Decode(unsigned char* pixels,
              unsigned int width,
              unsigned int height,
              unsigned int pitch,
              ImageFormat format) override
  {
    size_t image = 0;
    while (image < m_images)
    {
      mpo_start_decompress(&m_mpoinfo);
      JSAMPARRAY buffer;
      int row_stride = m_mpoinfo.cinfo.cinfo.output_width * m_mpoinfo.cinfo.cinfo.output_components;
      size_t lines = 0;
      while (lines < m_height)
      {
        buffer = (*m_mpoinfo.cinfo.cinfo.mem->alloc_sarray)((j_common_ptr)&m_mpoinfo.cinfo,
                                                            JPOOL_IMAGE, row_stride, m_height);
        size_t nl = mpo_read_scanlines(&m_mpoinfo, buffer, m_height - lines);
        for (size_t line = 0; line < nl; ++line)
        {
          unsigned char* dst = pixels + (line + lines) * pitch + image * m_width / 2 * 4;
          for (size_t i = 0; i < row_stride; i += 3)
          {
            *dst++ = buffer[line][i + 2];
            *dst++ = buffer[line][i + 1];
            *dst++ = buffer[line][i];
            if (format == ADDON_IMG_FMT_A8R8G8B8)
              *dst++ = 0xff;
          }
        }
        lines += nl;
      }
      mpo_finish_decompress(&m_mpoinfo);
      ++image;
    }

    return true;
  }

private:
  unsigned int m_width;
  unsigned int m_height;
  size_t m_images;
  bool m_allocated = false;
  mpo_decompress_struct m_mpoinfo;
  std::vector<unsigned char> m_data;
};

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
    addonInstance = new MPOPicture(instance, version);
    return ADDON_STATUS_OK;
  }
};

ADDONCREATOR(CMyAddon)
