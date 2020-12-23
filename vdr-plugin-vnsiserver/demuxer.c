/*
 *      vdr-plugin-vnsi - KODI server plugin for VDR
 *
 *      Copyright (C) 2005-2012 Team XBMC
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


#include "config.h"
#include "demuxer.h"
#include "parser.h"
#include "videobuffer.h"

#include <vdr/channels.h>
#include <libsi/si.h>

cStreamInfo::cStreamInfo()
{

}

cStreamInfo::cStreamInfo(const cStreamInfo& info)
{
  pID = info.pID;
  type = info.type;
  content = info.content;
  subtitlingType = info.subtitlingType;
  compositionPageId = info.compositionPageId;
  ancillaryPageId = info.ancillaryPageId;
  handleRDS = info.handleRDS;
  SetLanguage(info.language);
}

void cStreamInfo::SetLanguage(const char* lang)
{
  language[0] = lang[0];
  language[1] = lang[1];
  language[2] = lang[2];
  language[3] = 0;
}

cVNSIDemuxer::cVNSIDemuxer(bool bAllowRDS)
 : m_bAllowRDS(bAllowRDS)
{
}

cVNSIDemuxer::~cVNSIDemuxer()
{

}

void cVNSIDemuxer::Open(const cChannel &channel, cVideoBuffer *videoBuffer)
{
  cMutexLock lock(&m_Mutex);

  m_CurrentChannel = channel;
  m_VideoBuffer = videoBuffer;

  if (m_CurrentChannel.Vpid())
    m_WaitIFrame = true;
  else
    m_WaitIFrame = false;

  m_PtsWrap.m_Wrap = false;
  m_PtsWrap.m_NoOfWraps = 0;
  m_PtsWrap.m_ConfirmCount = 0;
  m_MuxPacketSerial = 0;
  m_Error = ERROR_DEMUX_NODATA;
  m_SetRefTime = true;
  m_seenFirstPacket = false;
}

void cVNSIDemuxer::Close()
{
  cMutexLock lock(&m_Mutex);

  for (auto *i : m_Streams)
  {
    DEBUGLOG("Deleting stream parser for pid=%i and type=%i", i->GetPID(), i->Type());
    delete i;
  }
  m_Streams.clear();
  m_StreamInfos.clear();
}

int cVNSIDemuxer::Read(sStreamPacket *packet, sStreamPacket *packet_side_data)
{
  uint8_t *buf;
  int len;
  cTSStream *stream;

  cMutexLock lock(&m_Mutex);

  if (!m_CurrentChannel.Vpid())
    m_WaitIFrame = false;

  // clear packet
  if (!packet)
    return -1;
  packet->data = NULL;
  packet->streamChange = false;
  packet->pmtChange = false;

  // read TS Packet from buffer
  len = m_VideoBuffer->Read(&buf, TS_SIZE, m_endTime, m_wrapTime);
  // eof
  if (len == -2)
    return -2;
  else if (len != TS_SIZE)
    return -1;

  m_Error &= ~ERROR_DEMUX_NODATA;

  int ts_pid = TsPid(buf);

  // parse PAT/PMT
  if (ts_pid == PATPID)
  {
    m_PatPmtParser.ParsePat(buf, TS_SIZE);
  }
#if APIVERSNUM >= 10733
  else if (m_PatPmtParser.IsPmtPid(ts_pid))
#else
  else if (ts_pid == m_PatPmtParser.PmtPid())
#endif
  {
    int patVersion, pmtVersion;
    m_PatPmtParser.ParsePmt(buf, TS_SIZE);
    if (m_PatPmtParser.GetVersions(patVersion, pmtVersion))
    {
      cChannel pmtChannel(m_CurrentChannel);
      SetChannelPids(&pmtChannel, &m_PatPmtParser);
      SetChannelStreamInfos(&pmtChannel);
      m_PatPmtParser.Reset();
      if (EnsureParsers())
      {
        packet->pmtChange = true;
        return 1;
      }
    }
  }
  else if (stream = FindStream(ts_pid))
  {
    int error = stream->ProcessTSPacket(buf, packet, packet_side_data, m_WaitIFrame);
    if (error == 0)
    {
      m_WaitIFrame = false;
      m_seenFirstPacket = true;

      packet->serial = m_MuxPacketSerial;
      if (m_SetRefTime)
      {
        m_refTime = m_VideoBuffer->GetRefTime();
        packet->reftime = m_refTime;
        m_SetRefTime = false;
      }
      return 1;
    }
    else if (error < 0)
    {
      m_Error |= abs(error);
      if (m_Error & (ERROR_PES_SCRAMBLE | ERROR_TS_SCRAMBLE))
      {
        if (m_seenFirstPacket)
        {
          ResetParsers();
          m_Error |= ERROR_CAM_ERROR;
          m_WaitIFrame = true;
        }
      }
    }
  }

  return 0;
}

bool cVNSIDemuxer::SeekTime(int64_t time)
{
  off_t pos, pos_min, pos_max, pos_limit, start_pos;
  int64_t ts, ts_min, ts_max, last_ts;
  int no_change;

  if (!m_VideoBuffer->HasBuffer())
    return false;

  cMutexLock lock(&m_Mutex);

//  INFOLOG("----- seek to time: %ld", time);

  // rescale to 90khz
  time = cTSStream::Rescale(time, 90000, DVD_TIME_BASE);

  m_VideoBuffer->GetPositions(&pos, &pos_min, &pos_max);

//  INFOLOG("----- seek to time: %ld", time);
//  INFOLOG("------pos: %ld, pos min: %ld, pos max: %ld", pos, pos_min, pos_max);

  if (!GetTimeAtPos(&pos_min, &ts_min))
  {
    ResetParsers();
    m_WaitIFrame = true;
    return false;
  }

//  INFOLOG("----time at min: %ld", ts_min);

  if (ts_min >= time)
  {
    m_VideoBuffer->SetPos(pos_min);
    ResetParsers();
    m_WaitIFrame = true;
    m_MuxPacketSerial++;
    return true;
  }

  int64_t timecur;
  GetTimeAtPos(&pos, &timecur);

  // get time at end of buffer
  unsigned int step= 1024;
  bool gotTime;
  do
  {
    pos_max -= step;
    gotTime = GetTimeAtPos(&pos_max, &ts_max);
    step += step;
  } while (!gotTime && pos_max >= step);

  if (!gotTime)
  {
    ResetParsers();
    m_WaitIFrame = true;
    return false;
  }

  if (ts_max <= time)
  {
    ResetParsers();
    m_WaitIFrame = true;
    m_MuxPacketSerial++;
    return true;
  }

//  INFOLOG(" - time in buffer: %ld", cTSStream::Rescale(ts_max-ts_min, DVD_TIME_BASE, 90000)/1000000);

  // bisect seek
  if(ts_min > ts_max)
  {
    ResetParsers();
    m_WaitIFrame = true;
    return false;
  }
  else if (ts_min == ts_max)
  {
    pos_limit = pos_min;
  }
  else
    pos_limit = pos_max;

  no_change = 0;
  ts = time;
  last_ts = 0;
  while (pos_min < pos_limit)
  {
    if (no_change==0)
    {
      // interpolate position
      pos = cTSStream::Rescale(time - ts_min, pos_max - pos_min, ts_max - ts_min)
          + pos_min - (pos_max - pos_limit);
    }
    else if (no_change==1)
    {
      // bisection, if interpolation failed to change min or max pos last time
      pos = (pos_min + pos_limit) >> 1;
    }
    else
    {
      // linear search if bisection failed
      pos = pos_min;
    }

    // clamp calculated pos into boundaries
    if( pos <= pos_min)
      pos = pos_min + 1;
    else if (pos > pos_limit)
      pos = pos_limit;
    start_pos = pos;

    // get time stamp at pos
    if (!GetTimeAtPos(&pos, &ts))
    {
      ResetParsers();
      m_WaitIFrame = true;
      return false;
    }
    pos = m_VideoBuffer->GetPosCur();

    // determine method for next calculation of pos
    if ((last_ts == ts) || (pos >= pos_max))
      no_change++;
    else
      no_change=0;

//    INFOLOG("--- pos: %ld, \t time: %ld, diff time: %ld", pos, ts, time-ts);

    // 0.4 sec is close enough
    if (abs(time - ts) <= 36000)
    {
      break;
    }
    // target is to the left
    else if (time <= ts)
    {
      pos_limit = start_pos - 1;
      pos_max = pos;
      ts_max = ts;
    }
    // target is to the right
    if (time >= ts)
    {
      pos_min = pos;
      ts_min = ts;
    }
    last_ts = ts;
  }

//  INFOLOG("----pos found: %ld", pos);
//  INFOLOG("----time at pos: %ld, diff time: %ld", ts, cTSStream::Rescale(timecur-ts, DVD_TIME_BASE, 90000));

  m_VideoBuffer->SetPos(pos);

  ResetParsers();
  m_WaitIFrame = true;
  m_MuxPacketSerial++;
  return true;
}

void cVNSIDemuxer::BufferStatus(bool &timeshift, uint32_t &start, uint32_t &end)
{
  cMutexLock lock(&m_Mutex);

  timeshift = m_VideoBuffer->HasBuffer();

  if (timeshift)
  {
    m_VideoBuffer->GetBufferTime(m_endTime, m_wrapTime);
    if (!m_wrapTime)
    {
      start = m_refTime;
    }
    else
    {
      start = m_endTime - (m_wrapTime - m_refTime);
    }
    end = m_endTime;
  }
  else
  {
    start = 0;
    end = 0;
  }
}

cTSStream *cVNSIDemuxer::GetFirstStream()
{
  m_StreamsIterator = m_Streams.begin();
  if (m_StreamsIterator != m_Streams.end())
    return *m_StreamsIterator;
  else
    return NULL;
}

cTSStream *cVNSIDemuxer::GetNextStream()
{
  ++m_StreamsIterator;
  if (m_StreamsIterator != m_Streams.end())
    return *m_StreamsIterator;
  else
    return NULL;
}

cTSStream *cVNSIDemuxer::FindStream(int Pid)
{
  for (auto *i : m_Streams)
  {
    if (Pid == i->GetPID())
      return i;
  }
  return NULL;
}

void cVNSIDemuxer::ResetParsers()
{
  for (auto *i : m_Streams)
  {
    i->ResetParser();
  }
  m_seenFirstPacket = false;
}

static bool Contains(const std::list<cStreamInfo> &list, int pID, eStreamType type)
{
  for (const auto &i : list)
    if (i.pID == pID && i.type == type)
      return true;

  return false;
}

bool cVNSIDemuxer::EnsureParsers()
{
  bool streamChange = false;

  auto it = m_Streams.begin();
  while (it != m_Streams.end())
  {
    if (!Contains(m_StreamInfos, (*it)->GetPID(), (*it)->Type()))
    {
      INFOLOG("Deleting stream for pid=%i and type=%i", (*it)->GetPID(), (*it)->Type());
      it = m_Streams.erase(it);
      streamChange = true;
    }
    else
      ++it;
  }

  for (const auto &i : m_StreamInfos)
  {
    cTSStream *stream = FindStream(i.pID);
    if (stream)
    {
      // TODO: check for change in lang
      stream->SetLanguage(i.language);
      continue;
    }

    if (i.type == stH264)
    {
      stream = new cTSStream(stH264, i.pID, &m_PtsWrap);
    }
    else if (i.type == stHEVC)
    {
      stream = new cTSStream(stHEVC, i.pID, &m_PtsWrap);
    }
    else if (i.type == stMPEG2VIDEO)
    {
      stream = new cTSStream(stMPEG2VIDEO, i.pID, &m_PtsWrap);
    }
    else if (i.type == stMPEG2AUDIO)
    {
      stream = new cTSStream(stMPEG2AUDIO, i.pID, &m_PtsWrap, i.handleRDS);
      stream->SetLanguage(i.language);
    }
    else if (i.type == stAACADTS)
    {
      stream = new cTSStream(stAACADTS, i.pID, &m_PtsWrap);
      stream->SetLanguage(i.language);
    }
    else if (i.type == stAACLATM)
    {
      stream = new cTSStream(stAACLATM, i.pID, &m_PtsWrap);
      stream->SetLanguage(i.language);
    }
    else if (i.type == stAC3)
    {
      stream = new cTSStream(stAC3, i.pID, &m_PtsWrap);
      stream->SetLanguage(i.language);
    }
    else if (i.type == stEAC3)
    {
      stream = new cTSStream(stEAC3, i.pID, &m_PtsWrap);
      stream->SetLanguage(i.language);
    }
    else if (i.type == stDVBSUB)
    {
      stream = new cTSStream(stDVBSUB, i.pID, &m_PtsWrap);
      stream->SetLanguage(i.language);
#if APIVERSNUM >= 10709
      stream->SetSubtitlingDescriptor(i.subtitlingType, i.compositionPageId, i.ancillaryPageId);
#endif
    }
    else if (i.type == stTELETEXT)
    {
      stream = new cTSStream(stTELETEXT, i.pID, &m_PtsWrap);
    }
    else
      continue;

    m_Streams.push_back(stream);
    INFOLOG("Created stream for pid=%i and type=%i", stream->GetPID(), stream->Type());
    streamChange = true;
  }
  m_StreamInfos.clear();

  return streamChange;
}

void cVNSIDemuxer::SetChannelStreamInfos(const cChannel *channel)
{
  m_StreamInfos.clear();

  cStreamInfo newStream;
  bool containsVideo = false;
  int index = 0;
  if (channel->Vpid())
  {
    newStream.pID = channel->Vpid();
#if APIVERSNUM >= 10701
    if (channel->Vtype() == 0x1B)
      newStream.type = stH264;
    else if (channel->Vtype() == 0x24)
      newStream.type = stHEVC;
    else
#endif
      newStream.type = stMPEG2VIDEO;

    m_StreamInfos.push_back(newStream);
    containsVideo = true;
  }

  const int *DPids = channel->Dpids();
  index = 0;
  for ( ; *DPids; DPids++)
  {
    newStream.pID = *DPids;
    newStream.type = stAC3;
    if (channel->Dtype(index) == SI::EnhancedAC3DescriptorTag)
      newStream.type = stEAC3;
    newStream.SetLanguage(channel->Dlang(index));
    m_StreamInfos.push_back(newStream);
    index++;
  }

  const int *APids = channel->Apids();
  index = 0;
  for ( ; *APids; APids++)
  {
    newStream.pID = *APids;
    newStream.type = stMPEG2AUDIO;
    if (channel->Atype(index) == 0x0F)
      newStream.type = stAACADTS;
    else if (channel->Atype(index) == 0x11)
      newStream.type = stAACLATM;
    newStream.handleRDS = m_bAllowRDS && newStream.type == stMPEG2AUDIO && !containsVideo ? true : false; // Relevant for RDS, if present only on mpeg 2 audio, use only if RDS is allowed
    newStream.SetLanguage(channel->Alang(index));
    m_StreamInfos.push_back(newStream);
    index++;
  }

  const int *SPids = channel->Spids();
  if (SPids)
  {
    index = 0;
    for ( ; *SPids; SPids++)
    {
      newStream.pID = *SPids;
      newStream.type = stDVBSUB;
      newStream.SetLanguage(channel->Slang(index));
      newStream.subtitlingType = channel->SubtitlingType(index);
      newStream.compositionPageId = channel->CompositionPageId(index);
      newStream.ancillaryPageId = channel->AncillaryPageId(index);
      m_StreamInfos.push_back(newStream);
    }
    index++;
  }

  if (channel->Tpid())
  {
    newStream.pID = channel->Tpid();
    newStream.type = stTELETEXT;
    m_StreamInfos.push_back(newStream);
  }
}

void cVNSIDemuxer::SetChannelPids(cChannel *channel, cPatPmtParser *patPmtParser)
{
  int Apids[MAXAPIDS + 1] = { 0 };
  int Atypes[MAXAPIDS + 1] = { 0 };
  int Dpids[MAXDPIDS + 1] = { 0 };
  int Dtypes[MAXDPIDS + 1] = { 0 };
  int Spids[MAXSPIDS + 1] = { 0 };
  char ALangs[MAXAPIDS][MAXLANGCODE2] = { 0 };
  char DLangs[MAXDPIDS][MAXLANGCODE2] = { 0 };
  char SLangs[MAXSPIDS][MAXLANGCODE2] = { 0 };
  int index = 0;

  const int *aPids = patPmtParser->Apids();
  index = 0;
  for ( ; *aPids; aPids++)
  {
    Apids[index] = patPmtParser->Apid(index);
    Atypes[index] = patPmtParser->Atype(index);
    strn0cpy(ALangs[index], patPmtParser->Alang(index), MAXLANGCODE2);
    index++;
  }

  const int *dPids = patPmtParser->Dpids();
  index = 0;
  for ( ; *dPids; dPids++)
  {
    Dpids[index] = patPmtParser->Dpid(index);
    Dtypes[index] = patPmtParser->Dtype(index);
    strn0cpy(DLangs[index], patPmtParser->Dlang(index), MAXLANGCODE2);
    index++;
  }

  const int *sPids = patPmtParser->Spids();
  index = 0;
  for ( ; *sPids; sPids++)
  {
    Spids[index] = patPmtParser->Spid(index);
    strn0cpy(SLangs[index], patPmtParser->Slang(index), MAXLANGCODE2);
    index++;
  }

  int Vpid = patPmtParser->Vpid();
  int Ppid = patPmtParser->Ppid();
  int VType = patPmtParser->Vtype();
  int Tpid = m_CurrentChannel.Tpid();
  channel->SetPids(Vpid, Ppid, VType,
                   Apids, Atypes, ALangs,
                   Dpids, Dtypes, DLangs,
                   Spids, SLangs,
                   Tpid);
}

bool cVNSIDemuxer::GetTimeAtPos(off_t *pos, int64_t *time)
{
  uint8_t *buf;
  int len;
  cTSStream *stream;
  int ts_pid;

  m_VideoBuffer->SetPos(*pos);
  ResetParsers();
  while ((len = m_VideoBuffer->Read(&buf, TS_SIZE, m_endTime, m_wrapTime)) == TS_SIZE)
  {
    ts_pid = TsPid(buf);
    if (stream = FindStream(ts_pid))
    {
      // only consider video or audio streams
      if ((stream->Content() == scVIDEO || stream->Content() == scAUDIO) &&
          stream->ReadTime(buf, time))
      {
        return true;
      }
    }
  }
  return false;
}

uint16_t cVNSIDemuxer::GetError()
{
  uint16_t ret = m_Error;
  m_Error = ERROR_DEMUX_NODATA;
  return ret;
}

