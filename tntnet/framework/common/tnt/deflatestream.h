/*
 * Copyright (C) 2005 Tommi Maekitalo
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


#ifndef TNT_DEFLATESTREAM_H
#define TNT_DEFLATESTREAM_H

#include <iostream>
#include <stdexcept>
#include <zlib.h>
#include <vector>

namespace tnt
{
  class DeflateError : public std::runtime_error
  {
      int zRet;

    public:
      DeflateError(int zRet_, const std::string& msg)
        : std::runtime_error(msg),
          zRet(zRet_)
          { }

      int getRet() const  { return zRet; }
  };

  class DeflateStreamBuf : public std::streambuf
  {
      z_stream stream;
      std::vector<char_type> obuffer;
      std::streambuf* sink;

    public:
      explicit DeflateStreamBuf(std::streambuf* sink_, int level = Z_DEFAULT_COMPRESSION,
        unsigned bufsize = 8192);
      ~DeflateStreamBuf();

      /// see std::streambuf
      int_type overflow(int_type c);
      /// see std::streambuf
      int_type underflow();
      /// see std::streambuf
      int sync();

      /// end deflate-stream
      int end();
      void setSink(std::streambuf* sink_)   { sink = sink_; }
      uLong getAdler() const                { return stream.adler; }
  };

  class DeflateStream : public std::ostream
  {
      DeflateStreamBuf streambuf;

    public:
      explicit DeflateStream(std::streambuf* sink, int level = Z_DEFAULT_COMPRESSION)
        : std::ostream(0),
          streambuf(sink, level)
        { init(&streambuf); }
      explicit DeflateStream(std::ostream& sink, int level = Z_DEFAULT_COMPRESSION)
        : std::ostream(0),
          streambuf(sink.rdbuf(), level)
        { init(&streambuf); }

      void end();
      void setSink(std::streambuf* sink)   { streambuf.setSink(sink); }
      void setSink(std::ostream& sink)     { streambuf.setSink(sink.rdbuf()); }
      uLong getAdler() const   { return streambuf.getAdler(); }
  };
}

#endif // TNT_DEFLATESTREAM_H

