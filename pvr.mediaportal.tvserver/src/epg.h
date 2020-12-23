/*
 *  Copyright (C) 2005-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#ifndef __EPG_H
#define __EPG_H

#include <stdlib.h>
#include <string>
#include "GenreTable.h"
#include "DateTime.h"

#include <kodi/addon-instance/pvr/Timers.h>

const int cKodiEpgIndexOffset = (PVR_TIMER_NO_EPG_UID + 1); ///> Offset used to map the MediaPortal schedule id's to the iClientIndex values


class cEpg
{
private:
  unsigned int m_uid;
  std::string m_title;
  std::string m_description;
  MPTV::CDateTime m_startTime;
  MPTV::CDateTime m_endTime;
  MPTV::CDateTime m_originalAirDate;
  int m_duration;
  std::string m_genre;
  int m_genre_type;
  int m_genre_subtype;
  int m_episodeNumber;
  std::string m_episodePart;
  std::string m_episodeName;
  int m_seriesNumber;
  int m_starRating;
  int m_parentalRating;
  CGenreTable* m_genretable;

public:
  cEpg();
  virtual ~cEpg();
  void Reset();

  bool ParseLine(std::string& data);
  unsigned int UniqueId(void) const { return m_uid; }
  time_t StartTime(void) const;
  time_t EndTime(void) const;
  time_t Duration(void) const { return m_duration; }
  time_t OriginalAirDate(void) const;
  const char *Title(void) const { return m_title.c_str(); }
  const char *PlotOutline(void) const;
  const char *Description(void) const { return m_description.c_str(); }
  const char *Genre(void) const { return m_genre.c_str(); }
  int GenreType(void) const { return m_genre_type; }
  int GenreSubType(void) const { return m_genre_subtype; }
  int SeriesNumber(void) const { return m_seriesNumber; };
  int EpisodeNumber(void) const { return m_episodeNumber; };
  const char* EpisodeName(void) const { return m_episodeName.c_str(); };
  const char* EpisodePart(void) const { return m_episodePart.c_str(); };
  int StarRating(void) const { return m_starRating; };
  int ParentalRating(void) const { return m_parentalRating; };
  void SetGenreTable(CGenreTable* genremap);
};

#endif //__EPG_H
