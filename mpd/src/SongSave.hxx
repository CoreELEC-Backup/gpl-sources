/*
 * Copyright 2003-2020 The Music Player Daemon Project
 * http://www.musicpd.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef MPD_SONG_SAVE_HXX
#define MPD_SONG_SAVE_HXX

#include <memory>

#define SONG_BEGIN "song_begin: "

struct Song;
struct AudioFormat;
class DetachedSong;
class BufferedOutputStream;
class TextFile;

void
song_save(BufferedOutputStream &os, const Song &song);

void
song_save(BufferedOutputStream &os, const DetachedSong &song);

/**
 * Loads a song from the input file.  Reading stops after the
 * "song_end" line.
 *
 * Throws on error.
 */
DetachedSong
song_load(TextFile &file, const char *uri,
	  std::string *target_r=nullptr,
	  AudioFormat *audio_format_r=nullptr);

#endif
