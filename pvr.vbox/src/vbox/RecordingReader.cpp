/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "RecordingReader.h"

#include <algorithm>

using namespace vbox;

RecordingReader::RecordingReader(const std::string& streamURL, std::time_t start, std::time_t end, int duration)
  : m_streamURL(streamURL), m_start(start), m_end(end), m_duration(duration)
{
  m_readHandle.CURLCreate(m_streamURL);
  m_readHandle.CURLOpen(ADDON_READ_NO_CACHE);
  m_len = m_readHandle.GetLength();
  m_nextReopen = std::time(nullptr) + REOPEN_INTERVAL;

  //If this is an ongoing recording set the duration to the eventual length of the recording
  if (start > 0 && end > 0)
  {
    m_duration = static_cast<int>(end - start);
  }

  kodi::Log(ADDON_LOG_DEBUG, "%s RecordingReader: Started - url=%s, start=%u, end=%u, duration=%d", __FUNCTION__,
              m_streamURL.c_str(), m_start, m_end, m_duration);
}

RecordingReader::~RecordingReader()
{
  kodi::Log(ADDON_LOG_DEBUG, "%s RecordingReader: Stopped", __FUNCTION__);
}

bool RecordingReader::Start()
{
  return m_readHandle.IsOpen();
}

ssize_t RecordingReader::ReadData(unsigned char* buffer, unsigned int size)
{
  /* check for playback of ongoing recording */
  if (m_end)
  {
    std::time_t now = std::time(nullptr);
    if (m_pos == m_len || now > m_nextReopen)
    {
      /* reopen stream */
      kodi::Log(ADDON_LOG_DEBUG, "%s RecordingReader: Reopening stream...", __FUNCTION__);
      m_readHandle.CURLOpen(ADDON_READ_REOPEN | ADDON_READ_NO_CACHE);
      m_len = m_readHandle.GetLength();
      m_readHandle.Seek(m_pos, SEEK_SET);

      // random value (10 MiB) we choose to switch to fast reopen interval
      bool nearEnd = m_len - m_pos <= 10 * 1024 * 1024;
      m_nextReopen = now + (nearEnd ? REOPEN_INTERVAL_FAST : REOPEN_INTERVAL);

      /* recording has finished */
      if (now > m_end)
        m_end = 0;
    }
  }

  ssize_t read = m_readHandle.Read(buffer, size);
  m_pos += read;
  return read;
}

int64_t RecordingReader::Seek(long long position, int whence)
{
  int64_t ret = m_readHandle.Seek(position, whence);
  // for unknown reason seek sometimes doesn't return the correct position
  // so let's sync with the underlaying implementation
  m_pos = m_readHandle.GetPosition();
  m_len = m_readHandle.GetLength();
  return ret;
}

int64_t RecordingReader::Position()
{
  return m_pos;
}

int64_t RecordingReader::Length()
{
  return m_len;
}

int RecordingReader::CurrentDuration()
{
  if (m_end != 0)
  {
    time_t now = std::time(nullptr);

    if (now < m_end)
    {
      kodi::Log(ADDON_LOG_DEBUG, "%s RecordingReader - Partial: %d", __FUNCTION__, static_cast<int>(now - m_start));
      return now - m_start;
    }
  }

  kodi::Log(ADDON_LOG_DEBUG, "%s RecordingReader - Full: %d", __FUNCTION__, m_duration);
  return m_duration;
}
