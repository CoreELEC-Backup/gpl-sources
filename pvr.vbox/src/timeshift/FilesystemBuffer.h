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

#include "Buffer.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

#include <kodi/Filesystem.h>

namespace timeshift
{

  /**
   * Timeshift buffer which buffers into a file
   */
  class ATTRIBUTE_HIDDEN FilesystemBuffer : public Buffer
  {
  public:
    /**
     * @param bufferPath the directory to store the buffer files in
     */
    FilesystemBuffer(const std::string& bufferPath);
    virtual ~FilesystemBuffer();

    virtual bool Open(const std::string inputUrl) override;
    virtual void Close() override;
    virtual int Read(byte* buffer, size_t length) override;
    virtual int64_t Seek(int64_t position, int whence) override;

    virtual bool CanPauseStream() const override { return true; }

    virtual bool CanSeekStream() const override { return true; }

    virtual int64_t Position() const override { return m_readPosition.load(); }

    virtual int64_t Length() const override { return m_writePosition.load(); }

  private:
    const static int INPUT_READ_LENGTH;

    /**
     * The method that runs on m_inputThread. It reads data from the input
     * handle and writes it to the output handle
     */
    void ConsumeInput();


    /**
     * Closes any open file handles and resets all file positions
     */
    void Reset();

    /**
     * The path to the buffer file
     */
    std::string m_bufferPath;

    /**
     * Read-only handle to the buffer file
     */
    kodi::vfs::CFile m_outputReadHandle;

    /**
     * Write-only handle to the buffer file
     */
    kodi::vfs::CFile m_outputWriteHandle;

    /**
     * The thread that reads from m_inputHandle and writes to the output
     * handles
     */
    std::thread m_inputThread;

    /**
     * Whether the buffer is active, i.e. m_inputHandle should be read from
     */
    std::atomic<bool> m_active;

    /**
     * Protects m_output*Handle
     */
    mutable std::mutex m_mutex;

    /**
     * Signaled whenever new packets have been added to the buffer
     */
    mutable std::condition_variable m_condition;

    /**
     * The current read position in the buffer file
     */
    std::atomic<int64_t> m_readPosition;

    /**
     * The current write position in the buffer file
     */
    std::atomic<int64_t> m_writePosition;
  };
} // namespace timeshift
