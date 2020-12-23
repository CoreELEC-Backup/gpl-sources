/*
*      Copyright (C) 2015 Sam Stenvall
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
*  along with XBMC; see the file COPYING.  If not, write to
*  the Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
*  MA 02110-1301  USA
*  http://www.gnu.org/copyleft/gpl.html
*
*/

#include "Buffer.h"

#include <sstream>

using namespace timeshift;

const int Buffer::DEFAULT_READ_TIMEOUT = 10;

bool Buffer::Open(const std::string inputUrl)
{
  // Append the read timeout parameter
  std::stringstream ss;
  ss << inputUrl << "|connection-timeout=" << m_readTimeout;

  // Remember the start time and open the input
  m_startTime = time(nullptr);

  return m_inputHandle.OpenFile(ss.str(), ADDON_READ_NO_CACHE);
}

Buffer::~Buffer()
{
  Buffer::Close();
}

void Buffer::Close()
{
  CloseHandle(m_inputHandle);
}

void Buffer::CloseHandle(kodi::vfs::CFile& handle)
{
  if (handle.IsOpen())
  {
    handle.Close();
  }
}
