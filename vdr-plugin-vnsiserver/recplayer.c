/*
 *      vdr-plugin-vnsi - KODI server plugin for VDR
 *
 *      Copyright (C) 2004-2005 Chris Tallon
 *      Copyright (C) 2010 Alwin Esch (Team XBMC)
 *      Copyright (C) 2010, 2011 Alexander Pipelka
 *      Copyright (C) 2015 Team KODI
 *
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
 *  along with KODI; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

/*
 * This code is taken from VOMP for VDR plugin.
 */

#include "recplayer.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef O_NOATIME
#define O_NOATIME 0
#endif

cRecPlayer::cRecPlayer(const cRecording* rec, bool inProgress)
  :m_inProgress(inProgress),
   m_recordingFilename(rec->FileName()),
   m_pesrecording(rec->IsPesRecording()),
   m_indexFile(m_recordingFilename.c_str(), false, m_pesrecording),
   m_file(-1), m_fileOpen(-1)
{
  // FIXME find out max file path / name lengths

  if(m_pesrecording)
    INFOLOG("recording '%s' is a PES recording", m_recordingFilename.c_str());

  m_fps = rec->FramesPerSecond();

  scan();
}

void cRecPlayer::cleanup() {
  m_segments.clear();
}

void cRecPlayer::scan()
{
  struct stat s;

  closeFile();

  m_totalLength = 0;
  m_fileOpen    = -1;
  m_totalFrames = 0;

  cleanup();

  for(int i = 0; ; i++) // i think we only need one possible loop
  {
    fileNameFromIndex(i);

    if(stat(m_fileName, &s) == -1) {
      break;
    }

    cSegment segment;
    segment.start = m_totalLength;
    segment.end = segment.start + s.st_size;

    m_segments.push_back(segment);

    m_totalLength += s.st_size;
    INFOLOG("File %i found, size: %lu, totalLength now %lu", i, s.st_size, m_totalLength);
  }

  m_totalFrames = m_indexFile.Last();
  INFOLOG("total frames: %u", m_totalFrames);
}

void cRecPlayer::reScan()
{
  struct stat s;

  m_totalLength = 0;

  for(size_t i = 0; ; i++) // i think we only need one possible loop
  {
    fileNameFromIndex(i);

    if(stat(m_fileName, &s) == -1) {
      break;
    }

    cSegment* segment;
    if (m_segments.size() < i+1)
    {
      m_segments.push_back(cSegment());
      segment = &m_segments.back();
      segment->start = m_totalLength;
    }
    else
      segment = &m_segments[i];

    segment->end = segment->start + s.st_size;

    m_totalLength += s.st_size;
  }

  m_totalFrames = m_indexFile.Last();
}


cRecPlayer::~cRecPlayer()
{
  cleanup();
  closeFile();
}

char* cRecPlayer::fileNameFromIndex(int index) {
  if (m_pesrecording)
    snprintf(m_fileName, sizeof(m_fileName), "%s/%03i.vdr", m_recordingFilename.c_str(), index+1);
  else
    snprintf(m_fileName, sizeof(m_fileName), "%s/%05i.ts", m_recordingFilename.c_str(), index+1);

  return m_fileName;
}

bool cRecPlayer::openFile(int index)
{
  if (index == m_fileOpen) return true;
  closeFile();

  fileNameFromIndex(index);
  INFOLOG("openFile called for index %i string:%s", index, m_fileName);

  m_file = open(m_fileName, O_RDONLY);
  if (m_file == -1)
  {
    INFOLOG("file failed to open");
    m_fileOpen = -1;
    return false;
  }
  m_fileOpen = index;
  return true;
}

void cRecPlayer::closeFile()
{
  if(m_file == -1) {
    return;
  }

  INFOLOG("file closed");
  close(m_file);

  m_file = -1;
  m_fileOpen = -1;
}

uint64_t cRecPlayer::getLengthBytes()
{
  return m_totalLength;
}

uint32_t cRecPlayer::getLengthFrames()
{
  return m_totalFrames;
}

double cRecPlayer::getFPS()
{
  return m_fps;
}

int cRecPlayer::getBlock(unsigned char* buffer, uint64_t position, int amount)
{
  // dont let the block be larger than 256 kb
  if (amount > 512*1024)
    amount = 512*1024;

  if ((uint64_t)amount > m_totalLength)
    amount = m_totalLength;

  if (position >= m_totalLength)
  {
    reScan();
    if (position >= m_totalLength)
    {
      return 0;
    }
  }

  if ((position + amount) > m_totalLength)
    amount = m_totalLength - position;

  // work out what block "position" is in
  std::vector<cSegment>::iterator begin = m_segments.begin(),
    end = m_segments.end(), segmentIterator = end;
  for (std::vector<cSegment>::iterator i = begin; i != end; ++i) {
    if ((position >= i->start) && (position < i->end)) {
      segmentIterator = i;
      break;
    }
  }

  // segment not found / invalid position
  if (segmentIterator == end)
    return 0;

  // open file (if not already open)
  if (!openFile(std::distance(begin, segmentIterator)))
    return 0;

  // work out position in current file
  uint64_t filePosition = position - segmentIterator->start;

  // seek to position
  if(lseek(m_file, filePosition, SEEK_SET) == -1)
  {
    ERRORLOG("unable to seek to position: %lu", filePosition);
    return 0;
  }

  // try to read the block
  int bytes_read = read(m_file, buffer, amount);

  // we may got stuck at end of segment
  if ((bytes_read == 0) && (position < m_totalLength))
    bytes_read += getBlock(buffer, position+1 , amount);

  if(bytes_read <= 0)
  {
    return 0;
  }

  if (!m_inProgress)
  {
#ifndef __FreeBSD__
    // Tell linux not to bother keeping the data in the FS cache
    posix_fadvise(m_file, filePosition, bytes_read, POSIX_FADV_DONTNEED);
#endif
  }

  return bytes_read;
}

uint64_t cRecPlayer::positionFromFrameNumber(uint32_t frameNumber)
{
  uint16_t retFileNumber;
  off_t retFileOffset;
  bool retPicType;
  int retLength;

  if (!m_indexFile.Get((int)frameNumber, &retFileNumber, &retFileOffset, &retPicType, &retLength))
    return 0;

  if (retFileNumber >= m_segments.size()) 
    return 0;

  uint64_t position = m_segments[retFileNumber].start + retFileOffset;
  return position;
}

uint32_t cRecPlayer::frameNumberFromPosition(uint64_t position)
{
  if (position >= m_totalLength)
  {
    DEBUGLOG("Client asked for data starting past end of recording!");
    return m_totalFrames;
  }

  std::vector<cSegment>::iterator begin = m_segments.begin(),
    end = m_segments.end(), segmentIterator = end;
  for (std::vector<cSegment>::iterator i = begin; i != end; ++i) {
    if ((position >= i->start) && (position < i->end)) {
      segmentIterator = i;
      break;
    }
  }

  if (segmentIterator == end)
    return m_totalFrames;

  uint32_t askposition = position - segmentIterator->start;
  int segmentNumber = std::distance(begin, segmentIterator);
  return m_indexFile.Get((int)segmentNumber, askposition);
}


bool cRecPlayer::getNextIFrame(uint32_t frameNumber, uint32_t direction, uint64_t* rfilePosition, uint32_t* rframeNumber, uint32_t* rframeLength)
{
  // 0 = backwards
  // 1 = forwards

  uint16_t waste1;
  off_t waste2;

  int iframeLength;
  int indexReturnFrameNumber;

  indexReturnFrameNumber = (uint32_t)m_indexFile.GetNextIFrame(frameNumber, (direction==1 ? true : false), &waste1, &waste2, &iframeLength);
  DEBUGLOG("GNIF input framenumber:%u, direction=%u, output:framenumber=%i, framelength=%i", frameNumber, direction, indexReturnFrameNumber, iframeLength);

  if (indexReturnFrameNumber == -1) return false;

  *rfilePosition = positionFromFrameNumber(indexReturnFrameNumber);
  *rframeNumber = (uint32_t)indexReturnFrameNumber;
  *rframeLength = (uint32_t)iframeLength;

  return true;
}
