/*
 * Copyright (C) 2003 Tommi Maekitalo
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "cxxtools/md5stream.h"
#include "cxxtools/log.h"
#include "md5.h"
#include <cstring>

log_define("cxxtools.md5stream")

namespace cxxtools
{

////////////////////////////////////////////////////////////////////////
// Md5streambuf
//
Md5streambuf::Md5streambuf()
  : context(new cxxtools_MD5_CTX())
{
  log_debug("initialize MD5");
  cxxtools_MD5Init(context);
}

Md5streambuf::~Md5streambuf()
{
  delete context;
}

std::streambuf::int_type Md5streambuf::overflow(
  std::streambuf::int_type ch)
{
  if (pptr() == 0)
  {
    // Ausgabepuffer ist leer - initialisieren
    log_debug("initialize MD5");
    cxxtools_MD5Init(context);
  }
  else
  {
    // konsumiere Zeichen aus dem Puffer
    log_debug("process " << (pptr() - pbase()) << " bytes of data");
    cxxtools_MD5Update(context,
              (const unsigned char*)pbase(),
              pptr() - pbase());
  }

  // setze Ausgabepuffer
  setp(buffer, buffer + bufsize);

  if (ch != traits_type::eof())
  {
    // das Zeichen, welches den overflow ausgel�st hat, stecken
    // wir in den Puffer.
    *pptr() = traits_type::to_char_type(ch);
    pbump(1);
  }

  return 0;
}

std::streambuf::int_type Md5streambuf::underflow()
{
  // nur Ausgabestrom
  return traits_type::eof();
}

int Md5streambuf::sync()
{
  if (pptr() != pbase())
  {
    // konsumiere Zeichen aus dem Puffer
    log_debug("process " << (pptr() - pbase()) << " bytes of data");
    cxxtools_MD5Update(context, (const unsigned char*)pbase(), pptr() - pbase());

    // leere Ausgabepuffer
    setp(buffer, buffer + bufsize);
  }

  return 0;
}

void Md5streambuf::getDigest(unsigned char digest_[16])
{
  if (pptr())
  {
    if (pptr() != pbase())
    {
      // konsumiere Zeichen aus dem Puffer
      log_debug("process " << (pptr() - pbase()) << " bytes of data");
      cxxtools_MD5Update(context, (const unsigned char*)pbase(), pptr() - pbase());
    }

    // deinitialisiere Ausgabepuffer
    setp(0, 0);
  }
  else
  {
    log_debug("initialize MD5");
    cxxtools_MD5Init(context);
  }

  log_debug("finalize MD5");
  cxxtools_MD5Final(digest, context);

  std::memcpy(digest_, digest, 16);
}

////////////////////////////////////////////////////////////////////////
// Md5stream
//
const char* Md5stream::getHexDigest()
{
  static const char hexDigits[] = "0123456789abcdef";
  unsigned char md5[16];
  getDigest(md5);
  int i;
  char* p = hexdigest;
  for (i = 0; i < 16; ++i)
  {
    *p++ = hexDigits[md5[i] >> 4];
    *p++ = hexDigits[md5[i] & 0xf];
  }
  *p = '\0';
  log_debug("md5: " << hexdigest);
  return hexdigest;
}

}
