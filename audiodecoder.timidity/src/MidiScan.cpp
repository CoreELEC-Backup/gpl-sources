/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "MidiScan.h"

#include <algorithm>
#include <ctype.h>
#include <kodi/Filesystem.h>
#include <locale>

CMidiScan::CMidiScan(const std::string& path) : m_path(path)
{
}

CMidiScan::~CMidiScan()
{
  delete[] m_fileData;
  if (m_msgbuff)
    free(m_msgbuff);
}

bool CMidiScan::Scan()
{
  kodi::vfs::FileStatus status;
  if (!kodi::vfs::FileExists(m_path) || !kodi::vfs::StatFile(m_path, status))
  {
    kodi::Log(ADDON_LOG_ERROR, "Wanted midi file '%s' not found", m_path.c_str());
    return false;
  }

  m_fileDataPtr = 0;
  m_fileDataSize = status.GetSize();
  delete m_fileData;
  m_fileData = new uint8_t[m_fileDataSize + 10];

  kodi::vfs::CFile file;
  if (!file.OpenFile(m_path))
  {
    kodi::Log(ADDON_LOG_ERROR, "Wanted midi file '%s' failed to open", m_path.c_str());
    return false;
  }

  while (1)
  {
    size_t size = file.Read(m_fileData, status.GetSize());
    if (size < 0)
      return false;
    else if (size == 0)
      break;
  }

  Read();

  int lastId = -1;
  bool karaoke = false;
  KaraokeData karaokeData;

  for (const auto& a : m_metadata)
  {
    if (lastId != a.id)
    {
      karaoke = false;
      if (a.type == MetaTypeSequenceTrackName)
      {
        if (a.text == "Words")
        {
          karaoke = true;
          lastId = a.id;
        }
      }
      else if (a.type == MetaTypeLyric)
      {
        karaoke = true;
        lastId = a.id;
      }

      continue;
    }

    if (karaoke)
    {
      if (a.time == 0 && a.text.empty())
      {
        continue;
      }

      int ptr = 0;
      if (a.text.size() > 2 && a.text[ptr] == '@')
      {
        ptr++;
        if (a.text[ptr] == 'L')
          karaokeData.language = a.text.c_str() + 2;
        else if (a.text[ptr] == 'T')
        {
          std::string data = a.text.c_str() + 2;
          if (std::all_of(data.begin(), data.end(), [](unsigned char c) {
                return c == ' ' || c == '"' || c == '-' || c == '\'' || isupper(c);
              }))
            std::transform(data.begin() + 1, data.end(), data.begin() + 1,
                           [](unsigned char c) { return tolower(c); });

          if (karaokeData.title.empty())
            karaokeData.title = data;
          else if (karaokeData.artist.empty())
            karaokeData.artist = data;
          else if (karaokeData.creator.empty())
            karaokeData.creator = data;
        }
        continue;
      }
      else if (a.text.size() > 1 && a.text[ptr] == '\\')
      {
        ptr++;
      }
      else if (a.text.size() > 1 && a.text[ptr] == '/')
      {
        ptr++;
      }

      karaokeData.text += GetTimeString(a.time) + (a.text.c_str() + ptr) + "\n";
    }
  }

  if (!karaokeData.text.empty())
  {
    kodi::Log(ADDON_LOG_INFO, "File %s contains lyrics", m_path.c_str());

    m_lyrics = "# Path: " + m_path + "\n";
    m_lyrics += "[id:" + RandomString(10) + "]\n";
    if (!karaokeData.title.empty())
      m_lyrics += "[ti:" + karaokeData.title + "]\n";
    if (!karaokeData.title.empty())
      m_lyrics += "[ar:" + karaokeData.artist + "]\n";
    m_lyrics += karaokeData.text + "\n";
  }

  if (m_artist.empty() && !karaokeData.artist.empty())
    m_artist = karaokeData.artist;
  else if (m_artist.empty() && !karaokeData.album.empty())
    m_artist = karaokeData.album;
  if (m_title.empty() && !karaokeData.title.empty())
    m_title = karaokeData.title;

  return true;
}

void CMidiScan::Read()
{
  ReadHeader();
  int track = 1;
  while (ReadTrack())
  {
    track++;
    if (track > m_ntrks)
      break;
  }
}

void CMidiScan::ReadHeader() /* read a header chunk */
{
  int format;

  if (ReadMT("MThd") == -1)
    return;

  m_toberead = Read32bit();
  m_bytesread = 0;
  format = Read16bit();
  m_ntrks = Read16bit();
  m_division = Read16bit();

  //printf("Header format=%d ntrks=%d division=%d\n", format, m_ntrks, m_division);

  /* flush any extra stuff, in case the length of header is not 6 */
  while (m_toberead > 0)
    GetCharacter();
}

int CMidiScan::ReadTrack() /* read a track chunk */
{
  /* This array is indexed by the high half of a status byte.  It's */
  /* value is either the number of bytes needed (1 or 2) for a channel */
  /* message, or 0 (meaning it's not  a channel message). */
  static int chantype[] = {
      0, 0, 0, 0, 0, 0, 0, 0, /* 0x00 through 0x70 */
      2, 2, 2, 2, 1, 1, 2, 0 /* 0x80 through 0xf0 */
  };
  long lookfor;
  int c, c1, type;
  int sysexcontinue = 0; /* 1 if last message was an unfinished sysex */
  int running = 0; /* 1 when running status used */
  int status = 0; /* status value (e.g. 0x90==note-on) */
  int laststatus; /* for running status */
  int needed;
  int time_increment;
  long varinum;

  laststatus = 0;
  if (ReadMT("MTrk") == -1)
    return (0);

  m_toberead = Read32bit();
  m_currtime = 0;
  m_bytesread = 0;

  //printf("Track start\n");

  while (m_toberead > 0)
  {

    time_increment = ReadVarinum(); /* delta time */
    if (time_increment < 0)
    {
      kodi::Log(ADDON_LOG_ERROR, "bad time increment = %d\n", time_increment);
    }
    m_currtime += time_increment; /* [SS] 2018-06-13 */

    c = GetCharacter();

    if (sysexcontinue && c != 0xf7)
      kodi::Log(ADDON_LOG_ERROR, "didn't find expected continuation of a sysex");

    /* if bit 7 not set, there is no status byte following the
     *  delta time, so it must a running status and we assume the
     *  last status occuring in the preceding channel message.
     */
    if ((c & 0x80) == 0) /* running status? */
    {
      if (status == 0)
        kodi::Log(ADDON_LOG_ERROR, "unexpected running status");
      running = 1;
    }
    else /* [SS] 2013-09-10 */
    {
      if (c >> 4 != 0x0f) /* if it is not a meta event save the status*/
      {
        laststatus = c;
      }
      running = 0;
      status = c;
    }

    /* [SS] 2013-09-10 */
    if (running)
      needed = chantype[(laststatus >> 4) & 0xf];
    else
      needed = chantype[(status >> 4) & 0xf];

    if (needed) /* ie. is it a channel message? */
    {
      if (running)
      {
        c1 = c;
        ChanMessage(laststatus, c1, (needed > 1) ? GetCharacter() : 0);
      }
      else
      {
        c1 = GetCharacter();
        ChanMessage(status, c1, (needed > 1) ? GetCharacter() : 0);
      }
      continue;
      ;
    }

    switch (c)
    {
      case 0xff: /* meta event */
        type = GetCharacter();
        varinum = ReadVarinum();
        lookfor = m_toberead - varinum;
        MsgInit();

        while (m_toberead > lookfor)
          MsgAdd(GetCharacter());

        MetaEvent(type);
        break;

      case 0xf0: /* start of system exclusive */
        varinum = ReadVarinum();
        lookfor = m_toberead - varinum;
        MsgInit();
        MsgAdd(0xf0);

        while (m_toberead > lookfor)
          MsgAdd(c = GetCharacter());

        if (c == 0xf7 || m_nomerge == 0)
        {
          //printf("Sysex, leng=%d\n",MsgLength());
        }
        else
        {
          sysexcontinue = 1; /* merge into next msg */
        }
        break;

      case 0xf7: /* sysex continuation or arbitrary stuff */
        varinum = ReadVarinum();
        lookfor = m_toberead - varinum;

        if (!sysexcontinue)
          MsgInit();

        while (m_toberead > lookfor)
          MsgAdd(c = GetCharacter());

        if (!sysexcontinue)
        {
          //printf("Arbitrary bytes, leng=%d\n",MsgLength());
        }
        else if (c == 0xf7)
        {
          //printf("Sysex, leng=%d\n",MsgLength());
          sysexcontinue = 0;
        }
        break;

      default:
        kodi::Log(ADDON_LOG_ERROR, "unexpected byte: 0x%02x", c);
        break;
    }
  }

  m_currentTrack++;
  //printf("Track end\n");

  return 1;
}

int CMidiScan::ReadMT(const char* s) /* read through the "MThd" or "MTrk" header string */
{
  int n = 0;
  const char* p = s;
  int c;

  while (n++ < 4 && (c = GetCharacter()) != -1)
  {
    if (c != *p++)
    {
      kodi::Log(ADDON_LOG_ERROR, "Wanted midi file '%s' expecting '%s'", m_path.c_str(), s);
    }
  }
  return c;
}

int32_t CMidiScan::Read32bit()
{
  int c1, c2, c3, c4;

  c1 = GetCharacter();
  c2 = GetCharacter();
  c3 = GetCharacter();
  c4 = GetCharacter();
  return To32bit(c1, c2, c3, c4);
}

int16_t CMidiScan::Read16bit()
{
  int c1, c2;
  c1 = GetCharacter();
  c2 = GetCharacter();
  return To16bit(c1, c2);
}

int32_t CMidiScan::ReadVarinum()
{
  int32_t value;
  int c;

  c = GetCharacter();
  value = c;
  if (c & 0x80)
  {
    value &= 0x7f;
    do
    {
      c = GetCharacter();
      value = (value << 7) + (c & 0x7f);
    } while (c & 0x80);
  }
  return (value);
}

int32_t CMidiScan::To32bit(int c1, int c2, int c3, int c4)
{
  int32_t value = 0L;

  value = (c1 & 0xff);
  value = (value << 8) + (c2 & 0xff);
  value = (value << 8) + (c3 & 0xff);
  value = (value << 8) + (c4 & 0xff);
  return (value);
}

int16_t CMidiScan::To16bit(int c1, int c2)
{
  return ((c1 & 0xff) << 8) + (c2 & 0xff);
}

int CMidiScan::GetCharacter() /* read a single character and abort on EOF */
{
  if (m_fileDataPtr >= m_fileDataSize)
    return -1;

  int c = m_fileData[m_fileDataPtr++];
  if (c == -1)
  {
    kodi::Log(ADDON_LOG_ERROR, "Wanted midi file '%s' premature EOF", m_path.c_str());
  }

  m_toberead--;
  m_bytesread++;
  return c;
}

void CMidiScan::ChanMessage(int status, int c1, int c2)
{
  int chan = status & 0xf;

  switch (status & 0xf0)
  {
    case 0x80:
      //printf("Note off, chan=%d pitch=%d vol=%d\n", chan + 1, c1, c2);
      break;
    case 0x90:
      //printf("Note on, chan=%d pitch=%d vol=%d\n", chan + 1, c1, c2);
      break;
    case 0xa0:
      //printf("Pressure, chan=%d pitch=%d press=%d\n", chan + 1, c1, c2);
      break;
    case 0xb0:
      //printf("Parameter, chan=%d c1=%d c2=%d\n", chan + 1, c1, c2);
      break;
    case 0xe0:
      //printf("Pitchbend, chan=%d lsb=%d msb=%d\n", chan + 1, c1, c2);
      break;
    case 0xc0:
      //printf("Program, chan=%d program=%d\n", chan + 1, c1);
      break;
    case 0xd0:
      //printf("Channel pressure, chan=%d pressure=%d\n", chan + 1, c1);
      break;
  }
}

void CMidiScan::MetaEvent(int type)
{
  int leng;
  char* m;

  leng = MsgLength();
  m = Msg();
  switch (type)
  {
    case 0x00:
      //printf("Meta event, sequence number = %d\n",To16bit(m[0],m[1]));
      break;
    case 0x01: /* Text event */
    case 0x02: /* Copyright notice */
    case 0x03: /* Sequence/Track name */
    case 0x04: /* Instrument name */
    case 0x05: /* Lyric */
    case 0x06: /* Marker */
    case 0x07: /* Cue point */
    case 0x08:
    case 0x09:
    case 0x0a:
    case 0x0b:
    case 0x0c:
    case 0x0d:
    case 0x0e:
    case 0x0f:
      /* These are all text events */
      MetaText(type, leng, m);
      break;
    case 0x2f: /* End of Track */
      //printf("Meta event, end of track\n");
      if (m_duration < m_currtime)
        m_duration = m_currtime;
      break;
    case 0x51: /* Set tempo */
      if (!m_millisecondsPerMIDIQuarterNote)
        m_millisecondsPerMIDIQuarterNote = To32bit(0, m[0], m[1], m[2]) / 1000;
      //printf("Tempo, microseconds-per-MIDI-quarter-note=%d\n",To32bit(0,m[0],m[1],m[2]));
      break;
    case 0x54:
      //printf("SMPTE, hour=%d minute=%d second=%d frame=%d fract-frame=%d\n",
      //m[0],m[1],m[2],m[3],m[4]);
      break;
    case 0x58:
      TimeSig(m[0], m[1], m[2], m[3]);
      break;
    case 0x59:
      //printf("Key signature, sharp/flats=%d  minor=%d\n",m[0],m[1]);
      break;
    case 0x7f:
      //printf("Meta event, sequencer-specific, type=0x%02x leng=%d\n",type,leng);
      break;
    default:
      //printf("Meta event, unrecognized, type=0x%02x leng=%d\n",type,leng);
      break;
  }
}

std::string CMidiScan::GetTimeString(int ms)
{
  int h = ms / (1000 * 60 * 60);
  ms -= h * (1000 * 60 * 60);

  int m = ms / (1000 * 60);
  ms -= m * (1000 * 60);

  int s = ms / 1000;
  ms -= s * 1000;

  char buff[100];
  snprintf(buff, sizeof(buff), "[%02i:%02i.%02i]", m, s, ms / 10 % 100);
  return buff;
}

void CMidiScan::MetaText(int type, int leng, char* mess)
{
  static const char* ttype[] = {NULL,
                                "Text Event", /* type=0x01 */
                                "Copyright Notice", /* type=0x02 */
                                "Sequence/Track Name",
                                "Instrument Name", /* ...       */
                                "Lyric",
                                "Marker",
                                "Cue Point", /* type=0x07 */
                                "Unrecognized"};
  int unrecognized = (sizeof(ttype) / sizeof(char*)) - 1;
  int n, c;
  char* p = mess;

  if (type < 1 || type > unrecognized)
    type = unrecognized;

  MetaData metaData;
  metaData.id = m_currentTrack;
  metaData.time = m_currtime * m_millisecondsPerMIDIQuarterNote / m_division;
  metaData.type = type;
  metaData.text.reserve(leng);

  //printf("Meta Text, time %i type=0x%02x (%s)  leng=%d\n", m_currtime, type, ttype[type], leng);
  for (n = 0; n < leng; n++)
  {
    c = (*p++) & 0xff;
    if (isprint(c) || isspace(c))
      metaData.text += c;

    //printf((isprint(c) || isspace(c)) ? "%c" : "\\0x%02x", c);
  }
  m_metadata.emplace_back(metaData);

  //printf("\n");
}

void CMidiScan::TimeSig(int nn, int dd, int cc, int bb)
{
  //int denom = 1;
  //while (dd-- > 0)
  //  denom *= 2;
  //printf("Time signature=%d/%d  MIDI-clocks/click=%d  32nd-notes/24-MIDI-clocks=%d\n", nn, denom,
  //       cc, bb);
}

void CMidiScan::MsgInit()
{
  m_msgindex = 0;
}

char* CMidiScan::Msg()
{
  return m_msgbuff;
}

int CMidiScan::MsgLength()
{
  return m_msgindex;
}

void CMidiScan::MsgAdd(int c)
{
  /* If necessary, allocate larger message buffer. */
  if (m_msgindex >= m_msgsize)
    BiggerMsg();
  m_msgbuff[m_msgindex++] = c;
}

void CMidiScan::BiggerMsg()
{
  /*   char *malloc(); */
  char* newmess;
  char* oldmess = m_msgbuff;
  int oldleng = m_msgsize;

  m_msgsize += MSGINCREMENT;
  /* to ensure a string is terminated with 0 [SS] 2017-08-30 */
  /*  newmess = (char *) malloc( (unsigned)(sizeof(char)*Msgsize) ); */
  newmess = (char*)calloc((unsigned)(sizeof(char) * m_msgsize), sizeof(char));

  if (newmess == nullptr)
    kodi::Log(ADDON_LOG_ERROR, "malloc error!");

  /* copy old message into larger new one */
  if (oldmess != nullptr)
  {
    char* p = newmess;
    char* q = oldmess;
    char* endq = &oldmess[oldleng];

    for (; q != endq; p++, q++)
      *p = *q;
    free(oldmess);
  }
  m_msgbuff = newmess;
}

std::string CMidiScan::RandomString(size_t length)
{
  auto randchar = []() -> char {
    const char charset[] = "0123456789"
                           "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    return charset[rand() % max_index];
  };
  std::string str(length, 0);
  std::generate_n(str.begin(), length, randchar);
  return str;
}
