/*
 *  Copyright (C) 2009-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <stdint.h>
#include <string>

struct MD5Context
{
  uint32_t buf[4];
  uint32_t bytes[2];
  uint32_t in[16];
};

class XBMC_MD5
{
public:
  XBMC_MD5(void);
  ~XBMC_MD5(void);
  void append(const void* inBuf, size_t inLen);
  void append(const std::string& str);
  void getDigest(unsigned char digest[16]);
  std::string getDigest();

  /*! \brief Get the MD5 digest of the given text
   \param text text to compute the MD5 for
   \return MD5 digest
   */
  static std::string GetMD5(const std::string& text);

private:
  MD5Context m_ctx;
};
