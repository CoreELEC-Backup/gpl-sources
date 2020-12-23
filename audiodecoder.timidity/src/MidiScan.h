/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <string>
#include <vector>

#define MSGINCREMENT 128

struct KaraokeData
{
  std::string language;
  std::string title;
  std::string artist;
  std::string album;
  std::string creator;
  std::string text;
};

typedef enum MetaType : int
{
  MetaTypeTextEvent = 0x01,
  MetaTypeCopyrightNotice = 0x02,
  MetaTypeSequenceTrackName = 0x03,
  MetaTypeInstrumentName = 0x04,
  MetaTypeLyric = 0x05,
  MetaTypeMarker = 0x06,
  MetaTypeCuePoint = 0x07,
  MetaTypeUnrecognized = 0x08,
} MetaType;

class CMidiScan
{
public:
  CMidiScan(const std::string& path);
  ~CMidiScan();

  bool Scan();

  const std::string GetArtist() const { return m_artist; }
  const std::string GetTitle() const { return m_title; }
  const std::string GetLyrics() const { return m_lyrics; }
  int32_t GetDuration() const
  {
    // NOTE About time it need a bit improvement, as not always 100% correct,
    // but better as nothing.
    return m_duration * m_millisecondsPerMIDIQuarterNote / m_division / 1000;
  }

private:
  void Read();
  void ReadHeader();
  int ReadTrack();
  int ReadMT(const char* s);
  int32_t Read32bit();
  int16_t Read16bit();
  int32_t ReadVarinum();
  int32_t To32bit(int c1, int c2, int c3, int c4);
  int16_t To16bit(int c1, int c2);
  int GetCharacter();
  void ChanMessage(int status, int c1, int c2);
  void MetaEvent(int type);
  void MetaText(int type, int leng, char* mess);
  void TimeSig(int nn, int dd, int cc, int bb);
  std::string GetTimeString(int ms);
  std::string RandomString(size_t length);

  std::string m_path;

  int m_nomerge = 0; /* 1 => continue'ed system exclusives are not collapsed. */
  int32_t m_currtime = 0; /* current time in delta-time units */

  int32_t m_toberead = 0;
  int32_t m_bytesread = 0;

  int32_t m_numbyteswritten = 0; /* linking with store.c */

  int m_ntrks;

  size_t m_fileDataPtr = 0;
  size_t m_fileDataSize = 0;
  uint8_t* m_fileData = nullptr;

  void MsgInit();
  char* Msg();
  int MsgLength();
  void MsgAdd(int c);
  void BiggerMsg();

  char* m_msgbuff = nullptr; /* message buffer */
  int m_msgsize = 0; /* Size of currently allocated Msg */
  int m_msgindex = 0; /* index of next available location in Msg */

  int m_currentTrack = 0;
  int m_millisecondsPerMIDIQuarterNote = 0;
  int m_division = 1;

  std::string m_artist;
  std::string m_title;
  std::string m_lyrics;
  int m_duration = 0;

  struct MetaData
  {
    int id;
    int type;
    int time;
    std::string text;
  };
  std::vector<MetaData> m_metadata;
};
