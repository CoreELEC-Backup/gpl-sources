#pragma once
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

#include <ctime>
#include <string>

/**
* The basic type all buffers operate on
*/
#ifdef _WIN32
#include <windows.h>
#else
typedef unsigned char byte;
#endif

#include <kodi/Filesystem.h>

namespace timeshift
{
  /**
   * Base class for all timeshift buffers
   */
  class ATTRIBUTE_HIDDEN Buffer
  {
  public:
    Buffer() = default;
    virtual ~Buffer();

    /**
     * Opens the input handle
     * @return whether the input was successfully opened
     */
    virtual bool Open(const std::string inputUrl);

    /**
     * Closes the buffer
     */
    virtual void Close();

    /**
     * Reads "length" bytes into the specified buffer
     * @return the number of bytes read
     */
    virtual int Read(byte* buffer, size_t length) = 0;

    /**
     * Seeks to the specified position
     * @return the new position
     */
    virtual int64_t Seek(int64_t position, int whence) = 0;

    /**
     * Whether the buffer supports pausing
     */
    virtual bool CanPauseStream() const = 0;

    /**
     * Whether the buffer supports seeking
     */
    virtual bool CanSeekStream() const = 0;

    /**
     * @return the current read position
     */
    virtual int64_t Position() const = 0;

    /**
     * @return the current length of the buffer
     */
    virtual int64_t Length() const = 0;

    /**
     * @return the time the buffering started
     */
    time_t GetStartTime() const { return m_startTime; }

    /**
     * @return basically the current time
     */
    time_t GetEndTime() const { return time(nullptr); }

    /**
     * Sets the read timeout (defaults to 10 seconds)
     * @param timeout the read timeout in seconds
     */
    void SetReadTimeout(int timeout) { m_readTimeout = timeout; }

  protected:
    const static int DEFAULT_READ_TIMEOUT;

    /**
     * Safely closes an open file handle.
     * @param the handle to close. The pointer will be nulled.
     */
    void CloseHandle(kodi::vfs::CFile& handle);

    /**
     * The input handle (where data is read from)
     */
    kodi::vfs::CFile m_inputHandle;

    /**
     * The time (in seconds) to wait when opening a read handle and when
     * waiting for the buffer to have enough data
     */
    int m_readTimeout = DEFAULT_READ_TIMEOUT;

  private:
    /**
     * The time the buffer was created
     */
    time_t m_startTime = 0;
  };
} // namespace timeshift
